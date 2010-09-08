#ifndef ___VECTOR_H__
#define ___VECTOR_H__

#include <xge/xge.h>
#include <xge/mempool.h>
#include <xge/archive.h>

//predeclaration
class EngineResource;

//===========================================================================
//! Classe per la memorizzazione di array di floats
/*!

Classe per la gestione degli array di tipo float.\n

E' l'equivalente di std::vector<float> tuttavia e' 
piu' ottimizzato rispetto ad allocazioni/deallocazioni
e realloc-azioni di memoria, cosi' come per gli append.\n

I suoi costruttori garantiscono la perfetta equivalenza con
STL std::vector<float> e quindi anche con le Python list ([1.0,...]).\n

Un Vector puo' essere interamente scaricato su GPU
e quindi, in fase di visualizzazione , puo' essere piu' veloce
nel rendering ad esempio di set di triangoli.\n

*/
//===========================================================================

class  XGE_API Vector
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

protected:

	//! numero di elementi nell'array
	int  num;
	
	//!  puntatore alla locazione di memoria che contiene gli elementi. 
	/*! 
		La memorial viene allocata usando la MemPool::alloc() o la MemPool::getSingleton()->realloc() e quindi
		deve! essere deallocata usando la MemPool::getSingleton()->free()
	*/
	float* pointer;

