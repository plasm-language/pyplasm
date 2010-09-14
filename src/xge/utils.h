#ifndef _UTILS_H__
#define _UTILS_H__

#include <xge/xge.h>


#ifndef min2
#define min2(a,b) (((a)<=(b))?(a):(b))
#endif

#ifndef max2
#define max2(a,b) (((a)>=(b))?(a):(b))
#endif

#ifndef min3
#define min3(a,b,c) (((a)<=(b))?min2(a,c):min2(b,c))
#endif

#ifndef max3
#define max3(a,b,c) (((a)>=(b))?max2(a,c):max2(b,c))
#endif

#ifndef med3
#define med3(a,b,c) ((min2(a,c)<=(b) && (b)<=max2(a,c))?(b):((min2(a,b)<=(c) && (c)<=max2(a,b))?(c):(a)))
#endif

#ifndef min4
#define min4(a,b,c,d) min2(a,min3(b,c,d))
#endif

#ifndef max4
#define max4(a,b,c,d) max2(a,max3(b,c,d))
#endif

#ifndef Swap
#define Swap(_type,a,b) {_type __tmp__=(a);(a)=(b);(b)=__tmp__;}
#endif


class XGE_API Utils
{
public:

	

	static inline bool IsPower2(int x)
	{
		return (((x) > 0) && (((x) & ((x) - 1)) == 0));
	}

	static inline float Degree2Rad(float a)
	{
		return  (M_PI*((a)/180.0f));
	}

	static inline float Rad2Degree(float a)
	{
		return ((a)*180.0f/M_PI);
	}


	static inline float Clampf(float v,float a,float b)
	{
		return (((v)<(a))?(a):(((v)>(b))?(b):(v)));
	}

	static inline int Clampi(int v,int a,int b)
	{
		return (((v)<(a))?(a):(((v)>(b))?(b):(v)));
	}


	static inline bool Overlap1d(float __a,float __b,float __p,float __q)
	{
		return ((__a)<=(__q) && (__b)>=(__p));
	}


	static inline void Mapf2i(int& ivalue,int ia,int ib,float fvalue,float fa,float fb)
	{
		float __alpha=(float) ( ((fvalue)-(fa)) / ((float)((fb)-(fa))) );
		(ivalue)=(int)( (ia) + __alpha * ((ib)-(ia)) );
		(ivalue)=Clampi( (ivalue) , (ia) , (ib) );
	}


	static inline void Mapi2f(float& fvalue,float fa,float fb,int ivalue,int ia,int ib)
	{ 
		float __alpha=(float)( ((ivalue)-(ia)) / ((float)((ib)-(ia))) );
		(fvalue)=(float)( (fa) + __alpha * ((fb)-(fa)) );
		(fvalue)=Clampf( (fvalue) , (fa) , (fb) );
	}


	//produce an error message
	static void Error(std::string location,const char * format, ...);

	//format a string
	static std::string Format(const char * format, ...);

	//parser ints
	static void ParseInts(int num,int* dest,const char* string_source,char* string_format);

	//parse floats
	static void ParseFloats(int num,float* dest,const char* string_source,char* string_format);

	//float random number  in range [a,b]
	static inline float FloatRand(float a,float b)
	{
		float ret=(a + ((b-a) * (rand()/(float)RAND_MAX)));
		if (ret<a) ret=a;
		if (ret>b) ret=b;
		return ret;
	}

	//integer random number in range [a,b]
	static inline int IntRand(int a,int b)
	{
		int ret=(int)(((float)a + ((float)(b-a) * (rand()/(float)RAND_MAX))));
		if (ret<a) ret=a;
		if (ret>b) ret=b;
		return ret;
	}

	//text if two floats are almost the same
	static inline bool FuzzyEqual(float a,float b,float epsilon=1e-8)
	{
		return (a>=b)?((a-b)<epsilon):((b-a)<epsilon);
	}

	//Log2 for integer
	static inline int Log2i(int v)
	{
		unsigned r = 0; 
		while (v >>= 1) r++;
		return r;
	}

