#ifndef _BOX_H__
#define _BOX_H__

#include <xge/xge.h>
#include <xge/vec.h>
#include <xge/plane.h>

class XGE_API Box2i
{
public:

	Vec2i p1,p2;
	unsigned char align;

	//constructor
	inline explicit Box2i(const Vec2i& p1,const Vec2i& p2,unsigned char align=0) {this->p1=p1;this->p2=p2;this->align=align;}

	//left...top
	inline int  left  () const {return p1.x;}
	inline int  right () const {return p2.x;}
	inline int  bottom() const {return p1.y;}
	inline int  top   () const {return p2.y;}


	//width/height/area
	inline int  width () const {DebugAssert(isValid());return right()-left();}
	inline int  height() const {DebugAssert(isValid());return top()-bottom();}
	inline int  area  () const {return width()*height();}

	//if valud
	inline bool isValid  () const {return right()>left() && top()>bottom() ;}

	//containment
	inline bool contains(const Vec2i p   ) const{DebugAssert(this->isValid());return p.x>=left() && p.x<=right() && p.y>=bottom() && p.y<=top();}
	inline bool contains(const Box2i& box) const{DebugAssert(this->isValid() && box.isValid());return  this->contains(box.p1) && this->contains(box.p2);}

	//sort operator
	bool operator <(const Box2i& b) const
	{
		const Box2i& a=*this;
		int a_area  =a.area();
		int b_area  =b.area();
		return 
			(a_area <b_area) || 
			(a_area==b_area && a.bottom() <b.bottom()) ||
			(a_area==b_area && a.bottom()==b.bottom() && a.left() <b.left()) ||
			(a_area==b_area && a.bottom()==b.bottom() && a.left()==b.left() && a.top() <b.top()) ||
			(a_area==b_area && a.bottom()==b.bottom() && a.left()==b.left() && a.top()==b.top() && a.right()<b.right());
	}
};


//===========================================================================
//! Un box in 3-dim
//===========================================================================
class XGE_API Box3f
{
public:

	void Write(Archive& ar)
	{
		ar.WriteFloat("p1x",p1.x);
		ar.WriteFloat("p1y",p1.y);
		ar.WriteFloat("p1z",p1.z);
		ar.WriteFloat("p2x",p2.x);
		ar.WriteFloat("p2y",p2.y);
		ar.WriteFloat("p2z",p2.z);
	}

	void Read(Archive& ar)
	{
		p1.x=ar.ReadFloat("p1x");
		p1.y=ar.ReadFloat("p1y");
		p1.z=ar.ReadFloat("p1z");
		p2.x=ar.ReadFloat("p2x");
		p2.y=ar.ReadFloat("p2y");
		p2.z=ar.ReadFloat("p2z");
	}

public:

	//! punto p1 del bounding box (deve essere p1<=p2 in X,Y,Z)
	Vec3f p1;

	//! punto p2 del bounding box (deve essere  p2>=p1 in X,Y,Z)
	Vec3f p2;

	//!default constructor
	/*!
		Costruisce un box di contenimento 3d non valido (in quanto p1 e p2 non sono impostati)

		@py
		b=Box3f()
		assert not b.isValid()
		@endpy
	*/
	inline Box3f()
	{
		reset();
	}

	//!copy constructor
	/*!
		\param src il box sorgente 

		@py
		import copy
		b1=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		b2=copy.copy(b1)
		assert b2.p1==b1.p1 and b2.p2==b1.p2
		@endpy
	*/	
	inline Box3f(const Box3f& src)
	{
		this->p1=src.p1;
		this->p2=src.p2;
	}

	//! constructor a partire da due punti
	/*!
		non e' richiesto che v0<=v1 in quanto internamente viene usata la Box3f::add(Vec3f)
	
		\param v0 il punto v0 del box
		\param v1 il punto v1 del box

		@py
		b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		assert b.isValid() and b.p1==Vec3f(0,0,0) and b.p2==Vec3f(1,1,1)
		@endpy
	*/
	inline explicit Box3f(const Vec3f& v0,const Vec3f& v1)
	{
		reset();
		add(v0);
		add(v1);
	}

