#ifndef _CLOCK_H__
#define _CLOCK_H__

#include <xge/xge.h>



//===========================================================================
//! Classe per il timing molto preciso
/*!
Esempio di uso
	Clock t1;
	...
	ti.msec();
*/
//===========================================================================

class XGE_API Clock
{
public:

	//! default constructor
	/*!
		Imposta il clock attuale

		@py
		c=Clock()
		@endpy
	*/
	inline Clock()
	{
		reset();
	}

	//! operator -
	int operator-(const Clock& a) const
	{ 
		#ifdef _WINDOWS
		LARGE_INTEGER frequency; 
		QueryPerformanceFrequency(&frequency);
		return (int)((this->timestamp.QuadPart-a.timestamp.QuadPart) * 1000.0f/frequency.QuadPart);
		#else
		const timeval& lcurrent     = this->timestamp;
		const timeval& m_LastCount  = a.timestamp;
		long msec=0;
		msec+= (lcurrent.tv_sec -  m_LastCount.tv_sec ) * 1000;
		msec+= (lcurrent.tv_usec - m_LastCount.tv_usec) /1000;
		return msec;
		#endif
	}

	//!assignment operator (side effect)
	/*!
		\param src il clock sorgente rispetto cui effettuare la copia
		\retun modifica la classe e torna la classe stessa

		@py
		c1=Clock()
		c2=Clock()
		c1.assign(c2)
		@endpy
	*/
	Clock& operator=(const Clock& src) 
	{
		this->timestamp=src.timestamp;
		return *this;
	}

	//! reset 
	void reset()
	{
		#ifdef _WINDOWS
		QueryPerformanceCounter(&timestamp);
		#else
		gettimeofday(&timestamp, NULL);
		#endif
	}

	//! Tempo trascorso in secondi
	/*!
		\return il tempo trascorso in secondi
		
		@py
		c1=Clock()
		assert c1.sec()<0.010 # assume has not passed 10 msec
		@endpy
	*/
	inline float sec() const
	{
		return 0.001f*msec();
	}

	//! tempo trascorso in millisecondi
	/*!
		\return il tempo trascorso in millisecondi

		see Clock::operator-(const Clock&)
	*/
	inline int msec() const
	{
		return Clock()-(*this);
	}

protected:

	//! internal timestamp, dipendente dalla piattaforma
	#ifdef _WINDOWS
	LARGE_INTEGER timestamp;
	#else	
	timeval timestamp;
	#endif

}; //end class





#endif //_CLOCK_H__


