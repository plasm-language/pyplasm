from pyplasm import *




# /////////////////////////////////////////////
# celle dw di una cella
# /////////////////////////////////////////////
def downCells(g,cell):
	it=g.goDw(cell);ret=[]
	while not it.end(): ret+=[it.getNode()];it.goForward()
	return ret

# /////////////////////////////////////////////
# celle up di una cella
# /////////////////////////////////////////////
def upCells(g,cell):
	it=g.goUp(cell);ret=[]
	while not it.end(): ret+=[it.getNode()];it.goForward()
	return ret

# /////////////////////////////////////////////
#  celle ad un certo livello del grafo
# /////////////////////////////////////////////
def cellsPerLevel(g,level):
	it=g.each(level) 
	ret=[]
	while not it.end():
		ret+=[it.getNode()];it.goForward()
	return ret	


# /////////////////////////////////////////////
# ad esempio per trovare tutte le 0-celle 
# /////////////////////////////////////////////
def findCells(g,nav,level,cell):
	num=g.findCells(level,cell,nav)
	return [nav.getCell(level,N) for N in range(0,num)] 


# ////////////////////////////////////////////////
def getIntersection(g,from_cells,up_direction):

	result=[]
	num = len(from_cells)

	# use the Tmp info to reset the "reachable" info
	for c in from_cells:
		it=g.goUp(c) if up_direction else g.goDown(c)
		while not it.end(): 
			reached=g.getNode(it.getNode())
			reached.Tmp=0
			it.goForward()
		
	# increment the "reachable" Tmp, if all nodes reaches one common father
	# then the father is added to the list
	for c in from_cells:
		it=g.goUp(c) if up_direction else g.goDown(c)
		while not it.end(): 
			reached =g.getNode(it.getNode())
			reached.Tmp=reached.Tmp+1	
			if reached.Tmp==num: 
				result+=[it.getNode()]
			it.goForward()
		
	return result



# /////////////////////////////////////////////
# creo un triangolo
# /////////////////////////////////////////////
def makeTriangle():


	pointdim=2
	g=Graph(pointdim)
	
	#un navigator serve per fare la visita dei nodi di un graho
	nav=GraphNavigator()
	
	#aggiungi tre vertici con le coordinate (la prima componente omogenea)
	v0=g.addNode(0);g.setVecf(v0,Vecf(1, 0,0,0))
	v1=g.addNode(0);g.setVecf(v1,Vecf(1, 1,0,0))
	v2=g.addNode(0);g.setVecf(v2,Vecf(1, 0,1,0))
	
	#aggiungi tre edge con le relative connessioni
	e01=g.addNode(1);g.addArch(v0,e01);g.addArch(v1,e01)
	e12=g.addNode(1);g.addArch(v1,e12);g.addArch(v2,e12)
	e20=g.addNode(1);g.addArch(v2,e20);g.addArch(v0,e20)
	
	#aggiungi la cella piena
	triangle=g.addNode(2)
	g.addArch(e01,triangle)
	g.addArch(e12,triangle)
	g.addArch(e20,triangle)
	
	#aggiungi la doppia connettivita (rende piu' veloce il traversal)
	g.addArch(triangle,v0)
	g.addArch(triangle,v1)
	g.addArch(triangle,v2)
	
	return g


