#include <xge/xge.h>
#include <xge/plane.h>
#include <xge/ray.h>




/////////////////////////////////////////////////////////
Plane4f Plane4f::bestFittingPlane(int npoints,const float* points)
{
	//simple (and fast) case
	if (npoints==3)
		return Plane4f(Vec3f(points+0),Vec3f(points+3),Vec3f(points+6));

	//use Planef
	Planef h=Planef::bestFittingPlane(3,npoints,points);

	//note the homogeneous coordinate in last position
	return Plane4f(h[1],h[2],h[3],h[0]);
}


/////////////////////////////////////////////////////////
Mat4f Plane4f::getProjectionMatrix(Vec3f Center)
{
	return Planef(this->w,this->x,this->y,this->z).getProjectionMatrix(Vecf(1.0f,Center.x,Center.y,Center.z)).toMat4f();
}






/////////////////////////////////////////////////////////////////
Matf Planef::getProjectionMatrix(Vecf Center)
{
	int Dim=this->dim;
	ReleaseAssert(Center.dim==Dim && Center[0]==1.0f);

	Planef h(*this);
	h.mem[0]=0;

	/*
	S_V_D decomposition to find null space
	REF1: find Null Space of a matrix. 
	ROTATE points so that the vector (0,h[1],h[2],..h[pointdim]) goes to (0,0,...1)
	REF2: change of basis of matrix, transform vector u to v, use 2 reflections in a plane.
	write the matrix:
		M1 = I - 2*u*u' 
		M2 = I - 2*w*w' 
	where:
		w = (u+v)/ Norm(u+v) (bisector of angle in the plane between u and v), 
		I is the n x n identity matrix,
		u_1 and w are column vectors 
		u_1' and w' are row vectors, 
	so u_1*u_1' for example is the outer product that creates and n x n  matrix. 
	The way it works is that matrix M1 reflects u into -u and matrix  M2 reflects -u into v 
	So you multiply R=M2*M1 to get the rotation matrix.  
	The determinant  of M1 and M2 is -1 (because they are reflections) and so the det of 
		the product R is 1, as expected for a rotation
	*/

	int main_direction=h.getMainDirection();
	Vecf v(Dim);
	v.mem[main_direction]=(h[main_direction]>0)?(+1.0f):(-1.0f);
	Vecf w=h+v;
	DebugAssert(w.module());
	w/=w.module();
	Matf Rot1(Dim),Rot2(Dim);
	for (int r=0;r<=Dim;r++)
	for (int c=0;c<=Dim;c++)
	{
		Rot1.set(r,c, ((r==c)?1:0)-2*h[r]*h[c]);
		Rot2.set(r,c, ((r==c)?1:0)-2*w[r]*w[c]);
	}
	//the matrix which will project all points to plane 0
	Matf T =(Rot2*Rot1 * Matf::translateV(-1.0*Vecf(Dim,0,&Center[1])) ).swapRows(main_direction,Dim);

	//problem of keeping the normals in the same direction for OpenGL
	#if 1
	if (Dim==3)
	{
		//this is the normal  would like to have!
		Vec3f N(h[1],h[2],h[3]);
		Mat4f D=T.toMat4f().invert();

		//this is how opengl works
		//see http://books.google.it/books?id=H4eYq7-2YhYC&pg=PA23&lpg=PA23&dq=opengl+normal+transform&source=bl&ots=q9ZGuKZ2S3&sig=_vc67-fLbPl4CZC-oEgX9eUxIsA&hl=it&ei=tv3qSv71KYLwmwOo4KSaDw&sa=X&oi=book_result&ct=result&resnum=4&ved=0CBgQ6AEwAw#v=onepage&q=opengl%20normal%20transform&f=false
		Mat4f Dit=Mat4f(
				D[ 0],D[ 1],D[ 2],0,
				D[ 4],D[ 5],D[ 6],0,
				D[ 8],D[ 9],D[10],0,
					0,    0,    0,1).invert().transpose();

		//how the normal is transformed by opengl
		//(0,0,1) is the default normal which will be assigned in 2d!
		Vec3f n=(Dit * Vec3f(0,0,1)).normalize(); 

		//if goes opposite directions (transformed by D)
		if ((N*n)<0)
		{
			//add a mirror
			Vecf vs(0,1,1,-1);
			T=Matf::scaleV(vs) * T;
		}
	}
	#endif

	return T;
}



