
# ////////////////////////////////////////////////////////////////////////////
mutable struct Box3d
	p1::Point3d
	p2::Point3d
	
	# constyructor
	function Box3d()
		new(Point3d(0,0,0),Point3d(0,0,0))
	end	
	
	# constructor
	function Box3d(p1::Point3d,p2::Point3d)
		new(p1,p2)
	end	
		
end

Base.:(==)(a::Box3d, b::Box3d) = a.p1 == b.p1 && a.p2 == b.p2

function invalidBox()
	m,M=typemin(Float64),typemax(Float64)
	return Box3d(Point3d(M,M,M),Point3d(m,m,m))	
end

function addPoint(box::Box3d,point::Point3d)
	for i in 1:3
		box.p1[i]=min(box.p1[i],point[i])
		box.p2[i]=max(box.p2[i],point[i])
	end
	return box
end

function getPoints(box::Box3d)
	return [
		Point3d(box.p1[1],box.p1[2],box.p1[3]),
		Point3d(box.p2[1],box.p1[2],box.p1[3]),
		Point3d(box.p2[1],box.p2[2],box.p1[3]),
		Point3d(box.p1[1],box.p2[2],box.p1[3]),
		Point3d(box.p1[1],box.p1[2],box.p2[3]),
		Point3d(box.p2[1],box.p1[2],box.p2[3]),
		Point3d(box.p2[1],box.p2[2],box.p2[3]),
		Point3d(box.p1[1],box.p2[2],box.p2[3])
	]
end