	//!crea un nuovo bounding box a partire dal centro e dalle sue dimensioni
	/*!
		\param center il centro del box
		\param dim le dimensioni x,y,z del box
		\return il box che ha come centro center e dimensioni dim

		@py
		b=Box3f.buildFromCenter(Vec3f(0,0,0),Vec3f(1,1,1))
		assert b.isValid() and b.p1==Vec3f(-0.5,-0.5,-0.5) and b.p2==Vec3f(+0.5,+0.5,+0.5)
		@endpy
	*/
	static inline Box3f buildFromCenter(const Vec3f& center,const Vec3f& dim)
	{
		Vec3f half_dim=dim/2;
		return Box3f(center-half_dim,center+half_dim);
	}


	//!setta il box a non essere valido (internal use)
	/*!
		@py
		b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		b.reset()
		assert not b.isValid()
		@endpy
	*/
	inline void reset()
	{
		p1=Vec3f(+FLT_MAX,+FLT_MAX,+FLT_MAX);
		p2=Vec3f(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	}


	//!accedi uno degli otto punti del box
	/*!
		i primi quattro punti sono sul piano Z=p1.z, gli ultimi quattro punti sono sul piano Z=p2.z

		\param i l'indice del punto da restituire
		\return uno degli otto punti del box

		@py
		b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		p=[Vec3f(0,0,0),Vec3f(1,0,0),Vec3f(1,1,0),Vec3f(0,1,0),Vec3f(0,0,1),Vec3f(1,0,1),Vec3f(1,1,1),Vec3f(0,1,1)]
		assert b.getPoint(0)==p[0] and b.getPoint(1)==p[1] and b.getPoint(2)==p[2] and b.getPoint(3)==p[3] and b.getPoint(4)==p[4] and b.getPoint(5)==p[5] and b.getPoint(6)==p[6] and b.getPoint(7)==p[7]
		@endpy
	*/
	inline Vec3f getPoint(int i) const
	{
		DebugAssert(i>=0 && i<8 && isValid());
		if (i==0) return Vec3f(p1.x,p1.y,p1.z);
		if (i==1) return Vec3f(p2.x,p1.y,p1.z);
		if (i==2) return Vec3f(p2.x,p2.y,p1.z);
		if (i==3) return Vec3f(p1.x,p2.y,p1.z);
		if (i==4) return Vec3f(p1.x,p1.y,p2.z);
		if (i==5) return Vec3f(p2.x,p1.y,p2.z);
		if (i==6) return Vec3f(p2.x,p2.y,p2.z);
		if (i==7) return Vec3f(p1.x,p2.y,p2.z);
		return Vec3f();
	}

	//! return the planes of a box
	inline Plane4f getPlane(int i) const
	{
		DebugAssert(i>=0 && i<6 && isValid());

		if (i==0) return Plane4f(-1,0,0,+p1.x); //x<=p1.x
		if (i==1) return Plane4f(+1,0,0,-p2.x); //x>=p2.x
		if (i==2) return Plane4f(0,-1,0,+p1.y);
		if (i==3) return Plane4f(0,+1,0,-p2.y);
		if (i==4) return Plane4f(0,0,-1,+p1.z);
		if (i==5) return Plane4f(0,0,+1,-p2.z);
		return Plane4f();

	}

	//! aggiunge un punto Vec3f al bounding box estendendone i confini (side effect)
	/*!
		\param v il punto da aggiungere al box
	
		@py
		b=Box3f();b.add(Vec3f(1,1,1))
		assert b.isValid() and b.p1==Vec3f(1,1,1) and b.p2==Vec3f(1,1,1)
		@endpy
	*/
	inline void add(const Vec3f& v)
	{
		this->p1=this->p1.Min(v);
		this->p2=this->p2.Max(v);
	}

	//! aggiunge un box al bounding box corrente estendendone i confini solo se other e' valido (side effect)
	/*!
		\param other il box da aggiungere
	
		@py
		b=Box3f()
		b.add(Box3f(Vec3f(0,0,0),Vec3f(1,1,1)))
		assert b.isValid() and b.p1==Vec3f(0,0,0) and b.p2==Vec3f(1,1,1)
		@endpy
	*/
	inline void add(const Box3f& other)
	{
		if (!other.isValid()) return;
		this->add(other.p1);
		this->add(other.p2);
	}

	//! test per vedere se la bounding box e' valida
	/*!
		Un box e' valido se :
		<ul>
			<li> x,y,z di p1 e p2 sono numeri validi
			<li> p1.x<=p2.x && p1.y<=p2.y && p1.z<=p2.z
		</ul>

		\return true se valid, false altrimenti

		@py
		b=Box3f()
		assert not b.isValid()
		@endpy
	*/
	inline bool isValid() const
	{
		return 
			   !isnan(this->p1.x) && finite(this->p1.x)
			&& !isnan(this->p1.y) && finite(this->p1.y)
			&& !isnan(this->p1.z) && finite(this->p1.z)
			&& !isnan(this->p2.x) && finite(this->p2.x)
			&& !isnan(this->p2.y) && finite(this->p2.y)
			&& !isnan(this->p2.z) && finite(this->p2.z)
			&& this->p1.x<=this->p2.x 
			&& this->p1.y<=this->p2.y 
			&& this->p1.z<=this->p2.z;
	}

	//! ritorna le dimensioni correnti della bounding box, se non valida ritorna il vettore nullo
	/*!
		\return le dimensioni del box, vettore nullo se il box non e' valido
	
		@py
		b=Box3f(Vec3f(0,0,0),Vec3f(1,2,3))
		assert b.size()==Vec3f(1,2,3)
		@endpy
	*/
	inline Vec3f size() const
	{
		if (!isValid()) return Vec3f(0,0,0);
		return p2-p1;
	}

	//! ritorna la  dimensione minima
	/*!
		\return la minima dimensione 

		@py
		b=Box3f(Vec3f(0,0,0),Vec3f(1,2,3))
		assert b.minsize()==1
		@endpy
	*/
	inline float minsize() const
	{
		Vec3f d=size();
		return min3(d.x,d.y,d.z);
	}

	//!ritorna la dimensione massima
	/*!
		\return la massima dimensione 

		@py
		b=Box3f(Vec3f(0,0,0),Vec3f(1,2,3))
		assert b.maxsize()==3
		@endpy
	*/
	inline float maxsize() const
	{
		Vec3f d=size();
		return max3(d.x,d.y,d.z);
	}

	//!trasla il bounding box di un vettore vt
	/*!
		\param vt il vettore traslazione
		\return il box traslato

		@py
		b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1)).translate(Vec3f(1,2,3))
		assert b.p1==Vec3f(1,2,3) and b.p2==Vec3f(2,3,4)
		@endpy
	*/
	inline Box3f translate(Vec3f vt) const
	{
		if (!isValid()) return Box3f();
		return Box3f(p1+vt,p2+vt);
	}

	//!scala il bounding box di una certa percentuale (es se percent=2.0 ne raddoppio le dimensioni)
	/*!
		\param percent la percentuale di scaling
		\return il box scalato
	
		@py
		b=Box3f(Vec3f(-1,-1,-1),Vec3f(1,1,1)).scale(2)
		assert b.p1==Vec3f(-2,-2,-2) and b.p2==Vec3f(+2,+2,+2)
		@endpy
	*/
	inline Box3f scale(float percent) const
	{
		if (!isValid()) return Box3f();
		Vec3f c=center();
		Vec3f half_size=size()*(percent*0.5f);
		return Box3f(c-half_size,c+half_size);
	}

	//!torna il punto centrale del bounding box
	/*!
		\retun il punto centrale del box

		@py
		b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		assert b.center()==Vec3f(0.5,0.5,0.5)
		@endpy
	*/
	inline Vec3f center() const
	{
		DebugAssert(isValid());
		return 0.5*(p1+p2);
	}

	//! testa se la bounding box contiene in punto v
	/*!
		\param v il punto da testare
		\return true se il box contiene il punto, altrimenti false
	
		@py
		b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		assert b.contains(Vec3f(0.5,0.5,0.5)) and not b.contains(Vec3f(1.5,1.5,1.5))
		@endpy
	*/
	inline bool contains(const Vec3f& v) const
	{
		DebugAssert(isValid());
		return v.x>=p1.x && v.x<=p2.x &&
			   v.y>=p1.y && v.y<=p2.y &&
			   v.z>=p1.z && v.z<=p2.z;
	}

	//! testa se la bounding box contiene il box other
	/*!
		\param other il box da testare
		\return true se il box e' dentro, altrimenti false
	
		@py
		b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		assert b.contains(Box3f(Vec3f(0.2,0.2,0.2),Vec3f(0.8,0.8,0.8))) 
		b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		assert not b.contains(Box3f(Vec3f(0.2,0.2,0.2),Vec3f(1.8,1.8,1.8))) 
		@endpy
	*/
	inline bool contains(const Box3f& other) const
	{
		return this->contains(other.p1) && this->contains(other.p2);
	}

	//! controlla se due bounding box si sovrappongono
	/*!
		\param other altro box
		\return  torna false su uno (o entrambi) non sono validi

		@py
		b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		assert b.overlap(Box3f(Vec3f(0.2,0.2,0.2),Vec3f(1.8,1.8,1.8))) 
		b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		assert not b.overlap(Box3f(Vec3f(1.2,1.2,1.2),Vec3f(1.8,1.8,1.8))) 
		@endpy
	*/
	inline bool overlap(const Box3f& other) const
	{
		if(!isValid() || !other.isValid())
			return false;

		return Utils::Overlap1d(p1.x,p2.x,other.p1.x,other.p2.x) 
			&& Utils::Overlap1d(p1.y,p2.y,other.p1.y,other.p2.y)
			&& Utils::Overlap1d(p1.z,p2.z,other.p1.z,other.p2.z);
	}

	//! calcola l'intersezione di due bounding box
	/*!
		\param other altro box
		\return intersezione dei due box. Restituisce un box non valido se uno dei due (o entrambi) non sono validi oppure se non esiste intersezione

		@py
		b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		assert b.intersection(Box3f(Vec3f(0.2,0.2,0.2),Vec3f(1.8,1.8,1.8)))==Box3f(Vec3f(0.2,0.2,0.2),Vec3f(1,1,1))
		@endpy
	*/
	inline Box3f intersection(const Box3f& other)
	{
		if(!isValid() || !other.isValid())
			return Box3f();

		//invalid box!
		if (!overlap(other))
			return Box3f();

		Box3f ret;
		ret.p1=p1.Max(other.p1);
		ret.p2=p2.Min(other.p2);
		return ret;
	}

	//! testa l'uguaglianza in senso stretto
	/*!
		\param other altro box da testare
		\return true se sono uguali, false altrimenti

		@py
		b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		assert b==Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		assert b!=Box3f(Vec3f(0,0,0),Vec3f(1.1,1.1,1.1))
		@endpy
	*/
	inline bool operator==(const Box3f& other) const
	{
		if (!other.isValid() || !this->isValid())
			return false;

		return other.p1==this->p1 && other.p2==this->p2;
	}

	//! testa l'uguaglianza in senso fuzzy (cioe' a meno di un certo errore)
	/*!
		\param other l'atro box
		\return true se fuzzyEqual, false altrimenti
			
		@py
		b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		assert b.fuzzyEqual(Box3f(Vec3f(0,0,0),Vec3f(1+1e-6,1+1e-6,1+1e-6)))
		@endpy
	*/
	inline bool fuzzyEqual(const Box3f& other,float Epsilon=0.001f) const
	{
		if (!other.isValid() || !this->isValid())
			return false;

		return other.p1.fuzzyEqual(this->p1,Epsilon) && other.p2.fuzzyEqual(this->p2,Epsilon);
	}

	//! calcola la superficie 
	/*!
		\return superficie del bounding box, torna 0 se non valido
		@py
		b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		assert b.surface()==6
		@endpy
	*/
    inline float surface() const
	{    
        if (!isValid()) return 0;
		Vec3f d=size();
        return 2.0f*(d.x*d.y + d.x*d.z + d.y*d.z);
	}


	//!calcola il volume
	/*!
		\return volume del bounding box, torna 0 se non valido

		@py
		b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		assert b.volume()==1
		@endpy
	*/
	inline float volume() const
	{
		if (!isValid()) return 0;
		Vec3f d=size();
        return d.x*d.y*d.z;
	}


	//! python repr
	/*!
		\return la python repr()
	
		@py
		b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		assert(eval(repr(b)))==Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		@endpy
	*/
	inline std::string repr() const
	{
		return Utils::Format("Box3f(Vec3f(%e,%e,%e),Vec3f(%e,%e,%e))",p1.x,p1.y,p1.z,p2.x,p2.y,p2.z);
	}

	//! python str
	/*!
		\return la python str()
	
		@py
		b=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		assert eval(str(b))==[[0,0,0],[1,1,1]]
		@endpy
	*/
	inline std::string str() const
	{
		return Utils::Format("[[%f,%f,%f],[%f,%f,%f]]",p1.x,p1.y,p1.z,p2.x,p2.y,p2.z);
	}

	//self testing
	static int SelfTest();

};



//===========================================================================
//! box in n-dim
/*!
	Nota che lavorando in n-dim tutti i punti (es p1 e p2) devono comprendere
	la coordinata omogenea. Quindi ad esempio in 3-dim i punti contengono 4 floats
	che sono (1.0,x,y,z). Cosi' come i vettori conterranno 4 floats che sono (0.0,x,y,z).
*/
//===========================================================================
class XGE_API Boxf
{

public:

