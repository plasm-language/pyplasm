#include <xge/xge.h>
#include <xge/mat.h>
#include <xge/quaternion.h>






static inline void normalize(float v[3])
{
    float r;

    r = sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
    if (r == 0.0) return;

    v[0] /= r;
    v[1] /= r;
    v[2] /= r;
}

static inline void cross(float v1[3], float v2[3], float result[3])
{
    result[0] = v1[1]*v2[2] - v1[2]*v2[1];
    result[1] = v1[2]*v2[0] - v1[0]*v2[2];
    result[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

static inline void matrix_lookat(float mat[16],float eyex, float eyey, float eyez, float centerx,float centery, float centerz, float upx, float upy,float upz)
{
    float forward[3], side[3], up[3];
    float m[4][4];

    forward[0] = centerx - eyex;
    forward[1] = centery - eyey;
    forward[2] = centerz - eyez;

    up[0] = upx;
    up[1] = upy;
    up[2] = upz;

    normalize(forward);

    /* Side = forward x up */
    cross(forward, up, side);
    normalize(side);

    /* Recompute up as: up = side x forward */
    cross(side, forward, up);

	//make identity
	memset(m,0,sizeof(float)*16);
	m[0][0]=m[1][1]=m[2][2]=m[3][3]=1;

    m[0][0] = side[0];
    m[1][0] = side[1];
    m[2][0] = side[2];

    m[0][1] = up[0];
    m[1][1] = up[1];
    m[2][1] = up[2];

    m[0][2] = -forward[0];
    m[1][2] = -forward[1];
    m[2][2] = -forward[2];

	//glTranslated(-eyex, -eyey, -eyez);
	float *_m = &m[0][0];
	_m[12] = _m[0] * (-eyex) + _m[4] * (-eyey) + _m[8]  * (-eyez) + _m[12];
	_m[13] = _m[1] * (-eyex) + _m[5] * (-eyey) + _m[9]  * (-eyez) + _m[13];
	_m[14] = _m[2] * (-eyex) + _m[6] * (-eyey) + _m[10] * (-eyez) + _m[14];
	_m[15] = _m[3] * (-eyex) + _m[7] * (-eyey) + _m[11] * (-eyez) + _m[15];

	memcpy(mat,m,sizeof(float)*16);
}

//////////////////////////////////////////////////////////////
static inline void matrix_frustum(float m[16],float left, float right,float bottom, float top,float nearval, float farval )
{
	float x, y, a, b, c, d;
	x = (2.0F*nearval) / (right-left);
	y = (2.0F*nearval) / (top-bottom);
	a = (right+left) / (right-left);
	b = (top+bottom) / (top-bottom);
	c = -(farval+nearval) / ( farval-nearval);
	d = -(2.0F*farval*nearval) / (farval-nearval);  /* error? */

	#define M(row,col)  m[col*4+row]
	M(0,0) = x;     M(0,1) = 0.0F;  M(0,2) = a;      M(0,3) = 0.0F;
	M(1,0) = 0.0F;  M(1,1) = y;     M(1,2) = b;      M(1,3) = 0.0F;
	M(2,0) = 0.0F;  M(2,1) = 0.0F;  M(2,2) = c;      M(2,3) = d;
	M(3,0) = 0.0F;  M(3,1) = 0.0F;  M(3,2) = -1.0F;  M(3,3) = 0.0F;
	#undef M
}


//////////////////////////////////////////////////////////////
static inline void matrix_ortho(float m[16],float left, float right,float bottom, float top,float nearval, float farval )
{
	#define M(row,col)  m[col*4+row]
	M(0,0) = 2.0F / (right-left);
	M(0,1) = 0.0F;
	M(0,2) = 0.0F;
	M(0,3) = -(right+left) / (right-left);

	M(1,0) = 0.0F;
	M(1,1) = 2.0F / (top-bottom);
	M(1,2) = 0.0F;
	M(1,3) = -(top+bottom) / (top-bottom);

	M(2,0) = 0.0F;
	M(2,1) = 0.0F;
	M(2,2) = -2.0F / (farval-nearval);
	M(2,3) = -(farval+nearval) / (farval-nearval);

	M(3,0) = 0.0F;
	M(3,1) = 0.0F;
	M(3,2) = 0.0F;
	M(3,3) = 1.0F;
	#undef M
}

//////////////////////////////////////////////////////////////
static inline void matrix_perspective(float mat[16],float fovy, float aspect, float zNear, float zFar)
{
	#define __glPi 3.14159265358979323846
    float m[4][4];
    double sine, cotangent, deltaZ;
    double radians = fovy / 2 * __glPi / 180;

    deltaZ = zFar - zNear;
    sine = sin(radians);
    cotangent = cos(radians) / sine;

	//make identity
	memset(m,0,sizeof(float)*16);
	m[0][0]=m[1][1]=m[2][2]=m[3][3]=1;

    m[0][0] = (float)(cotangent / aspect);
    m[1][1] = (float)(cotangent);
    m[2][2] = (float)(-(zFar + zNear) / deltaZ);
    m[2][3] = (float)(-1);
    m[3][2] = (float)(-2 * zNear * zFar / deltaZ);
    m[3][3] = (float)(0);

	memcpy(mat,m,sizeof(float)*16);
	#undef __glPi
}



//////////////////////////////////////////
Mat4f Mat4f::rotate(Vec3f axis,float angle)
{
	return Quaternion(axis,angle).getMatrix();
}

//////////////////////////////////////////
/*
void Mat4f::print(FILE* file)
{
	LOG(
		"%f %f %f %f \n"
		"%f %f %f %f \n"
		"%f %f %f %f \n"
		"%f %f %f %f \n\n",
		a11(),a12(),a13(),a14(),
		a21(),a22(),a23(),a24(),
		a31(),a32(),a33(),a34(),
		a41(),a42(),a43(),a44()
		);
}
*/

//////////////////////////////////////////
Mat4f Mat4f::invert()  const
{
	float Det=determinant();
	DebugAssert(Det);

	float _mat[16]={
			(mat[6]*mat[11]*mat[13] - mat[7]*mat[10]*mat[13] + mat[7]*mat[9]*mat[14] - mat[5]*mat[11]*mat[14] - mat[6]*mat[9]*mat[15] + mat[5]*mat[10]*mat[15]) / Det ,
			(mat[3]*mat[10]*mat[13] - mat[2]*mat[11]*mat[13] - mat[3]*mat[9]*mat[14] + mat[1]*mat[11]*mat[14] + mat[2]*mat[9]*mat[15] - mat[1]*mat[10]*mat[15]) / Det ,
			(mat[2]*mat[7 ]*mat[13] - mat[3]*mat[ 6]*mat[13] + mat[3]*mat[5]*mat[14] - mat[1]*mat[ 7]*mat[14] - mat[2]*mat[5]*mat[15] + mat[1]*mat[ 6]*mat[15]) / Det ,
			(mat[3]*mat[6 ]*mat[9 ] - mat[2]*mat[ 7]*mat[ 9] - mat[3]*mat[5]*mat[10] + mat[1]*mat[ 7]*mat[10] + mat[2]*mat[5]*mat[11] - mat[1]*mat[ 6]*mat[11]) / Det ,
			(mat[7]*mat[10]*mat[12] - mat[6]*mat[11]*mat[12] - mat[7]*mat[8]*mat[14] + mat[4]*mat[11]*mat[14] + mat[6]*mat[8]*mat[15] - mat[4]*mat[10]*mat[15]) / Det ,
			(mat[2]*mat[11]*mat[12] - mat[3]*mat[10]*mat[12] + mat[3]*mat[8]*mat[14] - mat[0]*mat[11]*mat[14] - mat[2]*mat[8]*mat[15] + mat[0]*mat[10]*mat[15]) / Det ,
			(mat[3]*mat[6 ]*mat[12] - mat[2]*mat[ 7]*mat[12] - mat[3]*mat[4]*mat[14] + mat[0]*mat[ 7]*mat[14] + mat[2]*mat[4]*mat[15] - mat[0]*mat[ 6]*mat[15]) / Det ,
			(mat[2]*mat[7 ]*mat[8 ] - mat[3]*mat[ 6]*mat[ 8] + mat[3]*mat[4]*mat[10] - mat[0]*mat[ 7]*mat[10] - mat[2]*mat[4]*mat[11] + mat[0]*mat[ 6]*mat[11]) / Det ,
			(mat[5]*mat[11]*mat[12] - mat[7]*mat[ 9]*mat[12] + mat[7]*mat[8]*mat[13] - mat[4]*mat[11]*mat[13] - mat[5]*mat[8]*mat[15] + mat[4]*mat[ 9]*mat[15]) / Det ,
			(mat[3]*mat[9 ]*mat[12] - mat[1]*mat[11]*mat[12] - mat[3]*mat[8]*mat[13] + mat[0]*mat[11]*mat[13] + mat[1]*mat[8]*mat[15] - mat[0]*mat[ 9]*mat[15]) / Det ,
			(mat[1]*mat[7 ]*mat[12] - mat[3]*mat[ 5]*mat[12] + mat[3]*mat[4]*mat[13] - mat[0]*mat[ 7]*mat[13] - mat[1]*mat[4]*mat[15] + mat[0]*mat[ 5]*mat[15]) / Det ,
			(mat[3]*mat[5 ]*mat[8 ] - mat[1]*mat[ 7]*mat[ 8] - mat[3]*mat[4]*mat[ 9] + mat[0]*mat[ 7]*mat[ 9] + mat[1]*mat[4]*mat[11] - mat[0]*mat[ 5]*mat[11]) / Det ,
			(mat[6]*mat[9 ]*mat[12] - mat[5]*mat[10]*mat[12] - mat[6]*mat[8]*mat[13] + mat[4]*mat[10]*mat[13] + mat[5]*mat[8]*mat[14] - mat[4]*mat[ 9]*mat[14]) / Det ,
			(mat[1]*mat[10]*mat[12] - mat[2]*mat[ 9]*mat[12] + mat[2]*mat[8]*mat[13] - mat[0]*mat[10]*mat[13] - mat[1]*mat[8]*mat[14] + mat[0]*mat[ 9]*mat[14]) / Det ,
			(mat[2]*mat[5 ]*mat[12] - mat[1]*mat[ 6]*mat[12] - mat[2]*mat[4]*mat[13] + mat[0]*mat[ 6]*mat[13] + mat[1]*mat[4]*mat[14] - mat[0]*mat[ 5]*mat[14]) / Det ,
			(mat[1]*mat[6 ]*mat[8 ] - mat[2]*mat[ 5]*mat[ 8] + mat[2]*mat[4]*mat[ 9] - mat[0]*mat[ 6]*mat[ 9] - mat[1]*mat[4]*mat[10] + mat[0]*mat[ 5]*mat[10]) / Det 
	};

	return Mat4f(_mat);
}


//////////////////////////////////////////
Mat4f Mat4f::ortho(float left,float right,float bottom,float top,float zNear,float zFar)
{
#if 0
	glPushMatrix();
	glLoadIdentity();
	glOrtho(left,right,bottom,top,zNear,zFar);
	float v[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, v);
	glPopMatrix();
	return Mat4f(v).transpose();
#else
	float m[16];
	matrix_ortho(m,left,right,bottom,top,zNear,zFar);
	return Mat4f(m).transpose();
#endif

	
}

//////////////////////////////////////////
Mat4f Mat4f::perspective(float fovy,float aspect,float zNear,float zFar)
{
#if 0
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(fovy,aspect,zNear,zFar);
	float v[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, v);
	glPopMatrix();
	return Mat4f(v).transpose();
#else
	float m[16];
	matrix_perspective(m,fovy,aspect,zNear,zFar);
	return Mat4f(m).transpose();
#endif
}

//////////////////////////////////////////
Mat4f Mat4f::frustum(float left,float right,float bottom,float top,float zNear,float zFar)
{
#if 0
	glPushMatrix();
	glLoadIdentity();
	glFrustum(left,right,bottom,top,zNear,zFar);
	float v[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, v);
	glPopMatrix();
	return Mat4f(v).transpose();
#else
	float m[16];
	matrix_frustum(m,left,right,bottom,top,zNear,zFar);
	return Mat4f(m).transpose();
#endif
}

Mat4f Mat4f::lookat(float eyex, float eyey, float eyez, float centerx,float centery, float centerz, float upx, float upy,float upz)
{
#if 0
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(eyex,eyey,eyez,centerx,centery,centerz,upx,upy,upz);
	float v[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, v);
	glPopMatrix();
	return Mat4f(v).transpose();
#else
	float m[16];
	matrix_lookat(m,eyex,eyey,eyez,centerx,centery,centerz,upx,upy,upz);
	return Mat4f(m).transpose();
#endif
}


/////////////////////////////////////////////////////////////////
Mat4f Mat4f::getProjectionMatrix(int npoints,const float* points)
{
	return Matf::getProjectionMatrix(3,npoints,points).toMat4f();
}



/////////////////////////////////////////////////////////////////
Mat4f Mat4f::getProjectionMatrix(const std::vector<Vec3f>& _points)
{
	int npoints=(int)_points.size();
	std::vector<float> points;
	for (int i=0;i<npoints;i++)
	{
		points.push_back(_points[i].x);
		points.push_back(_points[i].y);
		points.push_back(_points[i].z);
	}

	return Matf::getProjectionMatrix(3,npoints,&points[0]).toMat4f();

}






/////////////////////////////////////////////////////////////////
Matf Matf::getProjectionMatrix(int pointdim,int npoints,const float* points)
{
	Vecf Center(pointdim,1.0f,points);
	return Planef::bestFittingPlane(pointdim,npoints,points).getProjectionMatrix(Center);
}





/////////////////////////////////////////////////////////////////
int Mat4f::SelfTest()
{
	Log::printf("Testing Mat4f...\n");
	Mat4f identity;

	float _m2[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
	Mat4f m2(_m2);

	float _m3[16]={
		Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1),
		Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1),
		Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1),
		Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1)
	};
	Mat4f m3(_m3);

	ReleaseAssert(identity.a11()==1.0f && identity.a22()==1.0f && identity.a33()==1.0f && identity.a44()==1.0f);

	//equality and subtract
	ReleaseAssert((identity-identity)==Mat4f::zero());

	//transpose
	ReleaseAssert(identity.transpose()==identity);
	float _m2t[16]={1,5,9,13,2,6,10,14,3,7,11,15,4,8,12,16};
	ReleaseAssert(m2.transpose()==Mat4f(_m2t));

	//inverse
	ReleaseAssert(identity.invert()==identity);
	ReleaseAssert((m3.invert()*m3).almostIdentity(0.001f));

	//addition/subtraction
	ReleaseAssert((((m3+identity)-identity)-m3).almostZero(0.001f));

	//access
	float _m4[16]={
		m3.a11(),m3.a12(),m3.a13(),m3.a14(),
		m3.a21(),m3.a22(),m3.a23(),m3.a24(),
		m3.a31(),m3.a32(),m3.a33(),m3.a34(),
		m3.a41(),m3.a42(),m3.a43(),m3.a44()
	};

	Mat4f m4(_m4);

	float _m5[16]={
		m3(0,0),m3(0,1),m3(0,2),m3(0,3),
		m3(1,0),m3(1,1),m3(1,2),m3(1,3),
		m3(2,0),m3(2,1),m3(2,2),m3(2,3),
		m3(3,0),m3(3,1),m3(3,2),m3(3,3)
	};

	Mat4f m5(_m5);
	ReleaseAssert(m3==m4 && m3==m5);

	//multiplication
	ReleaseAssert(m3*identity==m3);
	ReleaseAssert((((m3*3.0f)*(1.0f/3.0f))-m3).almostZero(0.001f));


	//multiplication for a vector
	ReleaseAssert(
		   m3*Vec4f(1,0,0,0)==m3.col(0) 
		&& m3*Vec4f(0,1,0,0)==m3.col(1)
		&& m3*Vec4f(0,0,1,0)==m3.col(2)
		&& m3*Vec4f(0,0,0,1)==m3.col(3));

	ReleaseAssert(
		   Vec4f(1,0,0,0)*m3==m3.row(0) 
		&& Vec4f(0,1,0,0)*m3==m3.row(1)
		&& Vec4f(0,0,1,0)*m3==m3.row(2)
		&& Vec4f(0,0,0,1)*m3==m3.row(3));

	//scale and invert
	Mat4f m6=Mat4f::scale(1,2,3);
	float _m6_bis[16]={1/1.0f,0,0,0, 0,1/2.0f,0,0, 0,0,1/3.0f,0, 0,0,0,1};
	ReleaseAssert(((m6 * Mat4f(_m6_bis))).almostIdentity(0.001f));

	//translate and invert
	ReleaseAssert(((Mat4f::translate(-1,-2,-3) * Mat4f::translate(1,2,3))).almostIdentity(0.001f));

	//rotation and multiplication
	Mat4f m7=identity
		* Mat4f::rotatex(-(float)M_PI/4.0f)
		* Mat4f::rotatey(-(float)M_PI/4.0f)
		* Mat4f::rotatez(-(float)M_PI/4.0f)
		* Mat4f::rotatez(+(float)M_PI/4.0f)
		* Mat4f::rotatey(+(float)M_PI/4.0f)
		* Mat4f::rotatex(+(float)M_PI/4.0f);

	ReleaseAssert(m7.almostIdentity(0.001f));

	//decompose
	Vec3f angles((float)M_PI/4.0f,(float)M_PI/5.0f,(float)M_PI/6.0f);

	Mat4f m8=
		identity
		*Mat4f::translate(1,2,3)
		 //YXZ
		*Mat4f::rotatey(angles.y)
		*Mat4f::rotatex(angles.x)
		*Mat4f::rotatez(angles.z)
		*Mat4f::scale(1,2,3);

	Vec3f trans, rot, scale;
	m8.decompose(trans,rot,scale);
	ReleaseAssert(
		(trans - Vec3f(1,2,3)).module()<=0.001f &&
		(rot   - angles              ).module()<=0.001f &&
		(scale - Vec3f(1,2,3)).module()<=0.001f);

	//rotate generic axis

	Mat4f _ma=Mat4f::rotate(Vec3f(1,0,0),+angles.x);
	Mat4f _mb=Mat4f::rotatex(+angles.x);

	ReleaseAssert((Mat4f::rotate(Vec3f(1,0,0),+angles.x) - Mat4f::rotatex(+angles.x)).almostZero(0.001f));
	ReleaseAssert((Mat4f::rotate(Vec3f(0,1,0),+angles.y) - Mat4f::rotatey(+angles.y)).almostZero(0.001f));
	ReleaseAssert((Mat4f::rotate(Vec3f(0,0,1),+angles.z) - Mat4f::rotatez(+angles.z)).almostZero(0.001f));
	ReleaseAssert((Mat4f::rotate(Vec3f(1,0,0),-angles.x) - Mat4f::rotatex(-angles.x)).almostZero(0.001f));
	ReleaseAssert((Mat4f::rotate(Vec3f(0,1,0),-angles.y) - Mat4f::rotatey(-angles.y)).almostZero(0.001f));
	ReleaseAssert((Mat4f::rotate(Vec3f(0,0,1),-angles.z) - Mat4f::rotatez(-angles.z)).almostZero(0.001f));

	return 0;

}



