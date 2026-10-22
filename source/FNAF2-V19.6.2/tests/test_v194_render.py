# Legacy command redirects to the current industrial renderer validation.
from pathlib import Path
import runpy
runpy.run_path(str(Path(__file__).with_name('test_fixed_render.py')),run_name='__main__')
