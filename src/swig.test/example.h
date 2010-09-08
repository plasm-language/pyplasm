#ifndef _EXAMPLE_H__
#define _EXAMPLE_H__


#ifdef EXAMPLE_EXPORTS
#ifdef WIN32
#define EXAMPLE_API __declspec(dllexport)
#else
#define EXAMPLE_API __attribute__((visibility("default")))
#endif
#else
#ifdef WIN32
#define EXAMPLE_API __declspec(dllimport)
#else
#define EXAMPLE_API
#endif
#endif //EXAMPLE_EXPORTS

#pragma warning (disable:4018) //signed/unsigned mismatch
#pragma warning (disable:4996) //'sprintf': This function or variable may be unsafe. Consider using sprintf_s instead
#pragma warning (disable:4251) //template needs to have dll-interface to be used by clients of class

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <stdarg.h>


#ifdef _WINDOWS
typedef __int64           			int64;
typedef unsigned __int64 			uint64;
#else
typedef signed long long int        int64;
typedef unsigned long long int      uint64;
#endif

typedef uint64 Address;

 
// I prefer to use SLT tr1 extension instead of the HUGE boost library (with slow compiling time)
#include <memory>
#define SmartPointer std::tr1::shared_ptr



/////////////////////////////////////////////////
//format a string
/////////////////////////////////////////////////

inline std::string FormatMsg(const char * format, ...)
{
	va_list argList;
	va_start(argList,format);
	char* tmp=0;

	#ifdef _WINDOWS
	int length = _vscprintf(format, argList);
	tmp = (char*)malloc(length+1);
	vsprintf(tmp, format, argList);
	tmp[length] = 0;
	#else
	vasprintf(&tmp,format,argList);
	#endif
	va_end( argList );
	std::string ret=std::string(tmp);
	free(tmp);
	return ret;
}

/////////////////////////////////////////////////////////////////////////////////
//esempio di classe da esportare con contatore delle istanze per fare il debugging
/////////////////////////////////////////////////////////////////////////////////

class EXAMPLE_API SwigClass 
{
public:

	//for debugging purpouse
	int value;
	
	//default constructor
 	SwigClass()                      ;
 	
 	//copy constructor
	SwigClass(const SwigClass& src)  ;
	
	//other type of constructor
	explicit SwigClass(int value)    ;
	
	//destructor
	~SwigClass()                     ;

	//for debugging
	static int num_nodes; 

	//return the address
	Address AddressOf();
};


/////////////////////////////////////////////////////////////////////////////////
//example of virtual class
/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API VirtualClass 
{
	int value;
	
public:
  VirtualClass();
  
  virtual ~VirtualClass();
  
  virtual void fn();
  
  int getValue();
  
  void setValue(int value);



};




///////////////////////////////////////////////////////////
//Another example for doing better tests
///////////////////////////////////////////////////////////
class EXAMPLE_API Vec3f
{
public:

	float x,y,z;
	
	Vec3f() ;
	
	Vec3f(const Vec3f& src);
	
	explicit Vec3f(float _x,float _y,float _z);

	//assignment
	Vec3f& operator=(const Vec3f& src);
	
	//equal operator
	bool operator==(const Vec3f& src);
	
	//test operators
	Vec3f  operator+ (Vec3f b);
	Vec3f  operator- (Vec3f b);
	float  operator* (Vec3f b);

	Vec3f  operator* (float s);
	Vec3f  operator/ (float s);

	Vec3f& operator+=(Vec3f v);
	Vec3f& operator-=(Vec3f v);

	Vec3f& operator*=(float s);
	Vec3f& operator/=(float s);

	std::string repr();
	
	std::string str();
	
}; //end class




///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class EXAMPLE_API CppArray
{
public:

	int    size;
	float* mem;
	
	//default constructor
	CppArray();
	
	//defaul destructor
	~CppArray();
	
	//assignment from array of floats
	CppArray(const std::vector<float>& src);
	
	//allocator
	CppArray(int size);
	
	CppArray(const CppArray& src);
	
	CppArray& operator=(const CppArray& src);

	bool operator==(const CppArray& src) const;
	
	void push_back(float value);
	
	std::vector<float> c_vector();
	
	float get(int idx) const;
	
	//reference operator
	float& operator[](int idx);

	//set a value
	void set(int idx,float value);
	
	//utils for debugging
	std::string repr();
	std::string str();

};

/////////////////////////////////////////////////////////////////////////////////
// AtomicType
/////////////////////////////////////////////////////////////////////////////////

class EXAMPLE_API VarAtomicType
{
public:

	bool           var_bool;
	char           var_char;
	unsigned char  var_unsigned_char;
	short          var_short;
	unsigned short var_unsigned_short;
	int            var_int;
	unsigned int   var_unsigned_int;
	long           var_long;
	unsigned long  var_unsigned_long;
	float          var_float;
	double         var_double;
	std::string    var_string;
	
