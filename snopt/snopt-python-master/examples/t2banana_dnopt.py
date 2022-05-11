"""
This is an unconstrained example (t2banana).
"""

import numpy        as np
import scipy.sparse as sp
from   optimize     import dnopt, DNOPT_options

def t2obj(mode,x,fObj,gObj,nState):

    fObj = 0.0
    if mode == 0 or mode == 2:
        fObj = 100.0*(x[1] - x[0]**2)**2 + (1.0 - x[0])**2
    if mode == 0:
        return mode, fObj

    if mode == 1 or mode == 2:
        gObj[0] = -400.0*(x[1] - x[0]**2)*x[0] - 2.0 *(1.0 - x[0])
        gObj[1] =  200.0*(x[1] - x[0]**2)

    return mode, fObj, gObj


options = DNOPT_options()
inf      = 1.0e+20

options.setOption('Verbose',True)
options.setOption('Infinite bound',inf)
options.setOption('Verify level',3)
options.setOption('Print filename','t2banana.out')

n     = 2
nnJac = 0
nnObj = n

x0    = np.array([1.2, 1.0])
bl    = np.array([-10.0, -10.0])
bu    = np.array([  5.0,  10.0])

iObj  = 0

H = np.zeros((n,n))

names  = np.empty(n,dtype='|S8')
names[0] = "      x0"
names[1] = "      x1"

result = dnopt(t2obj,None,nnObj=nnObj,nnJac=nnJac,
               x0=x0,H=H,name='t2banana',names=names,
               iObj=iObj,bl=bl,bu=bu,options=options)

