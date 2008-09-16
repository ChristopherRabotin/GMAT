//$Id$
//------------------------------------------------------------------------------
//                                For
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Joey Gurganus
// Created: 2004/01/29
// Modified: W. Shoan 2004.09.13 Updated for use in Build 3
//
/**
 * Definition for the For command class
 */
//------------------------------------------------------------------------------

#include <ctype.h>               // for isalpha
#include "gmatdefs.hpp"
#include "For.hpp"
#include "BranchCommand.hpp"
#include "CommandException.hpp"
#include "StringUtil.hpp"        // for ToReal()
#include "MessageInterface.hpp"

#include <sstream>      // for std::stringstream, used to make generating string

//#define DEBUG_FOR_EXE 1
//#define DEBUG_FOR_INIT 1
//#define DEBUG_FOR_REAL
//#define DEBUG_FOR_PARAMS

//---------------------------------
// static data
//---------------------------------
const std::string
For::PARAMETER_TEXT[ForParamCount - BranchCommandParamCount] =
{
   "StartValue",
   "EndValue",
   "Step", 
   "IndexName",
   "StartName",
   "EndName",
   "IncrementName",
   //"IndexIsParam",
   //"StartIsParam",
   //"EndIsParam",
   //"IncrIsparam"
};

const Gmat::ParameterType
For::PARAMETER_TYPE[ForParamCount - BranchCommandParamCount] =
{
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   //Gmat::BOOLEAN_TYPE,
   //Gmat::BOOLEAN_TYPE,
   //Gmat::BOOLEAN_TYPE,
   //Gmat::BOOLEAN_TYPE,
};

const Real For::UNINITIALIZED_VALUE = -999.99;
const Real For::DEFAULT_START       = 1;
const Real For::DEFAULT_END         = 10;
const Real For::DEFAULT_INCREMENT   = 1;

//------------------------------------------------------------------------------
//  For(void)
//------------------------------------------------------------------------------
/**
 * Creates a For command.  (default constructor)
 */
//------------------------------------------------------------------------------
For::For(void) :
   BranchCommand   ("For"),
   //forName         (""),
   startValue      (DEFAULT_START),
   endValue        (DEFAULT_END),
   stepSize        (DEFAULT_INCREMENT),
   currentValue    (UNINITIALIZED_VALUE),
   //indexParam      (NULL),
   //startParam      (NULL),
   //endParam        (NULL),
   //incrParam       (NULL),
   index           (NULL),
   start           (NULL),
   end             (NULL),
   increment       (NULL),
   indexName       ("I"),
   startName       ("1"),
   endName         ("10"),
   incrName        ("1")//,
   //indexIsParam    (false),
   //startIsParam    (false),
   //endIsParam      (false),
   //incrIsParam     (false)
{
   parameterCount = ForParamCount;
}

//------------------------------------------------------------------------------
//  For(const For& t)
//------------------------------------------------------------------------------
/**
* Constructor that replicates a for command.  (Copy constructor)
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
For::For(const For& f) :
   BranchCommand   (f),
   //forName         (f.forName),
   startValue      (f.startValue),
   endValue        (f.endValue),
   stepSize        (f.stepSize),
   currentValue    (f.currentValue),
   //indexParam      (NULL),
   //startParam      (NULL),
   //endParam        (NULL),
   //incrParam       (NULL),
   index           (NULL),
   start           (NULL),
   end             (NULL),
   increment       (NULL),
   indexName       (f.indexName),
   startName       (f.startName),
   endName         (f.endName),
   incrName        (f.incrName)//,
   //indexIsParam    (f.indexIsParam),
   //startIsParam    (f.startIsParam),
   //endIsParam      (f.endIsParam),
   //incrIsParam     (f.incrIsParam)
{
}


//------------------------------------------------------------------------------
//  ~For(void)
//------------------------------------------------------------------------------
/**
 * Destroys the for command.  (destructor)
 */
//------------------------------------------------------------------------------
For::~For(void)
{
   ClearWrappers();
}


