//$Header$
//------------------------------------------------------------------------------
//                                 Achieve
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/01/26
//
/**
 * Definition for the Achieve command class
 */
//------------------------------------------------------------------------------


#include "Achieve.hpp"
/// @todo Rework command so it doesn't need the Moderator!!!
#include "Moderator.hpp" 

//#define DEBUG_ACHIEVE 1


//---------------------------------
// static data
//---------------------------------
const std::string
   Achieve::PARAMETER_TEXT[AchieveParamCount - GmatCommandParamCount] =
   {
      "TargeterName",
      "Goal",
      "GoalValue",
      "Tolerance"
   };
   
const Gmat::ParameterType
   Achieve::PARAMETER_TYPE[AchieveParamCount - GmatCommandParamCount] =
   {
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
      Gmat::REAL_TYPE
   };


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Achieve()
//------------------------------------------------------------------------------
/**
 * Creates an Achieve command.  (default constructor)
 */
//------------------------------------------------------------------------------
Achieve::Achieve() :
   GmatCommand             ("Achieve"),
   targeterName            (""),
   goalName                (""),
   goal                    (0.0),
   goalTarget              (NULL),
   tolerance               (0.0),
   goalObject              (NULL),
   parmId                  (-1),
   goalId                  (-1),
   targeter                (NULL),
   targeterDataFinalized   (false),
   goalParm                (NULL)
{
   parameterCount = AchieveParamCount;
}


//------------------------------------------------------------------------------
//  ~Achieve()
//------------------------------------------------------------------------------
/**
 * Destroys the Achieve command.  (destructor)
 */
//------------------------------------------------------------------------------
Achieve::~Achieve()
{
}

    
//------------------------------------------------------------------------------
//  Achieve(const Achieve& t)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a Achieve command.  (Copy constructor)
 *
 * @param t Command that is replicated here.
 */
//------------------------------------------------------------------------------
Achieve::Achieve(const Achieve& t) :
   GmatCommand             (t),
   targeterName            (t.targeterName),
   goalName                (t.goalName),
   goal                    (t.goal),
   goalTarget              (NULL),
   tolerance               (t.tolerance),
   goalObject              (NULL),
   parmId                  (t.parmId),
   goalId                  (t.goalId),
   targeter                (NULL),
   targeterDataFinalized   (false),
   goalParm                (NULL)
{
   parameterCount = AchieveParamCount;
}


//------------------------------------------------------------------------------
//  Achieve& operator=(const Achieve& t)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Achieve command.
 *
 * @param t Command that is replicated here.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Achieve& Achieve::operator=(const Achieve& t)
{
   if (this == &t)
      return *this;
    
   GmatCommand::operator=(t);
   targeterName = t.targeterName;
   goalName = t.goalName;
   goal = t.goal;
   goalTarget = NULL;
   tolerance = t.tolerance;
   goalObject = NULL;
   parmId = t.parmId;
   goalId = t.goalId;
   targeter = NULL;
   targeterDataFinalized = false;
   goalParm = NULL;

   return *this;
}



//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Achieve.
 *
 * @return clone of the Achieve.
 */
