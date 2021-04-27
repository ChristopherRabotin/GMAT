//$Id$
//------------------------------------------------------------------------------
//                         RayleighControlStateConstraintPathObject
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Claire Conway
// Created: 2017.02.16
//
/**
 * Developed based on RayleighControlStateConstraintPathObject.m
 */
//------------------------------------------------------------------------------
#ifndef RayleighControlStateConstraintPathObject_hpp
#define RayleighControlStateConstraintPathObject_hpp

#include "UserPathFunction.hpp"

/**
 * RayleighControlStateConstraintPathObject class
 */
class RayleighControlStateConstraintPathObject : public UserPathFunction
{
public:
   
   /// default constructor
   RayleighControlStateConstraintPathObject();
   /// copy constructor
   RayleighControlStateConstraintPathObject(const RayleighControlStateConstraintPathObject &copy);
   /// operator=
   RayleighControlStateConstraintPathObject& operator=(const RayleighControlStateConstraintPathObject &copy);
   /// default destructor
   virtual ~RayleighControlStateConstraintPathObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
	Real gravity;
};

#endif // RayleighControlStateConstraintPathObject_hpp