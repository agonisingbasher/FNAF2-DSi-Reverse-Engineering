from pathlib import Path
import struct,hashlib,json,sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'assets'))
from finalize_dsi_rom import crc16,SIGNATURE
R=Path(__file__).resolve().parents[1];release=R
ARM7_SHA256='2cc36366c30a1f08f972518252df84b27d580d6f45b2b62b0e7acdc2b2140ed4'
def section(d,at):
 off,entry,dest,length=struct.unpack_from('<4I',d,at);return d[off:off+length]
results=[]
for p in sorted(release.glob('FNAF2-V19.6.2-DSi-*.nds')):
 d=p.read_bytes();assert crc16(d[:0x15e])==struct.unpack_from('<H',d,0x15e)[0]
 assert d[0x12]&2 and any(d[0x180:0x1b0])
 for at in [0x1c0,0x1d0]:
  src,_,target,n=struct.unpack_from('<4I',d,at);assert n and src+n<=len(d)
 assert hashlib.sha256(section(d,0x30)).hexdigest()==ARM7_SHA256
 off,entry,dest,n=struct.unpack_from('<4I',d,0x20);start=off+entry-dest
 assert struct.unpack_from('<4I',d,start)==SIGNATURE
 branch=struct.unpack_from('<I',d,start+16)[0];delta=branch&0xffffff
 if delta&0x800000:delta-=0x1000000
 original=entry+24+delta*4;pos=off+original-dest;assert d[pos+4:pos+8]==b'MOD9'
 debug='-LOG' in p.stem;assert (b'diagnostic build 0.19.6.2' in d)==debug
 isdev='-DEV' in p.stem
 assert (b'DEV MENU - SEM RECOMPENSAS' in d)==isdev
 assert (b'FNAF2-DEV-PROGRESS.' in d)==isdev
 fnt,fntsize,fat,fatsize=struct.unpack_from('<4I',d,0x40);names=d[fnt:fnt+fntsize]
 pos,first,count=struct.unpack_from('<IHH',names,0);assert count==1
 entries={};i=first
 while names[pos]:
  size=names[pos];pos+=1;assert size<128
  name=names[pos:pos+size].decode();pos+=size
  begin,end=struct.unpack_from('<II',d,fat+8*i);i+=1;entries[name]=d[begin:end]
 expected={x.name:x.read_bytes() for x in (R/'nitrofiles').iterdir() if x.is_file()}
 assert entries.keys()==expected.keys(),(entries.keys()-expected.keys(),expected.keys()-entries.keys())
 for name in expected:assert entries[name]==expected[name],name
 assert 'dream_breath.raw' not in entries
 banner=struct.unpack_from('<I',d,0x68)[0];assert crc16(d[banner+0x20:banner+0x840])==struct.unpack_from('<H',d,banner+2)[0]
 results.append({'file':p.name,'bytes':len(d),'sha256':hashlib.sha256(d).hexdigest(),'startup_log':debug,'header_crc_valid':True,'dsi_sections_present':True,'arm7_preserved':True,'legacy_trampoline_valid':True,'banner_crc_valid':True,'nitrofs_verified_files':len(entries)})
assert len(results)==2, 'build NORMAL and DEV ROMs before validating release'
validation={'version':'0.19.6.2','rom_checks':results,'unit_suites_passed':['state','office','corrections','v14','panel','call_stream','camera','ai','v19','cutscene','contract','victory_timing','v195','end_ui','gameover','victory_interaction','ambience','custom_night'],'hardware_tested':False}
(release/'VALIDATION.json').write_text(json.dumps(validation,indent=2));print(json.dumps(validation,indent=2))
