//$Header$
//------------------------------------------------------------------------------
//                                 CallFunction
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Allison Greene
// Created: 2004/09/22
//
/**
 * Definition for the CallFunction command class
 */
//------------------------------------------------------------------------------


#ifndef CallFunction_hpp
#define CallFunction_hpp

#include "Command.hpp"
#include "Function.hpp"

#include "Parameter.hpp"
#include "Array.hpp"
#include "StringVar.hpp"

//#include <map>
#include <iostream>
#include <iomanip>

class CallFunction : public GmatCommand
{
public:
   CallFunction();
   virtual ~CallFunction();
    
   CallFunction(const CallFunction& cf);
   CallFunction&               operator=(const CallFunction& cf);
    
   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   virtual bool TakeAction(const std::string &action,
                          const std::string &actionData = "");

   // Methods used for configuration
   StringArray         GetRefObjectNameArray(const Gmat::ObjectType type) const;

   virtual bool        RenameRefObject(const Gmat::ObjectType type,
                                       const std::string &oldName,
                                       const std::string &newName);

   // Reference object accessor methods
   virtual GmatBase*   GetRefObject(const Gmat::ObjectType type,
                                    const std::string &name);
   virtual bool        SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                    const std::string &name = "");
   virtual ObjectArray& GetRefObjectArray(const Gmat::ObjectType type);

   // Parameter accessor methods -- overridden from GmatBase
   virtual std::string     GetParameterText(const Integer id) const;
   virtual Integer         GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                           GetParameterType(const Integer id) const;
   virtual std::string     GetParameterTypeString(const Integer id) const;

   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const std::string &label) const;
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
   
   // Methods used to run the command
   virtual bool            InterpretAction();
    
   virtual bool            Initialize();
   virtual bool            Execute();

   std::string FormEvalString();
   
   bool AddInputParameter(const std::string &paramName, Integer index);
   bool AddOutputParameter(const std::string &paramName, Integer index);

protected:

private:
   ObjectArray objectArray;
   std::vector<Parameter*> mInputList;
   std::vector<Parameter*> mOutputList;

   StringArray mInputListNames;
   StringArray mOutputListNames;

   Integer mNumInputParams;
   Integer mNumOutputParams;

   Function *mFunction;
   std::string mFunctionName;

   bool ExecuteMatlabFunction();
   void SendInParam(Parameter *param);
   void GetOutParams();
   void EvalMatlabString(std::string evalString);
   void ClearInputParameters();
   void ClearOutputParameters();

   enum
   {
      FUNCTION_NAME = GmatCommandParamCount,
      ADD_INPUT,
      ADD_OUTPUT,
      CallFunctionParamCount  /// Count of the parameters for this class
   };

   static const std::string PARAMETER_TEXT[
            CallFunctionParamCount - GmatCommandParamCount];
   static const Gmat::ParameterType PARAMETER_TYPE[
            CallFunctionParamCount - GmatCommandParamCount];

};

#endif // CallFunction_hpp
