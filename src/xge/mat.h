#ifndef _MAT_H__
#define _MAT_H__

#include <xge/xge.h>
#include <xge/archive.h>
#include <xge/vec.h>



//======================================================================
//! matrix 4x4 for working in 3d (homogeneous component in last col,row)
//======================================================================
class XGE_API Mat4f 
{
public:
	void Write(Archive& ar)
	{
		ar.WriteVectorFloat("mat",this->c_vector());	
	}

	void Read(Archive& ar)
	{
		std::vector<float> v=ar.ReadVectorFloat("mat");
		ReleaseAssert(v.size()==16);

		for (int i=0;i<16;i++) 
			this->mat[i]=v[i];
	}

public:

	//!inner data
	float mat[16];
	
	//!default constructor, build an identity matrix 4x4
	/*! 
		@py 
		m=Mat4f()
		assert [m[i] for i in range(0,16)]==[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]
		@endpy
	*/
	inline Mat4f()
	{
		mat[ 0]=1;mat[ 1]=0;mat[ 2]=0;mat[ 3]=0;
		mat[ 4]=0;mat[ 5]=1;mat[ 6]=0;mat[ 7]=0;
		mat[ 8]=0;mat[ 9]=0;mat[10]=1;mat[11]=0;
		mat[12]=0;mat[13]=0;mat[14]=0;mat[15]=1;
	}

	//! copy constructor
	/*! 
		@py 
		m=Mat4f([2,0,0,0,0,2,0,0,0,0,2,0,0,0,0,2])
		m.assign(Mat4f())
		assert m==Mat4f()
		@endpy
	*/
	inline Mat4f(const Mat4f& src)
	{
		memcpy(mat,src.mat,sizeof(float)*16);
	}

	//constructor from 16 values
	inline Mat4f(
		float a00,float a01,float a02,float a03,
		float a10,float a11,float a12,float a13,
		float a20,float a21,float a22,float a23,
		float a30,float a31,float a32,float a33)
	{
		mat[ 0]=a00;mat[ 1]=a01;mat[ 2]=a02;mat[ 3]=a03;
		mat[ 4]=a10;mat[ 5]=a11;mat[ 6]=a12;mat[ 7]=a13;
		mat[ 8]=a20;mat[ 9]=a21;mat[10]=a22;mat[11]=a23;
		mat[12]=a30;mat[13]=a31;mat[14]=a32;mat[15]=a33;
	}

	//! constructor from a pointer of float values (should contains 16 float)
	inline explicit Mat4f(const float* src)
		{memcpy(mat,src,sizeof(float)*16);}

	//! constructor from a pointer of double values (should contains 16 double)
	inline explicit Mat4f(const double* src)
		{for (int i=0;i<16;i++) mat[i]=(float)src[i];}

	//!constructor from STL std::vector<float> 
	/*! 
		The STL vector must contains 16 floats

		@py 
		m=Mat4f([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
		assert m==Mat4f()
		@endpy
	*/
	inline explicit Mat4f(const std::vector<float>& src)
	{
		if (src.size()!=16)
			Utils::Error(HERE,"Mat4f(const std::vector<float>& src) invalid argument src, src.size()=%d but it's size must be 16",(int)src.size());

		mat[ 0]=src[ 0];mat[ 1]=src[ 1];mat[ 2]=src[ 2];mat[ 3]=src[ 3];
		mat[ 4]=src[ 4];mat[ 5]=src[ 5];mat[ 6]=src[ 6];mat[ 7]=src[ 7];
		mat[ 8]=src[ 8];mat[ 9]=src[ 9];mat[10]=src[10];mat[11]=src[11];
		mat[12]=src[12];mat[13]=src[13];mat[14]=src[14];mat[15]=src[15];
	}

	//! assignment operator
	/*! 
		@py 
		m=Mat4f()
		m.assign(Mat4f(range(0,16)))
		assert [m[i] for i in range(0,16)]==range(0,16)
		@endpy
	*/
	inline Mat4f& operator=(const Mat4f& src)
	{
		memcpy(mat,src.mat,sizeof(float)*16);
		return *this;
	}

	//! assignment operator from STL std::vector
	/*! 
		The STL vector must contains 16 floats

		@py 
		m=Mat4f()
		m.assign(range(0,16))
		assert [m[i] for i in range(0,16)]==range(0,16)
		@endpy
	*/
	inline Mat4f operator=(const std::vector<float>& src)
	{
		if (src.size()!=16)
			Utils::Error(HERE,"Mat4f operator=(const std::vector<float>& src) invalid argument src, src.size()=%d but it's size must be 16",(int)src.size());

		memcpy(mat,&src[0],sizeof(float)*16);
		return *this;
	}

	//! get all elements as std::vector
	/*!
		@py
		list=Mat4f().c_vector()
		assert list==[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]
		@endpy
	*/
	std::vector<float> c_vector()
	{
		std::vector<float> ret;
		for (int i=0;i<16;i++) ret.push_back(mat[i]);
		return ret;
	}

	//! access operator
	/*! 
		@py 
		m=Mat4f()
		assert m.get(0)==1 and m.get(5)==1 and m.get(10)==1 and m.get(15)==1
		@endpy
	*/
	inline float get(int i) const
	{
		if (i<0 || i>=16)
			Utils::Error(HERE,"float Mat4f::get(int i) invalid argument i, i=%d but it must be in the range [0,16)",i);

		return this->mat[i];
	}

	inline void set(int i,float value) 
	{
		if (i<0 || i>=16)
			Utils::Error(HERE,"void Mat4f::set(int i,float value) invalid argument i, i=%d but it must be in the range [0,16)",i);

		this->mat[i]=value;
	}

	//! access elements (starting from 0)
	float operator () (int row, int col) 
		{return this->get(row*4+col);}

	//! access elements  (starting from 0)
	float get(int row,int col)
		{return this->get(row*4+col);}

	//!! set elements (starting from 0)
	void set(int row,int col,float value)
		{this->set(row*4+col,value);}


	//! access value (starting from 1)
	/*! 
		@py 
		m=Mat4f()
		assert m.a11()==1 and m.a22()==1 and m.a33()==1 and m.a44()==1
		@endpy
	*/
	inline float a11() {return mat[ 0];} 

	//! access value (starting from 1)
	inline float a12() {return mat[ 1];}

	//! access value (starting from 1)
	inline float a13() {return mat[ 2];}

	//! access value (starting from 1)
	inline float a14() {return mat[ 3];}

	//! access value (starting from 1)
	inline float a21() {return mat[ 4];}

	//! access value (starting from 1)
	inline float a22() {return mat[ 5];}

	//! access value (starting from 1)
	inline float a23() {return mat[ 6];}

	//! access value (starting from 1)
	inline float a24() {return mat[ 7];}

	//! access value (starting from 1)
	inline float a31() {return mat[ 8];}

	//! access value (starting from 1)
	inline float a32() {return mat[ 9];}

	//! access value (starting from 1)
	inline float a33() {return mat[10];}

