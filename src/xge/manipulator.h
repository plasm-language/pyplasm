#ifndef _MANIPULATOR_H__
#define _MANIPULATOR_H__

#include <xge/xge.h>
#include <xge/box.h>
#include <xge/mat.h>
#include <xge/ray.h>

//=====================================================================  
//! classe per la manipolazione degli oggetti
//=====================================================================       

class  XGE_API Manipulator
{

	SmartPointer<Batch> circle_x,circle_y,circle_z,line_x,line_y,line_z;
	
public:

	//! tipi di manipolazioni supportate
	enum Operation 
	{
		TRANSLATE,
		SCALE,
		ROTATE
	};

    //! epsilon for click
    float EPSILON;
	
	//! freeze transformation in X direction
	bool freeze_x;

	//! freeze transformation in Y direction
	bool freeze_y;

	//! freeze transformation in Z direction
	bool freeze_z;

    //! constructor
	/*!
		Costruisce un nuovo manipolatore

		\param epsilon for tolerance in picking
	*/
	explicit Manipulator(float epsilon=0.1f);

	//! set a new box to manipulate
	/*!
		Ogni volta che vuoi modificare un nuovo oggetto chiama questa funzione.
		Se vuoi essere sicuro che l'oggetto e' manipolabile (ad esempio se un
		certo raggio colpisce l'oggetto) usa la Manipulator::canManipulate()) prima
		di chiamare questa funzione.

		\param box il box di contenimento dell'oggetto da manipolare
		\param T la matrice iniziale di trasformazione del box dell'oggetto
	*/
	void setObject(Box3f box,Mat4f* T);
                
    //! to test when is applying transformation
	/*!
		Per testare se il manipolatore sta utilizzando il mouse
		per manipolare l'oggetto (cioe' e' in ascolto attivo sui
		movimenti del mouse)

		\return true se l'oggetto e' in fase di manipolazione, false altrimenti
	*/
    inline bool isRunning()
		{return this->bRunning;}

    //! test if inside the manipulable area
	/*!
		A partire da un raggio (che ad esempio puo' essere il raggio principale di un frustum
		, vedi Frustum::getRay()), da un box di contenimento e da una matrice iniziale del box
		dice se l'oggetto e' potenzialmente manipolabile o no.

		\param ray il raggio in world space
		\param box il box di contenimento dell'oggetto
		\param T la matrice attuale di trasformazione dell'oggetto
		
		\return true se il box e' manipolabile, false altrimenti
	*/
    static bool canManipulate(Ray3f ray,Box3f box,Mat4f* T);

	//! delegate the mouse press to the manipulator
	/*!
		\param ray the ray in world space
	*/
	inline void Mouse  (MouseEvent args,Ray3f ray);

    //! render
	/*!
		Visualizza il manipolatore (ad esempio le ellissi per la rotazione)
		se un oggetto e' manipolabile e/o in fase di manipolazione
	*/	
	void Render(Engine* engine);

	//! set the current operation
	/*!
		\param operation la nuova operazione (es Manipulator::TRANSLATE,Manipulator::SCALE,Manipulator::ROTATE)
	*/
	void setOperation(Operation operation);

	//! get the current operation

	/*!
		\return operazione corrente (es Manipulator::TRANSLATE,Manipulator::SCALE,Manipulator::ROTATE)
	*/
	Operation getOperation() {return this->operation;}

	//SelfTest
	static void SelfTest();

protected:

	//! reference axis
	enum {INVALID=-1,REF_X=0,REF_Y=1,REF_Z=2};
	int ref;

	//! apply to T or not
	bool bRunning;

	//! current operation
	Operation operation;

	//! current manipulator T_to_box
	Mat4f T_to_box;

	//! the current T_to_box applied to the object
	Mat4f* T;

	//! the original bounding box without transformations (==without T)
	Box3f box;

	//! direct/inverse reference axis
	Mat4f Direct,Inverse;
	Vec3f X,Y,Z,C;

	//! status for the actual transformation
	float cur_angle;
	float cur_position;
	int   cur_sign;

  

}; //end class



#endif //_MANIPULATOR_H__


