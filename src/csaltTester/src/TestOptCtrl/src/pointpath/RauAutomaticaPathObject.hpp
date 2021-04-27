//$Id$
//------------------------------------------------------------------------------
//                         RauAutomaticaPathObject
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Youngkwang Kim
// Created: 2016.11.07
//
/**
 * Developed based on RauAutomaticaPathObject.m
 */
//------------------------------------------------------------------------------
#ifndef RauAutomaticaPathObject_hpp
#define RauAutomaticaPathObject_hpp

#include "UserPathFunction.hpp"

/**
 * RauAutomaticaPathObject class
 */
class RauAutomaticaPathObject : public UserPathFunction
{
public:
   
   /// default constructor
   RauAutomaticaPathObject();
   /// copy constructor
   RauAutomaticaPathObject(const RauAutomaticaPathObject &copy);
   /// operator=
   RauAutomaticaPathObject& operator=(const RauAutomaticaPathObject &copy);
   /// default destructor
   virtual ~RauAutomaticaPathObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
};

#endif // RauAutomaticaPathObject_hpp