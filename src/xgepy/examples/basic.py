from pyplasm import *

import os,sys
from math import *	
import copy

v=Array() 
assert v.size()==0

#import copy
if True:
	src=Array([1,2,3])
	dst=copy.copy(src)
	print dst
	assert dst.size()==3 and dst[0]==1 and dst[1]==2 and dst[2]==3 

v=Array(3)
assert v.size()==3 and v[0]==0 and v[1]==0 and v[2]==0

v=Array([1,2,3])
assert v.size()==3 and v[0]==1 and v[1]==2 and v[2]==3 

v=Array([0,1,4,5],[0,1,2,3,4,5]) 
assert v.size()==4 and v[0]==0 and v[1]==1 and v[2]==4 and v[3]==5

v=Array([0,1,2])
v.assign(Array([3,4]))
assert v.size()==2 and v[0]==3 and v[1]==4

v=Array([0,1,2])
v.assign([3,4])
assert v.size()==2 and v[0]==3 and v[1]==4

assert Array(3).size()==3 

assert Array([0]).memsize()==4

v=Array([0,1,2])
v.resize(2)
assert v.size()==2 and v[0]==0 and v[1]==1

v=Array([0,1,2])
v.resize(4)
assert v.size()==4 and v[0]==0 and v[1]==1 and v[2]==2


v1=Array([0])
v2=Array([1])
v1.append(v2)
assert v1.size()==2 and v1[0]==0 and v1[1]==1

v=Array([1,2])
v.zero()
assert v.size()==2 and v[0]==0 and v[1]==0

assert Array([1,2,3])==Array([1,2,3])

assert Array([1,2,3])!=Array([3,4,5])

v=Array([1,2])
assert v.size()==2 and v[0]==1 and v[1]==2

v=Array([1,2,3,4]).extract([2,3],False)
assert v.size()==2 and v[0]==3 and v[1]==4

v=Array([1,2,3,4])
assert(eval(repr(v)))==Array([1,2,3,4])

v=Array([1,2,3,4])
assert eval(str(v))==[1,2,3,4]

b=Ball3f()
assert not b.isValid() and b.center==Vec3f(0,0,0)


if sys.platform!='cli':
	import copy
	src=Ball3f(10,Vec3f(1,2,3))
	dst=copy.copy(src)
	assert src.radius==dst.radius and src.center==dst.center

b=Ball3f(1,Vec3f(0,0,0))
assert b.radius==1 and b.center==Vec3f(0,0,0)

points=[1,0,0,  -1,0,0,  0,1,0,  0,-1,0 ] 
b=Ball3f.bestFittingBall(points) 
assert b.center.fuzzyEqual(Vec3f(0,0,0)) and fabs(1-b.radius)<1e-4

assert not Ball3f().isValid() and Ball3f(1.0,Vec3f(0,0,0)).isValid()

b=Ball3f()
b.assign(Ball3f(1,Vec3f(0,0,0)))
assert b.radius==1 and b.center==Vec3f(0,0,0)

assert Ball3f(1,Vec3f(0,0,0))==Ball3f(1,Vec3f(0,0,0)) and Ball3f(1,Vec3f(0,0,0))!=Ball3f(2,Vec3f(0,0,0))

b1=Ball3f(1,Vec3f(0,0,0))
b2=Ball3f(1+1e-8,Vec3f(1e-8,1e-8,1e-8))
assert b2.fuzzyEqual(b2)

assert Ball3f().surface()==0

assert Ball3f().volume()==0

assert eval(str(Ball3f(1,Vec3f(0,0,0))))==[1,[0,0,0]]
assert eval(repr(Ball3f(1,Vec3f(0,0,0))))==Ball3f(1,Vec3f(0,0,0))

b=Ballf()
assert not b.isValid() and b.radius<0

b=Ballf(Ballf(1,Vecf([1,0,0,0])))
assert b.radius==1 and b.center==Vecf([1,0,0,0])

b=Ballf(1,Vecf([1,0,0,0]))
assert b.dim()==3 and b.radius==1 and b.center==Vecf([1,0,0,0])

b=Ballf(1,Vecf(1,0,0,0))
assert b.dim()==3

points=[ 1,1,0,0,  1,-1,0,0,  1,0,1,0,  1,0,-1,0  ]
b=Ballf.bestFittingBall(3,points)
assert fabs(b.radius-1)<1e-4 and b.center.fuzzyEqual(Vecf([1,0,0,0]))

b=Ballf()
assert not b.isValid()

b=Ballf(1,Vecf(1,1,2,3))
b.assign(Ballf(1,Vecf(1,0,0,0)))
assert b.radius==1 and b.center==Vecf([1,0,0,0])

if sys.platform!='cli':
	import copy
	b=Ballf(1,Vecf(1,1,2,3))
	assert b==copy.copy(b)

if sys.platform!='cli':
	import copy
	b1=Ballf(1,Vecf(1,1,2,3))
	b2=copy.copy(b1)
	b2.radius+=1e-8
	assert b1.fuzzyEqual(b2)

b=Ballf(1,Vecf(1,0,0,0))
assert eval(str(b))==[1,[1,0,0,0]]

b=Ballf(1,Vecf(1,0,0,0))
assert eval(repr(b))==Ballf(1,Vecf(1,0,0,0))

b=Box3f()
assert not b.isValid()

if sys.platform!='cli':
	import copy
	b1=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
	b2=copy.copy(b1)
	assert b2.p1==b1.p1 and b2.p2==b1.p2

b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
assert b.isValid() and b.p1==Vec3f(0,0,0) and b.p2==Vec3f(1,1,1)

b=Box3f.buildFromCenter(Vec3f(0,0,0),Vec3f(1,1,1))
assert b.isValid() and b.p1==Vec3f(-0.5,-0.5,-0.5) and b.p2==Vec3f(+0.5,+0.5,+0.5)

b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
b.reset()
assert not b.isValid()

b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
p=[Vec3f(0,0,0),Vec3f(1,0,0),Vec3f(1,1,0),Vec3f(0,1,0),Vec3f(0,0,1),Vec3f(1,0,1),Vec3f(1,1,1),Vec3f(0,1,1)]
assert b.getPoint(0)==p[0] and b.getPoint(1)==p[1] and b.getPoint(2)==p[2] and b.getPoint(3)==p[3] and b.getPoint(4)==p[4] and b.getPoint(5)==p[5] and b.getPoint(6)==p[6] and b.getPoint(7)==p[7]

