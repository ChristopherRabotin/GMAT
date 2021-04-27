//$Id$
//------------------------------------------------------------------------------
//                         MoonLanderPathObject
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
 * Developed based on MoonLanderPathObject.m
 */
//------------------------------------------------------------------------------
#ifndef MoonLanderPathObject_hpp
#define MoonLanderPathObject_hpp

#include "UserPathFunction.hpp"

/**
 * MoonLanderPathObject class
 */
class MoonLanderPathObject : public UserPathFunction
{
public:
   
   /// default constructor
   MoonLanderPathObject();
   /// copy constructor
   MoonLanderPathObject(const MoonLanderPathObject &copy);
   /// operator=
   MoonLanderPathObject& operator=(const MoonLanderPathObject &copy);
   /// default destructor
   virtual ~MoonLanderPathObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
	Real gravity;
};

#endif // MoonLanderPathObject_hpp