//------------------------------------------------------------------------------
//  For& operator=(const For& f)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the for command.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
For& For::operator=(const For& f)
{
   if (this == &f)
     return *this;

   BranchCommand::operator=(f);
   //forName       = f.forName;
   startValue    = f.startValue;  // do I really want to do this?
   endValue      = f.endValue;
   stepSize      = f.stepSize;
   currentValue  = f.currentValue;
   //indexParam    = f.indexParam;
   //startParam    = f.startParam;
   //endParam      = f.endParam;
   //incrParam     = f.incrParam;
   index         = f.index;
   start         = f.start;
   end           = f.end;
   increment     = f.increment;
   indexName     = f.indexName;
   startName     = f.startName;
   endName       = f.endName;
   incrName      = f.incrName;
   //indexIsParam  = f.indexIsParam;
   //startIsParam  = f.startIsParam;
   //endIsParam    = f.endIsParam;
   //incrIsParam   = f.incrIsParam;
   return *this;
}


//------------------------------------------------------------------------------
//  bool Append(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Adds a command to the for loop.
 *
 * This method calls the BranchCommand base class method that adds a command
 * to the command sequence that branches off of the main mission sequence.  This
 * extension was needed so that the EndFor command can be set to point back 
 * to the head of the For loop.
 *
 * @return true if the Command is appended, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool For::Append(GmatCommand *cmd)
{
   if (!BranchCommand::Append(cmd))
     return false;

   // If at the end of a for branch, point that end back to this comand.
   if (cmd->GetTypeName() == "EndFor") 
   {
      if ((nestLevel== 0) && (branchToFill != -1))
      {
        cmd->Append(this);
        // for loop is complete; -1 points us back to the main sequence.
        branchToFill = -1;
      }
      else
      {
       --nestLevel;
      }
   }
   if (cmd->GetTypeName() == "For")
   {
      ++nestLevel;
   }

   return true;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the For loop.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool For::Initialize()
{
   #if DEBUG_FOR_INIT
   //MessageInterface::ShowMessage
   //   ("For::Initialize() indexIsParam=%d, startIsParam=%d, endIsParam=%d, "
   //    "incrIsParam=%d\n", indexIsParam, startIsParam, endIsParam, incrIsParam);
   #endif
   /*
   // Get Parameter pointers from the Sandbox
   if (indexIsParam)
   {
      indexParam = (Parameter*)FindObject(indexName);
      if (indexParam == NULL)
         throw CommandException
            ("Found undefined object \"" + indexName + "\" in line:\n   " +
             generatingString + "\n");
   }
   
   if (startIsParam)
   {
      startParam = (Parameter*)FindObject(startName);
      if (startParam == NULL)
         throw CommandException
            ("Found undefined object \"" + startName + "\" in line:\n   " +
             generatingString + "\n");
   }
   
   if (incrIsParam)
   {
      incrParam = (Parameter*)FindObject(incrName);
      if (incrParam == NULL)
         throw CommandException
            ("Found undefined object \"" + incrName + "\" in line:\n   " +
             generatingString + "\n");
   }
   
   if (endIsParam)
   {
      endParam = (Parameter*)FindObject(endName);
      if (endParam == NULL)
         throw CommandException
            ("Found undefined object \"" + endName + "\" in line:\n   " +
             generatingString + "\n");
   }
   */
   // Set references for the wrappers   
   #ifdef DEBUG_FOR_PARAMS
      MessageInterface::ShowMessage("Setting refs for index\n");
   #endif
   if (SetWrapperReferences(*index) == false)
      return false;
   CheckDataType(index, Gmat::REAL_TYPE, "For");
   #ifdef DEBUG_FOR_PARAMS
      MessageInterface::ShowMessage("Setting refs for start\n");
   #endif
   if (SetWrapperReferences(*start) == false)
      return false;
   CheckDataType(start, Gmat::REAL_TYPE, "For");
   #ifdef DEBUG_FOR_PARAMS
      MessageInterface::ShowMessage("Setting refs for end\n");
   #endif
   if (SetWrapperReferences(*end) == false)
      return false;
   CheckDataType(end, Gmat::REAL_TYPE, "For");
   #ifdef DEBUG_FOR_PARAMS
      MessageInterface::ShowMessage("Setting refs for increment\n");
   #endif
   if (SetWrapperReferences(*increment) == false)
      return false;
   CheckDataType(increment, Gmat::REAL_TYPE, "For");
   
   bool retval = BranchCommand::Initialize();
   
   return retval;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Execute this for loop.
 *
 *
 * @return true if the GmatCommand runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool For::Execute()
{
   #ifdef DEBUG_FOR_EXE
      MessageInterface::ShowMessage(
         "For::Execute() status: commandComplete = %s, "
         "commandExecuting = %s, branchExecuting = %s\n",
         ((commandComplete) ? "true" : "false"),
         ((commandExecuting) ? "true" : "false"),
         ((branchExecuting) ? "true" : "false") );
   #endif
   bool retval = true;

   if (branchExecuting)
   {
      #ifdef DEBUG_FOR_EXE
         MessageInterface::ShowMessage(
         "... branch still executing -> about to call ExecuteBranch\n");
      #endif
      retval = ExecuteBranch();
      if (!branchExecuting)
      {
         // Branch finished executing; update loop index
         currentValue += stepSize;
         //if (indexIsParam)   indexParam->SetReal(currentValue);
         index->SetReal(currentValue);
         #ifdef DEBUG_FOR_EXE
            MessageInterface::ShowMessage(
            "...... Branch done executing -> currentValue updated to %.4f\n",
            currentValue);
         #endif
      }
   }
   else 
   {
      if (!commandExecuting)
         BranchCommand::Execute();
   
      if (StillLooping())
      {
         branchExecuting = true;
         #ifdef DEBUG_FOR_EXE
            MessageInterface::ShowMessage(
            "... still looping -> about to call ExecuteBranch\n");
         #endif
         retval = ExecuteBranch();
      }
      else
      {
         commandComplete  = true;
         commandExecuting = false;
         currentValue     = UNINITIALIZED_VALUE;  // start the loop over
      }
   }
   
   BuildCommandSummary(true);
   return retval;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
/**
 * Resets the For command to an uninitialized state.
 */
//------------------------------------------------------------------------------
void For::RunComplete()
{
   currentValue = UNINITIALIZED_VALUE;

   BranchCommand::RunComplete();
}


//------------------------------------------------------------------------------
// Inherited from GmatBase
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the For.
 *
 * @return clone of the For.
 *
 */
//------------------------------------------------------------------------------
GmatBase* For::Clone(void) const
{
   return (new For(*this));
}

//------------------------------------------------------------------------------
//  const std::string& GetGeneratingString()
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * This method is used to retrieve the GmatCommand string from the script that
 * was parsed to build the GmatCommand.  It is used to save the script line, so
 * that the script can be written to a file without inverting the steps taken to
 * set up the internal object data.  As a side benefit, the script line is
 * available in the GmatCommand structure for debugging purposes.
 *
 * @param mode    Specifies the type of serialization requested.
 * @param prefix  Optional prefix appended to the object's name.  (Used to set
 *                indentation for GmatCommands.)
 * @param useName Name that replaces the object's name.  (Not used in
 *                commands.)
 *
 * @return The script line that, when interpreted, defines this For command.
 */
//------------------------------------------------------------------------------
const std::string& For::GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName)
{
   #ifdef DEBUG_FOR_REAL
      MessageInterface::ShowMessage("Entering For::GetGenStr\n");
      MessageInterface::ShowMessage("... startValue = %.12f\n", startValue);
      MessageInterface::ShowMessage("... stepSize   = %.12f\n", stepSize);
      MessageInterface::ShowMessage("... endValue   = %.12f\n", endValue);
   #endif
   std::stringstream gen;
   gen.precision(4);
   
   gen << indexName << " = ";
   gen << startName << ":";
   gen << incrName  << ":";
   gen << endName;
   
   if (mode == Gmat::NO_COMMENTS)
   {
      generatingString = "For " + gen.str() + ";";
      return generatingString;
   }
   
   generatingString = prefix + "For " + gen.str() + ";";

   // Then call the base class method
   return BranchCommand::GetGeneratingString(mode, prefix, useName);
}