	//! access value (starting from 1)
	inline float a34() {return mat[11];}

	//! access value (starting from 1)
	inline float a41() {return mat[12];}

	//! access value (starting from 1)
	inline float a42() {return mat[13];}

	//! access value (starting from 1)
	inline float a43() {return mat[14];}

	//! access value (starting from 1)
	inline float a44() {return mat[15];} 


	//! access operator
	inline float& operator[](int i) 
	{
		if (i<0 || i>=16)
			Utils::Error(HERE,"float& Mat4f operator[](int i) invalid argument i, i=%d but it must be in the range [0,16)",i);

		return this->mat[i];
	}

	//! access operator
	inline const float& operator[](int i) const
	{
		if (i<0 || i>=16)
			Utils::Error(HERE,"float& Mat4f operator[](int i) invalid argument i, i=%d but it must be in the range [0,16)",i);

		return this->mat[i];
	}

	//! strict comparison operator
	/*! 
		\return true if the two matrices are exacly the same, false otherwise

		@py 
		assert Mat4f(range(0,16))==Mat4f(range(0,16))
		assert Mat4f(range(0,16))!=Mat4f(range(1,17))
		@endpy
	*/
	inline bool operator==(const Mat4f& b)
	{
		const Mat4f& a=*this;
		for (int i=0;i<16;i++) 
			if (a.mat[i]!=b.mat[i]) return false;
		return true;
	}

	//! transpose a matrix
	/*! 
		@py 
		assert Mat4f(range(0,16)).transpose()==Mat4f([0,4,8,12,1,5,9,13,2,6,10,14,3,7,11,15])
		@endpy
	*/
	inline Mat4f transpose() const
	{
		float _mat[16]={
			mat[0],mat[4],mat[ 8],mat[12],
			mat[1],mat[5],mat[ 9],mat[13],
			mat[2],mat[6],mat[10],mat[14],
			mat[3],mat[7],mat[11],mat[15]
		};

		return Mat4f(_mat);
	}

