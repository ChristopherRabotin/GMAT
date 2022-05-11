#!/usr/bin/env python

import numpy    as np
from   snopt import sqopt, SNOPT_options

def userHx(x, Hx, state):
    for i in range(0,5):
        Hx[3*i  ] = .0002*x[3*i]
        Hx[3*i+1] = .0002*x[3*i+1]
        Hx[3*i+2] = .0003*x[3*i+2]

    return Hx

inf    = 1.0e+20

m      = 17
n      = 15

x0     = np.zeros(n)

xl = np.zeros(n)
xl[ 0] =  8.0
xl[ 1] = 43.0
xl[ 2] =  3.0

xu = np.array([inf, 57.0, 16.0, 90.0, 120.0, 60.0, 90.0, 120.0,
               60.0, 90.0, 120.0, 60.0, 90.0, 120.0, 60.0])

A       = np.zeros((m,n))
A[ 0,0] = -1.0
A[12,0] = 1.0

A[ 4,1] = -1.0
A[12,1] = 1.0

A[ 8,2] = -1.0
A[12,2] = 1.0

A[ 0,3] = 1.0
A[ 1,3] = -1.0
A[13,3] = 1.0

A[ 4,4] = 1.0
A[ 5,4] = -1.0
A[13,4] = 1.0

A[ 8,5] = 1.0
A[ 9,5] = -1.0
A[13,5] = 1.0

A[ 1,6] = 1.0
A[ 2,6] = -1.0
A[14,6] = 1.0

A[ 5,7] = 1.0
A[ 6,7] = -1.0
A[14,7] = 1.0

A[ 9,8] = 1.0
A[10,8] = -1.0
A[14,8] = 1.0

A[ 2,9] = 1.0
A[ 3,9] = -1.0
A[15,9] = 1.0

A[ 6,10] = 1.0
A[ 7,10] = -1.0
A[15,10] = 1.0

A[10,11] = 1.0
A[11,11] = -1.0
A[15,11] = 1.0

A[ 3,12] = 1.0
A[16,12] = 1.0

A[ 7,13] = 1.0
A[16,13] = 1.0

A[11,14] = 1.0
A[16,14] = 1.0

al = np.array([ -7.0, -7.0, -7.0, -7.0, -7.0, -7.0, -7.0, -7.0, -7.0,
                -7.0, -7.0, -7.0, 60.0, 50.0, 70.0, 85.0, 100.0])

au = np.array([6.0, 6.0, 6.0, 6.0, 7.0, 7.0, 7.0, 7.0,
               6.0, 6.0, 6.0, 6.0, inf, inf, inf, inf, inf])

c = np.array([ 2.3,  1.7,  2.2,  2.3,  1.7,  2.2,  2.3,  1.7,
               2.2,  2.3,  1.7,  2.2,  2.3,  1.7,  2.2 ])


# Solve the problem
options = SNOPT_options()
options.setOption('Print frequency',1)
options.setOption('Summary frequency',1)

result = sqopt(userHx, x0, xl=xl, xu=xu, A=A, al=al, au=au, c=c, name='HS118   ', options=options)
