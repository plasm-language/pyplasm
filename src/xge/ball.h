#ifndef _BALL_H__
#define _BALL_H__

#include <xge/xge.h>
#include <xge/vec.h>

//===========================================================================
//! classe per rappresentare bounding ball in 3d
/*
	Una bounding ball e' formata da un #center Vec3f(x,y,z) e da un #radius.
	Una bounding ball e' considerata valida solo se il suo #radius
	e' maggiore o uguale a zero (radius>=0). Per testarne la validita'
*	usare il metodo isValid().
*/
//===========================================================================
class XGE_API Ball3f
{
public:

	//! il centro della bounding ball come vettore Vec3f(x,y,z)
	Vec3f center; 

	//! Il raggio della bounding ball. Solo se il raggio>=0 la bounding ball e' valida
	float radius; 

	//! default constructor
	/*!
		Crea una ball con raggio non valido (==-1) e centro nell'origine
	
		@cpp
		Ball3f b;
		assert(!b.isValid() && b.cenmter==Vec3f(0,0,0));
		@endcpp

		@py
		b=Ball3f()
		assert not b.isValid() and b.center==Vec3f(0,0,0)
		@endpy
	*/
	inline Ball3f()
	{
		center=Vec3f(0,0,0);
		radius=-1; //invalid
	}

	//! copy constructor
	/*!
		Crea una ball copiando i dati da una altra ball sorgente

		\param other la ball sorgente da cui copiare

		@cpp
		Ball3f src(1,Vec3f(0,0,0));
		Ball3f dst(src);
		assert(src.radius==dst.radius && src.center==dst.center);
		@endcpp
		
		@py
		import copy
		src=Ball3f(10,Vec3f(1,2,3))
		dst=copy.copy(src)
		assert src.radius==dst.radius and src.center==dst.center
		@endpy
	*/
	inline Ball3f(const Ball3f& other)
	{
		this->center=other.center;
		this->radius=other.radius;
	}

	//! constuctor da raggio e centro
	/*!
		\param radius il raggio della ball. deve essere >=0
		\param center il centro della ball

		@cpp
		Ball3f b(1,Vec3f(0,0,0));
		assert(b.radius==1 and b.center==Vec3f(0,0,0));
		@endcpp

		@py
		b=Ball3f(1,Vec3f(0,0,0))
		assert b.radius==1 and b.center==Vec3f(0,0,0)
		@endpy
	*/
	inline explicit Ball3f(float radius,Vec3f center)
	{
		this->radius=radius;
		this->center=center;
	}

	//! Trova la best fitting ball a partire da lista di punti
	/*!
		In realta non trova la best fitting ma solo una ball che contiene tutti i punti di ingresso

		\param points i punti che la ball restituita deve contenere
		\return ritorna una ball che contiene i punti in ingresso

		
		@py
		points=[1,0,0,   -1,0,0,  0,1,0,  0,-1,0 ] # punti nel range [-1,+1] sugli assi X e Y
		b=Ball3f.bestFittingBall(points) 
		assert b.center.fuzzyEqual(Vec3f(0,0,0)) and fabs(1-b.radius)<1e-4
		@endpy
	*/
	static inline Ball3f bestFittingBall(const std::vector<float>& points)
	{
		Ball3f ret;

		int npoints=(int)points.size()/3;

		ret.center=Vec3f(0,0,0);
		float coeff=(float)(1.0/npoints);

		//compute the centroid and put it in [1...pointdim]
		for (int i=0;i<npoints;i++)
			ret.center+=coeff*Vec3f(&points[i*3]);

		ret.radius=0;
		for (int i=0;i<npoints;i++)
		{
			Vec3f T(&points[i*3]);
			ret.radius=max2(ret.radius,(T-ret.center).module());
		}

		return ret;
	}


	//! torna true se la bounding ball e' valida (cioe' #radius>=0) 
	/*!
		\return true se la ball e' valida (cioe' se #radius>=0) altrimenti false

		@cpp
		assert (!Ball3f().isValid() && Ball3f(1.0f,Vec3f(0,0,0)).isValid());
		@endcpp

		@py
		assert not Ball3f().isValid() and Ball3f(1.0,Vec3f(0,0,0)).isValid()
		@endpy
	*/
	inline bool isValid() const
		{return radius>=0;}


