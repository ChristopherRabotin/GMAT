//$Id$
//------------------------------------------------------------------------------
//                         ConwayOrbitExamplePointObject
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
 * Developed based on ConwayOrbitExamplePointObject.m
 */
//------------------------------------------------------------------------------
#ifndef ConwayOrbitExamplePointObject_hpp
#define ConwayOrbitExamplePointObject_hpp

#include "UserPointFunction.hpp"

/**
 * ConwayOrbitExamplePointObject class
 */
class ConwayOrbitExamplePointObject : public UserPointFunction
{
public:
   
   /// default constructor
   ConwayOrbitExamplePointObject();
   /// copy constructor
   ConwayOrbitExamplePointObject(const ConwayOrbitExamplePointObject &copy);
   /// operator=
   ConwayOrbitExamplePointObject& operator=(const ConwayOrbitExamplePointObject &copy);
   /// default destructor
   virtual ~ConwayOrbitExamplePointObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
};

#endif // ConwayOrbitExamplePointObject_hpp