//////////////////////////////////////////////////////////////////////////
/** Singular Value Decomposition.
<P>
For an m-by-n matrix A with m >= n, the singular value decomposition is
an m-by-n orthogonal matrix U, an n-by-n diagonal matrix S, and
an n-by-n orthogonal matrix V so that A = U*S*V'.
<P>
The singular values, sigma[k] = S[k][k], are ordered so that
sigma[0] >= sigma[1] >= ... >= sigma[n-1].
<P>
The singular value decompostion always exists, so the constructor will
never fail.  The matrix condition number and the effective numerical
rank can be computed from this decomposition.
*/
//////////////////////////////////////////////////////////////////////////


#define ABS(value) fabs((double)value)

class SVD 
{
public:

	class Array2D 
	{
	public:
		int M,N;
		std::vector<double> data;
		inline Array2D::Array2D(): data(), M(0), N(0) {} 
		inline Array2D::Array2D(const Array2D &A) : data(A.data), M(A.M), N(A.N) {}
		inline Array2D::Array2D(int m, int n) : data(m*n), M(m), N(n) {for (int i=0;i<m*n;i++) data[i]=0;}
		inline double* Array2D::operator[](int i)  { return &data[i*N]; }
		inline const double* Array2D::operator[](int i) const { return &data[i*N]; }
	};

protected:

	Array2D U, V;
	std::vector<double> s;
	int m, n;

	inline static double hypot(const double &a, const double &b)
	{
		if (a== 0) return ABS(b);
		double c = b/a;
		return ABS(a) * sqrt(1 + c*c);
	}

public:

