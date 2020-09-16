""" Application to generate tricky rotational solids by lathes.

    The input is maximally simplified as a list of simple ("turtle") commands,
    each giving the data to drive a local displacement of the
    tool tip from the local position.
    Current commands give linear ("b1") or quadratic ("b2", "c2")
    displacements. The command language can be easily extended to include
    higher degrees polynomials in strictly local form (sequences of
    "turn angles" (in degrees) and "forward scalar moves"). Such
    command can be straightworwardly translated into CNC commands

"""
from pyplasm import *
from math import cos, sin, pi, sqrt

def myprint(arg0,arg1):
    """
    Useful for debugging.
    """
    print "\n"+arg0+" =", arg1


def intersect2DLines(P0,T0,P2,T2):
    """
    To compute the intersection point P1 of two 2D lines.

    A line is given as a pair (point, vector).
    In particular, L0 = P0,T0;  L2 = P2,T2.
    Return the common point P1.
    The input of parallel lines is not tested.
    """
    a11 = T0[0]; a12 = -T2[0]; 
    a21 = T0[1]; a22 = -T2[1];
    b1  = P2[0] - P0[0]
    b2  = P2[1] - P0[1]
    alpha = float(b1*a22 - a12*b2)/float(a11*a22 - a12*a21)
    P1 = VECTSUM([P0, SCALARVECTPROD([alpha,T0])])
    return P1


class Nurbs(object):
    """
    A class do describe NURBS splines of arbitrary degree.

    """
    def __init__(self, degree, points, steps=10):
        self.pdim = len(points[0]) # number of coords of control points
        self.degree = degree # spline degree
        self.points = points # list of control points (list of lists of coords)
        self.steps = steps # number of samples in each spline segment
        self.n = len(self.points) # number of points
        self.m = self.degree + self.n # number of knots
        self.knots = (self.degree+1)*[0.] +  \
                     [float(k)/(self.n-2) for k in range(1,self.n-2)] + \
                     (self.degree+1)*[1.] # knot vector (list)
                                          # default to Pinned Uniform knots in [0,1]
        self.segments = self.n - self.degree + 1 # number of curve segments
        self.extent = self.knots[-1] - self.knots[0] # spline extent in parameter space
        self.domain = INTERVALS(self.extent)(self.steps*self.segments) # HPC domain
        self.curve = RATIONALBSPLINE(self.degree)(self.knots)(self.points) # spline mapping
        self.polyline = POLYLINE([p[:-1] for p in self.points]) # control polyline 

    def __str__(self):
        """
        To get the printing of instance data.

        """
        return ('Nurbs instance: \n .pdim: {0}'+ 
                        '\n .degree: {1}'+ 
                        '\n .points: {2}'+ 
                        '\n .steps: {3}'+ 
                        '\n .n: {4}'+ 
                        '\n .m: {5}'+ 
                        '\n .knots: {6}'+ 
                        '\n .segments: {7}'+ 
                        '\n .extent: {8}'+ 
        '').format(self.pdim, self.degree, self.points, self.steps, self.n,
                   self.m , self.knots , self.segments, self.extent )

    def set_knots(self,knots):
        """
        To change the knot vector to a value different from to the default value.

        Needed, for example, to generate a circle segment as a Nurb spline. 
        """
        self.knots = knots
        self.extent = self.knots[-1] - self.knots[0]
        self.domain = INTERVALS(self.extent)(self.steps*self.segments)
        self.curve = RATIONALBSPLINE(self.degree)(self.knots)(self.points)
        return self
        
    def translate(self,vect):
        """
        To translate the (d+1)-dimensional control points of a Nurb.
        
        Notice that the last coordinate is used as weight of the point.
        Therefore the transformation is applied after the projection in d-space.
        A (d+2)-th coordinate is used to store/restore the original weight
        """
        points = [[p[i]/p[-1] for i in range(self.pdim-1)] + [
            1.0] + [p[-1]] for p in self.points]
        points = [VECTSUM([p[:-2],vect]) + p[-2:] for p in points]
        points = [SCALARVECTPROD([p[:-1],p[-1]]) for p in points]
        return Nurbs(self.degree,points)
        
    def scalate(self,vect):
        """
        To scale the (d+1)-dimensional control points of a Nurb.
        
        Notice that the last coordinate is used as weight of the point.
        Therefore the transformation is applied after the projection in d-space.
        A (d+2)-th coordinate is used to store/restore the original weight
        """
        points = [[p[i]/p[-1] for i in range(self.pdim-1)] + [
            1.0] + [p[-1]] for p in self.points]
        points = [AA(PROD)(TRANS([p[:-2],vect])) + p[-2:] for p in points]
        points = [SCALARVECTPROD([p[:-1],p[-1]]) for p in points]
        return Nurbs(self.degree,points)
        
    def add(self,nurbs):
        """
        To join two pinned splines.

        TODO:  to finish ...
        """
        if self.degree == nurbs.degree:
            nurbs = nurbs.set_knots(
                VECTSUM([nurbs.knots,(len(nurbs.knots))*[self.extent]]))
            points = nurbs.points
            return Nurbs(self.degree,self.points + nurbs.translate(
                [1.0,0.]).points)

    def circleArc(self,r,thetaStart,thetaEnd):
        """
        Create arbitrary NURBS circular arc.
        Ported from Piegl & Tiller, the NURBS book
        """       
        DEG2RAD = pi/180
        O,X,Y = self.points # local frame
        thetaStart = DEG2RAD * thetaStart
        thetaEnd = DEG2RAD * thetaEnd
        if thetaEnd < thetaStart: thetaEnd = pi + thetaEnd
        theta = thetaEnd-thetaStart
        if theta < pi/4: narcs = 1
        elif theta <= pi: narcs = 2
        elif theta <= 3*pi/2: narcs = 3
        else: narcs = 4
        dtheta = theta/narcs
        n = 2*narcs     # n+1 controls points
        Pw = (n+1)*[0]
        U  = (n+4)*[0]
        w1 = cos(dtheta/2.0)   # dtheta/2 is base angle
        P0 = VECTSUM([O, SCALARVECTPROD([r*cos(thetaStart),X]),
                      SCALARVECTPROD([r*sin(thetaStart),Y]) ])
        T0 = VECTSUM([ SCALARVECTPROD([-sin(thetaStart),X]),
                       SCALARVECTPROD([cos(thetaStart),Y]) ])  # initialize start values
        Pw[0] = P0+[1]
        index = 0; angle = thetaStart
        for i in range(1,narcs+1):  # create narcs segments
            angle = angle + dtheta
            P2 = VECTSUM([O, SCALARVECTPROD([r*cos(angle),X]),
                      SCALARVECTPROD([r*sin(angle),Y]) ])
            Pw[index+2] = P2+[1]
            T2 = VECTSUM([ SCALARVECTPROD([-sin(angle),X]),
                       SCALARVECTPROD([cos(angle),Y]) ])
            P1 = intersect2DLines(P0,T0,P2,T2)
            Pw[index+1] = SCALARVECTPROD([w1,P1])+[w1]
            index = index + 2
            if i < narcs:
                P0 = P2; T0 = T2
        j = 2*narcs+1   # load thetaEnd knot vector
        for i in range(3):  U[i] = 0; U[i+j] = 1
        if narcs == 1: pass
        elif narcs == 2: U[3] = U[4] = 0.5
        elif narcs == 3: U[3] = U[4] = 1./3; U[5] = U[6] = 2./3
        elif narcs == 4: U[3] = U[4] = 1./4; U[5] = U[6] = 2./4; U[7] = U[8] = 3./4
        return Nurbs(2,Pw).set_knots(U)

