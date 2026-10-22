"""Compose the supplied, unchanged PNG layers into bounded indexed playback banks."""
from pathlib import Path
from PIL import Image
import struct, hashlib,json
from build_qol_ui import add_buttons,CONTINUE,PRE
R=Path(__file__).resolve().parents[1];SRC=R/'reference_assets/victory_exact/assets/victory';OUT=R/'nitrofiles'
def get(folder,name):return Image.open(SRC/folder/(name+'.png')).convert('RGBA')
def blank():return Image.new('RGBA',(256,192),(0,0,0,255))
def indexed(im):
 q=im.convert('RGB').quantize(colors=256,method=Image.Quantize.MEDIANCUT)
 p=q.getpalette();p=p+[0]*(768-len(p))
 vals=[0x8000|(p[i]>>3)|((p[i+1]>>3)<<5)|((p[i+2]>>3)<<10) for i in range(0,768,3)]
 return struct.pack('<256H',*vals)+q.tobytes()
def add(im,layer,xy,opacity=1):
 if opacity<1:
  layer=layer.copy();layer.putalpha(layer.getchannel('A').point(lambda a:round(a*opacity)))
 im.alpha_composite(layer,xy)
def ramp(t,a,b):return max(0,min(1,(t-a)/(b-a)))
clock=[]
im=blank();add(im,get('top','digit_5'),(64,72));add(im,get('top','am'),(96,72));clock.append(indexed(im))
roll=get('top','clock_roll')
for i in range(8):
 im=blank();add(im,roll.crop((0,i*48,32,(i+1)*48)),(64,72));add(im,get('top','am'),(96,72));clock.append(indexed(im))
(OUT/'v194_clock.bin').write_bytes(b''.join(clock))
confetti=get('top','confetti')
for group in ['toys','withered_light','withered_full','shadow_golden']:
 frames=[]
 for pose in ['a','b']:
  for i in range(8):
   im=blank();add(im,confetti.crop((0,i*192,256,(i+1)*192)),(0,0));add(im,get('top','survived'),(16,8));add(im,get('top','digit_6'),(64,40));add(im,get('top','am'),(96,40));add(im,get('top',group+'_'+pose),(0,88));frames.append(indexed(im))
 (OUT/f'v194_top_{group}.bin').write_bytes(b''.join(frames))
manifest=json.loads((R/'reference_assets/victory_exact/manifest.json').read_text())
for x in manifest:assert hashlib.sha256((R/'reference_assets/victory_exact'/x['path']).read_bytes()).hexdigest()==x['sha256']
print('Exact ZIP assets verified; victory clock and 4 unchanged upper A/B banks created')
