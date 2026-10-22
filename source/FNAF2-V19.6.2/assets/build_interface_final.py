from pathlib import Path
from PIL import Image
import struct
R=Path(__file__).resolve().parents[1]
def pack(name,size,magic,cell,out,mask=False):
 im=Image.open(R/'reference_assets/interface_final'/name).convert('RGB').resize(size,Image.Resampling.NEAREST)
 q=im.quantize(colors=255 if mask else 256)
 pal=q.getpalette();pal += [0]*(768-len(pal))
 vals=[0x8000|(pal[i]>>3)|((pal[i+1]>>3)<<5)|((pal[i+2]>>3)<<10) for i in range(0,768,3)]
 data=bytearray(q.tobytes())
 if mask:
  # Remove only connected dark panel background, keeping enclosed black details.
  from collections import deque
  pix=im.load();w,h=im.size
  for c in range(8):
   ox=(c%4)*256;oy=(c//4)*192
   queue=deque([(ox+60,oy+70)]);seen=set()
   while queue:
    x,y=queue.popleft()
    if (x,y) in seen or not(ox+16<=x<ox+239 and oy+34<=y<oy+178):continue
    seen.add((x,y));r,g,b=pix[x,y]
    if max(r,g,b)>35:continue
    data[y*w+x]=255
    queue.extend(((x+1,y),(x-1,y),(x,y+1),(x,y-1)))
  vals[255]=0
 (R/'nitrofiles'/out).write_bytes(struct.pack('<8s4H',magic,*size,*cell)+struct.pack('<256H',*vals)+data)
def build():
 pack('gameover.png',(1280,768),b'GOCRT4X5',(256,192),'gameover_crt_sheet.bin')
 pack('loading.png',(1024,576),b'LOAD4X3!',(256,192),'loading_top.bin',True)
 pack('swipe.png',(864,576),b'SWIPE3!!',(864,192),'loading_swipe.bin')
if __name__=='__main__':build()
