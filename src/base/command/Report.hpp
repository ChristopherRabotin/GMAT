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

#include "GmatCommand.hpp"
#include "ReportFile.hpp"
#include "Parameter.hpp"

/**
 * The Report command is used to write data to a ReportFile at specific times.
 * 
 * The ReportFile object is a subscriber used to generate data in an ASCII file.
 * Parameters added directly to the ReportFile are written whenever the 
 * publisher is fed data.  Parmeters that need to be seen only at specific times 
 * in a script are published using this command.
 * 
 * @note The Publisher does not currently pass text strings correctly.  The
 *       Report command is set to directly write data to the ReportFile until
 *       this defect is corrected.
 */
class GMAT_API Report : public GmatCommand
{
public:
   Report();
   virtual ~Report();
   Report(const Report &rep);
   Report& operator=(const Report &rep);
   
   virtual bool        TakeAction(const std::string &action,  
                                  const std::string &actionData = "");
   virtual std::string GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const StringArray&
                       GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool        RenameRefObject(const Gmat::ObjectType type,
                                       const std::string &oldName,
                                       const std::string &newName);
   virtual bool        SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                    const std::string &name,
                                    const Integer index);
   
   virtual GmatBase*   Clone() const;
   virtual const std::string&
                       GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
                                           const std::string &prefix = "",
                                           const std::string &useName = "");
   
   bool              Initialize();
   virtual bool      Execute();
   
protected:
   /// Name of the subscriber
   std::string                rfName;
   /// The ReportFile subscriber that receives the data
   ReportFile                 *reporter;
   /// The ID for the subscriber
   Integer                    reportID;
   /// Array of parameter names
   StringArray                parmNames;
   /// Array of actual parameter names including index
   StringArray                actualParmNames;
   /// Array of parameters that get written to the report
   std::vector<Parameter*>    parms;
   /// Flag indicating whether the header data has been written
   bool                       needsHeaders;
   /// Array of parameter row index
   std::vector<Integer>       parmRows;
   /// Array of parameter column index
   std::vector<Integer>       parmCols;

   void WriteHeaders(std::stringstream &datastream, Integer colWidth);
};

#endif      // Report_hpp
