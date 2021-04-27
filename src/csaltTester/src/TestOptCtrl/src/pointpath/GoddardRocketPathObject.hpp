//$Id$
//------------------------------------------------------------------------------
//                         GoddardRocketPathObject
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
 * Developed based on GoddardRocketPathObject.m
 */
//------------------------------------------------------------------------------
#ifndef GoddardRocketPathObject_hpp
#define GoddardRocketPathObject_hpp

#include "UserPathFunction.hpp"

/**
 * GoddardRocketPathObject class
 */
class GoddardRocketPathObject : public UserPathFunction
{
public:
   
   /// default constructor
   GoddardRocketPathObject();
   /// copy constructor
   GoddardRocketPathObject(const GoddardRocketPathObject &copy);
   /// operator=
   GoddardRocketPathObject& operator=(const GoddardRocketPathObject &copy);
   /// default destructor
   virtual ~GoddardRocketPathObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
   Real gravity;
};

#endif // GoddardRocketPathObject_hpp