	//default constructor
	VarAtomicType();

};


/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API VarSwigClass
{
public:
	SwigClass var;
};


/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API VarSmartPointerSwigClass
{
public:
	SmartPointer<SwigClass> var;
	VarSmartPointerSwigClass ();
};


/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API VarPointerSwigClass
{
public:
	VarPointerSwigClass () ;
	~VarPointerSwigClass();
	SwigClass* var;
};


/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API VarStdVectorInt
{
public:
	std::vector<int> var;
};

/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API VarPointerStdVectorInt
{
public:
	std::vector<int>* var;
	
	VarPointerStdVectorInt();
		
	~VarPointerStdVectorInt();
};


/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API VarStdVectorStdVectorInt
{
public:
	std::vector< std::vector<int> > var;	
};
	

/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API VarPointerStdVectorStdVectorInt
{
public:
	std::vector< std::vector<int> >* var;	
	
	VarPointerStdVectorStdVectorInt();
		
	~VarPointerStdVectorStdVectorInt();
	
};	
	
/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API VarStdVectorSwigClass
{
public:
	std::vector<SwigClass> var;
};


/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API VarPointerStdVectorSwigClass
{
public:
	std::vector<SwigClass>* var;
	
	VarPointerStdVectorSwigClass();
		
	~VarPointerStdVectorSwigClass();
	
};

/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API VarSmartPointerSwigClass_NoConstructor
{
public:
	SmartPointer<SwigClass> var;
};

/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API VarStdVectorSmartPointerSwigClass
{
public:
	std::vector<SmartPointer<SwigClass> > var;
};


/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API VarPointerStdVectorSmartPointerSwigClass
{
public:
	std::vector<SmartPointer<SwigClass> >* var;
	
	VarPointerStdVectorSmartPointerSwigClass();
		
	~VarPointerStdVectorSmartPointerSwigClass();
};


	
/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API VarStdVectorPointerSwigClass
{
public:
	std::vector<SwigClass*> var;
	
		VarStdVectorPointerSwigClass() ;
			
		~VarStdVectorPointerSwigClass();
};


/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API VarPointerStdVectorPointerSwigClass
{
public:
	std::vector<SwigClass*>* var;
	
		VarPointerStdVectorPointerSwigClass();
			
		~VarPointerStdVectorPointerSwigClass();
};


/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API InputAtomicType
{
public:
	InputAtomicType();
	int fn(int value);
};

/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API InputSwigClass
{
public:
	InputSwigClass();
	Address fn(SwigClass value);
};

/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API InputSmartPointerSwigClass
{
public:
	InputSmartPointerSwigClass();
	Address fn(SmartPointer<SwigClass> value);
};

/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API InputReferenceSwigClass
{
public:
	InputReferenceSwigClass();
	Address fn(SwigClass& value);
};


/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API InputPointerSwigClass
{
public:	
	InputPointerSwigClass();
	Address fn(SwigClass* value);
};


/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API InputStdVectorInt
{
public:
	void fn(std::vector<int> value);
};

/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API InputStdVectorStdVectorInt
{
public:
	void fn(std::vector<std::vector<int> > value);
};

/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API InputStdVectorSwigClass
{
public:
	void fn(std::vector<SwigClass> value);
};

/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API InputStdVectorSmartPointerSwigClass
{
public:
	void fn(std::vector< SmartPointer<SwigClass> >  value);
};
	
/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API InputStdVectorPointerSwigClass
{
public:
	void fn(std::vector<SwigClass*>  value);
};

/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API InputPointerStdVectorInt
{
public:
	void fn(std::vector<int>* value);
};

/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API InputPointerStdVectorStdVectorInt
{
public:
	void fn(std::vector<std::vector<int> >* value);
};

/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API InputPointerStdVectorSwigClass
{
public:
	void fn(std::vector<SwigClass>* value);
};


/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API InputPointerStdVectorSmartPointerSwigClass
{
public:
	void fn(std::vector< SmartPointer<SwigClass> >*  value);
};
	
	
/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API InputPointerStdVectorPointerSwigClass
{
public:
	void fn(std::vector<SwigClass*>*  value);
};



/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API InputReferenceStdVectorInt
{
public:
	void fn(std::vector<int>& value);
};

/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API InputReferenceStdVectorStdVectorInt
{
public:
	void fn(std::vector<std::vector<int> >& value);
};

/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API InputReferenceStdVectorSwigClass
{
public:
	void fn(std::vector<SwigClass>& value);
};


