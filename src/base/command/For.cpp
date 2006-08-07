//$Header$
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

#include <ctype.h>     // for isalpha
#include "For.hpp"
#include "BranchCommand.hpp"
#include "CommandException.hpp"
#include "MessageInterface.hpp"

#include <sstream>      // for std::stringstream, used to make generating string

//#define DEBUG_FOR 1

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
   "IndexIsParam",
   "StartIsParam",
   "EndIsParam",
   "IncrIsparam"
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
   Gmat::BOOLEAN_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::BOOLEAN_TYPE,
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
   forName         (""),
   startValue      (DEFAULT_START),
   endValue        (DEFAULT_END),
   stepSize        (DEFAULT_INCREMENT),
   currentValue    (UNINITIALIZED_VALUE),
   indexParam      (NULL),
   startParam      (NULL),
   endParam        (NULL),
   incrParam       (NULL),
   indexName       (""),
   startName       (""),
   endName         (""),
   incrName        (""),
   indexIsParam    (false),
   startIsParam    (false),
   endIsParam      (false),
   incrIsParam     (false)
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
forName         (f.forName),
startValue      (f.startValue),
endValue        (f.endValue),
stepSize        (f.stepSize),
currentValue    (f.currentValue),
indexParam      (NULL),
startParam      (NULL),
endParam        (NULL),
incrParam       (NULL),
indexName       (f.indexName),
startName       (f.startName),
endName         (f.endName),
incrName        (f.incrName),
indexIsParam    (f.indexIsParam),
startIsParam    (f.startIsParam),
endIsParam      (f.endIsParam),
incrIsParam     (f.incrIsParam)
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
   forName       = f.forName;
   startValue    = f.startValue;  // do I really want to do this?
   endValue      = f.endValue;
   stepSize      = f.stepSize;
   currentValue  = f.currentValue;
   indexParam    = f.indexParam;
   startParam    = f.startParam;
   endParam      = f.endParam;
   incrParam     = f.incrParam;
   indexName     = f.indexName;
   startName     = f.startName;
   endName       = f.endName;
   incrName      = f.incrName;
   indexIsParam  = f.indexIsParam;
   startIsParam  = f.startIsParam;
   endIsParam    = f.endIsParam;
   incrIsParam   = f.incrIsParam;
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
//  bool Initialize(void)
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the For loop.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool For::Initialize(void)
{
   // Get Parameter pointers from the Sandbox
   if (indexIsParam)
   {
      if (objectMap->find(indexName) == objectMap->end())
         throw CommandException("Undefined start variable:" + indexName +
                                " in For loop\n" + generatingString);
      
      indexParam = (Parameter*)(*objectMap)[indexName];
   }

   if (startIsParam)
   {
      if (objectMap->find(startName) == objectMap->end())
         throw CommandException("Undefined start variable:" + startName +
                                " in For loop\n" + generatingString);
      
      startParam = (Parameter*)(*objectMap)[startName];
   }

   if (endIsParam)
   {
      if (objectMap->find(endName) == objectMap->end())
         throw CommandException("Undefined stop variable:" + endName +
                                " in For loop\n" + generatingString);
      
      endParam = (Parameter*)(*objectMap)[endName];
   }
   
   if (incrIsParam)
   {
      if (objectMap->find(incrName) == objectMap->end())
         throw CommandException("Undefined increment variable:" + incrName +
                                " in For loop\n" + generatingString);
      
      incrParam = (Parameter*)(*objectMap)[incrName];
   }

   bool retval = BranchCommand::Initialize();

   return retval;
}


//------------------------------------------------------------------------------
//  bool Execute(void)
//------------------------------------------------------------------------------
/**
 * Execute this for loop.
 *
 *
 * @return true if the GmatCommand runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool For::Execute(void)
{
   bool retval = true;

   if (branchExecuting)
   {
      retval = ExecuteBranch();
      if (!branchExecuting)
      {
         // Branch finished executing; update loop index
         currentValue += stepSize;
         if (indexIsParam)   indexParam->SetReal(currentValue);
      }
   }
   else 
   {
      if (!commandExecuting)
         BranchCommand::Execute();
   
      if (StillLooping())
      {
         branchExecuting = true;
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
   std::stringstream gen;

   if (indexName != "")
      gen << indexName;
   else
      gen << forName;
   gen << " = ";
   
   if (startName != "")
      gen << startName;
   else
      gen << startValue;
   gen << ":";

   if (incrName != "")
   {
      gen << incrName;
      gen << ":";
   }
   else if (stepSize != 1.0)
   {
      gen << stepSize;
      gen << ":";
   }

   if (endName != "")
      gen << endName;
   else
      gen << endValue;

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
   
   switch (type)
   {
      case Gmat::PARAMETER:
      {
         if (name == indexName)
         {
            indexParam   = (Parameter*) obj;
            indexIsParam = true;
         }
         if (name == startName)
         {
            startParam   = (Parameter*) obj;
            startIsParam = true;
         }
         if (name == endName)
         {
            endParam   = (Parameter*) obj;
            endIsParam = true;
         }
         if (name == incrName)
         {
            incrParam   = (Parameter*) obj;
            incrIsParam = true;
         }
         return true;
      }
      default:
         break;
   }
   
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
   if (type == Gmat::PARAMETER)
   {
      if (indexName == oldName)   indexName = newName;
      if (startName == oldName)   startName = newName;
      if (endName == oldName)     endName   = newName;
      if (incrName == oldName)    incrName  = newName;
   }
   
   BranchCommand::RenameRefObject(type, oldName, newName);
   
   return true;
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
   if (id == START_VALUE)   return startValue;
   if (id == END_VALUE)     return endValue;
   if (id == STEP)          return stepSize;
   
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
bool For::SetStringParameter(const Integer id, 
                             const std::string &value)
{
   if (id == INDEX_NAME) 
   {
      indexName    = value; 
      return true;
   }
   else if (id == START_NAME)
   {
      startName = value;
      return true;
   }
   else if (id == END_NAME)
   {
      endName = value;
      return true;
   }
   else if (id == INCREMENT_NAME)
   {
      incrName = value;
      return true;
   }
   
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
bool For::GetBooleanParameter(const Integer id) const
{
   if (id == INDEX_IS_PARAM)       return indexIsParam;
   if (id == START_IS_PARAM)       return startIsParam;
   if (id == END_IS_PARAM)         return endIsParam;
   if (id == INCR_IS_PARAM)        return incrIsParam;

   return BranchCommand::GetBooleanParameter(id);
}

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
bool For::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
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
      if (startIsParam)  startValue = startParam->GetReal();
      if (endIsParam)    endValue   = endParam->GetReal();
      if (incrIsParam)   stepSize   = incrParam->GetReal();

      #if DEBUG_FOR
      MessageInterface::ShowMessage
         ("For::StillLooping() startValue=%f, stepSize=%f, endValue=%f\n",
          startValue, stepSize, endValue);
      #endif
      
      if ((stepSize == 0.0) ||
          ((stepSize > 0.0) && (startValue > endValue)) ||
          ((stepSize < 0.0) && (startValue < endValue)) )
         throw CommandException(
               "For loop values incorrect - will result in infinite loop.\n");
      
      currentValue = startValue;
      
      if (indexIsParam)
         indexParam->SetReal(currentValue);
      commandComplete  = false;      
   }
   if (((stepSize > 0.0) && (currentValue <= endValue)) ||
       ((stepSize < 0.0) && (currentValue >= endValue)) )
      return true;
   
   // not looping anymore
   return false;
}
