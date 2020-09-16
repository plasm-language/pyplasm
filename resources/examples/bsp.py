from pyplasm import *

print "Creo un cubo 3d"

# ho bisogno di un graph navigator per le operazioni booleane
nav=GraphNavigator()

#dimensione del cubo (lavoro in 3D)
cube_dim=3 

# il cubo avra' bounding box [0,0,0] [1,1,1]
From=0.0 
To  =1.0

# g e' il complesso a celle che conterra' tutte le celle convesse
g=Graph.cuboid(cube_dim,From,To)

# ID della cella cubo
cube=g.each(3).getNode()

# se si ha bisogno di visualizzare il lattice completo del cubo
# si vedranno 1 cella 3D, 6 celle 2D, 12 celle 1D, 8 celle 0D
if False: g.Print()

# esempio di splitting del cubo tramite iperpiano  
# iperpiano   -0.5 + 1*x + 0*y + 0*z >=0   che poi e' x>=0.5 (il primo elemento e' la componente omogenea)
splitting_plane=Planef([-0.5,1,0,0])


# splitto la cella cubo con iperpiano
tolerance = 1e-6  # per la classificazione dei punti
max_try = 10  # quante volte provare a fare lo splitting
[cell_below,cell_equal,cell_above]=g.split(nav,cube,splitting_plane,tolerance,max_try)

if cell_below==0 and cell_equal==0 and cell_above==0:
	print "splitting fallito (cella flat o classificazione impossibile)"
	
elif cell_below>0 and cell_equal==0 and cell_above==0:
	print "iperpiano lascia la cella TUTTA nello spazio below"
	
elif cell_above>0 and cell_below==0 and cell_equal==0:
	print "iperpiano lascia la cella TUTTA nello spazio above"
	
else:
	assert cell_above>0 and cell_below>0 and cell_equal>0
	print "Splitting riuscito, continuare lo splitting sulle celle cell_above e cell_below, come faresti con un bsp"

# quando si ha finito si puo convertire l'oggetto Graph in un Hpc e visualizzarlo
# nota, in visualizzazione le celle sono "triangolate"
hpc=Hpc(g)
Plasm.view(hpc)



