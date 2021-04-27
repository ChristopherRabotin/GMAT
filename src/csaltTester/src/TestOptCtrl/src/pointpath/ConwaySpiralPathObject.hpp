//$Id$
//------------------------------------------------------------------------------
//                         ConwaySpiralPathObject
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
 * Developed based on ConwaySpiralPathObject.m
 */
//------------------------------------------------------------------------------
#ifndef ConwaySpiralPathObject_hpp
#define ConwaySpiralPathObject_hpp

#include "UserPathFunction.hpp"

/**
 * ConwaySpiralPathObject class
 */
class ConwaySpiralPathObject : public UserPathFunction
{
public:
   
   /// default constructor
   ConwaySpiralPathObject();
   /// copy constructor
   ConwaySpiralPathObject(const ConwaySpiralPathObject &copy);
   /// operator=
   ConwaySpiralPathObject& operator=(const ConwaySpiralPathObject &copy);
   /// default destructor
   virtual ~ConwaySpiralPathObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
	Real gravity;
};

#endif // ConwaySpiralPathObject_hpp