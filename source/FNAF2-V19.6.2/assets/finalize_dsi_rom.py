#!/usr/bin/env python3
"""Finalize an ndstool ELF-built DSi ROM without stripping TWL sections.

Legacy detection entry uses explicitly reserved ARM9 bytes, outside secure padding.
All original MOD9 bytes and load lists remain at their original addresses.
"""
from pathlib import Path
import struct
import sys
import os
import subprocess
import re

SIGNATURE = (0xE3A00301, 0xE5800208, 0xE3A00013, 0xE129F000)

def crc16(data):
    crc = 0xffff
    for b in data:
        crc ^= b
        for _ in range(8):
            crc = (crc >> 1) ^ (0xa001 if crc & 1 else 0)
    return crc

def finalize(path):
    p = Path(path)
    d = bytearray(p.read_bytes())
    off, entry, dest, size = struct.unpack_from('<4I', d, 0x20)
    oldpos = off + entry - dest
    if struct.unpack_from('<4I', d, oldpos) == SIGNATURE:
        return
    assert d[oldpos+4:oldpos+8] == b'MOD9'
    assert d[0x12] & 2, 'DSi header must be preserved'
    assert any(d[0x180:0x1b0]), 'Missing MBK settings'
    for at in (0x1c0, 0x1d0):
        src, _, target, length = struct.unpack_from('<4I', d, at)
        assert length > 0 and src + length <= len(d)
    # Prefer the explicit zero-filled reservation in this ELF. Random ndstool
    # alignment can be zero when the ARM9 size is a multiple of 512 bytes.
    elf = p.with_suffix('.elf')
    nm = str(Path(os.environ['DEVKITARM'])/'bin/arm-none-eabi-nm')
    symbols = subprocess.check_output([nm, str(elf)], text=True)
    match = re.search(r'^([0-9a-fA-F]+) [A-Za-z] rom_alignment_padding$', symbols, re.M)
    assert match, 'Missing explicitly reserved entry trampoline'
    symbol_vma = int(match.group(1), 16)
    e = elf.read_bytes()
    assert e[:6] == b'\x7fELF\x01\x01', 'Need little-endian ELF32'
    phoff = struct.unpack_from('<I', e, 28)[0]
    phsize, phcount = struct.unpack_from('<HH', e, 42)
    newentry = None
    for index in range(phcount):
        kind, fileoff, vma, lma, filesz, memsz, flags, alignment = struct.unpack_from('<8I', e, phoff + index * phsize)
        if kind == 1 and vma <= symbol_vma and symbol_vma + 96 <= vma + filesz:
            newentry = lma + symbol_vma - vma
            break
    assert newentry is not None, 'Reservation has no file-backed LOAD segment'
    newpos = off + newentry - dest
    assert off + 0x800 <= newpos and newpos + 96 <= off + size
    assert d[newpos:newpos+96] == bytes(96), 'Trampoline reservation is not zero-filled'
    delta = entry - (newentry + 24)
    assert delta % 4 == 0 and -(1<<25) <= delta < (1<<25)
    branch = 0xea000000 | ((delta // 4) & 0xffffff)
    struct.pack_into('<5I', d, newpos, *SIGNATURE, branch)
    struct.pack_into('<I', d, 0x24, newentry)
    # ARM9 size and all load lists remain unchanged; entry uses reserved bytes.
    struct.pack_into('<H', d, 0x15e, crc16(d[:0x15e]))
    p.write_bytes(d)
    print(f'ARM9 loaded entry {newentry:08X} -> original {entry:08X}; TWL preserved')

if __name__ == '__main__':
    finalize(sys.argv[1])
