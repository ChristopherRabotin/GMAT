"""
This is the toy example problem in the SNOPT documentation.
     min         x1
     s.t   .   x1**2 + 4x2**2 <= 4
           (x1-2)**2 +  x2**2 <= 5
            x1 >=0

We define the function F(x):
   F(x)  = [     x1             ]
           [     x1**2 + 4x2**2 ]
           [ (x1-2)**2 +  x2**2 ]
with ObjRow = 1 to indicate the objective.

The Jacobian is:
  F'(x)  = [   1       0  ]   [ 1 0 ]    [   0       0  ]
           [   2x1    8x2 ] = [ 0 0 ]  + [   2x1    8x2 ]
           [ 2(x1-2)  2x2 ]   [ 0 0 ]    [ 2(x1-2)  2x2 ]
                             linear(A)     nonlinear (G)

"""

import numpy        as np
import scipy.sparse as sp
from  snopt import snopta, SNOPT_options


def sntoya_objF(status,x,needF,F,needG,G):
    F[0] = x[1]         # objective row
    F[1] = x[0]**2        + 4.0*x[1]**2
    F[2] = (x[0] - 2.0)**2 +     x[1]**2

    return status, F, G

def sntoya_objFG(status,x,needF,F,needG,G):
    F[0] = x[1]         # objective row
    F[1] = x[0]**2        + 4.0*x[1]**2
    F[2] = (x[0] - 2.0)**2 +     x[1]**2

    G[0] = 2*x[0]
    G[1] = 8*x[1]
    G[2] = 2*(x[0]-2)
    G[3] = 2*x[1]

    return status, F, G

inf   = 1.0e20
options = SNOPT_options()

options.setOption('Verbose',True)
options.setOption('Solution print',True)
options.setOption('Print filename','sntoya.out')

options.setOption('Summary frequency',1)


# Name arrays have to be dtype='|S1' and also have to be the
# correct length, else they are ignored by SNOPT:
xnames  = np.empty(2,dtype='|S8')
xnames[0] = "      x0"
xnames[1] = "      x1"

Fnames  = np.empty(3,dtype='|S8')
Fnames[0] = "      F0"
Fnames[1] = "      F1"
Fnames[2] = "      F2"

x0      = np.array([ 1.0, 1.0 ])
xlow    = np.array([ 0.0, -inf])
xupp    = np.array([ inf,  inf])

Flow    = np.array([ -inf, -inf, -inf ])
Fupp    = np.array([  inf,  4.0,  5.0 ])

n       = 2
nF      = 3

ObjRow  = 1


# We first solve the problem without providing derivative info
result = snopta(sntoya_objF,n,nF,x0=x0,
                xlow=xlow,xupp=xupp,
                Flow=Flow,Fupp=Fupp,
                ObjRow=ObjRow,
                xnames=xnames,Fnames=Fnames,
                name=' sntoyaF',options=options)

# Now we set up the derivative structures...

# A and G provide the linear and nonlinear components of
# the Jacobian matrix, respectively. Here, G and A are given
# as dense matrices.
#
# For the nonlinear components, enter any nonzero value to
# indicate the location of the nonlinear deriatives (in this case, 2).
# A must be properly defined with the correct derivative values.

A = np.array([ [0, 1],
               [0, 0],
               [0, 0]])

G = np.array([ [0, 0],
               [2, 2],
               [2, 2]])

# Alternatively, A and G can be input in coordinate form via scipy's
# coordinate matrix
#  A = sp.coo_matrix(A)
#  G = sp.coo_matrix(G)
# or explicitly in coordinate form as a tuple
#  iAfun = row indices of A
#  jAvar = col indices of A
#  A     = matrix values of A
#  (A,iAfun,jAvar)
#
#  iGfun = row indices of G
#  jGvar = col indices of G
#  (iGfun,jGvar)
#

result = snopta(sntoya_objFG,n,nF,x0=x0,name='sntoyaFG',xlow=xlow,xupp=xupp,
                Flow=Flow,Fupp=Fupp,ObjRow=ObjRow,A=A,G=G,xnames=xnames,Fnames=Fnames)
print(result)