	//! constructor
   SVD(const Array2D &Arg) 
   {
	   //Log::printf("Entering SVD\n");
      m = Arg.M;
      n = Arg.N;
      int nu = min2(m,n);
	  s = std::vector<double>(min2(m+1,n)); 
      U = Array2D(m, nu);
      V = Array2D(n,n);
	  std::vector<double> e(n);
      std::vector<double> work(m);
	  Array2D A(Arg);
      int wantu = 1;  					/* boolean */
      int wantv = 1;  					/* boolean */
	  int i=0, j=0, k=0;

      // Reduce A to bidiagonal form, storing the diagonal elements
      // in s and the super-diagonal elements in e.

      int nct = min2(m-1,n);
      int nrt = max2(0,min2(n-2,m));
      for (k = 0; k < max2(nct,nrt); k++) {
         if (k < nct) {

            // Compute the transformation for the k-th column and
            // place the k-th diagonal in s[k].
            // Compute 2-norm of k-th column without under/overflow.
            s[k] = 0;
            for (i = k; i < m; i++) {
               s[k] = hypot(s[k],A[i][k]);
            }
            if (s[k] != 0.0) {
               if (A[k][k] < 0.0) {
                  s[k] = -s[k];
               }
               for (i = k; i < m; i++) {
                  A[i][k] /= s[k];
               }
               A[k][k] += 1.0;
            }
            s[k] = -s[k];
         }
         for (j = k+1; j < n; j++) {
            if ((k < nct) && (s[k] != 0.0))  {

            // Apply the transformation.

               double t(0.0);
               for (i = k; i < m; i++) {
                  t += A[i][k]*A[i][j];
               }
               t = -t/A[k][k];
               for (i = k; i < m; i++) {
                  A[i][j] += t*A[i][k];
               }
            }

            // Place the k-th row of A into e for the
            // subsequent calculation of the row transformation.

            e[j] = A[k][j];
         }
         if (wantu & (k < nct)) {

            // Place the transformation in U for subsequent back
            // multiplication.

            for (i = k; i < m; i++) {
               U[i][k] = A[i][k];
            }
         }
         if (k < nrt) {

            // Compute the k-th row transformation and place the
            // k-th super-diagonal in e[k].
            // Compute 2-norm without under/overflow.
            e[k] = 0;
            for (i = k+1; i < n; i++) {
               e[k] = hypot(e[k],e[i]);
            }
            if (e[k] != 0.0) {
               if (e[k+1] < 0.0) {
                  e[k] = -e[k];
               }
               for (i = k+1; i < n; i++) {
                  e[i] /= e[k];
               }
               e[k+1] += 1.0;
            }
            e[k] = -e[k];
            if ((k+1 < m) & (e[k] != 0.0)) {

            // Apply the transformation.

               for (i = k+1; i < m; i++) {
                  work[i] = 0.0;
               }
               for (j = k+1; j < n; j++) {
                  for (i = k+1; i < m; i++) {
                     work[i] += e[j]*A[i][j];
                  }
               }
               for (j = k+1; j < n; j++) {
                  double t(-e[j]/e[k+1]);
                  for (i = k+1; i < m; i++) {
                     A[i][j] += t*work[i];
                  }
               }
            }
            if (wantv) {

            // Place the transformation in V for subsequent
            // back multiplication.

               for (i = k+1; i < n; i++) {
                  V[i][k] = e[i];
               }
            }
         }
      }

      // Set up the final bidiagonal matrix or order p.

      int p = min2(n,m+1);
      if (nct < n) {
         s[nct] = A[nct][nct];
      }
      if (m < p) {
         s[p-1] = 0.0;
      }
      if (nrt+1 < p) {
         e[nrt] = A[nrt][p-1];
      }
      e[p-1] = 0.0;

      // If required, generate U.

      if (wantu) {
         for (j = nct; j < nu; j++) {
            for (i = 0; i < m; i++) {
               U[i][j] = 0.0;
            }
            U[j][j] = 1.0;
         }
         for (k = nct-1; k >= 0; k--) {
            if (s[k] != 0.0) {
               for (j = k+1; j < nu; j++) {
                  double t(0.0);
                  for (i = k; i < m; i++) {
                     t += U[i][k]*U[i][j];
                  }
                  t = -t/U[k][k];
                  for (i = k; i < m; i++) {
                     U[i][j] += t*U[i][k];
                  }
               }
               for (i = k; i < m; i++ ) {
                  U[i][k] = -U[i][k];
               }
               U[k][k] = 1.0 + U[k][k];
               for (i = 0; i < k-1; i++) {
                  U[i][k] = 0.0;
               }
            } else {
               for (i = 0; i < m; i++) {
                  U[i][k] = 0.0;
               }
               U[k][k] = 1.0;
            }
         }
      }

      // If required, generate V.

      if (wantv) {
         for (k = n-1; k >= 0; k--) {
            if ((k < nrt) & (e[k] != 0.0)) {
               for (j = k+1; j < nu; j++) {
                  double t(0.0);
                  for (i = k+1; i < n; i++) {
                     t += V[i][k]*V[i][j];
                  }
                  t = -t/V[k+1][k];
                  for (i = k+1; i < n; i++) {
                     V[i][j] += t*V[i][k];
                  }
               }
            }
            for (i = 0; i < n; i++) {
               V[i][k] = 0.0;
            }
            V[k][k] = 1.0;
         }
      }

      // Main iteration loop for the singular values.

      int pp = p-1;
      int iter = 0;
      double eps(pow(2.0,-52.0));
      while (p > 0) {
         int k=0;
		 int kase=0;

         // Here is where a test for too many iterations would go.

         // This section of the program inspects for
         // negligible elements in the s and e arrays.  On
         // completion the variables kase and k are set as follows.

         // kase = 1     if s(p) and e[k-1] are negligible and k<p
         // kase = 2     if s(k) is negligible and k<p
         // kase = 3     if e[k-1] is negligible, k<p, and
         //              s(k), ..., s(p) are not negligible (qr step).
         // kase = 4     if e(p-1) is negligible (convergence).

         for (k = p-2; k >= -1; k--) {
            if (k == -1) {
               break;
            }
            if (ABS(e[k]) <= eps*(ABS(s[k]) + ABS(s[k+1]))) {
               e[k] = 0.0;
               break;
            }
         }
         if (k == p-2) {
            kase = 4;
         } else {
            int ks;
            for (ks = p-1; ks >= k; ks--) {
               if (ks == k) {
                  break;
               }
               double t( (ks != p ? ABS(e[ks]) : 0.) + 
                          (ks != k+1 ? ABS(e[ks-1]) : 0.));
               if (ABS(s[ks]) <= eps*t)  {
                  s[ks] = 0.0;
                  break;
               }
            }
            if (ks == k) {
               kase = 3;
            } else if (ks == p-1) {
               kase = 1;
            } else {
               kase = 2;
               k = ks;
            }
         }
         k++;

         // Perform the task indicated by kase.

         switch (kase) {

            // Deflate negligible s(p).

            case 1: {
               double f(e[p-2]);
               e[p-2] = 0.0;
               for (j = p-2; j >= k; j--) {
                  double t( hypot(s[j],f));
                  double cs(s[j]/t);
                  double sn(f/t);
                  s[j] = t;
                  if (j != k) {
                     f = -sn*e[j-1];
                     e[j-1] = cs*e[j-1];
                  }
                  if (wantv) {
                     for (i = 0; i < n; i++) {
                        t = cs*V[i][j] + sn*V[i][p-1];
                        V[i][p-1] = -sn*V[i][j] + cs*V[i][p-1];
                        V[i][j] = t;
                     }
                  }
               }
            }
            break;

            // Split at negligible s(k).

            case 2: {
               double f(e[k-1]);
               e[k-1] = 0.0;
               for (j = k; j < p; j++) {
                  double t(hypot(s[j],f));
                  double cs( s[j]/t);
                  double sn(f/t);
                  s[j] = t;
                  f = -sn*e[j];
                  e[j] = cs*e[j];
                  if (wantu) {
                     for (i = 0; i < m; i++) {
                        t = cs*U[i][j] + sn*U[i][k-1];
                        U[i][k-1] = -sn*U[i][j] + cs*U[i][k-1];
                        U[i][j] = t;
                     }
                  }
               }
            }
            break;

            // Perform one qr step.

            case 3: {

               // Calculate the shift.
   
               double scale = max2(max2(max2(max2(
                       ABS(s[p-1]),ABS(s[p-2])),ABS(e[p-2])), 
                       ABS(s[k])),ABS(e[k]));
               double sp = s[p-1]/scale;
               double spm1 = s[p-2]/scale;
               double epm1 = e[p-2]/scale;
               double sk = s[k]/scale;
               double ek = e[k]/scale;
               double b = ((spm1 + sp)*(spm1 - sp) + epm1*epm1)/2.0;
               double c = (sp*epm1)*(sp*epm1);
               double shift = 0.0;
               if ((b != 0.0) || (c != 0.0)) {
                  shift = sqrt(b*b + c);
                  if (b < 0.0) {
                     shift = -shift;
                  }
                  shift = c/(b + shift);
               }
               double f = (sk + sp)*(sk - sp) + shift;
               double g = sk*ek;
   
               // Chase zeros.
   
               for (j = k; j < p-1; j++) {
                  double t = hypot(f,g);
                  double cs = f/t;
                  double sn = g/t;
                  if (j != k) {
                     e[j-1] = t;
                  }
                  f = cs*s[j] + sn*e[j];
                  e[j] = cs*e[j] - sn*s[j];
                  g = sn*s[j+1];
                  s[j+1] = cs*s[j+1];
                  if (wantv) {
                     for (i = 0; i < n; i++) {
                        t = cs*V[i][j] + sn*V[i][j+1];
                        V[i][j+1] = -sn*V[i][j] + cs*V[i][j+1];
                        V[i][j] = t;
                     }
                  }
                  t = hypot(f,g);
                  cs = f/t;
                  sn = g/t;
                  s[j] = t;
                  f = cs*e[j] + sn*s[j+1];
                  s[j+1] = -sn*e[j] + cs*s[j+1];
                  g = sn*e[j+1];
                  e[j+1] = cs*e[j+1];
                  if (wantu && (j < m-1)) {
                     for (i = 0; i < m; i++) {
                        t = cs*U[i][j] + sn*U[i][j+1];
                        U[i][j+1] = -sn*U[i][j] + cs*U[i][j+1];
                        U[i][j] = t;
                     }
                  }
               }
               e[p-2] = f;
               iter = iter + 1;
            }
            break;

            // Convergence.

            case 4: {

               // Make the singular values positive.
   
               if (s[k] <= 0.0) {
                  s[k] = (s[k] < 0.0 ? -s[k] : 0.0);
                  if (wantv) {
                     for (i = 0; i <= pp; i++) {
                        V[i][k] = -V[i][k];
                     }
                  }
               }
   
               // Order the singular values.
   
               while (k < pp) {
                  if (s[k] >= s[k+1]) {
                     break;
                  }
                  double t = s[k];
                  s[k] = s[k+1];
                  s[k+1] = t;
                  if (wantv && (k < n-1)) {
                     for (i = 0; i < n; i++) {
                        t = V[i][k+1]; V[i][k+1] = V[i][k]; V[i][k] = t;
                     }
                  }
                  if (wantu && (k < m-1)) {
                     for (i = 0; i < m; i++) {
                        t = U[i][k+1]; U[i][k+1] = U[i][k]; U[i][k] = t;
                     }
                  }
                  k++;
               }
               iter = 0;
               p--;
            }
            break;
         }
      }

	  //Log::printf("Exiting SVD\n");
   }


