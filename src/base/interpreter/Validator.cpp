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
 * Implementation for the Validator class.
 * This class validates objects and commands and builds ElemementWrappers for
 * various data types.
 */
//------------------------------------------------------------------------------

#include "Validator.hpp"
#include "Moderator.hpp"
#include "InterpreterException.hpp"
#include "NumberWrapper.hpp"
#include "ParameterWrapper.hpp"
#include "VariableWrapper.hpp"
#include "ObjectPropertyWrapper.hpp"
#include "ArrayWrapper.hpp"
#include "ArrayElementWrapper.hpp"
#include "StringObjectWrapper.hpp"
#include "BooleanWrapper.hpp"
#include "StringWrapper.hpp"
#include "OnOffWrapper.hpp"
#include "ObjectWrapper.hpp"
#include "StringUtil.hpp"               // for GmatStringUtil::
#include "Assignment.hpp"


//#define DEBUG_VALIDATE_COMMAND
//#define DEBUG_WRAPPERS
//#define DEBUG_CHECK_OBJECT
//#define DEBUG_HANDLE_ERROR
//#define DEBUG_CREATE_PARAM

//------------------------------------------------------------------------------
// Validator(SolarSystem *ss = NULL, StringObjectMap *objMap = NULL)
//------------------------------------------------------------------------------
/*
 * Default constructor
 *
 * @param  ss  The solar system to be used for findnig bodies
 * @param  objMap  The object map to be used for finding object
 */
//------------------------------------------------------------------------------
Validator::Validator(SolarSystem *ss, StringObjectMap *objMap)
{
   theModerator = Moderator::Instance();
   StringArray parms = theModerator->GetListOfFactoryItems(Gmat::PARAMETER);
   copy(parms.begin(), parms.end(), back_inserter(theParameterList));
   
   theSolarSystem = NULL;
   theObjectMap = NULL;
   continueOnError = true;
   
   if (ss)
      theSolarSystem = ss;
   
   if (objMap)
      theObjectMap = objMap;
}


