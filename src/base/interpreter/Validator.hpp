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
   Validator(SolarSystem *ss = NULL, ObjectMap *objMap = NULL);
   ~Validator();
   
   void SetSolarSystem(SolarSystem *ss);
   void SetObjectMap(ObjectMap *objMap);
   void SetFunction(Function *func);
   
   bool CheckUndefinedReference(GmatBase *obj, bool contOnError = true);
   bool ValidateCommand(GmatCommand *cmd, bool contOnError = true,
                        bool manage = true);
   
   ElementWrapper* CreateElementWrapper(const std::string &desc,
                                        bool parametersFirst = false,
                                        bool manage = true);
   const StringArray& GetErrorList();
   
   GmatBase* FindObject(const std::string &name, 
                        const std::string &ofType = "");
   Parameter* GetParameter(const std::string name);
   Parameter* CreateParameter(const std::string &type,
                              const std::string &name,
                              const std::string &ownerName = "",
                              const std::string &depName = "",
                              bool manage = true);
   Parameter* CreateArray( const std::string &arrayStr, bool manage = true);   
   Parameter* CreateSystemParameter(const std::string &str,
                                    bool manage = true);
   AxisSystem* CreateAxisSystem(std::string type, GmatBase *owner);
   
   void CreateParameterWrapper(Parameter *param, ElementWrapper **ew,
                               Gmat::WrapperDataType &itsType);
   ElementWrapper* CreateWrapperWithDot(const std::string &descTrimmed,
                                        bool manage = true);
   ElementWrapper* CreateOtherWrapper(const std::string &descTrimmed,
                                      bool manage = true);
   
   bool IsParameterType(const std::string &desc);
   bool ValidateParameter(const StringArray &refNames, GmatBase *obj);
   bool ValidateSaveCommand(GmatBase *obj);
   
protected:
   
   bool HandleError();
   
   Moderator       *theModerator;
   SolarSystem     *theSolarSystem;
   Function        *theFunction;
   ObjectMap       *theObjectMap;
   StringArray     theParameterList;
   
   /// Error handling data
   bool            continueOnError;
   StringArray     theErrorList;
   std::string     theErrorMsg;
};

#endif // Validator_hpp
