//$Id$
//------------------------------------------------------------------------------
//                         OrbitRaisingPathObject
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan
// Created: 2016.05.25
//
/**
 * Developed based on OrbitRaisingPathObject.m
 */
//------------------------------------------------------------------------------
#ifndef OrbitRaisingPathObject_hpp
#define OrbitRaisingPathObject_hpp

#include "UserPathFunction.hpp"

/**
 * OrbitRaisingPathObject class
 */
class OrbitRaisingPathObject : public UserPathFunction
{
public:
   
   /// default constructor
   OrbitRaisingPathObject();
   /// copy constructor
   OrbitRaisingPathObject(const OrbitRaisingPathObject &copy);
   /// operator=
   OrbitRaisingPathObject& operator=(const OrbitRaisingPathObject &copy);
   /// default destructor
   virtual ~OrbitRaisingPathObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
};

#endif // OrbitRaisingPathObject_hpp