	//! punto p1 (deve essere p1<=p2 per tutte le coordinate, e p1[0]=1.0f dovendo essere un punto)
	Vecf p1;

	//!punto p2 (deve essere p2>=p1 per tutte le coordinate, e p2[0]=1.0f dovendo essere un punto)
	Vecf p2;

	//!default constructor
	/*! 
		Costruisce un box in 0-dim non valido

		@py
		b=Boxf(3); assert not b.isValid()
		@endpy
	*/
	inline Boxf()
	{
		reset(0);
	}

	//!copy constructor
	/*! 
		Costruisce un box copia dell'argomento src

		\param src il box di origine di cui effettuare la copia

		@py
		b=Boxf(Boxf(Vecf(1, 0,0),Vecf(1, 1,1)))
		assert b.p1.dim==2 and b.p1==Vecf(1, 0,0) and b.p2==Vecf(1, 1,1)
		@endpy
	*/
	inline Boxf(const Boxf& src)
		:p1(src.p1),p2(src.p2)
	{}


	//!constructor di un box con una certa dimensione
	/*! 
		Costruisce un box in dim-D non valido

		\param dim la dimensione voluta (es 3 per un box in 3-dim)

		@py
		b=Boxf(3)
		assert not b.isValid() and b.p1.dim==3
		@endpy
	*/
	inline explicit Boxf(int dim)
	{
		reset(dim);
	}

