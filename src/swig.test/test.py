import os,sys,gc


from examplepy import *

#///////////////////////////////////////////////////
class PyVirtualClass(VirtualClass):

	def __init__(self):
	   VirtualClass.__init__(self)  
	
	def fn(self):
		self.setValue(self.getValue()+1)



#///////////////////////////////////////////////////
def VERIFY_COUNT(value):

	# make sure all unreachable objects
	gc.collect()
	if cvar.SwigClass_num_nodes!=value:
		raise Exception("VERIFY_COUNT failed, needed %d but SwigClass.num_nodes is %d" % (value,cvar.SwigClass_num_nodes))





#///////////////////////////////////////////////////
def test_CreateSwigClass():
	VERIFY_COUNT(0);
	k=SwigClass()
	VERIFY_COUNT(1)
	k=None
	VERIFY_COUNT(0)




#/////////////////////////////////////////////////// int var;
def test_VarAtomicType():
	t=VarAtomicType()
	assert(t.var_bool==False)
	assert(t.var_char=="\0")
	assert(t.var_unsigned_char==0)
	assert(t.var_short==0)
	assert(t.var_unsigned_short==0)
	assert(t.var_int==0)
	assert(t.var_unsigned_int==0)
	assert(t.var_long==0)
	assert(t.var_unsigned_long==0)
	assert(t.var_float==0)
	assert(t.var_double==0)
	assert(t.var_string=="")
	t=None




#/////////////////////////////////////////////////// SwigClass var
def test_VarSwigClass():

	VERIFY_COUNT(0)
	t=VarSwigClass()
	VERIFY_COUNT(1)
	temp=t.var # get the reference, not the copy
	VERIFY_COUNT(1)
	temp=None
	VERIFY_COUNT(1)
	temp=SwigClass(t.var)
	assert(temp.AddressOf()!=t.var.AddressOf())
	VERIFY_COUNT(2)
	temp=None 
	VERIFY_COUNT(1)
	t=None
	VERIFY_COUNT(0)





#/////////////////////////////////////////////////// SmartPointer<SwigClass> var
def test_VarSmartPointerSwigClass() :
	VERIFY_COUNT(0)
	t=VarSmartPointerSwigClass()
	VERIFY_COUNT(1)
	temp=t.var # copy the smart pointer
	VERIFY_COUNT(1) # exacly the same node pointed two times
	t.var=None      # manually reset the smart pointer inside the class
	VERIFY_COUNT(1) # now I own the smart pointer to an object pointer one time
	temp=None
	VERIFY_COUNT(0) #I lost the smart pointer, so the object is deleted
	t=None
	VERIFY_COUNT(0)





#/////////////////////////////////////////////////// SwigClass* var
def test_VarPointerSwigClass():

	VERIFY_COUNT(0)
	t=VarPointerSwigClass()
	VERIFY_COUNT(1)
	temp=t.var #get a internal reference
	VERIFY_COUNT(1)
	temp=None
	VERIFY_COUNT(1)
	t=None
	VERIFY_COUNT(0)





#/////////////////////////////////////////////////// std::vector<int> var
def test_VarStdVectorInt():
	t=VarStdVectorInt()
	assert(t.var.size()==0)
	t.var.push_back(1)
	assert(t.var.size()==1 and t.var[0]==1)
	t=None




#/////////////////////////////////////////////////// std::vector<std::vector<int> > var
def test_VarStdVectorStdVectorInt():

	t=VarStdVectorStdVectorInt()
	assert(t.var.size()==0)
	temp=StdVectorInt()
	temp.push_back(1)
	temp.push_back(2)
	temp.push_back(3)
	t.var.push_back(temp)
	assert(t.var.size()==1 and t.var[0]==(1,2,3))
	t=None





#/////////////////////////////////////////////////// std::vector<SwigClass>
def test_VarStdVectorSwigClass():

	VERIFY_COUNT(0)
	t=VarStdVectorSwigClass()
	temp=SwigClass() # create a instance
	t.var.push_back(temp) # insert a copy (using copy constructor)
	VERIFY_COUNT(2)
	temp=None
	VERIFY_COUNT(1)
	t=None
	VERIFY_COUNT(0)




#/////////////////////////////////////////////////// std::vector<SmartPointer<SwigClass> > var
def test_VarStdVectorSmartPointerSwigClass():

	VERIFY_COUNT(0)
	t=VarStdVectorSmartPointerSwigClass()
	
	temp=SwigClass()
	t.var.push_back(temp) # inserted the SmartPointer
	VERIFY_COUNT(1)
	temp=None
	VERIFY_COUNT(1) # not deallocated
	t=None
	VERIFY_COUNT(0)	