	//! assignment operator (side effect)
	/*!
		\param other la ball dalla quale copiare i dati
		\return la ball stessa 

		@cpp
		Ball3f b();
		b=Ball3f(1,Vec3f(0,0,0));
		assert (b.radius==1 && b.center==Vec3f(0,0,0);
		@endcpp

		@py
		b=Ball3f()
		b.assign(Ball3f(1,Vec3f(0,0,0)))
		assert b.radius==1 and b.center==Vec3f(0,0,0)
		@endpy
	*/
	inline Ball3f& operator=(const Ball3f& other) 
	{
		this->center=other.center;
		this->radius=other.radius;
		return *this;
	}

	//! Test di uguaglianza
	/*!
		Confronta due Ball3f per vedere se sono uguali in senso stretto (stesso #center e stesso #radius). 
	   se una delle due ball (o entrambe) non sono valide ritorna false
		
		\param other altra ball da confrontare
		\return true se sono uguali, altrimenti false

		@cpp
		assert(Ball3f(1,Vec3f(0,0,0))==Ball3f(1,Vec3f(0,0,0)) && Ball3f(1,Vec3f(0,0,0))!=Ball3f(2,Vec3f(0,0,0)));
		@endcpp

		@py
		assert Ball3f(1,Vec3f(0,0,0))==Ball3f(1,Vec3f(0,0,0)) and Ball3f(1,Vec3f(0,0,0))!=Ball3f(2,Vec3f(0,0,0))
		@endpy
	*/
	inline bool operator==(const Ball3f& other) const
	{
		if (!other.isValid() || !this->isValid())
			return false;

		return other.center==this->center && other.radius==this->radius;
	}

	//! Test di uguaglianza fuzzy
	/*!	
		Confronta due ball per vedere se sono uguali in senso fuzzy (#center e #radius uguali a meno di un certo margine di errore)
	   se una delle due ball, o entrambe, non sono valide allora ritorna false
		\param other altra Ball3f da confrontare
		\return true se sono uguali in senso fuzzy, altrimenti false

		@cpp
		Ball3f b1(1,Vec3f(0,0,0));
		Ball3f b2(1+1e-8,Vec3f(1e-8,1e-8,1e-8));
		assert(b2.fuzzyEqual(b2));
		@endcpp

		@py
		b1=Ball3f(1,Vec3f(0,0,0))
		b2=Ball3f(1+1e-8,Vec3f(1e-8,1e-8,1e-8))
		assert b2.fuzzyEqual(b2)
		@endpy
	*/
	inline bool fuzzyEqual(const Ball3f& other) const
	{
		if (!other.isValid() || !this->isValid())
			return false;

		return other.center.fuzzyEqual(this->center) && Utils::FuzzyEqual(other.radius,this->radius);
	}

	//! calcola la superficie
	/*!
		\return a superficie della Ball3f, se !isValid() ritorna 0

		@cpp
		Ball3d b(1,Vec3f(0,0,0));
		assert(fabs(b.surface()-(4*M_PI))<1e-4);
		@endcpp

		@py
		assert Ball3f().surface()==0
		@endpy
	*/
    inline float surface() const
	{    
		if (!isValid()) return 0;
        return (float)(4*M_PI*radius*radius);
	}


	//!calcola il volume della ball
	/*!
		\return restituisce il volume della Ball3f, se !isValid() ritorna 0

		@cpp
		Ball3d b(1,Vec3f(0,0,0));
		assert(fabs(b.volume()-(4*M_PI/3))<1e-4);
		@endcpp

		@py
		assert Ball3f().volume()==0
		@endpy
	*/
	inline float volume() const
	{
		if (!isValid()) return 0;
		return (float)((4*M_PI*radius*radius*radius)/3);
	}

	//! python str
	/*!
		\return il valore python di str(Ball3f)
	
		@py
		assert eval(str(Ball3f(1,Vec3f(0,0,0))))==[1,[0,0,0]]
		@endpy
	*/
	inline std::string str() const
	{
		return Utils::Format("[%f,%s]",radius,center.str().c_str());
	}

	//! python repr
	/*!
		\return il valore python di repr(Ball3f)
	
		@py
		assert eval(repr(Ball3f(1,Vec3f(0,0,0))))==Ball3f(1,Vec3f(0,0,0))
		@endpy
	*/
	inline std::string repr() const
	{
		return Utils::Format("Ball3f(%e,%s)",radius,center.repr().c_str());
	}


	///self testing
	static int SelfTest();

};