	//!constructor di un box a partire da due punti
	/*! 
		Costruisce un box a partire dai punti p1 e p2 che devono
		avere dimensioni uguali. Internamente viene usata la add
		per cui non per forza deve essere p1.[xyz]<=p2.[xyz]

		\param p1 il primo punto
		\param p2 il secondo punto
		
		@py
		b=Boxf(Vecf(1, 0,0),Vecf(1, 1,1))
		assert b.p1.dim==2 and b.p1==Vecf(1, 0,0) and b.p2==Vecf(1, 1,1)
		@endpy
	*/
	inline explicit Boxf(Vecf p1,Vecf p2)
	{
		DebugAssert(p1.dim==p2.dim);
		reset(p1.dim);
		add(p1);
		add(p2);
	}

	//!constructor di un box con dimensione dim e con limiti [from,to] per tutti gli ass
	/*! 
		Costruisce un box in dim-D. Deve essere from<=to altrimenti
		viene generata una eccezione

		\param dim la dimensione desiderata
		\param from il limite inferiore
		\param to il limite superiore

		@py
		b=Boxf(3,0,1)
		assert b.p1.dim==3 and b.p1==Vecf(1, 0,0,0) and b.p2==Vecf(1, 1,1,1)
		@endpy
	*/
	inline explicit Boxf(int dim,float from,float to)
	{
		DebugAssert(from<to);
		reset(dim);
		Vecf p1(dim);p1.set(from);p1.set(0,1.0f);add(p1);
		Vecf p2(dim);p2.set(to  );p2.set(0,1.0f);add(p2);
	}

