#ifndef _QUATERION4F_H__
#define _QUATERION4F_H__

#include <xge/xge.h>
#include <xge/vec.h>
#include <xge/mat.h>

//predeclaration
class Quaternion;


//================================================
//! quaternion to work in 3D
//================================================
class XGE_API Quaternion
{
	//! cutoff for sine near zero
	static const float ms_fEpsilon;

	//! internal utility
	static inline float project_to_sphere(float r, float x, float y)
	{
		float d = sqrt(x*x + y*y);
		if (d < r * 0.70710678118654752440f) return sqrt(r*r - d*d);
		float t = r / 1.41421356237309504880f;
		return t*t / d;
	}


public:

	float x,y,z,w;


	//! internal use
	static const Quaternion ZERO;

	//! internal use
	static const Quaternion IDENTITY;

	//! default constructor
	inline Quaternion ()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 1;
	}

	//! copy constructor
	/*! 
		@py 
		q=Quaternion(Quaternion())
		@endpy
	*/
	inline Quaternion (const Quaternion& rkQ)
	{
		w = rkQ.w;
		x = rkQ.x;
		y = rkQ.y;
		z = rkQ.z;
	}


	//! default constructor
	/*! 
		@py 
		q=Quaternion()
		q=Quaternion(1,0,0,0)
		assert q.fuzzyEqual(Vec4f(0,0,0,1))
		@endpy
	*/
	inline explicit Quaternion (float fW ,float fX, float fY , float fZ )
	{
		w = fW;
		x = fX;
		y = fY;
		z = fZ;
	}

	//! constructor from STL std::vector<float> of 4 components
	/*! 
		@py 
		q=Quaternion([1,0,0,0])
		assert q.fuzzyEqual(Vec4f(0,0,0,1))
		@endpy
	*/
	inline explicit Quaternion(const std::vector<float>& src)
	{
		if (src.size()!=4)
			Utils::Error(HERE,"Quaternion::Quaternion(const std::vector<float>& src) invalid argument src, src.size()=%d but it's size must be 4",(int)src.size());

		w=src[0];
		x=src[1];
		y=src[2];
		z=src[3];
	}



	//! constructor from axis and angle
	/*! 
		@py 
		q=Quaternion(Vec3f(0,0,1),pi/4)
		@endpy
	*/
	inline explicit Quaternion(Vec3f axis, float angle)
	{
		// DebugAssert:  axis[] is unit length
		//
		// The quaternion representing the rotation is
		//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

		axis=axis.normalize();

		float halfangle = 0.5f*angle;
		float fSin = sin(halfangle);
		this->w = (float)cos(halfangle);
		this->x = fSin*axis.x;
		this->y = fSin*axis.y;
		this->z = fSin*axis.z;
	}

	//! costructor from Mat4f
	/*! 
		@py 
		q=Quaternion(Mat4f())
		assert q.fuzzyEqual(Vec4f(0,0,0,1))
		@endpy
	*/
	inline explicit Quaternion(Mat4f& m)
	{
		fromMatrix(m);
	}

	//! constructor from euler angles
	/*! 
		@py 
		q=Quaternion(Vec3f(1,0,0),Vec3f(0,1,0),Vec3f(0,0,1))
		assert q.fuzzyEqual(Vec4f(0,0,0,1))
		@endpy
	*/
	inline  explicit Quaternion (const Vec3f& xAxis, const Vec3f& yAxis, const Vec3f& zAxis)
	{
		Mat4f kRot;

		kRot.set(0,0, xAxis.x);
		kRot.set(1,0, xAxis.y);
		kRot.set(2,0, xAxis.z);

		kRot.set(0,1, yAxis.x);
		kRot.set(1,1, yAxis.y);
		kRot.set(2,1, yAxis.z);

		kRot.set(0,2, zAxis.x);
		kRot.set(1,2, zAxis.y);
		kRot.set(2,2, zAxis.z);

		this->fromMatrix(kRot);
	}

	//! get single component
	/*! 
		@py 
		v=Quaternion(1,2,3,4);v.set(1,10);assert v.get(0)==1 and v.get(1)==10 and v.get(2)==3  and v.get(3)==4
		@endpy
	*/
	inline float get(int i)
	{
		if (i<0 || i>3)
			Utils::Error(HERE,"float Vec4f::get(int i) argument i=%d ouside valid range [0..3]",i);

		if      (!i  ) return x;
		else if (i==1) return y;
		else if (i==2) return z;
		else           return w;
	}

	//! set single component
	/*! 
		@py 
		v=Quaternion(1,2,3,4);v.set(1,10);assert v.get(0)==1 and v.get(1)==10 and v.get(2)==3  and v.get(3)==4
		@endpy
	*/
	inline void set(int i,float value)
	{
		if (i<0 || i>3)
			Utils::Error(HERE,"void Vec4f::set(int i,float value) argument i=%d ouside valid range [0..3]",i);

		if      (!i  ) x=value;
		else if (i==1) y=value;
		else if (i==2) z=value;
		else           w=value;
	}


	//! access operator by []
	/*! 
		@py 
		v=Quaternion(1,2,3,4);assert v[0]==1 and v[1]==2 and v[2]==3  and v[3]==4
		@endpy
	*/
	inline float& operator[](int i)
	{
		if (i<0 || i>3)
			Utils::Error(HERE,"float& Vec4f::operator[](int i) argument i=%d ouside valid range [0..3]",i);

		if      (!i)   return x;
		else if (i==1) return y;
		else if (i==2) return z;
		else           return w;
	}

	//! access operator by []
	/*! 
		@py 
		v=Quaternion(1,2,3,4);assert v[0]==1 and v[1]==2 and v[2]==3  and v[3]==4
		@endpy
	*/
	inline const float& operator[](int i) const
	{
		if (i<0 || i>3)
			Utils::Error(HERE,"const float& Vec4f::operator[](int i) const argument i=%d ouside valid range [0..3]",i);

		if      (!i)   return x;
		else if (i==1) return y;
		else if (i==2) return z;
		else           return w;
	}

	//! set this quaternion to do a transformation as Mat4f
	/*! 
		@py 
		q=Quaternion()
		q.fromMatrix(Mat4f())
		assert q.fuzzyEqual(Vec4f(0,0,0,1))
		@endpy
	*/
	void fromMatrix(Mat4f& m);

	//! convert to Mat4f
	/*! 
		@py 
		q=Quaternion()
		assert q.getMatrix().almostIdentity(1e-4)
		@endpy
	*/
	inline Mat4f getMatrix() const
	{
		float kRot[3][3];
		float fTx  = 2.0f*x;
		float fTy  = 2.0f*y;
		float fTz  = 2.0f*z;
		float fTwx = fTx*w;
		float fTwy = fTy*w;
		float fTwz = fTz*w;
		float fTxx = fTx*x;
		float fTxy = fTy*x;
		float fTxz = fTz*x;
		float fTyy = fTy*y;
		float fTyz = fTz*y;
		float fTzz = fTz*z;

		kRot[0][0] = 1.0f-(fTyy+fTzz);
		kRot[0][1] = fTxy-fTwz;
		kRot[0][2] = fTxz+fTwy;
		kRot[1][0] = fTxy+fTwz;
		kRot[1][1] = 1.0f-(fTxx+fTzz);
		kRot[1][2] = fTyz-fTwx;
		kRot[2][0] = fTxz-fTwy;
		kRot[2][1] = fTyz+fTwx;
		kRot[2][2] = 1.0f-(fTxx+fTyy);

		float _mat[16]={
			kRot[0][0],kRot[0][1],kRot[0][2],0,
			kRot[1][0],kRot[1][1],kRot[1][2],0,
			kRot[2][0],kRot[2][1],kRot[2][2],0,
			0,0,0,1
		};

		Mat4f m(_mat);
		return m;

	}

	//! get axis of rotation
	/*! 
		@py 
		q=Quaternion()
		q.getAxis()
		@endpy
	*/
	inline Vec3f getAxis() const
	{
		float len = x*x+y*y+z*z;
		if ( len > 0.0 )
		{
			float ilen = 1.0f/sqrt(len);
			return Vec3f(x*ilen,y*ilen,z*ilen);
		}
		else
		{
			// angle is 0 (mod 2*pi), so any axis will do
			return Vec3f(1,0,0);
		}
	}

	//! get angle of rotation
	/*! 
		@py 
		q=Quaternion()
		assert q.getAngle()==0
		@endpy
	*/
	inline float getAngle() const
	{
		float len = x*x+y*y+z*z;
		return (len>0.0)?(2.0f*acos(Utils::Clampf(w,-1,+1))):0;
	}

	//! get local coordinate system
	/*! 
		@py 
		q=Quaternion()
		x,y,z=Vec3f(),Vec3f(),Vec3f();q.getAxis(x,y,z)
		assert x.fuzzyEqual(Vec3f(1,0,0)) and y.fuzzyEqual(Vec3f(0,1,0)) and z.fuzzyEqual(Vec3f(0,0,1))
		@endpy
	*/
	inline void getAxis (Vec3f& xAxis, Vec3f& yAxis, Vec3f& zAxis) const
	{
		Mat4f kRot=getMatrix();
		xAxis.x = kRot(0,0);xAxis.y = kRot(1,0); xAxis.z = kRot(2,0);
		yAxis.x = kRot(0,1);yAxis.y = kRot(1,1);yAxis.z = kRot(2,1);
		zAxis.x = kRot(0,2);zAxis.y = kRot(1,2);zAxis.z = kRot(2,2);
	}

	//! test strict equality
	/*! 
		@py 
		assert Quaternion()==Quaternion()
		@endpy
	*/
	bool operator==(const Quaternion& rhs) const
	{
		return (rhs.x == x) && (rhs.y == y) && (rhs.z == z) && (rhs.w == w);
	}

	//! assigment operator
	/*! 
		@py 
		q=Quaternion()
		q.assign(Quaternion(1,2,3,4));assert q==Quaternion(1,2,3,4)
		@endpy
	*/
	inline Quaternion& operator= (const Quaternion& rkQ)
	{
		w = rkQ.w;
		x = rkQ.x;
		y = rkQ.y;
		z = rkQ.z;
		return *this;
	}


	//! assignment operator from STL std::vector<float>
	/*! 
		@py 
		q=Quaternion()
		q.assign([1,2,3,4])
		assert q==Quaternion([1,2,3,4])
		@endpy
	*/
	inline Quaternion& operator= (const std::vector<float>& src)
	{
		if (src.size()!=4)
			Utils::Error(HERE,"Quaternion& operator=(const std::vector<float>& src) invalid argument src, src.size()=%d but it's size must be 4",(int)src.size());

		this->w=src[0];
		this->x=src[1];
		this->y=src[2];
		this->z=src[3];
		return *this;
	}


	//! test fuzzy equality
	inline bool  fuzzyEqual(const Vec4f& v,float Epsilon=0.001f) const {
		return (x > (v.x - Epsilon) && x < (v.x + Epsilon) && 
				y > (v.y - Epsilon) && y < (v.y + Epsilon) &&
				z > (v.z - Epsilon) && z < (v.z + Epsilon) &&
				w > (v.w - Epsilon) && w < (v.w + Epsilon));
	};

	//! operator Quaternion + Quaternion
	/*! 
		@py 
		q=Quaternion()+Quaternion()
		@endpy
	*/
	inline Quaternion operator+ (const Quaternion& rkQ) const
	{
		return Quaternion(w+rkQ.w,x+rkQ.x,y+rkQ.y,z+rkQ.z);
	}

	//! operator -Quaternion
	/*! 
		@py 
		q=Quaternion()-Quaternion()
		@endpy
	*/
	inline Quaternion operator- (const Quaternion& rkQ) const
	{
		return Quaternion(w-rkQ.w,x-rkQ.x,y-rkQ.y,z-rkQ.z);
	}

	//! operator   Quaternion*Quaternion
	/*! 
		@py 
		q=Quaternion()*Quaternion()
		@endpy
	*/
	inline Quaternion operator* (const Quaternion& rkQ) const
	{
		// NOTE:  Multiplication is not generally commutative, so in most
		// cases p*q != q*p.

		return Quaternion
			(
			w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z,
			w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
			w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
			w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x
			);
	}

	//! operator Quaternion*coeff
	/*! @
		py q=Quaternion()*10
		@endpy
	*/
	inline Quaternion operator* (float fScalar) const
	{
		return Quaternion(fScalar*w,fScalar*x,fScalar*y,fScalar*z);
	}

	//! friend operator coeff*Quaternion
	#ifndef SWIG
	friend Quaternion operator*(float fScalar, const Quaternion& rkQ)
	{
		return Quaternion(fScalar*rkQ.w,fScalar*rkQ.x,fScalar*rkQ.y,fScalar*rkQ.z);
	}
	#endif

	//!negate quaternion
	/*! 
		@py 
		q=-Quaternion()
		@endpy
	*/
	inline Quaternion operator-() const
	{
		return Quaternion(-w,-x,-y,-z);
	}

	//! Quaternion dot Quaternion
	/*! 
		@py 
		Quaternion().Dot(Quaternion())
		@endpy
	*/
	inline float Dot(const Quaternion& rkQ) const
	{
		return w*rkQ.w+x*rkQ.x+y*rkQ.y+z*rkQ.z;
	}

	//! Norm
	/*! 
		@py 
		q=Quaternion()
		assert q.Norm()==1
		@endpy
	*/
	inline float Norm() const
	{
		return w*w+x*x+y*y+z*z;
	}

	//! inverse quaternion
	/*! 
		@py 
		q=Quaternion()
		assert q.Inverse().fuzzyEqual(Vec4f(0,0,0,1))
		@endpy
	*/
	inline Quaternion Inverse() const
	{
		float fNorm = w*w+x*x+y*y+z*z;
		if ( fNorm > 0.0 )
		{
			float fInvNorm = 1.0f/fNorm;
			return Quaternion(w*fInvNorm,-x*fInvNorm,-y*fInvNorm,-z*fInvNorm);
		}
		else
		{
			// return an invalid result to flag the error
			return ZERO;
		}
	}

	//! unit inverse
	/*! 
		@py 
		Quaternion().UnitInverse()
		@endpy
	*/
	inline Quaternion UnitInverse() const
	{
		// DebugAssert:  'this' is unit length
		return Quaternion(w,-x,-y,-z);
	}

	//! exp
	inline Quaternion Exp() const
	{
		// If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
		// exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
		// use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

		float fAngle = sqrt(x*x+y*y+z*z);
		float fSin = sin(fAngle);

		Quaternion kResult;
		kResult.w = cos(fAngle);

		if ( fabs(fSin) >= ms_fEpsilon )
		{
			float fCoeff = fSin/fAngle;
			kResult.x = fCoeff*x;
			kResult.y = fCoeff*y;
			kResult.z = fCoeff*z;
		}
		else
		{
			kResult.x = x;
			kResult.y = y;
			kResult.z = z;
		}

		return kResult;
	}

	//! Log
	/*! 
		@py 
		Quaternion().Log()
		@endpy
	*/
	inline Quaternion Log() const
	{
		// If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
		// log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
		// sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

		Quaternion kResult;
		kResult.w = 0.0;

		if ( fabs(w) < 1.0 )
		{
			float fAngle = acos(w);
			float fSin = sin(fAngle);
			if ( fabs(fSin) >= ms_fEpsilon )
			{
				float fCoeff = fAngle/fSin;
				kResult.x = fCoeff*x;
				kResult.y = fCoeff*y;
				kResult.z = fCoeff*z;
				return kResult;
			}
		}

		kResult.x = x;
		kResult.y = y;
		kResult.z = z;

		return kResult;
	}

	//! operator Quaternion*Vec3f
	/*! 
		@py 
		Quaternion()* Vec3f()
		@endpy
	*/
	inline Vec3f operator*(const Vec3f& v) const
	{
		// nVidia SDK implementation
		Vec3f uv, uuv;
		Vec3f qvec(x, y, z);
		uv = qvec.cross(v);
		uuv = qvec.cross(uv);
		uv *= (2.0f * w);
		uuv *= 2.0f;

		return v + uv + uuv;

	}
	//! Slerp
	/*! 
		@py 
		Quaternion.Slerp(0.5,Quaternion(),Quaternion())
		@endpy
	*/
	static inline Quaternion Slerp(float fT, const Quaternion& rkP,const Quaternion& rkQ)
	{
		float fCos = rkP.Dot(rkQ);
		float fAngle = acos(fCos);

		if ( fabs(fAngle) < ms_fEpsilon )
			return rkP;

		float fSin = (float)sin(fAngle);
		float fInvSin = 1.0f/fSin;
		float fCoeff0 = (float)sin((1.0f-fT)*fAngle)*fInvSin;
		float fCoeff1 = (float)sin(fT*fAngle)*fInvSin;
		return fCoeff0*rkP + fCoeff1*rkQ;
	}

	//! SlerpExtraSpinsn
	/*! 
		@py 
		Quaternion.SlerpExtraSpins(0.5,Quaternion(),Quaternion(),1)
		@endpy
	*/
	static inline Quaternion SlerpExtraSpins(float fT,const Quaternion& rkP, const Quaternion& rkQ, int iExtraSpins)
	{
		float fCos = rkP.Dot(rkQ);
		float fAngle = acos(fCos);
		if ( fabs(fAngle) < ms_fEpsilon ) return rkP;
		float fSin = (float)sin(fAngle);
		float fPhase = (float)M_PI*iExtraSpins*fT;
		float fInvSin = 1.0f/fSin;
		float fCoeff0 = (float)sin((1.0-fT)*fAngle - fPhase)*fInvSin;
		float fCoeff1 = (float)sin(fT*fAngle + fPhase)*fInvSin;
		return fCoeff0*rkP + fCoeff1*rkQ;
	}

	//! Intermediate
	/*! 
		@py 
		Quaternion.Intermediate(Quaternion(),Quaternion(),Quaternion(),Quaternion(),Quaternion())
		@endpy
	*/
	static  inline void Intermediate(const Quaternion& rkQ0,const Quaternion& rkQ1, const Quaternion& rkQ2,Quaternion& rkA, Quaternion& rkB)
	{
		// DebugAssert:  q0, q1, q2 are unit quaternions
		Quaternion kQ0inv = rkQ0.UnitInverse();
		Quaternion kQ1inv = rkQ1.UnitInverse();
		Quaternion rkP0 = kQ0inv*rkQ1;
		Quaternion rkP1 = kQ1inv*rkQ2;
		Quaternion kArg = 0.25*(rkP0.Log()-rkP1.Log());
		Quaternion kMinusArg = -kArg;
		rkA = rkQ1*kArg.Exp();
		rkB = rkQ1*kMinusArg.Exp();
	}

	//! Squad
	/*! 
		@py 
		Quaternion.Squad(0.5,Quaternion(),Quaternion(),Quaternion(),Quaternion())
		@endpy
	*/
	static inline Quaternion Squad(float fT,const Quaternion& rkP, const Quaternion& rkA,const Quaternion& rkB, const Quaternion& rkQ)
	{
		float fSlerpT = 2.0f*fT*(1.0f-fT);
		Quaternion kSlerpP = Slerp(fT,rkP,rkQ);
		Quaternion kSlerpQ = Slerp(fT,rkA,rkB);
		return Slerp(fSlerpT,kSlerpP,kSlerpQ);
	}

	//! mouse movement to Quaternion 
	/*! 
		@py 
		Quaternion.trackball(10,10,20,20)
		@endpy
	*/
	static inline Quaternion trackball(float p1x, float p1y, float p2x, float p2y)
	{
		//zero rotation
		if (p1x == p2x && p1y == p2y)  return Quaternion();
		const float TRACKBALLSIZE =0.8f;
		Vec3f p1(p1x,p1y,project_to_sphere(TRACKBALLSIZE,p1x,p1y));
		Vec3f p2(p2x,p2y,project_to_sphere(TRACKBALLSIZE,p2x,p2y));
		Vec3f a=Vec3f(p2).cross(Vec3f(p1));
		Vec3f d=p1-p2;
		float t = d.module()/ (2.0f*TRACKBALLSIZE);
		if (t >  1.0f) t =  1.0f;
		if (t < -1.0f) t = -1.0f;
		return Quaternion(Vec3f(a),(float)(2.0f * asin(t)));
	}

	//! python repr()
	/*! 
		@py 
		assert eval(repr(Quaternion()))==Quaternion();
		@endpy
	*/
	inline std::string repr() const
	{
		return Utils::Format("Quaternion(%e,%e,%e,%e)",w,x,y,z);
	}

	//! self test
	static int SelfTest();

}; //end class




#endif //_QUATERION4F_H__