	//! build a zero matrix
	/*! 
		@py 
		assert Mat4f.zero()==Mat4f([0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0])
		@endpy
	*/
	inline static Mat4f zero()
	{
		float _mat[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		return Mat4f(_mat);
	}

	//!  calculate the determinant (internal use only)
	static inline float determinant(const float* mat)
	{
		return 
			mat[3] * mat[6] * mat[9]  * mat[12]-mat[2] * mat[7] * mat[9]  * mat[12]-mat[3] * mat[5] * mat[10] * mat[12]+mat[1] * mat[7]    * mat[10] * mat[12]+
			mat[2] * mat[5] * mat[11] * mat[12]-mat[1] * mat[6] * mat[11] * mat[12]-mat[3] * mat[6] * mat[8]  * mat[13]+mat[2] * mat[7]    * mat[8]  * mat[13]+
			mat[3] * mat[4] * mat[10] * mat[13]-mat[0] * mat[7] * mat[10] * mat[13]-mat[2] * mat[4] * mat[11] * mat[13]+mat[0] * mat[6]    * mat[11] * mat[13]+
			mat[3] * mat[5] * mat[8]  * mat[14]-mat[1] * mat[7] * mat[8]  * mat[14]-mat[3] * mat[4] * mat[9]  * mat[14]+mat[0] * mat[7]    * mat[9]  * mat[14]+
			mat[1] * mat[4] * mat[11] * mat[14]-mat[0] * mat[5] * mat[11] * mat[14]-mat[2] * mat[5] * mat[8]  * mat[15]+mat[1] * mat[6]    * mat[8]  * mat[15]+
			mat[2] * mat[4] * mat[9]  * mat[15]-mat[0] * mat[6] * mat[9]  * mat[15]-mat[1] * mat[4] * mat[10] * mat[15]+mat[0] * mat[5]    * mat[10] * mat[15];
	}

	//! calculta the determinant
	/*! 
		@py 
		assert Mat4f().determinant()==1
		@endpy
	*/
	inline float determinant() const
	{
		return determinant(this->mat);
	}

	//! invert matrix
	/*! 
		@py 
		assert Mat4f().invert()==Mat4f()
		@endpy
	*/
	Mat4f invert() const;

	//! scale matrix
	/*! 
		@py 
		m=Mat4f.scale(1,2,3)
		assert m[0]==1 and m[5]==2 and m[10]==3
		@endpy
	*/
	inline static Mat4f scale(float sx,float sy,float sz)
	{
		float _mat[16]={
			sx, 0, 0, 0,
			0 ,sy, 0, 0,
			0 , 0,sz, 0,
			0 , 0, 0, 1
		};

		return Mat4f(_mat);
	}

	//!scale matrix
	/*! 
		@py 
		m=Mat4f.scale(Vec3f(1,2,3))
		assert m[0]==1 and m[5]==2 and m[10]==3
		@endpy
	*/
	inline static Mat4f scale(Vec3f s)
		{return scale(s.x,s.y,s.z);}

	//! translate matrix
	/*! 
		@py 
		m=Mat4f.translate(1,2,3)
		assert m[3]==1 and m[7]==2 and m[11]==3
		@endpy
	*/
	inline static Mat4f translate(float tx,float ty,float tz)
	{
		float _mat[16]={
			1,0,0,tx,
			0,1,0,ty,
			0,0,1,tz,
			0,0,0,1
		};

		return Mat4f(_mat);
	}

	//! translate matrix
	/*! 
	
		@py 
		m=Mat4f.translate(Vec3f(1,2,3))
		assert m[3]==1 and m[7]==2 and m[11]==3
		@endpy
	*/
	inline static Mat4f translate(Vec3f t)
		{return translate(t.x,t.y,t.z);}

	//! rotation from a generic axis
	/*! 
		@py 
		assert (Mat4f.rotate(Vec3f(1,0,0),-pi/4) * Mat4f.rotate(Vec3f(1,0,0),+pi/4)).almostIdentity(1e-4)
		@endpy
	*/
	static Mat4f rotate(Vec3f axis,float angle);

	//! rotation along x
	/*! 
		@py 
		assert (Mat4f.rotatex(-pi/4) * Mat4f.rotatex(+pi/4)).almostIdentity(1e-4)
		@endpy
	*/
	inline static Mat4f rotatex(float angle)
	{
		float c=(float)cos(angle);
		float s=(float)sin(angle);
	
		float _mat[16]={
			1, 0, 0,0,
			0,+c,-s,0,
			0,+s,+c,0,
			0, 0, 0,1
		};

		return Mat4f(_mat);
	}

	//! rotation along y
	inline static Mat4f rotatey(float angle)
	{
		float c=(float)cos(angle);
		float s=(float)sin(angle);

		float _mat[16]={
			+c, 0, +s,0,
			 0, 1,  0,0,
			-s, 0, +c,0,
			 0, 0,  0,1
		};

		return Mat4f(_mat);
	}
	
	//! rotation along Z
	inline static Mat4f rotatez(float angle)
	{
		float c=(float)cos(angle);
		float s=(float)sin(angle);

		float _mat[16]={
			+c,-s,0,0,
			+s,+c,0,0,
			 0, 0,1,0,
			 0, 0,0,1
		};

		return Mat4f(_mat);
	}

	//! OpenGL ortho matrix
	/*! 
		@py 
		m=Mat4f.ortho(-1,+1,-1,+1,0.1,100)
		@endpy
	*/
	static Mat4f ortho(float left,float right,float bottom,float top,float zNear,float zFar);

	//! OpenGL perspective matrix
	/*! 
		@py 
		m=Mat4f.perspective(60,1,0.1,100)
		@endpy
	*/
	static Mat4f perspective(float fovy,float aspect,float zNear,float zFar);

	//! OpenGL frustum matrix
	/*! 
		@py 
		m=Mat4f.frustum(-1,+1,-1,+1,0.1,100)
		@endpy
	*/
	static Mat4f frustum(float left,float right,float bottom,float top,float zNear,float zFar);

	//! OpenGL lookat matrix
	/*! 
		@py 
		m=Mat4f.lookat(1,0,0,0,0,0,0,0,1)
		@endpy
	*/
	static Mat4f lookat(float eyex, float eyey, float eyez, float centerx,float centery, float centerz, float upx, float upy,float upz);
	
	//! fuzzy equality
	/*! 
		@py 
		m=Mat4f()
		assert m.fuzzyEqual(Mat4f(),0)
		@endpy
	*/
	inline bool fuzzyEqual(const Mat4f& src,float epsilon=0.001f) const
	{
		for (int i=0;i<16;++i)
			{if (fabs(this->mat[i]-src.mat[i])>epsilon) return false;}
		return true;
	}

	//! check if a matrix is almost zero
	/*! 
		@py 
		m=Mat4f()
		assert (m-m).almostZero(0)
		@endpy
	*/
	inline bool almostZero(float epsilon=0.001f)
		{return fuzzyEqual(zero(),epsilon);}

	//! check if a matrix is almost identity
	/*! 
		@py 
		m=Mat4f()
		assert m.almostIdentity(0)
		@endpy
	*/
	inline bool almostIdentity(float epsilon=0.001f)
		{return fuzzyEqual(Mat4f(),epsilon);}

	

	//! get the given row
	/*! 
		@py 
		m=Mat4f()
		assert m.row(0)==Vec4f(1,0,0,0) and m.row(1)==Vec4f(0,1,0,0) and m.row(2)==Vec4f(0,0,1,0) and m.row(3)==Vec4f(0,0,0,1)
		@endpy
	*/
	inline Vec4f row(int n) const
	{
		DebugAssert(n>=0 && n<4);
		     if (n==0) return Vec4f(mat[ 0],mat[ 1],mat[ 2],mat[ 3]);
		else if (n==1) return Vec4f(mat[ 4],mat[ 5],mat[ 6],mat[ 7]);
		else if (n==2) return Vec4f(mat[ 8],mat[ 9],mat[10],mat[11]);
		else           return Vec4f(mat[12],mat[13],mat[14],mat[15]);
	}

	//! get the given col
	/*! 
		@py 
		m=Mat4f()
		assert m.col(0)==Vec4f(1,0,0,0) and m.col(1)==Vec4f(0,1,0,0) and m.col(2)==Vec4f(0,0,1,0) and m.col(3)==Vec4f(0,0,0,1)
		@endpy
	*/
	inline Vec4f col(int n) const
	{
		DebugAssert(n>=0 && n<4);
		     if (n==0) return Vec4f(mat[ 0],mat[ 4],mat[ 8],mat[12]);
		else if (n==1) return Vec4f(mat[ 1],mat[ 5],mat[ 9],mat[13]);
		else if (n==2) return Vec4f(mat[ 2],mat[ 6],mat[10],mat[14]);
		else           return Vec4f(mat[ 3],mat[ 7],mat[11],mat[15]);
		
	}

	//!  operator Mat4f + Mat4f
	/*! 
		@py 
		m=Mat4f.zero()+Mat4f()
		assert m==Mat4f()
		@endpy
	*/
	inline Mat4f operator+(const Mat4f& b) const
	{
		const Mat4f& a=*this;
		const float* amat=a.mat;
		const float* bmat=b.mat;

		float _mat[16]={
			 amat[ 0]+bmat[ 0],amat[ 1]+bmat[ 1],amat[ 2]+bmat[ 2],amat[ 3]+bmat[ 3],
			 amat[ 4]+bmat[ 4],amat[ 5]+bmat[ 5],amat[ 6]+bmat[ 6],amat[ 7]+bmat[ 7],
			 amat[ 8]+bmat[ 8],amat[ 9]+bmat[ 9],amat[10]+bmat[10],amat[11]+bmat[11],
			 amat[12]+bmat[12],amat[13]+bmat[13],amat[14]+bmat[14],amat[15]+bmat[15]
		};

		return Mat4f(_mat);
	}

	//!  operator Mat4f - Mat4f
	/*! 
		@py 
		m=Mat4f()-Mat4f()
		assert m==Mat4f.zero()
		@endpy
	*/
	inline Mat4f operator-(const Mat4f& b) const
	{
		const Mat4f& a=(*this);
		const float* amat=a.mat;
		const float* bmat=b.mat;

		float _mat[16]={
			 amat[ 0]-bmat[ 0],amat[ 1]-bmat[ 1],amat[ 2]-bmat[ 2],amat[ 3]-bmat[ 3],
			 amat[ 4]-bmat[ 4],amat[ 5]-bmat[ 5],amat[ 6]-bmat[ 6],amat[ 7]-bmat[ 7],
			 amat[ 8]-bmat[ 8],amat[ 9]-bmat[ 9],amat[10]-bmat[10],amat[11]-bmat[11],
			 amat[12]-bmat[12],amat[13]-bmat[13],amat[14]-bmat[14],amat[15]-bmat[15]
		};

		return Mat4f(_mat);
	}

	//!  operator Mat4f * Mat4f
	/*! 
		@py 
		m=Mat4f()*Mat4f()
		assert m==Mat4f()
		@endpy
	*/
	inline Mat4f operator*(const Mat4f& b) const
	{
		const Mat4f& a=*this;
		const float* amat=a.mat;
		const float* bmat=b.mat;

		float _mat[16]={
			amat[ 0]*bmat[ 0]+amat[ 1]*bmat[ 4]+amat[ 2]*bmat[ 8]+amat[ 3]*bmat[12],
			amat[ 0]*bmat[ 1]+amat[ 1]*bmat[ 5]+amat[ 2]*bmat[ 9]+amat[ 3]*bmat[13],
			amat[ 0]*bmat[ 2]+amat[ 1]*bmat[ 6]+amat[ 2]*bmat[10]+amat[ 3]*bmat[14],
			amat[ 0]*bmat[ 3]+amat[ 1]*bmat[ 7]+amat[ 2]*bmat[11]+amat[ 3]*bmat[15], 
			amat[ 4]*bmat[ 0]+amat[ 5]*bmat[ 4]+amat[ 6]*bmat[ 8]+amat[ 7]*bmat[12], 
			amat[ 4]*bmat[ 1]+amat[ 5]*bmat[ 5]+amat[ 6]*bmat[ 9]+amat[ 7]*bmat[13], 
			amat[ 4]*bmat[ 2]+amat[ 5]*bmat[ 6]+amat[ 6]*bmat[10]+amat[ 7]*bmat[14], 
			amat[ 4]*bmat[ 3]+amat[ 5]*bmat[ 7]+amat[ 6]*bmat[11]+amat[ 7]*bmat[15], 
			amat[ 8]*bmat[ 0]+amat[ 9]*bmat[ 4]+amat[10]*bmat[ 8]+amat[11]*bmat[12], 
			amat[ 8]*bmat[ 1]+amat[ 9]*bmat[ 5]+amat[10]*bmat[ 9]+amat[11]*bmat[13], 
			amat[ 8]*bmat[ 2]+amat[ 9]*bmat[ 6]+amat[10]*bmat[10]+amat[11]*bmat[14], 
			amat[ 8]*bmat[ 3]+amat[ 9]*bmat[ 7]+amat[10]*bmat[11]+amat[11]*bmat[15], 
			amat[12]*bmat[ 0]+amat[13]*bmat[ 4]+amat[14]*bmat[ 8]+amat[15]*bmat[12], 
			amat[12]*bmat[ 1]+amat[13]*bmat[ 5]+amat[14]*bmat[ 9]+amat[15]*bmat[13], 
			amat[12]*bmat[ 2]+amat[13]*bmat[ 6]+amat[14]*bmat[10]+amat[15]*bmat[14], 
			amat[12]*bmat[ 3]+amat[13]*bmat[ 7]+amat[14]*bmat[11]+amat[15]*bmat[15]
		};

		return Mat4f(_mat);
	}

	//! friend operator coeff * matrix
#ifndef SWIG
	friend inline Mat4f operator*(const float c,const Mat4f& m)
	{
		const float* mmat=m.mat;

		float _mat[16]={
				c*mmat[ 0],c*mmat[ 1],c*mmat[ 2],c*mmat[ 3],
				c*mmat[ 4],c*mmat[ 5],c*mmat[ 6],c*mmat[ 7],
				c*mmat[ 8],c*mmat[ 9],c*mmat[10],c*mmat[11],
				c*mmat[12],c*mmat[13],c*mmat[14],c*mmat[15]
		};

		return Mat4f(_mat);
	}
#endif

	//! operator Mat4f * coeff
	/*! 
		@py 
		m=Mat4f()*2
		assert m==Mat4f([2,0,0,0,0,2,0,0,0,0,2,0,0,0,0,2])
		@endpy
	*/
	inline Mat4f operator*(const float c) const
	{
		const Mat4f& m=*this;
		return c*m;
	}

	//!  operactor Mat4f * Vec4f
	/*! 
		@py 
		m=Mat4f()
		assert m*Vec4f(1,0,0,0)==m.col(0) and m*Vec4f(0,1,0,0)==m.col(1) and m*Vec4f(0,0,1,0)==m.col(2) and m*Vec4f(0,0,0,1)==m.col(3)  
		@endpy
	*/
	inline Vec4f operator*(const Vec4f& v) const
	{
		const Mat4f& m=*this;
		const float* mmat=m.mat;
		float X=(mmat[ 0]*v.x+mmat[ 1]*v.y+mmat[ 2]*v.z+mmat[ 3]*v.w);
		float Y=(mmat[ 4]*v.x+mmat[ 5]*v.y+mmat[ 6]*v.z+mmat[ 7]*v.w);
		float Z=(mmat[ 8]*v.x+mmat[ 9]*v.y+mmat[10]*v.z+mmat[11]*v.w);
		float W=(mmat[12]*v.x+mmat[13]*v.y+mmat[14]*v.z+mmat[15]*v.w);
		return Vec4f(X,Y,Z,W);
	}

	//!  operator Mat4f * Vec3f
	/*! 
		@py 
		m=Mat4f()
		assert m*Vec3f(1,0,0)==Vec3f(1,0,0) and m*Vec3f(0,1,0)==Vec3f(0,1,0) and m*Vec3f(0,0,1)==Vec3f(0,0,1)
		@endpy
	*/
	inline Vec3f operator*(const Vec3f& v) const
	{
		const Mat4f& m=*this;
		const float* mmat=m.mat;
		float X=(mmat[ 0]*v.x+mmat[ 1]*v.y+mmat[ 2]*v.z+mmat[ 3]);
		float Y=(mmat[ 4]*v.x+mmat[ 5]*v.y+mmat[ 6]*v.z+mmat[ 7]);
		float Z=(mmat[ 8]*v.x+mmat[ 9]*v.y+mmat[10]*v.z+mmat[11]);
		float W=(mmat[12]*v.x+mmat[13]*v.y+mmat[14]*v.z+mmat[15]);
		DebugAssert(W);
		return Vec3f(X/W,Y/W,Z/W);
	}


	//! decompose the matrix (if possible) in translation, rotation and scale
	/*! 
		\return true if decomposed, false if failed

		@py 
		m=Mat4f()
		t,r,s=Vec3f(),Vec3f(),Vec3f()
		m.decompose(t,r,s)
		assert t.fuzzyEqual(Vec3f(0,0,0)) and s.fuzzyEqual(Vec3f(1,1,1)) and r.fuzzyEqual(Vec3f(0,0,0))
		@endpy
	*/
	bool decompose(Vec3f &trans, Vec3f &rot, Vec3f &scale) const
	{
		//since the original version is from the transposed matrix...
		Mat4f c=this->transpose();

		// Getting translation is trivial
		trans = Vec3f( c(3,0), c(3,1), c(3,2) );

		// Scale is length of columns
		scale.x = sqrt( c(0,0) * c(0,0) + c(0,1) * c(0,1) + c(0,2) * c(0,2) );
		scale.y = sqrt( c(1,0) * c(1,0) + c(1,1) * c(1,1) + c(1,2) * c(1,2) );
		scale.z = sqrt( c(2,0) * c(2,0) + c(2,1) * c(2,1) + c(2,2) * c(2,2) );

		if( scale.x == 0 || scale.y == 0 || scale.z == 0 ) 
			return false;

		// Detect negative scale with determinant and flip one arbitrary axis
		if( determinant() < 0 ) 
			scale.x = -scale.x;

		// Combined rotation matrix YXZ
		//
		// Cos[y]*Cos[z]+Sin[x]*Sin[y]*Sin[z]	Cos[z]*Sin[x]*Sin[y]-Cos[y]*Sin[z]	Cos[x]*Sin[y]	
		// Cos[x]*Sin[z]						Cos[x]*Cos[z]						-Sin[x]
		// -Cos[z]*Sin[y]+Cos[y]*Sin[x]*Sin[z]	Cos[y]*Cos[z]*Sin[x]+Sin[y]*Sin[z]	Cos[x]*Cos[y]

		rot.x = asinf( -c(2,1) / scale.z );
		
		// Special case: Cos[x] == 0 (when Sin[x] is +/-1)
		float f = fabsf( c(2,1) / scale.z );
		if( f > 0.999f && f < 1.001f )
		{
			// Pin arbitrarily one of y or z to zero
			// Mathematical equivalent of gimbal lock
			rot.y = 0;
			
			// Now: Cos[x] = 0, Sin[x] = +/-1, Cos[y] = 1, Sin[y] = 0
			// => m[0,0] = Cos[z] and m[1,0] = Sin[z]
			rot.z = atan2f( -c(1,0) / scale.y, c(0,0) / scale.x );
		}
		// Standard case
		else
		{
			rot.y = atan2f( c(2,0) / scale.z, c(2,2) / scale.z );
			rot.z = atan2f( c(0,1) / scale.x, c(1,1) / scale.y );
		}

		return true;
	}


	//! return the matrix which project points to lower dimension
	//! NOTE: points must be a vector of (x,y,z,x,y,z,...)
	static Mat4f getProjectionMatrix(int npoints,const float* points);

	static Mat4f getProjectionMatrix(const std::vector<Vec3f>& points);

	//!python str
	/*! 
		@py 
		assert eval(str(Mat4f()))==[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]
		@endpy
	*/
	inline std::string str() const
	{
		return Utils::Format("[%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]",
			mat[ 0],mat[ 1],mat[ 2],mat[ 3],
			mat[ 4],mat[ 5],mat[ 6],mat[ 7],
			mat[ 8],mat[ 9],mat[10],mat[11],
			mat[12],mat[13],mat[14],mat[15]);
	}

	//! python repr
	/*! 
		@py 
		assert eval(repr(Mat4f()))==Mat4f()
		@endpy
	*/
	inline std::string repr() const
	{
		return Utils::Format("Mat4f([%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e])",
			mat[ 0],mat[ 1],mat[ 2],mat[ 3],
			mat[ 4],mat[ 5],mat[ 6],mat[ 7],
			mat[ 8],mat[ 9],mat[10],mat[11],
			mat[12],mat[13],mat[14],mat[15]);
	}


	//self test
	static int SelfTest();

}; //end Mat4f class



//=========================================================
//! matrix in n-dim
/*!
	the homogeneous term is in first row/col
*/
//=========================================================
class XGE_API Matf
{
public:
	void Write(Archive& ar)
	{
		ar.WriteVectorFloat("c_vector",this->c_vector());
	}

