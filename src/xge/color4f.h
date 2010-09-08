#ifndef _COLOR4F_H__
#define _COLOR4F_H__

#include <xge/xge.h>
#include <xge/archive.h>

//=================================================================
//! Classe per la memorizzazione dei colori 
//=================================================================
class  XGE_API Color4f
{
public:
	void Write(Archive& ar)
	{
		ar.WriteFloat("r",this->r);
		ar.WriteFloat("g",this->g);
		ar.WriteFloat("b",this->b);
		ar.WriteFloat("a",this->a);

	}

	void Read(Archive& ar)
	{
		this->r=ar.ReadFloat("r");
		this->g=ar.ReadFloat("g");
		this->b=ar.ReadFloat("b");
		this->a=ar.ReadFloat("a");
	}

public:
	//!red component in the range [0,1]
	float r;

	//!green component in the range [0,1]
	float g;
	
	//!blue component in the range [0,1]
	float b;
	
	//!alpha component in the range [0,1]
	float a;

	//! default constuctor
	inline Color4f() :r(0.5f),g(0.5f),b(0.5f),a(1) 
		{}

	//!copy constructor
	/*!
		\param src il colore sorgente dal quale effettuare la copia

		@py
		c=Color4f(Color4f(0.2,0.3,0.4,1))
		assert fabs(c.r-0.2)<1e-4 and fabs(c.g-0.3)<1e-4 and fabs(c.b-0.4)<1e-4 and c.a==1.0
		@endpy
	*/
	inline Color4f(const Color4f& src) :r(src.r),g(src.g),b(src.b),a(src.a) 
		{}

	//! constructor dalle 4 componenti di color RGBA
	/*!
		\param R red component in the range [0,1]
		\param G green component in the range [0,1]
		\param B blue component in the range [0,1]
		\param A alpha component in the range [0,1]

	
		@py
		c=Color4f(0.2,0.3,0.4,1)
		assert fabs(c.r-0.2)<1e-4 and fabs(c.g-0.3)<1e-4 and fabs(c.b-0.4)<1e-4 and c.a==1.0
		@endpy
	*/
	inline explicit Color4f(float R,float G,float B,float A=1.0) :r(R),g(G),b(B),a(A) 
		{}


	//! constructor from STL std::vector<float>
	/*!
		\param src il vettore STL che deve avere dimensioni 3 (RGB) o 4 (RGBA)

		@py
		c=Color4f([0.2,0.3,0.4])
		assert fabs(c.r-0.2)<1e-4 and fabs(c.g-0.3)<1e-4 and fabs(c.b-0.4)<1e-4 and c.a==1.0

		c=Color4f([0.2,0.3,0.4,0.8])
		assert fabs(c.r-0.2)<1e-4 and fabs(c.g-0.3)<1e-4 and fabs(c.b-0.4)<1e-4 and fabs(c.a-0.8)<1e-4
		@endpy
	*/
	inline explicit Color4f(const std::vector<float>& src)
	{
		if (src.size()!=3 && src.size()!=4)
			Utils::Error(HERE,"Color4f(const std::vector<float>& src) invalid argument src, src.size()=%d but it's size must be 3 or 4",(int)src.size());

		r=Utils::Clampf(src[0],0,1);
		g=Utils::Clampf(src[1],0,1);
		b=Utils::Clampf(src[2],0,1);
		a=src.size()==4?Utils::Clampf(src[3],0,1):1.0f;
	}

	static inline Color4f Black () {return Color4f(0,0,0,1);}
	static inline Color4f White () {return Color4f(1,1,1,1);}
	static inline Color4f Red   () {return Color4f(1,0,0,1);}
	static inline Color4f Green () {return Color4f(0,1,0,1);}
	static inline Color4f Blue  () {return Color4f(0,0,1,1);}
	static inline Color4f Yellow() {return Color4f(1,1,0,1);}

	//! constructor from float*
	/*!
		\param color il puntatore a un array di 4 componenti RBGA
	*/
	inline explicit Color4f(float* color) :r(Utils::Clampf(color[0],0,1)),g(Utils::Clampf(color[1],0,1)),b(Utils::Clampf(color[2],0,1)),a(Utils::Clampf(color[3],0,1))
		{}


	//! get di una componente
	/*!
		\param i la componente da restituire (i deve essere nel range [0,3] corrispondenti a RGBA
		\return il valore (che e' nel range [0,1])
	
		@py
		v=Color4f(1,1,1)
		v.set(1,0)
		assert v.get(0)==1 and v.get(1)==0 and v.get(2)==1
		@endpy
	*/
	inline float get(int i)
	{
		if (i<0 || i>3)
			Utils::Error(HERE,"float Color4f::get(int i) argument i=%d ouside valid range [0..3]",i);

		if      (i==0) return r;
		else if (i==1) return g;
		else if (i==2) return b;
		return a;
	}


