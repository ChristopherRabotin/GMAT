//$Header$
//------------------------------------------------------------------------------
//                                 Minimize
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Wendy Shoan (GSFC/MAB) (begun by Daniel Hunter during summer 2006)
// Created: 2006.08.11
//
/**
 * Definition for the Minimize command class
 */
//------------------------------------------------------------------------------


#include "Minimize.hpp"
/// @todo Rework command so it doesn't need the Moderator!!!
#include "Moderator.hpp" 
#include "StringUtil.hpp"  // for ToDouble()

//#define DEBUG_MINIMIZE_PARSE 1
//#define DEBUG_MINIMIZE_INIT 1
//#define DEBUG_MINIMIZE_EXEC 1
//#define DEBUG_MINIMIZE
//#define DEBUG_MINIMIZE_PARAM


//---------------------------------
// static data
//---------------------------------
const std::string
   Minimize::PARAMETER_TEXT[MinimizeParamCount - GmatCommandParamCount] =
   {
      "OptimizerName",
      "MinimizedVariableName",
   };
   
const Gmat::ParameterType
   Minimize::PARAMETER_TYPE[MinimizeParamCount - GmatCommandParamCount] =
   {
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
   };


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Minimize()
//------------------------------------------------------------------------------
/**
 * Creates an Minimize command.  (default constructor)
 */
//------------------------------------------------------------------------------
Minimize::Minimize() :
   GmatCommand             ("Minimize"),
   optimizerName           (""),
   objectiveName           (""),
   objective               (NULL),
   objectiveValue          (-999.99),
   optimizer               (NULL),
   optimizerDataFinalized  (false),
   isMinimizeParm          (false),
   objId                   (-1)
{
   #ifdef DEBUG_MINIMIZE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage("Creating Minimize command ...\n");
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   parameterCount = MinimizeParamCount;
}


//------------------------------------------------------------------------------
//  Minimize(const Minimize& m)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a Minimize command.  (Copy constructor)
 *
 * @param t Command that is replicated here.
 */
//------------------------------------------------------------------------------
Minimize::Minimize(const Minimize& m) :
   GmatCommand             (m),
   optimizerName           (m.optimizerName),
   objectiveName           (m.objectiveName),
   objective               (NULL),
   objectiveValue          (m.objectiveValue),
   optimizer               (NULL),
   optimizerDataFinalized  (false),
   isMinimizeParm          (m.isMinimizeParm),
   objId                   (m.objId)
{
   #ifdef DEBUG_MINIMIZE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage("Creating (copying) Minimize command ...\n");
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   parameterCount = MinimizeParamCount;
}


//------------------------------------------------------------------------------
//  Minimize& operator=(const Minimize& m)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Minimize command.
 *
 * @param t Command that is replicated here.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Minimize& Minimize::operator=(const Minimize& m)
{
   if (this == &m)
      return *this;
    
   GmatCommand::operator=(m);
   optimizerName          = m.optimizerName;
   objectiveName          = m.objectiveName;
   objective              = NULL;
   objectiveValue         = m.objectiveValue;
   optimizer              = NULL;
   optimizerDataFinalized = false;
   isMinimizeParm         = m.isMinimizeParm;
   objId                  = m.objId;

   return *this;
}

//------------------------------------------------------------------------------
//  ~Minimize()
//------------------------------------------------------------------------------
/**
 * Destroys the Minimize command.  (destructor)
 */
//------------------------------------------------------------------------------
Minimize::~Minimize()
{
   //delete objective;  // yes? no? not in ths case, it seems
}

    


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Minimize.
 *
 * @return clone of the Minimize.
 */