	//! reset di un box
	/*! 
		Rende il box non valido. Funzione utile quando
		ad esempio si vogliono reimpostare i limiti

		\param dim la dimensione voluta che non per forza deve essere
		           uguale alla dimensione corrente
	
		@py
		b=Boxf(3,0,1); b.reset(2)
		assert b.p1.dim==2 and not b.isValid()
		@endpy
	*/
	inline void reset(int dim)
	{
		DebugAssert(dim>=0);
		p1=Vecf(dim);p1.set(+FLT_MAX);p1.set(0,1.0f); //they are points
		p2=Vecf(dim);p2.set(-FLT_MAX);p2.set(0,1.0f);
	}

	//!reset di un box
	/*! 
		Rende il box non valido. Funzione utile quando
		ad esempio si vogliono reimpostare i limiti. La dimensione
		, al contrario della Boxf::resize(int), rimane costante.

		@pyb=Boxf(3,0,1)
		b.reset(); assert b.p1.dim==3 and not b.isValid()
		@endpy
	*/
	inline void reset()
	{
		reset(this->p1.dim);
	}

	//! aggiunge un vertice
	/*! 
		Aggiunge il vertice v al box attuale causando l'estensione,
		se necessario, dei limiti p1 e p1
		
		\param v il vertice da aggiungere (che chiaramente deve avere
		la prima componente omogenea pari a 1)

		@py
		b=Boxf(3)
		b.add(Vecf(1,2,2,2))
		assert b.isValid() and b.p1==Vecf(1,2,2,2) and b.p2==b.p1
		@endpy
	*/
	inline void add(const Vecf& v)
	{
		DebugAssert(v.dim==p1.dim);
		this->p1 = this->p1.Min(v);
		this->p2 = this->p2.Max(v);
	}

