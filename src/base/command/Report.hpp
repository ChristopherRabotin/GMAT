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
 *  Class definition for the Report command.
 */
//------------------------------------------------------------------------------
 
#ifndef Report_hpp
#define Report_hpp

#include "Command.hpp"
#include "ReportFile.hpp"

class GMAT_API Report : public GmatCommand
{
public:
	Report();
	virtual ~Report();
   
   Report(const Report &rep);
   Report&           operator=(const Report &rep);
   
   virtual GmatBase* Clone() const;
   
   // Temporarily put this here to build the shell
   virtual bool      InterpretAction();
   
   virtual bool      Execute();
   
protected:
   /// Name of the subscriber
   std::string       rfName;
   /// The ReportFile subscriber that receives the data
   ReportFile        *reporter;
};

#endif      // Report_hpp
