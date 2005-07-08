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
#include "Parameter.hpp"

class GMAT_API Report : public GmatCommand
{
public:
	Report();
	virtual ~Report();
   
   Report(const Report &rep);
   Report&           operator=(const Report &rep);
   
   
//   virtual bool      SetRefObjectName(const Gmat::ObjectType type,
//                                      const std::string &name);
   virtual bool      SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                  const std::string &name,
                                  const Integer index);

   virtual GmatBase* Clone() const;
   
   bool              Initialize();
   virtual bool      Execute();
   
protected:
   /// Name of the subscriber
   std::string       rfName;
   /// The ReportFile subscriber that receives the data
   ReportFile        *reporter;
   /// The ID for the subscriber
   Integer           reportID;
   /// Array of parameter names
   StringArray       parmNames;
   /// Array of parameters
   std::vector<Parameter*>
                     parms;
};

#endif      // Report_hpp
