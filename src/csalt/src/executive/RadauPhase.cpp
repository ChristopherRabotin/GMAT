//------------------------------------------------------------------------------
//                              RadauPhase
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan / NASA
// Created: 2016.02.29
//
/**
 * From original MATLAB prototype:
 * Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#include <sstream>
#include "RadauPhase.hpp"
#include "DecVecTypeBetts.hpp"
#include "LowThrustException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_RADAU_PHASE

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
RadauPhase::RadauPhase() :
   Phase()
{
   // For Radau, the number of defect constraints is one less than the number
   // of time points
   constraintTimeOffset = 1;
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
RadauPhase::RadauPhase(const RadauPhase &copy) :
   Phase(copy)
{
   // moved from superclass
   if (copy.transUtil)
   {
      if (transUtil) delete transUtil;
      transUtil = new NLPFuncUtilRadau(
                      dynamic_cast<NLPFuncUtilRadau&>(*(copy.transUtil)));
   }
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
RadauPhase& RadauPhase::operator=(const RadauPhase &copy)
{
   if (&copy == this)
      return *this;
   
   Phase::operator=(copy);
   // moved from superclass
   if (copy.transUtil)
   {
      if (transUtil) delete transUtil;
      transUtil = new NLPFuncUtilRadau(
                      dynamic_cast<NLPFuncUtilRadau&>(*(copy.transUtil)));
   }
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
RadauPhase::~RadauPhase()
{
}

//------------------------------------------------------------------------------
//  void InitializeTranscription()
//------------------------------------------------------------------------------
/**
 * This method initializes the transcription
 *
 */
//------------------------------------------------------------------------------
void RadauPhase::InitializeTranscription()
{
   #ifdef DEBUG_RADAU_PHASE
      MessageInterface::ShowMessage("In RadauPhase::InitializeTranscription\n");
   #endif
   // Initialize the collocation helper class
   if (transUtil)
      delete transUtil;
   transUtil = new NLPFuncUtilRadau();
   transUtil->Initialize(config);
   #ifdef DEBUG_RADAU_PHASE
      MessageInterface::ShowMessage(
                              "LEAVING RadauPhase::InitializeTranscription\n");
   #endif
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