def orthogonal(vect2D):
    """
    Return the 2D vector normal to vect2D input.
    """
    x,y = vect2D
    return [y,-x]

def computeCenter(P0,P1,P2):
    """
    Compute the center of circle tangent in P0 and P2 to (P0,P1,P2) polyline.

    Notice that line segments P0-P1 and P1-P2 have equal lenghts.
    Return the circle center O.
    """
    T0 = orthogonal(VECTDIFF([P0,P1]))
    T2 = orthogonal(VECTDIFF([P2,P1]))
    return intersect2DLines(P0,T0,P2,T2)


def parseProfile(commands,offsetx=0,offsety=0):
    """
    To parse a list of turtle commands.
    
    "b1" => turn 'alpha' degrees and move forward for 'length' units
    "b2" => turn 'alpha' degrees, move forward for 'length' units; then
            turn 'beta' degrees and forward for 'length' units. Use the generated
            (p0,p1,p2) points as control points of a pinned uniform B-spline,
            since weights are ones; 'alpha'=0 gives a curve tangent to previous segment.
    "c2" => gives a circular arc of angle 'beta' forming an angle 'alpha' with the
            previous curve segment. The radius is implicitly computed from the 'lenght'
            parameter, which is the size of both control polyline segments.
            notice that the arc angle is equal to the deviation of the seoond control
            segment from the direction of the first one.
    TODO:   other local movements for highe degrees curves.
    
    Return a list of Nurbs objects.
    """
    # parser initializing
    profile = []
    p0, p1, p2 = [0,0], [0.5,0], [1,0]
    p0, p1, p2 = [VECTSUM([p,[offsetx,offsety]]) for p in [p0, p1, p2]]
    
    # main parser cycle
    for command in commands:
        
        opcode, alpha, length = command[:3]
        direction = UNITVECT(VECTDIFF([p2,p1]))
        p0 = p2
        alpha = alpha * pi/180
        dir1 = [ cos(alpha)*direction[0] - sin(alpha)*direction[1],
               sin(alpha)*direction[0] + cos(alpha)*direction[1] ]
        
        if opcode == "b1":
            # linear displacements
            p2 = VECTSUM([ p0, SCALARVECTPROD([ length, dir1 ]) ])            
            p1 = SCALARVECTPROD([0.5, VECTSUM([p0,p2])])
            profile += [Nurbs(2, [p0+[1],p1+[1],p2+[1]], steps=2)]
            
        elif opcode == "b2":
            # curved (quadratic) displacements
            beta = command[3] * pi/180
            dir2 = [ cos(beta)*dir1[0] - sin(beta)*dir1[1],
               sin(beta)*dir1[0] + cos(beta)*dir1[1] ]                        
            p1 = VECTSUM([ p0, SCALARVECTPROD([ length, dir1 ]) ])
            p2 = VECTSUM([ p1, SCALARVECTPROD([ length, dir2 ]) ])
            profile += [Nurbs(2, [p0+[1],p1+[1],p2+[1]], steps=10)]
            
        elif opcode == "c2":
            # circular displacements
            beta = command[3] * pi/180
            dir2 = [ cos(beta)*dir1[0] - sin(beta)*dir1[1],
               sin(beta)*dir1[0] + cos(beta)*dir1[1] ]                        
            p1 = VECTSUM([ p0, SCALARVECTPROD([ length, dir1 ]) ])
            p2 = VECTSUM([ p1, SCALARVECTPROD([ length, dir2 ]) ])
            O = computeCenter(p0,p1,p2)
            T0,T2 = AA(COMP([UNITVECT,VECTDIFF]))([ [p0,O], [p2,O] ])
            angle = ACOS(INNERPROD([ T0,T2 ]))*180/pi
            radius = VECTNORM(VECTDIFF([ p0,O ]))
            X = UNITVECT(VECTDIFF([ p0,O ]))
            Y = dir1
            profile += [Nurbs(2,[O,X,Y],steps=10).circleArc(radius,0,angle)]

        elif True: # TODO
            pass
        
    return profile