//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const Gmat::ObjectType type,
//                         const std::string &name) const
//------------------------------------------------------------------------------
/**
 * This method returns a pointer to the Parameter ref object (used as
 * the index for the For command).
 *
 * @return pointer to the index Parmeter.
 *
 */
//------------------------------------------------------------------------------
GmatBase* For::GetRefObject(const Gmat::ObjectType type,
                            const std::string &name)
{
   /* OLD CODE
   switch (type)
   {
      case Gmat::PARAMETER:
      {
         if (name == indexName)      return indexParam;
         else if (name == startName) return startParam;
         else if (name == endName)   return endParam;
         else if (name == incrName)  return incrParam;
      }
      default:
      break;
   }
    */  
   // Not handled here -- invoke the next higher GetRefObject call
   return BranchCommand::GetRefObject(type, name);
}

//------------------------------------------------------------------------------
//  bool SetRefObject(const Gmat::ObjectType type,
//                    const std::string &name) const
//------------------------------------------------------------------------------
/**
 * This method sets a pointer to the Parameter ref object (used as
 * the index for the For command).
 *
 * @return success of the operation.
 *
 */
//------------------------------------------------------------------------------
bool For::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                       const std::string &name)
{
   /* OLD CODE
   switch (type)
   {
      case Gmat::PARAMETER:
      {
         if (name == indexName)
         {
            indexParam   = (Parameter*) obj;
            indexIsParam = true;
         }
         else if (name == startName)
         {
            startParam   = (Parameter*) obj;
            startIsParam = true;
         }
         else if (name == endName)
         {
            endParam   = (Parameter*) obj;
            endIsParam = true;
         }
         else if (name == incrName)
         {
            incrParam   = (Parameter*) obj;
            incrIsParam = true;
         }
         return true;
      }
      default:
         break;
   }
    */  
   // Not handled here -- invoke the next higher SetRefObject call
   return BranchCommand::SetRefObject(obj, type, name);
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/*
 * Renames referenced objects
 *
 * @param <type> type of the reference object.
 * @param <oldName> old name of the reference object.
 * @param <newName> new name of the reference object.
 *
 * @return always true to indicate RenameRefObject() was implemented.
 */
//---------------------------------------------------------------------------
bool For::RenameRefObject(const Gmat::ObjectType type,
                          const std::string &oldName,
                          const std::string &newName)
{
   /* OLD CODE
   if (type == Gmat::PARAMETER)
   {
      if (indexName == oldName)   indexName = newName;
      if (startName == oldName)   startName = newName;
      if (endName   == oldName)   endName   = newName;
      if (incrName  == oldName)   incrName  = newName;
   }
   */ 
   if (index) 
   {
      index->RenameObject(oldName, newName);
       indexName  = index->GetDescription();
   }
   if (start) 
   {
      start->RenameObject(oldName, newName);
       startName  = start->GetDescription();
   }
   if (end) 
   {
      end->RenameObject(oldName, newName);
       endName  = end->GetDescription();
   }
   if (increment) 
   {
      increment->RenameObject(oldName, newName);
       incrName  = increment->GetDescription();
   }
   BranchCommand::RenameRefObject(type, oldName, newName);
   
   return true;
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by the For.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& For::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   //refObjectTypes.push_back(Gmat::PARAMETER);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref objects used by the For.
 *
 * @param <type> The type of object desired, or Gmat::UNKNOWN_OBJECT for the
 *               full list.
 * 
 * @return the list of object names.
 * 
 */
//------------------------------------------------------------------------------
const StringArray& For::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   refObjectNames.clear();
   /* OLD CODE
   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::PARAMETER)
   {
      if (indexIsParam)
         refObjectNames.push_back(indexName);
      if (startIsParam)
         refObjectNames.push_back(startName);
      if (endIsParam)
         refObjectNames.push_back(endName);
      if (incrIsParam)
         refObjectNames.push_back(incrName);
   }
  */ 
   return refObjectNames;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string For::GetParameterText(const Integer id) const
{
   if ((id >= BranchCommandParamCount) && (id < ForParamCount))
      return PARAMETER_TEXT[id - BranchCommandParamCount];
   return BranchCommand::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer For::GetParameterID(const std::string &str) const
{
   for (Integer i = BranchCommandParamCount; i < ForParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - BranchCommandParamCount])
         return i;
   }
   
   return BranchCommand::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType For::GetParameterType(const Integer id) const
{
   if ((id >= BranchCommandParamCount) && (id < ForParamCount))
      return PARAMETER_TYPE[id - BranchCommandParamCount];
   
   return BranchCommand::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string For::GetParameterTypeString(const Integer id) const
{
   return BranchCommand::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 * @param id    Integer ID for the requested parameter
 */
//------------------------------------------------------------------------------
Real For::GetRealParameter(const Integer id) const
{
   if (id == START_VALUE)
   {
      #ifdef DEBUG_FOR_REAL
         MessageInterface::ShowMessage(
         "Entering For::GetReal with id = %d, ", id);
         MessageInterface::ShowMessage("And the value is : %.12f\n", startValue);
      #endif
      return startValue;
   }
   if (id == END_VALUE)
   {
      #ifdef DEBUG_FOR_REAL
         MessageInterface::ShowMessage(
         "Entering For::GetReal with id = %d, ", id);
         MessageInterface::ShowMessage("And the value is : %.12f\n", endValue);
      #endif
      return endValue;
   }
   if (id == STEP)
   {
      #ifdef DEBUG_FOR_REAL
         MessageInterface::ShowMessage(
         "Entering For::GetReal with id = %d, ", id);
         MessageInterface::ShowMessage("And the value is : %.12f\n", stepSize);
      #endif
      return stepSize;
   }
   
   return BranchCommand::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    id  Integer ID for the parameter
 * @param    val The new value for the parameter
 */
//------------------------------------------------------------------------------
Real For::SetRealParameter(const Integer id, const Real value)
{
   /* NOT ALLOWED
   if (id == START_VALUE)
   {
      startValue   = value;
      startIsParam = false;
      return true;
   }
   if (id == END_VALUE)
   {
      endValue     = value;
      endIsParam   = false;
      return true;
   }
   if (id == STEP)
   {
      stepSize     = value;
      incrIsParam  = false;
      return true;
   }
    */  
   return BranchCommand::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 * @param label    string ID for the requested parameter
 */
//------------------------------------------------------------------------------
Real For::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    label    string ID for the requested parameter
 * @param    value    The new value for the parameter
 */
//------------------------------------------------------------------------------
Real For::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param id  ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string For::GetStringParameter(const Integer id) const
{
   if (id == INDEX_NAME)          return indexName;
   else if (id == START_NAME)     return startName;
   else if (id == END_NAME)       return endName;
   else if (id == INCREMENT_NAME) return incrName;
   
   return BranchCommand::GetStringParameter(id);
}

//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
* This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param id     ID for the requested parameter.
 * @param value  string value for the requested parameter.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool For::SetStringParameter(const Integer id, const std::string &value)
{
   bool retVal = false;
   //Real rval;
   //bool isReal = false;
   //std::string name, rowStr, colStr;
   //Integer row, col;
   
   //if (GmatStringUtil::ToReal(value, rval))
   //   isReal = true;
   
   if (id == INDEX_NAME) 
   {
      indexName = value;
      retVal    = true;
      //return true;
   }
   else if (id == START_NAME)
   {
      #ifdef DEBUG_FOR_REAL
         MessageInterface::ShowMessage(
         "In For::SetStrP, setting startName to %s\n", value.c_str());
      #endif
      startName = value;
      //startParamName = value;
      //startIsParam = true;
      //if (isReal)
      //{
      //   startName = "";
      //   startValue = rval;
      //   startIsParam = false;
      //}
      //else
      //{
      //   GmatStringUtil::GetArrayIndex(startName, rowStr, colStr, row, col, name);
      //   if (rowStr != "-1" || colStr != "-1")
      //      startParamName = name;
      //}
      
      //MessageInterface::ShowMessage
      //   ("===> For::SetStringParameter() startName=%s, startParamName=%s, "
      //    "isReal=%d, startIsParam=%d\n", startName.c_str(), startParamName.c_str(),
      //    isReal, startIsParam);
      
      //return true;
      retVal = true;
   }
   else if (id == END_NAME)
   {
      #ifdef DEBUG_FOR_REAL
         MessageInterface::ShowMessage(
         "In For::SetStrP, setting endName to %s\n", value.c_str());
      #endif
      endName = value;
      //endParamName = value;
      //endIsParam = true;
      //if (isReal)
      //{
      //   endName = "";
      //   endValue = rval;
      //   endIsParam = false;
      //}
      //else
      //{
      //   GmatStringUtil::GetArrayIndex(startName, rowStr, colStr, row, col, name);
      //   if (rowStr != "-1" || colStr != "-1")
      //      endParamName = name;
      //}
      
      //return true;
      retVal = true;
   }
   else if (id == INCREMENT_NAME)
   {
      incrName = value;
      //incrParamName = value;
      //incrIsParam = true;
      //if (isReal)
      //{
      //   incrName = "";
      //   stepSize = rval;
      //   incrIsParam = false;
      //}
      //else
      //{
      //   GmatStringUtil::GetArrayIndex(incrName, rowStr, colStr, row, col, name);
      //   if (rowStr != "-1" || colStr != "-1")
      //      incrParamName = name;
      //}
      
      //MessageInterface::ShowMessage
      //   ("===> For::SetStringParameter() isReal=%d, incrIsParam=%d\n",
      //    isReal, incrIsParam);
      
      //return true;
      retVal = true;
   }
   if (retVal) return true;  // is this right?
   return BranchCommand::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* Accessor method used to get a parameter value
 *
 * @param    label  label ID for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
std::string For::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
* Accessor method used to get a parameter value
 *
 * @param    label  Integer ID for the parameter
 * @param    value  The new value for the parameter
 */
//------------------------------------------------------------------------------

bool For::SetStringParameter(const std::string &label,
                                        const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  bool  GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the bool parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  bool value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
/*
bool For::GetBooleanParameter(const Integer id) const
{
   if (id == INDEX_IS_PARAM)       return indexIsParam;
   if (id == START_IS_PARAM)       return startIsParam;
   if (id == END_IS_PARAM)         return endIsParam;
   if (id == INCR_IS_PARAM)        return incrIsParam;

   return BranchCommand::GetBooleanParameter(id);
}
*/
//------------------------------------------------------------------------------
//  bool  GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the bool parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 *
 * @return  bool value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
/*
bool For::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}
*/
const StringArray& For::GetWrapperObjectNameArray()
{
   wrapperObjectNames.clear();
   wrapperObjectNames.push_back(indexName);

   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       startName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(startName);
    
   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       endName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(endName);
    
   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       incrName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(incrName);
    
   return wrapperObjectNames;
}


bool For::SetElementWrapper(ElementWrapper *toWrapper, 
                            const std::string &withName)
{
   bool retval = false;

   if (toWrapper == NULL) return false;
   
   // this would be caught by next part, but this message is more meaningful
   if (toWrapper->GetWrapperType() == Gmat::ARRAY)
   {
      throw CommandException("A value of type \"Array\" on command \"" + typeName + 
                  "\" is not an allowed value.\nThe allowed values are:"
                  " [ Real Number, Variable, Array Element, or Parameter ]. "); 
   }

   bool typeOK = true;
   Gmat::ParameterType baseType;
   std::string         baseStr;
   try
   {
      baseType = toWrapper->GetDataType();
      baseStr  = PARAM_TYPE_STRING[baseType];
      if ( (baseType != Gmat::REAL_TYPE) && (baseType  != Gmat::INTEGER_TYPE) )
         typeOK = false;
   }
   catch (BaseException &be)
   {
      // just ignore it here - will need to check data type of object property 
      // wrappers on initialization
   }
   if (!typeOK)
   {
      throw CommandException("A value of \"" + withName + "\" of base type \"" +
                  baseStr + "\" on command \"" + typeName + 
                  "\" is not an allowed value.\nThe allowed values are:"
                  " [ Real Number, Variable, Array Element, or Parameter ]. "); 
   }
   
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage(
      "   Setting wrapper \"%s\" on Conditional Branch command\n", 
      withName.c_str());
   #endif
   if (indexName == withName)
   {
      if (toWrapper->GetWrapperType() != Gmat::VARIABLE)
      {
         std::string errmsg = "The value of \"" + indexName;
         errmsg            += "\" for field \"Index Name\" on command \"";
         errmsg            += GetTypeName() + "\" is not an allowed value.\n";
         errmsg            += "The allowed values are: [ Variable].";
         throw CommandException(errmsg);
      }
      index       = toWrapper;
      retval      = true;
   }
   if (startName == withName)
   {
      start       = toWrapper;
      retval      = true;
   }
   if (endName == withName)
   {
      end         = toWrapper;
      retval      = true;
   }
   if (incrName == withName)
   {
      increment   = toWrapper;
      retval      = true;
   }

   return retval;
}

void For::ClearWrappers()
{
   std::vector<ElementWrapper*> temp;
   if (index)
   {
      temp.push_back(index);
      index = NULL;
   }
   if (start)
   {
      if (find(temp.begin(), temp.end(), start) == temp.end())
      {
         temp.push_back(start);
         start = NULL;
      }
   }
   if (end)
   {
      if (find(temp.begin(), temp.end(), end) == temp.end())
      {
         temp.push_back(end);
         end = NULL;
      }
   }
   if (increment)
   {
      if (find(temp.begin(), temp.end(), increment) == temp.end())
      {
         temp.push_back(increment);
         increment = NULL;
      }
   }
   
   ElementWrapper *wrapper;
   for (UnsignedInt i = 0; i < temp.size(); ++i)
   {
      wrapper = temp[i];
      delete wrapper;
   }
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// bool StillLooping()
//------------------------------------------------------------------------------
/**
 * Method to determine whether or not we should still execute the loop
 *
 * @return    flag indicating whether or not we should still execute the loop
 *            (false if counter has passed the specified end value)
 */
//------------------------------------------------------------------------------
bool For::StillLooping()
{
   // initialize the loop, if it's the first time through
   if (currentValue == UNINITIALIZED_VALUE)
   {
      //if (startIsParam)  startValue = startParam->GetReal();
      //if (endIsParam)    endValue   = endParam->GetReal();
      //if (incrIsParam)   stepSize   = incrParam->GetReal();
      startValue = start->EvaluateReal();
      endValue   = end->EvaluateReal();
      stepSize   = increment->EvaluateReal();
      //Parameter *param = NULL;
      //Integer row, col;
      
      // Handle start Array element
      //if (startIsParam)
      //{
      //   if (startParam->GetTypeName() == "Array")
      //   {            
      //      param = GetArrayIndex(startName, row, col);
      //      Rmatrix rmat = startParam->GetRmatrix();
      //      startValue = rmat.GetElement(row, col);
      //   }
      //   else
      //   {
      //      startValue = startParam->GetReal();
      //   }
      //}
      
      //// Handle end Array element
      //if (endIsParam)
      //{
      //   if (endParam->GetTypeName() == "Array")
      //   {            
      //      param = GetArrayIndex(endName, row, col);
      //      Rmatrix rmat = endParam->GetRmatrix();
      //      endValue = rmat.GetElement(row, col);
      //   }
      //   else
      //   {
      //      endValue = endParam->GetReal();
      //   }
      //}
      
      //// Handle increment Array element
      //if (incrIsParam)
      //{
      //   if (incrParam->GetTypeName() == "Array")
      //   {            
      //      param = GetArrayIndex(incrName, row, col);     
      //      Rmatrix rmat = incrParam->GetRmatrix();
      //      stepSize = rmat.GetElement(row, col);
      //   }
      //   else
      //   {
      //      stepSize = incrParam->GetReal();
      //   }
      //}
      
      #if DEBUG_FOR_EXE
      MessageInterface::ShowMessage
         ("For::StillLooping() startValue=%f, stepSize=%f, endValue=%f\n",
          startValue, stepSize, endValue);
      #endif
      
      if ((stepSize == 0.0) ||
          ((stepSize > 0.0) && (startValue > endValue)) ||
          ((stepSize < 0.0) && (startValue < endValue)) )
         throw CommandException(
               "For loop values incorrect - will result in infinite loop "
               "in line:\n   \"" + generatingString + "\"\n");
      
      currentValue = startValue;
      
      //if (indexIsParam)
      //   indexParam->SetReal(currentValue);
      index->SetReal(currentValue);
      commandComplete  = false;      
   }
   if (((stepSize > 0.0) && (currentValue <= endValue)) ||
       ((stepSize < 0.0) && (currentValue >= endValue)) )
      return true;
   
   // not looping anymore
   return false;
}


//------------------------------------------------------------------------------
// Parameter* GetArrayIndex(const std::string &arrayStr, Integer &row, Integer &col)
//------------------------------------------------------------------------------
/**
 * Retrives array index from the configured array.
 *
 * @param  arrayStr  String form of array (A(1,3), B(2,j), etc)
 *
 * @note Array name must be created and configured before acces.
 */
//------------------------------------------------------------------------------
/* OLD CODE - not neede with Parameters in Commands
Parameter* For::GetArrayIndex(const std::string &arrayStr,
                              Integer &row, Integer &col)
{
   std::string name, rowStr, colStr;
   
   // parse array name and index
   GmatStringUtil::GetArrayIndex(arrayStr, rowStr, colStr, row, col, name);
   
   // Remove - sign from the name
   if (name[0] == '-')
      name = name.substr(1);
   
   #if DEBUG_ARRAY_GET
   MessageInterface::ShowMessage
      ("For::GetArrayIndex() arrayStr=%s, name=%s, rowStr=%s, "
       "colStr=%s, row=%d, col=%d\n", arrayStr.c_str(), name.c_str(),
       rowStr.c_str(), colStr.c_str(), row, col);
   #endif
   
   GmatBase *obj = FindObject(name);
   
   if (obj == NULL)
   {
      throw CommandException
         ("For loop cannot find Array named \"" + name + "\" "
          "in line:\n   \"" + generatingString + "\"\n");
   }
   
   if (obj->GetTypeName() != "Array")
   {
      throw CommandException
         ("For loop cannot find Array index with \"" + name + "\" "
          "in line:\n   \"" + generatingString + "\"\n");
   }
   
   // get row value
   if (row == -1 && rowStr != "-1")
   {
      Parameter *param = (Parameter*)FindObject(rowStr);
      if (param == NULL)
      {
         throw CommandException
            ("For loop cannot find Array row index variable \"" + name + "\" "
             "in line:\n   \"" + generatingString + "\"\n");
      }
      
      if (param->GetReturnType() == Gmat::REAL_TYPE)
         row = (Integer)param->GetReal() - 1; // index starts at 0
      else
         if (param == NULL)
         {
            throw CommandException
               ("For loop cannot handle row index of Array \"" + name + "\" "
                "in line:\n   \"" + generatingString + "\"\n");
         }
   }
      
   // get column value
   if (col == -1 && colStr != "-1")
   {
      Parameter *param = (Parameter*)FindObject(colStr);
      if (FindObject(colStr) == NULL)
      {
         throw CommandException
            ("For loop cannot find Array column index variable \"" + name + "\" "
             "in line:\n   \"" + generatingString + "\"\n");
      }
      
      if (param->GetReturnType() == Gmat::REAL_TYPE)
         col = (Integer)param->GetReal() - 1; // index starts at 0
      else
         if (param == NULL)
         {
            throw CommandException
               ("For loop cannot handle column index of Array \"" + name + "\" "
                "in line:\n   \"" + generatingString + "\"\n");
         }
   }
   
   if (row == -1)
   {
      throw CommandException("For loop row index is invalid "
                             "in line:\n   \"" + generatingString + "\"\n");
   }
   
   if (col == -1)
   {
      throw CommandException("For loop column index is invalid "
                             "in line:\n   \"" + generatingString + "\"\n");
   }
   
   #if DEBUG_ARRAY_GET
   MessageInterface::ShowMessage
      ("   GetArrayIndex() row=%d, col=%d\n", row, col);
   #endif
   
   return (Parameter*)obj;
}
*/