	//! aggiunge un box al box attuale
	/*! 
		Estende i limiti del box attuale utilizzando il box dato come argomento

		\param other il box che estende il box attuale, che deve avere dimensioni compatibili

		@py
		b=Boxf(3)
		b.add(Boxf(3,0,1))
		assert b.isValid() and b.p1==Vecf(1,0,0,0) and b.p2==Vecf(1,1,1,1)
		@endpy
	*/
	inline void add(const Boxf& other)
	{
		if (!other.isValid()) return;
		this->add(other.p1);
		this->add(other.p2);
	}

	//! testa la validita' del box

	/// 
	/*!
		Testa se il bounding box corrente e' valido cioe' se entrambe le condizioni sono soddisfatte

		<ul>
			<li>ogni coordinata di p1 e p2 ha valore finito
			<li>p1[i]<=p2[i] con i nel range [0,Boxf::dim()]
		</ul>

		@py
		b=Boxf(3)
		assert not b.isValid()
		@endpy
	*/
	inline bool isValid() const
	{
		DebugAssert(p1.dim==p2.dim);
		DebugAssert(p1[0]==1 && p2[0]==1);//should be points

		for (int i=1;i<=p1.dim;i++)
		{
			if (isnan(p1[i]) || !finite(p1[i])) return false;
			if (this->p1[i]   >   this->p2[i] ) return false;
		}
		return true;
	}

	//! torna il punto al centro del box
	/*! 
		Se il box non e' valido viene generata una eccezione

		\return il punto centrale del box

		@py
		b=Boxf(3,0,1)
		assert b.center()==Vecf(1,0.5,0.5,0.5)
		@endpy
	*/
	inline Vecf center() const
	{
		DebugAssert(isValid());
		Vecf ret(p1.dim);
		ret.set(0,1.0f); //is a point
		for (int i=1;i<=p1.dim;i++) 
			ret.mem[i]=0.5f*(p1[i]+p2[i]);
		return ret;
	}

	//! return the dimension 
	/*!
		Torna la dimensione del box

		\return la dimensione del box

		@py
		b=Boxf(3,0,1)
		assert b.dim()==3
		@endpy
	*/
	inline int dim() const
	{
		return p1.dim;
	}

	//! ritorna le dimensioni del bounding box
	/*! 
		\return il vettore in uscita NOTA: ha la prima componente  pari a 0 (essendo un vettore e non un punto)

		@py
		b=Boxf(3,0,1)
		assert b.size()==Vecf(0, 1,1,1)
		@endpy
	*/
	inline Vecf size() const
	{
		//the returned value is a vector (first position should be zero)
		if (!isValid()) return Vecf(p1.dim); //all zeros
		Vecf ret=(p2-p1);
		DebugAssert(ret[0]==0);
		return ret;
	}

	//! indice della coordinata con dimensione minima
	/*! 
		\return indice della coordinata con dimensione minima (esclusa lo '0' perche' e' la componente omogenea)

		@py
		b=Boxf(Vecf(1,0,0,0),Vecf(1,1,2,3))
		assert b.minsizeidx()==1
		@endpy
	*/
	inline int minsizeidx() const
	{
		DebugAssert(isValid());
		Vecf d=size();
		int ret=0; //invalid
		for (int i=1;i<=p1.dim;i++) if (!ret || (d[i]<d[ret] || (d[i]==d[ret] && i>ret))) ret=i;
		return ret;
	}

