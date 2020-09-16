#ifndef _FILE_SYSTEM_H__
#define _FILE_SYSTEM_H__

#include <xge/xge.h>


class FileSystem
{
public:

	//! legge il contenuto di un file
	/*!
		Se il filename termina con .gz allora il file viene automaticamente decompresso (gunzipped)

		\param filename il nome del file da leggere (specificare un path assoluto o relativo alla current working directory)
		\param filesize torna la quantita' di dati letti
		\param bZeroTerminated se il buffer letto non termina con il carattere 0 e bZeroTerminated=true allora viene automaticamente appeso uno 0
		\return il buffer creato (con la MemPool::alloc()) oppure 0 se la lettura fallisce
	*/
	static unsigned char* ReadFile(std::string filename,unsigned long& filesize,bool bZeroTerminated=false);



}; //end class


#endif //_FILE_SYSTEM_H__

