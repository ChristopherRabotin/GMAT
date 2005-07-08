//$Header$
//------------------------------------------------------------------------------
//                            Report
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Purchase
// order MOMS418823
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2005/07/06
//
/**
 *  Class implementation for the Report command.
 */
//------------------------------------------------------------------------------


#include "Report.hpp"


Report::Report() :
   GmatCommand ("Report"),
   rfName      (""),
   reporter    (NULL)
{
}


Report::~Report()
{
}


Report::Report(const Report &rep) :
   GmatCommand    (rep),
   rfName         (rep.rfName),
   reporter       (NULL)
{
}


Report& Report::operator=(const Report &rep)
{
   if (this != &rep)
   {
      rfName = rep.rfName;
      reporter = NULL;
   }
   
   return *this;
}


GmatBase* Report::Clone() const
{
   return new Report(*this);
}


bool Report::InterpretAction()
{
   return true;
}


bool Report::Execute()
{
   return true;
}