	//Pow2 for integer
	static inline int Pow2i(int n)
	{
		return 1<<n;
	}


	//! Utility for string replace (always case sensitive!)
	static inline std::string Replace(std::string source, std::string find, std::string replace,bool caseSensitive=true) 
	{
		//TODO: other cases
		ReleaseAssert(caseSensitive==true);

		for (int j;(j=(int)source.find(find))!=std::string::npos;)
			source.replace(j, find.length(),replace);

		return source;
	}

	//! test if a string start with a certain string
	static inline bool StartsWith(std::string s,std::string what,bool caseSensitive=false)
	{
		if (s.length()<what.length())
			return false;

		for (int i=0;i<(int)what.length();i++)
		{
			int c1=caseSensitive?s[i]:tolower(s[i]);
			int c2=caseSensitive?what[i]:tolower(what[i]);
			if (c1!=c2) return false;
		}

		return true;
	}

	static inline std::string ToLower(std::string value)
	{
		std::string ret(value);
		std::transform(ret.begin(), ret.end(), ret.begin(), (int(*)(int)) std::tolower);
		return ret;
	}

	//static inline std::string ToUpper(std::string value)
	//{
	//	std::string ret(value);
	//	std::transform(ret.begin(), ret.end(), ret.begin(), (int(*)(int)) std::toupper);
	//	return ret;
	//}


	static inline std::string RTrim (std::string value, std::string spaces = " \t\r\n")
	{ 
		std::string ret(value); 
		std::string::size_type i (ret.find_last_not_of (spaces));
		if (i == std::string::npos) return "";
		return ret.erase (ret.find_last_not_of(spaces) + 1) ; 
	}  

	static inline std::string LTrim(std::string value, std::string spaces = " \t\r\n") 
	{ 
		std::string ret(value); 
		return ret.erase(0,value.find_first_not_of (spaces)); 
	} 

	static inline std::string Trim(std::string value, std::string spaces = " \t\r\n")
	{ 
		std::string ret(value); 
		return LTrim(RTrim (ret, spaces), spaces) ; 
	} 




	//STL index of an element
	template <class T>
	static inline int IndexOf(const std::vector<T>& v,const T& obj)
	{
		for (int i=0;i<(int)v.size();i++)
			if (v[i]==obj) return i;
		return -1;
	}

	//STL containment of item in a vector
	template <class T>
	static inline bool Contains(const std::vector<T>& v,const T& obj)
	{
		return IndexOf(v,obj)>=0;
	}

	//remove item from STL vector
	template <class T>
	static inline std::vector<T> RemoveAll(const std::vector<T>& v,const T& obj)
	{
		std::vector<T> ret;
		for (int i=0;i<(int)v.size();i++)
			if (v[i]!=obj) ret.push_back(v[i]);
		return ret;
	}

	//remove item from STL vector
	template <class T>
	static inline std::vector<T> RemoveAt(const std::vector<T>& v,int pos)
	{
		assert(pos>=0 && pos<(int)v.size());

		std::vector<T> ret;

		for (int i=0;i<(int)v.size();i++)
			if (i!=pos) ret.push_back(v[i]);

		return ret;
	}

	//remove last item from STL vector
	template <class T>
	static inline std::vector<T> RemoveLast(const std::vector<T>& v)
	{
		assert(v.size());
		return RemoveAt(v,(int)v.size()-1);
	}


	//insert an intem in a stl vector
	template <class T>
	static inline std::vector<T> Insert(const std::vector<T>& v,int pos,const T& obj)
	{
		assert(pos>=0 && pos<=(int)v.size());
		std::vector<T> ret;

		if (!v.size())
		{
			ret.push_back(obj);
		}
		else
		{
			for (int i=0;i<(int)v.size();i++)
			{
				if (i==pos) ret.push_back(obj);
				ret.push_back(v[i]);
			}

			if (pos==(int)v.size()) 
				ret.push_back(obj);
		}

		return ret;
	}

	//! self test
	static int SelfTest();

}; //class utils


#endif //_UTILS_H__