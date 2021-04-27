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
// Created: 2016.05.18
//
/**
 * From original MATLAB prototype:
 * Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#ifndef RadauPhase_hpp
#define RadauPhase_hpp

#include "Phase.hpp"
#include "csaltdefs.hpp"
#include "Rmatrix.hpp"
#include "Rvector.hpp"

class CSALT_API RadauPhase : public Phase
{
public:
   
   RadauPhase();
   RadauPhase(const RadauPhase &copy);
   RadauPhase& operator=(const RadauPhase &copy);
   virtual ~RadauPhase();   

   /// Initializes the RadauPhase
   virtual void  InitializeTranscription();
  
protected:
   
};
#endif // RadauPhase_hpp
