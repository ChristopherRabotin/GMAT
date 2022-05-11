"""
This is an unconstrained example (t2banana).
"""

import numpy        as np
import scipy.sparse as sp
from   snopt        import snoptb, SNOPT_options


def t2con(mode,x,fCon,gCon,nState):
    # No nonlinear constraints
    return mode, fCon, gCon


def t2obj(mode,x,fObj,gObj,nState):

    fObj = 0.0
    if mode == 0 or mode == 2:
        fObj = 100.0*(x[1] - x[0]**2)**2 + (1.0 - x[0])**2

    if mode == 0:
        return mode, fObj

    if mode == 1 or mode == 2:
        gObj[0] = -400.0*(x[1] - x[0]**2)*x[0] - 2.0*(1.0 - x[0])
        gObj[1] =  200.0*(x[1] - x[0]**2)

    return mode, fObj, gObj


options = SNOPT_options()
inf      = 1.0e+20

options.setOption('Infinite bound',inf)
options.setOption('Verify level',3)
options.setOption('Verbose',True)
options.setOption('Print filename','t2banana.out')

m     = 1
n     = 2

nnCon = 0
nnJac = 0
nnObj = n

x0    = np.zeros(n+m,float)
x0[0] = 1.2
x0[1] = 1.0

# J contains the sparsity pattern of the Jacobian matrix.
# For nonlinear elements, enter any nonzero number (in this case 100).
# Linear elements must be correctly defined.

# For an unconstrained, we create a sparse, dummy constraint with +/- inf bounds
J = np.array([[100.0, 0]])

bl    = -inf*np.ones(n+m)
bu    =  inf*np.ones(n+m)

bl[0] = -10.0
bu[0] =   5.0

bl[1] = -10.0
bu[1] =  10.0

iObj  = 0

result = snoptb(t2obj,t2con,nnObj=nnObj,nnCon=nnCon,nnJac=nnJac,
                x0=x0,J=J,name='t2banana',
                iObj=iObj,bl=bl,bu=bu,options=options)

