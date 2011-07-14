//$Id$
//------------------------------------------------------------------------------
//                                  Validator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
#include "ParameterInfo.hpp"            // for IsSettable()

//#define DEBUG_HANDLE_ERROR
//#define DEBUG_VALIDATE_COMMAND
//#define DEBUG_CHECK_OBJECT
//#define DEBUG_CREATE_PARAM
//#define DEBUG_AUTO_PARAM
//#define DEBUG_OBJECT_MAP
//#define DEBUG_FUNCTION
//#define DEBUG_SOLAR_SYSTEM
//#define DEBUG_FIND_OBJECT
//#define DEBUG_COORD_SYS_PROP
//#define DEBUG_PROP_SETUP_PROP
//#define DEBUG_FORCE_MODEL_PROP
//#define DBGLVL_WRAPPERS 1
//#define DEBUG_AXIS_SYSTEM

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif
//#ifndef DEBUG_TRACE
//#define DEBUG_TRACE
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif
#ifdef DEBUG_TRACE
#include <ctime>                 // for clock()
#endif

//---------------------------------
// static data
//---------------------------------
Validator* Validator::instance = NULL;

//------------------------------------------------------------------------------
// static Validator* Instance()
//------------------------------------------------------------------------------
Validator* Validator::Instance()
{
   if (instance == NULL)
      instance = new Validator;
   return instance;
}


