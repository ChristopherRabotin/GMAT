//$Id$
//------------------------------------------------------------------------------
//                                 PenUp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2004/02/26
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
/**
 * Class declaration for the PenUp command
 */
//------------------------------------------------------------------------------


#ifndef PenUp_hpp
#define PenUp_hpp

#include "PlotCommand.hpp"


/**
 * Command used to stop drawing data on an XY plot during a run.
 */
class GMAT_API PenUp : public PlotCommand
{
public:
   PenUp();
   virtual     ~PenUp();
   PenUp(const PenUp &c);
   PenUp&      operator=(const PenUp &c);
   
   virtual GmatBase* Clone() const;
   
   bool              Initialize();
   bool              Execute();

   DEFAULT_TO_NO_CLONES

protected:

};

#endif /* PenUp_hpp */