//------------------------------------------------------------------------------
// ~Validator()
//------------------------------------------------------------------------------
Validator::~Validator()
{
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
void Validator::SetSolarSystem(SolarSystem *ss)
{
   theSolarSystem = ss;
}


//------------------------------------------------------------------------------
// void SetObjectMap(StringObjectMap *objMap)
//------------------------------------------------------------------------------
void Validator::SetObjectMap(StringObjectMap *objMap)
{
   theObjectMap = objMap;
}


//------------------------------------------------------------------------------
// bool CheckUndefinedReference(GmatBase *obj, bool contOnError)
//------------------------------------------------------------------------------
/*
 * This method checks if reference objects of given object exist. First it
 * gets reference object type list by calling obj->GetRefObjectTypeArray() and
 * then gets reference object name list by calling obj->GetRefObjectNameArray(reftype).
 * If reference object type is Parameter, it checks if owner object of that
 * paramter exist; otherwise, it only check for the reference object name exist.
 *
 * @param  obj  input object of undefined reference object to be checked
 * @param <contOnError> flag indicating whether or not to continue on error (true)
 */
//------------------------------------------------------------------------------
bool Validator::CheckUndefinedReference(GmatBase *obj, bool contOnError)
{
   continueOnError = contOnError;
   
   #ifdef DEBUG_CHECK_OBJECT
   MessageInterface::ShowMessage
      ("Validator::CheckUndefinedReference() obj=<%p><%s>, continueOnError\n", obj,
       obj->GetName().c_str(), continueOnError);
   #endif
   
   theErrorList.clear();
   bool retval = true;
   ObjectTypeArray refTypes = obj->GetRefObjectTypeArray();
   StringArray refNames;
   
   #ifdef DEBUG_CHECK_OBJECT
   MessageInterface::ShowMessage
      ("Validator::CheckUndefinedReference() type='%s', name='%s', refTypes.size()=%d\n",
       obj->GetTypeName().c_str(), obj->GetName().c_str(), refTypes.size());
   #endif
   
   // Save command can have any object type, so handle it first
   if (obj->GetTypeName() == "Save")
      return ValidateSaveCommand(obj);
   
   // If Axis is NULL, create default MJ2000Eq
   if (obj->GetType() == Gmat::COORDINATE_SYSTEM)
   {
      if (obj->GetRefObject(Gmat::AXIS_SYSTEM, "") == NULL)
      {
         CreateAxisSystem("MJ2000Eq", obj);
         theErrorMsg = "The CoordinateSystem \"" + obj->GetName() +
            "\" has empty AxisSystem, so default MJ2000Eq was created";
         retval = retval && HandleError();
      }
   }
   
   // Check if undefined ref. objects exist
   for (UnsignedInt i=0; i<refTypes.size(); i++)
   {
      #ifdef DEBUG_CHECK_OBJECT
      MessageInterface::ShowMessage
         ("   %s\n", GmatBase::GetObjectTypeString(refTypes[i]).c_str());
      #endif
      
      try
      {
         refNames = obj->GetRefObjectNameArray(refTypes[i]);
         
         #ifdef DEBUG_CHECK_OBJECT
         MessageInterface::ShowMessage("      Objects names are:\n");
         for (UnsignedInt j=0; j<refNames.size(); j++)
            MessageInterface::ShowMessage
               ("      %s\n", refNames[j].c_str());
         #endif
         
         // Check System Parameters seperately since it follows certain naming
         // convention.  "owner.dep.type" where owner can be either Spacecraft
         // or Burn for now
         
         if (refTypes[i] == Gmat::PARAMETER)
         {
            retval = retval && ValidateParameter(refNames, obj);
         }
         else
         {
            for (UnsignedInt j=0; j<refNames.size(); j++)
            {
               std::string objName = obj->GetTypeName();
               if (obj->GetType() != Gmat::COMMAND)
                  objName = objName + " \"" + obj->GetName() + "\"";
               
               GmatBase *refObj = FindObject(refNames[j]);
               
               if (refObj == NULL)
               {
                  theErrorMsg = "Nonexistent " + GmatBase::GetObjectTypeString(refTypes[i]) +
                     " \"" + refNames[j] + "\" referenced in the " + objName;
                  retval = retval && HandleError();
               }
               else if (!refObj->IsOfType(refTypes[i]))
               {
                  #ifdef DEBUG_CHECK_OBJECT
                  MessageInterface::ShowMessage
                     ("refObj->IsOfType(refTypes[i])=%d, %s\n", refTypes[i],
                      GmatBase::GetObjectTypeString(refTypes[i]).c_str());
                  #endif
                  
                  theErrorMsg = "\"" + refNames[j] + "\" referenced in the " + objName +
                     " is not an object of " + GmatBase::GetObjectTypeString(refTypes[i]);
                  retval = retval && HandleError();
               }
            }
         }
      }
      catch (BaseException &e)
      {
         #ifdef DEBUG_CHECK_OBJECT
         // since command handles setting reference object during initialization,
         // skip if object is command
         if (obj->GetType() != Gmat::COMMAND)
            MessageInterface::ShowMessage(e.GetFullMessage());
         #endif
      }
   }
   
   #ifdef DEBUG_CHECK_OBJECT
   MessageInterface::ShowMessage("CheckUndefinedReference() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool ValidateCommand(GmatCommand *cmd, bool contOnError = true)
//------------------------------------------------------------------------------
/**
 * Checks the input command to make sure it wrappers are set up for it
 * correctly, if necessary.
 *
 * @param <cmd> the command to validate
 * @param <contOnError> flag indicating whether or not to continue on error
 */
//------------------------------------------------------------------------------
bool Validator::ValidateCommand(GmatCommand *cmd, bool contOnError)
{
   continueOnError = contOnError;
   
   #ifdef DEBUG_VALIDATE_COMMAND
   MessageInterface::ShowMessage
      ("Validator::ValidateCommand() cmd=<%p><%s>, continueOnError\n", cmd,
       cmd->GetTypeName().c_str(), continueOnError);
   #endif
   
   #ifdef DEBUG_VALIDATE_COMMAND
   MessageInterface::ShowMessage
      ("   Calling %s->GetWrapperObjectNameArray() to get object names\n",
       cmd->GetTypeName().c_str());
   #endif
   
   theErrorList.clear();
   cmd->ClearWrappers();
   const StringArray wrapperNames = cmd->GetWrapperObjectNameArray();
   #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage("In Validate, wrapper names are:\n");
      for (Integer ii=0; ii < (Integer) wrapperNames.size(); ii++)
         MessageInterface::ShowMessage("      %s\n", wrapperNames[ii].c_str());
   #endif
   
   //---------------------------------------------------------------------------
   // Special case for Assignment command (LHS = RHS).
   // Since such as Sat.X can be both Parameter or ObjectProperty, we want to
   // create a Parameter wapper if RHS is a Parameter for Assignment command.
   // So special code is needed to tell the CreateElementWrapper() to check for
   // Parameter first.
   //---------------------------------------------------------------------------
   if (cmd->GetTypeName() == "GMAT")
   {
      Assignment *acmd = (Assignment*)cmd;
      
      // Handle LHS
      std::string lhs = acmd->GetLHS();
      
      try
      {
         ElementWrapper *ew = CreateElementWrapper(lhs);
         
         if (ew == NULL)
            return false;
         
         if (cmd->SetElementWrapper(ew, lhs) == false)
         {
            theErrorMsg = "Undefined object \"" + lhs + "\" found in command \"" +
               cmd->GetTypeName();
            return HandleError();
         }
      }
      catch (BaseException &ex)
      {
         theErrorMsg = ex.GetFullMessage();
         return HandleError();
      }
      
      // Handle RHS
      // Note: Assignment::GetWrapperObjectNameArray() returns only for RHS elements
      for (StringArray::const_iterator i = wrapperNames.begin();
           i != wrapperNames.end(); ++i)
      {
         std::string name = (*i);
         if (name != "")
         {
            try
            {
               ElementWrapper *ew = NULL;
               if (IsParameterType(name))
                  ew = CreateElementWrapper(name, true);
               else
                  ew = CreateElementWrapper(name);
               
               if (cmd->SetElementWrapper(ew, name) == false)
               {
                  theErrorMsg = "Undefined object \"" + name + "\" found in Math Assignment";
                  return HandleError();
               }
            }
            catch (BaseException &ex)
            {
               theErrorMsg = ex.GetFullMessage();
               return HandleError();
            }
         }
      }
      
      // Set math wrappers to math tree
      acmd->SetMathWrappers();
   }
   else
   {
      for (StringArray::const_iterator i = wrapperNames.begin();
           i != wrapperNames.end(); ++i)
      {
         try
         {
            ElementWrapper *ew = CreateElementWrapper(*i);
            
            if (cmd->SetElementWrapper(ew, *i) == false)
            {
               theErrorMsg = "Undefined object \"" + (*i) + "\" found in command \"" +
                  cmd->GetTypeName();
               return HandleError();
            }
         }
         catch (BaseException &ex)
         {
            theErrorMsg = ex.GetFullMessage();
            return HandleError();
         }
      }
   }
   
   #ifdef DEBUG_VALIDATE_COMMAND
   MessageInterface::ShowMessage
      ("Validator::ValidateCommand() Calling CheckUndefinedReference(%s)\n",
       cmd->GetTypeName().c_str());
   #endif
   
   return CheckUndefinedReference(cmd);
   
}


//------------------------------------------------------------------------------
// ElementWrapper* CreateElementWrapper(const std::string &desc,
//                                      bool parametersFirst)
//------------------------------------------------------------------------------
/**
 * Creates the appropriate ElementWrapper, based on the description.
 *
 * @param  <desc>  description string for the element required
 * @param  <parametersFirst>  true if creating for wrappers for the Parameter
 *                            first then Object Property (false)
 *
 * @return pointer to the created ElementWrapper
 * @exception <Interpreterxception> thrown if wrapper cannot be created
 */
//------------------------------------------------------------------------------
ElementWrapper*
Validator::CreateElementWrapper(const std::string &desc, bool parametersFirst)
{
   Gmat::WrapperDataType itsType = Gmat::NUMBER;
   ElementWrapper *ew = NULL;
   Real           rval;
   // remove extra parens and blank spaces at either end of string
   std::string    descTrimmed = GmatStringUtil::Trim(desc);
   descTrimmed = GmatStringUtil::RemoveExtraParen(descTrimmed);
   descTrimmed = GmatStringUtil::Trim(descTrimmed);  
   
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("In Validator::CreateElementWrapper, original string is \"%s\"\n",
       desc.c_str(), "\"\n");
   MessageInterface::ShowMessage
      ("                                   and trimmed string is \"%s\"\n",
       descTrimmed.c_str(), "\"\n");
   #endif
   
   // first, check to see if it is enclosed with single quotes
   if (GmatStringUtil::IsEnclosedWith(descTrimmed, "'"))
   {
      ew = new StringWrapper();
      itsType = Gmat::STRING;
      
      #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
         ("In Validator, created a StringWrapper for \"%s\"\n",
          descTrimmed.c_str(), "\"\n");
      #endif
   }
   // and then, check to see if it is a number
   else if (GmatStringUtil::ToReal(descTrimmed,rval))
   {
      ew = new NumberWrapper();
      #ifdef DEBUG_WRAPPERS
         MessageInterface::ShowMessage(
            "In Validator, created a NumberWrapper for \"%s\"\n",
            descTrimmed.c_str(), "\"\n");
      #endif
   }
   else 
   {
      Parameter *p;
      // check to see if it is an array
      bool isOuterParen;
      Integer openParen, closeParen;
      GmatStringUtil::FindParenMatch(descTrimmed, openParen, closeParen, isOuterParen);
      #ifdef DEBUG_WRAPPERS
         MessageInterface::ShowMessage(
            "In Validator, descTrimmed = \"%s\"\n", descTrimmed.c_str());
      #endif
      if ((openParen  != -1) && (closeParen != -1) && (closeParen > openParen))
      {
         std::string arrayName = GmatStringUtil::Trim(descTrimmed.substr(0,openParen));
         #ifdef DEBUG_WRAPPERS
            MessageInterface::ShowMessage(
               "In Validator, arrayName = \"%s\"\n", arrayName.c_str());
         #endif
         if (!GmatStringUtil::IsBlank(arrayName))
         {
            if ((p = GetParameter(arrayName)) == NULL)
            {
               theErrorMsg = "The Array \"" + arrayName + "\"" + " does not exist";
               HandleError();
            }
            else if (!(p->IsOfType("Array")) )
            {
               theErrorMsg = "\"" + arrayName + "\"" + " is not an Array";
               HandleError();
            }
            else
            {
               ew = new ArrayElementWrapper();
               itsType = Gmat::ARRAY_ELEMENT;
               #ifdef DEBUG_WRAPPERS
               MessageInterface::ShowMessage(
                  "In Validator, created a ArrayElementWrapper for \"%s\"\n",
                  descTrimmed.c_str(), "\"\n");
               #endif
            }
         }
      }
      // check to see if it is an object property or a parameter
      else if (descTrimmed.find(".") != std::string::npos)
      {
         // if parametersFirst, always create parameter wrapper first
         if (parametersFirst)
         {
            Parameter *param = CreateSystemParameter(descTrimmed);
            if (param)
            {
               #ifdef DEBUG_WRAPPERS
                  MessageInterface::ShowMessage(
                     "In Interpreter(1), about to create a ParameterWrapper for \"%s\"\n",
                     descTrimmed.c_str(), "\"\n");
               #endif
               CreateParameterWrapper(param, &ew, itsType);
            }
            else
            {
               theErrorMsg = "\"" + descTrimmed + "\"" + " is not a valid parameter";
               HandleError();
            }
         }
         else
         {
            ew = CreateWrapperWithDot(descTrimmed);
         }
      }
      else // check to see if it is a Variable or some other parameter
      {
         ew = CreateOtherWrapper(descTrimmed);
      }
   }
   
   if (ew)
   {
      // set the description string; this will also setup the wrapper
      ew->SetDescription(desc);
      //ew->SetDescription(descTrimmed); // or do I want this?
      // if it's an ArrayElement, set up the row and column wrappers
      if (itsType == Gmat::ARRAY_ELEMENT)
      {
         std::string    rowName = ((ArrayElementWrapper*)ew)->GetRowName();
         ElementWrapper *row    = CreateElementWrapper(rowName);
         ((ArrayElementWrapper*)ew)->SetRow(row);
         std::string    colName = ((ArrayElementWrapper*)ew)->GetColumnName();
         ElementWrapper *col    = CreateElementWrapper(colName);
         ((ArrayElementWrapper*)ew)->SetColumn(col);
      }
   }
   return ew;
}


//------------------------------------------------------------------------------
// const StringArray& GetErrorList()
//------------------------------------------------------------------------------
const StringArray& Validator::GetErrorList()
{
   return theErrorList;
}


//------------------------------------------------------------------------------
// ElementWrapper* CreateWrapperWithDot(const std::string &descTrimmed)
//------------------------------------------------------------------------------
ElementWrapper* Validator::CreateWrapperWithDot(const std::string &descTrimmed)
{
   // try to parse the string for an owner and type
   // NOTE - need to check handling of "special cases, e.g.Sat1.X" <<<<<<<<<<
   // check for object parameter first
   Gmat::WrapperDataType itsType = Gmat::NUMBER;
   ElementWrapper *ew = NULL;
   std::string type, owner, dep;
   GmatStringUtil::ParseParameter(descTrimmed, type, owner, dep);
   // if it's not a valid object, then see if it's a parameter
   GmatBase *theObj = FindObject(owner);
   if (theObj == NULL)
   {
      Parameter *param = CreateSystemParameter(descTrimmed);
      
      if (param)
      {
         #ifdef DEBUG_WRAPPERS
         MessageInterface::ShowMessage
            ("In Interpreter(2), about to create a ParameterWrapper for \"%s\"\n",
             descTrimmed.c_str(), "\"\n");
         #endif
         
         CreateParameterWrapper(param, &ew, itsType);
      }
      else // there is an error
      {
         ew = new StringWrapper();
         itsType = Gmat::STRING;
                  
         #ifdef DEBUG_WRAPPERS
         MessageInterface::ShowMessage
            ("In Validator, created a StringWrapper for \"%s\"\n",
             descTrimmed.c_str(), "\"\n");
         #endif
      }
   }
   else
   {
      // if there are two dots, then treat it as a parameter
      if (descTrimmed.find_first_of(".") != descTrimmed.find_last_of("."))
      {
         Parameter *param = CreateSystemParameter(descTrimmed);
         if (param)
         {
            #ifdef DEBUG_WRAPPERS
            MessageInterface::ShowMessage
               ("In Interpreter(3), about to create a ParameterWrapper for \"%s\"\n",
                descTrimmed.c_str(), "\"\n");
            #endif
            
            CreateParameterWrapper(param, &ew, itsType);
         }
         else // there is an error
         {
            theErrorMsg = "\"" + descTrimmed + "\"" + 
               " is not a valid object property or parameter";
            HandleError();
         }
      }
      else
      {
         // if there is no such field for that object, it is a parameter
         bool isValidProperty = true;
         try
         {
            theObj->GetParameterID(type);
         }
         catch (BaseException &be)
         {
            isValidProperty = false;
            Parameter *param = CreateSystemParameter(descTrimmed);
            if (param)
            {
               #ifdef DEBUG_WRAPPERS
               MessageInterface::ShowMessage
                  ("In Interpreter(4), about to create a ParameterWrapper for \"%s\"\n",
                   descTrimmed.c_str(), "\"\n");
               #endif
               
               CreateParameterWrapper(param, &ew, itsType);
            }
            else // there is an error
            {
               theErrorMsg = "\"" + descTrimmed + "\"" + 
                  " is not a valid object property or parameter";
               HandleError();
            }
         }
         if (isValidProperty)
         {
            ew = new ObjectPropertyWrapper();
            itsType = Gmat::OBJECT_PROPERTY;
            
            #ifdef DEBUG_WRAPPERS
            MessageInterface::ShowMessage
               ("In Validator, created a ObjectPropertyWrapper for \"%s\"\n",
                descTrimmed.c_str(), "\"\n");
            #endif
         }
      }
   }
   
   return ew;
}


//------------------------------------------------------------------------------
// ElementWrapper* CreateOtherWrapper(const std::string &descTrimmed)
//------------------------------------------------------------------------------
ElementWrapper* Validator::CreateOtherWrapper(const std::string &descTrimmed)
{
   Gmat::WrapperDataType itsType = Gmat::NUMBER;
   ElementWrapper *ew = NULL;
   Parameter *p = GetParameter(descTrimmed);
   
   if ( (p) && (p->IsOfType("Variable")) )
   {
      ew = new VariableWrapper();
      itsType = Gmat::VARIABLE;
      
      #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
         ("In Validator, created a VariableWrapper for \"%s\"\n",
          descTrimmed.c_str(), "\"\n");
      #endif
   }
   else if ( (p) && p->IsOfType("Array") )
   {
      ew = new ArrayWrapper();
      itsType = Gmat::ARRAY;
      
      #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
         ("In Validator, created an ArrayWrapper for \"%s\"\n",
          descTrimmed.c_str(), "\"\n");
      #endif
   }
   else if ( (p) && p->IsOfType("String") )
   {
      ew = new StringObjectWrapper();
      itsType = Gmat::STRING_OBJECT;
      
      #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
         ("In Validator, created a StringObjectWrapper for \"%s\"\n",
          descTrimmed.c_str(), "\"\n");
      #endif
   }
   else if ( (p) && p->IsOfType("Parameter") )
   {
      #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
         ("In Interpreter(5), about to create a ParameterWrapper for \"%s\"\n",
          descTrimmed.c_str(), "\"\n");
      #endif
      
      CreateParameterWrapper(p, &ew, itsType);
   }
   else
   {
      // Check see if it is clonable object
      GmatBase *obj = FindObject(descTrimmed);
      
      if (obj != NULL)
      {
         ew = new ObjectWrapper();
         itsType = Gmat::OBJECT;
         
         #ifdef DEBUG_WRAPPERS
         MessageInterface::ShowMessage
            ("In Validator, created an ObjectWrapper for \"%s\"\n",
             descTrimmed.c_str(), "\"\n");
         #endif
      }
      else
      {
         bool bVal;
         if (GmatStringUtil::ToBoolean(descTrimmed, bVal))
         {
            ew = new BooleanWrapper();
            itsType = Gmat::BOOLEAN;
            
            #ifdef DEBUG_WRAPPERS
            MessageInterface::ShowMessage
               ("In Validator, created a BooleanWrapper for \"%s\"\n",
                descTrimmed.c_str(), "\"\n");
            #endif
         }
         else if (descTrimmed == "On" || descTrimmed == "Off" ||
                  descTrimmed == "on" || descTrimmed == "off")
         {
            ew = new OnOffWrapper();
            itsType = Gmat::ON_OFF;
            
            #ifdef DEBUG_WRAPPERS
            MessageInterface::ShowMessage
               ("In Validator, created a OnOffWrapper for \"%s\"\n",
                descTrimmed.c_str(), "\"\n");
            #endif
         }
         else
         {
            ew = new StringWrapper();
            itsType = Gmat::STRING;
            
            #ifdef DEBUG_WRAPPERS
            MessageInterface::ShowMessage
               ("In Validator, created a StringWrapper for \"%s\"\n",
                descTrimmed.c_str(), "\"\n");
            #endif
         }
      }
   }
   
   return ew;
}


//------------------------------------------------------------------------------
// GmatBase* FindObject(const std::string &name, const std::string &ofType = "")
//------------------------------------------------------------------------------
/**
 * Finds the object from the current object map.
 * (old method: Calls the Moderator to find a configured object.)
 *
 * @param  name    Name of the object.
 * @param  ofType  Type of object required; leave blank for no checking
 *
 * @return  object pointer found
 */
//------------------------------------------------------------------------------
GmatBase* Validator::FindObject(const std::string &name, 
                                            const std::string &ofType)
{
   #ifdef DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage
      ("Validator::FindObject() entered: name=%s\n", name.c_str());
   #endif
   
   if (theObjectMap == NULL)
      throw InterpreterException("The Object Map is not set in the Validator.\n");
   
   if (theSolarSystem == NULL)
      throw InterpreterException("The Solar System is not set in the Validator.\n");
   
   if (name == "")
      return NULL;
   
   if (name == "SolarSystem")
      return theSolarSystem;
   
   GmatBase *obj = NULL;
   std::string newName = name;
   
   // Ignore array indexing of Array
   std::string::size_type index = name.find_first_of("([");
   if (index != name.npos)
   {
      newName = name.substr(0, index);
      
      #ifdef DEBUG_FIND_OBJECT
      MessageInterface::ShowMessage
         ("Validator::FindObject() entered: newName=%s\n", newName.c_str());
      #endif
   }
   
   // Find object from the object map
   if (theObjectMap->find(newName) != theObjectMap->end())
      if ((*theObjectMap)[newName]->GetName() == newName)
         obj = (*theObjectMap)[newName];
   
   // try SolarSystem if obj is still NULL
   if (obj == NULL)
      obj = (GmatBase*)(theSolarSystem->GetBody(newName));
   
   // check for the requested type
   if ((obj != NULL) && (ofType != "") && (!obj->IsOfType(ofType)))
      obj = NULL;
   
   #ifdef DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage
      ("Validator::FindObject() returning <%p><%s><%s>\n", obj,
       (obj == NULL) ? "NULL" : obj->GetTypeName().c_str(),
       (obj == NULL) ? "NULL" : obj->GetName().c_str());
   #endif
   
   return obj;
}


//------------------------------------------------------------------------------
// Parameter* GetParameter(const std::string name)
//------------------------------------------------------------------------------
Parameter* Validator::GetParameter(const std::string name)
{
   GmatBase *obj = FindObject(name);
   if (obj && obj->GetType() == Gmat::PARAMETER)
      return (Parameter*)obj;
   
   return NULL;
}


//------------------------------------------------------------------------------
// Parameter* CreateSystemParameter(const std::string &name, bool manage = true)
//------------------------------------------------------------------------------
/**
 * Creates a system Parameter from the input parameter name. If the name contains
 * dots, it consider it as a system parameter.  If it is not a system Parameter
 * it checks if object by given name is a Parameter.
 *
 * @param  name   parameter name to be parsed for Parameter creation
 *                Such as, sat1.Earth.ECC, sat1.SMA
 * @param  manage true if parameter to be managed in the configuration (true)
 *
 * @return Created Paramteter pointer or pointer of the Parameter by given name
 *         NULL if it is not a system Parameter nor named object is not a Parameter
 *
 */
//------------------------------------------------------------------------------
Parameter* Validator::CreateSystemParameter(const std::string &str,
                                              bool manage)
{
   #ifdef DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("Validator::CreateSystemParameter() str=%s\n", str.c_str());
   #endif
   
   Parameter *param = NULL;
   std::string paramType, ownerName, depName;
   GmatStringUtil::ParseParameter(str, paramType, ownerName, depName);
   
   #ifdef DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("   paramType=%s, ownerName=%s, depName=%s\n", paramType.c_str(),
       ownerName.c_str(), depName.c_str());
   #endif
   
   // Create parameter, if type is a System Parameter
   if (find(theParameterList.begin(), theParameterList.end(), paramType) != 
       theParameterList.end())
   {
      param = CreateParameter(paramType, str, ownerName, depName, manage);
      
      #ifdef DEBUG_CREATE_PARAM
      MessageInterface::ShowMessage
         ("   Parameter created with paramType=%s, ownerName=%s, depName=%s\n",
          paramType.c_str(), ownerName.c_str(), depName.c_str());
      #endif
   }
   else
   {
      // Find the object and check if it is a Parameter
      GmatBase *obj = FindObject(str);
      if (obj != NULL && obj->GetType() == Gmat::PARAMETER)
      {
         param = (Parameter*)obj;
      
         #ifdef DEBUG_CREATE_PARAM
         MessageInterface::ShowMessage("   Parameter <%s> found\n", str.c_str());
         #endif
      }
   }
   
   #ifdef DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("Validator::CreateSystemParameter() returning <%p><%s><%s>\n", param,
       (param == NULL) ? "NULL" : param->GetTypeName().c_str(),
       (param == NULL) ? "NULL" : param->GetName().c_str());
   #endif
   
   return param;
}


//------------------------------------------------------------------------------
// Parameter* CreateParameter(const std::string &type, const std::string &name,
//                            const std::string &ownerName, const std::string &depName
//                            bool manage = true)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to create a Parameter.
 * 
 * @param  type       Type of parameter requested
 * @param  name       Name for the parameter.
 * @param  ownerName  object name of parameter requested ("")
 * @param  depName    Dependent object name of parameter requested ("")
 * @param  manage     true if created object to be added to configuration (true)
 * 
 * @return Pointer to the constructed Parameter.
 */
//------------------------------------------------------------------------------
Parameter* Validator::CreateParameter(const std::string &type, 
                                        const std::string &name,
                                        const std::string &ownerName,
                                        const std::string &depName,
                                        bool manage)
{
   #ifdef DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("Validator::CreateParameter() type='%s', name='%s', ownerName='%s', "
       "depName='%s', manage=%d\n", type.c_str(), name.c_str(),
       ownerName.c_str(), depName.c_str(), manage);
   #endif
   
   // Check if create an array
   if (type == "Array")
      return CreateArray(name, manage);
   else
      return theModerator->CreateParameter(type, name, ownerName, depName, manage);
}


