
import sys,types,math
from pyplasm import *


import pyplasm.xge.test.basic
import pyplasm.xge.test.plasm

# questi non funzionano  in CPYTHON per il problema del global lock!!!
if sys.platform=='cli':
	import pyplasm.xge.test.manipulator
	import pyplasm.xge.test.viewer

import pyplasm.examples.arm2d
import pyplasm.examples.threecubes
import pyplasm.examples.colors
import pyplasm.examples.temple
import pyplasm.examples.pisa
