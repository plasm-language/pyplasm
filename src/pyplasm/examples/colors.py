from pyplasm import *

def out():
    point=MKPOL([[[0]],[[1]],[[1]]])
    edge=CUBOID([1])
    face=CUBOID([1,1])
    cell=CUBOID([1,1,1])
    sep=T([1,2,3])([1,0,0])

    return STRUCT([
            COLOR(RED)(point)
            ,sep
            ,COLOR(GREEN)(point)
            ,sep
            ,COLOR(BLUE)(point)
            ,sep
            ,COLOR(RED)(edge)
            ,sep
            ,COLOR(GREEN)(edge)
            ,sep
            ,COLOR(BLUE)(edge)
            ,sep
            ,COLOR(RED)(face)
            ,sep
            ,COLOR(GREEN)(face)
            ,sep
            ,COLOR(BLUE)(face)
            ,sep
            ,COLOR(RED)(cell)
            ,sep
            ,COLOR(GREEN)(cell)
            ,sep
            ,COLOR(BLUE)(cell)
            ])

VIEW(out())