   void getU (Array2D &A) 
   {
   	  int minm = min2(m+1,n);

	  A = Array2D(m, minm);

	  for (int i=0; i<m; i++)
	  	for (int j=0; j<minm; j++)
			A[i][j] = U[i][j];
   	
   }

   /* Return the right singular vectors */
   Array2D getV () 
		{return V;}

   /** Return the one-dimensional array of singular values */

   void getSingularValues (std::vector<double> &x) 
	{x = s;}

   /** Return the diagonal matrix of singular values
   @return     S
   */

   void getS (Array2D &A) 
   {
   	  A = Array2D(n,n);
      for (int i = 0; i < n; i++) {
         for (int j = 0; j < n; j++) {
            A[i][j] = 0.0;
         }
         A[i][i] = s[i];
      }
   }

   /** Two norm  (max2(S)) */
   double norm2 () 
   {
      return s[0];
   }

   /** Two norm of condition number (max2(S)/min2(S)) */

   double cond () 
   {
      return s[0]/s[min2(m,n)-1];
   }

   /** Effective numerical matrix rank
   @return     Number of nonnegligible singular values.
   */

   int rank () 
   {
      double eps = pow(2.0,-52.0);
      double tol = max2(m,n)*s[0]*eps;
      int r = 0;
      for (int i = 0; i < (int)s.size(); i++) {
         if (s[i] > tol) {
            r++;
         }
      }
      return r;
   }
};