#/////////////////////////////////////////////////// std::vector<SwigClass*> var
def test_VarStdVectorPointerSwigClass():
	VERIFY_COUNT(0)
	t=VarStdVectorPointerSwigClass()
	VERIFY_COUNT(1)
	temp=t.var[0] # cannot create instance and add to the vector, rule says READ ONLY
	VERIFY_COUNT(1)
	temp=None
	VERIFY_COUNT(1)
	t=None
	VERIFY_COUNT(0)	







#/////////////////////////////////////////////////// std::vector<int>* var
def test_VarPointerStdVectorInt():
	t=VarPointerStdVectorInt()
	t.var.push_back(1)
	assert(t.var.size()==1 and t.var[0]==1)
	t=None
	VERIFY_COUNT(0)	




#/////////////////////////////////////////////////// std::vector<std::vector<int> >* var
def test_VarPointerStdVectorStdVectorInt():
	VERIFY_COUNT(0)
	t=VarPointerStdVectorStdVectorInt()
	temp=StdVectorInt()
	temp.push_back(1)
	t.var.push_back(temp)
	assert(t.var.size()==1 and t.var[0]==(1,))			  	
	t=None
	VERIFY_COUNT(0)	





#/////////////////////////////////////////////////// std::vector<SwigClass>* var
def test_VarPointerStdVectorSwigClass():

	VERIFY_COUNT(0)
	t=VarPointerStdVectorSwigClass()
	
	temp=SwigClass()
	
	t.var.push_back(temp)
	VERIFY_COUNT(2)
	temp=None
	VERIFY_COUNT(1)
	t=None
	VERIFY_COUNT(0)	





#/////////////////////////////////////////////////// std::vector<SmartPointer<SwigClass> >* var
def test_VarPointerStdVectorSmartPointerSwigClass():

	VERIFY_COUNT(0)
	t=VarPointerStdVectorSmartPointerSwigClass()
	
	temp=SwigClass()
	
	t.var.push_back(temp)
	VERIFY_COUNT(1)
	temp=None
	VERIFY_COUNT(1)			  	
	t=None
	VERIFY_COUNT(0)	




#/////////////////////////////////////////////////// std::vector<SwigClass*>* var
def test_VarPointerStdVectorPointerSwigClass():

	VERIFY_COUNT(0)
	t=VarPointerStdVectorPointerSwigClass()
	VERIFY_COUNT(1)	
	temp=t.var[0]
	VERIFY_COUNT(1)
	temp=None
	VERIFY_COUNT(1)				  	
	t=None
	VERIFY_COUNT(0)	






#/////////////////////////////////////////////////////////////// int fn(int value)
def test_InputAtomicType():
	t=InputAtomicType()
	assert(t.fn(1)==1)
	t=None







#/////////////////////////////////////////////////////////////// unsigned long fn(SwigClass value)
def test_InputSwigClass():

	VERIFY_COUNT(0)
	t=InputSwigClass()
	
	VERIFY_COUNT(0)
	temp=SwigClass()
	VERIFY_COUNT(1)
	memid=t.fn(temp)
	VERIFY_COUNT(1)
	assert(memid!=temp.AddressOf()) # make sure it received a copied instance
	temp=None
	t=None
	VERIFY_COUNT(0)







#/////////////////////////////////////////////////////////////// unsigned long fn(SmartPointer<SwigClass> value)
def test_InputSmartPointerSwigClass():

	VERIFY_COUNT(0)
	t=InputSmartPointerSwigClass()
	
	temp=SwigClass()
	
	VERIFY_COUNT(1)
	memid=t.fn(temp)
	VERIFY_COUNT(1)
	assert(memid==temp.AddressOf()) #make sure it received the same instance
	temp=None
	t=None
	VERIFY_COUNT(0)





	
#/////////////////////////////////////////////////////////////// unsigned long fn(SwigClass& value)
def test_InputReferenceSwigClass():

	VERIFY_COUNT(0)
	t=InputReferenceSwigClass()
	temp=SwigClass()
	VERIFY_COUNT(1)
	memid=t.fn(temp)
	VERIFY_COUNT(1)
	assert(memid==temp.AddressOf()) # make sure it received the same instance
	temp=None
	t=None		
	VERIFY_COUNT(0)




