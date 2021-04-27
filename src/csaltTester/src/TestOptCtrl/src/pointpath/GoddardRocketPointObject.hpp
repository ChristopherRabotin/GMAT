//$Id$
//------------------------------------------------------------------------------
//                         GoddardRocketPointObject
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Claire Conway
// Created: 2017.02.21
//
/**
 * Developed based on GoddardRocketPointObject.m
 */
//------------------------------------------------------------------------------
#ifndef GoddardRocketPointObject_hpp
#define GoddardRocketPointObject_hpp

#include "UserPointFunction.hpp"

/**
 * GoddardRocketPointObject class
 */
class GoddardRocketPointObject : public UserPointFunction
{
public:
   
   /// default constructor
   GoddardRocketPointObject();
   /// copy constructor
   GoddardRocketPointObject(const GoddardRocketPointObject &copy);
   /// operator=
   GoddardRocketPointObject& operator=(const GoddardRocketPointObject &copy);
   /// default destructor
   virtual ~GoddardRocketPointObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
};

#endif // GoddardRocketPointObject_hpp