	void Read(Archive& ar)
	{
		(*this)=ar.ReadVectorFloat("c_vector");
	}

public:

	//! location of memory containing the matrix
	float*   mem;

	//! dimension of the matrix
	int      dim;

	//!default constructor
	/*! 
		@py 
		m=Matf()
		assert m.dim==0 and m(0,0)==1
		@endpy
	*/
	inline Matf()
	{
		this->dim=0;
		mem=(float*)MemPool::getSingleton()->malloc(1* sizeof(float));
		DebugAssert(mem);
		mem[0]=1.0f;
	}


	//! copy constructor
	/*! 
		@py 
		m=Matf(Matf(3))
		assert m.dim==3 and m(0,0)==1 and m(1,1)==1 and m(2,2)==1 and m(3,3)==1
		@endpy
	*/
	inline Matf(const Matf& src)
	{
		this->dim=src.dim;
		this->mem=(float*)MemPool::getSingleton()->malloc(sizeof(float)*(dim+1)*(dim+1));
		DebugAssert(mem);
		memcpy(this->mem,src.mem,sizeof(float)*(dim+1)*(dim+1));
	}



	//! constructor of an identity matrix in dim-dimension
	/*! 
		@py 
		m=Matf(3)
		assert m.dim==3 and m(0,0)==1 and m(1,1)==1 and m(2,2)==1 and m(3,3)==1
		@endpy
	*/
	inline explicit Matf(int dim)
	{
		//setup an identity matrix
		this->dim=dim;
		mem=(float*)MemPool::getSingleton()->calloc((dim+1)*(dim+1),sizeof(float));
		DebugAssert(mem);
		for (int i=0;i<=dim;i++) 
			this->set(i,i,1.0f);
	}

