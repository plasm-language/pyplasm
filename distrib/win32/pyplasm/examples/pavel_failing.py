from pyplasm import *

mycube = CUBOID([1, 1, 1])
mycyl = CYLINDER ([0.25, 2.0])(128)
mycyl = T(1)(0.5)(mycyl)
mycyl = R([2, 3])(-PI/4)(mycyl)
mycyl = R([1, 3])(-PI/4)(mycyl)
out = UNION([mycube, mycyl])
VIEW(SKELETON(1)(out))