b=Box3f();b.add(Vec3f(1,1,1))
assert b.isValid() and b.p1==Vec3f(1,1,1) and b.p2==Vec3f(1,1,1)

b=Box3f()
b.add(Box3f(Vec3f(0,0,0),Vec3f(1,1,1)))
assert b.isValid() and b.p1==Vec3f(0,0,0) and b.p2==Vec3f(1,1,1)

b=Box3f()
assert not b.isValid()

b=Box3f(Vec3f(0,0,0),Vec3f(1,2,3))
assert b.size()==Vec3f(1,2,3)

b=Box3f(Vec3f(0,0,0),Vec3f(1,2,3))
assert b.minsize()==1

b=Box3f(Vec3f(0,0,0),Vec3f(1,2,3))
assert b.maxsize()==3

b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1)).translate(Vec3f(1,2,3))
assert b.p1==Vec3f(1,2,3) and b.p2==Vec3f(2,3,4)

b=Box3f(Vec3f(-1,-1,-1),Vec3f(1,1,1)).scale(2)
assert b.p1==Vec3f(-2,-2,-2) and b.p2==Vec3f(+2,+2,+2)

b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
assert b.center()==Vec3f(0.5,0.5,0.5)

b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
assert b.contains(Vec3f(0.5,0.5,0.5)) and not b.contains(Vec3f(1.5,1.5,1.5))

b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
assert b.contains(Box3f(Vec3f(0.2,0.2,0.2),Vec3f(0.8,0.8,0.8))) 
b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
assert not b.contains(Box3f(Vec3f(0.2,0.2,0.2),Vec3f(1.8,1.8,1.8))) 

b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
assert b.overlap(Box3f(Vec3f(0.2,0.2,0.2),Vec3f(1.8,1.8,1.8))) 
b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
assert not b.overlap(Box3f(Vec3f(1.2,1.2,1.2),Vec3f(1.8,1.8,1.8))) 

b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
assert b.intersection(Box3f(Vec3f(0.2,0.2,0.2),Vec3f(1.8,1.8,1.8)))==Box3f(Vec3f(0.2,0.2,0.2),Vec3f(1,1,1))

b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
assert b==Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
assert b!=Box3f(Vec3f(0,0,0),Vec3f(1.1,1.1,1.1))

b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
assert b.fuzzyEqual(Box3f(Vec3f(0,0,0),Vec3f(1+1e-6,1+1e-6,1+1e-6)))

b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
assert b.surface()==6

b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
assert b.volume()==1

b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
assert(eval(repr(b)))==Box3f(Vec3f(0,0,0),Vec3f(1,1,1))

b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
assert eval(str(b))==[[0,0,0],[1,1,1]]

b=Boxf(3); assert not b.isValid()

b=Boxf(Boxf(Vecf(1, 0,0),Vecf(1, 1,1)))
assert b.p1.dim==2 and b.p1==Vecf(1, 0,0) and b.p2==Vecf(1, 1,1)

b=Boxf(3)
assert not b.isValid() and b.p1.dim==3

b=Boxf(Vecf(1, 0,0),Vecf(1, 1,1))
assert b.p1.dim==2 and b.p1==Vecf(1, 0,0) and b.p2==Vecf(1, 1,1)

b=Boxf(3,0,1)
assert b.p1.dim==3 and b.p1==Vecf(1, 0,0,0) and b.p2==Vecf(1, 1,1,1)

b=Boxf(3,0,1); b.reset(2)
assert b.p1.dim==2 and not b.isValid()
b=Boxf(3,0,1)
b.reset(); assert b.p1.dim==3 and not b.isValid()

b=Boxf(3)
b.add(Vecf(1,2,2,2))
assert b.isValid() and b.p1==Vecf(1,2,2,2) and b.p2==b.p1

b=Boxf(3)
b.add(Boxf(3,0,1))
assert b.isValid() and b.p1==Vecf(1,0,0,0) and b.p2==Vecf(1,1,1,1)

b=Boxf(3)
assert not b.isValid()

b=Boxf(3,0,1)
assert b.center()==Vecf(1,0.5,0.5,0.5)

b=Boxf(3,0,1)
assert b.dim()==3

b=Boxf(3,0,1)
assert b.size()==Vecf(0, 1,1,1)

b=Boxf(Vecf(1,0,0,0),Vecf(1,1,2,3))
assert b.minsizeidx()==1

b=Boxf(Vecf(1,0,0,0),Vecf(1,1,2,3))
assert b.maxsizeidx()==3

b=Boxf(Vecf(1,0,0,0),Vecf(1,1,2,3))
assert b.minsize()==1
b=Boxf(Vecf(1,0,0,0),Vecf(1,1,2,3))
assert b.maxsize()==3

b=Boxf(Vecf(1,0,0,0),Vecf(1,1,2,3))
assert b==Boxf(Vecf(1,0,0,0),Vecf(1,1,2,3))
b=Boxf(Vecf(1,0,0,0),Vecf(1,1,2,3))
assert b!=Boxf(Vecf(1,0,0,0),Vecf(1,1,2,4))\

b=Boxf(Vecf(1,0,0,0),Vecf(1,1,2,3))
assert b.fuzzyEqual(Boxf(Vecf(1,0,0,0),Vecf(1,1+1e-8,2+1e-8,3+1e-8)))

b=Boxf(Vecf(1,-1,-1,-1),Vecf(1,+1,+1,+1)).scale(2)
assert b.p1==Vecf(1.0,-2,-2,-2) and b.p2==Vecf(1.0,+2,+2,+2)

b=Boxf(Vecf(1,-1,-1,-1),Vecf(1,+1,+1,+1))
assert b.volume()==8

b=Boxf(3,0,1)
assert b.overlap(Boxf(3,0.5,1.5)) and not b.overlap(Boxf(3,1.5,2.5))

b=Boxf(3,0,1)
assert b.contains(Vecf(1,0.5,0.5,0.5))

b=Boxf(3,0,1).toBox3f()
assert b.p1==Vec3f(0,0,0) and b.p2==Vec3f(1,1,1)
b=Boxf(3,0,1)
assert(eval(repr(b)))==Boxf(3,0,1)

b=Boxf(3,0,1)
assert eval(str(b))==[[1,0,0,0],[1,1,1,1]]

c=Clock()

c1=Clock()
c2=Clock()
c1.assign(c2)

c1=Clock()
c2=Clock()
assert (c1-c2)<10 

