"""Compose provided PNG lettering into reference-style 256x192 touch layouts.
No runtime fonts, PNG decoding, or disk reads in the victory animation loop.
"""
from pathlib import Path
from PIL import Image,ImageDraw
import struct,hashlib,json
R=Path(__file__).resolve().parents[1]
SRC=R/'reference_assets/ui_update/assets'
OUT=R/'nitrofiles'
PRE=R/'previews';PRE.mkdir(exist_ok=True)
RECTS=((8,150,124,186),(132,150,248,186))
def blank():return Image.new('RGBA',(256,192),(0,0,0,255))
def load(p):return Image.open(SRC/p).convert('RGBA')
def button(p,primary):
 original=load(p)
 mask=Image.new('L',original.size)
 mask.putdata([255 if min(c[:3])>180 else 0 for c in original.getdata()])
 box=mask.getbbox();mask=mask.crop(box)
 im=Image.new('RGBA',(116,36),(0,0,0,255));d=ImageDraw.Draw(im)
 color=(63,223,240,255) if primary else (181,189,196,255)
 d.rectangle((0,0,115,35),fill=(7,31,35,255) if primary else (20,22,25,255),outline=color,width=1)
 d.rectangle((3,3,112,32),outline=color,width=1)
 im.paste(color if primary else (232,236,239,255),((116-mask.width)//2,(36-mask.height)//2),mask)
 # Reference brackets, without shrinking native supplied glyphs.
 cx=(116-mask.width)//2
 d.line((cx-6,13,cx-8,13,cx-8,22,cx-6,22),fill=color)
 d.line((cx+mask.width+5,13,cx+mask.width+7,13,cx+mask.width+7,22,cx+mask.width+5,22),fill=color)
 return im
CONTINUE=button(Path('victory/bottom/button_tap_continue.png'),True)
MENU=button(Path('victory/bottom/button_game_menu.png'),False)
RETRY=button(Path('gameover/button_retry_night.png'),True)
def add_buttons(im,first,opacity=1):
 for layer,rect in zip((first,MENU),RECTS):
  layer=layer.copy();layer.putalpha(layer.getchannel('A').point(lambda a:round(a*opacity)))
  im.alpha_composite(layer,rect[:2])
def indexed(im):
 q=im.convert('RGB').quantize(colors=256,method=Image.Quantize.MEDIANCUT);p=q.getpalette();p += [0]*(768-len(p))
 vals=[0x8000|(p[i]>>3)|((p[i+1]>>3)<<5)|((p[i+2]>>3)<<10) for i in range(0,768,3)]
 return struct.pack('<256H',*vals)+q.tobytes()
def gameover():
 source=load(Path('gameover/gameover_bottom_bg.png'));im=blank()
 # Copy supplied red heading only, enlarge uniformly; plain black matches the reference.
 title=source.crop((55,18,202,44));mask=Image.new('L',title.size)
 mask.putdata([255 if c[0]>150 and c[1]<100 else 0 for c in title.getdata()])
 mask=mask.crop(mask.getbbox());mask=mask.resize((224,34),Image.Resampling.NEAREST)
 im.paste((238,43,50,255),(16,28),mask)
 # Supplied complete legal sentence, native pixel size, muted as in the reference.
 phrase=source.crop((48,59,208,96));m=Image.new('L',phrase.size)
 m.putdata([255 if min(c[:3])>180 else 0 for c in phrase.getdata()])
 im.paste((157,153,140,255),(48,89),m)
 add_buttons(im,RETRY);return im
if __name__=='__main__':
 # Static Game Over replaced by the single CRT sheet; victory buttons unchanged.
 (OUT/'qol_gameover.bin').unlink(missing_ok=True)
 for name,im in [('tap_continue',CONTINUE),('game_menu',MENU),('retry_night',RETRY)]:im.save(PRE/(name+'.png'))
 print('Legacy button-mask reference previews built; static Game Over bank removed')
