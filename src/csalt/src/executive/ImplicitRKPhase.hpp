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

#ifndef ImplicitRKPhase_hpp
#define ImplicitRKPhase_hpp

#include "Phase.hpp"
#include "csaltdefs.hpp"
#include "Rmatrix.hpp"
#include "Rvector.hpp"
#include "NLPFuncUtil_ImplicitRK.hpp"

class CSALT_API ImplicitRKPhase : public Phase
{
public:
   
   ImplicitRKPhase();
   ImplicitRKPhase(const ImplicitRKPhase &copy);
   ImplicitRKPhase& operator=(const ImplicitRKPhase &copy);
   ~ImplicitRKPhase();

   /// Initializes the ImplicitRKPhase
   void InitializeTranscription();

   void SetTranscription(std::string type);
  
protected:
   /// the collocation method
   std::string collocationMethod;
};
#endif // ImplicitRKPhase_hpp
