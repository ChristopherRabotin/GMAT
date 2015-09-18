# -*- coding: utf-8 -*-
"""
Created on Tue Feb 10 13:14:38 2015

@author: djc@thinksysinc.com
"""
import math

def sqrt(a):
   retval = -1.0;
   
   if a >= 0:
      retval = math.sqrt(a);
   
   return retval;

def cross(a, b, c, x, y, z):
   rx = b * z - c * y
   ry = c * x - a * z
   rz = a * y - b * x
   
   return [rx, ry, rz]
   
def magnitude(a, b, c):
   return sqrt(a**2 + b**2 + c**2)
   
def crossmag(a, b, c, x, y, z):
   data = cross(a, b, c, x, y, z)
   return magnitude(data[0], data[1], data[2])

"""
This function takes a 6 element input vector and builds the 3x3 wedge product,
using the first 3 elements if the input as the first vector and the remaining 
elements as the second
"""
def anticross(arr):
   am = []
   am.append([0,arr[0]*arr[4] - arr[1]*arr[3], arr[0]*arr[5] - arr[2]*arr[3]])
   am.append([0,0,arr[1]*arr[5] - arr[2]*arr[4]])
   am.append([0,0,0])
   
   am[1][0] = -am[0][1]
   am[2][0] = -am[0][2]
   am[2][1] = -am[1][2]
   
   return am