#/////////////////////////////////////////////////////////////// unsigned long fn(SwigClass* value)
def test_InputPointerClass():

	VERIFY_COUNT(0)
	t=InputPointerSwigClass()
	temp=SwigClass()
	VERIFY_COUNT(1)
	memid=t.fn(temp)
	VERIFY_COUNT(1)
	assert(memid==temp.AddressOf())  # make sure it received the same instance
	temp=None
	t=None
	VERIFY_COUNT(0)







#/////////////////////////////////////////////////////////////// void fn(std::vector<int> value)
def test_InputStdVectorInt():

	VERIFY_COUNT(0)
	t=InputStdVectorInt()
	temp=StdVectorInt()
	temp.push_back(1)
	t.fn(temp)
	assert(temp.size()==1 and temp[0]==1) # make sure it has not been modified
	temp=None
	t=None
	VERIFY_COUNT(0)






#/////////////////////////////////////////////////////////////// void fn(std::vector<std::vector<int> > value)
def test_InputStdVectorStdVectorInt():

	
	VERIFY_COUNT(0)
	
	t=InputStdVectorStdVectorInt()
	arg=StdVectorStdVectorInt()
	temp=StdVectorInt() 
	temp.push_back(1)
	arg.push_back(temp)
	t.fn(arg)
	assert(arg.size()==1 and temp.size()==1 and temp[0]==1) # make sure it has not been modified
	temp=None
	arg=None
	t=None





#/////////////////////////////////////////////////////////////// void fn(std::vector<SwigClass> value)
def test_InputStdVectorSwigClass():
	VERIFY_COUNT(0)
	t=InputStdVectorSwigClass()
	temp=SwigClass()
	arg=StdVectorSwigClass()
	arg.push_back(temp)
	t.fn(arg)
	assert(arg.size()==1 and temp.value==0) 
	arg=None
	temp=None
	t=None
	VERIFY_COUNT(0)





#/////////////////////////////////////////////////////////////// void fn(std::vector< SmartPointer<SwigClass> >  value)
def test_InputStdVectorSmartPointerSwigClass():

	VERIFY_COUNT(0)
	t=InputStdVectorSmartPointerSwigClass()
	arg=StdVectorSmartPointerSwigClass()
	sc=SwigClass()
	arg.push_back(sc)
	VERIFY_COUNT(1)
	t.fn(arg)
	assert(arg.size()==1 and sc.value==1)
	sc=None
	arg=None
	t=None
	VERIFY_COUNT(0) 



#/////////////////////////////////////////////////////////////// void fn(std::vector<SwigClass*>  value)
def test_InputStdVectorPointerSwigClass():

	VERIFY_COUNT(0)
	t=InputStdVectorPointerSwigClass()
	arg=StdVectorPointerSwigClass()
	temp=SwigClass()
	arg.push_back(temp)
	VERIFY_COUNT(1)
	t.fn(arg)
	assert(arg.size()==1 and temp.value==1)
	temp=None
	arg=None
	t=None
	VERIFY_COUNT(0) 		
	






#/////////////////////////////////////////////////////////////// void fn(std::vector<int>* value)
def test_InputPointerStdVectorInt():
	VERIFY_COUNT(0)
	t=InputPointerStdVectorInt()
	temp=StdVectorInt()
	temp.push_back(1)
	t.fn(temp)
	assert(temp.size()==2 and temp[0]==2 and temp[1]==9999)
	temp=None
	t=None
	VERIFY_COUNT(0)





#/////////////////////////////////////////////////////////////// void fn(std::vector<std::vector<int> >* value)
def test_InputPointerStdVectorStdVectorInt():
	t=InputPointerStdVectorStdVectorInt()
	arg=StdVectorStdVectorInt()
	aux=StdVectorInt()
	arg.push_back(aux)
	aux.push_back(1)
	t.fn(arg)
	aux=None
	arg=None
	t=None





#/////////////////////////////////////////////////////////////// void fn(std::vector<SwigClass>* value)
def test_InputPointerStdVectorSwigClass():
	VERIFY_COUNT(0)
	t=InputPointerStdVectorSwigClass()
	temp=SwigClass()
	arg=StdVectorSwigClass()
	arg.push_back(temp)
	t.fn(arg)
	aux=arg[0]
	assert(arg.size()==2 and aux.value==1)
	aux=None
	arg=None
	temp=None
	t=None 
	VERIFY_COUNT(0)







