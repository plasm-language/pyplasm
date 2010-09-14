#include "example.h"

//questo funziona sia sui SO a 32 che 64 bit
inline Address AddressOf(void* var)
{
	return ((uint64)(var));
}



/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
VirtualClass::VirtualClass()
	{value=0;}

VirtualClass::~VirtualClass()
	{} 

void VirtualClass::fn()
	{;}

int VirtualClass::getValue()
	{return this->value;}

void VirtualClass::setValue(int value)
	{this->value=value;}



/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

int SwigClass::num_nodes=0;

SwigClass::SwigClass()                      
{
	++num_nodes;this->value=0;
} 

SwigClass::SwigClass(const SwigClass& src)  
	{++num_nodes;this->value=src.value;} 

SwigClass::SwigClass(int value)    
	{++num_nodes;this->value=value;}

SwigClass::~SwigClass()                     
	{--num_nodes;} 

Address SwigClass::AddressOf()
{
	return ::Address(this);
}





///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////


Vec3f::Vec3f() 
	{x=y=z=0;}

Vec3f::Vec3f(const Vec3f& src) 
	{this->x=src.x;this->y=src.y;this->z=src.z;}

Vec3f::Vec3f(float _x,float _y,float _z):x(_x),y(_y),z(_z) 
	{;}

Vec3f& Vec3f::operator=(const Vec3f& src)
	{this->x=src.x;this->y=src.y;this->z=src.z;return *this;}

bool Vec3f::operator==(const Vec3f& src)
	{return this->x==src.x && this->y==src.y && this->z==src.z;}

Vec3f  Vec3f::operator+ (Vec3f b)  {return Vec3f(x+b.x,y+b.y,z+b.z);}
Vec3f  Vec3f::operator- (Vec3f b)  {return Vec3f(x-b.x,y-b.y,z-b.z);}
float  Vec3f::operator* (Vec3f b)  {return x*b.x+y*b.y+z*b.z;}

Vec3f  Vec3f::operator* (float s)  {return Vec3f(x*s,y*s,z*s);}
Vec3f  Vec3f::operator/ (float s)  {return Vec3f(x/s,y/s,z/s);} 

Vec3f& Vec3f::operator+=(Vec3f v)  {x+=v.x;y+=v.y;z+=v.z;return *this;}
Vec3f& Vec3f::operator-=(Vec3f v)  {x-=v.x;y-=v.y;z-=v.z;return *this;}

Vec3f& Vec3f::operator*=(float s)  {x*=s;y*=s;z*=s;return *this;}
Vec3f& Vec3f::operator/=(float s)  {x/=s;y/=s;z/=s;return *this;}

std::string Vec3f::repr()
{
	char s[1024];
	sprintf(s,"Vec3f(%.2f,%.2f,%.2f)",x,y,z);
	return s;
}

std::string Vec3f::str()
{
	char s[1024];
	sprintf(s,"(%.2f,%.2f,%.2f)",x,y,z);
	return s;
}	
	


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

CppArray::CppArray()
{
	this->size=0;
	this->mem=0;
}

CppArray::~CppArray()
{
	if (this->size)
		free(this->mem);
}


CppArray::CppArray(const std::vector<float>& src)
{
	this->size=src.size();
	
	if (this->size)
	{
		this->mem=(float*)malloc(sizeof(float)*size);
		memcpy(this->mem,&src[0],sizeof(float)*size);
	}
	else
	{
		this->mem=0;
	}

}

CppArray::CppArray(int size)
{
	this->size=size;

	if (this->size)
	{
		this->mem=(float*)malloc(sizeof(float)*size);
		memset(this->mem,0,sizeof(float)*size);
	}
	else
	{
		this->mem=0;
	}	
}

CppArray::CppArray(const CppArray& src) 
{
	this->size=src.size;
	
	if (this->size)
	{
		this->mem=(float*)malloc(sizeof(float)*this->size);
		memcpy(this->mem,src.mem,sizeof(float)*this->size);
	}
	else
	{
		this->mem=0;
	}
}	

CppArray& CppArray::operator=(const CppArray& src)
{
	this->size=src.size;
	this->mem=(float*)malloc(sizeof(float)*this->size);
	memcpy(this->mem,src.mem,sizeof(float)*this->size);
	return *this;
}


bool CppArray::operator==(const CppArray& src) const
{
	return this->size==src.size && !memcmp(this->mem,src.mem,sizeof(float)*this->size);
}

void CppArray::push_back(float value)
{
	++this->size;
	this->mem=(float*)realloc(this->mem,sizeof(float)*this->size);
	this->mem[this->size-1]=value;
}

std::vector<float> CppArray::c_vector()
{
	std::vector<float> ret(this->size);
	
	for (int i=0;i<this->size;i++)
		ret[i]=this->mem[i];
		
	return ret;
}



