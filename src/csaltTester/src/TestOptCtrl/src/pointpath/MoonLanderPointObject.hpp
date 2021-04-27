//$Id$
//------------------------------------------------------------------------------
//                         MoonLanderPointObject
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
 * Developed based on MoonLanderPointObject.m
 */
//------------------------------------------------------------------------------
#ifndef MoonLanderPointObject_hpp
#define MoonLanderPointObject_hpp

#include "UserPointFunction.hpp"

/**
 * MoonLanderPointObject class
 */
class MoonLanderPointObject : public UserPointFunction
{
public:
   
   /// default constructor
   MoonLanderPointObject();
   /// copy constructor
   MoonLanderPointObject(const MoonLanderPointObject &copy);
   /// operator=
   MoonLanderPointObject& operator=(const MoonLanderPointObject &copy);
   /// default destructor
   virtual ~MoonLanderPointObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
};

#endif // MoonLanderPointObject_hpp