/////////////////////////////////////////////////////////////////
Matf Matf::invert() const
{
	Matf A(*this);
	int Dim=A.dim;
	Vecf S(A.dim);
	Matf B(A.dim);
	Vecf X(A.dim);

	for(int I=0; I<=Dim; I++ ) 
	{
		X[I] = I;
		float scalemax = 0.;
		for(int J=0; J<=Dim; J++ ) scalemax = max2(scalemax,fabs(A(I,J)));
		S[I] = scalemax;
	}

	int signDet = 1;
	for(int  K=0; K<=Dim; K++ ) 
	{
		float ratiomax = 0.0f;
		int jPivot = K;
		for(int I=K; I<=Dim; I++ ) 
		{
			float ratio = fabs(A(X[I],K))/S[X[I]];
			if( ratio > ratiomax ) 
			{
				jPivot=I;
				ratiomax = ratio;
			}
		}
		int indexJ = X[K];
		if(jPivot!=K) 
		{
			indexJ = X[jPivot];
			X[jPivot] = X[K];   
			X[K] = indexJ;
			signDet *= -1;
		}

		for(int I=(K+1); I<=Dim; I++ ) 
		{
			float coeff = A(X[I],K)/A(indexJ,K);
			for(int J=(K+1); J<=Dim; J++ )
			{
				float value=A(X[I],J)-coeff*A(indexJ,J);
				A.set(X[I],J ,value);
			}

			A.set(X[I],K,coeff);
			for(int J=0; J<=Dim; J++ )  
			{
				float value=B(X[I],J) - A(X[I],K)*B(indexJ,J);
				B.set(X[I],J,value);
			}
		}
	}

	//float determ = signDet;
	//for(int I=0; I<=Dim; I++ ) 
	//	determ *= A(X[I],I);

	Matf ret(A);
	for(int K=0; K<=Dim; K++ ) 
	{
		float value=B(X[Dim],K)/A(X[Dim],Dim);
		ret.set(Dim,K, value);
		for(int I=A.dim-1; I>=0; I--) 
		{
			float sum = B(X[I],K);
			for(int J=(I+1);J<=Dim; J++) sum -= A(X[I],J)*ret(J,K);

			value=sum/A(X[I],I);
			ret.set(I,K,value);
		}
	}
	return ret;   
}



