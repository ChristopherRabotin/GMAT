//$Id$
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

class MatlabWs : public Subscriber
{
public:
   MatlabWs(const std::string &name, Parameter *firstParam = NULL);
   virtual ~MatlabWs(void);
   MatlabWs(const MatlabWs &copy);
   MatlabWs& operator=(const MatlabWs &right);
   
   Integer              GetNumParameters();
   bool                 AddParameter(const std::string &paramName, Integer index);
   
   // methods inherited from Subscriber
   virtual bool         Initialize();
   
   // inherited from GmatBase
   virtual GmatBase*    Clone(void) const;
   
   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");
   
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id, const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);
   
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   
   virtual bool         SetStringParameter(const Integer id, const std::string &value,
                                           const Integer index);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;
   
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");
   
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   
protected:
   Integer mUpdateFrequency;
   Integer mNumParams;
   Integer mDataCount;
   Integer mSendCount;
   
   std::vector<Parameter*> mParams;
   StringArray mParamNames;
   StringArray mAllRefObjectNames;
   
   virtual bool         Distribute(Integer len);
   virtual bool         Distribute(const Real * dat, Integer len);
   
private:
   void ClearParameters();
   
   enum
   {
      ADD = SubscriberParamCount,
      UPDATE_FREQUENCY,
      MatlabWsParamCount  /// Count of the parameters for this class
   };
   
   static const std::string
      PARAMETER_TEXT[MatlabWsParamCount - SubscriberParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[MatlabWsParamCount - SubscriberParamCount];
   
};


#endif
