//$Id$
//------------------------------------------------------------------------------
//                               ClearPlot
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
 * Class implementation for the ClearPlot command
 */
//------------------------------------------------------------------------------


#ifndef ClearPlot_hpp
#define ClearPlot_hpp

#include "PlotCommand.hpp"


/**
 * Command used to remove data from an XY plot during a run
 */
class GMAT_API ClearPlot : public PlotCommand
{
public:
   ClearPlot();
   virtual          ~ClearPlot();
   ClearPlot(const ClearPlot &c);
   ClearPlot&      operator=(const ClearPlot &c);
   
   virtual GmatBase* Clone() const;   
   
   bool              Initialize();
   bool              Execute();

   DEFAULT_TO_NO_CLONES

protected:

};

#endif /* ClearPlot_hpp */
