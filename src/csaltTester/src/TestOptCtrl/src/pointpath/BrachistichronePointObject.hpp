//$Id$
//------------------------------------------------------------------------------
//                         BrachistichronePointObject
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
 * Developed based on BrachistichronePointObject.m
 */
//------------------------------------------------------------------------------
#ifndef BrachistichronePointObject_hpp
#define BrachistichronePointObject_hpp

#include "UserPointFunction.hpp"

/**
 * BrachistichronePointObject class
 */
class BrachistichronePointObject : public UserPointFunction
{
public:
   
   /// default constructor
   BrachistichronePointObject();
   /// copy constructor
   BrachistichronePointObject(const BrachistichronePointObject &copy);
   /// operator=
   BrachistichronePointObject& operator=(const BrachistichronePointObject &copy);
   /// default destructor
   virtual ~BrachistichronePointObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
};

#endif // BrachistichronePointObject_hpp