	//! indice della coordinata con dimensione massima
	/*! 
		\return indice della coordinata con dimensione minima (esclusa lo '0' perche' e' la componente omogenea)

		@py
		b=Boxf(Vecf(1,0,0,0),Vecf(1,1,2,3))
		assert b.maxsizeidx()==3
		@endpy
	*/
	inline int maxsizeidx() const
	{
		DebugAssert(isValid());
		Vecf d=size();
		int ret=0; //invalid
		for (int i=1;i<=p1.dim;i++) if (!ret || (d[i]>d[ret] || (d[i]==d[ret] && i<ret))) ret=i;
		return ret;
	}

	//! minima dimensione
	/*! 
		\return minima dimensione (esclusa lo '0' perche' e' la componente omogenea)

		@py
		b=Boxf(Vecf(1,0,0,0),Vecf(1,1,2,3))
		assert b.minsize()==1
		@endpy
	*/
	inline float minsize() const
		{return size()[minsizeidx()];}

	///massima dimensione
	/*! 
		\return minima dimensione (esclusa lo '0' perche' e' la componente omogenea)

		@pyb=Boxf(Vecf(1,0,0,0),Vecf(1,1,2,3))
		assert b.maxsize()==3
		@endpy
	*/
	inline float maxsize() const
		{return size()[maxsizeidx()];}


	//! test di uguaglianza in senso stretto
	/*! 
		\param other l'altro box rispetto cui verificare uguaglianza
		\return true se entrambi i box sono validi e sono uguali, altrimenti false

		@py
		b=Boxf(Vecf(1,0,0,0),Vecf(1,1,2,3))
		assert b==Boxf(Vecf(1,0,0,0),Vecf(1,1,2,3))
		b=Boxf(Vecf(1,0,0,0),Vecf(1,1,2,3))
		assert b!=Boxf(Vecf(1,0,0,0),Vecf(1,1,2,4))\
		@endpy
	*/
	inline bool operator==(const Boxf& other) const
	{
		if (!other.isValid() || !this->isValid())
			return false;

		if (other.dim()!=this->dim())
			return false;

		return other.p1==this->p1 && other.p2==this->p2;
	}

	//! testa l'uguaglianza in senso fuzzy (cioe' a meno di un certo errore)
	/*! 
		\param other l'altro box rispetto cui verificare uguaglianza
		\return true se entrambi i box sono validi e sono uguali a meno di un certo errore, altrimenti false

		@py
		b=Boxf(Vecf(1,0,0,0),Vecf(1,1,2,3))
		assert b.fuzzyEqual(Boxf(Vecf(1,0,0,0),Vecf(1,1+1e-8,2+1e-8,3+1e-8)))
		@endpy
	*/
	inline bool fuzzyEqual(const Boxf& other,float Epsilon=0.001f) const
	{
		if (!other.isValid() || !this->isValid())
			return false;

		if (other.dim()!=this->dim())
			return false;

		return other.p1.fuzzyEqual(this->p1,Epsilon) && other.p2.fuzzyEqual(this->p2,Epsilon);
	}

	//! scala il bounding box
	/*! 
		Ad esempio un valore percent di 2.0 ne raddoppia le dimensioni

		\param percent percentuale di scalamento
		\retun il box scalato

		@py
		b=Boxf(Vecf(1,-1,-1,-1),Vecf(1,+1,+1,+1)).scale(2)
		assert b.p1==Vecf(1.0,-2,-2,-2) and b.p2==Vecf(1.0,+2,+2,+2)
		@endpy
	*/
	inline Boxf scale(float percent) const
	{
		DebugAssert(isValid());
		Vecf c=center();
		Vecf new_half_size=size()*(percent*0.5f);
		return Boxf(c-new_half_size,c+new_half_size);
	}


	//! calcola il volume
	/*! 
		\return il volume del box

		@py
		b=Boxf(Vecf(1,-1,-1,-1),Vecf(1,+1,+1,+1))
		assert b.volume()==8
		@endpy
	*/
	inline float volume() const
	{
		if (!isValid()) return 0;
		Vecf d=size();
		float ret=1;
		for (int i=1;i<=p1.dim;i++) ret*=d[i];
		return ret;
	}

	//!  testa se due bounding box si sovrappongono
	/*! 
		\param other altro box rispetto a cui testare l'overlapping
		\return true se entrambi i box sono validi e overlapping, altrimenti false

		@py
		b=Boxf(3,0,1)
		assert b.overlap(Boxf(3,0.5,1.5)) and not b.overlap(Boxf(3,1.5,2.5))
		@endpy
	*/
	inline bool overlap(const Boxf& other) const
	{
		if (!isValid() || !other.isValid() || other.dim()!=this->dim())
			return false;

		for (int I=1;I<=p1.dim;I++) 
		{
			if (!Utils::Overlap1d(p1[I],p2[I],other.p1[I],other.p2[I])) 
				return false;
		}
		return true;
	}