public:

	//! id del buffer  su scheda grafica. 
	/*! Se gpu non e' nullo vuol dire che l'array e' presente sulla RAM della GPU */
	SmartPointer<EngineResource> gpu;


	//! default constructor. Costruisce un Vector di zero elementi 
	/*!
		@cpp
		Vector v;
		assert(!v.size());
		@endcpp

		@py
		v=Vector() 
		assert v.size()==0
		@endpy
	*/
	inline Vector()
	{
		this->num=0;
		this->pointer=0;
		this->gpu.reset();
	}

	//!copy constructor
	/*!
		@py
		import copy
		src=Vector([1,2,3])
		dst=copy.copy(src)
		assert dst.size()==3 and dst[0]==1 and dst[1]==2 and dst[2]==3  # risultato [1,2,3]
		@endpy
	*/
	//>>>
	inline Vector(const Vector& src) 
	{
		this->num=src.num;
		this->pointer=0;
		this->gpu.reset();

		if (!num)  return;
		pointer=(float*)MemPool::getSingleton()->malloc(sizeof(float)*num);
		memcpy(pointer,src.pointer,sizeof(float)*num);
	}


	//! constructor
	/*!
		\param num il numero di elementi da allocare per l'array
		
		Inizializza un array con un certo numero di elementi tutti
		inizializzati a zero (tramite memset())

		@cpp
		Vector v(3);
		assert(v.size()==3 and !v[0] && !v[1] && !v[2]); //risultato [0,0,0]
		@endcpp

		@py
		v=Vector(3)
		assert v.size()==3 and v[0]==0 and v[1]==0 and v[2]==0 # risultato [0,0,0]
		@endpy
	*/
	//>>>
	inline explicit Vector(int num) 
	{
		this->num=num;
		this->pointer=0;
		this->gpu.reset();

		if (!num) 
			return;
		
		this->pointer=(float*)MemPool::getSingleton()->malloc(sizeof(float)*num);
		memset(this->pointer,0,sizeof(float)*num);
	}

	//!constructor
	/*!
		\param num il numero di elementi totali dell'array
		\param src locazione di memoria dalla quale verranno inizializzati gli elementi (tramite memcpy())

		@cpp
		float src[]={1,2,3};
		Vector v(3,src);
		assert(v.size()==3 and v[0]==1 && v[1]==2 && v[2]==3); //risultato [1,2,3]
		@endcpp

	*/
	inline explicit Vector(int num,const float* src) 
	{
		this->num=num;
		this->pointer=0;
		this->gpu.reset();

		if (!num) 
			return;
		
		this->pointer=(float*)MemPool::getSingleton()->malloc(sizeof(float)*num);
		
		if (src) 
			memcpy(this->pointer,src,sizeof(float)*num);
		else
			memset(this->pointer,0,sizeof(float)*num);
	}



	//!constructor from STL vector
	/*!
		\param src e' il vettore STL di float

		@cpp
		std::vector<float> src;//[1,2,3]
		src.push_back(1);
		src.push_back(2);
		src.push_back(3);
		Vector v(src);
		assert (v.size()==3 && v[0]==1 && v[1]==2 && v[2]==3); //risultato [1,2,3]
		@endcpp

		@py
		v=Vector([1,2,3])
		assert v.size()==3 and v[0]==1 and v[1]==2 and v[2]==3 # risultato [1,2,3]
		@endpy
	*/
	//>>>
	inline explicit Vector(const std::vector<float>& src)
	{
		this->num=(int)src.size();
		this->pointer=0;
		this->gpu.reset();

		if (!num) 
			return;

		pointer=(float*)MemPool::getSingleton()->malloc(sizeof(float)*num);

		for (int i=0;i<num;i++) 
			pointer[i]=src[i];
	}

	//! constructor from STL indices and buffer
	/*!
		\param indices STL std:vector<int> che contiene gli indici che referenziano il buffer
		\param buffer  STL std::vector<float> che contiene tutti i floats referenziati da indices

		@cpp
		std::vector<int> indices; //[0,1,4,5]
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(4);
		indices.push_back(5);
		std::vector<float> buffer; //[0,1,2,3,4,5]]
		for (int i=0;i<=5;i++) buffer.push_back((float)i);
		Vector v(indices,buffer);
		assert(v.size()==4 && v[0]==0 && v[1]==1 && v[2]==4 && v[3]==5); //risultato [0,1,4,5]
		@endcpp

		@py
		v=Vector([0,1,4,5],[0,1,2,3,4,5]) 
		assert v.size()==4 and v[0]==0 and v[1]==1 and v[2]==4 and v[3]==5 # risultato [0,1,4,5]
		@endpy

	*/
	//
	inline explicit Vector(const std::vector<int>& indices,const std::vector<float>& buffer)
	{
		this->num=(int)indices.size();
		this->pointer=0;
		this->gpu.reset();

		if (!num) 
			return;

		this->pointer=(float*)MemPool::getSingleton()->malloc(sizeof(float)*num);

		int N=0;

		for (std::vector<int>::const_iterator it=indices.begin();it!=indices.end();it++) 
			pointer[N++]=buffer[*it];
	}

	//!constructor from STL std::vector<int> e float*
	/*!
		\param indices STL std:vector<int> che contiene gli indici che referenziano il buffer
		\param buffer  locazione di memoria che contiene tutti i floats referenziati da indices

		@cpp
		std::vector<int> indices; //[0,1,4,5]
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(4);
		indices.push_back(5);
		float buffer[]={0,1,2,3,4,5};
		Vector v(indices,buffer);
		assert(v.size()==4 && v[0]==0 && v[1]==1 && v[2]==4 && v[3]==5); //risultato [0,1,4,5]
		@endcpp
	*/
	inline explicit Vector(const std::vector<int>& indices,const float* buffer)
	{
		this->num=(int)indices.size();
		this->pointer=0;
		this->gpu.reset();

		if (!num) 
			return;

		this->pointer=(float*)MemPool::getSingleton()->malloc(sizeof(float)*num);

		int N=0;
		for (std::vector<int>::const_iterator it=indices.begin();it!=indices.end();it++) 
			pointer[N++]=buffer[*it];
	}


	//! destructor
	~Vector();

	

	//!assignment operator
	/*!
		\param src il buffer sorgente dal quale copiare

		@cpp
		float buffer[]={10,11,12};
		Vector v;
		v=Vector(3,buffer); 
		assert(v.size()==3 && v[0]==10 && v[1]==11 && v[2]==12);
		@endcpp

		@py
		v=Vector([0,1,2])
		v.assign(Vector([3,4]))
		assert v.size()==2 and v[0]==3 and v[1]==4
		@endpy	
	*/
	inline Vector& operator=(const Vector& src)
	{
		//useless call
		if (&src==this) return *this;

		if (pointer)  MemPool::getSingleton()->free(sizeof(float)*num,pointer);

		this->num=(int)src.size();
		this->pointer=0;
		this->gpu.reset();  //do not share gpu buffer

		if (!num) return *this;
		this->pointer=(float*)MemPool::getSingleton()->malloc(sizeof(float)*num);
		memcpy(this->pointer,src.pointer,sizeof(float)*num);
		return *this;
	}

	//! assignment from STL std::vector<float>
	/*!
		\param src STL std::vector<float> dal quale copiare

		@cpp
		std::vector<float> src;
		src.push_back(3);
		src.push_back(4);
		Vector v;
		v=src;
		assert(v.size()==2 && v[0]==3 && v[1]==4);
		@endcpp

		@py
		v=Vector([0,1,2])
		v.assign([3,4])
		assert v.size()==2 and v[0]==3 and v[1]==4
		@endpy

	*/
	inline Vector& operator=(const std::vector<float>& src)
	{
		if (pointer)  MemPool::getSingleton()->free(sizeof(float)*num,pointer);
		

		this->num=(int)src.size();
		this->pointer=0;
		this->gpu.reset(); //do not share gpu buffer

		if (!num) return *this;
		this->pointer=(float*)MemPool::getSingleton()->malloc(sizeof(float)*num);
		memcpy(this->pointer,&src[0],sizeof(float)*num);
		return *this;
	}



	//! return the size of the array
	/*!
		\return la dimensione dell'array

		@cpp
		Vector v(3);
		assert(v.size()==3);
		@endcpp

		@py
		assert Vector(3).size()==3 
		@endpy
	*/
	inline int size() const
		{return num;}

	//! memory size
	/*!
		\return la dimensione della memoria occupata dall'array, pari a sizeof(float)*Vector::size()

		@cpp
		Vector v(1);
		assert(v.memsize()==sizeof(float)*v.size());
		@endcpp

		@py
		assert Vector([0]).memsize()==4
		@endpy
	*/
	inline int memsize() const
		{return num*sizeof(float);}

	//! pointer to the memory
	/*!
		\return il puntatore alla locazione di memoria

		@cpp
		Vector v(1);
		assert(v.mem()==&v[0]);
		@endcpp
	*/
	inline float* mem()
		{return this->pointer;}


	//! resize (with side effect)
	/*!
		\param new_size la nuova dimensione dell'array

		@cpp
		float buffer_v1[]={0,1,2};
		Vector v1(3,buffer_v1);
		v1.resize(2);
		assert(v1.size()==2 && v1[0]==0 && v1[1]==1);

		float buffer_v2[]={0,1,2};
		Vector v2(3,buffer_v2);
		v2.resize(4);
		assert(v2.size()==4 && v2[0]==0 && v2[1]==1 && v2[2]==2);
		@endcpp

		@py
		v=Vector([0,1,2])
		v.resize(2)
		assert v.size()==2 and v[0]==0 and v[1]==1

		v=Vector([0,1,2])
		v.resize(4)
		assert v.size()==4 and v[0]==0 and v[1]==1 and v[2]==2

		@endpy
	*/
	inline void resize(int new_size)
	{
		this->pointer=(float*)MemPool::getSingleton()->realloc(sizeof(float)*num,this->pointer,sizeof(float)*new_size);
		this->num=new_size;
	}

	//! append another array (side effect)
	/*!
		\param to_append l'altro array da aggiungere alla fine

		@cpp
		float buffer_v1[]={0};Vector v1(1,buffer_v1);
		float buffer_v2[]={1};Vector v2(1,buffer_v2);
		v1.append(v2);
		assert(v1.size()==2 && v1[0]==0 & v1[1]==1);
		@endcpp

		@py
		v1=Vector([0])
		v2=Vector([1])
		v1.append(v2)
		assert v1.size()==2 and v1[0]==0 and v1[1]==1
		@endpy
	*/
	inline void append(const Vector& to_append)
	{
		if (!to_append.size())
			return;

		int old_size=this->size();
		int new_size=old_size+to_append.size();
		this->resize(new_size);
		memcpy(this->pointer+old_size,to_append.pointer,sizeof(float)*to_append.size());
	}

	//! set memory to zero (side effect)
	/*!
		Setta tutti gli elementi a zero

		@cpp
		Vector v(2);v[0]=1;v[1]=2;
		v.zero();
		assert(v.size()==2 and !v[0] and !v[1);
		@endcpp

		@py
		v=Vector([1,2])
		v.zero()
		assert v.size()==2 and v[0]==0 and v[1]==0
		@endpy
	*/
	inline void zero()
		{if (pointer) memset(this->pointer,0,memsize());}

	//! test equality operator
	/*!
		\param src L'altro array rispetto al quale effettuare il confronto
		
		@cpp
		float buffer[]={1,2,3};
		assert(Vector(3,buffer)==Vector(3,buffer));
		@endcpp

		@py
		assert Vector([1,2,3])==Vector([1,2,3])
		@endpy
	*/
	inline bool operator==(const Vector& src) const
	{
		if (this->num!=src.num)
			return false;

		if (!this->num)
			return true;

		return memcmp(this->pointer,src.pointer,this->memsize())==0;
	}


	//! disequality operator
	/*!
		\param src L'altro array rispetto al quale effettuare il confronto

		@cpp
		float buffer1[]={1,2,3};
		float buffer2[]={3,4,5};
		assert(Vector(3,buffer1)!=Vector(3,buffer1));
		@endcpp

		@py
		assert Vector([1,2,3])!=Vector([3,4,5])
		@endpy
	*/
	inline bool operator!=(const Vector& src) const
	{
		return !(this->operator==(src));
	}

	//! get
	float get(int i) const
	{
		if (i<0 || i>=this->num)
			Utils::Error(HERE,"float& Vector::operator[](int i) argument i=%d ouside valid range [0..%d]",i,this->num-1);

		return pointer[i];
	}

	//!set
	inline void set(int i,float value) 
	{
		if (i<0 || i>=this->num)
			Utils::Error(HERE,"void Vector::set(int i,float value) argument i=%d ouside valid range [0..%d]",i,this->num-1);

		pointer[i]=value;
	}

	//! access operator by []
	/*!
		\param i l'indice dell'elemento da tornare (se fuori range genera una eccezione)

		@cpp
		float buffer[]={1,2};
		Vector v(2,buffer);
		assert(v.size()==2 and v[0]==1 and v[1]==2);
		@endcpp

		@py
		v=Vector([1,2])
		assert v.size()==2 and v[0]==1 and v[1]==2
		@endpy
	*/
	inline float& operator[](int i)
	{
		if (i<0 || i>=this->num)
			Utils::Error(HERE,"float& Vector::operator[](int i) argument i=%d ouside valid range [0..%d]",i,this->num-1);

		return pointer[i];
	
	}

	//! const access operator by []
	inline const float& operator[](int i) const
	{
		if (i<0 || i>=this->num)
			Utils::Error(HERE,"const float Vector::operator[](int i) argument i=%d ouside valid range [0..%d]",i,this->num-1);

		return pointer[i];
	}

	//!special function to extract values by indices (no side effect)
	/*!
		\param indices to extract

		@cpp
		float buffer[]={1,2,3,4};
		Vector v(4,buffer);
		std::vector indices;
		indices.push_back(2);
		indices.push_back(3);
		v=v.extract(indices);
		assert(v.size()==2 and v[0]==3 and v[1]==4);
		@endcpp
	
		@py
		v=Vector([1,2,3,4]).extract([2,3])
		assert v.size()==2 and v[0]==3 and v[1]==4
		@endpy
	*/
	Vector extract(const std::vector<int>& indices,bool bComplement)
	{
		//all the indices not in _indices
		if (bComplement)
		{
			std::vector<int> complement;

			for (int I=0;I<this->num;I++)
			{
				if (!Utils::Contains(indices,I))
					complement.push_back(I);
			}

			return Vector(complement,this->pointer);
		}
		else
		{
			return Vector(indices,this->pointer);
		}
	}

	//! extract utility [from,to)
	Vector extract(int From, int To,bool bComplement)
	{
		std::vector<int> indices;

		for (int I=From;I<To;I++)
			indices.push_back(I);

		return extract(indices,bComplement);
	}

	//! parse float a string (for internal use only)
	static void parse(int num,float* dest,const char* source,char* _format="%e");

	//! get stl vector
	std::vector<float> c_vector() const
	{
		std::vector<float> ret(this->num);
		for (int i=0;i<num;i++)
			ret[i]=(*this)[i];
		return ret;
	}

	//! funzione equivalentea alla python repr
	/*!
	
		@py
		v=Vector([1,2,3,4])
		assert(eval(repr(v)))==Vector([1,2,3,4])
		@endpy
	*/
	inline std::string repr() const
	{
		std::string ret="Vector([";
		for (int i=0;i<this->num;i++)
		{
			if (i) ret+=",";
			ret+=Utils::Format("%e",this->pointer[i]);
		}
		ret+="])";
		return ret;
	}

	//! funzione equivalente alla python str
	/*!
		@py
		v=Vector([1,2,3,4])
		assert eval(str(v))==[1,2,3,4]
		@endpy
	*/
	inline std::string str() const
	{
		std::string ret="[";
		for (int i=0;i<num;i++)
		{
			if (i) ret+=",";
			ret+=Utils::Format("%f",this->pointer[i]);
		}
		ret+="]";
		return ret;
	}

	//self test
	static int SelfTest();

}; //end class



#endif  //___VECTOR_H__


