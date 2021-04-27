//$Id$
//------------------------------------------------------------------------------
//                         BrachistichronePathObject
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Jeremy Knittel
// Created: 2016.11.03
//
/**
 * Developed based on BrachistichronePathObject.m
 */
//------------------------------------------------------------------------------
#ifndef BrachistichronePathObject_hpp
#define BrachistichronePathObject_hpp

#include "UserPathFunction.hpp"

/**
 * BrachistichronePathObject class
 */
class BrachistichronePathObject : public UserPathFunction
{
public:
   
   /// default constructor
   BrachistichronePathObject();
   /// copy constructor
   BrachistichronePathObject(const BrachistichronePathObject &copy);
   /// operator=
   BrachistichronePathObject& operator=(const BrachistichronePathObject &copy);
   /// default destructor
   virtual ~BrachistichronePathObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
	Real gravity;
};

#endif // BrachistichronePathObject_hpp