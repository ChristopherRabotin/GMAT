//$Id$
//------------------------------------------------------------------------------
//                         RayleighPathObject
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Claire Conway
// Created: 2017.02.03
//
/**
 * Developed based on RayleighPathObject.m
 */
//------------------------------------------------------------------------------
#ifndef RayleighPathObject_hpp
#define RayleighPathObject_hpp

#include "UserPathFunction.hpp"

/**
 * RayleighPathObject class
 */
class RayleighPathObject : public UserPathFunction
{
public:
   
   /// default constructor
   RayleighPathObject();
   /// copy constructor
   RayleighPathObject(const RayleighPathObject &copy);
   /// operator=
   RayleighPathObject& operator=(const RayleighPathObject &copy);
   /// default destructor
   virtual ~RayleighPathObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
	Real gravity;
};

#endif // RayleighPathObject_hpp