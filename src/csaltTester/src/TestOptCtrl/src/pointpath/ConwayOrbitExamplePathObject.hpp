//$Id$
//------------------------------------------------------------------------------
//                         ConwayOrbitExamplePathObject
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
 * Developed based on ConwayOrbitExamplePathObject.m
 */
//------------------------------------------------------------------------------
#ifndef ConwayOrbitExamplePathObject_hpp
#define ConwayOrbitExamplePathObject_hpp

#include "UserPathFunction.hpp"

/**
 * ConwayOrbitExamplePathObject class
 */
class ConwayOrbitExamplePathObject : public UserPathFunction
{
public:
   
   /// default constructor
   ConwayOrbitExamplePathObject();
   /// copy constructor
   ConwayOrbitExamplePathObject(const ConwayOrbitExamplePathObject &copy);
   /// operator=
   ConwayOrbitExamplePathObject& operator=(const ConwayOrbitExamplePathObject &copy);
   /// default destructor
   virtual ~ConwayOrbitExamplePathObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
	Real gravity;
};

#endif // ConwayOrbitExamplePathObject_hpp