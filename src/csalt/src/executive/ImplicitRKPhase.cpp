//------------------------------------------------------------------------------
//                              ImplicitRKPhase
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan / NASA
// Created: 2016.05.18
//
/**
* From original MATLAB prototype:
* Author: S. Hughes.  steven.p.hughes@nasa.gov
*/
//------------------------------------------------------------------------------

#include "ImplicitRKPhase.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
ImplicitRKPhase::ImplicitRKPhase() :
   Phase()
{
   // For ImplicitRK, the number of defect constraints is one less than the number
   // of time points
   constraintTimeOffset = 1;
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
ImplicitRKPhase::ImplicitRKPhase(const ImplicitRKPhase &copy) :
   Phase(copy),
   collocationMethod (copy.collocationMethod)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
ImplicitRKPhase& ImplicitRKPhase::operator=(const ImplicitRKPhase &copy)
{
   if (this == &copy)
      return *this;
   
   Phase::operator=(copy);

   collocationMethod = copy.collocationMethod;

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
ImplicitRKPhase::~ImplicitRKPhase()
{
}

//------------------------------------------------------------------------------
//  void InitializeTranscription()
//------------------------------------------------------------------------------
/**
 * This method initializes the ImplicitRKPhase
 *
 *
 */
//------------------------------------------------------------------------------
void  ImplicitRKPhase::InitializeTranscription()
{
   // default collocation is RungeKutta 8
   if (strcmp(collocationMethod.c_str(),"") == 0)
   {
      SetTranscription("RungeKutta8");
      if (transUtil)
         delete transUtil;
      transUtil = new NLPFuncUtil_ImplicitRK(collocationMethod);
      transUtil->Initialize(config);
   }
   else
   {
      if (transUtil)
         delete transUtil;
      transUtil = new NLPFuncUtil_ImplicitRK(collocationMethod);
      transUtil->Initialize(config);
   }
}

//------------------------------------------------------------------------------
//  void SetTranscription(std::string type)
//------------------------------------------------------------------------------
/**
 * This method sets the transcription
 *
 * @param <type>       the input collocation type
 *
 */
//------------------------------------------------------------------------------
void ImplicitRKPhase::SetTranscription(std::string type)
{
   collocationMethod = type;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none