#undef ABS



/////////////////////////////////////////////////////////
Planef Planef::bestFittingPlane(int pointdim,int npoints,const float* points)
{
	//dimension
	const int N=pointdim+1;

	//must be M>=N
	const int M=max2((int)npoints,N); 

	SVD::Array2D A(M,N);
	for (int I=0;I<M;I++) 
	{
		//NOTE: I'm adding the homogeneous coordinate!
		Vecf p(pointdim,1.0,points+pointdim * (I % npoints) );

		for (int J=0;J<=pointdim;J++)  
			A[I][J]=p[J];
	}

	SVD svd(A);
	SVD::Array2D V=svd.getV();

	Planef ret(pointdim);
	for (int J=0;J<=pointdim;J++)  
		ret.mem[J]=-1.0f*(float)V[J][N-1];

	//general normalization for all coefficients
	ret.normalize();


	//try to maintain the orientation of the points
	#if 1
	if (pointdim==3)
	{
		Vec3f N(ret[1],ret[2],ret[3]);

		Vec3f Nacc;

		for (int i=0;i<npoints;i++)
		{
			Vec3f pp(&points[((i==0        )?(npoints-1):(i-1))*3]);
			Vec3f p0(&points[(                           (i  ))*3]);
			Vec3f pn(&points[((i==npoints-1)?(0        ):(i+1))*3]);

			//the normal given by these 3 points
			Nacc+=(pn-p0).cross(pp-p0);
		}

		//it does change, so reverse it
		if (N*Nacc.normalize()<0)
			ret*=-1;
	}
	#endif

	//check the plane really contains the points
	#if 0
	for (int I=0;I<npoints;I++)
	{
		float d=ret.getDistance(Vecf(pointdim,1.0f,points+I*pointdim));
		DebugAssert(fabs(d)<0.001f);
	}
	#endif

	return ret;
}





