from pathlib import Path
import struct
from PIL import Image

root=Path(__file__).resolve().parents[1]
source=root/'assets/contract_application_reference.png'
target=root/'nitrofiles/contract_form.bin'
image=Image.open(source).convert('RGB').resize((256,167),Image.Resampling.LANCZOS)
canvas=Image.new('RGB',(256,192),(0,0,0))
canvas.paste(image,(0,0))
# The base form includes an ACCEPT button. Remove that area; APPLY is drawn
# at runtime in its place after the player has signed.
from PIL import ImageDraw
draw=ImageDraw.Draw(canvas)
draw.rectangle((203,146,255,164),fill=(0,0,0))
pixels=[(r>>3)|((g>>3)<<5)|((b>>3)<<10)|0x8000 for r,g,b in canvas.get_flattened_data()]
target.parent.mkdir(parents=True,exist_ok=True)
target.write_bytes(struct.pack('<%dH'%len(pixels),*pixels))
