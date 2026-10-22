/* Reserve inert ROM bytes so the legacy entry trampoline fits ndstool alignment. */
.section .rodata.rom_alignment_padding,"a",%progbits
.balign 4
.global rom_alignment_padding
rom_alignment_padding:
.space 96, 0