/////////////////////////////////////////////////////////////////
int Matf::SelfTest()
{
	Log::printf("Testing Matf...\n");


	Matf M1(0);M1.mem[0]=0.0f;
	ReleaseAssert(M1.dim==0 && M1(0,0)==0.0f);

	Matf M2(0,1,2,3);
	ReleaseAssert(M2.dim==1 && M2(0,0)==0.0f && M2(0,1)==1.0f && M2(1,0)==2.0f && M2(1,1)==3.0f);

	Matf M3(0,1,2,3,4,5,6,7,8);
	ReleaseAssert(M3.dim==2 
		&& M3(0,0)==0.0f && M3(0,1)==1.0f && M3(0,2)==2.0f 
		&& M3(1,0)==3.0f && M3(1,1)==4.0f && M3(1,2)==5.0f 
		&& M3(2,0)==6.0f && M3(2,1)==7.0f && M3(2,2)==8.0f);

	float _M4[16]={0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15};
	Matf M4(3,_M4);
	ReleaseAssert(M4.dim==3
		&& M4(0,0)== 0.0f && M4(0,1)== 1.0f && M4(0,2)== 2.0f && M4(0,3)== 3.0f 
		&& M4(1,0)== 4.0f && M4(1,1)== 5.0f && M4(1,2)== 6.0f && M4(1,3)== 7.0f 
		&& M4(2,0)== 8.0f && M4(2,1)== 9.0f && M4(2,2)==10.0f && M4(2,3)==11.0f 
		&& M4(3,0)==12.0f && M4(3,1)==13.0f && M4(3,2)==14.0f && M4(3,3)==15.0f );


	Matf m2(1);
	ReleaseAssert(m2.dim==1 && m2(0,0)==1 && m2(0,1)==0 && m2(1,0)==0 && m2(1,1)==1);

	Matf m3(2);
	ReleaseAssert(m3.dim==2 && m3(0,0)==1 && m3(0,1)==0 && m3(0,2)==0 && m3(1,0)==0 && m3(1,1)==1 && m3(1,2)==0 && m3(2,0)==0 && m3(2,1)==0 && m3(2,2)==1);
 
	ReleaseAssert(m3.transpose()==m3);

	//transpose
	{
		Matf m(1,2,3,4);
		m=m.transpose();
		ReleaseAssert(m(0,0)==1 && m(0,1)==3 && m(1,0)==2 && m(1,1)==4);
	}

	//assignment
	{
		Matf m(1,2,3,4);
		Matf n(2);
		n=m;
		ReleaseAssert(n(0,0)==1 && n(0,1)==2 && n(1,0)==3 && n(1,1)==4);
	}
 
	//zero matrix
	ReleaseAssert(Matf::zero(3).almostZero(0));

	//extract
	{
		Matf m(1,2,3,4);

		//down size (1->0)
		ReleaseAssert(m.extract(0).dim==0 && m.extract(0)==Matf(0));
		
		//super size (1->2)
		ReleaseAssert(m.extract(2).dim==2 && m.extract(2)==Matf(1,2,0, 3,4,0, 0,0,1));
	}

	//swap rows
	{
		Matf m(1.0,2.0,3.0,4.0);
		ReleaseAssert(m.dim==1);

		ReleaseAssert(m.swapRows(0,1)==Matf(3.0,4.0,1.0,2.0));
		
		std::vector<int> perm;
		perm.push_back(1);
		perm.push_back(0);
		ReleaseAssert(m.swapRows(perm)==Matf(3.0,4.0,1.0,2.0));
	}

	//swap cols
	{
		Matf m(1,2,3,4);
		ReleaseAssert(m.dim==1);

		ReleaseAssert(m.swapCols(0,1)==Matf(2,1,4,3));
		
		std::vector<int> perm;
		perm.push_back(1);
		perm.push_back(0);
		ReleaseAssert(m.swapCols(perm)==Matf(2,1,4,3));
	}

	//plus/minus
	{
		ReleaseAssert(Matf(1,2,3,4)+Matf(10,20,30,40) == Matf(11,22,33,44));
		ReleaseAssert(Matf(1,2,3,4)-Matf(10,20,30,40) == Matf(-9,-18,-27,-36));
	}

	//product
	{
		ReleaseAssert(Matf(1,2,3,4)*Matf(5,6,7,8)==Matf(19,22,43,50));
	}

	//product
	{
		ReleaseAssert((Matf(1,2,3,4)*0.1f).fuzzyEqual(Matf(0.1f,0.2f,0.3f,0.4f)));
	}

	//vector product
	{
		Matf m(1,2,3, 4,5,6, 7,8,9);
		Vecf c0=m.col(0);
		ReleaseAssert(m*Vecf(1.0f,0.0f,0.0f)==c0 && m*Vecf(0.0f,1.0f,0.0f)==m.col(1) && m*Vecf(0.0f,0.0f,1.0f)==m.col(2));
	}

	//plane product
	{
		Matf m(1,2,3, 4,5,6, 7,8,9);
		ReleaseAssert(Vecf(1.0f,0.0f,0.0f)*m==m.row(0) && Vecf(0.0f,1.0f,0.0f)*m==m.row(1) && Vecf(0.0f,0.0f,1.0f)*m==m.row(2));
	}

	//invert
	{
		for (int dim=1;dim<8;dim++)
		{
			for (int ntry=0;ntry<10;ntry++)
			{
				Matf m(dim);
				for (int r=0;r<=dim;r++)
				for (int c=0;c<=dim;c++) 
					m.set(r,c,Utils::FloatRand(0,1));
				ReleaseAssert((m.invert() * m).almostIdentity());
			}
		}
	}

	//scale
	{
		for (int i=0;i<8;i++)
		{
			int dim=3;
			Vecf v(1,Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1));
			Vecf s(0,Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1));
			Matf mv=Matf::scaleV(s);
			Matf mh=Matf::scaleH(s);
			ReleaseAssert(mv.dim==dim && mh.dim==dim);
			ReleaseAssert(mv.invert().fuzzyEqual(mh));
			Vecf t=Vecf(v[0],v[1]*s[1],v[2]*s[2],v[3]*s[3]);
			ReleaseAssert((mv*v).fuzzyEqual(t));
		}
	}
	
	//translate
	{
		for (int i=0;i<8;i++)
		{
			int dim=3;
			Vecf v(1,Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1));
			Vecf s(0,Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1));
			Matf mv=Matf::translateV(s);
			Matf mh=Matf::translateH(s);
			ReleaseAssert(mv.invert().fuzzyEqual(mh));
			Vecf t=Vecf(v[0]+s[0],v[1]+s[1],v[2]+s[2],v[3]+s[3]);
			ReleaseAssert((mv*v).fuzzyEqual(t));
		}
	}

	//rotate
	{
		for (int i=0;i<8;i++)
		{
			int dim=3;
			Vecf v(1,Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1));
			Vecf a(0,Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1));

			Matf mv=Matf::rotateV(dim,1,2,a[1])*Matf::rotateV(dim,1,3,a[2])*Matf::rotateV(dim,2,3,a[3]);
			Matf mh=Matf::rotateH(dim,2,3,a[3])*Matf::rotateH(dim,1,3,a[2])*Matf::rotateH(dim,1,2,a[1]);
			ReleaseAssert(mv.invert().fuzzyEqual(mh));
		}
	}

	//test congruency with mat4f
	{
		//x rotation
		ReleaseAssert(Matf::rotateV(3 ,2,3,(float)M_PI/2).toMat4f().fuzzyEqual(Mat4f::rotatex((float)M_PI/2)));

		//y rotation
		ReleaseAssert(Matf::rotateV(3 ,3,1,(float)M_PI/2).toMat4f().fuzzyEqual(Mat4f::rotatey((float)M_PI/2)));

		//z rotation
		ReleaseAssert(Matf::rotateV(3 ,1,2,(float)M_PI/2).toMat4f().fuzzyEqual(Mat4f::rotatez((float)M_PI/2)));
	}
	
	return 0;
}