//$Id$
//------------------------------------------------------------------------------
//                         RauAutomaticaPointObject
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
 * Developed based on RauAutomaticaPointObject.m
 */
//------------------------------------------------------------------------------
#ifndef RauAutomaticaPointObject_hpp
#define RauAutomaticaPointObject_hpp

#include "UserPointFunction.hpp"

/**
 * RauAutomaticaPointObject class
 */
class RauAutomaticaPointObject : public UserPointFunction
{
public:

   /// default constructor
   RauAutomaticaPointObject();
   /// copy constructor
   RauAutomaticaPointObject(const RauAutomaticaPointObject &copy);
   /// operator=
   RauAutomaticaPointObject& operator=(const RauAutomaticaPointObject &copy);
   /// default destructor
   virtual ~RauAutomaticaPointObject();


   /// EvaluateFunctions
   void EvaluateFunctions();

   /// EvaluateJacobians
   void EvaluateJacobians();
};

#endif // RauAutomaticaPointObject_hpp