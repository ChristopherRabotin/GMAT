"""
An example SNOPTB problem.
"""

import numpy as np
from snopt import snoptb, SNOPT_options


def hexCon(mode,x,fCon,gCon,nState):
    two = 2.0

    fCon[ 0] =    x[0]**2          +   x[5]**2
    fCon[ 1] =   (x[1] - x[0])**2  +  (x[6] - x[5])**2
    fCon[ 2] =   (x[2] - x[0])**2  +   x[5]**2
    fCon[ 3] =   (x[0] - x[3])**2  +  (x[5] - x[7])**2
    fCon[ 4] =   (x[0] - x[4])**2  +  (x[5] - x[8])**2
    fCon[ 5] =    x[1]**2          +   x[6]**2
    fCon[ 6] =   (x[2] - x[1])**2  +   x[6]**2
    fCon[ 7] =   (x[3] - x[1])**2  +  (x[7] - x[6])**2
    fCon[ 8] =   (x[1] - x[4])**2  +  (x[6] - x[8])**2
    fCon[ 9] =   (x[3] - x[2])**2  +   x[7]**2
    fCon[10] =   (x[4] - x[2])**2  +   x[8]**2
    fCon[11] =    x[3]**2          +   x[7]**2
    fCon[12] =   (x[3] - x[4])**2  +  (x[8] - x[7])**2
    fCon[13] =    x[4]**2          +   x[8]**2

    # Nonlinear Jacobian elements for column 1.
    # rows = [1,2,3,4,5].
    gCon[ 0] =   two*x[0]
    gCon[ 1] = - two*(x[1] - x[0])
    gCon[ 2] = - two*(x[2] - x[0])
    gCon[ 3] =   two*(x[0] - x[3])
    gCon[ 4] =   two*(x[0] - x[4])

    # Nonlinear Jacobian elements for column 2.
    # Rows = [2,6,7,8,9].

    gCon[ 5] =   two*(x[1] - x[0])
    gCon[ 6] =   two*x[1]
    gCon[ 7] = - two*(x[2] - x[1])
    gCon[ 8] = - two*(x[3] - x[1])
    gCon[ 9] =   two*(x[1] - x[4])

    # Nonlinear Jacobian elements for column 3.
    # Rows = [3,7,10,11].

    gCon[10] =   two*(x[2] - x[0])
    gCon[11] =   two*(x[2] - x[1])
    gCon[12] = - two*(x[3] - x[2])
    gCon[13] = - two*(x[4] - x[2])

    # Nonlinear Jacobian elements for column 4.
    # Rows = [4,8,10,12,13].

    gCon[14] = - two*(x[0] - x[3])
    gCon[15] =   two*(x[3] - x[1])
    gCon[16] =   two*(x[3] - x[2])
    gCon[17] =   two*x[3]
    gCon[18] =   two*(x[3] - x[4])

    # Nonlinear Jacobian elements for column 5.
    # Rows = [5,9,11,13,14].

    gCon[19] = - two*(x[0] - x[4])
    gCon[20] = - two*(x[1] - x[4])
    gCon[21] =   two*(x[4] - x[2])
    gCon[22] = - two*(x[3] - x[4])
    gCon[23] =   two*x[4]

    # Nonlinear Jacobian elements for column 6.
    # Rows = [1,2,3,4,5].

    gCon[24] =   two*x[5]
    gCon[25] = - two*(x[6] - x[5])
    gCon[26] =   two*x[5]
    gCon[27] =   two*(x[5] - x[7])
    gCon[28] =   two*(x[5] - x[8])

    # Nonlinear Jacobian elements for column 7.
    # Rows = [2,6,7,8,9].

    gCon[29] =   two*(x[6] - x[5])
    gCon[30] =   two*x[6]
    gCon[31] =   two*x[6]
    gCon[32] = - two*(x[7] - x[6])
    gCon[33] =   two*(x[6] - x[8])

    # Nonlinear Jacobian elements for column 8.
    # Rows = [4,8,10,12,13].

    gCon[34] = - two*(x[5] - x[7])
    gCon[35] =   two*(x[7] - x[6])
    gCon[36] =   two*x[7]
    gCon[37] =   two*x[7]
    gCon[38] = - two*(x[8] - x[7])

    # Nonlinear Jacobian elements for column 9.
    # Rows = [5,9,11,13,14].

    gCon[39] = - two*(x[5] - x[8])
    gCon[40] = - two*(x[6] - x[8])
    gCon[41] =   two*x[8]
    gCon[42] =   two*(x[8] - x[7])
    gCon[43] =   two*x[8]

    return mode, fCon, gCon

def hexObj(mode,x,fObj,gObj,nState):
    fObj = - x[1]*x[5] + x[0]*x[6] - x[2]*x[6] - x[4]*x[7] + x[3]*x[8] + x[2]*x[7]

    gObj[0] =   x[6]
    gObj[1] = - x[5]
    gObj[2] = - x[6] + x[7]
    gObj[3] =   x[8]
    gObj[4] = - x[7]
    gObj[5] = - x[1]
    gObj[6] = - x[2] + x[0]
    gObj[7] = - x[4] + x[2]
    gObj[8] =   x[3]

    return mode, fObj, gObj


inf     = 1.0e+20

options = SNOPT_options()
options.setOption('Infinite bound',inf)
options.setOption('Specs filename','snmainb.spc')
options.setOption('Print filename','snmainb.out')

m      = 18
n      = 9
nnCon  = 14
ne     = 52
nnJac  = n
nnObj  = n

bl     = -inf*np.ones(n+m)
bu     =  inf*np.ones(n+m)

