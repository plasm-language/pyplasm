#if defined (SWIGPYTHON)
%module(directors="1") examplepy
#endif

#if defined (SWIGCSHARP)
%module(directors="1") examplecs
#endif

%{ #include "example.h" %}

// I prefer to use TR1 extension of STL instead boost for compilation time!
#define SWIG_SHARED_PTR_NAMESPACE std
#define SWIG_SHARED_PTR_SUBNAMESPACE tr1
 
%include <windows.i>
%include exception.i
%include <stl.i>
%include <boost_shared_ptr.i>


////////////////////////////////////////////////////////
// Handle exception 
////////////////////////////////////////////////////////
%exception
{
	try 
		{$function} 
	catch(std::exception const& e) 
		{SWIG_exception(SWIG_RuntimeError, e.what());} 
	catch(const char* msg)
		{SWIG_exception(SWIG_RuntimeError,msg);} 
	catch(...) 
		{SWIG_exception(SWIG_RuntimeError,"Unknown exception");}
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//common templates for Atomic Types
/////////////////////////////////////////////////////////////////////////////////////////////////

%template(StdVectorBool                                    ) std::vector< bool           >;
%template(StdVectorChar                                    ) std::vector< char           >;
%template(StdVectorUnsignedChar                            ) std::vector< unsigned char  >;
%template(StdVectorShort                                   ) std::vector< short          >;
%template(StdVectorUnsignedShort                           ) std::vector< unsigned short >;
%template(StdVectorInt                                     ) std::vector< int            >;
%template(StdVectorUnsignedInt                             ) std::vector< unsigned int   >;
%template(StdVectorLong                                    ) std::vector< long           >;
%template(StdVectorUnsignedLong                            ) std::vector< unsigned long  >;
%template(StdVectorFloat                                   ) std::vector< float          >;
%template(StdVectorDouble                                  ) std::vector< double         >;
%template(StdVectorStdString                               ) std::vector< std::string    >;
%template(StdVectorStdVectorInt                            ) std::vector< std::vector< int> >;

////////////////////////////////////////////////////////
// Python specific stuff
////////////////////////////////////////////////////////
#if defined (SWIGPYTHON)
%rename(__str__) str;
%rename(__repr__) repr;
%rename(assign) *::operator=;
%rename(__getitem__    ) *::operator[](int) const;
%rename(__getitem_ref__) *::operator[](int idx); //hidden
#endif //SWIGPYTHON


////////////////////////////////////////////////////////
// C Sharp specific stuff
////////////////////////////////////////////////////////

#if defined (SWIGCSHARP)

//rename all operators inside class
%rename(IsEqual)            *::operator ==;
%rename(IsNotEqual)         *::operator !=;
%rename(Assign)             *::operator =;
%rename(PlusEqual)          *::operator +=;
%rename(MinusEqual)         *::operator -=;
%rename(MultiplyEqual)      *::operator *=;
%rename(DivideEqual)        *::operator /=;
%rename(PercentEqual)       *::operator %=;
%rename(Plus)               *::operator +;
%rename(Minus)              *::operator -;
%rename(Multiply)           *::operator *;
%rename(Divide)             *::operator /;
%rename(Percent)            *::operator %;
%rename(Not)                *::operator !;
%rename(SquareBraketConst)  *::operator[](int idx) const;
%rename(SquareBraket)       *::operator[](int idx);
%rename(RoundBraket)        *::operator ();
%rename(LessThan)           *::operator <;
%rename(LessThanEqual)      *::operator <=;
%rename(GreaterThan)        *::operator >;
%rename(GreaterThanEqual)   *::operator >=;
%rename(And)                *::operator &&;
%rename(Or)                 *::operator ||;
%rename(PlusPlusPrefix)     *::operator++();
%rename(PlusPlusPostfix)    *::operator++(int);
%rename(MinusMinusPrefix)   *::operator--();
%rename(MinusMinusPostfix)  *::operator--(int);


%typemap(cscode) Vec3f
%{
	public static Vec3f  operator+ (Vec3f a,Vec3f b)  {return a.Plus(b);}
  public static Vec3f  operator- (Vec3f a,Vec3f b)  {return a.Minus(b);}
  public static float  operator* (Vec3f a,Vec3f b)  {return a.Multiply(b);}
  public static Vec3f  operator* (Vec3f a,float s)  {return a.Multiply(s);}
  public static Vec3f  operator/ (Vec3f a,float s)  {return a.Divide(s);} 
  public static bool   operator==(Vec3f a,Vec3f b)  {if (System.Object.ReferenceEquals(a, b)) return true ;if ((Object)a==null || (Object)b==null) return false; return  a.IsEqual(b);}
  public static bool   operator!=(Vec3f a,Vec3f b)  {if (System.Object.ReferenceEquals(a, b)) return false;if ((Object)a==null || (Object)b==null) return true ; return !a.IsEqual(b);}
%}

%typemap(cscode) CppArray
%{
	public float this[int index] {get {return this.get(index);}   set {this.set(index,value) ;} }
%}


//TODO: insert here the code for the C# operators, seems not possible to do automatically

#endif //SWIGCSHARP




/////////////////////////////////////////////////////////////////////////////////////////////////
//here your classes (do not mix std::vector with both shared (std::vector<SmartPointer<SwigClass> >) and non shared (std::vector<SmartPointer*> ) for the same class)
//here you can see that I do but only for debugging purpouse (NOTE this condition is valid only for python, not for C# in which I can mix both)
/////////////////////////////////////////////////////////////////////////////////////////////////

//here all the class virtuals
%feature("director") VirtualClass;

SWIG_SHARED_PTR(SwigClass, SwigClass)
SWIG_SHARED_PTR(CArray, CArray)

%template(StdVectorSwigClass              ) std::vector< SwigClass >;

%template(StdVectorSmartPointerSwigClass  ) std::vector< std::tr1::shared_ptr<SwigClass> >; // do not in your code mix (here only for debugging purpouse)!
%template(StdVectorPointerSwigClass       ) std::vector< SwigClass* >;                      // do not in your code mix (here only for debugging purpouse)!

%template(StdVectorVec3f                  ) std::vector< Vec3f >;
%template(StdVectorPointerVec3f           ) std::vector< Vec3f* >;
%template(StdVectorCppArray               ) std::vector< CppArray > ;
%template(StdVectorSmartPointerCppArray   ) std::vector< std::tr1::shared_ptr<CppArray> > ;


/////////////////////////////////////////////////////////////////////////////////////////////////
//your header here
/////////////////////////////////////////////////////////////////////////////////////////////////
%include "example.h"





