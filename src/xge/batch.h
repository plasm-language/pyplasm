#ifndef _BATCH_H__
#define _BATCH_H__

#include <xge/xge.h>

#include <xge/archive.h>
#include <xge/mat.h>
#include <xge/array.h>
#include <xge/texture.h>
#include <xge/box.h>
#include <xge/ray.h>
#include <xge/pick.h>


//===================================================================
//! La classe principale per la visualizzazione di "pezzi" di modelli 
/*!
	Quando un modello viene prodotto dal Plasm, viene in realta' prodotto un grafo
	Hpc che fa riferimento a Graph dove c'e' la geometria in senso stretto.

	Non appena pero' vengono aggiunte proprieta' di rendering (come ad esempio
	texture coordinates, texture1 coordinates, materiali o altro) le strutture
	Plasm vengono convertite in Batch che sono dei job atomici per
	la visualizzazione su scheda grafica (eventualmente trasferendo i buffer
	sulla RAM della GPU).

Esempio python di un Batch che ruota tipo animazione:

class MyRotatingObject(Batch):

	def __init__(self):
		Batch.__init__(self)
		self.primitive=Batch.TRIANGLES #triangles
		self.matrix=Mat4f() #identity matrix
		self.vertices=Array([0,0,0, 1,0,0, 1,1,0,  0,0,0, 1,1,0,0,1,0]) # two triangles (bottom face of a cube)
		self.normals =Array([0,0,1, 0,0,1, 0,0,1,  0,0,1, 0,0,1,0,0,1]) # the normal
		self.texture0=Texture.open("resources/img/gioconda.jpg")
		self.texture0coords=Array([0,0, 1,0, 1,1,  0,0,1,1,0,1])


*/
//===================================================================
class Batch 
{
public:

	//for serialization
	void Write(Archive& ar);
	void Read (Archive& ar);

	//! tipo di primitiva da disegnare (es POINTS, LINES, TRIANGLES,  etc etc)
	enum
	{
		POINTS         = 0x0000,
		LINES          = 0x0001,
		LINE_LOOP      = 0x0002,
		LINE_STRIP     = 0x0003,
		TRIANGLES      = 0x0004,
		TRIANGLE_STRIP = 0x0005,
		TRIANGLE_FAN   = 0x0006,
		QUADS          = 0x0007,
		QUAD_STRIP     = 0x0008,
		POLYGON        = 0x0009
	};
	int primitive; 

	//!materiale da utilizzare
	Color4f ambient;
	Color4f diffuse;
	Color4f specular;
	Color4f emission;
	float   shininess;

	//!transformation matrix, che viene aggiunta (glPushMatrix()) allo stack di trasformazini  corrente
	Mat4f matrix;

	//! buffer di vertici
	/*!
		Nota che per come e' fatto opengl, se vengono vistualizzati dei triangoli
		allora questa variabile conterra' tutti i vertici in streaming (==non esiste 
		indicizzazione e quindi riutilizzo dei vertici)
	*/
	SmartPointer<Array> vertices;

	//! buffer delle normali
	SmartPointer<Array> normals;

	//! buffer dei colori
	SmartPointer<Array> colors;

	//! texture0 (cioe' texture map principale)
	SmartPointer<Texture> texture0;

	//! texture1 coords
	SmartPointer<Array> texture0coords;

	//! texture1
	SmartPointer<Texture> texture1;

	//! texture1 coordinates
	SmartPointer<Array> texture1coords;

	//! default constructor
	/*!
		Setta un Batch vuoto (no vertici, normali, materiali etc)
		Puoi sempre testare se un Batch e' vuoto o pieno vedendo il
		suo primitive che deve essere >= 0 (invece questo costruttore lo imposta
		ad un valore negativo)
	*/
	inline Batch()
	{
		this->primitive=Batch::POINTS;
		this->ambient  =DefaultAmbientColor ;
		this->diffuse  =DefaultDiffuseColor ;
		this->specular =DefaultSpecularColor;
		this->emission =DefaultEmissionColor;
		this->shininess=DefaultShininess    ;
	}

