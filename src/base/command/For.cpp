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

#include "For.hpp"
#include "BranchCommand.hpp"
#include "CommandException.hpp"

//---------------------------------
// static data
//---------------------------------
const std::string
For::PARAMETER_TEXT[ForParamCount - BranchCommandParamCount] =
{
   "StartValue",
   "EndValue",
   "Step"
};

const Gmat::ParameterType
For::PARAMETER_TYPE[ForParamCount - BranchCommandParamCount] =
{
Gmat::REAL_TYPE,
Gmat::REAL_TYPE,
Gmat::REAL_TYPE
};

const Real For::UNINITIALIZED_VALUE = -999.99;

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
   startValue      (UNINITIALIZED_VALUE),
   endValue        (UNINITIALIZED_VALUE),
   stepSize        (1.0),
   currentValue    (UNINITIALIZED_VALUE)
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
currentValue    (f.currentValue)
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
   BranchCommand::Execute();

   if (StillLooping())
   {
      //if (!commandComplete) {
      retval = ExecuteBranch();
      currentValue += stepSize;
   }
   else
   {
      commandComplete  = true;
      commandExecuting = false;
      currentValue     = UNINITIALIZED_VALUE;  // start the loop over
   }
     
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
   if (id == START_VALUE)   return (startValue   = value);
   if (id == END_VALUE)     return (endValue     = value);
   if (id == STEP)          return (stepSize     = value);
   
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
   // throw an exception if the For command has not yet been initialized fully
   if ((startValue == UNINITIALIZED_VALUE) || 
       (endValue   == UNINITIALIZED_VALUE) )
      throw CommandException(
            "For command has not been initialized fully/properly.");
   
   // initialize the loop, if it's the first time through
   if (currentValue == UNINITIALIZED_VALUE)
   {
      if ((stepSize == 0.0) ||
          ((stepSize > 0.0) && (startValue > endValue)) ||
          ((stepSize < 0.0) && (startValue < endValue)) )
         throw CommandException(
               "For loop values incorrect - will result in infinite loop.");
      
      currentValue = startValue;
   }
   if (((stepSize > 0.0) && (currentValue <= endValue)) ||
       ((stepSize < 0.0) && (currentValue >= endValue)) )
      return true;
   
   // not looping anymore
   return false;
}
