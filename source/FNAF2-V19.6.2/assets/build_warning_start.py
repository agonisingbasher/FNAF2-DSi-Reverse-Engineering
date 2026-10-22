from pathlib import Path
import struct
from PIL import Image

root=Path(__file__).resolve().parents[1]
source=root/'assets/warning_start_reference.png'
target=root/'nitrofiles/warning_start.bin'
on=Image.open(source).convert('RGB').resize((256,192),Image.Resampling.LANCZOS)
off=on.copy()
pixels=off.load()
# Remove both the START LED and its red spill from the OFF frame. The halo is
# larger than the LED, so neutralize every red glow pixel around the lamp.
for y in range(149,188):
    for x in range(176,222):
        r,g,b=pixels[x,y]
        radius=((x-198.5)**2/23**2+(y-168.5)**2/19**2)**0.5
        if radius>1.0 or r<max(g,b)*1.18 or r<22: continue
        dark_gray=0 if radius<0.24 else g
        pixels[x,y]=(dark_gray,dark_gray,dark_gray)

def rgb555(image):
    return [(r>>3)|((g>>3)<<5)|((b>>3)<<10)|0x8000 for r,g,b in image.get_flattened_data()]

frames=[rgb555(off),rgb555(on)]
target.parent.mkdir(parents=True,exist_ok=True)
target.write_bytes(b''.join(struct.pack('<%dH'%len(frame),*frame) for frame in frames))
