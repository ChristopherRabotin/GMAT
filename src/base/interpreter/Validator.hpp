//$Id$
//------------------------------------------------------------------------------
//                                  Validator
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CCA54C
//
// Author: Linda Jun
// Created: 2008/04/01 (Made separate class out of Interpreter::CreateElementWrapper())
//
// Original Wrapper code from the Interpreter.
// Author: Wendy C. Shoan
//
/**
 * Definition for the Validator class.
 * This class validates objects command and builds ElemementWrappers for
 * various data types.
 */
//------------------------------------------------------------------------------
#ifndef Validator_hpp
#define Validator_hpp

#include "gmatdefs.hpp"

// Forward references
class Interpreter;
class Moderator;
class SolarSystem;
class GmatCommand;
class Function;
class Parameter;
class AxisSystem;
class ElementWrapper;

class GMAT_API Validator
{
public:
   
   static Validator* Instance();
   
   void SetInterpreter(Interpreter *interp);
   void SetSolarSystem(SolarSystem *ss);
   void SetObjectMap(ObjectMap *objMap);
   void SetFunction(Function *func);
   
   bool StartServer(GmatCommand *cmd);
   bool CheckUndefinedReference(GmatBase *obj, bool contOnError = true);
   bool ValidateCommand(GmatCommand *cmd, bool contOnError = true,
                        Integer manage = 1);
   
   ElementWrapper* CreateElementWrapper(const std::string &desc,
                                        bool parametersFirst = false,
                                        Integer manage = 1);
   const StringArray& GetErrorList();
   
   GmatBase* FindObject(const std::string &name, 
                        const std::string &ofType = "");
   Parameter* GetParameter(const std::string name);
   Parameter* CreateParameter(const std::string &type,
                              const std::string &name,
                              const std::string &ownerName = "",
                              const std::string &depName = "",
                              Integer manage = 1);
   Parameter* CreateArray( const std::string &arrayStr, Integer manage = 1);   
   Parameter* CreateSystemParameter(bool &paramCreated, const std::string &str,
                                    Integer manage = 1);
   AxisSystem* CreateAxisSystem(std::string type, GmatBase *owner);
   
   bool IsParameterType(const std::string &desc);
   bool ValidateParameter(const StringArray &refNames, GmatBase *obj);
   bool ValidateSaveCommand(GmatBase *obj);
   
private:

   bool CreateAssignmentWrappers(GmatCommand *cmd, Integer manage);
   
   ElementWrapper* CreateSolarSystemWrapper(GmatBase *obj, const std::string &owner,
                                            const std::string &depobj,
                                            const std::string &type,
                                            Integer manage = 1);
   ElementWrapper* CreateForceModelWrapper(GmatBase *obj, const std::string &owner,
                                           const std::string &depobj,
                                           const std::string &type,
                                           Integer manage = 1);
   ElementWrapper* CreateWrapperWithDot(bool parametersFirst, Integer manage = 1);
   ElementWrapper* CreateValidWrapperWithDot(GmatBase *obj,
                                             const std::string &type,
                                             const std::string &owner,
                                             const std::string &depobj,
                                             bool parametersFirst,
                                             Integer manage);
   ElementWrapper* CreateOtherWrapper(Integer manage = 1);
   ElementWrapper* CreateParameterWrapper(Parameter *param,
                                          Gmat::WrapperDataType &itsType);
   ElementWrapper* CreatePropertyWrapper(GmatBase *obj, const std::string &type,
                                         Integer manage, bool checkSubProp = true);
   ElementWrapper* CreateSubPropertyWrapper(GmatBase *obj,
                                            const std::string &type,
                                            Integer manage);
   
   bool ValidateSubCommand(GmatCommand *cmd, Integer level, Integer manage = 1);
   bool CreateCoordSystemProperty(GmatBase *obj, const std::string &prop,
                                  const std::string &value);
   bool CreatePropSetupProperty(GmatBase *obj, const std::string &prop,
                                const std::string &value);
   bool CreateForceModelProperty(GmatBase *obj, const std::string &prop,
                                 const std::string &value);
   bool HandleError(bool addFunction = true);
   
   Interpreter     *theInterpreter;
   Moderator       *theModerator;
   SolarSystem     *theSolarSystem;
   GmatCommand     *theCommand;
   Function        *theFunction;
   ObjectMap       *theObjectMap;
   StringArray     theParameterList;
   
   std::string     theDescription;
   
   /// Error handling data
   bool            continueOnError;
   StringArray     theErrorList;
   std::string     theErrorMsg;
   
   static Validator *instance;
   Validator();
   ~Validator();      
};

#endif // Validator_hpp
