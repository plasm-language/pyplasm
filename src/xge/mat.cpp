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
	XgeDebugAssert(Det);

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