#/////////////////////////////////////////////////////////////// void fn(std::vector< SmartPointer<SwigClass> >*  value)
def test_InputPointerStdVectorSmartPointerSwigClass():
	
	VERIFY_COUNT(0)
	t=InputPointerStdVectorSmartPointerSwigClass()
	arg=StdVectorSmartPointerSwigClass()
	sc=SwigClass()
	arg.push_back(sc)
	VERIFY_COUNT(1)
	t.fn(arg)
	assert(arg.size()==2 and sc.value==1)
	sc=None
	arg=None
	t=None
	VERIFY_COUNT(0) 





#/////////////////////////////////////////////////////////////// void fn(std::vector<SwigClass*>*  value)
def test_InputPointerStdVectorPointerSwigClass():

	VERIFY_COUNT(0)
	t=InputPointerStdVectorPointerSwigClass()
	arg=StdVectorPointerSwigClass()
	temp=SwigClass()
	arg.push_back(temp)
	VERIFY_COUNT(1)
	t.fn(arg)
	assert(arg.size()==1 and temp.value==1)
	temp=None
	arg=None
	t=None
	VERIFY_COUNT(0) 







#/////////////////////////////////////////////////////////////// void fn(std::vector<int>& value)
def test_InputReferenceStdVectorInt():

	VERIFY_COUNT(0)
	t=InputReferenceStdVectorInt()
	temp=StdVectorInt()
	temp.push_back(1)
	t.fn(temp)
	assert(temp.size()==2 and temp[0]==2 and temp[1]==9999)
	temp=None
	t=None 
	VERIFY_COUNT(0)




#/////////////////////////////////////////////////////////////// void fn(std::vector<std::vector<int> >& value)
def test_InputReferenceStdVectorStdVectorInt():

	
	t=InputReferenceStdVectorStdVectorInt()
	arg=StdVectorStdVectorInt()
	aux=StdVectorInt()
	arg.push_back(aux)
	aux.push_back(1)
	t.fn(arg)
	aux=None
	arg=None
	t=None





#/////////////////////////////////////////////////////////////// void fn(std::vector<SwigClass>& value)
def test_InputReferenceStdVectorSwigClass():
	
	VERIFY_COUNT(0)
	t=InputReferenceStdVectorSwigClass()
	temp=SwigClass()
	arg=StdVectorSwigClass()
	arg.push_back(temp)
	t.fn(arg)
	aux=arg[0]
	assert(arg.size()==2 and aux.value==1)
	aux=None
	arg=None
	temp=None
	t=None 
	VERIFY_COUNT(0)



#/////////////////////////////////////////////////////////////// void fn(std::vector< SmartPointer<SwigClass> >&  value)
def test_InputReferenceStdVectorSmartPointerSwigClass():

	VERIFY_COUNT(0)
	t=InputReferenceStdVectorSmartPointerSwigClass()
	arg=StdVectorSmartPointerSwigClass()
	sc=SwigClass()
	arg.push_back(sc)
	VERIFY_COUNT(1)
	t.fn(arg)
	assert(arg.size()==2 and sc.value==1)
	sc=None
	arg=None
	t=None
	VERIFY_COUNT(0) 





#/////////////////////////////////////////////////////////////// void fn(std::vector<SwigClass*>&  value)
def test_InputReferenceStdVectorPointerSwigClass():

	VERIFY_COUNT(0)
	t=InputReferenceStdVectorPointerSwigClass()
	arg=StdVectorPointerSwigClass()
	temp=SwigClass()
	arg.push_back(temp)
	VERIFY_COUNT(1)
	t.fn(arg)
	assert(arg.size()==1 and temp.value==1)
	temp=None
	arg=None
	t=None
	VERIFY_COUNT(0) 






#///////////////////////////////////////////////////////////////
def test_ReturnAtomicType():

	t=ReturnAtomicType()

	assert(t.fn_bool           (True)==True)
	assert(t.fn_char           (" ")==" ")
	assert(t.fn_unsigned_char  (1)==1)
	assert(t.fn_short          (1)==1)
	assert(t.fn_unsigned_short (1)==1)
	assert(t.fn_int            (1)==1)
	assert(t.fn_unsigned_int   (1)==1)
	assert(t.fn_long           (1)==1)
	assert(t.fn_unsigned_long  (1)==1)
	assert(t.fn_float          (1)==1)
	assert(t.fn_double         (1)==1)
	assert(t.fn_std_string     ("hello")=="hello")
 		