c1=Clock()
assert c1.sec()<0.010 

c=Color4f(Color4f(0.2,0.3,0.4,1))
assert fabs(c.r-0.2)<1e-4 and fabs(c.g-0.3)<1e-4 and fabs(c.b-0.4)<1e-4 and c.a==1.0

c=Color4f(0.2,0.3,0.4,1)
assert fabs(c.r-0.2)<1e-4 and fabs(c.g-0.3)<1e-4 and fabs(c.b-0.4)<1e-4 and c.a==1.0

c=Color4f([0.2,0.3,0.4])
assert fabs(c.r-0.2)<1e-4 and fabs(c.g-0.3)<1e-4 and fabs(c.b-0.4)<1e-4 and c.a==1.0

c=Color4f([0.2,0.3,0.4,0.8])
assert fabs(c.r-0.2)<1e-4 and fabs(c.g-0.3)<1e-4 and fabs(c.b-0.4)<1e-4 and fabs(c.a-0.8)<1e-4

v=Color4f(1,1,1)
v.set(1,0)
assert v.get(0)==1 and v.get(1)==0 and v.get(2)==1

v=Color4f(0.1,0.2,0.3,1.0)
v.set(1,1)
assert fabs(v.get(0)-0.1)<1e-4 and v.get(1)==1.0 and fabs(v.get(2)-0.3)<1e-4 and v.get(3)==1.0

c=Color4f(0.1,0.2,0.3,1.0)
assert fabs(c[0]-0.1)<1e-4 and fabs(c[1]-0.2)<1e-4 and fabs(c[2]-0.3)<1e-4 and c[3]==1.0

assert Color4f(0.1,0.2,0.3,1.0)==Color4f(0.1,0.2,0.3,1.0) and Color4f(0.1,0.2,0.3,1.0)!=Color4f(0.1,0.2,0.4,1.0)

c=Color4f.randomRGB()
assert c.a==1

c=Color4f.randomRGBA()
assert c.a>=0 and c.a<=1

assert eval(str(Color4f(0.1,0.2,0.3,1.0)))==[0.1,0.2,0.3,1.0]

assert eval(repr(Color4f(0.1,0.2,0.3,1.0)))==Color4f(0.1,0.2,0.3,1.0)

f=Frustum()
box=f.aabb

f=Frustum()
f.getRay()==Ray3f(Vec3f(0,0,0),Vec3f(0,0,-1))

f=Frustum()
assert not f.intersect(Box3f(Vec3f(1,1,1),Vec3f(2,2,2)))

f=Frustum()
assert f.contains(Vec3f(0,0,-3))
 
m=Mat4f()
assert [m[i] for i in range(0,16)]==[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]
 
m=Mat4f([2,0,0,0,0,2,0,0,0,0,2,0,0,0,0,2])
m.assign(Mat4f())
assert m==Mat4f()
 
m=Mat4f([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
assert m==Mat4f()
 
m=Mat4f()
m.assign(Mat4f(range(0,16)))
assert [m[i] for i in range(0,16)]==range(0,16)
 
m=Mat4f()
m.assign(range(0,16))
assert [m[i] for i in range(0,16)]==range(0,16)
 
m=Mat4f()
assert m.get(0)==1 and m.get(5)==1 and m.get(10)==1 and m.get(15)==1
 
m=Mat4f()
assert m.a11()==1 and m.a22()==1 and m.a33()==1 and m.a44()==1
 
assert Mat4f(range(0,16))==Mat4f(range(0,16))
assert Mat4f(range(0,16))!=Mat4f(range(1,17))
 
assert Mat4f(range(0,16)).transpose()==Mat4f([0,4,8,12,1,5,9,13,2,6,10,14,3,7,11,15])
 
assert Mat4f.zero()==Mat4f([0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0])
 
assert Mat4f().determinant()==1
 
assert Mat4f().invert()==Mat4f()
 
m=Mat4f.scale(1,2,3)
assert m[0]==1 and m[5]==2 and m[10]==3
 
m=Mat4f.scale(Vec3f(1,2,3))
assert m[0]==1 and m[5]==2 and m[10]==3
 
m=Mat4f.translate(1,2,3)
assert m[3]==1 and m[7]==2 and m[11]==3
 
m=Mat4f.translate(Vec3f(1,2,3))
assert m[3]==1 and m[7]==2 and m[11]==3
 
assert (Mat4f.rotate(Vec3f(1,0,0),-pi/4) * Mat4f.rotate(Vec3f(1,0,0),+pi/4)).almostIdentity(1e-4)
 
assert (Mat4f.rotatex(-pi/4) * Mat4f.rotatex(+pi/4)).almostIdentity(1e-4)
 
m=Mat4f.ortho(-1,+1,-1,+1,0.1,100)
 
m=Mat4f.perspective(60,1,0.1,100)
 
m=Mat4f.frustum(-1,+1,-1,+1,0.1,100)
 
m=Mat4f.lookat(1,0,0,0,0,0,0,0,1)
 
m=Mat4f()
assert m.fuzzyEqual(Mat4f(),0)
 
m=Mat4f()
assert (m-m).almostZero(0)
 
m=Mat4f()
assert m.almostIdentity(0)
 
m=Mat4f()
assert m.row(0)==Vec4f(1,0,0,0) and m.row(1)==Vec4f(0,1,0,0) and m.row(2)==Vec4f(0,0,1,0) and m.row(3)==Vec4f(0,0,0,1)
 
m=Mat4f()
assert m.col(0)==Vec4f(1,0,0,0) and m.col(1)==Vec4f(0,1,0,0) and m.col(2)==Vec4f(0,0,1,0) and m.col(3)==Vec4f(0,0,0,1)
 
m=Mat4f.zero()+Mat4f()
assert m==Mat4f()
 
m=Mat4f()-Mat4f()
assert m==Mat4f.zero()
 
m=Mat4f()*Mat4f()
assert m==Mat4f()
 
m=Mat4f()*2
assert m==Mat4f([2,0,0,0,0,2,0,0,0,0,2,0,0,0,0,2])
 
m=Mat4f()
assert m*Vec4f(1,0,0,0)==m.col(0) and m*Vec4f(0,1,0,0)==m.col(1) and m*Vec4f(0,0,1,0)==m.col(2) and m*Vec4f(0,0,0,1)==m.col(3)  
 
m=Mat4f()
assert m*Vec3f(1,0,0)==Vec3f(1,0,0) and m*Vec3f(0,1,0)==Vec3f(0,1,0) and m*Vec3f(0,0,1)==Vec3f(0,0,1)
 
m=Mat4f()
t,r,s=Vec3f(),Vec3f(),Vec3f()
m.decompose(t,r,s)
assert t.fuzzyEqual(Vec3f(0,0,0)) and s.fuzzyEqual(Vec3f(1,1,1)) and r.fuzzyEqual(Vec3f(0,0,0))
 
assert eval(str(Mat4f()))==[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]
 
assert eval(repr(Mat4f()))==Mat4f()
 

m=Matf()
assert m.dim==0 and m.get(0,0)==1
 
m=Matf(Matf(3))
assert m.dim==3 and m.get(0,0)==1 and m.get(1,1)==1 and m.get(2,2)==1 and m.get(3,3)==1
 
m=Matf(3)
assert m.dim==3 and m.get(0,0)==1 and m.get(1,1)==1 and m.get(2,2)==1 and m.get(3,3)==1
 
m=Matf(0,1,2,3)
assert m.get(0,0)==0 and m.get(0,1)==1 and m.get(1,0)==2 and m.get(1,1)==3
 
m=Matf(0,1,2,3,4,5,6,7,8)
assert m.get(0,0)==0 and m.get(0,1)==1 and m.get(0,2)==2 and m.get(1,0)==3 and m.get(1,1)==4 and m.get(1,2)==5 and m.get(2,0)==6 and m.get(2,1)==7 and m.get(2,2)==8
 
m=Matf(range(0,9))
assert m.get(0,0)==0 and m.get(0,1)==1 and m.get(0,2)==2 and m.get(1,0)==3 and m.get(1,1)==4 and m.get(1,2)==5 and m.get(2,0)==6 and m.get(2,1)==7 and m.get(2,2)==8

m1=Matf()
m2=Matf(3)
m1.assign(m2)
assert m1.dim==3 and m1.almostIdentity(0)
 
m=Matf([1,0,0,1])
assert m.get(0,0)==1 and m.get(0,1)==0 and m.get(1,0)==0 and m.get(1,1)==1
 
m=Matf([1,0,0,1])
m.set(0,0,1.0)
 
m=Matf([1,0,0,1])
assert m.get(0,0)==1 and m.get(0,1)==0 and m.get(1,0)==0 and m.get(1,1)==1
 
m=Matf([1,0,0,1])
assert m[0]==1 and m[1]==0 and m[2]==0 and m[3]==1
 
m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
assert m.row(0)==Vecf(1,0,0,0)
 
m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
assert m.col(0)==Vecf(1,0,0,0)
 
m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]).swapRows(0,3)
assert m.row(0)==Vecf(0,0,0,1)
 
