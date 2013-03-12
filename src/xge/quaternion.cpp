
#include <xge/xge.h>
#include <xge/quaternion.h>



const float Quaternion::ms_fEpsilon = 1e-03f;
const Quaternion Quaternion::ZERO(0.0f,0.0f,0.0f,0.0f);
const Quaternion Quaternion::IDENTITY(1.0f,0.0f,0.0f,0.0f);


////////////////////////////////////////////////////////////////////////
void Quaternion::fromMatrix(Mat4f& m)
{
	float kRot[3][3]=
	 {
		{m.a11(),m.a12(),m.a13()},
		{m.a21(),m.a22(),m.a23()},
		{m.a31(),m.a32(),m.a33()}
	 };

	 // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
	 // article "Quaternion Calculus and Fast Animation".
 
	 float fTrace = kRot[0][0]+kRot[1][1]+kRot[2][2];
	 float fRoot;
 
	 if ( fTrace > 0.0 )
	 {
		 // |w| > 1/2, may as well choose w > 1/2
		 fRoot = (float)sqrt(fTrace + 1.0);  // 2w
		 w = 0.5f*fRoot;
		 fRoot = 0.5f/fRoot;  // 1/(4w)
		 x = (kRot[2][1]-kRot[1][2])*fRoot;
		 y = (kRot[0][2]-kRot[2][0])*fRoot;
		 z = (kRot[1][0]-kRot[0][1])*fRoot;
	 }
	 else
	 {
		 // |w| <= 1/2
		 static int s_iNext[3] = { 1, 2, 0 };
		 int i = 0;
		 if ( kRot[1][1] > kRot[0][0] )
			 i = 1;
		 if ( kRot[2][2] > kRot[i][i] )
			 i = 2;
		 int j = s_iNext[i];
		 int k = s_iNext[j];
 
		 fRoot = (float)sqrt(kRot[i][i]-kRot[j][j]-kRot[k][k] + 1.0);
		 float* apkQuat[3] = { &x, &y, &z };
		 *apkQuat[i] = 0.5f * fRoot;
		 fRoot = (float)0.5f/fRoot;
		 w = (kRot[k][j]-kRot[j][k])*fRoot;
		 *apkQuat[j] = (kRot[j][i]+kRot[i][j])*fRoot;
		 *apkQuat[k] = (kRot[k][i]+kRot[i][k])*fRoot;
	 }
}