//===========================================================================
//!Classe per rappresentare bounding ball in n-dim
/*
	Una bounding ball e' formata da un #center Vecf(...) e da un #radius.
	Una bounding ball e' considerata valida solo se il suo #radius
	e' maggiore o uguale a zero (radius>=0). Per testarne la validita'
	usare il metodo isValid().

	Importante: la classe "lavora" in n-dim quindi hai bisogno che il centro della ball abbia la prima coordinata 
	omogenea che deve! essere pari a 1. Ad esempio per costruire una ball in 3D a partire dal raggio e dal centro:\n

Esempio C++:
	float radius=1;
	float x=0,y=0,z=0;
	Ballf b(radius,Vecf(1, x,y,z));
	assert(b.dim()==3 && b.center[0]==1 && b.center[1]==x && b.center[2]==y && b.center[3]==z);


Esempio python:

	radius=1
	x,y,z=(0,0,0)
	b=Ballf(radius,Vecf(1,x,y,z))
	assert b.dim()==3 and b.center[0]==1 and b.center[1]==x and b.center[2]==y and b.center[3]==z


*/
//===========================================================================
class XGE_API Ballf
{
	
public:

	//! il centro della bounding ball come vettore Vecf(...)
	Vecf  center;

	//! Il raggio della bounding ball. Solo se il raggio>=0 la bounding ball e' valida
	float radius;

	//! default constructor
	/*
		@cpp
		Ballf b;
		assert(!b.isValid() && b.radius<0);
		@endcpp

		@py
		b=Ballf()
		assert not b.isValid() and b.radius<0
		@endpy
	*/
	inline Ballf()
	{
		center=Vecf();
		radius=-1; //invalid
	}

	//!copy constructor
	/*!
		\param other l'altra ball dalla quale copiare i dati

		@cpp
		Ballf src(1,Vecf(1,0,0,0));
		Ballf dst(src);
		assert(dst.radius==1 && dst.center==Vecf(1,0,0,0));
		@endcpp
		
		@py
		b=Ballf(Ballf(1,Vecf([1,0,0,0])))
		assert b.radius==1 and b.center==Vecf([1,0,0,0])
		@endpy
	*/
	inline Ballf(const Ballf& other)
	{
		this->center=other.center;
		this->radius=other.radius;
	}


	//!constuctor da raggio e centro
	/*!
		\param radius il raggio della ball
		\param center il centro della ball (NOTA: lavoro in n-dim quindi hai bisogno della prima coordinata omogenea che deve! essere pari a 1)

		@cpp
		Ballf b(1,Vecf(1,0,0,0)); # esempio di ball in 3d
		assert (b.dim()==3 && b.radius==1 and b.center==Vecf(1,0,0,0));
		@endcpp

		@py
		b=Ballf(1,Vecf([1,0,0,0])) # esempio di ball in 3d
		assert b.dim()==3 and b.radius==1 and b.center==Vecf([1,0,0,0])
		@endpy
	*/
	inline explicit Ballf(float radius,Vecf center)
	{
		DebugAssert(center[0]==1); //must be a point
		this->radius=radius;
		this->center=center;
	}

	//! ritorna la dimensione (point dim) della Ballf (che e' la dimensione del Vecf #center)
	/*!
		\return la dimensione della sfera (es in 3d ritorna 3 anche se BBallf::center ha in realta' 4 floats, compresa la coordinata omogenea)

		@cpp
		Ballf b(1,Vecf(1,0,0,0));
		assert(b.dim()==3);
		@endcpp

		@py
		b=Ballf(1,Vecf(1,0,0,0))
		assert b.dim()==3
		@endpy
	*/
	inline int dim() const
	{
		return center.dim;
	}


	//! trova la best fitting ball dei punti
	/*!
		Trova una ball che contiene i #points (in realta' non torna
		la best fitting ball, il cui calcolo sarebbe troppo lento) ma
		una ball che semplicemente contiene i punti dati in ingresso.

		\param points i punti che la ball restituita deve contenere
		\return una approssimazione grossolata della best fitting ball, cioe' una ball che contiene i punti in ingresso

		@py
		points=[ 1,1,0,0,   1,-1,0,0,   1,0,1,0,   1,0,-1,0  ] # esempio di ball in 3d
		b=Ballf.bestFittingBall(3,points)
		assert fabs(b.radius-1)<1e-4 and b.center.fuzzyEqual(Vecf([1,0,0,0]))
		@endpy
	*/
	static inline Ballf bestFittingBall(int pointdim,const std::vector<float>& points)
	{
		Ballf ret;
		int npoints=(int)points.size()/(pointdim+1);
		DebugAssert(npoints*(pointdim+1)==points.size());
		ret.center=Vecf(pointdim);

		//media pesata
		float coeff=(1.0f/npoints);
		for (int i=0;i<npoints;i++)
		{
			ReleaseAssert(points[i*(pointdim+1)]==1.0f);
			ret.center+=coeff*Vecf(pointdim,&points[i*(pointdim+1)]);
		}

		ret.center.set(0,1.0f);

		//raggio minimo
		ret.radius=0;
		for (int i=0;i<npoints;i++)
		{
			Vecf T(pointdim,&points[i*(pointdim+1)]);
			ret.radius=max2(ret.radius,(T-ret.center).module());
		}

		return ret;
	}

