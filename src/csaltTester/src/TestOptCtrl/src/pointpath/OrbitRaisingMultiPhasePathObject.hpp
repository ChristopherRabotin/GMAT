//$Id$
//------------------------------------------------------------------------------
//                         OrbitRaisingMultiPhasePathObject
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan
// Created: 2016.09.28
//
/**
 * Developed based on OrbitRaisingMultiPhasePathObject.m
 */
//------------------------------------------------------------------------------
#ifndef OrbitRaisingMultiPhasePathObject_hpp
#define OrbitRaisingMultiPhasePathObject_hpp

#include "UserPathFunction.hpp"

/**
 * OrbitRaisingMultiPhasePathObject class
 */
class OrbitRaisingMultiPhasePathObject : public UserPathFunction
{
public:
   
   /// default constructor
   OrbitRaisingMultiPhasePathObject();
   /// copy constructor
   OrbitRaisingMultiPhasePathObject(const OrbitRaisingMultiPhasePathObject &copy);
   /// operator=
   OrbitRaisingMultiPhasePathObject& operator=(const OrbitRaisingMultiPhasePathObject &copy);
   /// default destructor
   virtual ~OrbitRaisingMultiPhasePathObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
};

#endif // OrbitRaisingMultiPhasePathObject_hpp