float CppArray::get(int idx) const
{
	if (idx<0 || idx>=this->size)
		throw std::out_of_range("out of range");
		
	return this->mem[idx];
}


float& CppArray::operator[](int idx)
{
	if (idx<0 || idx>=this->size)
		throw std::out_of_range("out of range");

	return this->mem[idx];
}



void CppArray::set(int idx,float value)
{
	if (idx<0 || idx>=this->size)
		throw std::out_of_range("out of range");
		
	this->mem[idx]=value;
}


std::string CppArray::str() 
{
	std::string ret="";
	for (int i=0;i<this->size;i++)
		ret+=FormatMsg("%.2f ",this->mem[i]);
	return ret;
}
	
std::string CppArray::repr()
{
	return this->str();
}

	
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////


VarAtomicType::VarAtomicType()
{
	var_bool=false;
	var_char=0;
	var_unsigned_char=0;
	var_short=0;
	var_unsigned_short=0;
	var_int=0;
	var_unsigned_int=0;
	var_long=0;
	var_unsigned_long=0;
	var_float=0;
	var_double=0;
	var_string="";
}	




/////////////////////////////////////////////////////////////////////////////////

VarSmartPointerSwigClass::VarSmartPointerSwigClass () {var.reset(new SwigClass());}



/////////////////////////////////////////////////////////////////////////////////

VarPointerSwigClass::VarPointerSwigClass () {var=new SwigClass();}
VarPointerSwigClass::~VarPointerSwigClass() {delete var;}





/////////////////////////////////////////////////////////////////////////////////

VarPointerStdVectorInt::VarPointerStdVectorInt()
	{var=new std::vector<int>;}
	
VarPointerStdVectorInt::~VarPointerStdVectorInt()
	{delete var;}



/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////

VarPointerStdVectorStdVectorInt::VarPointerStdVectorStdVectorInt()
	{var=new std::vector< std::vector<int> >;}
	
VarPointerStdVectorStdVectorInt::~VarPointerStdVectorStdVectorInt()
	{delete var;}


/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////


VarPointerStdVectorSwigClass::VarPointerStdVectorSwigClass()
	{var=new std::vector<SwigClass>;}
	
VarPointerStdVectorSwigClass::~VarPointerStdVectorSwigClass()
	{delete var;}


/////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////

VarPointerStdVectorSmartPointerSwigClass::VarPointerStdVectorSmartPointerSwigClass()
	{var=new std::vector<SmartPointer<SwigClass> >;}
	
VarPointerStdVectorSmartPointerSwigClass::~VarPointerStdVectorSmartPointerSwigClass()
	{delete var;}



/////////////////////////////////////////////////////////////////////////////////

VarStdVectorPointerSwigClass::VarStdVectorPointerSwigClass() 
	{var.push_back(new SwigClass);}
	
VarStdVectorPointerSwigClass::~VarStdVectorPointerSwigClass()
{
	for (int n=0;n<var.size();n++)
		delete var[n];
}



/////////////////////////////////////////////////////////////////////////////////
VarPointerStdVectorPointerSwigClass::VarPointerStdVectorPointerSwigClass() 
{
	var=new std::vector<SwigClass*>;
	var->push_back(new SwigClass);
}
	
VarPointerStdVectorPointerSwigClass::~VarPointerStdVectorPointerSwigClass()
{
	for (int i=0;i<var->size();i++)delete (*var)[i];
	delete var;
}	



/////////////////////////////////////////////////////////////////////////////////

InputAtomicType::InputAtomicType() {;};
int InputAtomicType::fn(int value) {return value;}


/////////////////////////////////////////////////////////////////////////////////

InputSwigClass::InputSwigClass(){;}
Address InputSwigClass::fn(SwigClass value) {return AddressOf(&value);}


/////////////////////////////////////////////////////////////////////////////////

InputSmartPointerSwigClass::InputSmartPointerSwigClass(){;}
Address InputSmartPointerSwigClass::fn(SmartPointer<SwigClass> value) {return AddressOf(&(*value));}


/////////////////////////////////////////////////////////////////////////////////

InputReferenceSwigClass::InputReferenceSwigClass(){;}
Address InputReferenceSwigClass::fn(SwigClass& value) {return AddressOf(&value);}



/////////////////////////////////////////////////////////////////////////////////

InputPointerSwigClass::InputPointerSwigClass(){;}
Address InputPointerSwigClass::fn(SwigClass* value) {return AddressOf(value);}



/////////////////////////////////////////////////////////////////////////////////

void InputStdVectorInt::fn(std::vector<int> value)
{
	if (value.size()>0) value[0]++;
	value.push_back(9999);	
}


/////////////////////////////////////////////////////////////////////////////////