def bulk(sizex,sizey,segments):
    """
    To compute the top and bottom 2D surfaces generated by the profile curve.

    'segments' = list of Nurbs curves;
    'sizex', 'sizey' = measures of the bulk material section (a rectangle). 

    Return a list of pairs (n,mapping).
    The pair number equates the number of curve segments in the input profile.
    """
    topSurface, bottomSurface = [],[]
    for seg in segments:
        extremePoints = [seg.points[0],seg.points[-1]]
        topPoints = [[p[0],sizey] for p in extremePoints]
        bottomPoints = [[p[0],0.0] for p in extremePoints]
        topSurface += [[seg.steps, BEZIER(S2)([BEZIER(S1)(topPoints),seg.curve])]]
        bottomSurface += [[seg.steps, BEZIER(S2)([seg.curve,BEZIER(S1)(bottomPoints)])]]
    return topSurface, bottomSurface 
    
def dom(n):
    """
    To generate a uniform partition of the unit interval.

    Return a HPC value (the pyplam geometric type).
    """
    return INTERVALS(1)(n)

def ROTATIONALSOLID (section):
    """
    To generate a rotational mapping of a planar section.

    Return a mapping function that can be used by a MAP primitive
    on a 3D domain.
    """
    def map_fn(point):
        u, v, w = point
        x, y = section([u, v])
        return [x, y * math.cos(w), y * math.sin(w)]
    return map_fn

#/////////////////////////////////////////////////////////////

if __name__=="__main__":

    # command input (better from file ...)
    commands = [["b1",30,2],["b2",15,1,-90],["b1",45,2],
                ["b1",-45,1.5],["c2",30,1.5,90]]

    # input parsing and 2D curve segments generation
    segments = parseProfile(commands,offsetx=-1,offsety=1.5)
    for seg in segments: print "\n", seg

    # visual check of generated 2D spline
    profile = STRUCT([MAP(seg.curve)(seg.domain) for seg in segments])
    frame = SKELETON(1)(BOX([1,2])(profile))
    VIEW(STRUCT([frame, profile]))

    # split of the 2D section in two regions
    sizex,sizey = VECTSUM([ SIZE([1,2])(profile), [0.0, 2.0] ])
    topSurface, bottomSurface = bulk(sizex,sizey,segments)

    # visual check of material removal from bulk
    top = STRUCT([MAP(f)(PROD([dom(n),dom(10)])) for n,f in topSurface] )
    bottom = STRUCT([MAP(f)(PROD([dom(n),dom(1)])) for n,f in bottomSurface] )
    VIEW(STRUCT([
        COLOR(GREEN)(SKELETON(1)(top)),
        COLOR(CYAN)(bottom) ]))

    # rotational solid generation
    dom3D = INSR(PROD)([dom(n),dom(1),S(1)(3*PI/2)(dom(64))])
    VIEW(dom3D)
    lathered = STRUCT([MAP(ROTATIONALSOLID(f))(dom3D) for n,f in bottomSurface])
    VIEW(lathered)