//------------------------------------------------------------------------------
GmatBase* Achieve::Clone() const
{
   return (new Achieve(*this));
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
bool Achieve::RenameRefObject(const Gmat::ObjectType type,
                              const std::string &oldName,
                              const std::string &newName)
{
   if (type == Gmat::SOLVER)
   {
      if (targeterName == oldName)
         targeterName = newName;
   }
   else if (type == Gmat::SPACECRAFT)
   {
      // set new goal name using object and property
      std::string::size_type pos = goalName.find(oldName);
      if (pos != goalName.npos)
         goalName.replace(pos, oldName.size(), newName);
   }
   else if (type == Gmat::PARAMETER)
   {
      if (goalName == oldName)
         goalName = newName;
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
std::string Achieve::GetParameterText(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < AchieveParamCount)
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
Integer Achieve::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatCommandParamCount; i < AchieveParamCount; i++)
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
Gmat::ParameterType Achieve::GetParameterType(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < AchieveParamCount)
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
std::string Achieve::GetParameterTypeString(const Integer id) const
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
Real Achieve::GetRealParameter(const Integer id) const
{
   if (id == toleranceID)
      return tolerance;
    
   return GmatCommand::GetRealParameter(id);
}


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
Real Achieve::SetRealParameter(const Integer id, const Real value)
{
   if (id == toleranceID) {
      tolerance = value;
      return tolerance;
   }
    
   return GmatCommand::SetRealParameter(id, value);
}


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
std::string Achieve::GetStringParameter(const Integer id) const
{
   if (id == targeterNameID)
      return targeterName;
        
   if (id == goalNameID)
      return goalName;
        
   if (id == goalID) {
      return goalString;
   }

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
bool Achieve::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == targeterNameID) {
      targeterName = value;
      return true;
   }

   if (id == goalNameID) {
      goalName = value;
      return true;
   }

   if (id == goalID) {
      goalString = value;
      // Goal can be either a parameter or a number; ConstructGoal determines this.
      Real realValue;
      if (ConstructGoal(value.c_str()))
         // It's a parameter; just set dummy value here -- gets reset on execution
         realValue = 54321.0;
      else
         realValue = atof(goalString.c_str());

      #ifdef DEBUG_ACHIEVE
         MessageInterface::ShowMessage("GoalString = '%s'\n",
            goalString.c_str());
      #endif

      goal = realValue;
      return true;
   }

   return GmatCommand::SetStringParameter(id, value);
}


// Multiple variables on the same line are not allowed in the current build.
// const StringArray& Achieve::GetStringArrayParameter(const Integer id) const; 


//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                                     const std::string &name = "")
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
bool Achieve::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                           const std::string &name)
{
   if (type == Gmat::SOLVER) {
      if (targeterName == obj->GetName()) {
         targeter = (Solver*)obj;
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
 * The Achieve command has the following syntax:
 *
 *     Achieve myDC(Sat1.SMA = 21545.0, {Tolerance = 0.1});
 *
 * where myDC is a Solver used to Achieve a set of variables to achieve the
 * corresponding goals.  This method breaks the script line into the 
 * corresponding pieces, and stores the name of the Solver so it can be set to
 * point to the correct object during initialization.
 */
//------------------------------------------------------------------------------
bool Achieve::InterpretAction()
{
   /// @todo: Clean up this hack for the Achieve::InterpretAction method
   // Sample string:  "Achieve myDC(Sat1.SMA = 21545.0, {Tolerance = 0.1});"
    
   // Set starting location to the space following the command string
   Integer loc = generatingString.find("Achieve", 0) + 7, end, strend;
   const char *str = generatingString.c_str();
    
   // Skip white space
   while (str[loc] == ' ')
      ++loc;
    
   // Stop at the opening paren
   end = generatingString.find("(", loc);
    
    
   std::string component = generatingString.substr(loc, end-loc);
   if (component == "")
      throw CommandException("Achieve string does specify the targeter");
   SetStringParameter(targeterNameID, component);
    
   // Find the goal
   loc = end + 1;
   // Skip white space
   while (str[loc] == ' ')
      ++loc;
   // Stop at the opening paren
   end = generatingString.find("=", loc);
   strend = end-1;
   // Drop trailing white space
   while (str[strend] == ' ')
      --strend;
    
   component = generatingString.substr(loc, strend-loc+1);
   goalName = component;

   // Get an instance if this is a Parameter
   Moderator *mod = Moderator::Instance();

   #if DEBUG_ACHIEVE
      MessageInterface::ShowMessage
         ("Achieve::InterpretAction() goalName = \"%s\"\n", goalName.c_str());
   #endif

   std::string parmObj, parmType, parmSystem;
   InterpretParameter(goalName, parmType, parmObj, parmSystem);

   #if DEBUG_ACHIEVE
      MessageInterface::ShowMessage
         ("Achieve::InterpretAction() parmObj=%s, parmType=%s, "
          "parmSystem = \"%s\"\n", parmObj.c_str(),
          parmType.c_str(), parmSystem.c_str());
   #endif
    
   goalParm = mod->CreateParameter(parmType, goalName);
   if (!goalParm)
      throw CommandException("Unable to create parameter " + goalName);
   goalParm->SetRefObjectName(Gmat::SPACECRAFT, parmObj); //loj: 9/13/04 added

   if (goalParm->IsCoordSysDependent()) {
      if (parmSystem == "")
         parmSystem = "EarthMJ2000Eq";
      goalParm->SetStringParameter("DepObject", parmSystem);
      goalParm->SetRefObjectName(Gmat::COORDINATE_SYSTEM, parmSystem);
   }

   if (goalParm->IsOriginDependent()) {
      if (parmSystem == "")
         parmSystem = "Earth";
      goalParm->SetStringParameter("DepObject", parmSystem);
      if (goalParm->NeedCoordSystem())
         /// @todo Update coordinate system to better value for body parms
         goalParm->SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");

   }

   // Find the value
   loc = end + 1;
    
   // Goal can be either a parameter or a number; ConstructGoal determines this.
   Real value;
   if (ConstructGoal(&str[loc]))
      // It's a parameter; just set dummy value here -- gets reset on execution
      value = 54321.0;    
   else
      value = atof(goalString.c_str());

   #ifdef DEBUG_ACHIEVE
      MessageInterface::ShowMessage("GoalString = '%s'\n", goalString.c_str());
   #endif

   goal = value;

   // Find perts
   loc = generatingString.find("Tolerance", strend);
   end = generatingString.find("=", loc);
    
   value = atof(&str[end+1]);
   SetRealParameter(toleranceID, value);
    
   return true;
}


//------------------------------------------------------------------------------
// bool ConstructGoal(const char* str)
//------------------------------------------------------------------------------
/**
 * Builds goals -- either as a parameter or as a numerc value, depending on the
 * script contents.
 * 
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool Achieve::ConstructGoal(const char* str)
{
   #ifdef DEBUG_ACHIEVE
      MessageInterface::ShowMessage("ConstructGoal(%s) called\n", str);
   #endif
   // Skip white space
   Integer start = 0, dot, end;
   while (str[start] == ' ')
      ++start;
   // Find last character in this piece
   end = dot = start;
   while ((str[end] != ' ') && (str[end] != ',') && (str[end] != '{')) {
      if ((str[end] == '.') && (dot == start))
         dot = end;
      ++end;
   }

   std::string sstr = str;
   goalString = sstr.substr(start, end-start);

   #ifdef DEBUG_ACHIEVE
      MessageInterface::ShowMessage("%s%s\"\n",
                           "Achieve::ConstructGoal called with string \"", str);
      MessageInterface::ShowMessage("%s%s\"\n",
                 "Achieve::ConstructGoal goalString is \"", goalString.c_str());
      MessageInterface::ShowMessage("%s%s\"\n",
                           "   Examining the substring \"", 
                           sstr.substr(start, end-start).c_str());
   #endif

   if ((dot > start) && (dot < end)) {    // Could be a parameter

      std::string parmType, parmObj, parmSystem;
      InterpretParameter(goalString, parmType, parmObj, parmSystem);

      #if DEBUG_ACHIEVE
         MessageInterface::ShowMessage
            ("   Achieve::ConstructGoal() parmObj=%s, parmType=%s, "
             "parmSystem = \"%s\"\n", parmObj.c_str(),
             parmType.c_str(), parmSystem.c_str());
      #endif

      Moderator *mod = Moderator::Instance();

      if (mod->IsParameter(parmType))
      {
         goalTarget = mod->CreateParameter(parmType, goalString);
         if (!goalTarget)
            throw CommandException("Unable to create parameter " + goalString);
         goalTarget->SetRefObjectName(Gmat::SPACECRAFT, parmObj);

         if (goalTarget->IsCoordSysDependent()) {
            if (parmSystem == "")
               parmSystem = "EarthMJ2000Eq";
            goalTarget->SetStringParameter("DepObject", parmSystem);
            goalTarget->SetRefObjectName(Gmat::COORDINATE_SYSTEM, parmSystem);
         }

         if (goalTarget->IsOriginDependent()) {
            if (parmSystem == "")
               parmSystem = "Earth";
            goalTarget->SetStringParameter("DepObject", parmSystem);
            if (goalTarget->NeedCoordSystem())
               /// @todo Update coordinate system to better value for body parms
               goalTarget->SetRefObjectName(Gmat::COORDINATE_SYSTEM,
                  "EarthMJ2000Eq");
         }
         return true;
      }
      #ifdef DEBUG_ACHIEVE
         else {
            MessageInterface::ShowMessage("\"%s\" is not a parameter\n",
                                          goalString.c_str());
         }
      #endif
   }
   
   return false;
}


//------------------------------------------------------------------------------
//  bool InterpretParameter(const std::string text, std::string &paramType,
//                          std::string &paramObj, std::string &parmSystem)
//------------------------------------------------------------------------------
/**
 * Breaks apart a parameter declaration into its component pieces
 *
 * @param text The string that gets decomposed.
 * @param paramType Type of parameter that is needed.
 * @param paramObj The Object used for the parameter calculations.
 * @param parmSystem The coordinate system or body used for the parameter
 *                   calculations (or the empty string if this piece is
 *                   unspecified).
 *
 * @return true if the decomposition worked.
 */
//------------------------------------------------------------------------------
bool Achieve::InterpretParameter(const std::string text,
                                 std::string &paramType, 
                                 std::string &paramObj, 
                                 std::string &parmSystem)
{
   Integer start = 0, dotLoc = text.find(".", 0);
   if (dotLoc == (Integer)std::string::npos)
      throw CommandException("Propagate::InterpretParameter: Unable to "
               "interpret parameter object in the string " +
               text);
               
   paramObj = text.substr(start, dotLoc - start);
   start = dotLoc + 1;
   dotLoc = text.find(".", start);
   if (dotLoc != (Integer)std::string::npos) {
      parmSystem = text.substr(start, dotLoc - start);
      start = dotLoc + 1;
   }
   else {
      parmSystem = "";
   }
   
   paramType = text.substr(start);
   
   #ifdef DEBUG_PROPAGATE_INIT
      MessageInterface::ShowMessage(
         "Built parameter %s for object %s with CS %s", paramType.c_str(),
         paramObj.c_str(), parmSystem.c_str());
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Achieve command.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Achieve::Initialize()
{
   bool retval = GmatCommand::Initialize();

   if (targeter == NULL)
      throw CommandException(
         "Targeter not initialized for Achieve command\n  \""
         + generatingString + "\"\n");

   Integer id = targeter->GetParameterID("Goals");
   targeter->SetStringParameter(id, goalName);
    

   // Break component into the object and its parameter
   std::string objectName, parmName;
   Integer loc = goalName.find(".");
   objectName = goalName.substr(0, loc);
   parmName = goalName.substr(loc+1, goalName.length() - (loc+1));
   GmatBase *obj = (*objectMap)[objectName];
   if (obj == NULL) {
      std::string errorstr = "Could not find object ";
      errorstr += objectName;
      throw CommandException(errorstr);
   }
    
   if (goalParm != NULL) {
      goalParm->AddRefObject(obj);
   }
   else {
      id = obj->GetParameterID(parmName);
      if (id == -1) {
         std::string errorstr = "Could not find parameter ";
         errorstr += parmName;
         errorstr += " on object ";
         errorstr += objectName;
         throw CommandException(errorstr);
      }
      Gmat::ParameterType type = obj->GetParameterType(id);
      if (type != Gmat::REAL_TYPE) {
         std::string errorstr = "The targeter goal ";
         errorstr += parmName;
         errorstr += " on object ";
         errorstr += objectName;
         errorstr += " is not Real.";
         throw CommandException(errorstr);
      }
   }
    
   goalObject = obj;
   parmId = id;

   // The targeter cannot be finalized until all of the loop is initialized
   targeterDataFinalized = false;
   return retval;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Achieve the variables defined for this targeting loop.
 *
 * This method (will eventually) feeds data to the targeter state machine in 
 * order to determine the variable values needed to achieve the user specified
 * goals.
 *
 * @return true if the GmatCommand runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool Achieve::Execute()
{
   bool retval = true;
   if (!targeterDataFinalized) {
      // Tell the targeter about the goals and tolerances
      Real goalData[2];
      goalData[0] = goal;
      goalData[1] = tolerance;
      goalId = targeter->SetSolverResults(goalData, goalName);

      targeterDataFinalized = true;
      return retval;
   }
    
   Real val;
   // Evaluate the floating target (if there is one) and set it on the targeter
   if (goalTarget != NULL) {
      val = goalTarget->EvaluateReal();
      #ifdef DEBUG_ACHIEVE_EXEC
         MessageInterface::ShowMessage("Floating target: val = %lf\n", val);
      #endif
      targeter->UpdateSolverGoal(goalId, val);
   }

   // Evaluate goal and pass it to the targeter
   if (goalParm != NULL)
   {
      val = goalParm->EvaluateReal();
      #ifdef DEBUG_ACHIEVE_EXEC
         MessageInterface::ShowMessage("Parameter target: %s val = %lf\n",
            goalParm->GetTypeName().c_str(), val);
      #endif
   }
   else 
   {
      val = goalObject->GetRealParameter(parmId);
      #ifdef DEBUG_ACHIEVE_EXEC
         MessageInterface::ShowMessage("Object target: val = %lf\n", val);
      #endif
   }

   targeter->SetResultValue(goalId, val);
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
 * @return The script line that, when interpreted, defines this Achieve command.
 */
//------------------------------------------------------------------------------
const std::string& Achieve::GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName)
{
   // Build the local string
   std::stringstream tol;
   tol << tolerance;
   std::string gen = prefix + "Achieve " + targeterName + "(" + goalName +
                     " = " + goalString + ", {Tolerance = " + tol.str();

   generatingString = gen + "});";
   // Then call the base class method
   return GmatCommand::GetGeneratingString();
}
