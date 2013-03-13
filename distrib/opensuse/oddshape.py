# a pecular object, that has 3 distinct projections: circle, square, triangle
from pyplasm import *

cyl   = CYLINDER([5.0, 9])(64)
prism = CYLINDER([5.777, 10.0])(3)
prism = R([1,3])(PI/2)(prism)
prism = T([1,2,3])([5,0,3])(prism)
#out = UNION([cyl, prism])
out = INTERSECTION([cyl, prism])

VIEW(out)