	//!constructor from C array of floats
	inline explicit Matf(int dim,const float* src)
	{
		this->dim=dim;
		this->mem=(float*)MemPool::getSingleton()->malloc(sizeof(float)*(dim+1)*(dim+1));
		DebugAssert(mem);
		memcpy(this->mem,src,sizeof(float)*(dim+1)*(dim+1));
	}

	//! constructor from C array of double
	inline explicit Matf(int dim,const double* src)
	{
		this->dim=dim;
		this->mem=(float*)MemPool::getSingleton()->malloc(sizeof(float)*(dim+1)*(dim+1));
		DebugAssert(mem);
		for (int i=0;i<(dim+1)*(dim+1);i++) 
			this->mem[i]=(float)src[i];
	}

	//! constructor for 1D matrix (so it is 2x2 matrix, considering the homo component)
	/*! 
		@py 
		m=Matf(0,1,2,3)
		assert m(0,0)==0 and m(0,1)==1 and m(1,0)==2 and m(1,1)==3
		@endpy
	*/
	inline explicit Matf(float a00,float a01,float a10,float a11)
	{
		this->dim=1;
		this->mem=(float*)MemPool::getSingleton()->malloc(sizeof(float)*(dim+1)*(dim+1));
		DebugAssert(mem);
		this->mem[0]=a00;this->mem[1]=a01;
		this->mem[2]=a10;this->mem[3]=a11;
	}

	//! constructor for 2D (so it will be a 3x3 matrix, considering the homo component)
	/*! 
		@py 
		m=Matf(0,1,2,3,4,5,6,7,8)
		assert m(0,0)==0 and m(0,1)==1 and m(0,2)==2 and m(1,0)==3 and m(1,1)==4 and m(1,2)==5 and m(2,0)==6 and m(2,1)==7 and m(2,2)==8
		@endpy
	*/
	inline explicit Matf(float a00,float a01,float a02,float a10,float a11,float a12,float a20,float a21,float a22)
	{
		this->dim=2;
		this->mem=(float*)MemPool::getSingleton()->malloc(sizeof(float)*(dim+1)*(dim+1));
		DebugAssert(mem);
		this->mem[0]=a00;this->mem[1]=a01;this->mem[2]=a02;
		this->mem[3]=a10;this->mem[4]=a11;this->mem[5]=a12;
		this->mem[6]=a20;this->mem[7]=a21;this->mem[8]=a22;
	}