//------------------------------------------------------------------------------
// Parameter* CreateArray(onst std::string &arrayStr, bool manage = true)
//------------------------------------------------------------------------------
Parameter* Validator::CreateArray(const std::string &arrayStr, bool manage)
{
   std::string name;
   Integer row, col;
   GmatStringUtil::GetArrayIndex(arrayStr, row, col, name, "[]");
   bool isOk = true;
   
   if (row == -1)
   {
      theErrorMsg = "Validator::CreateArray() invalid number of rows found in: " +
         arrayStr + "\n";
      HandleError();
      isOk = false;
   }
   
   if (col == -1)
   {
      theErrorMsg = "Validator::CreateArray() invalid number of columns found in: " +
         arrayStr + "\n";
      HandleError();
      isOk = false;
   }
   
   Parameter *param = NULL;
   
   if (isOk)
   {
      param = theModerator->CreateParameter("Array", name, "", "", manage);
      
      #ifdef DEBUG_CREATE_ARRAY
      MessageInterface::ShowMessage
         ("Validator::CreateArray() row=%d, col=%d\n", row, col);
      #endif
      
      ((Array*)param)->SetSize(row, col);
   }
   
   return param;
}


//------------------------------------------------------------------------------
// AxisSystem* CreateAxisSystem(std::string type, GmatBase *owner)
//------------------------------------------------------------------------------
AxisSystem* Validator::CreateAxisSystem(std::string type, GmatBase *owner)
{
   if (owner == NULL)
   {
      theErrorMsg = "Validator::CreateAxisSystem needs a "
         "CoordinateSystem object that acts as its owner; received a NULL "
         "pointer instead.";
      HandleError();
      return NULL;
   }
   if (owner->GetType() != Gmat::COORDINATE_SYSTEM)
   {
      theErrorMsg = "Validator::CreateAxisSystem needs a "
         "CoordinateSystem object that acts as its owner; received a pointer "
         "to " + owner->GetName() + "instead.";
      HandleError();
      return NULL;
   }
   
   AxisSystem* axes = (AxisSystem *)(theModerator->CreateAxisSystem(type, ""));
   owner->SetRefObject(axes, axes->GetType(), axes->GetName());
   return axes;
}


