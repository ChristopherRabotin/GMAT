//$Id$
//------------------------------------------------------------------------------
//                         BreakwellPathObject
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
 * Developed based on BreakwellPathObject.m
 */
//------------------------------------------------------------------------------
#ifndef BreakwellPathObject_hpp
#define BreakwellPathObject_hpp

#include "UserPathFunction.hpp"

/**
 * BreakwellPathObject class
 */
class BreakwellPathObject : public UserPathFunction
{
public:
   
   /// default constructor
   BreakwellPathObject();
   /// copy constructor
   BreakwellPathObject(const BreakwellPathObject &copy);
   /// operator=
   BreakwellPathObject& operator=(const BreakwellPathObject &copy);
   /// default destructor
   virtual ~BreakwellPathObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
	Real gravity;
};

#endif // BreakwellPathObject_hpp