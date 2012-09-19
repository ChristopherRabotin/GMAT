//$Id$
//------------------------------------------------------------------------------
//                                   Function
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Allison Greene
// Created: 2004/09/22
//
/**
 * Defines the Function base class used for Matlab and Gmat functions.
 */
//------------------------------------------------------------------------------
#ifndef Function_hpp
#define Function_hpp

#include "GmatBase.hpp"
#include "FunctionException.hpp"
#include "ElementWrapper.hpp"
#include "PhysicalModel.hpp"
#include "GmatCommand.hpp"
#include "ObjectInitializer.hpp"
#include "Validator.hpp"
#include <map>

/**
 * All function classes are derived from this base class.
 */
class GMAT_API Function : public GmatBase
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
   virtual bool         IsNewFunction();
   virtual void         SetNewFunction(bool flag);
   virtual bool         Initialize();
   virtual bool         Execute(ObjectInitializer *objInit, bool reinitialize);
   virtual void         Finalize();
   virtual bool         IsFcsFinalized();
   virtual void         SetObjectMap(ObjectMap *objMap);
   virtual void         SetGlobalObjectMap(std::map<std::string, GmatBase *> *map);
   virtual void         SetSolarSystem(SolarSystem *ss);
   virtual void         SetInternalCoordSystem(CoordinateSystem *cs);
   virtual void         SetTransientForces(std::vector<PhysicalModel*> *tf);
   virtual void         SetScriptErrorFound(bool errFlag);
   virtual bool         ScriptErrorFound();
   virtual bool         IsFunctionControlSequenceSet();
   virtual bool         SetFunctionControlSequence(GmatCommand *cmd);
   virtual GmatCommand* GetFunctionControlSequence();
   virtual std::string  GetFunctionPathAndName();
   
   virtual bool         SetInputElementWrapper(const std::string &forName,
                                               ElementWrapper *wrapper);
   virtual ElementWrapper* GetOutputArgument(Integer argNumber);
   virtual ElementWrapper* GetOutputArgument(const std::string &byName);
   virtual WrapperArray&   GetWrappersToDelete();
   virtual void         ClearInOutArgMaps(bool deleteInputs, bool deleteOutputs);
   
   // methods to set/get the automatic objects
   virtual void         ClearAutomaticObjects();
   virtual void         AddAutomaticObject(const std::string &withName, GmatBase *obj,
                                           bool alreadyManaged);
   virtual GmatBase*    FindAutomaticObject(const std::string &name);
   virtual ObjectMap&   GetAutomaticObjects();
   
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
   
   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// Fully-qualified path for function script
   std::string          functionPath;
   /// Function name
   std::string          functionName;
   /// Function input names
   StringArray          inputNames;
   /// Function output names
   StringArray          outputNames;
   // @todo - should these next five items remain here or move to GmatFunction??
   /// Function input name and element wrapper map  // @todo - is this needed?
   WrapperMap           inputArgMap;
   /// Function output name element wrapper map
   WrapperMap           outputArgMap;
   /// Output wrapper type array
   WrapperTypeArray     outputWrapperTypes;
   /// Output row count used for returning one Array type
   IntegerArray         outputRowCounts;
   /// Output column count used for returning one Array type
   IntegerArray         outputColCounts;
   /// Old wrappers to delete
   WrapperArray         wrappersToDelete;
   /// Object array to delete
   ObjectArray          objectsToDelete;
   /// Objects already in the Sandbox object map
   ObjectArray          sandboxObjects;
   /// Object store for the Function 
   ObjectMap            *objectStore;
   /// Object store obtained from the Sandbox
   ObjectMap            *globalObjectStore;
   /// Solar System, set by the local Sandbox, to pass to the function
   SolarSystem          *solarSys;
   /// Internal CS, set by the local Sandbox, to pass to the function
   CoordinateSystem     *internalCoordSys;
   /// transient forces to pass to the function
   std::vector<PhysicalModel *> 
                        *forces;
   // @todo - should these next four items remain here or move to GmatFunction??
   /// the function control sequence
   GmatCommand          *fcs;
   /// have the commands in the FCS been finalized?
   bool                 fcsFinalized;
   /// objects automatically created on parsing (but for whom a references object cannot be
   /// set at that time)
   ObjectMap            automaticObjectMap;
   // Validator used to create the ElementWrappers
   Validator            *validator;
   /// Object store needed by the validator
   ObjectMap            validatorStore;
   /// the flag indicating script error found in function, this flag is set by Interpreter
   bool                 scriptErrorFound;
   /// the flag indicating local objects are initialized
   bool                 objectsInitialized;
   
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
   
   // for debug
   void ShowObjectMap(ObjectMap *objMap, const std::string &title = "",
                      const std::string &mapName = "");
   void ShowObjects(const std::string &title);
};


#endif // Function_hpp