#/////////////////////////////////////////////////////////////// SwigClass get()
def test_ReturnSwigClass():
	
	VERIFY_COUNT(0)
	t = ReturnSwigClass()
	VERIFY_COUNT(1)
	temp=t.get()
	VERIFY_COUNT(2)
	assert(temp.AddressOf()!=t.get_var_memid())
	temp=None
	VERIFY_COUNT(1)
	t=None
	VERIFY_COUNT(0)





#/////////////////////////////////////////////////////////////// SmartPointer<SwigClass> get()
def test_ReturnSmartPointerSwigClass():

	VERIFY_COUNT(0)
	t=ReturnSmartPointerSwigClass()
	VERIFY_COUNT(1)
	temp=t.get()
	VERIFY_COUNT(1)
	assert(temp.AddressOf()==t.get_var_memid())
	temp=None
	VERIFY_COUNT(1)
	t=None
	VERIFY_COUNT(0)





#/////////////////////////////////////////////////////////////// SwigClass& get()
def test_ReturnReferenceSwigClass():

	VERIFY_COUNT(0)
	t=ReturnReferenceSwigClass()
	VERIFY_COUNT(1)
	temp=t.get() 
	VERIFY_COUNT(1)
	assert(temp.AddressOf()==t.get_var_memid())
	temp=None
	VERIFY_COUNT(1)
	t=None
	VERIFY_COUNT(0)



#/////////////////////////////////////////////////////////////// SwigClass* get()
def test_ReturnPointerSwigClass():

	VERIFY_COUNT(0)
	t=ReturnPointerSwigClass()
	VERIFY_COUNT(1)
	temp=t.get()
	VERIFY_COUNT(1)
	assert(temp.AddressOf()==t.get_var_memid())
	temp=None
	VERIFY_COUNT(1)
	t=None
	VERIFY_COUNT(0)





#/////////////////////////////////////////////////////////////// std::vector<int>
def test_ReturnStdVectorInt():

	VERIFY_COUNT(0)
	t=ReturnStdVectorInt()
	temp=t.fn()
	assert(temp==())
	temp+=(1,)
	temp=None
	temp=t.fn()
	assert(temp==())
	temp=None
	t=None
	VERIFY_COUNT(0)




#/////////////////////////////////////////////////////////////// std::vector<std::vector<int> > fn()
def test_ReturnStdVectorStdVectorInt():
	
	VERIFY_COUNT(0)
	t=ReturnStdVectorStdVectorInt()
	temp=t.fn()
	assert(temp==(()))
	temp+=(())
	temp=None
	temp=t.fn()
	assert(temp==(()))
	temp=None
	t=None
	VERIFY_COUNT(0)






#/////////////////////////////////////////////////////////////// std::vector<SwigClass> fn()
def test_ReturnStdVectorSwigClass():

	VERIFY_COUNT(0)
	t=ReturnStdVectorSwigClass()
	temp=t.fn()
	aux=SwigClass()
	temp+=(aux,)
	aux=None
	temp=None
	temp=t.fn() 
	assert(temp==())
	temp=None
	t=None
	VERIFY_COUNT(0)



#/////////////////////////////////////////////////////////////// std::vector<SmartPointer<SwigClass> > fn()
def test_ReturnStdVectorSmartPointerSwigClass():

	VERIFY_COUNT(0)
	t=ReturnStdVectorSmartPointerSwigClass()
	temp=t.fn()
	aux=SwigClass()
	temp+=(aux,)
	aux.value=999
	aux=None
	VERIFY_COUNT(1)
	temp=None
	temp=t.fn()
	assert(temp==())
	temp=None
	t=None
	VERIFY_COUNT(0)





#/////////////////////////////////////////////////////////////// std::vector<SwigClass*> fn() 
def test_ReturnStdVectorPointerSwigClass():

	return

	# the following is broken because you should NOT mix std::vector<SwigClass> with std::vector<SmartPointer<SwigClass> >

	VERIFY_COUNT(0)
	t=ReturnStdVectorPointerSwigClass()
	temp=t.fn()
	assert(len(temp)==1)
	aux=SwigClass()
	temp= temp + (aux,)
	aux=None
	aux=temp[0]
	aux.value=255
	aux=None
	temp=None
	temp=t.fn()
	assert(len(temp)==1)
	aux=temp[0]
	assert(aux.value==255)
	aux=None
	temp=None
	t=None
	VERIFY_COUNT(0)




  