	//! constructor from STL std::vector<float>
	/*! 
		@py 
		m=Matf(range(0,9))
		assert m(0,0)==0 and m(0,1)==1 and m(0,2)==2 and m(1,0)==3 and m(1,1)==4 and m(1,2)==5 and m(2,0)==6 and m(2,1)==7 and m(2,2)==8
		@endpy
	*/
	inline explicit Matf(const std::vector<float>& src)
	{
		int _len=(int)sqrt((float)src.size());

		if (_len*_len!=src.size())
			Utils::Error(HERE,"Matf(const std::vector<float>& src) invalid argument src, src.size()=%d but it's size must be in [1,4,9,16,..]",(int)src.size());

		this->dim=(int)(sqrt((float)src.size())-1);

		if (this->dim<0)
			Utils::Error(HERE,"Matf(const std::vector<float>& src) invalid argument src, src.size()=%d but it's size must be>0",(int)src.size());

		this->mem=(float*)MemPool::getSingleton()->malloc(sizeof(float)*(dim+1)*(dim+1));
		DebugAssert(mem);
		memcpy(this->mem,&src[0],sizeof(float)*(dim+1)*(dim+1));
	}


	//! constructor from another Mat4f
	/*!
		@py
		m1=Matf()
		m2=Matf(3)
		m1.assign(m2)
		assert m1.dim==3 and m1.almostIdentity(0)
		@endpy
	*/
	inline explicit Matf(Mat4f& src)
	{
		this->dim=3;
		this->mem=(float*)MemPool::getSingleton()->malloc(sizeof(float)*(dim+1)*(dim+1));
		DebugAssert(mem);

		this->mem[ 0]=src.a44();this->mem[ 1]=src.a41();this->mem[ 2]=src.a42();this->mem[ 3]=src.a43();
		this->mem[ 4]=src.a14();this->mem[ 5]=src.a11();this->mem[ 6]=src.a12();this->mem[ 7]=src.a13();
		this->mem[ 8]=src.a24();this->mem[ 9]=src.a21();this->mem[10]=src.a22();this->mem[11]=src.a23();
		this->mem[12]=src.a34();this->mem[13]=src.a31();this->mem[14]=src.a32();this->mem[15]=src.a33();
	}

	//! destructor
	inline ~Matf()
	{
		MemPool::getSingleton()->free(sizeof(float)*(dim+1)*(dim+1),this->mem);
	}

	//! get all elements as std::vector
	/*!
		@py
		list=Matf(3).c_vector()
		assert list==[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]
		@endpy
	*/
	std::vector<float> c_vector()
	{
		std::vector<float> ret;
		int tot=(dim+1)*(dim+1);
		for (int i=0;i<tot;i++) ret.push_back(get(i));
		return ret;
	}

	inline float get(int i) const
	{
		if (i<0 || i>=(dim+1)*(dim+1))
			Utils::Error(HERE,"const float& Matf::operator[](int i) const invalid argument i, i=%d but it must be in the range [0,%d)",i,(dim+1)*(dim+1));

		return this->mem[i];
	}

	inline void set(int i,float value)
	{
		if (i<0 || i>=(dim+1)*(dim+1))
			Utils::Error(HERE,"const float& Matf::operator[](int i) const invalid argument i, i=%d but it must be in the range [0,%d)",i,(dim+1)*(dim+1));

		this->mem[i]=value;
	}

	//! access operator  by () (stating index is 0)
	/*! 
		@py 
		m=Matf([1,0,0,1])
		assert m.get(0,0)==1 and m.get(0,1)==0 and m.get(1,0)==0 and m.get(1,1)==1
		@endpy
	*/
	inline float get(int r,int c) const
	{

		if (!(r>=0 && r<=dim && c>=0 && c<=dim))
			Utils::Error(HERE,"float Matf::get(int r,int c) indices out of range");

		return this->mem[r*(dim+1)+c];
	}

	//! access operator () (stating index is 0)
	/*! 
		@py 
		m=Matf([1,0,0,1])
		m.set(0,0,1.0)
		@endpy
	*/
	inline void set(int r,int c,float value) 
	{
		if (!(r>=0 && r<=dim && c>=0 && c<=dim))
			Utils::Error(HERE,"void Matf::set(int r,int c,float value) indices out of range");

		this->mem[r*(dim+1)+c]=value;
	}


	//! access operator () (stating index is 0)
	/*! 
		@py 
		m=Matf([1,0,0,1])
		assert m(0,0)==1 and m(0,1)==0 and m(1,0)==0 and m(1,1)==1
		@endpy
	*/
	inline float operator()(int r,int c) const
	{
		return this->get(r,c);
	}


	//! access operator by [] (stating index is 0)
	/*! 
		@py 
		m=Matf([1,0,0,1])
		assert m[0]==1 and m[1]==0 and m[2]==0 and m[3]==1
		@endpy
	*/
	inline float& operator[](int i)
	{
		if (i<0 || i>=(dim+1)*(dim+1))
			Utils::Error(HERE,"float& Matf::operator[](int i) invalid argument i, i=%d but it must be in the range [0,%d)",i,(dim+1)*(dim+1));

		return this->mem[i];
	}

	inline const float& operator[](int i) const
	{
		if (i<0 || i>=(dim+1)*(dim+1))
			Utils::Error(HERE,"const float& Matf::operator[](int i) const invalid argument i, i=%d but it must be in the range [0,%d)",i,(dim+1)*(dim+1));

		return this->mem[i];
	}

	


	//!get a specific row
	/*! 
		@py 
		m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
		assert m.row(0)==Vecf(1,0,0,0)
		@endpy
	*/
	inline Vecf row(int r) const
	{
		if (!(r>=0 && r<=dim))
			Utils::Error(HERE,"Vecf Matf::row(int r) index %d out of valid range [%d,%d]",r,0,dim);


		Vecf ret(this->dim);
		for (int c=0;c<=dim;c++) 
			ret.mem[c]=this->get(r,c);
		return ret;
	}

	//! get a specific column
	/*! 
		@py 
		m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
		assert m.col(0)==Vecf(1,0,0,0)
		@endpy
	*/
	inline Vecf col(int c) const
	{
		DebugAssert(c>=0 && c<=dim);
		Vecf ret(this->dim);
		for (int r=0;r<=dim;r++) 
			ret.mem[r]=(*this)(r,c);
		return ret;
	}

	//! swap rows
	inline Matf swapRows(const std::vector<int>& perm) const
	{
		int n=(int)(perm.size()-1);
		DebugAssert(n==dim);
		Matf ret(this->dim);
		for (int r=0;r<=dim;r++)
		for (int c=0;c<=dim;c++)  
		{
			DebugAssert(perm[r]>=0 && perm[r]<=dim);
			ret.set(r,c,(*this)(perm[r],c));
		}
		return ret;
	}
	
	//! swal columns r1 and r2
	/*! 
		@py 
		m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]).swapRows(0,3)
		assert m.row(0)==Vecf(0,0,0,1)
		@endpy
	*/
	inline Matf swapRows(int r1,int r2) const
	{
		DebugAssert(r1>=0 && r1<=dim && r2>=0 && r2<=dim);

		 //useless call
		if (r1==r2) return *this;
		Matf ret(*this);
		for (int c=0;c<=dim;c++) 
		{
			float temp1=ret.get(r1,c);
			float temp2=ret.get(r2,c);
			ret.set(r1,c,temp2);
			ret.set(r2,c,temp1);
		}
		return ret;
	}

