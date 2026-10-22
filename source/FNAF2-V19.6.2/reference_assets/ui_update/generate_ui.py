#!/usr/bin/env python3
"""Generate the four native-scale FNAF2 DSi UI assets with Pillow."""
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont
import json, hashlib, zipfile

ROOT=Path(__file__).resolve().parent
ASSET=ROOT/'assets'
FONT='/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf'
WHITE=(232,236,239,255); CYAN=(63,223,240,255)

def face(px):
    return ImageFont.truetype(FONT,px)

def pixel_text(im,text,x,y,px,color,anchor='lt'):
    font=face(px)
    box=font.getbbox(text,anchor=anchor)
    if not box: return
    mask=Image.new('L',im.size,0)
    ImageDraw.Draw(mask).text((x,y),text,font=font,anchor=anchor,fill=255,stroke_width=0)
    mask=mask.point(lambda a: 255 if a>=120 else 0)
    im.paste(Image.new('RGBA',im.size,color),(0,0),mask)

def btn(path,text,primary=False):
    im=Image.new('RGBA',(224,16),(0,0,0,0));d=ImageDraw.Draw(im)
    line=CYAN if primary else (132,143,149,255)
    d.rectangle((0,0,223,15), fill=(19,23,27,255),outline=line,width=1)
    d.line((2,2,221,2),fill=(39,50,56,255))
    # color subtly echoes the bright primary outline, but the letters stay white.
    pixel_text(im,text,112,8,10,WHITE,'mm')
    im.save(path,optimize=True)

def background(path):
    im=Image.new('RGBA',(256,192),(3,3,5,255));d=ImageDraw.Draw(im)
    for y in range(0,192,4): d.line((0,y,255,y),fill=(9,5,7,255))
    # restrained CRT accent / corrupted-display markers
    d.rectangle((7,9,248,184),outline=(36,12,17,255),width=1)
    d.line((14,47,241,47),fill=(88,19,25,255),width=1)
    pixel_text(im,'GAME OVER',128,31,25,(238,43,50,255),'mm')
    # three lines are needed to keep the long disclaimer legible on 256 px.
    rows=['FAZBEAR ENTERTAINMENT IS', 'NOT RESPONSIBLE FOR DEATH', 'OR DISMEMBERMENT.']
    for j,row in enumerate(rows):
        pixel_text(im,row,128,65+j*12,10,WHITE,'mm')
    d.line((22,120,233,120),fill=(50,15,19,255))
    im.save(path,optimize=True)

def make():
    victory=ASSET/'victory'/'bottom'; over=ASSET/'gameover'
    victory.mkdir(parents=True,exist_ok=True);over.mkdir(parents=True,exist_ok=True)
    btn(victory/'button_game_menu.png','GAME MENU')
    btn(victory/'button_tap_continue.png','TAP CONTINUE',True)
    background(over/'gameover_bottom_bg.png')
    btn(over/'button_retry_night.png','RETRY NIGHT',True)
    for p in sorted(ASSET.rglob('*.png')):
        with Image.open(p) as im:
            assert im.mode=='RGBA'
            assert im.size == ((256,192) if p.name=='gameover_bottom_bg.png' else (224,16)), (p,im.size)
    return sorted(ASSET.rglob('*.png'))

if __name__=='__main__':
    for p in make(): print(p.relative_to(ROOT))
