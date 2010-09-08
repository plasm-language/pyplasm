#ifndef _FILE_SYSTEM_H__
#define _FILE_SYSTEM_H__

#include <xge/xge.h>


class XGE_API FileSystem
{
public:

	//! all the path which starts with ":" 
	static std::string ResourcesDir;

	//!get Filename for file starting with ":" (means take from ResourcesDir)
	//Example   FullPath(":symbols/videocamera.xml") -> c:/trs2009_resources/videocamera.xml
	static std::string FullPath(std::string filename);

	//!resolve an absolute path, it it starts with ResourcesDir it will be replaced by ":"
	//!Example shortPath("c:/trs2009_resources/videocamera.xml") -> ":symbols/videocamera.xml"
	static std::string ShortPath(std::string filename);

	//! restituisce l'estensione del nome di un file
	/*!
		\param filename il filename di cui trovare l'estensione
		\return l'estensione del file (compreso di punto es .tga), altrimenti "" se il file non esiste
	*/
	 static std::string Extension(std::string filename);

	//! legge il contenuto di un file
	/*!
		Se il filename termina con .gz allora il file viene automaticamente decompresso (gunzipped)

		\param filename il nome del file da leggere (specificare un path assoluto o relativo alla current working directory)
		\param filesize torna la quantita' di dati letti
		\param bZeroTerminated se il buffer letto non termina con il carattere 0 e bZeroTerminated=true allora viene automaticamente appeso uno 0
		\return il buffer creato (con la MemPool::alloc()) oppure 0 se la lettura fallisce
	*/
	static unsigned char* ReadFile(std::string filename,unsigned long& filesize,bool bZeroTerminated=false);

	//! return the file size
	static uint64 FileSize(std::string filename);

	//! test if a file exists
	static bool FileExists(std::string filename);


}; //end class


#endif //_FILE_SYSTEM_H__

