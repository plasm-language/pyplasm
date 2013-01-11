#include <xge/xge.h>
#include <xge/vec.h>



/////////////////////////////////////////////////////////////
Vec4f Vec4f::operator*(const Mat4f& m) const
{
	const Vec4f& v=(*this);
	const float* mmat=m.mat;

	return Vec4f(
		(mmat[ 0]*v.x+mmat[ 4]*v.y+mmat[ 8]*v.z+mmat[12]*v.w),
		(mmat[ 1]*v.x+mmat[ 5]*v.y+mmat[ 9]*v.z+mmat[13]*v.w),
		(mmat[ 2]*v.x+mmat[ 6]*v.y+mmat[10]*v.z+mmat[14]*v.w),
		(mmat[ 3]*v.x+mmat[ 7]*v.y+mmat[11]*v.z+mmat[15]*v.w)
	);
}



/////////////////////////////////////////////////////////////
Vecf Vecf::operator*(const Matf& m) const
{
	const Vecf& h=*this;

	XgeDebugAssert(m.dim==h.dim);
	int dim=h.dim;
	Vecf ret(dim);
	for (int c=0;c<=dim;c++)
	for (int r=0;r<=dim;r++) 
		ret.mem[c]+=h.mem[r]*(float)m(r,c);
	return ret;
}
 

