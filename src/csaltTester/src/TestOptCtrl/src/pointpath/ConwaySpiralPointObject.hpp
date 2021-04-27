//$Id$
//------------------------------------------------------------------------------
//                         ConwaySpiralPointObject
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Claire Conway
// Created: 2017.02.13
//
/**
 * Developed based on ConwaySpiralPointObject.m
 */
//------------------------------------------------------------------------------
#ifndef ConwaySpiralPointObject_hpp
#define ConwaySpiralPointObject_hpp

#include "UserPointFunction.hpp"

/**
 * ConwaySpiralPointObject class
 */
class ConwaySpiralPointObject : public UserPointFunction
{
public:
   
   /// default constructor
   ConwaySpiralPointObject();
   /// copy constructor
   ConwaySpiralPointObject(const ConwaySpiralPointObject &copy);
   /// operator=
   ConwaySpiralPointObject& operator=(const ConwaySpiralPointObject &copy);
   /// default destructor
   virtual ~ConwaySpiralPointObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
};

#endif // ConwaySpiralPointObject_hpp