//------------------------------------------------------------------------------
GmatBase* Minimize::Clone() const
{
   return (new Minimize(*this));
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames referenced objects.
 *
 * @param type Type of the object that is renamed.
 * @param oldName The current name for the object.
 * @param newName The name the object has when this operation is complete.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool Minimize::RenameRefObject(const Gmat::ObjectType type,
                               const std::string &oldName,
                               const std::string &newName)
{
   if (type == Gmat::SOLVER)
   {
      if (optimizerName == oldName)
         optimizerName = newName;
   }
   else if (type == Gmat::PARAMETER)
   {
      if (objectiveName == oldName)
         objectiveName = newName;
   }
   
   return true;
}

// Parameter accessors

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Minimize::GetParameterText(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < MinimizeParamCount)
      return PARAMETER_TEXT[id - GmatCommandParamCount];
   return GmatCommand::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer Minimize::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatCommandParamCount; i < MinimizeParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatCommandParamCount])
         return i;
   }

   return GmatCommand::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Minimize::GetParameterType(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < MinimizeParamCount)
      return PARAMETER_TYPE[id - GmatCommandParamCount];

   return GmatCommand::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Minimize::GetParameterTypeString(const Integer id) const
{
   return GmatCommand::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
//  Real  GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param id ID for the requested parameter value.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
//Real Minimize::GetRealParameter(const Integer id) const
//{
//   if (id == toleranceID)
//      return tolerance;
//    
//   return GmatCommand::GetRealParameter(id);
//}


//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * @param id    ID for the parameter whose value to change.
 * @param value value for the parameter.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
//Real Minimize::SetRealParameter(const Integer id, const Real value)
//{
//   if (id == toleranceID) {
//      tolerance = value;
//      return tolerance;
//   }
//    
//   return GmatCommand::SetRealParameter(id, value);
//}


//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Minimize::GetStringParameter(const Integer id) const
{
   if (id == OPTIMIZER_NAME)
      return optimizerName;
        
   if (id == MINIMIZED_VARIABLE_NAME)
      return objectiveName;
        
   return GmatCommand::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param id    ID for the requested parameter.
 * @param value string value for the requested parameter.
 *
 * @return  success flag.
 */
//------------------------------------------------------------------------------
bool Minimize::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_MINIMIZE_PARAM // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
   MessageInterface::ShowMessage
      ("Minimize::SetStringParameter() id=%d, value=%s\n", id, value.c_str());
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   
   if (id == OPTIMIZER_NAME) {
      optimizerName = value;
      return true;
   }

   if (id == MINIMIZED_VARIABLE_NAME) {
      objectiveName = value;
      return true;
   }

   return GmatCommand::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                                   const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Sets referenced objects.
 *
 * @param obj reference object pointer.
 * @param type type of the reference object.
 * @param name name of the reference object.
 *
 * @return success of the operation.
 */
//------------------------------------------------------------------------------
bool Minimize::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                            const std::string &name)
{   
   #ifdef DEBUG_MINIMIZE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage(
      "setting ref obj %s of type %s on Minimize command\n",
      (obj->GetName()).c_str(), (obj->GetTypeName()).c_str());
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   if (type == Gmat::SOLVER) 
   {
      if (optimizerName == obj->GetName()) 
      {
         optimizer = (Solver*)obj;
         return true;
      }
      return false;
   }
   if (type == Gmat::PARAMETER) 
   {
      if (objectiveName == obj->GetName()) 
      {
         objective = (Variable*)obj;
         return true;
      }
      return false;
   }

   return GmatCommand::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
//  bool InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * The Minimize command has the following syntax:
 *
 *     Minimize myOPT(Var1);
 *
 * where myOPT is a Solver used to Minimize a set of variables to achieve the
 * corresponding goals.  This method breaks the script line into the 
 * corresponding pieces, and stores the name of the Solver so it can be set to
 * point to the correct object during initialization.
 */
//------------------------------------------------------------------------------
bool Minimize::InterpretAction()
{
   #ifdef DEBUG_MINIMIZE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage("Interpreting Minimize command ...\n");
      MessageInterface::ShowMessage("generatingString = %s",
         generatingString.c_str());
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   /// @todo: Clean up this hack for the Minimize::InterpretAction method
   // Sample string:  "Minimize myDC(Sat1.SMA = 21545.0, {Tolerance = 0.1});"
   
   // Set starting location to the space following the command string
   Integer loc = generatingString.find("Minimize", 0) + 8, end, strend;
   const char *str = generatingString.c_str();
   
   // Skip white space
   while (str[loc] == ' ')
      ++loc;
   
   // Stop at the opening paren
   end = generatingString.find("(", loc);
   
   std::string component = generatingString.substr(loc, end-loc);
   #ifdef DEBUG_MINIMIZE_PARSE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage
         ("Minimize::InterpretAction() optimizerName = \"%s\"\n", 
         component.c_str());
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   if (component == "")
      throw CommandException("Minimize string does specify the optimizer");
   
   SetStringParameter(OPTIMIZER_NAME, component);
   
   // Find the variable name
   loc = end + 1;
   // Skip white space
   while (str[loc] == ' ')
      ++loc;
   // Stop at the opening paren
   //end = generatingString.find("=", loc);
   end = generatingString.find(")", loc);
   strend = end-1;
   // Drop trailing white space
   while (str[strend] == ' ')
      --strend;
    
   component = generatingString.substr(loc, strend-loc+1);
   objectiveName = component;


   #ifdef DEBUG_MINIMIZE_PARSE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage
         ("Minimize::InterpretAction() objectiveName = \"%s\"\n", 
         objectiveName.c_str());
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~

   // Get an instance if this is a Parameter
   Moderator *mod = Moderator::Instance();
   //std::string parmObj, parmType, parmSystem;
   if (mod->GetParameter(objectiveName) == NULL)
      throw CommandException(
      "Minimize::Parameter %s not known to Moderator\n");
   objective = (Variable*) mod->GetParameter(objectiveName);

   // ************** but would it already have been created, as we're still
   // in the script interpreting phase here ******************** TBD *********
   // query the objectMap here .....
   /*
   if (!objectMap)
      throw CommandException("ObjectMap not set for Minimize command!");
      
   if (objectMap->find(objectiveName) == objectMap->end()) 
   {
      std::string errorStr = "Minimize: parameter %s not found in objectMap\n" +
         objectiveName;
      throw CommandException(errorStr);
   }
   objective = (Variable*) (*objectMap)[objectiveName];
   */
   isMinimizeParm = true;
   
   #ifdef DEBUG_MINIMIZE_PARSE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage("Minimize::InterpretAction() exiting\n");
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      
   return true;
}



//------------------------------------------------------------------------------
//  bool InterpretParameter(const std::string text, std::string &paramType,
//                          std::string &paramObj, std::string &parmSystem)
//------------------------------------------------------------------------------
/**
 * Breaks apart a parameter declaration into its component pieces
 *
 * @param text       The string that gets decomposed.
 * @param paramType  Type of parameter that is needed.
 * @param paramObj   The Object used for the parameter calculations.
 * @param parmSystem The coordinate system or body used for the parameter
 *                   calculations (or the empty string if this piece is
 *                   unspecified).
 *
 * @return success flag
 */
//------------------------------------------------------------------------------
/*
bool Minimize::InterpretParameter(const std::string text,
                                 std::string &paramType, 
                                 std::string &paramObj, 
                                 std::string &parmSystem)
{
   
   Real rval = 54321.12345;
   // check to see if it is a number first
   if (GmatStringUtil::ToDouble(text, &rval))
      return true;
   
   Integer start = 0, dotLoc = text.find(".", 0);
   if (dotLoc == (Integer)std::string::npos)
   {
//      throw CommandException("Minimize::InterpretParameter: Unable to "
//               "interpret parameter object in the string " +
//               text);
      parmObj = text; // the Variable is the parameter
      parmSystem = "";
      parmType   = "";
      return true;
   }
   
   paramObj = text.substr(start, dotLoc - start);
   start = dotLoc + 1;
   dotLoc = text.find(".", start);
   if (dotLoc != (Integer)std::string::npos) 
   {
      parmSystem = text.substr(start, dotLoc - start);
      start = dotLoc + 1;
   }
   else 
   {
      parmSystem = "";
   }
   
   paramType = text.substr(start);
   
   #ifdef DEBUG_MINIMIZE_PARSE // ~~~~ debug ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      MessageInterface::ShowMessage(
         "Minimize::InterpretParameter() Built parameter %s for object %s "
         "with CS %s\n", paramType.c_str(), paramObj.c_str(), parmSystem.c_str());
   #endif // ~~~~ end debug ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      
   return true;
}
*/

//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Minimize command.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Minimize::Initialize()
{
   #if DEBUG_MINIMIZE_INIT // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
   MessageInterface::ShowMessage
      ("Minimize::Initialize() entered. optimizer=%p, objective=%p\n", 
      optimizer, objective);
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   
   bool retval = GmatCommand::Initialize();

   if (optimizer == NULL)
      throw CommandException(
         "Optimizer not initialized for Minimize command\n  \""
         + generatingString + "\"\n");

   //Integer id = optimizer->GetParameterID("Goals");  // no clue
   //optimizer->SetStringParameter(id, goalName);

   // find objectiveName
   GmatBase *obj = (*objectMap)[objectiveName];
   
   if (obj == NULL) 
   {
      std::string errorstr = "Could not find variable parameter ";
      errorstr += objectiveName;
      throw CommandException(errorstr);
   }

   objective = (Variable*)obj;
   
   //goalObject = obj;
   //parmId = id;

   // find achieveParm
   /*
   if (isMinimizeParm)
   {
      #if DEBUG_MINIMIZE_INIT
      MessageInterface::ShowMessage
         ("Minimize::Initialize() Find achieveParm=%s from objectMap\n",
          achieveName.c_str());
      #endif
      if (objectMap->find(achieveName) != objectMap->end())
         achieveParm = (Parameter*)((*objectMap)[achieveName]);

      if (isMinimizeArray)
      {
         // if variable index is used, get variable from the objectMap
         if (achieveArrRow == -1)
            if (objectMap->find(achieveArrRowStr) != objectMap->end())
               achieveArrRowParm = (Parameter*)((*objectMap)[achieveArrRowStr]);
            else
               throw CommandException("Cannot find array row index variable\n");

         if (achieveArrCol == -1)
            if (objectMap->find(achieveArrColStr) != objectMap->end())
               achieveArrColParm = (Parameter*)((*objectMap)[achieveArrColStr]);
            else
               throw CommandException("Cannot find array column index variable\n");
      }
   }
   */
   // The optimizer cannot be finalized until all of the loop is initialized
   optimizerDataFinalized = false;
   
   #if DEBUG_MINIMIZE_INIT // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
   MessageInterface::ShowMessage
      ("Minimize::Initialize() exiting. optimizer=%p, objective=%p\n", 
      optimizer, objective);
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   
   return retval;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Minimize the variables defined for this targeting loop.
 *
 * This method (will eventually) feeds data to the optimizer state machine in 
 * order to determine the variable values needed to achieve the user specified
 * goals.
 *
 * @return true if the GmatCommand runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool Minimize::Execute()
{
   #ifdef DEBUG_MINIMIZE_EXEC // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
   MessageInterface::ShowMessage
      ("Minimize::Execute() optimizerDataFinalized=%d\n, optimizer=%s, objective=%p\n", 
       optimizerDataFinalized, optimizer, objective);
   MessageInterface::ShowMessage
      ("   objectiveName=%s\n", objectiveName.c_str());
   if (objective)
      MessageInterface::ShowMessage("   objective=%s\n", objective->GetName().c_str());
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   
   bool retval = true;
   if (!optimizerDataFinalized) 
   {
      // Tell the optimizer about the objective function
      Real minData[1];
      minData[0] = objectiveValue; 
      //minData[1] = tolerance;
      objId = optimizer->SetSolverResults(minData, objectiveName, "Objective");

      optimizerDataFinalized = true;
      return retval;
   }
   
   Real val = -999.999;
   // Evaluate variable and pass it to the optimizer
   if (objective != NULL)
   {
      val = objective->EvaluateReal();
      #ifdef DEBUG_MINIMIZE_EXEC // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage
            ("   objective=%s, %p\n", objective->GetTypeName().c_str(), objective);
         MessageInterface::ShowMessage("   Parameter target: %s val = %lf\n",
            objective->GetTypeName().c_str(), val);
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      optimizer->SetResultValue(objId, val, "Objective");
   }
   else 
   {  // ERROR ERROR - should I throw an exception here? *********** TBD **************
      MessageInterface::ShowMessage(
         "Minimize: Warning - objective function is NULL\n"); 
      val = -999.999;
      //val = goalObject->GetRealParameter(parmId);  // again,no clue
      optimizer->SetResultValue(objId, val, "Objective");
      #ifdef DEBUG_MINIMIZE_EXEC // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage("   Object target: val = %lf\n", val);
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   }

   //targeter->SetResultValue(goalId, val);
   BuildCommandSummary(true);
   
   return retval;
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
 * @param mode    Specifies the type of serialization requested.  (Not yet used
 *                in commands)
 * @param prefix  Optional prefix appended to the object's name.  (Not yet used
 *                in commands)
 * @param useName Name that replaces the object's name.  (Not yet used in
 *                commands)
 *
 * @return The script line that, when interpreted, defines this Minimize command.
 */
//------------------------------------------------------------------------------
const std::string& Minimize::GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName)
{
   // Build the local string
   std::stringstream tol;
   //tol << tolerance;
   std::string gen = prefix + "Minimize " + optimizerName + "(" + objectiveName;

   generatingString = gen + ");";
   // Then call the base class method
   return GmatCommand::GetGeneratingString();
}