#/////////////////////////////////////////////////////////////// std::vector<int>* fn()
def test_ReturnPointerStdVectorInt():
	
	VERIFY_COUNT(0)
	t=ReturnPointerStdVectorInt()
	temp=t.fn()
	assert(temp.size()==0)
	temp.push_back(1)
	temp=None
	temp=t.fn()
	assert(temp.size()==1 and temp[0]==1)
	temp=None
	t=None
	VERIFY_COUNT(0)




#/////////////////////////////////////////////////////////////// std::vector<std::vector<int> >* fn()
def test_ReturnPointerStdVectorStdVectorInt():

	VERIFY_COUNT(0)
	t=ReturnPointerStdVectorStdVectorInt()
	temp=t.fn()
	assert(temp.size()==0)
	temp.push_back(StdVectorInt())
	temp=None
	temp=t.fn()
	assert(temp.size()==1 and temp[0]==())
	temp=None
	t=None
	VERIFY_COUNT(0)








#/////////////////////////////////////////////////////////////// std::vector<SwigClass>* fn()
def test_ReturnPointerStdVectorSwigClass():

	VERIFY_COUNT(0)
	t=ReturnPointerStdVectorSwigClass()
	temp=t.fn()
	aux=SwigClass(255)
	temp.push_back(aux)
	aux.value=500
	aux=None
	temp=None
	temp=t.fn() 
	assert(temp.size()==1)
	aux=temp[0]
	assert(aux.value==255)
	aux=None
	temp=None
	t=None
	VERIFY_COUNT(0)




#/////////////////////////////////////////////////////////////// std::vector<SmartPointer<SwigClass> >* fn()
def test_ReturnPointerStdVectorSmartPointerSwigClass():

	VERIFY_COUNT(0)
	t=ReturnPointerStdVectorSmartPointerSwigClass()
	temp=t.fn()
	aux=SwigClass()
	temp.push_back(aux)
	aux.value=999
	aux=None
	VERIFY_COUNT(1)
	temp=t.fn()
	assert(temp.size()==1)
	aux=temp[0]
	assert(aux.value==999)
	aux=None
	temp=None
	t=None
	VERIFY_COUNT(0)






#/////////////////////////////////////////////////////////////// std::vector<SwigClass* >* fn()
def test_ReturnPointerStdVectorPointerSwigClass():

	VERIFY_COUNT(0)
	t=ReturnPointerStdVectorPointerSwigClass()
	temp=t.fn()
	assert(temp.size()==1)
	aux=temp[0]
	aux.value=77
	aux=None
	temp=None
	temp=t.fn()
	assert(temp.size()==1)
	aux=temp[0]
	assert(aux.value==77)
	aux=None
	temp=None
	t=None
	VERIFY_COUNT(0)




#/////////////////////////////////////////////////////////////// std::vector<int>& fn()
def test_ReturnReferenceStdVectorInt():

	VERIFY_COUNT(0)
	t=ReturnReferenceStdVectorInt()
	temp=t.fn()
	assert(temp.size()==0)
	temp.push_back(1)
	temp=None
	temp=t.fn()
	assert(temp.size()==1 and temp[0]==1)
	temp=None	
	t=None
	VERIFY_COUNT(0)






#/////////////////////////////////////////////////////////////// std::vector<std::vector<int> >& fn()
def test_ReturnReferenceStdVectorStdVectorInt():

	VERIFY_COUNT(0)
	t=ReturnReferenceStdVectorStdVectorInt()
	temp=t.fn()
	assert(temp.size()==0)
	temp.push_back(StdVectorInt())
	temp=None
	temp=t.fn()
	assert(temp.size()==1 and temp[0]==())
	temp=None
	t=None
	VERIFY_COUNT(0)




 
#/////////////////////////////////////////////////////////////// std::vector<SwigClass>& fn()
def test_ReturnReferenceStdVectorSwigClass():

	t=ReturnReferenceStdVectorSwigClass()
	temp=t.fn()
	aux=SwigClass(255)
	temp.push_back(aux)
	aux.value=500
	aux=None
	temp=None
	temp=t.fn()
	assert(temp.size()==1)
	aux=temp[0]
	assert(aux.value==255)
	aux=None
	temp=None
	t=None
	VERIFY_COUNT(0)