//------------------------------------------------------------------------------
// void CreateParameterWrapper(Parameter *param, ElementWrapper **ew,
//                             Gmat::WrapperDataType &itsType)
//------------------------------------------------------------------------------
/*
 * Creates ParameterWrapper.
 *
 * @param <param> Pointer to Paramter
 * @param <ew> Adress of outpout ElementWrapper pointer
 * @param <itsType> outpout wrapper type
 */
//------------------------------------------------------------------------------
void Validator::CreateParameterWrapper(Parameter *param, ElementWrapper **ew,
                                            Gmat::WrapperDataType &itsType)
{
   *ew = NULL;
   
   if (param->IsOfType("String"))
   {
      *ew = new StringObjectWrapper();
      itsType = Gmat::STRING_OBJECT;
      
      #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
         ("In Validator, created a StringObjectWrapper for \"%s\"\n",
          param->GetName().c_str(), "\"\n");
      #endif
   }
   else
   {
      *ew = new ParameterWrapper();
      itsType = Gmat::PARAMETER_OBJECT;
      
      #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
         ("In Validator, created a ParameterWrapper for \"%s\"\n",
          param->GetName().c_str(), "\"\n");
      #endif
   }
}


//------------------------------------------------------------------------------
// bool IsParameterType(const std::string &desc)
//------------------------------------------------------------------------------
/*
 * Checks if input description is a Parameter.
 * If desctiption has dots, it will parse the components into Object, Depdency,
 * and Type. If type is one of the system parameters, it will return true.
 *
 * @param  desc  Input string to check for Parameter type
 * @return  true  if type is a Parameter type
 */