/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API InputReferenceStdVectorSmartPointerSwigClass
{
public:
	void fn(std::vector< SmartPointer<SwigClass> >&  value);
};
	
	
/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API InputReferenceStdVectorPointerSwigClass
{
public:
	void fn(std::vector<SwigClass*>&  value);
};

	
/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API ReturnAtomicType
{
public:
	ReturnAtomicType() ;

	bool           fn_bool           (bool value)           ;
	char           fn_char           (char value)           ;
	unsigned char  fn_unsigned_char  (unsigned char value)  ;
	short          fn_short          (short value)          ;
	unsigned short fn_unsigned_short (unsigned short value) ;
	int            fn_int            (int value)            ;
	unsigned int   fn_unsigned_int   (unsigned int value)   ;
	long           fn_long           (long value)           ;
	unsigned long  fn_unsigned_long  (unsigned long value)  ;
	float          fn_float          (float value)          ;
	double         fn_double         (double value)         ;
	std::string    fn_std_string     (std::string value)    ;
};


/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API ReturnSwigClass
{
	SwigClass var;
public:
	ReturnSwigClass ();
	~ReturnSwigClass();
	SwigClass get();
	Address get_var_memid() ;
};

/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API ReturnSmartPointerSwigClass
{
	SmartPointer<SwigClass> var;
public:
	ReturnSmartPointerSwigClass () ;
	~ReturnSmartPointerSwigClass() ;
	SmartPointer<SwigClass> get() ;
	Address get_var_memid() ;
};

/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API ReturnReferenceSwigClass
{
		SwigClass* var;
public:
	ReturnReferenceSwigClass () ;
	~ReturnReferenceSwigClass() ;
	SwigClass& get() ;
	Address get_var_memid() ;
};


/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API ReturnPointerSwigClass
{
	SwigClass* var;
public:
	ReturnPointerSwigClass () ;
	~ReturnPointerSwigClass() ;
	SwigClass* get() ;
	Address get_var_memid() ;
};




/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API ReturnStdVectorInt
{
	std::vector<int>  var;
	
public:

	std::vector<int> fn();
};

/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API ReturnStdVectorStdVectorInt
{
	std::vector<std::vector<int> > var;
public:

	std::vector<std::vector<int> > fn();
};
	
/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API ReturnStdVectorSwigClass
{
	std::vector<SwigClass> var;
public:
	std::vector<SwigClass> fn();
};

///////////////////////////////////////////////////////////////////////////////// 
class EXAMPLE_API ReturnStdVectorSmartPointerSwigClass
{
	std::vector<SmartPointer<SwigClass> > var;
public:

	std::vector<SmartPointer<SwigClass> > fn();
};
	        
/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API ReturnStdVectorPointerSwigClass   
{
	std::vector<SwigClass*> var;
public:

	ReturnStdVectorPointerSwigClass();
	
	~ReturnStdVectorPointerSwigClass();

	std::vector<SwigClass* > fn();
};

	
/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API ReturnPointerStdVectorInt
{
	std::vector<int>  var;
	
public:

	std::vector<int>* fn();
};

/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API ReturnPointerStdVectorStdVectorInt
{
	std::vector<std::vector<int> > var;
public:

	std::vector<std::vector<int> >* fn();
};
	
/////////////////////////////////////////////////////////////////////////////////  
class EXAMPLE_API ReturnPointerStdVectorSwigClass
{
	std::vector<SwigClass> var;
public:
	std::vector<SwigClass>* fn();
};

///////////////////////////////////////////////////////////////////////////////// 
class EXAMPLE_API ReturnPointerStdVectorSmartPointerSwigClass
{
	std::vector<SmartPointer<SwigClass> > var;
public:

	std::vector<SmartPointer<SwigClass> >* fn();
};
	        
/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API ReturnPointerStdVectorPointerSwigClass   
{
	std::vector<SwigClass*> var;
public:

	ReturnPointerStdVectorPointerSwigClass();
	
	~ReturnPointerStdVectorPointerSwigClass();

	std::vector<SwigClass* >* fn();
};	
	
	
   
/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API ReturnReferenceStdVectorInt
{
	std::vector<int>  var;
	
public:

	std::vector<int>& fn();
};

/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API ReturnReferenceStdVectorStdVectorInt
{
	std::vector<std::vector<int> > var;
public:

	std::vector<std::vector<int> >& fn();
};
	
/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API ReturnReferenceStdVectorSwigClass
{
	std::vector<SwigClass> var;
public:
	std::vector<SwigClass>& fn();
};

/////////////////////////////////////////////////////////////////////////////////  
class EXAMPLE_API ReturnReferenceStdVectorSmartPointerSwigClass
{
	std::vector<SmartPointer<SwigClass> > var;
public:

	std::vector<SmartPointer<SwigClass> >& fn();
};
	        
/////////////////////////////////////////////////////////////////////////////////
class EXAMPLE_API ReturnReferenceStdVectorPointerSwigClass   
{
	std::vector<SwigClass*> var;
public:

	ReturnReferenceStdVectorPointerSwigClass();
	
	~ReturnReferenceStdVectorPointerSwigClass();

	std::vector<SwigClass* >& fn();
};	



#endif //_EXAMPLE_H__

