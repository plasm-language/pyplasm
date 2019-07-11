using LinearAlgebra

const Matrix3=MMatrix{3, 3, Float64}
const Matrix4=MMatrix{4, 4, Float64}

Matrix3(a0,a1,a2,a3,a4,a5,a6,a7,a8)= Matrix3([a0 a1 a2 ; a3 a4 a5 ; a6 a7 a8])
Matrix3() =  Matrix4(1,0,0, 0,1,0, 0,0,1)

Matrix4(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15)= Matrix4([a0 a1 a2 a3 ; a4 a5 a6 a7; a8 a9 a10 a11; a12 a13 a14 a15])
Matrix4() =  Matrix4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1)

function flatten(T::Matrix3)
	return Vector{Float32}([ 
		T[1,1],T[1,2],T[1,3],
		T[2,1],T[2,2],T[2,3], 
		T[3,1],T[3,2],T[3,3]])
end

function flatten(T::Matrix4)
	return Vector{Float32}([
		T[1,1],T[1,2],T[1,3],T[1,4],
		T[2,1],T[2,2],T[2,3],T[2,4],
		T[3,1],T[3,2],T[3,3],T[3,4],
		T[4,1],T[4,2],T[4,3],T[4,4]])
end



function dropW(T)
	return Matrix3(
		T[1,1],T[1,2],T[1,3],
		T[2,1],T[2,2],T[2,3],
		T[3,1],T[3,2],T[3,3])
end

function translateMatrix(vt::Point3d)
	return Matrix4(
		1.0, 0.0, 0.0, vt[1],
		0.0, 1.0, 0.0, vt[2],
		0.0, 0.0, 1.0, vt[3],
		0.0, 0.0, 0.0, 1.0)	
end

function scaleMatrix(vs::Point3d)
	return Matrix4(
		vs[1], 0.0, 0.0, 0.0,
		0.0, vs[2], 0.0, 0.0,
		0.0, 0.0, vs[3], 0.0,
		0.0, 0.0, 0.0, 1.0)					
end	

function lookAtMatrix(eye::Point3d, center::Point3d, up::Point3d)
	forward = normalized(center-eye)
	side    = normalized(cross(forward,up))
	up      = cross(side,forward)
	m = Matrix4(
		side[1],up[1],-forward[1], 0.0,
		side[2],up[2],-forward[2], 0.0,
		side[3],up[3],-forward[3], 0.0,
		0.0,0.0,0.0,1.0
	)
	return transpose(m) * translateMatrix(-1*eye)
end
	

function perspectiveMatrix(fovy::Float64, aspect::Float64, zNear::Float64, zFar::Float64)
	radians =  deg2rad(fovy/2.0)
	cotangent = cos(radians) / sin(radians)
	m=Matrix4()
	m[1,1] = cotangent / aspect
	m[2,2] = cotangent
	m[3,3] = -(zFar + zNear) / (zFar - zNear)
	m[3,4] = -1.0
	m[4,3] = -2.0 * zNear * zFar / (zFar - zNear)
	m[4,4] =  0.0
	return transpose(m)
end


function orthoMatrix(left::Float64, right::Float64, bottom::Float64, top::Float64, nearZ::Float64, farZ::Float64)
	m=Matrix4()
	m[1,1] = 2 / (right-left); m[1,2] =                0; m[1,3] =                 0; m[1,4] = -(right+left) / (right-left)
	m[2,1] =                0; m[2,2] = 2 / (top-bottom); m[2,3] =                 0; m[2,4] = -(top+bottom) / (top-bottom)
	m[3,1] =                0; m[3,2] =                0; m[3,3] = -2 / (farZ-nearZ); m[3,4] = -(farZ+nearZ) / (farZ-nearZ)
	m[4,1] =                0; m[4,2] =                0; m[4,3] =                 0; m[4,4] = 1
	return m
end

function getLookAt(T::Matrix4)
	T=inv(T)
	pos=          (Point3d(  T[1,4], T[2,4], T[3,4]))
	dir=normalized(Point3d( -T[1,3],-T[2,3],-T[3,3]))
	vup=normalized(Point3d(  T[1,2], T[2,2], T[3,2]))
	pos,dir,vup
end