//------------------------------------------------------------------------------
bool Validator::IsParameterType(const std::string &desc)
{
   std::string type, owner, dep;
   GmatStringUtil::ParseParameter(desc, type, owner, dep);
   
   if (theModerator->IsParameter(type))
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// bool ValidateParameter(const StringArray &refNames, GmatBase *obj)
//------------------------------------------------------------------------------
bool Validator::ValidateParameter(const StringArray &refNames, GmatBase *obj)
{
   bool retval = true;
   
   for (UnsignedInt j=0; j<refNames.size(); j++)
   {
      if (FindObject(refNames[j]) == NULL)
      {
         std::string type, ownerName, depObj;
         GmatStringUtil::ParseParameter(refNames[j], type, ownerName, depObj);
         
         // Check only system parameters
         if (type == "")
         {
            theErrorMsg = "Nonexistent object \"" + refNames[j] + 
               "\" referenced in the " + obj->GetTypeName() + "\" " +
               obj->GetName() + "\"";
            HandleError();
            retval = false;
         }
         else
         {
            if (FindObject(ownerName) == NULL)
            {
               theErrorMsg = "Nonexistent object \"" + ownerName + 
                  "\" referenced in \"" + refNames[j] + "\"";
               HandleError();
               retval = false;
            }
            
            if (!theModerator->IsParameter(type))
            {
               theErrorMsg = "\"" + type + "\" is not a valid Parameter Type";
               HandleError();
               retval = false;
            }
         }
      }
   }
   
   return retval;
}


//------------------------------------------------------------------------------
// bool ValidateSaveCommand(GmatBase *obj)
//------------------------------------------------------------------------------
bool Validator::ValidateSaveCommand(GmatBase *obj)
{
   bool retval = true;
   
   ObjectTypeArray refTypes = obj->GetRefObjectTypeArray();
   
   #ifdef DEBUG_CHECK_OBJECT
   for (UnsignedInt i=0; i<refTypes.size(); i++)
      MessageInterface::ShowMessage
         ("   %s\n", GmatBase::GetObjectTypeString(refTypes[i]).c_str());
   #endif
   
   StringArray refNames = obj->GetRefObjectNameArray(Gmat::UNKNOWN_OBJECT);
   
   for (UnsignedInt j=0; j<refNames.size(); j++)
   {
      #ifdef DEBUG_CHECK_OBJECT
      MessageInterface::ShowMessage("For Save, name = %s\n", refNames.at(j).c_str()); 
      #endif
      
      GmatBase *refObj = FindObject(refNames[j]);
      if (refObj == NULL)
      {
         theErrorMsg = "Nonexistent object \"" + refNames[j] + "\" referenced in the " +
            obj->GetTypeName() + " command\"";
         HandleError();
         retval = false;
      }
      else if (refObj->GetType() == Gmat::PARAMETER)
      {
         // per Steve Hughes 2007.10.16 - arrays are OK - WCS
         // (but not array elements)
         if ( (!(refObj->IsOfType("Array"))) ||
              (GmatStringUtil::IsParenPartOfArray(refNames[j])) )
         {
            theErrorMsg = "\"" + refNames[j] + "\" referenced in the " +
               obj->GetTypeName() + " command is not a saveable object";
            HandleError();
            retval = false;
         }
      }
   }
   
   return retval;
}


//------------------------------------------------------------------------------
// bool HandleError()
//------------------------------------------------------------------------------
/*
 * Handles error condition dependends on the continue on error flag.
 * It uses the data member theErrorMsg and continueOnError.
 *
 */
//------------------------------------------------------------------------------
bool Validator::HandleError()
{
   #ifdef DEBUG_HANDLE_ERROR
   MessageInterface::ShowMessage
      ("Validator::HandleError() **** TheErrorMsg=\n<%s>\n\n", theErrorMsg.c_str());
   #endif
   
   if (continueOnError)
   {
      theErrorList.push_back(theErrorMsg);
      return false;
   }
   else
   {
      InterpreterException ex(theErrorMsg);
      throw ex;
   }
}
