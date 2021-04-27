//$Id$
//------------------------------------------------------------------------------
//                         OrbitRaisingMultiPhasePointObject
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan
// Created: 2016.07.21
//
/**
 * Developed based on OrbitRaisingMultiPhasePointObject.m
 */
//------------------------------------------------------------------------------
#ifndef OrbitRaisingMultiPhasePointObject_hpp
#define OrbitRaisingMultiPhasePointObject_hpp

#include "UserPointFunction.hpp"

/**
 * OrbitRaisingMultiPhasePointObject class
 */
class OrbitRaisingMultiPhasePointObject : public UserPointFunction
{
public:
   
   /// default constructor
   OrbitRaisingMultiPhasePointObject();
   /// copy constructor
   OrbitRaisingMultiPhasePointObject(const OrbitRaisingMultiPhasePointObject &copy);
   /// operator=
   OrbitRaisingMultiPhasePointObject& operator=(const OrbitRaisingMultiPhasePointObject &copy);
   /// default destructor
   virtual ~OrbitRaisingMultiPhasePointObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
};

#endif // OrbitRaisingMultiPhasePointObject_hpp