	//! permutate columns
	inline Matf swapCols(const std::vector<int>& perm) const
	{
		int n=(int)(perm.size()-1);
		DebugAssert(n==dim);
		Matf ret(this->dim);
		for (int r=0;r<=dim;r++)
		for (int c=0;c<=dim;c++)
		{
			DebugAssert(perm[c]>=0 && perm[c]<=dim);
			ret.set(r,c,(*this)(r,perm[c]));
		}
		return ret;
	}

	//! permutate columns c1, c2
	/*! 
		@py 
		m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]).swapCols(0,3);
		assert m.col(0)==Vecf(0,0,0,1)
		@endpy
	*/
	inline Matf swapCols(int c1,int c2) const
	{
		DebugAssert(c1>=0 && c1<=dim && c2>=0 && c2<=dim);
		if (c1==c2) return *this;
		Matf ret(*this);
		for (int r=0;r<=dim;r++) 
		{
			float temp1=ret(r,c1);
			float temp2=ret(r,c2);
			ret.set(r,c1,temp2);
			ret.set(r,c2,temp1);
		}
		return ret;
	}

	//! put the homogeneous position in last position (used when you need to convert to Mat4f)
	/*! 
		@py 
		m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]).toMat4f()
		assert m==Mat4f()
		@endpy
	*/
	inline Mat4f toMat4f() const
	{
		Matf temp=this->extract(3);
		//put homogeneous of temp in last position
		std::vector<int> perm;
		perm.push_back(1);
		perm.push_back(2);
		perm.push_back(3);
		perm.push_back(0);
		temp=temp.swapCols(perm).swapRows(perm);
		return Mat4f((float*)temp.mem);
	}

	//! transpose
	/*! 
		@py 
		m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
		assert m.transpose()==m
		@endpy
	*/
	inline Matf transpose() const
	{
		Matf ret(dim);
		for (int i=0;i<=dim;++i)
		for (int j=0;j<=dim;++j)  
			ret.set(i,j,this->get(j,i));
		return ret;
	}

	//! assignment operator
	/*! 
		@py 
		m=Matf()
		m.assign(Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]))
		assert m.dim==3
		@endpy
	*/
	inline Matf& operator=(const Matf& src)
	{
		this->mem=(float*)MemPool::getSingleton()->realloc(sizeof(float)*(dim+1)*(dim+1),this->mem,sizeof(float)*(src.dim+1)*(src.dim+1));
		this->dim=src.dim;
		memcpy(this->mem,src.mem,sizeof(float)*(src.dim+1)*(src.dim+1));
		return *this;
	}

	//! assignment operator using STL std::vector
	/*! 
		@py 
		m=Matf()
		m.assign([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
		assert m.dim==3
		@endpy
	*/
	inline Matf& operator=(const std::vector<float>& src)
	{
		int _len=(int)sqrt((float)src.size());
		if (_len*_len!=src.size())
			Utils::Error(HERE,"Matf& operator=(const std::vector<float>& src) invalid argument src, src.size()=%d but it's size must be [1,4,9,16,...]",(int)src.size());

		int dim=(int)(sqrt((float)src.size())-1);

		if (dim<0)
			Utils::Error(HERE,"Matf& operator=(const std::vector<float>& src) invalid argument src, src.size()=%d but it's size must be>0",(int)src.size());

		this->mem=(float*)MemPool::getSingleton()->realloc(sizeof(float)*(this->dim+1)*(this->dim+1),this->mem,sizeof(float)*(dim+1)*(dim+1));
		this->dim=dim;
		memcpy(this->mem,&src[0],sizeof(float)*(dim+1)*(dim+1));
		return *this;
	}


	//! strict equality operator
	/*! 
		@py 
		m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
		assert m==m and m!=Matf([2,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
		@endpy
	*/
	inline bool operator==(const Matf& src) const
	{
		if(src.dim!=dim) return false;
		return memcmp(this->mem,src.mem,sizeof(float)*(dim+1)*(dim+1))==0;
	}

	//! fuzzy equality
	/*! 
		@py 
		m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
		assert m.fuzzyEqual(m,0)
		@endpy
	*/
	inline bool fuzzyEqual(const Matf& src,float epsilon=0.001f) const
	{
		if(src.dim!=dim) return false;
		for (int i=0;i<=dim;++i)
		for (int j=0;j<=dim;++j) 
		{
			float temp=this->get(i,j)-src.get(i,j);
			if (fabs(temp)>epsilon)  
				return false;
		}
		return true;
	}

	//! get a zero matrix
	/*! 
		@py 
		m=Matf.zero(3)
		assert m==Matf([0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0])
		@endpy
	*/
	inline static Matf zero(int dim)
	{
		Matf ret(dim);
		memset(ret.mem,0,sizeof(float)*(dim+1)*(dim+1));
		return ret;
	}

	//! test if almost zero
	/*! 
		@py 
		m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
		assert (m-m).almostZero(0)
		@endpy
	*/
	inline bool almostZero(float epsilon=0.001f) const
		{return fuzzyEqual(zero(this->dim));}

	//! test if almost identity
	/*! 
		@py 
		m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
		assert m.almostIdentity(0)
		@endpy
	*/
	inline bool almostIdentity(float epsilon=0.001f) const
		{return fuzzyEqual(Matf(this->dim));}

	//! change dimension (can be less or greater than actual dimension)
	/*! 
		@py 
		m=Matf([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
		assert m.extract(2)==Matf([1,0,0,0,1,0,0,0,1])

		m=Matf([1,0,0,1])
		assert m.extract(2)==Matf([1,0,0,0,1,0,0,0,1])
		@endpy
	*/
	inline Matf extract(int dim) const
	{
		//useless call
		if (dim==this->dim) 
			return *this;

		Matf ret(dim); //is an identity matrix at the beginning
		if (dim > this->dim) dim=this->dim;
		for (int r=0;r<=dim;r++)
		for (int c=0;c<=dim;c++) 
			ret.set(r,c,(*this)(r,c));

		return ret;
	}

	//! operator Matf + Matf
	/*! 
		@py 
		assert (Matf(3)+Matf.zero(3))==Matf(3)
		@endpy
	*/
	inline Matf operator+(const Matf& b) const
	{
		const Matf& a=*this;
		DebugAssert(a.dim==b.dim);
		int dim=a.dim;
		Matf ret(dim);
		for (int r=0;r<=dim;r++)
		for (int c=0;c<=dim;c++) 
			ret.set(r,c,a(r,c)+b(r,c));
		return ret;
	}