/////////////////////////////////////////////
int Plane4f::SelfTest()
{
	Log::printf("Testing Plane4f...\n");


	//from equation
	Plane4f h(Vec3f(1,1,1),10.0f);
	ReleaseAssert(fabs(h.getNormal().module()-1)<0.0001f && fabs(h.getDistance(Vec3f(0,0,0))-(-10.0f))<0.0001f);

	//three points
	Vec3f O(2,3,-1);
	h=Plane4f(O+Vec3f(1,0,0),O+Vec3f(0,1,0),O);
	ReleaseAssert((h.getNormal()-Vec3f(0,0,1)).module()<0.0001f && fabs(h.getDistance(Vec3f(0,0,0))-(1.0f))<0.0001f);

	//fitting plane
	h=Plane4f(Vec3f(1,1,1),1);
	int npoints=20;

	std::vector<float> points;
	for (int i=0;i<npoints;i++)
	{
		Vec3f O(0,0,0);
		Vec3f D(Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1));
		Ray3f r(O,D);
		Vec3f P=r.intersection(h);

		Vec3f Q=P+Vec3f(Utils::FloatRand(0,0.001f),Utils::FloatRand(0,0.001f),Utils::FloatRand(0,0.001f));
		points.push_back(Q.x);
		points.push_back(Q.y);
		points.push_back(Q.z);
	}

	Plane4f hbis=Plane4f::bestFittingPlane(points);
	ReleaseAssert(hbis.fuzzyEqual(h) || hbis.fuzzyEqual(-1.0f*h));

	//check the possibility to find a plane even with very little points
	{
		std::vector<float> points;

		points.push_back(  3.0f);points.push_back(-7.0f);points.push_back(11.0f);
		points.push_back(-15.0f);points.push_back(21.0f);points.push_back(33.0f);

		Plane4f h=Plane4f::bestFittingPlane(points);

		ReleaseAssert(fabs(h.getDistance(Vec3f(&points[0])))<0.001f && fabs(h.getDistance(Vec3f(&points[3])))<0.001f);
	}

	//the ability to maintain the orientation of points in 3d
	{
		float points_ccw[]={0,0,0,1,0,0,1,1,0,2,1,0,2,0,0,3,0,0,3,2,0,0,2,0};
		Plane4f h_ccw=Plane4f::bestFittingPlane(8,points_ccw);
		ReleaseAssert(h_ccw[2]==1);

		float points_cw[]={0,2,0,3,2,0,3,0,0,2,0,0,2,1,0,1,1,0,1,0,0,0,0,0};
		Plane4f h_cw=Plane4f::bestFittingPlane(8,points_cw);
		ReleaseAssert(h_cw[2]==-1);
	}
	
	//all ok
	return 0;

}



