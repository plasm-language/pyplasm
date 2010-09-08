//////////////////////////////////////////
//SWIG CONFIGURATION FILE
//////////////////////////////////////////

#if defined (SWIGPYTHON)
%module(directors="1") xgepy
#endif

#if defined (SWIGCSHARP)
%module(directors="1") xgecs
#endif

%{ #include "xge/xge.h" %}

// I prefer to use TR1 extension of STL instead boost for compilation time!
#define SWIG_SHARED_PTR_NAMESPACE    std
#define SWIG_SHARED_PTR_SUBNAMESPACE tr1
  
%include <stl.i>
%include <boost_shared_ptr.i>

#if 0
%include <exception.i>
// Handle exception 
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
#endif

////////////////////////////////////////////////////////
//common templates for STL vector
////////////////////////////////////////////////////////

%template(StdVectorUnsignedChar ) std::vector< unsigned char  >;
%template(StdVectorInt          ) std::vector< int            >;
%template(StdVectorStdVectorInt ) std::vector< std::vector< int> >;
%template(StdVectorFloat        ) std::vector< float          >;
%template(StdVectorString       ) std::vector< std::string    >;

////////////////////////////////////////////////////////
// Python specific stuff
////////////////////////////////////////////////////////

#if defined (SWIGPYTHON)
%rename(__str__) str;
%rename(__repr__) repr;
%rename(assign) *::operator=;
%rename(__getitem__    ) *::operator[](int) const;
%rename(__getitem_ref__) *::operator[](int idx); 
%ignore  *::operator++;
%ignore  *::operator--;
%ignore SinglePool;
%ignore MemPool;
#endif //SWIGPYTHON


////////////////////////////////////////////////////////
// C Sharp specific stuff
////////////////////////////////////////////////////////

#if defined (SWIGCSHARP)
//rename all operators inside class
%rename(IsEqual)            *::operator ==;
%rename(IsNotEqual)         *::operator !=;
%rename(assign)             *::operator =;
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
%rename(__getitem__    )    *::operator[](int) const;
%rename(__getitem_ref__)    *::operator[](int idx); 
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
%rename(__str__)            *::str;
%rename(__repr__)           *::repr;


//___________________________________________________________________________________
//___________________ typemap std::vector<float> ____________________________________
//___________________________________________________________________________________

%typemap(cstype,out="double[]") const std::vector<float>&,std::vector<float> "System.Collections.IEnumerable"
%typemap(csin,
	pre=" 
		if ($csinput == null) throw new ArgumentNullException(\"$csinput\");
		StdVectorFloat temp_$csinput=new StdVectorFloat();
		foreach (object obj in $csinput) 
			temp_$csinput.Add(Convert.ToSingle(obj));"
	) const std::vector<float>&,std::vector<float> "$csclassname.getCPtr(temp_$csinput)"
%typemap(csout, excode=SWIGEXCODE) std::vector<float> {
    IntPtr cPtr = $imcall;$excode
    if (cPtr == IntPtr.Zero) return null;
    StdVectorFloat temp = new StdVectorFloat(cPtr, true);
    double[] ret = new double[temp.Count];
    for (int i=0;i<temp.Count;i++) ret[i]=(double)temp[i];
    return ret;
  }	

//___________________________________________________________________________________
//___________________ typemap std::vector<int> ____________________________________
//___________________________________________________________________________________

%typemap(cstype,out="int[]") const std::vector<int>&,std::vector<int> "System.Collections.IEnumerable"
%typemap(csin,
	pre=" 
		if ($csinput == null) throw new ArgumentNullException(\"$csinput\");
		StdVectorInt temp_$csinput=new StdVectorInt();
		foreach (object obj in $csinput) 
			temp_$csinput.Add(Convert.ToInt32(obj));"
	) const std::vector<int>&,std::vector<int> "$csclassname.getCPtr(temp_$csinput)"
%typemap(csout, excode=SWIGEXCODE) std::vector<int> {
    IntPtr cPtr = $imcall;$excode
    if (cPtr == IntPtr.Zero) return null;
    StdVectorInt temp = new StdVectorInt(cPtr, true);
    int[] ret = new int[temp.Count];
    for (int i=0;i<temp.Count;i++) ret[i]=temp[i];
    return ret;
  }	


//___________________________________________________________________________________
//___________________ typemap std::vector<std::vector<int> > ___________________
//___________________________________________________________________________________
%typemap(cstype) const std::vector<std::vector<int> >&,std::vector<std::vector<int> > "System.Collections.IEnumerable"
%typemap(csin,
	pre=" 
		if ($csinput == null) throw new ArgumentNullException(\"$csinput\");
		StdVectorStdVectorInt temp_$csinput=new StdVectorStdVectorInt();
		foreach (System.Collections.IEnumerable i in $csinput) 
		{
			StdVectorInt temp_local=new StdVectorInt();
			foreach (object obj in i) 
				temp_local.Add(Convert.ToInt32(obj));
			temp_$csinput.Add(temp_local);
		}"
	) const std::vector<std::vector<int> >&,std::vector<std::vector<int> > "$csclassname.getCPtr(temp_$csinput)"
 
 
 //___________________________________________________________________________________
//___________________ typemap std::vector<std::string> ___________________
//___________________________________________________________________________________

%typemap(cstype,out="string[]") const std::vector<std::string >&,std::vector<std::string > "System.Collections.IEnumerable"
%typemap(csin,
	pre=" 
		if ($csinput == null) throw new ArgumentNullException(\"$csinput\");
		StdVectorString temp_$csinput=new StdVectorString();
		foreach (object obj in $csinput) 
			temp_$csinput.Add(obj.ToString());"
	) const std::vector<std::string >&,std::vector<std::string > "$csclassname.getCPtr(temp_$csinput)"
%typemap(csout, excode=SWIGEXCODE) std::vector<std::string > {
    IntPtr cPtr = $imcall;$excode
    if (cPtr == IntPtr.Zero) return null;
    StdVectorString temp = new StdVectorString(cPtr, true);
    String[] ret = new String[temp.Count];
    for (int i=0;i<temp.Count;i++) ret[i]=temp[i];
    return ret;
  }	

//___________________________________________________________________________________
//___________________ typemap std::vector<Vec3f> ___________________
//___________________________________________________________________________________

%typemap(cstype,out="Vec3f[]") const std::vector<Vec3f>&,std::vector<Vec3f> "System.Collections.IEnumerable"
%typemap(csin,
	pre=" 
		if ($csinput == null) throw new ArgumentNullException(\"$csinput\");
		StdVectorVec3f temp_$csinput=new StdVectorVec3f();
		foreach (object obj in $csinput) 
			temp_$csinput.Add((Vec3f)obj);"
	) const std::vector<Vec3f>&,std::vector<Vec3f> "$csclassname.getCPtr(temp_$csinput)"
%typemap(csout, excode=SWIGEXCODE) std::vector<Vec3f> {
    IntPtr cPtr = $imcall;$excode
    if (cPtr == IntPtr.Zero) return null;
    StdVectorVec3f temp = new StdVectorVec3f(cPtr, true);
    Vec3f[] ret = new Vec3f[temp.Count];
    for (int i=0;i<temp.Count;i++) ret[i]=temp[i];
    return ret;
  }	

 
 
//___________________________________________________________________________________
//___________________ typemap std::vector<SmartPointer<Hpc> > ___________________
//___________________________________________________________________________________
%typemap(cstype,out="Hpc[]") const std::vector<std::tr1::shared_ptr<Hpc> >&,std::vector<std::tr1::shared_ptr<Hpc> > "System.Collections.IEnumerable"
%typemap(csin,
	pre=" 
		if ($csinput == null) throw new ArgumentNullException(\"$csinput\");
		StdVectorSmartPointerHpc temp_$csinput=new StdVectorSmartPointerHpc();
		foreach (object obj in $csinput) 
			temp_$csinput.Add((Hpc)obj);"
	) const std::vector<std::tr1::shared_ptr<Hpc> >&,std::vector<std::tr1::shared_ptr<Hpc> > "$csclassname.getCPtr(temp_$csinput)"
%typemap(csout, excode=SWIGEXCODE) std::vector<std::tr1::shared_ptr<Hpc> > {
    IntPtr cPtr = $imcall;$excode
    if (cPtr == IntPtr.Zero) return null;
    StdVectorSmartPointerHpc temp = new StdVectorSmartPointerHpc(cPtr, true);
    Hpc[] ret = new Hpc[temp.Count];
    for (int i=0;i<temp.Count;i++) ret[i]=temp[i];
    return ret;
  }	

//___________________________________________________________________________________
//___________________ typemap std::vector<SmartPointer<Batch> > ___________________
//___________________________________________________________________________________

%typemap(cstype,out="Batch[]") const std::vector<std::tr1::shared_ptr<Batch> >&,std::vector<std::tr1::shared_ptr<Batch> > "System.Collections.IEnumerable"
%typemap(csin,
	pre=" 
		if ($csinput == null) throw new ArgumentNullException(\"$csinput\");
		StdVectorSmartPointerBatch temp_$csinput=new StdVectorSmartPointerBatch();
		foreach (object obj in $csinput) 
			temp_$csinput.Add((Batch)obj);"
	) const std::vector<std::tr1::shared_ptr<Batch> >&,std::vector<std::tr1::shared_ptr<Batch> > "$csclassname.getCPtr(temp_$csinput)"
%typemap(csout, excode=SWIGEXCODE) std::vector<std::tr1::shared_ptr<Batch> > {
    IntPtr cPtr = $imcall;$excode
    if (cPtr == IntPtr.Zero) return null;
    StdVectorSmartPointerBatch temp = new StdVectorSmartPointerBatch(cPtr, true);
    Batch[] ret = new Batch[temp.Count];
    for (int i=0;i<temp.Count;i++) ret[i]=temp[i];
    return ret;
  }	
  
 
//___________________________________________________________________________________
//___________________ typemap std::vector<SmartPointer<Texture> > ___________________
//___________________________________________________________________________________

%typemap(cstype,out="Texture[]") const std::vector<std::tr1::shared_ptr<Texture> >&,std::vector<std::tr1::shared_ptr<Texture> > "System.Collections.IEnumerable"
%typemap(csin,
	pre=" 
		if ($csinput == null) throw new ArgumentNullException(\"$csinput\");
		StdVectorSmartPointerTexture temp_$csinput=new StdVectorSmartPointerTexture();
		foreach (object obj in $csinput) 
			temp_$csinput.Add((Texture)obj);"
	) const std::vector<std::tr1::shared_ptr<Texture> >&,std::vector<std::tr1::shared_ptr<Texture> > "$csclassname.getCPtr(temp_$csinput)"
%typemap(csout, excode=SWIGEXCODE) std::vector<std::tr1::shared_ptr<Texture> > {
    IntPtr cPtr = $imcall;$excode
    if (cPtr == IntPtr.Zero) return null;
    StdVectorSmartPointerTexture temp = new StdVectorSmartPointerTexture(cPtr, true);
    Texture[] ret = new Texture[temp.Count];
    for (int i=0;i<temp.Count;i++) ret[i]=temp[i];
    return ret;
  }	

// reintroduce C# operators



%typemap(cscode) Clock
%{
  public static float  operator- (Clock  a,Clock  b)  {return a.Minus(b);}
%}    

%typemap(cscode) Vector
%{
  public float this[int index] {get {return this.get(index);} set {this.set(index,value) ;} }
  public static bool   operator==(Vector a,Vector b)  {if (System.Object.ReferenceEquals(a, b)) return true ;if ((Object)a==null || (Object)b==null) return false; return  a.IsEqual(b);}
  public static bool   operator!=(Vector a,Vector b)  {if (System.Object.ReferenceEquals(a, b)) return false;if ((Object)a==null || (Object)b==null) return true ; return !a.IsEqual(b);}
  public override bool Equals(object b) {if (b == null || GetType() != b.GetType()) return false;return this.IsEqual((Vector)b);}
  public override int  GetHashCode() {return  base.GetHashCode();}
%}   

%typemap(cscode) Ball3f
%{
	public static bool   operator==(Ball3f a,Ball3f b)  {if (System.Object.ReferenceEquals(a, b)) return true ;if ((Object)a==null || (Object)b==null) return false; return  a.IsEqual(b);}
	public static bool   operator!=(Ball3f a,Ball3f b)  {if (System.Object.ReferenceEquals(a, b)) return false;if ((Object)a==null || (Object)b==null) return true ; return !a.IsEqual(b);}
	public override bool Equals(object b) {if (b == null || GetType() != b.GetType()) return false;return this.IsEqual((Ball3f)b);}
	public override int  GetHashCode() {return  base.GetHashCode();}
%}   
        
%typemap(cscode) Ballf
%{
	public static bool   operator==(Ballf a,Ballf b)  {if (System.Object.ReferenceEquals(a, b)) return true ;if ((Object)a==null || (Object)b==null) return false; return  a.IsEqual(b);}
	public static bool   operator!=(Ballf a,Ballf b)  {if (System.Object.ReferenceEquals(a, b)) return false;if ((Object)a==null || (Object)b==null) return true ; return !a.IsEqual(b);}
	public override bool Equals(object b) {if (b == null || GetType() != b.GetType()) return false;return this.IsEqual((Ballf)b);}
	public override int  GetHashCode() {return  base.GetHashCode();}
%}     
    
%typemap(cscode) Box3f
%{
	public static bool   operator==(Box3f a,Box3f b)  {if (System.Object.ReferenceEquals(a, b)) return true ;if ((Object)a==null || (Object)b==null) return false; return  a.IsEqual(b);}
	public static bool   operator!=(Box3f a,Box3f b)  {if (System.Object.ReferenceEquals(a, b)) return false;if ((Object)a==null || (Object)b==null) return true ; return !a.IsEqual(b);}
	public override bool Equals(object b) {if (b == null || GetType() != b.GetType()) return false;return this.IsEqual((Box3f)b);}
	public override int  GetHashCode() {return  base.GetHashCode();}
%}     
    
 %typemap(cscode) Boxf
%{
	public static bool   operator==(Boxf a,Boxf b)  {if (System.Object.ReferenceEquals(a, b)) return true ;if ((Object)a==null || (Object)b==null) return false; return  a.IsEqual(b);}
	public static bool   operator!=(Boxf a,Boxf b)  {if (System.Object.ReferenceEquals(a, b)) return false;if ((Object)a==null || (Object)b==null) return true ; return !a.IsEqual(b);}
	public override bool Equals(object b) {if (b == null || GetType() != b.GetType()) return false;return this.IsEqual((Boxf)b);}
	public override int  GetHashCode() {return  base.GetHashCode();}
%}     
  
 %typemap(cscode) Color4f
%{
	public float this[int index] {get {return this.get(index);} set {this.set(index,value) ;} }
	public static bool   operator==(Color4f a,Color4f b)  {if (System.Object.ReferenceEquals(a, b)) return true ;if ((Object)a==null || (Object)b==null) return false; return  a.IsEqual(b);}
	public static bool   operator!=(Color4f a,Color4f b)  {if (System.Object.ReferenceEquals(a, b)) return false;if ((Object)a==null || (Object)b==null) return true ; return !a.IsEqual(b);}
	public override bool Equals(object b) {if (b == null || GetType() != b.GetType()) return false;return this.IsEqual((Color4f)b);}
	public override int  GetHashCode() {return  base.GetHashCode();}
%}   

%typemap(cscode) Frustum
%{
	public static bool   operator==(Frustum a,Frustum b)  {if (System.Object.ReferenceEquals(a, b)) return true ;if ((Object)a==null || (Object)b==null) return false; return  a.IsEqual(b);}
	public static bool   operator!=(Frustum a,Frustum b)  {if (System.Object.ReferenceEquals(a, b)) return false;if ((Object)a==null || (Object)b==null) return true ; return !a.IsEqual(b);}
	public override bool Equals(object b) {if (b == null || GetType() != b.GetType()) return false;return this.IsEqual((Frustum)b);}
	public override int  GetHashCode() {return  base.GetHashCode();}	
%}     

%typemap(cscode) Mat4f
%{
	public static Mat4f  operator+ (Mat4f  a,Mat4f  b)  {return a.Plus(b);}
	public static Mat4f  operator- (Mat4f  a,Mat4f  b)  {return a.Minus(b);}
	public static Mat4f  operator* (Mat4f  a,Mat4f  b)  {return a.Multiply(b);}
	public static Mat4f  operator* (Mat4f  a,float  s)  {return a.Multiply(s);}
	public static Mat4f  operator* (float  s,Mat4f  a)  {return a.Multiply(s);}
	public static Vec4f  operator* (Mat4f  a,Vec4f  v)  {return a.Multiply(v);}
	public static Vec3f  operator* (Mat4f  a,Vec3f  v)  {return a.Multiply(v);}
	public float this[int index] {get {return this.get(index);} set {this.set(index,value) ;} }
	public static bool   operator==(Mat4f a,Mat4f b)  {if (System.Object.ReferenceEquals(a, b)) return true ;if ((Object)a==null || (Object)b==null) return false; return  a.IsEqual(b);}
	public static bool   operator!=(Mat4f a,Mat4f b)  {if (System.Object.ReferenceEquals(a, b)) return false;if ((Object)a==null || (Object)b==null) return true ; return !a.IsEqual(b);}
	public override bool Equals(object b) {if (b == null || GetType() != b.GetType()) return false;return this.IsEqual((Mat4f)b);}
	public override int  GetHashCode() {return  base.GetHashCode();}
%}    


%typemap(cscode) Matf
%{
	public static Matf  operator+ (Matf  a,Matf  b)  {return a.Plus(b);}
	public static Matf  operator- (Matf  a,Matf  b)  {return a.Minus(b);}
	public static Matf  operator* (Matf  a,Matf  b)  {return a.Multiply(b);}
	public static Matf  operator* (Matf  a,float s)  {return a.Multiply(s);}
	public static Matf  operator* (float s,Matf  a)  {return a.Multiply(s);}
	public static Vecf  operator* (Matf  a,Vecf  v)  {return a.Multiply(v);}
	public float this[int index] {get {return this.get(index);} set {this.set(index,value) ;} }
	public static bool   operator==(Matf a,Matf b)  {if (System.Object.ReferenceEquals(a, b)) return true ;if ((Object)a==null || (Object)b==null) return false; return  a.IsEqual(b);}
	public static bool   operator!=(Matf a,Matf b)  {if (System.Object.ReferenceEquals(a, b)) return false;if ((Object)a==null || (Object)b==null) return true ; return !a.IsEqual(b);}
	public override bool Equals(object b) {if (b == null || GetType() != b.GetType()) return false;return this.IsEqual((Matf)b);}
	public override int  GetHashCode() {return  base.GetHashCode();}
%}  

%typemap(cscode) Quaternion
%{
	public static Quaternion  operator+ (Quaternion a,Quaternion b)  {return a.Plus(b);}
	public static Quaternion  operator- (Quaternion a,Quaternion b)  {return a.Minus(b);}
	public static Quaternion  operator* (Quaternion a,Quaternion b)  {return a.Multiply(b);}
	public static Quaternion  operator* (Quaternion a,float        s)  {return a.Multiply(s);}
	public static Quaternion  operator* (float s       ,Quaternion a)  {return a.Multiply(s);}
	public static Vec3f         operator* (Quaternion a,Vec3f b       )  {return a.Multiply(b);} 
	public static Quaternion  operator- (Quaternion a               )  {return a.Minus();} 
	public float this[int index] {get {return this.get(index);} set {this.set(index,value) ;} }
	public static bool   operator==(Quaternion a,Quaternion b)  {if (System.Object.ReferenceEquals(a, b)) return true ;if ((Object)a==null || (Object)b==null) return false; return  a.IsEqual(b);}
	public static bool   operator!=(Quaternion a,Quaternion b)  {if (System.Object.ReferenceEquals(a, b)) return false;if ((Object)a==null || (Object)b==null) return true ; return !a.IsEqual(b);}
	public override bool Equals(object b) {if (b == null || GetType() != b.GetType()) return false;return this.IsEqual((Quaternion)b);}
	public override int  GetHashCode() {return  base.GetHashCode();}
%}

%typemap(cscode) Ray3f
%{
	public static bool   operator==(Ray3f a,Ray3f b)  {if (System.Object.ReferenceEquals(a, b)) return true ;if ((Object)a==null || (Object)b==null) return false; return  a.IsEqual(b);}
	public static bool   operator!=(Ray3f a,Ray3f b)  {if (System.Object.ReferenceEquals(a, b)) return false;if ((Object)a==null || (Object)b==null) return true ; return !a.IsEqual(b);}
	public override bool Equals(object b) {if (b == null || GetType() != b.GetType()) return false;return this.IsEqual((Ray3f)b);}
	public override int  GetHashCode() {return  base.GetHashCode();}
%}   

%typemap(cscode) Rayf
%{
	public static bool   operator==(Rayf a,Rayf b)  {if (System.Object.ReferenceEquals(a, b)) return true ;if ((Object)a==null || (Object)b==null) return false; return  a.IsEqual(b);}
	public static bool   operator!=(Rayf a,Rayf b)  {if (System.Object.ReferenceEquals(a, b)) return false;if ((Object)a==null || (Object)b==null) return true ; return !a.IsEqual(b);}
	public override bool Equals(object b) {if (b == null || GetType() != b.GetType()) return false;return this.IsEqual((Rayf)b);}
	public override int  GetHashCode() {return  base.GetHashCode();}
%}   

%typemap(cscode) Triangle2i
%{
	public static bool   operator==(Triangle2i a,Triangle2i b)  {if (System.Object.ReferenceEquals(a, b)) return true ;if ((Object)a==null || (Object)b==null) return false; return  a.IsEqual(b);}
	public static bool   operator!=(Triangle2i a,Triangle2i b)  {if (System.Object.ReferenceEquals(a, b)) return false;if ((Object)a==null || (Object)b==null) return true ; return !a.IsEqual(b);}
	public override bool Equals(object b) {if (b == null || GetType() != b.GetType()) return false;return this.IsEqual((Triangle2i)b);}
	public override int  GetHashCode() {return  base.GetHashCode();}
%}   

%typemap(cscode) Vec2f
%{
	public float this[int index] {get {return this.get(index);} set {this.set(index,value) ;} }
	public static bool   operator==(Vec2f a,Vec2f b)  {if (System.Object.ReferenceEquals(a, b)) return true ;if ((Object)a==null || (Object)b==null) return false; return  a.IsEqual(b);}
	public static bool   operator!=(Vec2f a,Vec2f b)  {if (System.Object.ReferenceEquals(a, b)) return false;if ((Object)a==null || (Object)b==null) return true ; return !a.IsEqual(b);}
	public override bool Equals(object b) {if (b == null || GetType() != b.GetType()) return false;return this.IsEqual((Vec2f)b);}
	public override int  GetHashCode() {return  base.GetHashCode();}
%} 

%typemap(cscode) Vec2i
%{
	public int this[int index] {get {return this.get(index);} set {this.set(index,value) ;} }
	public static bool   operator==(Vec2i a,Vec2i b)  {if (System.Object.ReferenceEquals(a, b)) return true ;if ((Object)a==null || (Object)b==null) return false; return  a.IsEqual(b);}
	public static bool   operator!=(Vec2i a,Vec2i b)  {if (System.Object.ReferenceEquals(a, b)) return false;if ((Object)a==null || (Object)b==null) return true ; return !a.IsEqual(b);}
	public override bool Equals(object b) {if (b == null || GetType() != b.GetType()) return false;return this.IsEqual((Vec2i)b);}
	public override int  GetHashCode() {return  base.GetHashCode();}
%}

%typemap(cscode) Vec3f
%{
	public static Vec3f  operator+ (Vec3f a,Vec3f b)  {return a.Plus(b);}
	public static Vec3f  operator- (Vec3f a,Vec3f b)  {return a.Minus(b);}
	public static float  operator* (Vec3f a,Vec3f b)  {return a.Multiply(b);}
	public static Vec3f  operator* (Vec3f a,float s)  {return a.Multiply(s);}
	public static Vec3f  operator* (float s,Vec3f a)  {return a.Multiply(s);}
	public static Vec3f  operator/ (Vec3f a,float s)  {return a.Divide(s);} 
	public float this[int index] {get {return this.get(index);} set {this.set(index,value) ;} }
	public static bool   operator==(Vec3f a,Vec3f b)  {if (System.Object.ReferenceEquals(a, b)) return true ;if ((Object)a==null || (Object)b==null) return false; return  a.IsEqual(b);}
	public static bool   operator!=(Vec3f a,Vec3f b)  {if (System.Object.ReferenceEquals(a, b)) return false;if ((Object)a==null || (Object)b==null) return true ; return !a.IsEqual(b);}
	public override bool Equals(object b) {if (b == null || GetType() != b.GetType()) return false;return this.IsEqual((Vec3f)b);}
	public override int  GetHashCode() {return  base.GetHashCode();}
%}

%typemap(cscode) Vec4f
%{
	public static Vec4f  operator+ (Vec4f a,Vec4f b)  {return a.Plus(b);}
	public static Vec4f  operator- (Vec4f a,Vec4f b)  {return a.Minus(b);}
	public static float  operator* (Vec4f a,Vec4f b)  {return a.Multiply(b);}
	public static Vec4f  operator* (Vec4f a,float s)  {return a.Multiply(s);}
	public static Vec4f  operator* (float s,Vec4f a)  {return a.Multiply(s);}
	public static Vec4f  operator/ (Vec4f a,float s)  {return a.Divide(s);} 
	public static Vec4f  operator* (Vec4f v,Mat4f matrix) {return v.Multiply(matrix);}
	public float this[int index] {get {return this.get(index);} set {this.set(index,value) ;} }
	public static bool   operator==(Vec4f a,Vec4f b)  {if (System.Object.ReferenceEquals(a, b)) return true ;if ((Object)a==null || (Object)b==null) return false; return  a.IsEqual(b);}
	public static bool   operator!=(Vec4f a,Vec4f b)  {if (System.Object.ReferenceEquals(a, b)) return false;if ((Object)a==null || (Object)b==null) return true ; return !a.IsEqual(b);}
	public override bool Equals(object b) {if (b == null || GetType() != b.GetType()) return false;return this.IsEqual((Vec4f)b);}
	public override int  GetHashCode() {return  base.GetHashCode();}
%}

%typemap(cscode) Vecf
%{
	public static Vecf  operator+ (Vecf a,Vecf b)  {return a.Plus(b);}
	public static Vecf  operator- (Vecf a,Vecf b)  {return a.Minus(b);}
	public static float  operator* (Vecf a,Vecf b)  {return a.Multiply(b);}
	public static Vecf  operator* (Vecf a,float s)  {return a.Multiply(s);}
	public static Vecf  operator* (float s,Vecf a)  {return a.Multiply(s);}
	public static Vecf  operator/ (Vecf a,float s)  {return a.Divide(s);} 
	public static Vecf  operator* (Vecf v,Matf matrix) {return v.Multiply(matrix);}
	public float this[int index] {get {return this.get(index);} set {this.set(index,value) ;} }
	public static bool   operator==(Vecf a,Vecf b)  {if (System.Object.ReferenceEquals(a, b)) return true ;if ((Object)a==null || (Object)b==null) return false; return  a.IsEqual(b);}
	public static bool   operator!=(Vecf a,Vecf b)  {if (System.Object.ReferenceEquals(a, b)) return false;if ((Object)a==null || (Object)b==null) return true ; return !a.IsEqual(b);}
	public override bool Equals(object b) {if (b == null || GetType() != b.GetType()) return false;return this.IsEqual((Vecf)b);}
	public override int  GetHashCode() {return  base.GetHashCode();}
%}

#endif //SWIGCSHARP


/////////////////////////////////////////////////////////////////////////////////////////////////
//here configuration for your classes 
//NOTE: do not mix std::vector with both shared (std::vector<SmartPointer<SwigClass> >) and non shared (std::vector<SmartPointer*> ) for the same class
/////////////////////////////////////////////////////////////////////////////////////////////////

%ignore FrustumIterator::Item;
%ignore RayIterator::Item;
%ignore SinglePool;
%ignore MemPool;

%feature("director") Viewer;

SWIG_SHARED_PTR(Matf,Matf)
SWIG_SHARED_PTR(Vector,Vector)
SWIG_SHARED_PTR(Texture,Texture)
SWIG_SHARED_PTR(Graph,Graph)
SWIG_SHARED_PTR(Hpc,Hpc)
SWIG_SHARED_PTR(Batch,Batch)
SWIG_SHARED_PTR(Octree,Octree)
SWIG_SHARED_PTR(EngineResource,EngineResource)

%template(StdVectorVec3f)                             std::vector<Vec3f>;
%template(StdVectorSmartPointerHpc)                   std::vector<std::tr1::shared_ptr<Hpc> >;
%template(StdVectorSmartPointerBatch)                 std::vector<std::tr1::shared_ptr<Batch> >;
%template(StdVectorSmartPointerTexture)               std::vector<std::tr1::shared_ptr<Texture> >;
%template(StdVectorSmartPointerEngineResource)        std::vector<std::tr1::shared_ptr<EngineResource> >;

////////////////////////////////////////////////////////////
//your header here
////////////////////////////////////////////////////////////

%include <xge/xge.h>
%include <xge/config.h>
%include <xge/utils.h>
%include <xge/log.h>
%include <xge/clock.h>
%include <xge/keyboard.h>
%include <xge/spinlock.h>
%include <xge/mempool.h>
%include <xge/archive.h>
%include <xge/encoder.h>
%include <xge/filesystem.h>
//%include <xge/thread.h>
%include <xge/mouseevent.h>
%include <xge/vec.h>
%include <xge/triangle.h>
%include <xge/plane.h>
%include <xge/mat.h>
%include <xge/ball.h>
%include <xge/box.h>
%include <xge/ray.h>
%include <xge/quaternion.h>
%include <xge/localsystem.h>
%include <xge/vector.h>
%include <xge/color4f.h>
%include <xge/frustum.h>
%include <xge/texture.h>
%include <xge/manipulator.h>
%include <xge/batch.h>
%include <xge/pick.h>
%include <xge/graph.h>
%include <xge/engine.h>
%include <xge/viewer.h>
%include <xge/octree.h>
%include <xge/unwrapper.h>
%include <xge/bake.h>
%include <xge/plasm.h>