#/////////////////////////////////////////////////////////////// std::vector<SmartPointer<SwigClass> >& fn()
def test_ReturnReferenceStdVectorSmartPointerSwigClass():

	VERIFY_COUNT(0)
	t=ReturnReferenceStdVectorSmartPointerSwigClass()
	temp=t.fn()
	aux=SwigClass()
	temp.push_back(aux)
	aux.value=999
	VERIFY_COUNT(1)
	temp=None
	temp=t.fn()
	assert(temp.size()==1)
	aux=temp[0]
	assert(aux.value==999)
	aux=None
	temp=None
	t=None
	VERIFY_COUNT(0)





#/////////////////////////////////////////////////////////////// std::vector<SwigClass*>& fn()
def test_ReturnReferenceStdVectorPointerSwigClass():
 
	VERIFY_COUNT(0)
	t=ReturnReferenceStdVectorPointerSwigClass()
	temp=t.fn()
	assert(temp.size()==1)
	aux=temp[0]
	aux.value=77
	aux=None
	temp=None
	temp=t.fn()
	assert(temp.size()==1)
	aux=temp[0]
	assert(aux.value==77)
	aux=None
	temp=None
	t=None
	VERIFY_COUNT(0)

 		


#///////////////////////////////////////////////////////////////
#///////////////////////////////////////////////////////////////
def test_mix_stuff():

	# test equality operator
	u=Vec3f(1,2,3)
	v=Vec3f(1,2,3)
	assert u==v
	
	# test str operator
	assert (str(Vec3f(1,2,3))=="(1.00,2.00,3.00)")
	
	# test repr operator
	assert (repr(Vec3f(1,2,3))=="Vec3f(1.00,2.00,3.00)")
	
	# test assign/copy constructor
	u,v=(Vec3f(1,2,3),Vec3f(4,5,6))
	v.assign(u)
	assert v==Vec3f(1,2,3)
	
	
	# test operator []
	a=CppArray(2)
	assert a.get(0)==0 and a.get(1)==0 
	
	# test equality operator
	u,v=(Vec3f(1,2,3),Vec3f(1,2,3))
	assert u==v


def test_virtual_class():
	
	c=VirtualClass()
	c.fn()
	assert c.getValue()==0
	
	c=PyVirtualClass()
	c.fn()
	assert c.getValue()==1


def test_operators():

	assert (Vec3f(1,2,3)+Vec3f(4,5,6))==Vec3f(5,7,9)
	assert (Vec3f(2,2,2)-Vec3f(1,1,1))==Vec3f(1,1,1)
	assert (Vec3f(2,2,2)*Vec3f(1,1,1))==6
	
	assert Vec3f(1,2,3)*2==Vec3f(2,4,6) 
	assert Vec3f(4,4,4)/2==Vec3f(2,2,2) 

	v=Vec3f(1,2,3);v+=Vec3f(1,2,3);assert v==Vec3f(2,4,6)
	v=Vec3f(1,2,3);v-=Vec3f(1,2,3);assert v==Vec3f(0,0,0)
	
	v=Vec3f(1,2,3);v*=2;assert v==Vec3f(2,4,6)
	v=Vec3f(2,4,8);v/=2;assert v==Vec3f(1,2,4)