	//!copy constructor
	/*!
		\param src la Batch sorgente dalla quale copiare
	*/
	inline Batch(const Batch& src)
	{
		this->primitive= src.primitive; 
		this->ambient    = src.ambient;
		this->diffuse   = src.diffuse;
		this->specular  = src.specular;
		this->emission  = src.emission;
		this->shininess = src.shininess;
		this->matrix          = src.matrix; 
		this->vertices        = src.vertices; 
		this->normals         = src.normals; 
		this->colors          = src.colors; 
		this->texture0        = src.texture0; 
		this->texture0coords  = src.texture0coords; 
		this->texture1        = src.texture1; 
		this->texture1coords  = src.texture1coords; 
	}

	//! default color
	static Color4f DefaultAmbientColor;
	static Color4f DefaultDiffuseColor;
	static Color4f DefaultSpecularColor;
	static Color4f DefaultEmissionColor;
	static float   DefaultShininess;

	//! if you want to set a color without lighting
	inline void setColor(Color4f c)
	{
		this->ambient=Color4f(c.r,c.g,c.b,1.0f);
		this->diffuse=Color4f(  0,  0,  0,c.a );
	}


	//! when you want to invalidate the bounding box because something changed 
	/*!
		Forza il ricalcolo del Batch::box quando verra' invocata la Batch::getBox3f()
	*/
	inline void invalidateBox()
		{this->box=Box3f();}

	//! special function to extract triangles from indices (internal use)
	/*!
		Se Batch::primitive!=Batch::TRIANGLES viene generata una eccezione

		Questo metodo viene usato per l'unwrapping.

		\param indice dei triangoli da estrarre
	*/
	
	SmartPointer<Batch> getTriangles(const std::vector<int>& triangle_indices);

	//! return the bounding box of the object
	/*!
		\return bounding box del Batch. Se Batch::box e' invalido allora viene ricalcolato automaticamente
	*/
	Box3f getBox();

	//! return a pick, valid or not
	/*!
		Questo metodo funziona solo su Batch::primitive==Batch::TRIANGLES oppure Batch::primitive==Batch::QUADS.
		Nota che se un pick valido e' trovato, la variabile Pick::ids  
		conterra' come primo elemento l'indice del primo vertice del triangolo o del quad colpito

		\param ray il raggio rispetto al quale effettuare il testing
		\return ritorna un pick valido se uno dei triangoli e' colpito dal raggio, altrimenti torna pick non valido
	*/
	Pick getPick(const Ray3f& ray,Mat4f global_matrix);

	//! get normals as a batch (for debugging purpouse)
	SmartPointer<Batch> getNormals();

	//! builders 
	//! create a quad
	static SmartPointer<Batch>               Quad    (int x1,int y1,int x2,int y2,int z=0);
	static SmartPointer<Batch>               Cube   (const Box3f& box);
	static std::vector<SmartPointer<Batch> > Sky    (const Box3f& box,std::string sky_name);
	static SmartPointer<Batch>               Circle (float angle_delta);

	//! open ply
	static std::vector<SmartPointer<Batch> >  openPly(std::string filename,bool bReverseNormals=false);

	static std::vector<SmartPointer<Batch> >  openObj(std::string filename);
	static void                               saveObj(std::string filename,std::vector<SmartPointer<Batch> > batches);

	//! open point cloud (note: the color will be stored in normals)
	static std::vector<SmartPointer<Batch> >  openPtx(std::string filename,bool bUseReflectance=false);

	//! open point cloud in PTS format
	//static std::vector<SmartPointer<Batch> > openPts(std::string filename,bool bUseReflectance=false);

	//!utility to open/save/view
	static void                               Save (std::string filename,std::vector< SmartPointer<Batch> > batches);
	static std::vector< SmartPointer<Batch> > Open (std::string filename);

	//!merge two batch (if possible)
	static SmartPointer<Batch> Merge(SmartPointer<Batch> A,SmartPointer<Batch> B);

	//! optimize batches by merging
	static std::vector<SmartPointer<Batch> > Optimize(std::vector<SmartPointer<Batch> > batches,int max_vertices_per_batch=65536,int max_depth=OCTREE_MAX_DEPTH_DEFAULT,float LOOSE_K=OCTREE_LOOSE_K_DEFAULT) ;

protected:

	//! bounding box
	/*!
		Il bounding box viene automaticamente calcolato dalla classe quando non e' valido.
		Il calcolo avviene applicato la Batch::matrix a tutti i Batch::vertices.
		L'utente puo' forzare il ricalcolo del bounding box (ad esempio perche' ha modificato
		la geometria) con Batch::invalidateBox()
	*/
	Box3f box;

};




#endif // _GPU_BATCH_H__

