Quando devi costruire una nuova classe C++  che deve essere utilizzata in altri linguaggi 
di programmazione (es Csharp o Python) attraverso il tool SWIG, per evitare 
problemi di memory allocation, usare sempre le regole qui sotto


DEFINIZIONI

	AtomicType:=
		bool
		char
		unsigned char
		short
		unsigned short
		int
		unsigned int
		long
		unsigned long
		float
		double
		std::string
		
	SwigClass:=
		una classe che viene esportata attraverso SWIG
	
	

/////////////////////////////////////////////////////////////////////////
str() and repr() guidelines
///////////////////////////////////////////////////////////////////////// 

E' caldamente consigliato aggiungere i metodi per la stampa di debug
che vengono convertire automaticamente nei linguaggi "ospiti" (C# e Python)

	std::string repr()
		{...your code...}
	
	std::string str()
		{...your code...}
		
		
		
/////////////////////////////////////////////////////////////////////////
Pointer and SmartPointer guidelines 
/////////////////////////////////////////////////////////////////////////

Se una classe viene gestita con gli SmartPointer, usa sempre liste std::vector<SmartPointer<SwigClass> >
e non std::vector<SwigClass> cioe' non mischiare STL vector con e senza smart pointer perche' 
SWIG crea confusione nell'instanziazione dei template
(problema che sembra esserci solo in Python, non in C#)


///////////////////////////////////////////////////////////////////////////////////////
Constructors guidelines 
///////////////////////////////////////////////////////////////////////////////////////

la classe DEVE AVERE il costruttore di default e un costruttore di copia, se questi non sono previsti
metterli comunque e generare al loro interno una eccezione

	SwigClass()
		{...your_code...}
	
	SwigClass()
		{throw throw std::runtime_error("costruttore di default non supportato");}

	SwigClass(const SwigClass&)
		{...your_code...}
	
	SwigClass(const SwigClass&)
		{throw throw std::runtime_error("costruttore di copia non supportato");}
		
		
		
///////////////////////////////////////////////////////////////////////////////////////
Varabili di classe 
/////////////////////////////////////////////////////////////////////////////////////// 

I tipi di variabili che puoi inserire all'interno di una classe devono essere uno dei seguenti casi

	T var;
	
dove T:=
	        AtomicType
	        
	        SwigClass
	        
	        SmartPointer<SwigClass>
	        
	        SwigClass*                                
	        
	        std::vector<AtomicType>
	        
	        std::vector<AtomicType>*                  
	        
	        //Unico caso di STL vector a due livelli attualmente supportato dai template SWIG (modificare il file *.i se si necessitano di altri casi)
	        std::vector<std::vector<int> >  
	        
	        std::vector<std::vector<int> >*           
	        
	        std::vector<SwigClass> 
	        
	        std::vector<SwigClass>*                   
	        
	        std::vector<SmartPointer<SwigClass> >
	        
	        std::vector<SmartPointer<SwigClass> >*    
	        
	        std::vector<SwigClass*>                   
	        
	        std::vector<SwigClass*>*                  
	        
	        
     
[NOTA] 
quando hai puntatori C++ (*) si presuppone che puntino ad istanze che appartengono a SwigClass, 
e che cioe' "vivono" fino al distruttore di SwigClass (es SwigClass*, std::vector<SwigClass>*, etc etc)
Questo e' importante quando consideri i problemi derivanti dal Garbage Collector del linguaggio up-level (c#, python) e il C++.

Ad esempio se ho una classe del tipo:

class SwigClass
{
public:

	std::vector<SwigClass*> childs;
	SwigClass();
	
};
	
e' probabile che il distruttore di SwigClass si preoccupi di deallocare tutte le istanze SwigClass quando
il suo distruttore e' invocato. Se pero' ad esempio in C# avessi inserito un oggetto c#:

	SwigClass father=new SwigClass();
	SwigClass child =new SwigClass();
	father.childs.Add(child);
	
allora quando <father> viene distrutto probabilmente distruggera' tutti i suoi figli,
ma in quel caso la variable C# <child> conterrebbe un riferimento ad un oggetto deallocato
causando un SEGMENTATION FAULT.
	

	       

	        
///////////////////////////////////////////////////////////////////////////////////////////////////////////
Class input arguments guidelines 
///////////////////////////////////////////////////////////////////////////////////////////////////////////

Gli argomenti di ingresso di una funzione della classe possono essere solo di questi tipi
	
class SwigClass
{

public:

	void fn(T);
};
	
dove T:= [const] (opzionale) 
	
			AtomicType   
			
			SwigClass  
			 
			SmartPointer<SwigClass> 
			
			SwigClass&                             
			             
			SwigClass*                                
			
			std::vector<AtomicType>  
			
			//Unico caso di STL vector a due livelli attualmente supportato dai template SWIG (modificare il file *.i se si necessitano di altri casi)
			std::vector<std::vector<int> >            
			
			std::vector<SwigClass>
			std::vector<SmartPointer<SwigClass> >
			std::vector<SwigClass*>                   
			
			std::vector<AtomicType>*                  
			std::vector<std::vector<int> >*          
			std::vector<SwigClass>*                   
			std::vector<SmartPointer<SwigClass> >*   
			std::vector<SwigClass*>*                  
			
			std::vector<AtomicType>&                  
			std::vector<std::vector<int> >&           
			std::vector<SwigClass>&                     
			std::vector<SmartPointer<SwigClass> >&    
			std::vector<SwigClass*>&    
	            
	       
[NOTA ]
Quando ho puntatori C++ o tipi riferimento, mi impegno a leggere la variabile ed eventualmente a modificarla, ma non posso
sapere fino a quando "vive" e quindi non posso salvarmi l'indirizzo per utilizzarlo
successivamente. Il problema se uso gli SmartPointer

	     

///////////////////////////////////////////////////////////////////////////////////////////////////////////
Class return values guidelines
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class SwigClass
{
public:
	
	T fn();
	
}'
	
dove T:=[const] opzionale
	
			AtomicType   
			
			SwigClass  
			
			SmartPointer<SwigClass> 
			
			SwigClass&  [vedi nota (1)]
			         
			SwigClass*  [vedi nota (1)] 
			
			std::vector<AtomicType>  
			
			 //Unico caso di STL vector a due livelli attualmente supportato dai template SWIG (modificare il file *.i se si necessitano di altri casi)
			std::vector<std::vector<int> >          
			
			std::vector<SwigClass>
			std::vector<SmartPointer<SwigClass> >
			std::vector<SwigClass*>                  
			
			std::vector<AtomicType>*
			std::vector<std::vector<int> >*          
			std::vector<SwigClass>*                  
			std::vector<SmartPointer<SwigClass> >*  
			std::vector<SwigClass*>*                
			
			std::vector<AtomicType>&                 
			std::vector<std::vector<int> >&          
			std::vector<SwigClass>&                  
			std::vector<SmartPointer<SwigClass> >&  
			std::vector<SwigClass*>&	                   
	
 [NOTA (1)] 
 tutti i valori di ritorno di tipo puntatore semplice e di tipo reference si assume
 che tornino oggetti di cui SwigClass e' proprietaria, e che "vivono" (==non vengono
 distrutti) fino a che SwigClass "vive". 
 
 Questo e' importante quando consideri  i problemi derivanti dal Garbage Collector del linguaggio up-level (c#, python) e
 il C++.
 

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
ogni volta che una libreria C++ ti restituisce 
un qualcosa (tramite SWIG), tutte il suo contenuto e' proprieta'
della classe (a meno di non usare shared pointer).

Quindi stai molto attento a non tenerti riferimenti appesi.
Ad esempio con la semplice classe Pick:

	class Pick
	{
		Vec3f position
	}


	Pick getPick(...)


se in C# faccio:


List<Vec3f> points;

{
	Pick pick=getPick();
	Vec3f point=pick.position;
	points.add(pick.position); //SBAGLIATO!
}
	
una volta che pick e' deallocato anche pck.position verra' deallocato
e quindi mi ritrovo in points un riferimento ad un elemento che non c'e'
piu'!!!

La stessa cosa vale per le Mesh.Ad esempio in C#

Mesh m=new Mesh;
Batch batch=m.batches[0]; 
QUESTO E' un riferimento interno!!!