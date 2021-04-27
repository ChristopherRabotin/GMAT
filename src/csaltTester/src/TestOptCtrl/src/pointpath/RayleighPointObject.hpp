//$Id$
//------------------------------------------------------------------------------
//                         RayleighPointObject
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
 * Developed based on RayleighPointObject.m
 */
//------------------------------------------------------------------------------
#ifndef RayleighPointObject_hpp
#define RayleighPointObject_hpp

#include "UserPointFunction.hpp"

/**
 * RayleighPointObject class
 */
class RayleighPointObject : public UserPointFunction
{
public:
   
   /// default constructor
   RayleighPointObject();
   /// copy constructor
   RayleighPointObject(const RayleighPointObject &copy);
   /// operator=
   RayleighPointObject& operator=(const RayleighPointObject &copy);
   /// default destructor
   virtual ~RayleighPointObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
};

#endif // RayleighPointObject_hpp