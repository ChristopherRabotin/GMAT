"""
This is the toy example problem in the SNOPT documentation.

    min (x1 + x2 + x3)**2 + 3x3 + 5x4
    st.
         x3 >= 0, x4 >= 0
         x1**2 + x2**2 + x3      = 2
                 x2**4      + x4 = 4
         2x1   + 4x2            >= 0


 The linear part of the objective 3x3 + 5x4 is given
 as part of the constraints:

    min (x1 + x2 + x3)**2 + e_4
    st.
           x3 >= 0, x4 >= 0
           x1**2 + x2**2 + x3        =  2
                   x2**4      +  x4  =  4
           2x1   + 4x2               >= 0
     inf >                3x3 + 5x4  > -inf

 The Jacobian matrix is
    [2*x1   2*x2    1.0    0]
    [0      4*x3      0  1.0]
    [2.0     4.0      0    0]
    [0.0     0.0    3.0  5.0]
 with
    iObj = 4 (indicating the linear objective term)
"""

import numpy        as np
import scipy.sparse as sp
from   snopt        import snoptb, SNOPT_options


def toycon(mode,x,fCon,gCon,nState):
    # Nonlinear terms of the gradient only
    if mode == 0 or mode == 2:
        fCon[0] = x[0]**2 + x[1]**2
        fCon[1] = x[1]**4

    if mode >= 1:
        gCon[0] = 2.0*x[0]
        gCon[1] = 2.0*x[1]
        gCon[2] = 4.0*x[1]**3

    return mode, fCon, gCon


def toyobj(mode,x,fObj,gObj,nState):

    sumx = x[0] + x[1] + x[2]
    # Nonlinear objective term only
    fObj = 0.0
    if mode == 0 or mode == 2:
        fObj = sumx**2

    if mode == 1 or mode == 2:
        pass
        #gObj[0] = 2.0*sum
        #gObj[1] = 2.0*sum
        #gObj[2] = 2.0*sum

    return mode, fObj#, gObj


options = SNOPT_options()
inf      = 1.0e+20

options.setOption('Infinite bound',inf)
options.setOption('Verify level',3)
options.setOption('Print filename','sntoyb.out')

m     = 4
n     = 4

nnCon = 2
nnJac = 2
nnObj = 3

x0    = np.zeros(n+m,float)

# J contains the sparsity pattern of the Jacobian matrix.
# For nonlinear elements, enter any nonzero number (in this case 100).
# Linear elements must be correctly defined.
J = np.array([ [100.0, 100.0, 1.0,   0],
               [0.0  , 100.0,   0, 1.0],
               [2.0  ,   4.0,   0,   0],
               [0.0  ,   0.0, 3.0, 5.0]])

# Alternatively, the user can provide the sparsity pattern in
# sparse-by-column format.  Here, indJ contains the row indices,
# locJ are the column pointers, and J contains the matrix values.
#
# indJ = np.array([0,2,0,1,2,0,3,1,3],int)
# locJ = np.array([0,2,5,7,9],int)
# J    = np.array([100.0, 2.0, 100.0, 100.0, 4.0, 1.0, 3.0, 1.0, 5.0],float)


bl    = -inf*np.ones(n+m)
bu    =  inf*np.ones(n+m)

bl[2] = 0.0
bl[3] = 0.0

bl[4] = 2.0
bu[4] = 2.0

bl[5] = 4.0
bu[5] = 4.0

bl[6] = 0.0

iObj  = 4

options.setOption('Verbose',True)
options.setOption('Derivative level',0)

result = snoptb(toyobj,toycon,nnObj=nnObj,nnCon=nnCon,nnJac=nnJac,
                x0=x0,J=J,name='  sntoyb',
                iObj=iObj,bl=bl,bu=bu,options=options)