void InputStdVectorStdVectorInt::fn(std::vector<std::vector<int> > value)
{
	if (value.size()>0 && value[0].size()>0) value[0][0]++;
	value.push_back(std::vector<int>());
}


/////////////////////////////////////////////////////////////////////////////////

void InputStdVectorSwigClass::fn(std::vector<SwigClass> value)
{
	if (value.size()>0) value[0].value++;
	value.push_back(SwigClass());	
}


/////////////////////////////////////////////////////////////////////////////////

void InputStdVectorSmartPointerSwigClass::fn(std::vector< SmartPointer<SwigClass> >  value)
{
	if (value.size()>0) value[0]->value++;
	value.push_back(SmartPointer<SwigClass>(new SwigClass()));
}

/////////////////////////////////////////////////////////////////////////////////

void InputStdVectorPointerSwigClass::fn(std::vector<SwigClass*>  value)
{
	if (value.size()>0) value[0]->value++;		
}


/////////////////////////////////////////////////////////////////////////////////

void InputPointerStdVectorInt::fn(std::vector<int>* value)
{
	if (value->size()>0) (*value)[0]++;
	value->push_back(9999);	
}


/////////////////////////////////////////////////////////////////////////////////

void InputPointerStdVectorStdVectorInt::fn(std::vector<std::vector<int> >* value)
{
	if (value->size()>0 && (*value)[0].size()>0) ((*value)[0][0])++;
	value->push_back(std::vector<int>());
}


/////////////////////////////////////////////////////////////////////////////////

void InputPointerStdVectorSwigClass::fn(std::vector<SwigClass>* value)
{
	if (value->size()>0) (*value)[0].value++;
	value->push_back(SwigClass());	
}



/////////////////////////////////////////////////////////////////////////////////

void InputPointerStdVectorSmartPointerSwigClass::fn(std::vector< SmartPointer<SwigClass> >*  value)
{
	if (value->size()>0) (*value)[0]->value++;
	value->push_back(SmartPointer<SwigClass>(new SwigClass()));
}



/////////////////////////////////////////////////////////////////////////////////

void InputPointerStdVectorPointerSwigClass::fn(std::vector<SwigClass*>*  value)
{
	if (value->size()>0) (*value)[0]->value++;		
}




/////////////////////////////////////////////////////////////////////////////////

void InputReferenceStdVectorInt::fn(std::vector<int>& value)
{
	if (value.size()>0) value[0]++;
	value.push_back(9999);	
}


/////////////////////////////////////////////////////////////////////////////////

void InputReferenceStdVectorStdVectorInt::fn(std::vector<std::vector<int> >& value)
{
	if (value.size()>0 && value[0].size()>0) (value[0][0])++;
	value.push_back(std::vector<int>());
}


/////////////////////////////////////////////////////////////////////////////////

void InputReferenceStdVectorSwigClass::fn(std::vector<SwigClass>& value)
{
	if (value.size()>0) value[0].value++;
	value.push_back(SwigClass());	
}



/////////////////////////////////////////////////////////////////////////////////

void InputReferenceStdVectorSmartPointerSwigClass::fn(std::vector< SmartPointer<SwigClass> >&  value)
{
	if (value.size()>0) value[0]->value++;
	value.push_back(SmartPointer<SwigClass>(new SwigClass()));
}



/////////////////////////////////////////////////////////////////////////////////

void InputReferenceStdVectorPointerSwigClass::fn(std::vector<SwigClass*>&  value)
{
	if (value.size()>0) value[0]->value++;		
}



/////////////////////////////////////////////////////////////////////////////////

ReturnAtomicType::ReturnAtomicType() {;};

bool           ReturnAtomicType::fn_bool           (bool value)           {return value;}
char           ReturnAtomicType::fn_char           (char value)           {return value;}
unsigned char  ReturnAtomicType::fn_unsigned_char  (unsigned char value)  {return value;}
short          ReturnAtomicType::fn_short          (short value)          {return value;}
unsigned short ReturnAtomicType::fn_unsigned_short (unsigned short value) {return value;}
int            ReturnAtomicType::fn_int            (int value)            {return value;}
unsigned int   ReturnAtomicType::fn_unsigned_int   (unsigned int value)   {return value;}
long           ReturnAtomicType::fn_long           (long value)           {return value;}
unsigned long  ReturnAtomicType::fn_unsigned_long  (unsigned long value)  {return value;}
float          ReturnAtomicType::fn_float          (float value)          {return value;}
double         ReturnAtomicType::fn_double         (double value)         {return value;}
std::string    ReturnAtomicType::fn_std_string     (std::string value)    {return value;}


/////////////////////////////////////////////////////////////////////////////////

ReturnSwigClass::ReturnSwigClass () {;}
ReturnSwigClass::~ReturnSwigClass() {;};
SwigClass ReturnSwigClass::get() {return var;}
Address ReturnSwigClass::get_var_memid() {return AddressOf(&var);}


