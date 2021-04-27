//$Id$
//------------------------------------------------------------------------------
//                         HyperSensitivePointObject
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan
// Created: 2016.10.18
//
/**
 * Developed based on HyperSensitivePointObject.m
 */
//------------------------------------------------------------------------------
#ifndef HyperSensitivePointObject_hpp
#define HyperSensitivePointObject_hpp

#include "UserPointFunction.hpp"

/**
 * HyperSensitivePointObject class
 */
class HyperSensitivePointObject : public UserPointFunction
{
public:
   
   /// default constructor
   HyperSensitivePointObject();
   /// copy constructor
   HyperSensitivePointObject(const HyperSensitivePointObject &copy);
   /// operator=
   HyperSensitivePointObject& operator=(const HyperSensitivePointObject &copy);
   /// default destructor
   virtual ~HyperSensitivePointObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
};

#endif // HyperSensitivePointObject_hpp