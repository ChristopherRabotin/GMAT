//$Header$
//------------------------------------------------------------------------------
//                                  MatlabWs
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/09/08
//
/**
 * Declares MatlabWs class which evaluates parameters to be sent to MATLAB
 * workspace.
 */
//------------------------------------------------------------------------------
#ifndef MatlabWs_hpp
#define MatlabWs_hpp

#include "Subscriber.hpp"

#include "Parameter.hpp"
#include <map>
#include <iostream>
#include <iomanip>

class MatlabWs : public Subscriber
{
public:
   MatlabWs(const std::string &name, Parameter *firstVarParam = NULL);

   virtual ~MatlabWs(void);

   MatlabWs(const MatlabWs &copy);
   MatlabWs& operator=(const MatlabWs &right);
    
   // methods inherited from Subscriber
   virtual bool Initialize();
                      
   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                     GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   
   virtual Integer     GetIntegerParameter(const Integer id) const;
   virtual Integer     SetIntegerParameter(const Integer id,
                                          const Integer value);
   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool        SetStringParameter(const Integer id,
                                          const std::string &value);
                                          
   virtual const StringArray& GetStringArrayParameter(const Integer id) const;
   virtual const StringArray& GetStringArrayParameter(const std::string &label) const;
   
   virtual bool GetBooleanParameter(const Integer id) const;
   virtual bool GetBooleanParameter(const std::string &label) const;
   virtual bool SetBooleanParameter(const Integer id, const bool value);
   virtual bool SetBooleanParameter(const std::string &label,
                                    const bool value);

   Integer GetNumVarParameters();
   bool AddVarParameter(const std::string &paramName);
   
protected:
   /// Precision for output of real data
   Integer precision;
   Integer mEvaluateFrequency;
   
   std::vector<Parameter*> mVarParams; //loj: 6/4/04 remove this later
   std::map<std::string, Parameter*> mVarParamMap;
   
   Integer mNumVarParams;
   
   StringArray mVarParamNames;
     
   virtual bool        Distribute(Integer len);
   virtual bool        Distribute(const Real * dat, Integer len);
   
private:
    void ClearVarParameters();
    bool initial;
    
    enum
    {
       PRECISION = SubscriberParamCount,
       VAR_LIST,
       ADD,
       CLEAR,
       MatlabWsParamCount  /// Count of the parameters for this class
    };

   static const std::string
      PARAMETER_TEXT[MatlabWsParamCount - SubscriberParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[MatlabWsParamCount - SubscriberParamCount];

};


#endif