m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]).swapCols(0,3);
assert m.col(0)==Vecf(0,0,0,1)
 
m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]).toMat4f()
assert m==Mat4f()
 
m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
assert m.transpose()==m
 
m=Matf()
m.assign(Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]))
assert m.dim==3
 
m=Matf()
m.assign([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
assert m.dim==3
 
m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
assert m==m and m!=Matf([2,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
 
m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
assert m.fuzzyEqual(m,0)
 
m=Matf.zero(3)
assert m==Matf([0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0])
 
m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
assert (m-m).almostZero(0)
 
m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
assert m.almostIdentity(0)
 
m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
assert m.extract(2)==Matf([1,0,0,0,1,0,0,0,1])

m=Matf([1,0,0,1])
assert m.extract(2)==Matf([1,0,0,0,1,0,0,0,1])
 
assert (Matf(3)+Matf.zero(3))==Matf(3)
 
assert (Matf(3)-Matf.zero(3))==Matf(3)
 
assert (Matf(3)*Matf(3))==Matf(3)
 
m=Matf.scaleV(Vecf(0,1,2,3))
assert m.get(0,0)==1 and m.get(1,1)==1 and m.get(2,2)==2 and m.get(3,3)==3
 
assert (Matf.scaleV(Vecf(0,1,2,3)) * Matf.scaleH(Vecf(0,1,2,3))).almostIdentity(1e-3)
 
m=Matf.translateV(Vecf(0,1,2,3))
assert m.get(0,0)==1 and m.get(1,0)==1 and m.get(2,0)==2 and m.get(3,0)==3
 
assert (Matf.translateV(Vecf(0,1,2,3)) * Matf.translateH(Vecf(0,1,2,3))).almostIdentity(1e-3)
 
assert (Matf.rotateV(3,1,2,pi/4) * Matf.rotateV(3,1,2,-pi/4)).almostIdentity(1e-4)
 
assert (Matf.rotateH(3,1,2,pi/4) * Matf.rotateH(3,1,2,-pi/4)).almostIdentity(1e-4)
 
assert (Matf([1,0,0,1]) * 10)==Matf([10,0,0,10])
 
assert (Matf([1,0,0,1]) * Vecf([1,0]))==Vecf([1,0])
 
assert Matf(3).invert().fuzzyEqual(Matf(3))
 
assert eval(str(Matf([1,0,0,1])))==[1,0,0,1]
 
assert eval(repr(Matf([1,0,0,1])))==Matf([1,0,0,1])
 
h=Plane4f()
assert h[0]==0 and h[1]==0 and h[2]==1 and h[3]==0 and h.get(0)==0 and h.get(1)==0 and h.get(2)==1 and h.get(3)==0
 
h=Plane4f(1,0,0,1)
assert h[0]==1 and h[1]==0 and h[2]==0 and h[3]==1
 
h=Plane4f([1,0,0,1]); assert h[0]==1 and h[1]==0 and h[2]==0 and h[3]==1 
 
h=Plane4f(Vec3f(0,0,1),Vec3f(0,0,1))
assert h[0]==0 and h[1]==0 and h[2]==1 and h[3]==-1 
 
h=Plane4f(Vec3f(0,0,1),10) 
assert h[0]==0 and h[1]==0 and h[2]==1 and h[3]==-10
 
h=Plane4f(Vec3f(0,0,0),Vec3f(1,0,0),Vec3f(0,1,0))
assert h[0]==0 and h[1]==0 and h[2]==1 and h[3]==0
 
h=Plane4f()
h.set(Vec3f(0,0,0),Vec3f(1,0,0),Vec3f(0,1,0)); assert h[0]==0 and h[1]==0 and h[2]==1 and h[3]==0
 
h=Plane4f(1,1,1,0)
assert fabs(1-sqrt(h[0]*h[0]+h[1]*h[1]+h[2]*h[2]))<1e-4 and h[0]==h[1] and h[1]==h[2]
 
h=Plane4f.bestFittingPlane([  0,0,0,  1,0,0,  0,1,0 ])
assert h[0]==0 and h[1]==0 and h[2]==1 and h[3]==0
 
h=Plane4f(0,0,1,0)
assert h.getNormal()==Vec3f(0,0,1)
 
h=Plane4f(0,0,1,0)
assert h.getDistance(Vec3f(0,0,5))==5
 
h=Plane4f(0,0,1,0)
assert h.getDistance(0,0,5)==5
 
h=Plane4f(0,0,1,0).reverse()
assert h[0]==0 and h[1]==0 and h[2]==-1 and h[3]==0
 
h=Plane4f(0,0,1,0).forceBelow(Vec3f(0,0,5))
assert h[0]==0 and h[1]==0 and h[2]==-1 and h[3]==0
 
h=Plane4f(0,0,1,0).forceAbove(Vec3f(0,0,-5))
assert h[0]==0 and h[1]==0 and h[2]==-1 and h[3]==0
 
assert eval(repr(Plane4f(0,0,1,0)))==Plane4f(0,0,1,0)
 
assert eval(str(Plane4f(0,0,1,0)))==[0,0,1,0]
 
h=Planef()
assert h.dim==1 and h[0]==0 and h[1]==1
 
h=Planef(Planef(0,0,0,1))
assert h.dim==3 and h[0]==0 and h[1]==0 and h[2]==0 and h[3]==1
 
h=Planef(3)
assert h.dim==3 and h[0]==0 and h[1]==0 and h[2]==0 and h[3]==1
 
h=Planef([0,0,0,1])
assert h.dim==3 and h[0]==0 and h[1]==0 and h[2]==0 and h[3]==1
 
h=Planef(0,0,1)
assert h.dim==2 and h[0]==0 and h[1]==0 and h[2]==1
 
h=Planef(0,0,0,1)
assert h.dim==3 and h[0]==0 and h[1]==0 and h[2]==0 and h[3]==1
 
h=Planef(Vecf(0,0,0,1))
assert h.dim==3 and h[0]==0 and h[1]==0 and h[2]==0 and h[3]==1
 
h=Planef(Vecf([0,0,0,1]),1)
assert h.dim==3 and h[0]==-1 and h[1]==0 and h[2]==0 and h[3]==1
 
h=Planef(Vecf([0,0,0,1]),Vecf([1,0,0,0]))
assert h.dim==3 and h[0]==0 and h[1]==0 and h[2]==0 and h[3]==1
 
h=Planef(0,1,1,1)
assert h.dim==3 and h[0]==0 and fabs(1-sqrt(h[1]*h[1]+h[2]*h[2]+h[3]*h[3]))<1e-4
 
h=Planef.bestFittingPlane(3,[  0,0,0,  1,0,0,  0,1,0  ])
assert h.dim==3 and h[0]==0 and h[1]==0 and h[2]==0 and h[3]==1
 
h=Planef([10, 0,0,1]);
assert h.getNormal()==Vecf([0,0,0,1])
 
h=Planef([0, 0,0,1])
assert h.getDistance(Vecf([1, 0,0,10]))==10
 
h=Planef([0, 0,0,1]).reverse()
assert h.dim==3 and h[0]==0 and h[1]==0 and h[2]==0 and h[3]==-1
 
h=Planef([0, 0,0,1]).forceBelow(Vecf(1,0,0,1))
assert h.dim==3 and h[0]==0 and h[1]==0 and h[2]==0 and h[3]==-1
 
h=Planef([0, 0,0,1]).forceAbove(Vecf(1,0,0,-1))
assert h.dim==3 and h[0]==0 and h[1]==0 and h[2]==0 and h[3]==-1
 
h=Planef([0, 0,0,1])
assert h.projectPoint(Vecf([1,0,0,10]))==Vecf([1,0,0,0])
 
h=Planef.getRandomPlane(3)
assert h.dim==3 and fabs(1-h.getNormal().module())<1e-3
 
h=Planef.getRandomPlane(3)
assert fabs(h.getDistance(h.getRandomPoint()))<1e-3
 
h=Planef([0,0,0,1])
assert Boxf(Vecf(1,0,0,2),Vecf(1,1,1,3)).isAbove(h)
 
h=Planef([0,0,0,1])
assert Boxf(Vecf(1,0,0,-3),Vecf(1,1,1,-2)).isBelow(h)
 
h=Planef([0,0,0,1])
assert h.getMainDirection()==3
 
h=Planef([0,0,0,1])
assert eval(repr(h))==Planef([0,0,0,1])
 
h=Planef([0,0,0,1])
assert eval(str(h))==[0,0,0,1]
 

q=Quaternion(Quaternion())
 
q=Quaternion()
q=Quaternion(1,0,0,0)
assert q.fuzzyEqual(Vec4f(0,0,0,1))
 
q=Quaternion([1,0,0,0])
assert q.fuzzyEqual(Vec4f(0,0,0,1))
 
q=Quaternion(Vec3f(0,0,1),pi/4)
 
q=Quaternion(Mat4f())
assert q.fuzzyEqual(Vec4f(0,0,0,1))
 
q=Quaternion(Vec3f(1,0,0),Vec3f(0,1,0),Vec3f(0,0,1))
assert q.fuzzyEqual(Vec4f(0,0,0,1))
 
q=Quaternion()
q.fromMatrix(Mat4f())
assert q.fuzzyEqual(Vec4f(0,0,0,1))
 
q=Quaternion()
assert q.getMatrix().almostIdentity(1e-4)
 
q=Quaternion()
q.getAxis()
 
q=Quaternion()
assert q.getAngle()==0
 
q=Quaternion()
x,y,z=Vec3f(),Vec3f(),Vec3f();q.getAxis(x,y,z)
assert x.fuzzyEqual(Vec3f(1,0,0)) and y.fuzzyEqual(Vec3f(0,1,0)) and z.fuzzyEqual(Vec3f(0,0,1))
 
assert Quaternion()==Quaternion()
 
q=Quaternion()
q.assign(Quaternion(1,2,3,4));assert q==Quaternion(1,2,3,4)
 
q=Quaternion()
q.assign([1,2,3,4])
assert q==Quaternion([1,2,3,4])
 
q=Quaternion()+Quaternion()
 
q=Quaternion()-Quaternion()
 
q=Quaternion()*Quaternion()
 
q=-Quaternion()
 
Quaternion().Dot(Quaternion())
 
q=Quaternion()
assert q.Norm()==1
 
q=Quaternion()
assert q.Inverse().fuzzyEqual(Vec4f(0,0,0,1))
 
Quaternion().UnitInverse()
 
Quaternion().Log()
 
Quaternion()* Vec3f()
 
Quaternion.Slerp(0.5,Quaternion(),Quaternion())
 
Quaternion.SlerpExtraSpins(0.5,Quaternion(),Quaternion(),1)
 
Quaternion.Intermediate(Quaternion(),Quaternion(),Quaternion(),Quaternion(),Quaternion())
 
Quaternion.Squad(0.5,Quaternion(),Quaternion(),Quaternion(),Quaternion())
 
Quaternion.trackball(10,10,20,20)
 
assert eval(repr(Quaternion()))==Quaternion()
 
r=Ray3f()
assert r.origin==Vec3f(0,0,0) and r.dir==Vec3f(0,0,1)
 
r=Ray3f(Vec3f(1,2,3),Vec3f(0,1,0))
assert r.origin==Vec3f(1,2,3) and r.dir==Vec3f(0,1,0)
 
r=Ray3f()
assert r.getPoint(1)==Vec3f(0,0,1)
 
r=Ray3f()
r.setOrigin(Vec3f(1,2,3))
assert r.origin==Vec3f(1,2,3)
 
r=Ray3f()
r.setDir(Vec3f(1,0,0))
assert r.dir==Vec3f(1,0,0)
 
r=Ray3f()
assert r.intersection(Plane4f(0,0,1,-1))==Vec3f(0,0,1)
 
r=Ray3f()
assert r==Ray3f()
 
assert Ray3f()!=Ray3f(Vec3f(0,0,0),Vec3f(1,0,0))
 
assert fabs((Ray3f().intersectTriangle(Vec3f(0,0,1),Vec3f(1.01,0,1),Vec3f(1.01,1.01,1))-1.0)<0.001)
assert fabs((Ray3f(Vec3f(0,0,0),Vec3f(1,1,1)).intersectTriangle(Vec3f(0,0,1),Vec3f(1.01,0,1),Vec3f(1.01,1.01,1))-sqrt(3.0))<0.001)
assert Ray3f(Vec3f(0,0,0),Vec3f(1,0,0)).intersectTriangle(Vec3f(0,0,1),Vec3f(1.01,0,1),Vec3f(1.01,1.01,1))==-1
 
b=Box3f(Vec3f(-1,-1,-1),Vec3f(+1,+1,+1))
I=Ray3f(Vec3f(0,0,0),Vec3f(0,0,1)).intersectBox(b); assert len(I)==2 and fabs(I[0]+1)<1e-4 and fabs(I[1]-1)<1e-4
I=Ray3f(Vec3f(0,0,0),Vec3f(0,1,0)).intersectBox(b); assert len(I)==2 and fabs(I[0]+1)<1e-4 and fabs(I[1]-1)<1e-4
I=Ray3f(Vec3f(0,0,0),Vec3f(1,0,0)).intersectBox(b); assert len(I)==2 and fabs(I[0]+1)<1e-4 and fabs(I[1]-1)<1e-4
 
assert eval(str(Ray3f()))==[[0,0,0],[0,0,1]]
 
assert eval(repr(Ray3f()))==Ray3f()
 
r=Rayf()
assert r.origin==Vecf([1]) and r.dir==Vecf([0])
 
r=Rayf(Vecf(1,0,0,0),Vecf(0,0,0,1))
assert r.origin==Vecf([1,0,0,0]) and r.dir==Vecf([0,0,0,1])
 
r=Rayf(Vecf(1,0,0,0),Vecf(0,0,0,1))
assert r.getPoint(1)==Vecf([1,0,0,1])
 
r=Rayf(Vecf(1,0,0,0),Vecf(0,0,0,1))
r.setOrigin(Vecf(1,1,2,3)); assert r.origin==Vecf([1,1,2,3])
 
r=Rayf(Vecf(1,0,0,0),Vecf(0,0,0,1))
r.setDir(Vecf(0,1,0,0)); assert r.dir==Vecf([0,1,0,0])
 
r=Rayf(Vecf(1,0,0,0),Vecf(0,0,0,1))
r.intersection(Planef(-1,0,0,1))==Vecf(1,0,0,1)
 
r=Rayf()
assert r==Rayf()
 
r=Rayf(Vecf(1,0,0,0),Vecf(0,0,0,1))
assert r!=Rayf(Vecf(1,0,0,0),Vecf(0,0,1,0))
 
r=Rayf(Vecf(1,0,0,0),Vecf(0,0,0,1))
assert eval(str(r))==[[1,0,0,0],[0,0,0,1]]
 
r=Rayf(Vecf(1,0,0,0),Vecf(0,0,0,1))
assert eval(repr(r))==r

t=Texture()
assert t.width==0 and t.height==0 and t.bpp==0 and t.filename==""
 
t=Texture(100,100,24)
assert t.width==100 and t.height==100 and t.bpp==24 and t.memsize()==100*100*3
 
if sys.platform!='cli':
	t=Texture(2,2,24,[1,2,3, 4,5,6, 7,8,9, 10,11,12])
	assert t.width==2 and t.height==2 and t.bpp==24 and [t.get(i) for i in range(0,4*3)]==range(1,13)

t=Texture(2,2,8)
assert t.memsize()==2*2*1

t=Texture(2,2,8)
t.change(4,4,24)
assert t.memsize()==4*4*3
 
if sys.platform!='cli':
	t=Texture(4,4,24)
	t.change(2,2,8,[1,2,3,4])
	assert t.memsize()==2*2*1 and t.get(0)==1 and t.get(1)==2 and t.get(2)==3 and t.get(3)==4

if sys.platform!='cli':	 
	t=Texture(2,2,8,[0,1,2,3])
	assert [t.get(i) for i in range(0,4)]==range(0,4)
 
t=Texture(2,2,8)
t.set(0,0);t.set(1,1)
t.set(2,2);t.set(3,3)
assert [t.get(i) for i in range(0,4)]==range(0,4)
 
repr(Texture())
 
str(Texture())
 
v=Vec2f();assert v.x==0 and v.y==0
 
v=Vec2f(Vec2f(1,2)); assert v.x==1 and v.y==2
 
v=Vec2f(1,2);assert v.x==1 and v.y==2
 
assert Vec2f([1,2])==Vec2f(1,2) 
 
v=Vec2f(1,2)
v.assign(Vec2f(10,11))
assert v.x==10 and v.y==11
 
assert Vec2f().assign([1,2])==Vec2f(1,2)
 
assert Vec2f(1,2)==Vec2f(1,2) and Vec2f(1,2)!=Vec2f(3,4)
 
assert Vec2f(1,2).get(0)==1 and Vec2f(1,2).get(1)==2
 
v=Vec2f(1,2);v.set(1,10);assert v==Vec2f(1,10) 
 
v=Vec2f(1,2); assert v[0]==1 and v[1]==2
 
v=Vec2f(1,2); assert v[0]==1 and v[1]==2
 
assert eval(str(Vec2f(1,2)))==[1,2]
 
assert(eval(repr(Vec2f(1,2))))==Vec2f(1,2)
 
v=Vec3f(); assert v.x==0 and v.y==0 and v.z==0
 
v=Vec3f(1,2); assert v.x==1 and v.y==2 and v.z==0
v=Vec3f(1,2,3); assert v.x==1 and v.y==2 and v.z==3
 
assert Vec3f([1,2,3])==Vec3f(1,2,3) 
 
assert Vec3f().assign(Vec3f(1,2,3))==Vec3f(1,2,3)
 
assert Vec3f().assign([1,2,3])==Vec3f(1,2,3)
 
assert(Vec3f(1,2,3).module2()==14)
 
assert(Vec3f(2,0,0).module()==2)
 
assert(fabs(Vec3f(1,1,1).distance(Vec3f(2,2,2))-sqrt(3))<0.001)
 
assert(fabs(Vec3f(1,1,1).normalize().module()-1)<0.001)
 
v=Vec3f(1,2,3);v*=10;assert(v.x==10 and v.y==20 and v.z==30)
 
v=Vec3f(10,20,30);v/=10;assert(v.x==1 and v.y==2 and v.z==3)
 
v=Vec3f(10,20,30);v+=Vec3f(1,2,3);assert(v.x==11 and v.y==22 and v.z==33)
 
v=Vec3f(11,22,33);v-=Vec3f(1,2,3);assert(v.x==10 and v.y==20 and v.z==30)
 
v1=Vec3f(1,0,0);v2=Vec3f(0,1,0);v3=Vec3f(0,0,1)
assert(v1.cross(v2)==v3 and v1.cross(v2)==v3 and v3.cross(v1)==v2)
 
v=Vec3f(1,1,1)/0
assert(not v.isValid());
 
assert Vec3f(1,2,3).fuzzyEqual(Vec3f(1.0001,2.0001,3.0001),0.001)
assert not Vec3f(1,2,3).fuzzyEqual(Vec3f(1.0001,2.0001,3.0001),0.00001)
 
assert (Vec3f(1,2,3) + Vec3f(10,20,30)) == Vec3f(11,22,33)
 
assert (Vec3f(11,22,33) - Vec3f(1,2,3)) == Vec3f(10,20,30)
 
assert (Vec3f(1,1,1) * Vec3f(10,20,30)) == 60
 
assert (Vec3f(1,2,3) * 10) == Vec3f(10,20,30)
 
assert (Vec3f(10,20,30)/10) == Vec3f(1,2,3)
 
assert Vec3f(1,2,3) == Vec3f(1,2,3)
 
assert Vec3f(1,2,3) != Vec3f(1,2,4)
 
assert Vec3f(-1,-2,3).Abs()==Vec3f(1,2,3)
 
assert Vec3f(1,2,3).Min(Vec3f(-1,3,-4))==Vec3f(-1,2,-4)
 
assert Vec3f(1,2,3).Max(Vec3f(-1,3,-4))==Vec3f(1,3,3)
 
assert Vec3f(1,2,3).Max()==3
 
assert Vec3f(1,2,3).Min()==1
 
assert Vec3f(1,2,3).maxidx()==2
 
assert Vec3f(1,2,3).minidx()==0
 
v=Vec3f(1,2,3);v.set(1,10);assert v.get(0)==1 and v.get(1)==10 and v.get(2)==3
 
v=Vec3f(1,2,3);v.set(1,10);assert v.get(0)==1 and v.get(1)==10 and v.get(2)==3
 
assert Vec3f(1,2,3)[0]==1 and Vec3f(1,2,3)[1]==2 and Vec3f(1,2,3)[2]==3
 
assert Vec3f(4,0,0).rotate(Vec3f(0,0,1),pi/2).fuzzyEqual(Vec3f(0,1,0))
 
v=Vec3f(1,2,3)
assert(eval(repr(v)))==Vec3f(1,2,3)
 
v=Vec3f(1,2,3)
assert eval(str(v))==[1,2,3]
 
v=Vec4f()
assert v.x==0 and v.y==0 and v.z==0 and v.w==0
 
v=Vec4f(Vec4f(1,2,3,4))
assert v.x==1 and v.y==2 and v.z==3 and v.w==4
 
v=Vec4f(1,2,3,4)
assert v.x==1 and v.y==2 and v.z==3 and v.w==4
 
assert Vec4f([1,2,3,4])==Vec4f(1,2,3,4) 

assert(Vec4f(1,2,3,4).module2()==30)
 
assert(Vec4f(2,2,2,2).module()==4)
 
assert(fabs(Vec4f(1,1,1,1).normalize().module()-1)<0.001)
 
v=Vec4f(1,2,3,4);v.set(1,10);assert v.get(0)==1 and v.get(1)==10 and v.get(2)==3  and v.get(3)==4
 
v=Vec4f(1,2,3,4);v.set(1,10);assert v.get(0)==1 and v.get(1)==10 and v.get(2)==3  and v.get(3)==4
 
v=Vec4f(1,2,3,4);assert v[0]==1 and v[1]==2 and v[2]==3  and v[3]==4
 
v=Vec4f(1,2,3,4);assert v[0]==1 and v[1]==2 and v[2]==3  and v[3]==4
 
v=Vec4f(1,2,3,4);v*=10;assert(v.x==10 and v.y==20 and v.z==30 and v.w==40)
 
v=Vec4f(10,20,30,40);v/=10;assert(v.x==1 and v.y==2 and v.z==3 and v.w==4)
 
v=Vec4f(10,20,30,40);v+=Vec4f(1,2,3,4);assert(v.x==11 and v.y==22 and v.z==33 and v.w==44)
 
v=Vec4f(11,22,33,44);v-=Vec4f(1,2,3,4);assert(v.x==10 and v.y==20 and v.z==30 and v.w==40)
 
v=Vec4f()
v.assign(Vec4f(1,2,3,4));assert(v==Vec4f(1,2,3,4))
 
assert Vec4f().assign([1,2,3,4])==Vec4f(1,2,3,4)
v=Vec4f(1,2,3,4)
w=v.assign([5,6,7,8]); assert v.x==5 and v.y==6 and v.z==7 and v.w==8
 
assert Vec4f(1,2,3,4).Min(Vec4f(-1,3,-4,5))==Vec4f(-1,2,-4,4)
 
assert Vec4f(1,2,3,4).Max(Vec4f(-1,3,-4,5))==Vec4f(1,3,3,5)
 
assert (Vec4f(1,1,1,1) * Vec4f(10,20,30,40)) == 100
 
assert (Vec4f(1,2,3,4) + Vec4f(10,20,30,40)) == Vec4f(11,22,33,44)
 
assert (Vec4f(11,22,33,44) - Vec4f(1,2,3,4)) == Vec4f(10,20,30,40)
 
assert (Vec4f(1,2,3,4) * 10) == Vec4f(10,20,30,40)
 
assert (Vec4f(10,20,30,40)/10) == Vec4f(1,2,3,4)
 
assert(Vec4f(1,2,3,4) == Vec4f(1,2,3,4))
 
assert Vec4f(1,2,3,4) != Vec4f(1,2,4,4)
 
m=Mat4f()
assert Vec4f(1,0,0,0)*m==m.row(0) and Vec4f(0,1,0,0)*m==m.row(1) and Vec4f(0,0,1,0)*m==m.row(2) and Vec4f(0,0,0,1)*m==m.row(3)  
 
assert Vec4f(1,2,3,4).fuzzyEqual(Vec4f(1.0001,2.0001,3.0001,4.0001),0.001)
assert not Vec4f(1,2,3,4).fuzzyEqual(Vec4f(1.0001,2.0001,3.0001,4.0001),0.00001)
 
v=Vec4f(1,2,3,4)
assert(eval(repr(v)))==Vec4f(1,2,3,4)
 
v=Vec4f(1,2,3,4)
assert eval(str(v))==[1,2,3,4]
 
v=Vecf()
assert v.dim==0 and v[0]==0
 
v=Vecf(Vecf([1,2,3]))
assert v==Vecf(1,2,3)
 
v=Vecf([1,2,3])
assert v==Vecf(1,2,3)
 
v=Vecf(3)
assert v.dim==3 and v[0]==0 and v[1]==0 and v[2]==0
 
v=Vecf(10.0,20.0,30.0)
assert v.dim==2 and v[0]==10 and v[1]==20 and v[2]==30 
 
v=Vecf(10.0,20.0,30.0,40.0)
assert v.dim==3 and v[0]==10 and v[1]==20 and v[2]==30 and v[3]==40.0
 
assert Vecf(1,2,3,4,5)==Vecf([1,2,3,4,5])
 
assert Vecf(1,2,3,4,5,6)==Vecf([1,2,3,4,5,6])
 
assert Vecf(1,2,3,4,5,6,7)==Vecf([1,2,3,4,5,6,7])		
 
v=Vecf(1,2,3,4)
v.set(1,10);assert v.get(0)==1 and v.get(1)==10 and v.get(2)==3  and v.get(3)==4
 
v=Vecf(1,2,3,4)
v.set(1,10)
assert v.get(0)==1 and v.get(1)==10 and v.get(2)==3  and v.get(3)==4
 
v=Vecf(2)
v.set(1.0)
assert v[0]==1 and v[1]==1 and v[2]==1
 
v=Vecf(1,2,3)
assert v[0]==1 and v[1]==2 and v[2]==3
 
v=Vecf(10,20,30)
v/=10
assert v==Vecf(1,2,3)
 
v=Vecf([1.0,2.0])
v+=Vecf([3.0,4.0])
assert v==Vecf([4.0,6.0])
 
v=Vecf([1.0,2.0])
v-=Vecf([3.0,4.0])
assert v==Vecf([-2.0,-2.0])
 
v=Vecf()
v.assign(Vecf([1,2]))
assert v.dim==1 and v[0]==1 and v[1]==2
 
v=Vecf()
v.assign([1,2,3,4])
assert v==Vecf([1,2,3,4])

v=Vecf([1,2,3,4])
v.assign([5,6,7,8])
assert v[0]==5 and v[1]==6 and v[2]==7 and v[3]==8
 
assert Vecf([1,2]) * Vecf([3,4]) == 11
 
assert (Vecf([1.0,2.0]) + Vecf([3.0,4.0]))==Vecf([4.0,6.0])
 
assert Vecf([1.0,2.0]) - Vecf([3.0,4.0])==Vecf([-2.0,-2.0])
 
assert Vecf([1.0,2.0])*10==Vecf([10.0,20.0])
 
assert Vecf([10.0,20.0])/10==Vecf([1.0,2.0])
 
assert Vecf([1,2,3,4])==Vecf([1,2,3,4]) 
 
assert Vecf([1,2,3,4])!=Vecf([1,2,3,5]) 
 
assert Vecf([1,2,3]).Min(Vecf([-1,3,-4]))==Vecf([-1,2,-4])
 
assert Vecf([1,2,3]).Max(Vecf([-1,3,-4]))==Vecf([1,3,3])
 
assert(Vecf([1,2,3,4]).module2()==30)
 
assert(Vecf([2,2,2,2]).module()==4)
 
assert(fabs(Vecf([1,1,1,1]).normalize().module()-1)<0.001)
 
assert Vecf([1,2,3,4]).fuzzyEqual(Vecf([1.0001,2.0001,3.0001,4.0001]),0.001)
assert not Vecf(1,2,3,4).fuzzyEqual(Vecf([1.0001,2.0001,3.0001,4.0001]),0.00001)
 
assert (Vecf([1,0]) * Matf([1,0,0,1]))==Vecf([1,0])
 
v=Vecf([1,2,3,4])
assert(eval(repr(v)))==Vecf([1,2,3,4])
 
v=Vecf([1,2,3,4])
assert eval(str(v))==[1,2,3,4]

print "self test on basic data succeded"