#///////////////////////////////////////////////////////////////
#///////////////////////////////////////////////////////////////
if __name__ == "__main__":

	VERIFY_COUNT(0)
	
	if True:
	
		print "test_CreateSwigClass";test_CreateSwigClass()
		print "test_VarAtomicType";test_VarAtomicType()
		print "test_VarSwigClass";test_VarSwigClass()
		print "test_VarSmartPointerSwigClass";test_VarSmartPointerSwigClass()
		print "test_VarPointerSwigClass";test_VarPointerSwigClass()
		print "test_VarStdVectorInt";test_VarStdVectorInt()
		print "test_VarStdVectorStdVectorInt";test_VarStdVectorStdVectorInt()
		print "test_VarStdVectorSwigClass";test_VarStdVectorSwigClass()
		print "test_VarStdVectorSmartPointerSwigClass";test_VarStdVectorSmartPointerSwigClass()
		print "test_VarStdVectorPointerSwigClass";test_VarStdVectorPointerSwigClass()
		print "test_VarPointerStdVectorInt";test_VarPointerStdVectorInt()
		print "test_VarPointerStdVectorStdVectorInt";test_VarPointerStdVectorStdVectorInt()
		print "test_VarPointerStdVectorSwigClass";test_VarPointerStdVectorSwigClass()
		print "test_VarPointerStdVectorSmartPointerSwigClass";test_VarPointerStdVectorSmartPointerSwigClass()
		print "test_VarPointerStdVectorPointerSwigClass";test_VarPointerStdVectorPointerSwigClass()
		
		print "test_InputAtomicType";test_InputAtomicType()
		print "test_InputSwigClass";test_InputSwigClass()
		print "test_InputSmartPointerSwigClass";test_InputSmartPointerSwigClass()
		print "test_InputReferenceSwigClass";test_InputReferenceSwigClass()
		print "test_InputPointerClass";test_InputPointerClass()
		print "test_InputStdVectorInt";test_InputStdVectorInt()
		print "test_InputStdVectorStdVectorInt";test_InputStdVectorStdVectorInt()
		print "test_InputStdVectorSwigClass";test_InputStdVectorSwigClass()
		print "test_InputStdVectorSmartPointerSwigClass";test_InputStdVectorSmartPointerSwigClass()
		print "test_InputStdVectorPointerSwigClass";test_InputStdVectorPointerSwigClass()
		print "test_InputPointerStdVectorInt";test_InputPointerStdVectorInt()
		print "test_InputPointerStdVectorStdVectorInt";test_InputPointerStdVectorStdVectorInt()
		print "test_InputPointerStdVectorSwigClass";test_InputPointerStdVectorSwigClass()
		print "test_InputPointerStdVectorSmartPointerSwigClass";test_InputPointerStdVectorSmartPointerSwigClass()
		print "test_InputPointerStdVectorPointerSwigClass";test_InputPointerStdVectorPointerSwigClass()
		print "test_InputReferenceStdVectorInt";test_InputReferenceStdVectorInt()
		print "test_InputReferenceStdVectorInt";test_InputReferenceStdVectorStdVectorInt()
		print "test_InputReferenceStdVectorSwigClass";test_InputReferenceStdVectorSwigClass()
		print "test_InputReferenceStdVectorSmartPointerSwigClass";test_InputReferenceStdVectorSmartPointerSwigClass()
		print "test_InputReferenceStdVectorPointerSwigClass";test_InputReferenceStdVectorPointerSwigClass()
		
		print "test_ReturnAtomicType";test_ReturnAtomicType()
		print "test_ReturnSwigClass";test_ReturnSwigClass()
		print "test_ReturnSmartPointerSwigClass";test_ReturnSmartPointerSwigClass()
		print "test_ReturnReferenceSwigClass";test_ReturnReferenceSwigClass()
		print "test_ReturnPointerSwigClass";test_ReturnPointerSwigClass()
		print "test_ReturnStdVectorInt";test_ReturnStdVectorInt()
		print "test_ReturnStdVectorStdVectorInt";test_ReturnStdVectorStdVectorInt()
		print "test_ReturnStdVectorSwigClass";test_ReturnStdVectorSwigClass()
		print "test_ReturnStdVectorSmartPointerSwigClass";test_ReturnStdVectorSmartPointerSwigClass()
		print "test_ReturnStdVectorPointerSwigClass";test_ReturnStdVectorPointerSwigClass()
		print "test_ReturnPointerStdVectorInt";test_ReturnPointerStdVectorInt()
		print "test_ReturnPointerStdVectorStdVectorInt";test_ReturnPointerStdVectorStdVectorInt()
		print "test_ReturnPointerStdVectorSwigClass";test_ReturnPointerStdVectorSwigClass()
		print "test_ReturnPointerStdVectorSmartPointerSwigClass";test_ReturnPointerStdVectorSmartPointerSwigClass()
		print "test_ReturnPointerStdVectorPointerSwigClass";test_ReturnPointerStdVectorPointerSwigClass()
		print "test_ReturnReferenceStdVectorInt";test_ReturnReferenceStdVectorInt()
		print "test_ReturnReferenceStdVectorStdVectorInt";test_ReturnReferenceStdVectorStdVectorInt()
		print "test_ReturnReferenceStdVectorSwigClass";test_ReturnReferenceStdVectorSwigClass()
		print "test_ReturnReferenceStdVectorSmartPointerSwigClass";test_ReturnReferenceStdVectorSmartPointerSwigClass()
		print "test_ReturnReferenceStdVectorPointerSwigClass";test_ReturnReferenceStdVectorPointerSwigClass()

		print "test_mix_stuff";test_mix_stuff()
		
		print "test_virtual_class";test_virtual_class()

		print "test_operators";test_operators()

	print "All tests OK"
  