	//!  operator Matf - Matf
	/*! 
		@py 
		assert (Matf(3)-Matf.zero(3))==Matf(3)
		@endpy
	*/
	inline Matf operator-(const Matf& b) const
	{
		const Matf& a=*this;
		DebugAssert(a.dim==b.dim);
		int dim=a.dim;
		Matf ret(dim);
		for (int r=0;r<=dim;r++)
		for (int c=0;c<=dim;c++) 
			ret.set(r,c,a(r,c)-b(r,c));
		return ret;
	}

	//! operator Matf * Matf
	/*! 
		@py 
		assert (Matf(3)*Matf(3))==Matf(3)
		@endpy
	*/
	inline Matf operator*(const Matf& b) const
	{
		const Matf& a=*this;

		if (a.dim!=b.dim)
			Utils::Error(HERE,"product between matrices with different dimensions");

		int dim=a.dim;
		Matf ret(dim);
		memset(ret.mem,0,sizeof(float)*(dim+1)*(dim+1));

		for (int i=0;i<=dim;++i)
		for (int j=0;j<=dim;++j)
		for (int k=0;k<=dim;++k)
		{
			ret.set(i,j,ret(i,j)+a(i,k)*b(k,j));
		}

		return ret;
	}

	//! scale matrix for vertex transformation
	/*! 
		@py 
		m=Matf.scaleV(Vecf(0,1,2,3))
		assert m(0,0)==1 and m(1,1)==1 and m(2,2)==2 and m(3,3)==3
		@endpy
	*/
	inline static Matf scaleV(Vecf v)
	{
		Matf ret(v.dim);
		DebugAssert(v[0]==0);
		for (int i=1;i<=v.dim;i++) ret.set(i,i,v[i]);
		return ret;

	}

	//! scale matrix for planes transformation
	/*! 
		@py 
		assert (Matf.scaleV(Vecf(0,1,2,3)) * Matf.scaleH(Vecf(0,1,2,3))).almostIdentity(1e-3)
		@endpy
	*/
	inline static Matf scaleH(Vecf v)
	{
		Matf ret(v.dim);
		DebugAssert(v[0]==0);
		for (int i=1;i<=v.dim;i++)  ret.set(i,i,1.0f/v[i]);
		return ret;
	}

	//! translate matrix for vertices transformation
	/*! 
		@py 
		m=Matf.translateV(Vecf(0,1,2,3))
		assert m(0,0)==1 and m(1,0)==1 and m(2,0)==2 and m(3,0)==3
		@endpy
	*/
	inline static Matf translateV(Vecf v)
	{
		Matf ret(v.dim);
		DebugAssert(v[0]==0);
		for (int i=1;i<=v.dim;i++)  ret.set(i,0,v[i]);
		return ret;
	}

	//! translate matrix for planes transformation
	/*! 
		@py 
		assert (Matf.translateV(Vecf(0,1,2,3)) * Matf.translateH(Vecf(0,1,2,3))).almostIdentity(1e-3)
		@endpy
	*/
	inline static Matf translateH(Vecf v)
	{
		Matf ret(v.dim);
		DebugAssert(v[0]==0);
		for (int i=1;i<=v.dim;i++) ret.set(i,0,-v[i]);
		return ret;
	}

	//! rotate matrix for vectors transformation
	// in 3D x==rotateV(2,3,angle)  y=rotate(3,1,angle) z==rotate(1,2,angle) 
	/*! 
		@py 
		assert (Matf.rotateV(3,1,2,pi/4) * Matf.rotateV(3,1,2,-pi/4)).almostIdentity(1e-4)
		@endpy
	*/
	inline static Matf rotateV(int dim,int i,int j,float angle)
	{
		DebugAssert(i>=1 && i<=dim && j>=1 && j<=dim);
		Matf ret(dim);

		//TODO:: probably I should consider sign here? but i break plasm!
		float c=(float)cos(angle);
		float s=(float)sin(angle);
		ret.set(i,i,+c);ret.set(i,j,-s);
		ret.set(j,i,+s);ret.set(j,j,+c);
		return ret;
	}

	//! rotate matrix for planes transformation
	/*! 
		@py 
		assert (Matf.rotateH(3,1,2,pi/4) * Matf.rotateH(3,1,2,-pi/4)).almostIdentity(1e-4)
		@endpy
	*/
	inline static Matf rotateH(int dim,int i,int j,float angle)
	{
		DebugAssert(i>=1 && i<=dim && j>=1 && j<=dim);
		Matf ret(dim);

		//TODO:: probably I should consider sign here? but i break plasm!
		float c=(float)cos(angle);
		float s=(float)sin(angle);
		ret.set(i,i,+c);ret.set(i,j,+s);
		ret.set(j,i,-s);ret.set(j,j,+c);
		return ret;
	}


	//! Matf * coeff
	/*! 
		@py 
		assert (Matf([1,0,0,1]) * 10)==Matf([10,0,0,10])
		@endpy
	*/
	inline Matf operator*(const float coeff)
	{
		Matf ret(this->dim);
		for (int r=0;r<=dim;++r)
		for (int c=0;c<=dim;++c) 
			ret.set(r,c,(*this)(r,c)*coeff);
		return ret;
	}

	//! Matf * vecf
	/*! 
		@py 
		assert (Matf([1,0,0,1]) * Vecf([1,0]))==Vecf([1,0])
		@endpy
	*/
	inline Vecf operator*(const Vecf& v) const
	{
		const Matf& m=*this;
		DebugAssert(m.dim==v.dim);
		int dim=v.dim;
		Vecf ret(dim);
		for (int r=0;r<=dim;r++)
		for (int c=0;c<=dim;c++) 
			ret.mem[r]+=(float)m(r,c)*v[c];
		return ret;
	}



	//! invert matrix
	/*! 
		@py 
		assert Matf(3).invert().fuzzyEqual(Matf(3))
		@endpy
	*/
	Matf invert() const;


	//! return the matrix which project points to lower dimension
	//! NOTE: points must be a vector without! homogeneous coordinates
	static Matf getProjectionMatrix(int pointdim,int npoints,const float* points);

	
	//! python str()
	/*! 
		@py 
		assert eval(str(Matf([1,0,0,1])))==[1,0,0,1]
		@endpy
	*/
	inline std::string str() const
	{
		std::string ret="[";
		for (int i=0;i<=this->dim;i++)
		for (int j=0;j<=this->dim;j++)
		{
			if (i || j) ret+=",";
			ret+=Utils::Format("%f",(*this)(i,j));
		}
		ret+="]";
		return ret;
	}

	//!python repr()
	/*! 
		@py 
		assert eval(repr(Matf([1,0,0,1])))==Matf([1,0,0,1])
		@endpy
	*/
	inline std::string repr() const
	{
		std::string ret="Matf([";
		for (int i=0;i<=this->dim;i++)
		for (int j=0;j<=this->dim;j++)
		{
			if (i || j) ret+=",";
			ret+=Utils::Format("%e",(*this)(i,j));
		}
		ret+="])";
		return ret;
	}


	//self test
	static int SelfTest();

}; //end class


#endif //_MAT_H__