# Nonlinear constraints
bu[1+n:nnCon+n] = 1.0

# Linear constraints
bl[1+n+nnCon:m+n] = 0.0

# Variables
bl[0] =  0.0
bl[2] = -1.0
bl[4] =  0.0
bl[5] =  0.0
bl[6] =  0.0

bu[2] =  1.0
bu[7] =  0.0
bu[8] =  0.0

# Initial x
x     = np.zeros(n+m)
x[0]  =  .1e+0
x[1]  =  .125e+0
x[2]  =  .666666e+0
x[3]  =  .142857e+0
x[4]  =  .111111e+0
x[5]  =  .2e+0
x[6]  =  .25e+0
x[7]  = -.2e+0
x[8]  = -.25e+0

# Jacobian
locJ = np.zeros(n+1,'i')
indJ = np.zeros(ne,'i')
valJ = np.zeros(ne,float)

locJ[0]  = 0

indJ[ 0] =  0
indJ[ 1] =  1
indJ[ 2] =  2
indJ[ 3] =  3
indJ[ 4] =  4

valJ[ 0] =  0.0
valJ[ 1] =  0.0
valJ[ 2] =  0.0
valJ[ 3] =  0.0
valJ[ 4] =  0.0

#     Column 1.
#     Linear element in row 6 next.

indJ[ 5] = 14
valJ[ 5] = -1.0

#     Column 2.
#     Nonlinear elements in rows [2, 6, 7, 8, 9].

locJ[ 1] =  6

indJ[ 6] =  1
indJ[ 7] =  5
indJ[ 8] =  6
indJ[ 9] =  7
indJ[10] =  8

valJ[ 6] =  0.0
valJ[ 7] =  0.0
valJ[ 8] =  0.0
valJ[ 9] =  0.0
valJ[10] =  0.0

#     Column 2.
#     Linear elements in rows [15,16].

indJ[11] = 14
indJ[12] = 15

valJ[11] =  1.0
valJ[12] = -1.0

#     Column 3.
#     Nonlinear elements in rows [3, 7, 10, 11].

locJ[ 2] =  13

indJ[13] =  2
indJ[14] =  6
indJ[15] =  9
indJ[16] = 10

valJ[13] =  0.0
valJ[14] =  0.0
valJ[15] =  0.0
valJ[16] =  0.0

#     Column 3.
#     Linear elements in rows [16, 17].

indJ[17] = 15
indJ[18] = 16

valJ[17] =  1.0
valJ[18] =  1.0

#     Column 4.
#     Nonlinear elements in rows [20, 21, 22, 23, 24].

locJ[ 3] = 19

indJ[19] =  3
indJ[20] =  7
indJ[21] =  9
indJ[22] = 11
indJ[23] = 12

valJ[19] =  0.0
valJ[20] =  0.0
valJ[21] =  0.0
valJ[22] =  0.0
valJ[23] =  0.0

#     Column 4.
#     Linear elements in rows [17, 18].

indJ[24] = 16
indJ[25] = 17

valJ[24] = -1.0
valJ[25] =  1.0

#     Column 5.
#     Nonlinear elements in rows [5, 9, 11, 13, 14].

locJ[ 4] = 26

indJ[26] =  4
indJ[27] =  8
indJ[28] = 10
indJ[29] = 12
indJ[30] = 13

valJ[26] =  0.0
valJ[27] =  0.0
valJ[28] =  0.0
valJ[29] =  0.0
valJ[30] =  0.0

#     Column 5.
#     Linear element in row 18.

indJ[31] = 17

valJ[31] = -1.0

#     Column 6.
#     Nonlinear elements in rows [1, 2, 3, 4, 5, 6].

locJ[5]  = 32

indJ[32] =  0
indJ[33] =  1
indJ[34] =  2
indJ[35] =  3
indJ[36] =  4

valJ[32] =  0.0
valJ[33] =  0.0
valJ[34] =  0.0
valJ[35] =  0.0
valJ[36] =  0.0

#     Column 7.
#     Nonlinear elements in rows [2, 6, 7, 8, 9].

locJ[6]  =  37

indJ[37] =  1
indJ[38] =  5
indJ[39] =  6
indJ[40] =  7
indJ[41] =  8

valJ[37] =  0.0
valJ[38] =  0.0
valJ[39] =  0.0
valJ[40] =  0.0
valJ[41] =  0.0

#     Column 8.
#     Nonlinear elements in rows [4, 8, 10, 12, 13].

locJ[7]  =  42

indJ[42] =  3
indJ[43] =  7
indJ[44] =  9
indJ[45] = 11
indJ[46] = 12

valJ[42] =  0.0
valJ[43] =  0.0
valJ[44] =  0.0
valJ[45] =  0.0
valJ[46] =  0.0

#     Column 9.
#     Nonlinear elements in rows [5, 9, 11, 13, 14].

locJ[8]  =  47

indJ[47] =  4
indJ[48] =  8
indJ[49] = 10
indJ[50] = 12
indJ[51] = 13

valJ[47] =  0.0
valJ[48] =  0.0
valJ[49] =  0.0
valJ[50] =  0.0
valJ[51] =  0.0

#     Don't forget to finish off  locJ.
#     This is crucial.

locJ[ 9] =  51 + 1

# Put components of J into a tuple
J = (valJ,indJ,locJ)

options.setOption('Verbose',True)
result = snoptb(hexObj,hexCon,nnObj=nnObj,nnCon=nnCon,nnJac=nnJac,
                name=' snmainb',x0=x,bl=bl,bu=bu,J=J,m=m,n=n,options=options)