	//! set di una componente
	/*!
		\param i la componente da settare (i deve essere nel range [0,3] corrispondenti a RGBA
		\param value il nuovo valore (che deve essere nel range [0,1])
		
		@py
		v=Color4f(0.1,0.2,0.3,1.0)
		v.set(1,1)
		assert fabs(v.get(0)-0.1)<1e-4 and v.get(1)==1.0 and fabs(v.get(2)-0.3)<1e-4 and v.get(3)==1.0
		@endpy
	*/
	inline void set(int i,float value)
	{
		if (i<0 || i>3)
			Utils::Error(HERE,"void Color4f::set(int i,float value) argument i=%d ouside valid range [0..3]",i);

		if      (i==0) r=Utils::Clampf(value,0,1);
		else if (i==1) g=Utils::Clampf(value,0,1);
		else if (i==2) b=Utils::Clampf(value,0,1);
		else           a=Utils::Clampf(value,0,1);
	}



	//! access operator by []
	/*!
		\param i la componente da restituire (i deve essere nel range [0,3] corrispondenti a RGBA
		\return il valore (che e' nel range [0,1])

		@py
		c=Color4f(0.1,0.2,0.3,1.0)
		assert fabs(c[0]-0.1)<1e-4 and fabs(c[1]-0.2)<1e-4 and fabs(c[2]-0.3)<1e-4 and c[3]==1.0
		@endpy
	*/
	inline float& operator[](int i)
	{
		if (i<0 || i>3)
			Utils::Error(HERE,"float& Color4f::operator[](int i) argument i=%d ouside valid range [0..3]",i);

		if (i==0) return r;
		if (i==1) return g;
		if (i==2) return b;
		return a;
	}

	inline const float& operator[](int i) const
	{
		if (i<0 || i>3)
			Utils::Error(HERE,"float& Color4f::operator[](int i) argument i=%d ouside valid range [0..3]",i);

		if (i==0) return r;
		if (i==1) return g;
		if (i==2) return b;
		return a;
	}

	//! equality comparison
	/*!
		\param src altro colore rispetto cui fare il test
		\return true se i due colori sono uguali, altrimenti false
	
		@py
		assert Color4f(0.1,0.2,0.3,1.0)==Color4f(0.1,0.2,0.3,1.0) and Color4f(0.1,0.2,0.3,1.0)!=Color4f(0.1,0.2,0.4,1.0)
		@endpy
	*/
	inline bool operator==(const Color4f& src) const
	{
		return this->r==src.r && this->g==src.g && this->b==src.b && this->a==src.a;
	}

	inline bool operator!=(const Color4f& src) const
	{
		return this->r!=src.r || this->g!=src.g || this->b!=src.b || this->a!=src.a;
	}


	//! conversion to float*
	/*!
		\return il puntatore alla memoria
	*/
	//inline  operator const float*() const
	//	{return &r;}

	//! conversion to float*
	/*!
		\return il puntatore alla memoria
	*/
	//inline  operator float*() 
	//	{return &r;}

	//! scan from string (internal utility)
	inline static Color4f sscanf(const char* value)
		{Color4f ret;::sscanf(value,"%f %f %f %f",&ret.r,&ret.g,&ret.b,&ret.a);return ret;}
	
	//! random RGB color (alpha channel set to 1)
	/*!
		\return il colore random
	
		@py
		c=Color4f.randomRGB()
		assert c.a==1
		@endpy
	*/
	static inline Color4f randomRGB()
	{
		return Color4f(Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1));
	}

	//! get random RGBA color
	/*!
		\return il colore random

		@py
		c=Color4f.randomRGBA()
		assert c.a>=0 and c.a<=1
		@endpy
	*/
	static inline Color4f randomRGBA()
	{
		return Color4f(Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1));
	}


	//! python str()
	/*!
		\return la python str() del colore
	
		@py
		assert eval(str(Color4f(0.1,0.2,0.3,1.0)))==[0.1,0.2,0.3,1.0]
		@endpy
	*/
	std::string str() const
	{
		return Utils::Format("[%f,%f,%f,%f]",r,g,b,a);
	}

	//! python repr()
	/*!
		\return la python repr() del colore
		@py
		assert eval(repr(Color4f(0.1,0.2,0.3,1.0)))==Color4f(0.1,0.2,0.3,1.0)
		@endpy
	*/
	std::string repr() const
	{
		return Utils::Format("Color4f(%e,%e,%e,%e)",r,g,b,a);
	}

};



#endif  //_COLOR4F_H__


 