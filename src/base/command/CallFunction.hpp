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
   virtual std::string GetRefObjectName(const Gmat::ObjectType type) const;
   StringArray         GetRefObjectNameArray(const Gmat::ObjectType type) const;
   virtual bool        SetRefObjectName(const Gmat::ObjectType type,
                                            const std::string &name);
   
   virtual bool        RenameRefObject(const Gmat::ObjectType type,
                                       const std::string &oldName,
                                       const std::string &newName);

   virtual bool        SetObject(GmatBase *obj, const Gmat::ObjectType type);
   virtual GmatBase*   GetObject(const Gmat::ObjectType type,
                                 const std::string objName = "");
   virtual void        ClearObject(const Gmat::ObjectType type);

   // Reference object accessor methods
   virtual GmatBase*   GetRefObject(const Gmat::ObjectType type,
                                    const std::string &name,
                                    const Integer index=0);
   virtual bool        SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                    const std::string &name,
                                    const Integer index=0);
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

   // Methods used to run the command
   virtual bool            InterpretAction();
    
   virtual bool            Initialize();
   virtual bool            Execute();

   Integer GetNumInputParams();
   Integer GetNumOutputParams();
   std::string FormEvalString();

protected:

private:
   ObjectArray objectArray;
   std::vector<Parameter*> mInputList;
   std::vector<Parameter*> mOutputList;

   StringArray mInputListNames;
   StringArray mOutputListNames;

   Function *mFunction;
   std::string mFunctionName;

   bool ExecuteMatlabFunction();

   enum
   {
      FUNCTION_NAME = GmatCommandParamCount,
      CallFunctionParamCount  /// Count of the parameters for this class
   };

   static const std::string PARAMETER_TEXT[
            CallFunctionParamCount - GmatCommandParamCount];
        static const Gmat::ParameterType PARAMETER_TYPE[
            CallFunctionParamCount - GmatCommandParamCount];

};

#endif // CallFunction_hpp
