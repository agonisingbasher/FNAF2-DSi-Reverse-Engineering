from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix="fnaf-tests-") as temp:
 for name in ["state","office","corrections","v14","panel","call_stream","camera","ai","v19","cutscene","contract","victory_timing","v195","end_ui","gameover","victory_interaction","ambience","custom_night"]:
  sources=["game_state.c","office_state.c","office_audio.c"]
  if name=="v14":sources.append("progress_store.c")
  if name=="panel":sources.append("office_panel.c")
  if name=="call_stream":sources=["call_stream.c"]
  if name=="camera":sources=["camera_state.c"]
  if name in ("ai","v19"):sources=["night_ai.c","office_state.c"]
  if name=="cutscene":sources=["cutscene_state.c"]
  if name=="contract":sources=["contract_state.c"]
  if name=="victory_timing":sources=[]
  if name=="v195":sources=["night_ai.c","office_state.c"]
  if name=="end_ui":sources=["end_ui.c"]
  if name=="gameover":sources=["gameover_state.c","touch_ui.c"]
  if name=="victory_interaction":sources=["victory_interaction.c","gameover_state.c","touch_ui.c"]
  if name=="custom_night":sources=["custom_night.c","night_ai.c","office_state.c","game_state.c"]
  if name=="ambience":sources=["ambience_state.c"]
  out=str(Path(temp,name+"-test"))
  subprocess.run(["cc","-Wall","-Wextra","-Werror","-I"+str(root/"tests/host"),"-I"+str(root/"source"),str(root/"tests"/(name+"_test.c")),*[str(root/"source"/s) for s in sources],"-o",out],check=True)
  subprocess.run([out,str(root/"tests/fixtures/night1-v2.dat")],check=True)
  print(name+": passed")