/////////////////////////////////////////////
int Planef::SelfTest()
{
	Log::printf("Testing Planef...\n");

	Planef h(Vecf(0.0f,0.0f,0.0f,1.0f),3);//z>=3

	ReleaseAssert(h.getDistance(Vecf(1.0f, 0.0f,0.0f,3.0f))== 0);
	ReleaseAssert(h.getDistance(Vecf(1.0f, 0.0f,0.0f,0.0f))==-3);

	ReleaseAssert(h.forceAbove(Vecf(1.0f, 0.0f,0.0f,10.0f))==   h);
	ReleaseAssert(h.forceBelow(Vecf(1.0f, 0.0f,0.0f,10.0f))==-1*h);

	ReleaseAssert(fabs(Planef(Vecf(0.0f,1.0f,1.0f,1.0f),0).getNormal().module()-1)<0.0001f);

	//set best fitting plane with Vec3f(1.0,x,y,z)

	for (int dim=1;dim<16;dim++)
	{
		//plane x+y+...>=1
		Planef h(dim);
		h.mem[0]=-1;
		for (int _i=1;_i<=dim;_i++) h.mem[_i]=1.0f;
		h.normalize();

		const int npoints=20;
		std::vector<float> points;

		for (int i=0;i<npoints;i++)
		{
			//it's a point in the origin
			Vecf O(dim);
			O.mem[0]=1.0f; 

			//it's a random direction (first component is 0)
			Vecf D(dim);
			for (int j=1;j<=dim;j++) 
				D.mem[j]=Utils::FloatRand(0,1);

			Rayf r(O,D);
			Vecf inters=r.intersection(h);
			ReleaseAssert(inters[0]==1 && fabs(h.getDistance(inters))<0.001f);

			//NOTE: I'm removing the homo coordinae
			for (int U=1;U<=dim;U++)
				points.push_back(inters[U]);

			ReleaseAssert(fabs(h.getDistance(inters))<0.001f);
		}

		Planef hbis=Planef::bestFittingPlane(dim,npoints,&points[0]);
		ReleaseAssert(hbis.fuzzyEqual(h) || hbis.fuzzyEqual(-1.0f*h));
	}


	//set best fitting plane with a vector of float x,y,z,x,y,z
	for (int dim=1;dim<16;dim++)
	{
		//plane x+y+...>=1
		Planef h(dim);
		h.mem[0]=-1;
		for (int _i=1;_i<=dim;_i++) h.mem[_i]=1.0f;
		h.normalize();

		const int npoints=20;

		float* points=new float[npoints*dim];

		for (int i=0;i<npoints;i++)
		{
			//it's a point in the origin
			Vecf O(dim);
			O.mem[0]=1.0f; 

			//it's a random direction (first component is 0)
			Vecf D(dim);
			for (int j=1;j<=dim;j++) D.mem[j]=Utils::FloatRand(0,1);

			Rayf r(O,D);
			Vecf inters=r.intersection(h);
			ReleaseAssert(inters[0]==1 && fabs(h.getDistance(inters))<0.001f);

			memcpy(points+i*dim,&inters.mem[1],sizeof(float)*dim);
		}

		Planef hbis=Planef::bestFittingPlane(dim,npoints,points);
		ReleaseAssert(hbis.fuzzyEqual(h) || hbis.fuzzyEqual(-1.0f*h));
		delete [] points;
	}

	//random points on random planes
	{
		for (int dim=2;dim<10;dim++)
		{
			Planef h=Planef::getRandomPlane(dim);

			for (int I=0;I<dim*4;I++)
			{
				Vecf p=h.getRandomPoint();
				ReleaseAssert (h.getDistance(p)<0.0001f);
			}
		}
	}

	//check the possibility to find a plane even with very little points

	{
		int Dim=5;
		int npoints=2;
		std::vector<float> points;
		points.push_back(  3.0f);points.push_back( -7.0f);points.push_back(11.0f);points.push_back(+0.5f);points.push_back(+100);
		points.push_back(-15.0f);points.push_back( 21.0f);points.push_back(33.0f);points.push_back(-0.7f);points.push_back(-46);

		Planef h=Planef::bestFittingPlane(Dim,points);
		ReleaseAssert(h.dim==Dim);

		//check if points are on the plane
		for (int J=0;J<npoints;J++)
		{
			Vecf T(Dim,1,&points[J*Dim]);
			float distance=fabs(h.getDistance(T));
			ReleaseAssert(distance<0.001f);
		}
		
	}



	return 0;
}

