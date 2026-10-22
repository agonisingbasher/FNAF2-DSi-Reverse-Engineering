"""Prepare one native 2x2 industrial sheet. Preserve document pixels/text."""
from pathlib import Path
from PIL import Image
import struct,json,hashlib
R=Path(__file__).resolve().parents[1];src=R/'reference_assets/victory_industrial/victory_sheet_original.png'
im=Image.open(src).convert('RGB').resize((512,384),Image.Resampling.NEAREST)
q=im.quantize(colors=256,method=Image.Quantize.MEDIANCUT);p=q.getpalette();p += [0]*(768-len(p))
v=[0x8000|(p[i]>>3)|((p[i+1]>>3)<<5)|((p[i+2]>>3)<<10) for i in range(0,768,3)]
data=struct.pack('<8s4H',b'VICT2X2!',512,384,256,192)+struct.pack('<256H',*v)+q.tobytes()
(R/'nitrofiles/victory_industrial_sheet.bin').write_bytes(data)
(R/'reference_assets/victory_industrial/manifest.json').write_text(json.dumps({'source_sha256':hashlib.sha256(src.read_bytes()).hexdigest(),'original_dimensions':list(Image.open(src).size),'runtime_dimensions':[512,384],'bytes':len(data),'night_to_cell':[0,0,0,0,1,2,3],'night_5_6_7_document_text_changed':False},indent=2))
# The old generic lower banks must no longer be shipped or loaded.
for old in (R/'nitrofiles').glob('v194_bottom_n*.bin'):old.unlink()
print('Industrial victory sheet:',len(data),'bytes; generic lower banks removed')
