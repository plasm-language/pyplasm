from pyplasm import *

# Subdivisions.
n1 = 64  # Angular.
n2 = 64  # In the axial direction.

# Partition of unit interval into n parts
def dom(n): 
    return INTERVALS(1)(n)

def ROTATIONALSOLID (args):
    section = args
    def map_fn(point):
	u, v, w = point
	x, y, z = section([u, v])
	ret = [x*math.cos(w), x*math.sin(w), z]
	return ret
    return map_fn
  
# Curves
c0 = BEZIER(S1)([[0, 0, 0], [0, 0, 3]])
c1 = BEZIER(S1)([[2, 0, 0], [4, 0, 1], [0, 0, 2], [1, 0, 3]])
dom1D = dom(n1)
profile0 = MAP(c0)(dom1D)
profile1 = MAP(c1)(dom1D)
profiles = STRUCT([profile0, profile1])
#VIEW(profiles, [0.4, 0.9, 0.6])            # Throws an error.

# Surface
section = BEZIER(S2)([c1, c0])
dom2D = PROD(AA(dom)([n2, 1]))
VIEW(dom2D, [0.4, 0.9, 0.6])
VIEW(MAP(section)(dom2D), [0.4, 0.9, 0.6])

# Solid
domain = PROD([dom2D, S(1)(2*PI)(dom1D)])
VIEW(domain, [0.4, 0.9, 0.6])
out = MAP(ROTATIONALSOLID(section))(domain)
VIEW(out, [0.4, 0.9, 0.6])
#VIEW(SKELETON(1)(out), [0.4, 0.9, 0.6])    # Throws an error.
