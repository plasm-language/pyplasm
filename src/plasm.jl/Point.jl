using StaticArrays

const Point2d=MVector{2,Float64}
const Point3d=MVector{3,Float64}
const Point4d=MVector{4,Float64}

Point2d() = Point3d(0.0,0.0)
Point3d() = Point3d(0.0,0.0,0.0)
Point4d() = Point4d(0.0,0.0,0.0,0.0)

function norm(p::Point2d)
	return sqrt(p[1]*p[1] + p[2]*p[2])
end

function norm(p::Point3d)
	return sqrt(p[1]*p[1] + p[2]*p[2] + p[3]*p[3])
end

function norm(p::Point4d)
	return sqrt(p[1]*p[1] + p[2]*p[2] + p[3]*p[3] + p[4]*p[4])
end

function normalized(p::Point3d)
	len=norm(p)
	return Point3d(p[1] / len, p[2] / len, p[3] / len)
end

function normalized(p::Point4d)
	len=norm(p)
	return Point3d(p[1] / len, p[2] / len, p[3] / len, p[4] / len)
end

function dropW(p::Point4d)
	return Point3d(p[1] / p[4], p[2] / p[4], p[3] / p[4])
end