	//! testa se la bounding ball e' valida (==se #radius>=0) 
	/*!
		\return true se il raggio>=0, altrimenti false

		@cpp
		Ballf b();
		assert (!b.isValid());
		@endcpp

		@py
		b=Ballf()
		assert not b.isValid()
		@endpy
	*/
	inline bool isValid() const
		{return radius>=0;}

	//! assignment operator (side effect)
	/*
		\param other la ball dalla quale copiare i dati
		\return la classe stessa (side effect)

		@cpp
		Ballf b(1,Vecf(1 ,1,2,3));
		b.assign(Ballf(1 ,Vecf(1,0,0,0)));
		assert (b.radius==1 and b.center==Vecf(1,0,0,0));
		@endcpp

		@py
		b=Ballf(1,Vecf(1,1,2,3))
		b.assign(Ballf(1,Vecf(1,0,0,0)))
		assert b.radius==1 and b.center==Vecf([1,0,0,0])
		@endpy
	*/
	inline Ballf& operator=(const Ballf& other) 
	{
		this->center=other.center;
		this->radius=other.radius;
		return *this;
	}

	//! Test di uguaglianza
	/*!
		Confronta due Ballf per vedere se sono uguali in senso stretto (stesso #center e stesso #radius)
	    se una delle due ball, o entrambe, non sono valide allora ritorna false

		\param other altra Ballf da confrontare
		\return true se sono uguali, altrimenti false

		@cpp
		Ballf b(1,Vecf(1, 1,2,3));
		assert(b==Ballf(b));
		@endcpp

		@py
		import copy
		b=Ballf(1,Vecf(1,1,2,3))
		assert b==copy.copy(b)
		@endpy
	*/
	inline bool operator==(const Ballf& other) const
	{
		if (!other.isValid() || !this->isValid())
			return false;

		return other.center==this->center && other.radius==this->radius;
	}


	//! Test di uguaglianza fuzzy
	/*!	
		Confronta due Ballf per vedere se sono uguali in senso fuzzy (#center e #radius uguali a meno di un certo margine di errore)
	    se una delle due ball, o entrambe, non sono valide allora ritorna false

		\param other altra Ballf da confrontare
		\return true se sono uguali in senso fuzzy, altrimenti false

		@cpp
		Ballf b1(1,Vecf(1,1,2,3));
		Ballf b2(b1);
		b2.radius+=1e-8;
		assert (b1.fuzzyEqual(b2));
		@endcpp

		@py
		import copy
		b1=Ballf(1,Vecf(1,1,2,3))
		b2=copy.copy(b1)
		b2.radius+=1e-8
		assert b1.fuzzyEqual(b2)
		@endpy
	*/
	inline bool fuzzyEqual(const Ballf& other) const
	{
		if (!other.isValid() || !this->isValid())
			return false;

		return other.center.fuzzyEqual(this->center) && Utils::FuzzyEqual(other.radius,this->radius);
	}

	//! python str
	/*!
		\return la python str() della ball

		@py
		b=Ballf(1,Vecf(1,0,0,0))
		assert eval(str(b))==[1,[1,0,0,0]]
		@endpy
	*/
	inline std::string str() const
	{
		return Utils::Format("[%f,%s]",radius,center.str().c_str());
	}

	//! python repr
	/*!
		\return la python repr() della ball

		@py
		b=Ballf(1,Vecf(1,0,0,0))
		assert eval(repr(b))==Ballf(1,Vecf(1,0,0,0))
		@endpy
	*/
	inline std::string repr() const
	{
		return Utils::Format("Ballf(%e,%s)",radius,center.repr().c_str());
	}

	//self testing
	static int SelfTest();

}; //end class
 

#endif //_BALL_H__