# /////////////////////////////////////////////
# creo un tetrahedro
# /////////////////////////////////////////////
def makeTet():

	pointdim=3
	g=Graph(pointdim)
	nav=GraphNavigator()
	
	v0=g.addNode(0);g.setVecf(v0,Vecf(1, 0,0,0))
	v1=g.addNode(0);g.setVecf(v1,Vecf(1, 1,0,0))
	v2=g.addNode(0);g.setVecf(v2,Vecf(1, 0,1,0))
	v3=g.addNode(0);g.setVecf(v3,Vecf(1, 0,0,1))
	
	e01=g.addNode(1);g.addArch(v0,e01);g.addArch(v1,e01)
	e12=g.addNode(1);g.addArch(v1,e12);g.addArch(v2,e12)
	e20=g.addNode(1);g.addArch(v2,e20);g.addArch(v0,e20)
	e03=g.addNode(1);g.addArch(v0,e03);g.addArch(v3,e03)
	e13=g.addNode(1);g.addArch(v1,e13);g.addArch(v3,e13)
	e23=g.addNode(1);g.addArch(v2,e23);g.addArch(v3,e23)
	
	# attenzione: non sono correttamente orientate! bisognerebbe crearle coerentemente orientate
	f012=g.addNode(2);g.addArch(e01,f012);g.addArch(e12,f012);g.addArch(e20,f012)
	f013=g.addNode(2);g.addArch(e01,f013);g.addArch(e13,f013);g.addArch(e03,f013)
	f123=g.addNode(2);g.addArch(e12,f123);g.addArch(e23,f123);g.addArch(e13,f123)
	f203=g.addNode(2);g.addArch(e20,f203);g.addArch(e03,f203);g.addArch(e23,f203)
	
	# per pointdim>=3, le (pointdim-1)-celle devono avere l'informazione circa l'iperpiano passante
	g.setVecf(f012,g.getFittingPlane(f012))
	g.setVecf(f013,g.getFittingPlane(f013))
	g.setVecf(f123,g.getFittingPlane(f123))
	g.setVecf(f203,g.getFittingPlane(f203))
	
	tet=g.addNode(3)
	g.addArch(f012,tet)
	g.addArch(f013,tet)
	g.addArch(f123,tet)
	g.addArch(f203,tet)
	
	g.addArch(tet,v0)
	g.addArch(tet,v1)
	g.addArch(tet,v2)
	g.addArch(tet,v3)
	
	return g
	
	
def printGraph(g):

	pointdim=g.pointdim

	#stampa il grafo (solo per debugging)
	#g.Print()
	#print ""
	
	# ESEMPIO: stampa tutte le celle a tutti i livelli
	print "Graph cells"
	for level in range(0,pointdim+1):
		print " level(",level,")",cellsPerLevel(g,level)
	print ""
	
	# ESEMPIO : ottieni informazioni su una certa cella
	for cell in [v0,v1,v2,e01,e12,e20,triangle]:
		print "cell id(",cell, ") level(",g.Level(cell), ") downCells(",downCells(g,cell), ") upCells("  ,upCells(g,cell),")",
		# stampa le coordinate
		if g.Level(cell)==0: print "coord(",g.getVecf(cell),")",
		print ""
		# stampa la connettivita a tutti i livelli
		for level in range(0,pointdim+1):
			print "   findCells(g,nav,",level,",cell",cell,")",findCells(g,nav,level,cell) 
		print ""
	
	
if False:
	g1=Graph.cuboid(1)
	g2=Graph.cuboid(2)
	vmat = Matf(1)
	hmat = Matf(1)
	g=Graph.power(vmat,hmat,  g1,None,None,  g2,None,None)
	print "vmat",vmat,"hmat",hmat
	g.Print()
	Plasm.View(Hpc(g,vmat,hmat))


if False:
	hpc=Plasm.cube(3)
	# appiattisce la struttura dell'hpc in modo che abbio i grafi a livello 0
	hpc=Plasm.shrink(hpc,False)
	# ogni item di <graphs> sara una tripla: [grafo, matrice di trasformazione dei vertici, matrice di trasformazione dei piani]
	graphs=[]
	for i in range(0,hpc.childs.size()):
		child=hpc.childs[i]
		if child.g: 
			child.g.Print()
			graphs+=[[child.g,child.vmat,child.hmat],]
	print graphs	
	

if False:
	g=makeTriangle()
	printGraph(g)
	Plasm.View(Hpc(g))
	

if False:
	g=makeTet()
	g.Print()
	Plasm.View(Hpc(g))


# ////////////////////////////////////////////
# find all paths
# ////////////////////////////////////////////
def multiTraverse(g,cell,nrecursion,up_direction=False):
	if nrecursion==0: return [[cell]]
	ret=[]
	for Down in downCells(g,cell) if not up_direction else upCells(g,cell): ret+=[[cell] + L for L in multiTraverse(g,Down,nrecursion-1,up_direction)]
	return ret


#g=makeTet()
#g.Print()
#print multiTraverse(g,15,3,False)






		
			

				
			
		
		
		
		
	
	

	
	
	
	