/////////////////////////////////////////////////////////////////////////////////

ReturnSmartPointerSwigClass::ReturnSmartPointerSwigClass () {var.reset(new SwigClass());}
ReturnSmartPointerSwigClass::~ReturnSmartPointerSwigClass() {var.reset();};
SmartPointer<SwigClass> ReturnSmartPointerSwigClass::get() {return var;}
Address ReturnSmartPointerSwigClass::get_var_memid() {return AddressOf(&*var);}


/////////////////////////////////////////////////////////////////////////////////

ReturnReferenceSwigClass::ReturnReferenceSwigClass () {var=new SwigClass();}
ReturnReferenceSwigClass::~ReturnReferenceSwigClass() {delete var;};
SwigClass& ReturnReferenceSwigClass::get() {return *var;}
Address ReturnReferenceSwigClass::get_var_memid() {return AddressOf(var);}



/////////////////////////////////////////////////////////////////////////////////

ReturnPointerSwigClass::ReturnPointerSwigClass () {var=new SwigClass();}
ReturnPointerSwigClass::~ReturnPointerSwigClass() {delete var;};
SwigClass* ReturnPointerSwigClass::get() {return var;}
Address ReturnPointerSwigClass::get_var_memid() {return AddressOf(var);}





/////////////////////////////////////////////////////////////////////////////////

std::vector<int> ReturnStdVectorInt::fn()
	{return var;}


/////////////////////////////////////////////////////////////////////////////////


std::vector<std::vector<int> > ReturnStdVectorStdVectorInt::fn()
	{return var;}


/////////////////////////////////////////////////////////////////////////////////

std::vector<SwigClass> ReturnStdVectorSwigClass::fn()
	{return var;}


///////////////////////////////////////////////////////////////////////////////// 


std::vector<SmartPointer<SwigClass> > ReturnStdVectorSmartPointerSwigClass::fn()
	{return var;}

        

ReturnStdVectorPointerSwigClass::ReturnStdVectorPointerSwigClass()
	{var.push_back(new SwigClass());}

ReturnStdVectorPointerSwigClass::~ReturnStdVectorPointerSwigClass()
	{for (int i=0;i<var.size();i++)delete var[i];}

std::vector<SwigClass* > ReturnStdVectorPointerSwigClass::fn()
	{return var;}


/////////////////////////////////////////////////////////////////////////////////

std::vector<int>* ReturnPointerStdVectorInt::fn()
	{return &var;}


/////////////////////////////////////////////////////////////////////////////////

std::vector<std::vector<int> >* ReturnPointerStdVectorStdVectorInt::fn()
	{return &var;}

/////////////////////////////////////////////////////////////////////////////////  

std::vector<SwigClass>* ReturnPointerStdVectorSwigClass::fn()
	{return &var;}


///////////////////////////////////////////////////////////////////////////////// 

std::vector<SmartPointer<SwigClass> >* ReturnPointerStdVectorSmartPointerSwigClass::fn()
	{return &var;}

        
/////////////////////////////////////////////////////////////////////////////////

ReturnPointerStdVectorPointerSwigClass::ReturnPointerStdVectorPointerSwigClass()
	{var.push_back(new SwigClass());}

ReturnPointerStdVectorPointerSwigClass::~ReturnPointerStdVectorPointerSwigClass()
	{for (int i=0;i<var.size();i++)delete var[i];}

std::vector<SwigClass* >* ReturnPointerStdVectorPointerSwigClass::fn()
	{return &var;}



 
/////////////////////////////////////////////////////////////////////////////////


std::vector<int>& ReturnReferenceStdVectorInt::fn()
	{return var;}


/////////////////////////////////////////////////////////////////////////////////

std::vector<std::vector<int> >& ReturnReferenceStdVectorStdVectorInt::fn()
	{return var;}


/////////////////////////////////////////////////////////////////////////////////

std::vector<SwigClass>& ReturnReferenceStdVectorSwigClass::fn()
	{return var;}


/////////////////////////////////////////////////////////////////////////////////  


std::vector<SmartPointer<SwigClass> >& ReturnReferenceStdVectorSmartPointerSwigClass::fn()
	{return var;}

        
/////////////////////////////////////////////////////////////////////////////////

ReturnReferenceStdVectorPointerSwigClass::ReturnReferenceStdVectorPointerSwigClass()
	{var.push_back(new SwigClass());}

ReturnReferenceStdVectorPointerSwigClass::~ReturnReferenceStdVectorPointerSwigClass()
	{for (int i=0;i<var.size();i++)delete var[i];}

std::vector<SwigClass* >& ReturnReferenceStdVectorPointerSwigClass::fn()
	{return var;}



