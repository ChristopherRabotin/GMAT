//$Header$
//------------------------------------------------------------------------------
//                                  ReportFile
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: LaMont Ruley
// Created: 2003/10/23
// Modified:  2004/7/16 by Allison Greene to include copy constructor
//                         and operator=
//
/**
 * Definition for the ReportFile class.
 */
//------------------------------------------------------------------------------


#ifndef ReportFile_hpp
#define ReportFile_hpp


#include "Subscriber.hpp"
#include <fstream>

#include "Parameter.hpp"
#include <map>
#include <iostream>
#include <iomanip>

class ReportFile :
    public Subscriber
{
public:
   ReportFile(const std::string &name, const std::string &fileName = "", 
               Parameter *firstVarParam = NULL);

   virtual ~ReportFile(void);

   ReportFile(const ReportFile &);
   ReportFile& operator=(const ReportFile&);
    
   // methods inherited from Subscriber
   virtual bool Initialize();
                      
   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;

   virtual bool TakeAction(const std::string &action,
                           const std::string &actionData = "");
   
   virtual bool RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName);
   
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                          const Integer value);

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool SetStringParameter(const Integer id, const std::string &value);
   virtual bool SetStringParameter(const std::string &label,
                                   const std::string &value);

   virtual bool SetStringParameter(const Integer id, const std::string &value,
                                   const Integer index);
   virtual bool SetStringParameter(const std::string &label,
                                   const std::string &value,
                                   const Integer index);
   virtual const StringArray& GetStringArrayParameter(const Integer id) const;
   virtual const StringArray& GetStringArrayParameter(const std::string &label) const;
   

   virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name);
   virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name = "");

   virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type);

   Integer GetNumVarParameters();
   bool AddVarParameter(const std::string &paramName, Integer index);
   
protected:
   /// Name of the report file
   std::string    filename;
   /// Precision for output of real data
   Integer        precision;  
   /// Width of column
   Integer        columnWidth;   
   /// Write the headers on the top of the column
   bool           writeHeaders;
   
   /// output data stream
   std::ofstream       dstream;
   std::vector<Parameter*> mVarParams;

   Integer mNumVarParams;
   StringArray mVarParamNames;
   StringArray mAllRefObjectNames;
   Integer lastUsedProvider;
   
   virtual bool        Distribute(Integer len);
   virtual bool        Distribute(const Real * dat, Integer len);
   virtual bool        OpenReportFile(void);
   
private:
    void ClearVarParameters();
    void WriteHeaders();
    bool initial;

    enum
    {
                FILENAME = SubscriberParamCount,
      PRECISION,
      ADD,
      WRITE_HEADERS,
      COL_WIDTH,
      ReportFileParamCount  /// Count of the parameters for this class
    };

    static const std::string PARAMETER_TEXT[ReportFileParamCount - SubscriberParamCount];
         static const Gmat::ParameterType PARAMETER_TYPE[ReportFileParamCount - SubscriberParamCount];

};


#endif
