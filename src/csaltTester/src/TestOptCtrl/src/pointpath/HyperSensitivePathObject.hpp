//$Id$
//------------------------------------------------------------------------------
//                         HyperSensitivePathObject
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan
// Created: 2016.10.17
//
/**
 * Developed based on HyperSensitivePathObject.m
 */
//------------------------------------------------------------------------------
#ifndef HyperSensitivePathObject_hpp
#define HyperSensitivePathObject_hpp

#include "UserPathFunction.hpp"

/**
 * HyperSensitivePathObject class
 */
class HyperSensitivePathObject : public UserPathFunction
{
public:
   
   /// default constructor
   HyperSensitivePathObject();
   /// copy constructor
   HyperSensitivePathObject(const HyperSensitivePathObject &copy);
   /// operator=
   HyperSensitivePathObject& operator=(const HyperSensitivePathObject &copy);
   /// default destructor
   virtual ~HyperSensitivePathObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
};

#endif // HyperSensitivePathObject_hpp