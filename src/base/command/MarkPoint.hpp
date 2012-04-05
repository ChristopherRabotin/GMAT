//$Id$
//------------------------------------------------------------------------------
//                                MarkPoint
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2009/10/06
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
/**
 * Class implementation for the MarkPoint command
 */
//------------------------------------------------------------------------------


#ifndef MarkPoint_hpp
#define MarkPoint_hpp

#include "PlotCommand.hpp"


/**
 * Command used to restore plotting on an XY plot during a run.
 */
class GMAT_API MarkPoint : public PlotCommand
{
public:
   MarkPoint();
   virtual         ~MarkPoint();
   MarkPoint(const MarkPoint &c);
   MarkPoint&      operator=(const MarkPoint &c);
   
   virtual GmatBase*    Clone() const;
   
   bool                 Initialize();
   bool                 Execute();
      
   DEFAULT_TO_NO_CLONES

protected:
   
};

#endif /* MarkPoint_hpp */