//------------------------------------------------------------------------------
// void SetInterpreter(Interpreter *interp)
//------------------------------------------------------------------------------
void Validator::SetInterpreter(Interpreter *interp)
{
   theInterpreter = interp;
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
void Validator::SetSolarSystem(SolarSystem *ss)
{
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage("Validator::SetSolarSystem() ss=<%p>\n", ss);
   #endif
   
   theSolarSystem = ss;
   
   #ifdef DEBUG_SOLAR_SYSTEM
   MessageInterface::ShowMessage
      ("Validator::SetSolarSystem() Setting SolarSystem <%p> to Moderator\n", ss);
   #endif
   
   // Set SolarSyste to use for the Moderator
   theModerator->SetInternalSolarSystem(ss);
}


//------------------------------------------------------------------------------
// void SetObjectMap(ObjectMap *objMap)
//------------------------------------------------------------------------------
void Validator::SetObjectMap(ObjectMap *objMap)
{
   if (objMap != NULL)
   {
      theObjectMap = objMap;
      
      #ifdef DEBUG_OBJECT_MAP
      if (theFunction)
         ShowObjectMap("Validator::SetObjectMap()");
      #endif
      
      #ifdef DEBUG_OBJECT_MAP
      MessageInterface::ShowMessage
         ("Validator::SetObjectMap() Setting object map <%p> to Moderator\n",
          objMap);
      #endif
      
      // Set object map to use for the Moderator
      theModerator->SetObjectMap(objMap);
   }
}


//------------------------------------------------------------------------------
// void SetFunction(Function *func)
//------------------------------------------------------------------------------
void Validator::SetFunction(Function *func)
{
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("Validator::SetFunction() function=<%p>'%s'\n", func,
       func ? func->GetName().c_str() : "NULL");
   #endif
   
   theFunction = func;
}


//------------------------------------------------------------------------------
// bool StartMatlabServer(GmatCommand *cmd)
//------------------------------------------------------------------------------
/*
 * This method starts GmatServer through the Moderator
 *
 * @param cmd  The command pointer requesting the server starup
 *             (currently not used but it may be useful for debugging)
 *
 * @return the result of Moderator::StartMatlabServer()
 */
//------------------------------------------------------------------------------
bool Validator::StartMatlabServer(GmatCommand *cmd)
{
   #ifdef DEBUG_SERVER
   MessageInterface::ShowMessage
      ("Validator::StartMatlabServer() the command <%p>'%s' requested Server startup\n",
       cmd, cmd->GetTypeName().c_str());
   #endif
   
   return theModerator->StartMatlabServer();
}


//------------------------------------------------------------------------------
// bool CheckUndefinedReference(GmatBase *obj, bool contOnError)
//------------------------------------------------------------------------------
/*
 * This method checks if reference objects of given object exist. First it
 * gets reference object type list by calling obj->GetRefObjectTypeArray() and
 * then gets reference object name list by calling obj->GetRefObjectNameArray(reftype).
 * If reference object type is Parameter, it checks if owner object of that
 * Parameter exist; otherwise, it only check for the reference object name exist.
 *
 * @param  obj  input object of undefined reference object to be checked
 * @param <contOnError> flag indicating whether or not to continue on error (true)
 *
 * @return true if no undefined references found, false otherwise
 */
//------------------------------------------------------------------------------
bool Validator::CheckUndefinedReference(GmatBase *obj, bool contOnError)
{
   if (obj == NULL)
      return false;
   
   continueOnError = contOnError;
   
   #ifdef DEBUG_CHECK_OBJECT
   MessageInterface::ShowMessage
      ("Validator::CheckUndefinedReference() obj=<%p><%s>, continueOnError=%d\n", obj,
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
         AxisSystem *axis = CreateAxisSystem("MJ2000Eq", obj);

         // Treat this as a warning not as an error (LOJ: 2010.05.14)
         theErrorMsg = "The CoordinateSystem \"" + obj->GetName() +
            "\" has empty AxisSystem, so default MJ2000Eq was created";
         MessageInterface::ShowMessage("*** WARNING *** %s\n", theErrorMsg.c_str());
         //retval = HandleError() && retval;
         
         // Set AxisSystem to CoordinateSystem
         obj->SetRefObject(axis, axis->GetType(), axis->GetName());
         
         // Since CoordinateSystem::SetRefObject() clones AxisSystem, delete it from here
         // (LOJ: 2009.03.03)
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (axis, "localAxes", "Validator::CreateAxisSystem()", "deleting localAxes");
         #endif
         delete axis;
      }
   }
   
   // Check if undefined ref. objects exist
   for (UnsignedInt i=0; i<refTypes.size(); i++)
   {
      #ifdef DEBUG_CHECK_OBJECT
      MessageInterface::ShowMessage
         ("   %s\n", GmatBase::GetObjectTypeString(refTypes[i]).c_str());
      #endif
      
      // We don't need to check for unknown object type
      if (refTypes[i] == Gmat::UNKNOWN_OBJECT)
      {
         #ifdef DEBUG_CHECK_OBJECT
         MessageInterface::ShowMessage("      Skipping UnknownObject\n");
         #endif
         continue;
      }
      
      try
      {
         refNames = obj->GetRefObjectNameArray(refTypes[i]);
         
         #ifdef DEBUG_CHECK_OBJECT
         MessageInterface::ShowMessage("      Object names are:\n");
         for (UnsignedInt j=0; j<refNames.size(); j++)
            MessageInterface::ShowMessage("      %s\n", refNames[j].c_str());
         #endif
         
         // Check System Parameters separately since it follows certain naming
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
               
               #ifdef DEBUG_CHECK_OBJECT
               MessageInterface::ShowMessage
                  ("   Checking if '%s' exist\n", refNames[j].c_str());
               #endif
               
               GmatBase *refObj = FindObject(refNames[j]);
               
               if (refObj == NULL)
               {
                  // Check if it is local object type
                  #ifdef DEBUG_CHECK_OBJECT
                  MessageInterface::ShowMessage
                     ("   Checking if '%s' is local object type\n", refNames[j].c_str());
                  #endif
                  if (!theInterpreter->IsObjectType(refNames[j]))
                  {
                     theErrorMsg = "Nonexistent " +
                        GmatBase::GetObjectTypeString(refTypes[i]) +
                        " \"" + refNames[j] + "\" referenced in the " + objName;
                     retval = HandleError() && retval;
                  }
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
                  retval = HandleError() && retval;
               }
               else
               {
                  #ifdef DEBUG_CHECK_OBJECT
                  MessageInterface::ShowMessage
                     ("   Found '%s', <%p>'%s'\n", refNames[j].c_str(), refObj,
                      refObj->GetTypeName().c_str(), refObj->GetName().c_str());
                  #endif
               }
            }
         }
      }
      catch (BaseException &e)
      {
         // Use exception to remove Visual C++ warning
         e.GetMessageType();

         #ifdef DEBUG_CHECK_OBJECT
         // since command handles setting reference object during initialization,
         // skip if object is command
         if (obj->GetType() != Gmat::COMMAND)
            MessageInterface::ShowMessage(e.GetFullMessage());
         #endif
      }
   }
   
   #ifdef DEBUG_CHECK_OBJECT
   MessageInterface::ShowMessage
      ("Validator::CheckUndefinedReference() theErrorList.size()=%d, returning %d\n",
       theErrorList.size(), retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool ValidateCommand(GmatCommand *cmd, bool contOnError, Integer manage)
//------------------------------------------------------------------------------
/**
 * Checks the input command to make sure wrappers are set up correctly.
 *
 * @param <cmd> the command to validate
 * @param <contOnError> flag indicating whether or not to continue on error (true)
 * @param <manage>  0, if Parameter is not managed
 *                  1, if Parameter is added to configuration (default)
 *                  2, if Parameter is added to function object map
 *
 * @return true if command has no error, false otherwise
 */
//------------------------------------------------------------------------------
bool Validator::ValidateCommand(GmatCommand *cmd, bool contOnError, Integer manage)
{
   #ifdef DEBUG_TRACE
   static Integer callCount = 0;
   callCount++;      
   clock_t t1 = clock();
   MessageInterface::ShowMessage
      ("=== Validator::ValidateCommand() entered, <%s> '%s' Count = %d\n",
       cmd->GetTypeName().c_str(),
       cmd->GetGeneratingString(Gmat::NO_COMMENTS).c_str(), callCount);
   #endif
   
   if (cmd == NULL)
   {
      theErrorMsg = "The input command is NULL";
      return HandleError();
   }
   
   if (theInterpreter == NULL)
   {
      theErrorMsg = "The Interpreter is not set";
      return HandleError();
   }
   
   theCommand = cmd;
   continueOnError = contOnError;
   std::string typeName = cmd->GetTypeName();
   
   #ifdef DEBUG_VALIDATE_COMMAND
   std::string genStr;
   if (cmd->GetTypeName() != "BeginScript")
      genStr = cmd-> GetGeneratingString(Gmat::NO_COMMENTS);
   MessageInterface::ShowMessage
      ("====================> Validator::ValidateCommand() cmd=<%p><%s>\"%s\"\n"
       "   continueOnError=%d, manage=%d\n", cmd, cmd->GetTypeName().c_str(),
       genStr.c_str(), continueOnError, manage);
   MessageInterface::ShowMessage
      ("   theFunction=<%p>'%s'\n", theFunction,
       theFunction ? theFunction->GetName().c_str() : "NULL");
   MessageInterface::ShowMessage
      ("   Calling %s->GetWrapperObjectNameArray() to get object names\n",
       cmd->GetTypeName().c_str());
   #endif
   
   // If objects are not managed, just return true (2008.05.21)
   if (manage == 0)
   {
      #ifdef DEBUG_VALIDATE_COMMAND
      MessageInterface::ShowMessage("Objects are not managed, so just returning true\n");
      #endif
      return true;
   }
   
   // Now, start creating wrappers
   theErrorList.clear();
   cmd->ClearWrappers();
   const StringArray wrapperNames = cmd->GetWrapperObjectNameArray();
   #ifdef DEBUG_VALIDATE_COMMAND
   MessageInterface::ShowMessage
      ("   %s has %d wrapper names:\n", cmd->GetTypeName().c_str(), wrapperNames.size());
   for (Integer ii=0; ii < (Integer) wrapperNames.size(); ii++)
      MessageInterface::ShowMessage("      %s\n", wrapperNames[ii].c_str());
   #endif
   
   #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("In ValidateCommand, has %d wrapper names:\n", wrapperNames.size());
      for (Integer ii=0; ii < (Integer) wrapperNames.size(); ii++)
         MessageInterface::ShowMessage("      %s\n", wrapperNames[ii].c_str());
   #endif
   
   // Set function pointer to command
   if (theFunction != NULL)
      cmd->SetCurrentFunction(theFunction);
   
   // Handle Assignment command (LHS = RHS) separately
   if (cmd->GetTypeName() == "GMAT")
   {
      if (!CreateAssignmentWrappers(cmd, manage))
      {
         // Handle error if function (LOJ: 2009.03.17)
         // Hmm, I cannot recall why writing error message only when
         // objects are not configured. Showing too many error messages?
         // Anyway we also need to handle assignment error in the main script.
         // This will fix Bug 1670 ((LOJ: 2009.12.09)
         //if (manage != 1)
         //{
            #if DBGLVL_WRAPPERS > 1
            MessageInterface::ShowMessage
               ("1 Could not create an ElementWrapper for \"" + theDescription + "\"\n");
            #endif
            theErrorMsg = "Undefined function or variable \"" + theDescription +
               "\" found ";
            
            return HandleError();
            //}
            //else
            //return false;
      }
   }
   else
   {
      bool paramFirst = false;
      if (cmd->IsOfType("ConditionalBranch") ||
          cmd->GetTypeName() == "Report" || cmd->GetTypeName() == "Propagate")
          paramFirst = true;
      
      for (StringArray::const_iterator i = wrapperNames.begin();
           i != wrapperNames.end(); ++i)
      {
         try
         {
            ElementWrapper *ew = CreateElementWrapper(*i, paramFirst, manage);
            
            if (ew == NULL)
            {
               #if DBGLVL_WRAPPERS > 1
               MessageInterface::ShowMessage
                  ("2 Could not create an ElementWrapper for \"" + theDescription + "\"\n");
               #endif
               theErrorMsg = " Undefined function or variable \"" + *i + "\" found ";
               
               return HandleError();
            }
            
            #if DBGLVL_WRAPPERS > 1
            MessageInterface::ShowMessage
               ("   (3)Setting ElementWrapper type %d for '%s' to '%s'\n",
                ew->GetWrapperType(), ew->GetDescription().c_str(), typeName.c_str());
            #endif
            
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
   
   // Let's try branch command
   if ((cmd->GetChildCommand(0)) != NULL)
   {
      if (!ValidateSubCommand(cmd, 0, manage))
         return false;
   }
   
   bool retval = CheckUndefinedReference(cmd);
   
   #ifdef DEBUG_VALIDATE_COMMAND
   MessageInterface::ShowMessage
      ("Validator::ValidateCommand() <%p><%s> returning %d as result of "
       "CheckUndefinedReference()\n", cmd,
       cmd->GetGeneratingString(Gmat::NO_COMMENTS).c_str(), retval);
   #endif
   
   #ifdef DEBUG_TRACE
   clock_t t2 = clock();
   MessageInterface::ShowMessage
      ("=== Validator::ValidateCommand() Count = %d, Run Time: %f seconds\n",
       callCount, (Real)(t2-t1)/CLOCKS_PER_SEC);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// void HandleCcsdsEphemerisFile(ObjectMap *objMap, bool deleteOld = false)
//------------------------------------------------------------------------------
void Validator::HandleCcsdsEphemerisFile(ObjectMap *objMap, bool deleteOld)
{
   theModerator->HandleCcsdsEphemerisFile(objMap, deleteOld);
}


//------------------------------------------------------------------------------
// ElementWrapper* CreateElementWrapper(const std::string &desc,
//                                      bool parametersFirst, Integer manage)
//------------------------------------------------------------------------------
/**
 * Creates the appropriate ElementWrapper, based on the description.
 *
 * @param  <desc>  description string for the element required
 * @param  <parametersFirst>  true if creating for wrappers for the Parameter
 *                            first then Object Property (false)
 * @param <manage>  0, if parameter is not managed
 *                  1, if parameter is added to configuration (default)
 *                  2, if Parameter is added to function object map
 *
 * @return pointer to the created ElementWrapper
 * @exception <Interpreterxception> thrown if wrapper cannot be created
 */
//------------------------------------------------------------------------------
ElementWrapper*
Validator::CreateElementWrapper(const std::string &desc, bool parametersFirst,
                                Integer manage)
{
   Gmat::WrapperDataType itsType = Gmat::NUMBER_WT;
   ElementWrapper *ew = NULL;
   Real           rval;
   
   // remove extra parens and blank spaces at either end of string
   theDescription = GmatStringUtil::Trim(desc);
   theDescription = GmatStringUtil::RemoveExtraParen(theDescription);
   theDescription = GmatStringUtil::Trim(theDescription);
   
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("In Validator::CreateElementWrapper, original string is \"%s\"\n",
       desc.c_str(), "\"\n");
   MessageInterface::ShowMessage
      ("                                 and trimmed string is \"%s\"\n",
       theDescription.c_str(), "\"\n");
   MessageInterface::ShowMessage
      ("   parametersFirst=%d, manage=%d\n", parametersFirst, manage);
   #endif
   
   // first, check to see if it is enclosed with single quotes
   // or if it is an array of strings, enclosed in braces
   if (GmatStringUtil::IsEnclosedWith(theDescription, "'") ||
       GmatStringUtil::IsEnclosedWithBraces(theDescription))
   {
      ew = new StringWrapper();
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (ew, theDescription, "Validator::CreateElementWrapper()", "ew = new StringWrapper()");
      #endif
      ew->SetDescription(theDescription);
      itsType = Gmat::STRING_WT;
      
      #if DBGLVL_WRAPPERS
      MessageInterface::ShowMessage
         (">>> In Validator, it's enclosed with quotes or braces so created a StringWrapper "
          "<%p> for \"%s\"\n", ew, theDescription.c_str(), "\"\n");
      #endif
   }
   // and then, check to see if it is a number
   else if (GmatStringUtil::ToReal(theDescription,rval))
   {
      ew = new NumberWrapper();
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (ew, theDescription, "Validator::CreateElementWrapper()", "ew = new NumberWrapper()");
      #endif
      ew->SetDescription(theDescription);
      #if DBGLVL_WRAPPERS
      MessageInterface::ShowMessage
         (">>> In Validator, created a NumberWrapper <%p> for \"%s\"\n", ew,
          theDescription.c_str(), "\"\n");
      #endif
   }
   else 
   {
      Parameter *p;
      // check to see if it is an array element
      bool isOuterParen;
      Integer openParen, closeParen;
      GmatStringUtil::FindParenMatch(theDescription, openParen, closeParen, isOuterParen);
      #if DBGLVL_WRAPPERS > 1
         MessageInterface::ShowMessage(
            "In Validator, theDescription = \"%s\"\n", theDescription.c_str());
      #endif
      if ((openParen  != -1) && (closeParen != -1) && (closeParen > openParen))
      {
         std::string arrayName = GmatStringUtil::Trim(theDescription.substr(0,openParen));
         #if DBGLVL_WRAPPERS > 1
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
            else if (!(p->IsOfType(Gmat::ARRAY)) )
            {
               theErrorMsg = "\"" + arrayName + "\"" + " is not an Array";
               HandleError();
            }
            else
            {
               ew = new ArrayElementWrapper();
               #ifdef DEBUG_MEMORY
               MemoryTracker::Instance()->Add
                  (ew, theDescription, "Validator::CreateElementWrapper()",
                   "ew = new ArrayElementWrapper()");
               #endif
               ew->SetDescription(theDescription);
               ew->SetRefObject(p);
               itsType = Gmat::ARRAY_ELEMENT_WT;
               #if DBGLVL_WRAPPERS
               MessageInterface::ShowMessage
                  (">>> In Validator, created a ArrayElementWrapper <%p> for \"%s\"\n",
                   ew, theDescription.c_str(), "\"\n");
               #endif
            }
         }
      }
      // check to see if it is an object property or a Parameter - but make sure it is not
      // an array of Reals (which can and usually do contain '.')
      else if ((theDescription.find(".") != std::string::npos) &&
              !(GmatStringUtil::IsEnclosedWithBrackets(theDescription)))
      {
         ew = CreateWrapperWithDot(parametersFirst, manage);
      }
      // check to see if it is a Variable or some other parameter
      else
      {
         ew = CreateOtherWrapper(manage);
      }
   }
   
   
   if (ew)
   {
      // if it's an ArrayElement, set up the row and column wrappers
      if (itsType == Gmat::ARRAY_ELEMENT_WT)
      {
         std::string rowName = ((ArrayElementWrapper*)ew)->GetRowName();
         std::string colName = ((ArrayElementWrapper*)ew)->GetColumnName();
         
         #if DBGLVL_WRAPPERS > 1
         MessageInterface::ShowMessage
            ("==> Creating ElementWrapper for row '%s' and column '%s'\n",
             rowName.c_str(), colName.c_str());
         #endif                  
         ElementWrapper *row    = CreateElementWrapper(rowName, false, manage);
         ((ArrayElementWrapper*)ew)->SetRow(row);
         ElementWrapper *col    = CreateElementWrapper(colName, false, manage);
         ((ArrayElementWrapper*)ew)->SetColumn(col);
      }      
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("Validator::CreateElementWrapper() returning <%p> for '%s'\n",
       ew, ew->GetDescription().c_str());
      #endif
   }
   else
   {
      createDefaultStringWrapper = true;
      
      #if DBGLVL_WRAPPERS
      MessageInterface::ShowMessage
         ("Validator::CreateElementWrapper() createDefaultStringWrapper set to true\n");
      MessageInterface::ShowMessage
         ("Validator::CreateElementWrapper() returning NULL, could not create an "
          "ElementWrapper for \"" + theDescription + "\"\n");
      #endif
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


//----------------------------
// private methods
//----------------------------


//------------------------------------------------------------------------------
// bool CreateAssignmentWrappers(GmatCommand *cmd, Integer manage)
//------------------------------------------------------------------------------
bool Validator::CreateAssignmentWrappers(GmatCommand *cmd, Integer manage)
{
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("Validator::CreateAssignmentWrappers() entered, cmd=<%p>, theDescription='%s', "
       "manage=%d\n", cmd, theDescription.c_str(), manage);
   #endif
   
   std::string typeName = cmd->GetTypeName();
   const StringArray wrapperNames = cmd->GetWrapperObjectNameArray();
   
   //---------------------------------------------------------------------------
   // Special case for Assignment command (LHS = RHS).
   // Since such as Sat.X can be both Parameter or ObjectProperty, we want to
   // create a Parameter wrapper if RHS is a Parameter for Assignment command.
   // So special code is needed to tell the CreateElementWrapper() to check for
   // Parameter first.
   //---------------------------------------------------------------------------
   
   Assignment *acmd = (Assignment*)cmd;
   
   std::string lhs = acmd->GetLHS();
   std::string rhs = acmd->GetRHS();
   bool isMathTree = false;
   if (acmd->GetMathTree() != NULL)
      isMathTree = true;
   
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage("   =====> It is %sa math equation\n", isMathTree ? "" : "not ");
   #endif
   
   StringArray parts = GmatStringUtil::SeparateDots(lhs);
   GmatBase *theObj = FindObject(parts[0]);
   
   // Check if lhs is PropSetup and need to create a Propagator
   if (theObj != NULL &&
       theObj->GetType() == Gmat::PROP_SETUP && parts.size() >= 2)
   {
      if (parts[1] == "Type")
      {
//         throw InterpreterException("Cannot set a Propagator type (" + rhs +
//               ") on the PropSetup " + theObj->GetName() +
//               "; this action is not allowed in the Mission Control Sequence.");
         if (!CreatePropSetupProperty(theObj, lhs, rhs))
         {
            MessageInterface::ShowMessage
               ("==> Validator::ValidateCommand() returning false\n");
            return false;
         }
      }
   }
   
   // Check if lhs is ForceModel and need to create a PhysicalModel
   if (theObj != NULL &&
       theObj->GetType() == Gmat::ODE_MODEL && parts.size() >= 2)
   {
      if (parts[1] == "PrimaryBodies" || parts[1] == "PointMasses" ||
          parts[1] == "SRP" || parts[1] == "Drag" || parts[1] == "RelativisticCorrection")
      {
         if (!CreateForceModelProperty(theObj, lhs, rhs))
         {
            MessageInterface::ShowMessage
               ("==> Validator::ValidateCommand() returning false\n");
            return false;
         }
      }
   }
   
   // Check if lhs is CoordinateSystem and need to create an AxisSystem
   if (theObj != NULL &&
       theObj->GetType() == Gmat::COORDINATE_SYSTEM && parts.size() >= 2)
   {
      if (parts[1] == "Axes")
      {
         if (!CreateCoordSystemProperty(theObj, lhs, rhs))
         {
            MessageInterface::ShowMessage
               ("==> Validator::ValidateCommand() returning false\n");
            return false;
         }
      }
   }
   
   //-------------------------------------------------------------------
   // Handle LHS
   //-------------------------------------------------------------------
   ElementWrapper *leftEw = NULL;
   static bool writeWarning = true; // To write warning messaage per session
   
   try
   {         
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage("==========> Create Assignment LHS wrapper\n");
      MessageInterface::ShowMessage("   createDefaultStringWrapper set to false\n");
      #endif
      
      createDefaultStringWrapper = false;
      std::string type, owner, dep;
      GmatStringUtil::ParseParameter(lhs, type, owner, dep);
      
      // If lhs has two dots and settable, treat it as Parameter.
      // This will enable assignment such as Sat.Thruster1.FuelMass = 735;
      // Handle deprecated Element* on Thruster (2009.12.15)
      std::string newType = type;
      if ((type == "Element1" || type == "Element2" || type == "Element3") &&
          GmatStringUtil::NumberOfOccurrences(lhs, '.') > 1)
      {
         newType = GmatStringUtil::Replace(newType, "Element", "ThrustDirection");
         #if DBGLVL_WRAPPERS > 0
         MessageInterface::ShowMessage
            ("   Parameter type '%s' in '%s' changed to '%s'\n", type.c_str(),
             lhs.c_str(), newType.c_str());
         #endif
         if (writeWarning)
         {
            MessageInterface::ShowMessage
               ("*** WARNING *** The Parameter type \"" + type + "\" of Thruster is "
                "deprecated and will be removed from a future build; please use \"" +
                newType + "\" instead in \"" + lhs + ".\"\n");
            writeWarning = false;
         }
      }
      
      bool isLhsSettable = ParameterInfo::Instance()->IsSettable(newType);
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("   ==> '%s' is%ssettable\n", newType.c_str(), isLhsSettable ? " " : " NOT ");
      #endif
      
      if (lhs == "" || lhs == "Not_Set")
         createDefaultStringWrapper = true;
      
      if (lhs.find_first_of(".") != lhs.find_last_of(".") && isLhsSettable)
         leftEw = CreateElementWrapper(lhs, true, manage);
      else
         leftEw = CreateElementWrapper(lhs, false, manage);
      
      if (leftEw == NULL)
         return false;
      
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("   (1)Setting ElementWrapper type %d for '%s' to '%s'\n",
          leftEw->GetWrapperType(), leftEw->GetDescription().c_str(), typeName.c_str());
      #endif
      
      if (cmd->SetElementWrapper(leftEw, lhs) == false)
      {
         theErrorMsg = "Failed to set ElementWrapper for LHS object \"" + lhs +
            "\" in Assignment";
         return HandleError();
      }
   }
   catch (BaseException &ex)
   {
      theErrorMsg = ex.GetFullMessage();
      return HandleError(false);
   }
   
   //-------------------------------------------------------------------
   // Handle RHS
   //-------------------------------------------------------------------
   /**
    * @note
    * Assignment::GetWrapperObjectNameArray() returns only RHS elements.
    */
   //-------------------------------------------------------------------
   
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage("==========> Create Assignment RHS wrapper\n");
   MessageInterface::ShowMessage("   Has %d wrapper names\n", wrapperNames.size());
   for (Integer ii=0; ii < (Integer) wrapperNames.size(); ii++)
      MessageInterface::ShowMessage("      %s\n", wrapperNames[ii].c_str());
   #endif
   
   createDefaultStringWrapper = true;
   // Actually we want to check for return data type not the wrapper type here.
   // (This will fix bug 2196)
   //if (leftEw->GetWrapperType() == Gmat::VARIABLE_WT ||
   //    leftEw->GetWrapperType() == Gmat::ARRAY_ELEMENT_WT)
   if (leftEw->GetDataType() == Gmat::REAL_TYPE)
      createDefaultStringWrapper = false;
   
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("   createDefaultStringWrapper set to %s\n",
       createDefaultStringWrapper ? "true" : "false");
   #endif
   
   // check if there is missing single quote in RHS if LHS is string type(loj: 2008.07.22)
   // it will catch missing end quote
   if (leftEw->GetDataType() == Gmat::STRING_TYPE ||
       leftEw->GetDataType() == Gmat::STRINGARRAY_TYPE)
   {
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage("==========> LHS type is STRING or STRINGARRAY\n");
      #endif
     // first remove ending ; from the RHS
      rhs = GmatStringUtil::RemoveLastString(rhs, ";");
      if (GmatStringUtil::HasMissingQuote(rhs, "'"))
      {
         std::string fnName;
         if (theFunction)
            fnName = "GmatFunction \"" + theFunction->GetFunctionPathAndName() + "\"";
         
         theErrorMsg = "Assignment command has missing end quote on the "
            "right-hand-side in " + fnName + "\n   \"" +
            cmd->GetGeneratingString(Gmat::NO_COMMENTS) + "\"";
         return HandleError();
      }
   }
   
   for (StringArray::const_iterator i = wrapperNames.begin();
        i != wrapperNames.end(); ++i)
   {
      std::string origVal = (*i);
      
      // Since wrapperNames includes wrapper name for LHS, skip if same as LHS.
      // For math tree we need to create new ParameterWrapper if it is object property
      // since math tree works with Parameters only
      if ((origVal == lhs && !isMathTree) ||
          (origVal == lhs && isMathTree && origVal.find(".") == origVal.npos))
         continue;
      
      std::string name = origVal;
      bool addedQuotes = false;
      
      // Special handling for FILENAME_TYPE which doesn't require enclosing with quotes
      // Bug 2063 fix
      if (createDefaultStringWrapper)
      {
         if (leftEw->GetDataType() == Gmat::FILENAME_TYPE &&
             !GmatStringUtil::IsEnclosedWith(name, "'"))
         {
            // Check if name is not an object name (Bug 2148 fix)
            if (FindObject(name) == NULL)
            {
               name = GmatStringUtil::AddEnclosingString(origVal, "'");
               addedQuotes = true;
               #if DBGLVL_WRAPPERS > 1
               MessageInterface::ShowMessage
                  ("   ===> It is FILENAME_TYPE and not an object name, "
                   "so added quotes, name='%s'\n", name.c_str());
               #endif
            }
         }
      }
      if (name != "")
      {
         #if DBGLVL_WRAPPERS > 1
         MessageInterface::ShowMessage("   Passing '%s' to CreateElementWrapper()\n", name.c_str());
         #endif
         
         try
         {                  
            ElementWrapper *ew = NULL;
            if (IsParameterType(name))
               ew = CreateElementWrapper(name, true, manage);
            else
               ew = CreateElementWrapper(name, false, manage);
            
            #if DBGLVL_WRAPPERS > 1
            if (ew == NULL)
               MessageInterface::ShowMessage("   (2) ElementWrapper is NULL\n");
            else
               MessageInterface::ShowMessage
                  ("   (2)Setting ElementWrapper type %d for '%s' to '%s'\n",
                   ew->GetWrapperType(), ew->GetDescription().c_str(), typeName.c_str());
            #endif
            
            std::string strToUse = name;
            if (addedQuotes)
               strToUse = origVal;
            
            #if DBGLVL_WRAPPERS > 1
            MessageInterface::ShowMessage
               ("   Calling cmd->SetElementWrapper(<%p>, '%s')\n", ew, strToUse.c_str());
            #endif
            if (cmd->SetElementWrapper(ew, strToUse) == false)
            {
               theErrorMsg = "Failed to set ElementWrapper for RHS object \"" + strToUse +
                  "\" in Assignment";
               return HandleError();
            }
         }
         catch (BaseException &ex)
         {
            theErrorMsg = ex.GetFullMessage();
            return HandleError(false);
         }
      }
   }
   
   // Set math wrappers to math tree
   acmd->SetMathWrappers();
   
   // Handle special case for Formation in GmatFunction(loj: 2008.07.08)
   // Since spacecrafts are add to formation when Assignment command
   // is executed, it throws an exception of no spacecraft are set
   // when Formation::BuildState() is called in Propagate::Initialzize().
   // Formation.Add = {Sat1, Sat2}
   if (theFunction != NULL && theObj != NULL &&
       (theObj->IsOfType(Gmat::FORMATION) && lhs.find(".Add") != lhs.npos))
   {
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage("   Handling Formation Add\n");
      #endif
      TextParser tp;
      StringArray names = tp.SeparateBrackets(rhs, "{}", " ,", false);
      for (UnsignedInt i=0; i<names.size(); i++)
         theObj->SetStringParameter("Add", names[i]);
   }
   
   // Special case for ReporFile, we need to create a wrapper for each
   // Parameter that is added to ReportFile,
   // so call Interpreter::ValidateSubscriber() to create wrappers.
   // ReportFile.Add = {Sat1.A1ModJulian, Sat1.EarthMJ2000Eq.X}
   if (theFunction != NULL && theObj != NULL &&
       (theObj->IsOfType(Gmat::REPORT_FILE) && lhs.find(".Add") != lhs.npos))
   {
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage("   Handling ReportFile Add\n");
      #endif
      TextParser tp;
      StringArray names = tp.SeparateBrackets(rhs, "{}", " ,", false);
      for (UnsignedInt i=0; i<names.size(); i++)
         theObj->SetStringParameter("Add", names[i]);
      
      if (theInterpreter != NULL)
      {
         if (!theInterpreter->ValidateSubscriber(theObj))
         {
            theErrorMsg = "Failed to create ElementWrapper for \"" +
               theDescription + "\"";
            return HandleError();
         }
      }
      else
      {
         theErrorMsg = "Failed to create ElementWrapper for \"" +
            theDescription + "\". The Interpreter is not set.";
         return HandleError();
      }
   }
   
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("Validator::CreateAssignmentWrappers() returning true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// ElementWrapper* CreateSolarSystemWrapper(const std::string &owner, ...)
//------------------------------------------------------------------------------
ElementWrapper* Validator::CreateSolarSystemWrapper(GmatBase *obj,
                                                   const std::string &owner,
                                                   const std::string &depobj,
                                                   const std::string &type,
                                                   Integer manage)
{
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("Validator::CreateSolarSystemWrapper() entered, theDescription='%s'"
       "\n   obj=<%p><%s>, owner='%s', depobj='%s', type='%s'\n", theDescription.c_str(),
       obj, obj ? obj->GetName().c_str() : "NULL", owner.c_str(), depobj.c_str(),
       type.c_str());
   #endif
   
   ElementWrapper *ew = NULL;
   SolarSystem *ss = (SolarSystem *)obj;
   
   if (depobj == "")
   {
      ew = new ObjectPropertyWrapper();
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (ew, theDescription, "Validator::CreateSolarSystemWrapper()",
          "ew = new ObjectPropertyWrapper()");
      #endif
      ew->SetDescription(theDescription);
      ew->SetRefObject(obj);
      
      #if DBGLVL_WRAPPERS
      MessageInterface::ShowMessage
         (">>> In Validator, created a ObjectPropertyWrapper <%p> for property \"%s\"\n    "
          "of <%s>'%s', theDescription='%s'\n", ew, type.c_str(), obj->GetTypeName().c_str(),
          obj->GetName().c_str(), theDescription.c_str());
      #endif
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("Validator::CreateSolarSystemWrapper() returning <%p>\n", ew);
      #endif
      return ew;
   }
   else
   {
      // Script has the form of:
      // GMAT SolarSystem.Earth.NutationUpdateInterval = 60.0;
      // GMAT SolarSystem.Earth.UseTTForEphemeris = true;
      // GMAT SolarSystem.Earth.DateFormat  = TAIModJulian;
      // GMAT SolarSystem.Earth.StateType   = Keplerian;
      // GMAT SolarSystem.Earth.InitalEpoch = 21544.500371
      // GMAT SolarSystem.Earth.SMA         = 149653978.978377
      // GMAT SolarSystem.Ephemeris = {SLP}
      
      StringArray parts = GmatStringUtil::SeparateDots(depobj);
      std::string bodyName = parts[0];
      
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("   bodyName='%s', type='%s'\n", bodyName.c_str(), type.c_str());
      #endif
      
      // Get body from the SolarSystem
      GmatBase *body = (GmatBase*)ss->GetBody(bodyName);
      
      if (body == NULL)
      {
         theErrorMsg = "The body named \"" + bodyName + "\" not found in the SolarSystem\n";
         HandleError();
      }
      else
      {
         try
         {
            body->GetParameterID(type);
            ew = new ObjectPropertyWrapper();
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Add
               (ew, theDescription, "Validator::CreateSolarSystemWrapper()",
                "ew = new ObjectPropertyWrapper()");
            #endif
            ew->SetDescription(theDescription);
            ew->SetRefObjectName(bodyName, 0);
            ew->SetRefObject(body);
            
            #if DBGLVL_WRAPPERS
            MessageInterface::ShowMessage
               (">>> In Validator, created a ObjectPropertyWrapper <%p> for property \"%s\"\n    "
                "of <%s>'%s', theDescription='%s'\n", ew, type.c_str(), body->GetTypeName().c_str(),
                body->GetName().c_str(), theDescription.c_str());
            #endif
         }
         catch (BaseException &e)
         {
            theErrorMsg = e.GetFullMessage();
            HandleError();
         }
      }
   }
   
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("Validator::CreateSolarSystemWrapper() returning <%p>\n", ew);
   #endif
   
   return ew;
}


//------------------------------------------------------------------------------
// ElementWrapper* CreateForceModelWrapper(const std::string &owner, ...)
//------------------------------------------------------------------------------
ElementWrapper* Validator::CreateForceModelWrapper(GmatBase *obj,
                                                   const std::string &owner,
                                                   const std::string &depobj,
                                                   const std::string &type,
                                                   Integer manage)
{
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("Validator::CreateForceModelWrapper() entered, theDescription='%s'"
       "\n   obj=<%p><%s>, owner='%s', depobj='%s', type='%s'\n", theDescription.c_str(),
       obj, obj ? obj->GetName().c_str() : "NULL", owner.c_str(), depobj.c_str(),
       type.c_str());
   #endif
   
   ElementWrapper *ew = NULL;
      
   //------------------------------------------------------------
   // Create wrapper for ForceModel object properties
   //------------------------------------------------------------
   ew = CreatePropertyWrapper(obj, type, manage, false);
   if (ew != NULL)
   {
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("Validator::CreateForceModelWrapper() returning <%p>\n", ew);
      #endif
      return ew;
   }
   
   //------------------------------------------------------------
   // Create wrapper for ForceModel owned object properties
   //------------------------------------------------------------
   StringArray parts = GmatStringUtil::SeparateDots(depobj);
   std::string pmType = parts[0];
   std::string forceType = ODEModel::GetScriptAlias(pmType);
   std::string propName = type;
   
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("   Setting pmType=%s, forceType=%s, propName=%s\n", pmType.c_str(),
       forceType.c_str(), propName.c_str());
   #endif
   
   GmatBase *ownedObj;
   Integer propId;
   Gmat::ParameterType propType;
   if (theInterpreter->FindPropertyID(obj, propName, &ownedObj, propId, propType))
   {
      #if DBGLVL_WRAPPERS
      MessageInterface::ShowMessage
         ("   after FindPropertyID(), propId=%d, ownedObj=<%p><%s>'%s'\n",
          propId, ownedObj, ownedObj ? ownedObj->GetTypeName().c_str() : "NULL",
          ownedObj ? ownedObj->GetName().c_str() : "NULL");
      #endif
      
      ew = new ObjectPropertyWrapper();
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (ew, theDescription, "Validator::CreateForceModelWrapper()",
          "ew = new ObjectPropertyWrapper()");
      #endif
      ew->SetDescription(theDescription);
      
      #if DBGLVL_WRAPPERS
      MessageInterface::ShowMessage
         (">>> In Validator, created a ObjectPropertyWrapper <%p> for owned property \"%s\"\n    "
          "of <%s>'%s', theDescription='%s'\n", ew, propName.c_str(),
          ownedObj->GetTypeName().c_str(), ownedObj->GetName().c_str(),
          theDescription.c_str());
      #endif
      
      ew->SetRefObjectName(ownedObj->GetName());
      ew->SetRefObject(ownedObj);
      // @note
      // Handle special case for GmatFunction(loj: 2008.07.07)
      // For ForceModel, the PhysicalModel is created as local object
      // but name is added automatically to refObjectNames in SetupWrapper(),
      // so we need to clear.
      if (ownedObj->IsOfType(Gmat::PHYSICAL_MODEL))
         ew->ClearRefObjectNames();
      
   }
   else
   {
      #if DBGLVL_WRAPPERS
      MessageInterface::ShowMessage
         ("===> theInterpreter->FindPropertyID() returned false\n");
      #endif
   }
   
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("Validator::CreateForceModelWrapper() returning <%p>\n", ew);
   #endif
   
   return ew;
   
}


//------------------------------------------------------------------------------
// ElementWrapper* CreateWrapperWithDot(bool parametersFirst, Integer manage)
//------------------------------------------------------------------------------
ElementWrapper* Validator::CreateWrapperWithDot(bool parametersFirst, Integer manage)
{
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("Validator::CreateWrapperWithDot() entered, theDescription='%s', "
       "parametersFirst=%d, manage=%d\n", theDescription.c_str(), parametersFirst, manage);
   #endif
   
   // try to parse the string for an owner and type
   // NOTE - need to check handling of "special cases, e.g.Sat1.X" <<<<<<<<<<
   // check for object parameter first
   ElementWrapper *ew = NULL;
   std::string type, owner, depobj;
   GmatStringUtil::ParseParameter(theDescription, type, owner, depobj);
   GmatBase *obj = FindObject(owner);
   
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("   obj=<%p><%s>'%s', owner='%s', depobj='%s', type='%s'\n", obj,
       obj ? obj->GetTypeName().c_str() : "NULL", obj ? obj->GetName().c_str() : "NULL", 
       owner.c_str(), depobj.c_str(), type.c_str());
   #endif
   
   // if cannot find object and manage option is to use configuration,
   // we cannot continue, so just return NULL (loj: 2008.07.24)
   if (obj == NULL && manage == 1)
   {
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("Validator::CreateWrapperWithDot() returning NULL, the configured "
          "object '%s' not found\n", owner.c_str());
      #endif
      return NULL;
   }
   
   //-----------------------------------------------------------------
   // Special case for SolarSystem
   //-----------------------------------------------------------------
   if (obj != NULL && obj->GetType() == Gmat::SOLAR_SYSTEM)
   {
      ew = CreateSolarSystemWrapper(obj, owner, depobj, type, manage);
      
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("Validator::CreateSolarSystemWrapper() returning <%p>\n", ew);
      #endif
      
      return ew;
   }
   
   //-----------------------------------------------------------------
   // Special case for ForceModel
   //-----------------------------------------------------------------
   if (obj != NULL && obj->GetType() == Gmat::ODE_MODEL)
   {
      ew = CreateForceModelWrapper(obj, owner, depobj, type, manage);
      
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("Validator::CreateForceModelWrapper() returning <%p>\n", ew);
      #endif
      
      return ew;
   }
   
   //-----------------------------------------------------------------
   // Now continue with the rest
   //-----------------------------------------------------------------
   ew = CreateValidWrapperWithDot(obj, type, owner, depobj,
                                  parametersFirst, manage);
   
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("Validator::CreateWrapperWithDot() returning <%p>\n", ew);
   #endif
   
   return ew;
   
}


//------------------------------------------------------------------------------
// ElementWrapper* CreateOtherWrapper(Integer manage)
//------------------------------------------------------------------------------
ElementWrapper* Validator::CreateOtherWrapper(Integer manage)
{
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("Validator::CreateOtherWrapper() entered, theDescription='%s', manage=%d\n",
       theDescription.c_str(), manage);
   #endif
   
   Gmat::WrapperDataType itsType = Gmat::NUMBER_WT;
   ElementWrapper *ew = NULL;
   Parameter *p = GetParameter(theDescription);
   
   if ( (p) && (p->IsOfType(Gmat::VARIABLE)) )
   {
      ew = new VariableWrapper();
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (ew, theDescription, "Validator::CreateOtherWrapper()",
          "ew = new VariableWrapper()");
      #endif
      ew->SetDescription(theDescription);
      ew->SetRefObject(p);
      itsType = Gmat::VARIABLE_WT;
      
      #if DBGLVL_WRAPPERS
      MessageInterface::ShowMessage
         (">>> In Validator, created a VariableWrapper for \"%s\"\n",
          theDescription.c_str(), "\"\n");
      #endif
   }
   else if ( (p) && p->IsOfType(Gmat::ARRAY) )
   {
      ew = new ArrayWrapper();
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (ew, theDescription, "Validator::CreateOtherWrapper()",
          "ew = new ArrayWrapper()");
      #endif
      ew->SetDescription(theDescription);
      ew->SetRefObject(p);
      itsType = Gmat::ARRAY_WT;
      
      #if DBGLVL_WRAPPERS
      MessageInterface::ShowMessage
         (">>> In Validator, created an ArrayWrapper <%p> for \"%s\"\n", ew,
          theDescription.c_str(), "\"\n");
      #endif
   }
   else if ( (p) && p->IsOfType(Gmat::STRING) )
   {
      ew = new StringObjectWrapper();
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (ew, theDescription, "Validator::CreateOtherWrapper()",
          "ew = new StringObjectWrapper()");
      #endif
      ew->SetDescription(theDescription);
      ew->SetRefObject(p);
      itsType = Gmat::STRING_OBJECT_WT;
      
      #if DBGLVL_WRAPPERS
      MessageInterface::ShowMessage
         (">>> In Validator, it's a String so created a StringObjectWrapper <%p> for \"%s\"\n",
          ew, theDescription.c_str(), "\"\n");
      #endif
   }
   else if ( (p) && p->IsOfType(Gmat::PARAMETER) )
   {
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("In Validator(5), about to create a ParameterWrapper for \"%s\"\n",
          theDescription.c_str(), "\"\n");
      #endif
      
      ew = CreateParameterWrapper(p, itsType);
   }
   else
   {
      // Check see if it is clonable object
      GmatBase *obj = FindObject(theDescription);
      
      if (obj != NULL)
      {
         ew = new ObjectWrapper();
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (ew, theDescription, "Validator::CreateOtherWrapper()",
             "ew = new ObjectWrapper()");
         #endif
         ew->SetDescription(theDescription);
         ew->SetRefObject(obj);
         itsType = Gmat::OBJECT_WT;
         
         #if DBGLVL_WRAPPERS
         MessageInterface::ShowMessage
            (">>> In Validator, created an ObjectWrapper <%p> for \"%s\", Set refObj to <%p>'%s'\n",
             ew, theDescription.c_str(), obj, obj->GetName().c_str());
         #endif
      }
      else
      {
         bool bVal;
         if (GmatStringUtil::ToBoolean(theDescription, bVal))
         {
            ew = new BooleanWrapper();
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Add
               (ew, theDescription, "Validator::CreateOtherWrapper()",
                "ew = new BooleanWrapper()");
            #endif
            ew->SetDescription(theDescription);
            itsType = Gmat::BOOLEAN_WT;
            
            #if DBGLVL_WRAPPERS
            MessageInterface::ShowMessage
               (">>> In Validator, created a BooleanWrapper <%p> for \"%s\"\n", ew,
                theDescription.c_str(), "\"\n");
            #endif
         }
         else if (theDescription == "On" || theDescription == "Off" ||
                  theDescription == "on" || theDescription == "off")
         {
            ew = new OnOffWrapper();
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Add
               (ew, theDescription, "Validator::CreateOtherWrapper()",
                "ew = new OnOffWrapper()");
            #endif
            ew->SetDescription(theDescription);
            itsType = Gmat::ON_OFF_WT;
            
            #if DBGLVL_WRAPPERS
            MessageInterface::ShowMessage
               (">>> In Validator, created a OnOffWrapper <%p> for \"%s\"\n", ew,
                theDescription.c_str(), "\"\n");
            #endif
         }
         //===========================================================
         #ifdef __ALLOW_MATH_EXP_IN_FUNCTION__
         //===========================================================
         // check if it is math equation or single undefined variable
         // so that number wrapper can be created for "2+2" or "x" for 
         // GmatFunction input value (loj: 2008.08.27)
         else if ((GmatStringUtil::IsMathEquation(theDescription)) ||
                  (GmatStringUtil::IsValidName(theDescription)))
         {
            ew = new NumberWrapper();
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Add
               (ew, theDescription, "Validator::CreateOtherWrapper()",
                "ew = new NumberWrapper()");
            #endif
            ew->SetDescription(theDescription);
            itsType = Gmat::NUMBER_WT;
            
            #if DBGLVL_WRAPPERS
            MessageInterface::ShowMessage
               (">>> In Validator, created a VariableWrapper <%p> for \"%s\"\n", ew,
                theDescription.c_str(), "\"\n");
            #endif
         }
         //===========================================================
         #endif
         //===========================================================
         else
         {
            // For bug fix 1918, if not creating default StringWrapper 
            // throw an exception instead (LOJ: 2010.08.24)
            #if DBGLVL_WRAPPERS
            MessageInterface::ShowMessage
               ("   createDefaultStringWrapper = %d\n", createDefaultStringWrapper);
            #endif
            if (createDefaultStringWrapper)
            {
               // Create default StringWrapper
               ew = new StringWrapper();
               #ifdef DEBUG_MEMORY
               MemoryTracker::Instance()->Add
                  (ew, theDescription, "Validator::CreateOtherWrapper()",
                   "ew = new StringWrapper()");
               #endif
               ew->SetDescription(theDescription);
               itsType = Gmat::STRING_WT;
               
               #if DBGLVL_WRAPPERS
               MessageInterface::ShowMessage
                  (">>> In Validator, it's not an On/Off type so created a StringWrapper "
                   "<%p> for \"%s\"\n", ew, theDescription.c_str(), "\"\n");
               #endif
            }
            else
            {            
               theErrorMsg = "Nonexistent object \"" + theDescription +
                  "\" referenced during creating Wrapper for \"" +
                  theCommand->GetGeneratingString(Gmat::NO_COMMENTS) + "\"";
               continueOnError = false;
               HandleError();
            }
         }
      }
   }
   
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("Validator::CreateOtherWrapper() returning <%p>\n", ew);
   #endif
   
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
GmatBase* Validator::FindObject(const std::string &name, const std::string &ofType)
{
   #ifdef DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage
      ("Validator::FindObject() entered: name = '%s', ofType = '%s'\n",
       name.c_str(), ofType.c_str());
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
   
   #ifdef DEBUG_OBJECT_MAP
   if (theFunction != NULL)
      ShowObjectMap("Validator::FindObject()");
   #endif
   
   // Find object from the object map
   if (theObjectMap->find(newName) != theObjectMap->end())
   {
      #ifdef DEBUG_FIND_OBJECT
      MessageInterface::ShowMessage
         ("   name of map obj=<%s>\n", (*theObjectMap)[newName]->GetName().c_str());
      #endif
      if ((*theObjectMap)[newName]->GetName() == newName)
         obj = (*theObjectMap)[newName];
   }
   
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
   
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("Validator::GetParameter() obj=<%p><%s>'%s'\n", obj,
       (obj ? obj->GetTypeName().c_str() : "UnknownType"), name.c_str());
   #endif
   
   if (obj && obj->GetType() == Gmat::PARAMETER)
   {
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage("Validator::GetParameter() returning <%p>\n", obj);
      #endif
      
      return (Parameter*)obj;
   }
   
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage("Validator::GetParameter() returning NULL\n");
   #endif
   
   return NULL;
}


//------------------------------------------------------------------------------
// Parameter* CreateSystemParameter(bool &paramCreated, const std::string &name,
//                                  Integer manage = 1)
//------------------------------------------------------------------------------
/**
 * Creates a system Parameter from the input parameter name. If the name contains
 * dots, it consider it as a system parameter.  If it is not a system Parameter
 * it checks if object by given name is a Parameter.
 *
 * @param paramCreated will set to true if one or more Parameters were created
 *
 * @param  name   parameter name to be parsed for Parameter creation
 *                Such as, sat1.Earth.ECC, sat1.SMA
 * @param  manage   0, if parameter is not managed
 *                  1, if parameter is added to configuration (default)
 *                  2, if Parameter is added to function object map
 * @param paramCreated true if one or more Parameters are created
 *
 * @return Created Paramteter pointer or pointer of the Parameter by given name
 *         NULL if it is not a system Parameter nor named object is not a Parameter
 *
 */
//------------------------------------------------------------------------------
Parameter* Validator::CreateSystemParameter(bool &paramCreated,
                                            const std::string &str, Integer manage)
{
   #ifdef DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("Validator::CreateSystemParameter() str='%s', manage=%d\n", str.c_str(), manage);
   #endif
   
   // Since GmatFunction can have such as "GMAT XYPlot.Add = {sat.X, sat.Y};",
   // we need to create multiple Parameters for valid Parameters.
   // So see if there is {} and handle all names in the loop (loj: 2008.06.18)
   // All new Parameters should be added to the function automatic object store.
   TextParser tp;
   Parameter *param = NULL;
   paramCreated = false;
   
   if (str == "")
   {
      #ifdef DEBUG_CREATE_PARAM
      MessageInterface::ShowMessage
         ("Validator::CreateSystemParameter() returning NULL, input string is blank\n");
      #endif
      return NULL;
   }
   
   // Check if string enclosed with single quotes (LOJ: 2010.08.09)
   if (GmatStringUtil::IsEnclosedWith(str, "'"))
   {
      #ifdef DEBUG_CREATE_PARAM
      MessageInterface::ShowMessage
         ("Validator::CreateSystemParameter() returning NULL, input string is is "
          "enclosed with single quotes\n");
      #endif
      return NULL;
   }
   
   StringArray names;
   // If { found add comma to delimiter to handle "GMAT XYPlot.Add = {sat.X, sat.Y}"
   // otherwise just use space to catch invalid Parameter type
   // Is this reasonable assumption? (loj: 2008.06.18)
   if (str.find("{") != str.npos)
      names = tp.SeparateBrackets(str, "{}", " ,", false);
   else
      names = tp.SeparateBrackets(str, "{}", " ", false);
   
   for (UnsignedInt i=0; i<names.size(); i++)
   {
      #ifdef DEBUG_CREATE_PARAM
      MessageInterface::ShowMessage("   names[%d]='%s'\n", i, names[i].c_str());   
      #endif
      
      std::string paramType, ownerName, depName;
      GmatStringUtil::ParseParameter(names[i], paramType, ownerName, depName);
      
      #ifdef DEBUG_CREATE_PARAM
      MessageInterface::ShowMessage
         ("   paramType='%s', ownerName='%s', depName='%s'\n", paramType.c_str(),
          ownerName.c_str(), depName.c_str());
      #endif
      
      // Create parameter, if type is a System Parameter
      if (find(theParameterList.begin(), theParameterList.end(), paramType) != 
          theParameterList.end())
      {
         bool alreadyManaged;
         param = CreateAutoParameter(paramType, names[i], alreadyManaged, ownerName,
                                     depName, manage);
         paramCreated = true;
         
         #ifdef DEBUG_CREATE_PARAM
         MessageInterface::ShowMessage
            ("   Parameter created with paramType='%s', ownerName='%s', depName='%s', "
             "theFunction=<%p>'%s'\n", paramType.c_str(), ownerName.c_str(), depName.c_str(),
             theFunction, theFunction ? theFunction->GetName().c_str() : "NULL");
         #endif
         
         // Add unmanaged Parameter to function
         if (theFunction != NULL)
         {
            #ifdef DEBUG_AUTO_PARAM
            MessageInterface::ShowMessage
               ("Validator::CreateSystemParameter() Adding <%p><%s> '%s' to "
                "function's automatic object map\n", param, param->GetTypeName().c_str(),
                param->GetName().c_str());
            #endif
            
            // if automatic parameter is in the objectMap, set flag so that
            // it won't be deleted in the function since it is deleted in the
            // Sandbox. (LOJ: 2009.03.16)
            theFunction->AddAutomaticObject(param->GetName(),(GmatBase*)param,
                                            alreadyManaged);
         }
      }
      else
      {
         // Find the object and check if it is a Parameter
         GmatBase *obj = FindObject(names[i]);
         if (obj != NULL && obj->GetType() == Gmat::PARAMETER)
         {
            param = (Parameter*)obj;
            
            #ifdef DEBUG_CREATE_PARAM
            MessageInterface::ShowMessage("   Parameter <%s> found\n", names[i].c_str());
            #endif
         }
      }
   }
   
   Parameter *realParam = NULL;
   if (names.size() == 1)
      realParam = param;
   
   #ifdef DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("Validator::CreateSystemParameter() paramCreated=%d, returning <%p><%s>'%s'\n",
       paramCreated, realParam,
       (realParam == NULL) ? "NULL" : realParam->GetTypeName().c_str(),
       (realParam == NULL) ? "NULL" : realParam->GetName().c_str());
   #endif
   
   return realParam;
}


//------------------------------------------------------------------------------
// Parameter* CreateAutoParameter(const std::string &type, const std::string &name,
//                            const std::string &ownerName, const std::string &depName
//                            Integer manage = 1)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to create a Parameter. If object is not managed this
 * method does not check for existing Parameter before creating one since
 * Moderator::CreateParameter() sets Parameter reference objects if Parameter
 * was created without reference during GmatFunction parsing.
 * 
 * @param  type       Type of parameter requested
 * @param  name       Name for the parameter.
 * @param  ownerName  object name of parameter requested ("")
 * @param  depName    Dependent object name of parameter requested ("")
 * @param  manage     0, if parameter is not managed
 *                    1, if parameter is added to configuration (default)
 *                    2, if Parameter is added to function object map
 * 
 * @return Pointer to the constructed Parameter.
 */
//------------------------------------------------------------------------------
Parameter* Validator::CreateAutoParameter(const std::string &type, 
                                          const std::string &name,
                                          bool &alreadyManaged,
                                          const std::string &ownerName,
                                          const std::string &depName,
                                          Integer manage)
{
   #ifdef DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("Validator::CreateAutoParameter() type='%s', name='%s', ownerName='%s', "
       "depName='%s', manage=%d\n", type.c_str(), name.c_str(),
       ownerName.c_str(), depName.c_str(), manage);
   #endif
   
   Parameter *param = NULL;
   
   // Check if create an array
   if (type == "Array")
      param = CreateArray(name, manage);
   else
      param = theModerator->CreateAutoParameter(type, name, alreadyManaged,
                                                ownerName, depName, manage);
   
   #ifdef DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("Validator::CreateAutoParameter() returning %s <%p><%s> '%s'\n",
       alreadyManaged ? "old" : "new", param,
       (param == NULL) ? "NULL" : param->GetTypeName().c_str(),
       (param == NULL) ? "NULL" : param->GetName().c_str());
   #endif
   
   return param;
}


//------------------------------------------------------------------------------
// Parameter* CreateParameter(const std::string &type, const std::string &name,
//                            const std::string &ownerName, const std::string &depName
//                            Integer manage = 1)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to create a Parameter. If object is not managed this
 * method does not check for existing Parameter before creating one since
 * Moderator::CreateParameter() sets Parameter reference objects if Parameter
 * was created without reference during GmatFunction parsing.
 * 
 * @param  type       Type of parameter requested
 * @param  name       Name for the parameter.
 * @param  ownerName  object name of parameter requested ("")
 * @param  depName    Dependent object name of parameter requested ("")
 * @param  manage     0, if parameter is not managed
 *                    1, if parameter is added to configuration (default)
 *                    2, if Parameter is added to function object map
 * 
 * @return Pointer to the constructed Parameter.
 */
//------------------------------------------------------------------------------
Parameter* Validator::CreateParameter(const std::string &type, 
                                      const std::string &name,
                                      const std::string &ownerName,
                                      const std::string &depName,
                                      Integer manage)
{
   #ifdef DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("Validator::CreateParameter() type='%s', name='%s', ownerName='%s', "
       "depName='%s', manage=%d\n", type.c_str(), name.c_str(),
       ownerName.c_str(), depName.c_str(), manage);
   #endif
   
   Parameter *param = NULL;
   
   // Check if create an array
   if (type == "Array")
      param = CreateArray(name, manage);
   else
      param = theModerator->CreateParameter(type, name, ownerName, depName, manage);
   
   #ifdef DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("Validator::CreateParameter() returning new <%p><%s> '%s'\n", param,
       (param == NULL) ? "NULL" : param->GetTypeName().c_str(),
       (param == NULL) ? "NULL" : param->GetName().c_str());
   #endif
   
   return param;
}


//------------------------------------------------------------------------------
// Parameter* CreateArray(onst std::string &arrayStr, Integer manage = 1)
//------------------------------------------------------------------------------
Parameter* Validator::CreateArray(const std::string &arrayStr, Integer manage)
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
   #ifdef DEBUG_AXIS_SYSTEM
   MessageInterface::ShowMessage
      ("Validator::CreateAxisSystem() type = '%s', owner='%s'\n",
       type.c_str(), owner->GetName().c_str());
   #endif
   
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
   
   AxisSystem *axis = NULL;
   
   // Clone the axis if it is not NULL and has the same type(LOJ: 2009.10.06)
   // So primary and secondary names can be copied
   // This will fix bug 1386 (using ObjectReferenced CoordinateSystem inside a function)
   // Before this fix: we used to get
   // CoordinateSystem exception: Primary "" is not yet set in object referenced!
   
   // Get AxisSystem from the CoordinateSystem
   AxisSystem *ownedAxis = (AxisSystem *)(owner->GetRefObject(Gmat::AXIS_SYSTEM, ""));
   if (ownedAxis != NULL)
   {
      #ifdef DEBUG_AXIS_SYSTEM
      MessageInterface::ShowMessage
         ("   ownedAxis=<%p><%s>'%s', usingPrimary=%d\n", ownedAxis,
          ownedAxis->GetTypeName().c_str(), 
          ownedAxis->GetName().c_str(), ownedAxis->UsesPrimary());
      #endif
      
      if (type == ownedAxis->GetTypeName())
      {
         axis = (AxisSystem *)(ownedAxis->Clone());
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (axis, axis->GetName(), "Validator::CreateAxisSystem()",
             "axis = (AxisSystem *)(ownedAxis->Clone()");
         #endif
      }
      else
         axis = (AxisSystem *)(theModerator->CreateAxisSystem(type, ""));
   }
   else
   {
      axis = (AxisSystem *)(theModerator->CreateAxisSystem(type, ""));
   }
   
   #ifdef DEBUG_AXIS_SYSTEM
   MessageInterface::ShowMessage
      ("Validator::CreateAxisSystem() returning <%p>\n", axis);
   #endif
   
   return axis;
}


//------------------------------------------------------------------------------
// ElementWrapper* CreateValidWrapperWithDot(GmatBase *obj, ...)
//------------------------------------------------------------------------------
ElementWrapper* Validator::CreateValidWrapperWithDot(GmatBase *obj,
                                                     const std::string &type,
                                                     const std::string &owner,
                                                     const std::string &depobj,
                                                     bool parametersFirst,
                                                     Integer manage)
{
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("Validator::CreateValidWrapperWithDot() entered, obj=<%p><%s>, "
       "parametersFirst=%d, manage=%d\n", obj, obj ? obj->GetName().c_str() : "NULL",
       parametersFirst, manage);
   #endif
   
   Gmat::WrapperDataType itsType = Gmat::NUMBER_WT;
   ElementWrapper *ew = NULL;
   
   // if there are two dots, then treat it as a Parameter
   // e.g. Sat.Thruster1.K1
   if (GmatStringUtil::NumberOfOccurrences(theDescription, '.') > 1)
   {
      // see if reallay create a ParameterWrapper first, there are a few exceptions.
      bool paramFirst = true;
      if (parametersFirst)
      {
         if (obj == NULL)
            paramFirst = true;
         else if (obj->IsOfType(Gmat::BURN))
            paramFirst = false;
      }
      else
      {
         if (theFunction != NULL)
            paramFirst = false;
      }
      
      if (paramFirst)
      {
         bool paramCreated = false;
         Parameter *param = CreateSystemParameter(paramCreated, theDescription, manage);         
         
         // param is not NULL if only one Parameter is created,
         // so create ParameterWrapper
         if (param)
         {
            #if DBGLVL_WRAPPERS > 1
            MessageInterface::ShowMessage
               ("In Validator(1), about to create a ParameterWrapper for \"%s\"\n",
                theDescription.c_str(), "\"\n");
            #endif
            ew = CreateParameterWrapper(param, itsType);
         }
      }
   }
   
   if (ew == NULL && parametersFirst)
   {
      bool paramCreated = false;
      Parameter *param = CreateSystemParameter(paramCreated, theDescription, manage);
      
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("   CreateSystemParameter() returned param=<%p>, paramCreated=%d\n", param,
          paramCreated);
      #endif
      
      if (param)
      {
         #if DBGLVL_WRAPPERS > 1
         MessageInterface::ShowMessage
            ("In Validator(2), about to create a ParameterWrapper for \"%s\"\n",
             theDescription.c_str(), "\"\n");
         #endif
         ew = CreateParameterWrapper(param, itsType);
      }
   }
   
   if (ew == NULL)
   {
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("In Validator(3), about to create a ObjectPropertyWrapper for \"%s\"\n",
          theDescription.c_str(), "\"\n");
      #endif
      ew = CreatePropertyWrapper(obj, type, manage);
   }
   
   if (ew == NULL)
   {
      bool paramCreated = false;
      Parameter *param = CreateSystemParameter(paramCreated, theDescription, manage);
      
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("   CreateSystemParameter() returned param=<%p>, paramCreated=%d\n", param,
          paramCreated);
      #endif
      
      // param is not NULL if only one Parameter is created,
      // so create ParameterWrapper
      if (param)
      {
         #if DBGLVL_WRAPPERS > 1
         MessageInterface::ShowMessage
            ("In Validator(4), about to create a ParameterWrapper for \"%s\"\n",
             theDescription.c_str(), "\"\n");
         #endif
         ew = CreateParameterWrapper(param, itsType);
      }
      else if (paramCreated)
      {
         // Multiple automatic objects are already created in CreateSystemParameter(),
         // so create StringWrapper if Parameters were created.
         // ReportFile.Add = {sat1.A1ModJulian, sat1.EarthMJ2000Eq.X};
         #if DBGLVL_WRAPPERS > 1
         MessageInterface::ShowMessage
            ("In Validator(4), about to create a StringWrapper for \"%s\"\n",
             theDescription.c_str(), "\"\n");
         #endif
         ew = new StringWrapper();
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (ew, theDescription, "Validator::CreateValidWrapperWithDot()",
             "ew = new StringWrapper()");
         #endif
         ew->SetDescription(theDescription);
         itsType = Gmat::STRING_WT;
      }
   }
   
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("Validator::CreateValidWrapperWithDot() returning <%p>\n", ew);
   #endif
   
   return ew;
}


//------------------------------------------------------------------------------
// ElementWrapper* CreateParameterWrapper(Parameter *param,
//                                        Gmat::WrapperDataType &itsType)
//------------------------------------------------------------------------------
/*
 * Creates ParameterWrapper.
 *
 * @param <param> Pointer to Paramter
 * @param <itsType> outpout wrapper type
 */
//------------------------------------------------------------------------------
ElementWrapper* Validator::CreateParameterWrapper(Parameter *param,
                                                  Gmat::WrapperDataType &itsType)
{
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("Validator::CreateParameterWrapper() entered, param=<%p>'%s', theDescription='%s'\n",
       param, param ? param->GetName().c_str() : "NULL", theDescription.c_str());
   #endif
   
   ElementWrapper *ew = NULL;
   
   // @note
   // Since GmatFunction can have such as "GMAT XYPlot.Add = {sat.X};",
   // we want to set Parameter name to description. (loj: 2008.06.19)
   
   if (param->IsOfType(Gmat::STRING))
   {
      ew = new StringObjectWrapper();
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (ew, theDescription, "Validator::CreateParameterWrapper()",
          "ew = new StringObjectWrapper()");
      #endif
      ew->SetDescription(theDescription);
      ew->SetRefObject(param);
      itsType = Gmat::STRING_OBJECT_WT;
      
      #if DBGLVL_WRAPPERS
      MessageInterface::ShowMessage
         (">>> In Validator, it's a String so created a StringObjectWrapper <%p> "
          "for \"%s\"\n", ew, param->GetName().c_str(), "\"\n");
      #endif
   }
   else
   {
      ew = new ParameterWrapper();
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (ew, theDescription, "Validator::CreateParameterWrapper()",
          "ew = new ParameterWrapper()");
      #endif
      // We want to set Parameter exact name to wrapper so that it can be found
      // from the object map (loj: 2008.06.30)
      // For example, "xyplot.IndVar = { Sat.TAIModJulian };"
      // the description is "{ Sat.TAIModJulian }" which will not work in GmatFunction
      //ew->SetDescription(theDescription);
      ew->SetDescription(param->GetName());
      ew->SetRefObject(param);
      itsType = Gmat::PARAMETER_WT;
      
      #if DBGLVL_WRAPPERS
      MessageInterface::ShowMessage
         (">>> In Validator, created a ParameterWrapper <%p> for Parameter \"%s\"\n",
          ew, param->GetName().c_str());
      #endif
   }
   
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("Validator::CreateParameterWrapper() returning <%p>\n", ew);
   #endif
   
   return ew;
}


//------------------------------------------------------------------------------
// ElementWrapper* CreatePropertyWrapper()
//------------------------------------------------------------------------------
/*
 * Creates ElementWrapper for object property.
 */
//------------------------------------------------------------------------------
ElementWrapper* Validator::CreatePropertyWrapper(GmatBase *obj,
                                                 const std::string &type,
                                                 Integer manage, bool checkSubProp)
{
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("Validator::CreatePropertyWrapper() entered, obj=<%p><%s><%s>, manage=%d, "
       "checkSubProp=%d\n", obj, obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL", manage, checkSubProp);
   #endif
   
   if (obj == NULL)
   {
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("Validator::CreatePropertyWrapper() returning NULL, input object is NULL,\n");
      #endif
      
      return NULL;
   }
   
   ElementWrapper *ew = NULL;
   
   // check if there is such property for that object
   try
   { 
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("   calling %s %s->GetParameterID(%s)\n", obj->GetTypeName().c_str(),
          obj->GetName().c_str(), type.c_str());
      #endif
      
      obj->GetParameterID(type);
      ew = new ObjectPropertyWrapper();
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (ew, theDescription, "Validator::CreateParameterWrapper()",
          "ew = new ObjectPropertyWrapper()");
      #endif
      ew->SetDescription(theDescription);
      
      #if DBGLVL_WRAPPERS
      MessageInterface::ShowMessage
         (">>> In Validator, created a ObjectPropertyWrapper <%p> for property \"%s\"\n    "
          "of <%s>'%s', theDescription='%s'\n", ew, type.c_str(), obj->GetTypeName().c_str(),
          obj->GetName().c_str(), theDescription.c_str());
      #endif
      
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("   Setting the object <%p><%s> to ObjectPropertyWrapper\n", obj,
          obj->GetName().c_str());
      #endif
      
      ew->SetRefObject(obj);
      
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("Validator::CreatePropertyWrapper() returning <%p>\n", ew);
      #endif
      
      return ew;
   }
   catch (BaseException &)
   {
      ; // do nothing here
   }
   
   // if not checking owend object property
   if (!checkSubProp)
   {
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("Validator::CreatePropertyWrapper() returning <%p>\n", ew);
      #endif
      
      return ew;
   }
   
   // create wrapper for owned object property
   ew = CreateSubPropertyWrapper(obj, type, manage);
   
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("Validator::CreatePropertyWrapper() returning <%p>\n", ew);
   #endif
   
   return ew;
}


//------------------------------------------------------------------------------
// ElementWrapper* CreateSubPropertyWrapper()
//------------------------------------------------------------------------------
/*
 * Creates ElementWrapper for object property.
 */
//------------------------------------------------------------------------------
ElementWrapper* Validator::CreateSubPropertyWrapper(GmatBase *obj,
                                                    const std::string &type,
                                                    Integer manage)
{
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("Validator::CreateSubPropertyWrapper() entered, obj=<%p><%s><%s>, manage=%d\n",
       obj, obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL", manage);
   #endif
   
   if (obj == NULL)
   {
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("Validator::CreateSubPropertyWrapper() returning NULL, input object is NULL,\n");
      #endif
      
      return NULL;
   }
   
   ElementWrapper *ew = NULL;
   
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("   Object can have owend objects, so check owend object property\n");
   #endif
   
   Integer ownedId = -1;
   Gmat::ParameterType owendType;
   GmatBase *ownedObj = NULL;
   
   if (theInterpreter)
   {
      theInterpreter->FindPropertyID(obj, theDescription, &ownedObj,
                                     ownedId, owendType);
      
      #if DBGLVL_WRAPPERS > 1
      MessageInterface::ShowMessage
         ("   ownedObj=<%p>, ownedId=%d, owendType=%d\n",
          ownedObj, ownedId, owendType);
      #endif
      
      if (ownedId != -1)
      {
         ew = new ObjectPropertyWrapper();
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (ew, theDescription, "Validator::CreateSubPropertyWrapper()",
             "ew = new ObjectPropertyWrapper()");
         #endif
         ew->SetDescription(theDescription);
         
         #if DBGLVL_WRAPPERS
         MessageInterface::ShowMessage
            (">>> In Validator, created a ObjectPropertyWrapper <%p> for owned "
             "property \"%s\"\n    of <%s>'%s', theDescription='%s'\n", ew,
             type.c_str(), ownedObj->GetTypeName().c_str(),
             ownedObj->GetName().c_str(), theDescription.c_str());
         #endif
         
         ew->SetRefObjectName(ownedObj->GetName(), 0);
         ew->SetRefObject(ownedObj);
         
         // Removed to address bug 2203
//         // @note
//         // Handle special case for GmatFunction(loj: 2008.07.07)
//         // AxisSystem of CoordinateSystem, or Propagator of PropSetup are
//         // created as local objects, but name is added automatically to
//         // refObjectNames in SetupWrapper(), so we need to clear.
//         // GMAT CS.Axes = ObjectReferenced;
//         // GMAT Prop.Type = BulirschStoer;
//         if ((obj->IsOfType(Gmat::COORDINATE_SYSTEM) &&
//              ownedObj->IsOfType(Gmat::AXIS_SYSTEM)) ||
//             (obj->IsOfType(Gmat::PROP_SETUP) &&
//              ownedObj->IsOfType(Gmat::PROPAGATOR)))
//            ew->ClearRefObjectNames();
      }
      else
      {
         theErrorMsg = "\"" + theDescription + "\"" + 
            " does not have a valid field of object or owned object";
         HandleError();
      }
   }
   
   #if DBGLVL_WRAPPERS > 1
   MessageInterface::ShowMessage
      ("Validator::CreateSubPropertyWrapper() returning <%p>\n", ew);
   #endif
   
   return ew;
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
   #ifdef DEBUG_CHECK_OBJECT
   MessageInterface::ShowMessage
      ("Validator::ValidateParameter() entered. There are %d ref objects\n",
       refNames.size());
   #endif
   bool retval = true;
   
   for (UnsignedInt j=0; j<refNames.size(); j++)
   {
      if (FindObject(refNames[j]) == NULL)
      {
         std::string type, ownerName, depObj;
         GmatStringUtil::ParseParameter(refNames[j], type, ownerName, depObj);
         
         #ifdef DEBUG_CHECK_OBJECT
         MessageInterface::ShowMessage
            ("   refName='%s', type='%s', owner='%s', dep='%s'\n", refNames[j].c_str(),
             type.c_str(), ownerName.c_str(), depObj.c_str());
         #endif
         
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
   
   #ifdef DEBUG_CHECK_OBJECT
   MessageInterface::ShowMessage
      ("Validator::ValidateParameter() returning %d\n", retval);
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// bool ValidateSaveCommand(GmatBase *obj)
//------------------------------------------------------------------------------
bool Validator::ValidateSaveCommand(GmatBase *obj)
{
   bool retval = true;
   
   ObjectTypeArray refTypes = obj->GetRefObjectTypeArray();
   
   #ifdef DEBUG_VALIDATE_COMMAND
   for (UnsignedInt i=0; i<refTypes.size(); i++)
      MessageInterface::ShowMessage
         ("   %s\n", GmatBase::GetObjectTypeString(refTypes[i]).c_str());
   #endif
   
   StringArray refNames = obj->GetRefObjectNameArray(Gmat::UNKNOWN_OBJECT);
   
   for (UnsignedInt j=0; j<refNames.size(); j++)
   {
      #ifdef DEBUG_VALIDATE_COMMAND
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
         // Correctely check array elements (loj: 2008.04.21)
         //if ( (!(refObj->IsOfType(Gmat::ARRAY))) ||
         //     (GmatStringUtil::IsParenPartOfArray(refNames[j])) )
         if ( refObj->IsOfType(Gmat::ARRAY) &&
              GmatStringUtil::IsParenPartOfArray(refNames[j]))
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
// bool ValidateSubCommand(GmatCommand *brCmd, Integer level, Integer manage = 1)
//------------------------------------------------------------------------------
bool Validator::ValidateSubCommand(GmatCommand *brCmd, Integer level, Integer manage)
{
   #ifdef DEBUG_VALIDATE_COMMAND
   MessageInterface::ShowMessage
      ("ValidateSubCommand() '%s', level=%d, manage=%d\n",
       brCmd->GetTypeName().c_str(), level, manage);
   #endif
   
   GmatCommand* current = brCmd;
   Integer childNo = 0;
   GmatCommand* nextInBranch;
   GmatCommand* child;
   std::string cmdstr;
   
   while((child = current->GetChildCommand(childNo)) != NULL)
   {
      nextInBranch = child;
      
      while ((nextInBranch != NULL) && (nextInBranch != current))
      {
         for (int i=0; i<=level; i++)
         {
            #ifdef DEBUG_VALIDATE_COMMAND
            MessageInterface::ShowMessage("---");
            #endif
         }
         
         cmdstr = nextInBranch->GetGeneratingString(Gmat::NO_COMMENTS) + "\n";
         
         #ifdef DEBUG_VALIDATE_COMMAND
         MessageInterface::ShowMessage("%s", cmdstr.c_str());
         #endif
         
         if (!ValidateCommand(nextInBranch, false, manage))
         {
            MessageInterface::ShowMessage
               ("==> ValidateCommand() returned false\n");
            return false;
         }
         
         if (!CheckUndefinedReference(nextInBranch))
         {
            MessageInterface::ShowMessage
               ("==> CheckUndefinedReference() returned false\n");
             return false;
         }
         
         if (nextInBranch->GetChildCommand() != NULL)
            if (!ValidateSubCommand(nextInBranch, level+1, manage))
            {
               MessageInterface::ShowMessage
                  ("==> ValidateSubCommand() returned false\n");
               return false;
            }
         
         nextInBranch = nextInBranch->GetNext();
      }
      
      ++childNo;
   }
   
   #ifdef DEBUG_VALIDATE_COMMAND
   MessageInterface::ShowMessage("ValidateSubCommand() returning true\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool CreateCoordSystemProperty(GmatBase *obj, const std::string &prop)
//------------------------------------------------------------------------------
bool Validator::CreateCoordSystemProperty(GmatBase *obj, const std::string &prop,
                                          const std::string &value)
{
   #ifdef DEBUG_COORD_SYS_PROP
   MessageInterface::ShowMessage
      ("Validator::CreateCoordSystemProperty() obj=<%p>'%s', prop='%s', value='%s'\n",
       obj, obj->GetName().c_str(), prop.c_str(), value.c_str());
   #endif
   
   AxisSystem *axis = CreateAxisSystem(value, obj);
   if (axis == NULL)
      return false;
   
   #ifdef DEBUG_COORD_SYS_PROP
   MessageInterface::ShowMessage
      ("Validator::CreateCoordSystemProperty() AxisSystem '%s' created\n", value.c_str());
   MessageInterface::ShowMessage("   Setting CS ref objects and Initializing...\n");
   #endif
   
   // Set AxisSystem to CoordinateSystem
   obj->SetRefObject(axis, axis->GetType(), axis->GetName());
   
   // Since CoordinateSystem clones AxisSystem, delete it from here
   // (LOJ: 2009.03.03)
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Remove
      (axis, "localAxes", "Validator::CreateAxisSystem()", "deleting localAxes");
   #endif
   delete axis;
   
   StringArray refNameList = obj->GetRefObjectNameArray(Gmat::SPACE_POINT);
   for (UnsignedInt j = 0; j < refNameList.size(); j++)
   {
      #ifdef DEBUG_COORD_SYS_PROP
      MessageInterface::ShowMessage
         ("   refNameList[%d]=%s\n", j, refNameList[j].c_str());
      #endif
      
      GmatBase *refObj = FindObject(refNameList[j]);
      if ((refObj == NULL) || !(refObj->IsOfType(Gmat::SPACE_POINT)))
      {            
         theErrorMsg = "Nonexistent SpacePoint \"" + refNameList[j] +
            "\" referenced in \"" + obj->GetName() + "\"";
         return HandleError();
      }
      else
      {
         obj->SetRefObject(refObj, Gmat::SPACE_POINT, refObj->GetName());
      }
   }
   
   obj->Initialize();
   
   #ifdef DEBUG_COORD_SYS_PROP
   MessageInterface::ShowMessage
      ("Validator::CreateCoordSystemProperty() returning true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool CreatePropSetupProperty(GmatBase *obj, const std::string &prop)
//------------------------------------------------------------------------------
bool Validator::CreatePropSetupProperty(GmatBase *obj, const std::string &prop,
                                        const std::string &value)
{
   #ifdef DEBUG_PROP_SETUP_PROP
   MessageInterface::ShowMessage
      ("Validator::CreatePropSetupProperty() obj=<%p>'%s', prop='%s', value='%s'\n",
       obj, obj->GetName().c_str(), prop.c_str(), value.c_str());
   #endif
   
   if (obj == NULL)
      return false;
   
   if (obj->GetType() != Gmat::PROP_SETUP)
   {
      theErrorMsg = "Validator::CreatePropSetupProperty needs a "
         "PropSetup object that acts as its owner; received a pointer "
         "to " + obj->GetName() + "instead.";
      HandleError();
      return false;
   }
   
   GmatBase *propagator = (GmatBase*)theModerator->CreatePropagator(value, "");
   propagator->SetName(value);
   
   #ifdef DEBUG_PROP_SETUP_PROP
   MessageInterface::ShowMessage
      ("   Created a Propagator <%p> of type '%s'\n", obj, value.c_str());
   #endif
   
   obj->SetRefObject(propagator, propagator->GetType(), propagator->GetName());
   
   // Since PropSetup::SetRefObject() clones the propagator,
   // we need to delete original here (LOJ: 2009.03.18)
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Remove
      (propagator, "value", "Validator::CreatePropSetupProperty()",
       "deleting unnamed propagator");
   #endif
   delete propagator;
   
   #ifdef DEBUG_PROP_SETUP_PROP
   MessageInterface::ShowMessage
      ("Validator::CreatePropSetupProperty() returning true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool CreateForceModelProperty(GmatBase *obj, const std::string &prop)
//------------------------------------------------------------------------------
bool Validator::CreateForceModelProperty(GmatBase *obj, const std::string &prop,
                                         const std::string &value)
{
   #ifdef DEBUG_FORCE_MODEL_PROP
   MessageInterface::ShowMessage
      ("Validator::CreateForceModelProperty() obj=<%p>'%s', prop='%s', value='%s'\n",
       obj, obj->GetName().c_str(), prop.c_str(), value.c_str());
   #endif
   
   if (theInterpreter)
      return theInterpreter->SetForceModelProperty(obj, prop, value, NULL);
   else
      return false;
}


//------------------------------------------------------------------------------
// bool HandleError(bool addFunction)
//------------------------------------------------------------------------------
/*
 * Handles error condition dependends on the continue on error flag.
 * It uses the data member theErrorMsg and continueOnError.
 *
 */
//------------------------------------------------------------------------------
bool Validator::HandleError(bool addFunction)
{
   #ifdef DEBUG_HANDLE_ERROR
   MessageInterface::ShowMessage
      ("Validator::HandleError() continueOnError=%d, TheErrorMsg=\n   **** ERROR **** %s\n\n",
       continueOnError, theErrorMsg.c_str());
   #endif
   
   if (continueOnError)
   {
      theErrorList.push_back(theErrorMsg);
      return false;
   }
   else
   {
      std::string fnMsg;
      if (addFunction)
      {
         if (theFunction != NULL)
         {
            fnMsg = theFunction->GetFunctionPathAndName();
            fnMsg = "\n(In Function \"" + fnMsg + "\")\n";
         }
      }
      theErrorMsg = theErrorMsg + fnMsg;
      
      InterpreterException ex(theErrorMsg);
      throw ex;
   }
}


//------------------------------------------------------------------------------
// void ShowObjectMap(const std::string &label = "")
//------------------------------------------------------------------------------
void Validator::ShowObjectMap(const std::string &label)
{
   if (theObjectMap != NULL)
   {
      MessageInterface::ShowMessage
         ("=====%s, Here is the object map in use <%p>, it has %d objects\n",
          label.c_str(), theObjectMap, theObjectMap->size());
      for (std::map<std::string, GmatBase *>::iterator i = theObjectMap->begin();
           i != theObjectMap->end(); ++i)
      {
         MessageInterface::ShowMessage
            ("   %30s  <%p><%s>'%s'\n", i->first.c_str(), i->second,
             i->second == NULL ? "NULL" : (i->second)->GetTypeName().c_str(),
             i->second == NULL ? "NULL" : (i->second)->GetName().c_str());
      }
   }
   else
   {
      MessageInterface::ShowMessage("=====%s, theObjectMap is NULL\n");
   }
}


//------------------------------------------------------------------------------
// Validator()
//------------------------------------------------------------------------------
/*
 * Default constructor
 *
 * @param  ss  The solar system to be used for findnig bodies
 * @param  objMap  The object map to be used for finding object
 */
//------------------------------------------------------------------------------
Validator::Validator()
{
   theModerator = Moderator::Instance();
   StringArray parms = theModerator->GetListOfFactoryItems(Gmat::PARAMETER);
   copy(parms.begin(), parms.end(), back_inserter(theParameterList));
   
   theSolarSystem = NULL;
   theCommand = NULL;
   theFunction = NULL;
   theObjectMap = NULL;
   createDefaultStringWrapper = true;
   continueOnError = true;
}


//------------------------------------------------------------------------------
// ~Validator()
//------------------------------------------------------------------------------
Validator::~Validator()
{
}


