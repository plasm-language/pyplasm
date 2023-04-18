from pyplasm import *

#lower_ring = TORUS([9.5, 10.0])([64, 16])
#upper_ring = T(3)(2.5)(lower_ring)

#lower_ring_inner= TORUS([7.5, 8.0])([64, 16])
#upper_ring_inner = T(2)(2.5)(lower_ring_inner)

cyl_outer = CYLINDER([10.0, 3.0])(64)
cyl_inner = CYLINDER([8.0, 3.0])(64)
cyl_inner = T(3)(0.5)(cyl_inner)
out = DIFF([cyl_outer, cyl_inner])

cyl = CYLINDER([2.0, 30.0])(64)
cyl1 = R([1, 3])(PI/2)(cyl)
cyl1 = T([1, 3])([15, 3.0])(cyl1)

out = DIFF([out, cyl1])

cyl2 = R([2, 3])(PI/2)(cyl)
cyl2 = T([2, 3])([15, 3.0])(cyl2)

out = DIFF([out, cyl2])

#out = cyl1
#out = UNION([lower_ring, upper_ring])
#out = UNION([out, upper_ring])
#out = UNION([out, cyl1])
VIEW(out)