	//! contains a point
	/*! 
		Testa se il box contiene il punto. Se il box non e' valido
		o le dimensioni del box e del vertice non sono compatibili
		viene generata una eccezione

		\param p il vertice da testare
		\return true se il box contiene p altrimenti false

		@py
		b=Boxf(3,0,1)
		assert b.contains(Vecf(1,0.5,0.5,0.5))
		@endpy
	*/
	inline bool contains(const Vecf& p) const
	{
		DebugAssert(isValid() && p.dim==this->p1.dim);
		for (int I=1;I<=p1.dim;I++) if (p[I]<this->p1[I] || p[I]>this->p2[I]) return false;
		return true;
	}

	//! converte un box in n-dim in un box in 3-dim
	/*! 
		Funzione di conversione

		\return il box in 3-dim (#ref Box3f)

		@py
		b=Boxf(3,0,1).toBox3f()
		assert b.p1==Vec3f(0,0,0) and b.p2==Vec3f(1,1,1)
		@endpy
	*/
	inline Box3f toBox3f() const
	{
		return Box3f(
			Vec3f(this->p1.dim>=1?p1[1]:0,this->p1.dim>=2?p1[2]:0.0f,this->p1.dim>=3?p1[3]:0),
			Vec3f(this->p1.dim>=1?p2[1]:0,this->p1.dim>=2?p2[2]:0.0f,this->p1.dim>=3?p2[3]:0));
	}

	//! test if the plane leave completely! the bounding box in the above space 
	/*! 
		@py 
		h=Planef([0,0,0,1])
		assert Boxf(Vecf(1,0,0,2),Vecf(1,1,1,3)).isAbove(h)
		@endpy
	*/
	inline bool isAbove(const Planef& h) const
	{
		const Boxf& aabb=*this;
		DebugAssert(aabb.isValid() && aabb.dim()==h.dim);
		float acc=h.mem[0];
		for (int I=1;I<=h.dim;I++) acc+=h.mem[I]*(h.mem[I]>=0?aabb.p1[I]:aabb.p2[I]);
		return (acc>=0);
	}

	//! test if the plane leave completely! the bounding box in the below space
	/*! 
		@py 
		h=Planef([0,0,0,1])
		assert Boxf(Vecf(1,0,0,-3),Vecf(1,1,1,-2)).isBelow(h)
		@endpy
	*/
	inline bool isBelow(const Planef& h) const
	{
		const Boxf& aabb=*this;
		DebugAssert(aabb.isValid() && aabb.dim()==h.dim);
		float acc=h.mem[0];
		for (int I=1;I<=h.dim;I++) acc+=h.mem[I]*(h.mem[I]<=0?aabb.p1[I]:aabb.p2[I]);
		return (acc<=0);
	}


	//! python repr
	/*! 
		\return la python repr() dell'ogggetto

		@pyb=Boxf(3,0,1)
		assert(eval(repr(b)))==Boxf(3,0,1)
		@endpy
	*/
	inline std::string repr() const
	{
		std::string ret="Boxf(Vecf([";
		for (int i=0;i<=p1.dim;i++) ret+=(i?",":"")+Utils::Format("%e",p1[i]);
		ret+="]),Vecf([";
		for (int i=0;i<=p2.dim;i++) ret+=(i?",":"")+Utils::Format("%e ",p2[i]);
		ret+="]))";
		return ret;
	}


	//! python str
	/*! 
		\return la python str() dell'oggetto

		@py
		b=Boxf(3,0,1)
		assert eval(str(b))==[[1,0,0,0],[1,1,1,1]]
		@endpy
	*/
	inline std::string str() const
	{
		std::string ret="[[";
		for (int i=0;i<=p1.dim;i++) ret+=(i?",":"")+Utils::Format("%f ",p1[i]);
		ret+="],[";
		for (int i=0;i<=p2.dim;i++) ret+=(i?",":"")+Utils::Format("%f ",p2[i]);
		ret+="]]";
		return ret;
	}


	///self testing
	static int SelfTest();

};

#endif //_BOX_H__


