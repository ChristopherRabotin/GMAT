//$Id$
//------------------------------------------------------------------------------
//                                   Function
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Allison Greene
// Created: 2004/09/22
//
/**
 * Defines the Funtion base class used for Matlab and Gmat functions.
 */
//------------------------------------------------------------------------------
#ifndef Function_hpp
#define Function_hpp

#include "GmatBase.hpp"
#include "FunctionException.hpp"
#include "ElementWrapper.hpp"
#include "PhysicalModel.hpp"
#include "GmatCommand.hpp"
#include "Validator.hpp"
#include <map>

/**
 * All function classes are derived from this base class.
 */
class Function : public GmatBase
{
public:
   Function(const std::string &typeStr, const std::string &nomme);
   virtual ~Function();
   Function(const Function &f);
   Function&            operator=(const Function &f);
   
   virtual WrapperTypeArray
                        GetOutputTypes(IntegerArray &rowCounts,
                                       IntegerArray &colCounts) const;
   virtual void         SetOutputTypes(WrapperTypeArray &outputTypes,
                                       IntegerArray &rowCounts,
                                       IntegerArray &colCounts);
   virtual bool         Initialize();
   virtual bool         Execute();
   virtual Real         Evaluate();
   virtual Rmatrix      MatrixEvaluate();
   virtual void         Finalize();
   virtual void         SetObjectMap(std::map<std::string, GmatBase *> *map);
   virtual void         SetGlobalObjectMap(std::map<std::string, GmatBase *> *map);
   virtual void         SetSolarSystem(SolarSystem *ss);
   virtual void         SetTransientForces(std::vector<PhysicalModel*> *tf);
   virtual bool         IsFunctionControlSequenceSet();
   virtual bool         SetFunctionControlSequence(GmatCommand *cmd);
   
   virtual bool         SetInputElementWrapper(const std::string &forName, ElementWrapper *wrapper);
   virtual ElementWrapper*
                        GetOutputArgument(Integer argNumber);
   virtual ElementWrapper*
                        GetOutputArgument(const std::string &byName);
   
   // methods to set/get the automatic objects
   virtual void         AddAutomaticObject(const std::string &withName, GmatBase *obj);
   virtual StringObjectMap  
                        GetAutomaticObjects() const;
   
   // Inherited (GmatBase) methods
   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");
   
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   
//   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                                     const std::string &name = "");

protected:
   /// Fully-qualified path for function script
   std::string functionPath;
   /// Function name
   std::string functionName;
   /// Function input names
   StringArray inputNames;
   /// Function output names
   StringArray outputNames;
   /// Function input name and element wrapper map  // @todo - is this needed?
   std::map<std::string, ElementWrapper*> inputArgMap;
   /// Function output name element wrapper map
   std::map<std::string, ElementWrapper*> outputArgMap;
   /// Output wrapper type array
   WrapperTypeArray outputWrapperTypes;
   /// Output row count used for returning one Array type
   IntegerArray outputRowCounts;
   /// Output column count used for returning one Array type
   IntegerArray outputColCounts;
   
   /// Object store for the Function 
   std::map<std::string, GmatBase *>
                        *objectStore;
   /// Object store obtained from the Sandbox
   std::map<std::string, GmatBase *>
                        *globalObjectStore;
   /// Solar System, set by the local Sandbox, to pass to the function
   SolarSystem          *solarSys;
   /// transient forces to pass to the function
   std::vector<PhysicalModel *> 
                        *forces;
   /// the function control sequence
   GmatCommand          *fcs;
   /// objects automatically created on parsing (but for whom a references object cannot be
   /// set at that time)
   std::map<std::string, GmatBase *>          
                        automaticObjects;
   // Validator used to create the ElementWrappers
   Validator            validator;
   /// Object store needed by the validator
   std::map<std::string, GmatBase *>
                        store;

   enum
   {
      FUNCTION_PATH = GmatBaseParamCount,
      FUNCTION_NAME,
      FUNCTION_INPUT,
      FUNCTION_OUTPUT,
      FunctionParamCount  /// Count of the parameters for this class
   };
   
   static const std::string
      PARAMETER_TEXT[FunctionParamCount - GmatBaseParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[FunctionParamCount - GmatBaseParamCount];
   
   GmatBase* FindObject(const std::string &name);
   
};


#endif // Function_hpp
