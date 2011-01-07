from pyplasm import *

print "Creo un cubo 3d"

# ho bisogno di un graph navigator per le operazioni booleane
nav=GraphNavigator()

#dimensione del cubo
cube_dim=3 

# il cubo avra' bounding box [0,0,0] [1,1,1]
From=0.0 
To  =1.0
g=Graph.cuboid(cube_dim,From,To)

# ID della cella cubo
cube=g.each(3).getNode()

# se si ha bisogno di visualizzare il lattice completo del cubo
# si vedranno 1 cella 3D, 6 celle 2D, 12 celle 1D, 8 celle 0D
g.Print()

# esempio di splitting del cubo tramite iperpiano  
# iperpiano   0.5 + 1*x + 0*y + 0*z >=0   che poi e' x>=0.5 (il primo elemento e' la componente omogenea)
splitting_plane=Planef([-0.5,1,0,0])

# per la classificazione dei punti
tolerance = 1e-6 

# quante volte provare a fare lo splitting
max_try = 10 

cell_below=-1
cell_above=-1
cell_equal=-1

[cell_below,cell_equal,cell_above]=g.split(nav,cube,splitting_plane,tolerance,max_try)

if retcode==SPLIT_FLAT:
	print "SPLIT_FLAT"
	
if retcode==SPLIT_OK:
	print "SPLIT_OK"
	
if retcode==SPLIT_IMPOSSIBLE:
	print "SPLIT_IMPOSSIBLE"





