//$Id$
//------------------------------------------------------------------------------
//                         RayleighControlStateConstraintPointObject
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
 * Developed based on RayleighControlStateConstraintPointObject.m
 */
//------------------------------------------------------------------------------
#ifndef RayleighControlStateConstraintPointObject_hpp
#define RayleighControlStateConstraintPointObject_hpp

#include "UserPointFunction.hpp"

/**
 * RayleighControlStateConstraintPointObject class
 */
class RayleighControlStateConstraintPointObject : public UserPointFunction
{
public:
   
   /// default constructor
   RayleighControlStateConstraintPointObject();
   /// copy constructor
   RayleighControlStateConstraintPointObject(const RayleighControlStateConstraintPointObject &copy);
   /// operator=
   RayleighControlStateConstraintPointObject& operator=(const RayleighControlStateConstraintPointObject &copy);
   /// default destructor
   virtual ~RayleighControlStateConstraintPointObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
};

#endif // RayleighControlStateConstraintPointObject_hpp