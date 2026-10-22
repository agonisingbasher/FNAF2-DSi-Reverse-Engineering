from pathlib import Path
import struct
from PIL import Image
ROOT=Path(__file__).resolve().parents[1]; A=ROOT/'assets'; N=ROOT/'nitrofiles'; SIZE=(256,192)
def convert(im):
 im=im.convert('RGB').resize(SIZE,Image.Resampling.LANCZOS)
 vals=[0x8000|(r>>3)|((g>>3)<<5)|((b>>3)<<10) for r,g,b in im.get_flattened_data()]
 return struct.pack('<%dH'%len(vals),*vals)
N.mkdir(exist_ok=True)
for i in range(4): (N/f'title_top_{i}.bin').write_bytes(convert(Image.open(A/f'title_top_frame_{i+1}.png')))
(N/'title_warning.bin').write_bytes(convert(Image.open(A/'title_warning_preview.png')))
(N/'title_menu_frames.bin').write_bytes(b''.join(convert(Image.open(A/f'menu_{i:02}.png')) for i in range(32)))
music=(A/'title_music_12000_mono_s8.raw').read_bytes()
if len(music)!=177192: raise ValueError(f'bad music length {len(music)}')
(N/'title_music.raw').write_bytes(music)
im=Image.open(A/'newspaper_reference.png').convert('RGB')
(N/'newspaper.bin').write_bytes(convert(im))
print('NitroFS title resources created')
