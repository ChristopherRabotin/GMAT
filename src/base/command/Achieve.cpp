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

#define DEBUG_ACHIEVE 1

//------------------------------------------------------------------------------
//  Achieve(void)
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
   goalParm                (NULL),
   targeterNameID          (parameterCount),
   goalNameID              (parameterCount+1),
   goalID                  (parameterCount+2),
   toleranceID             (parameterCount+3)
{
   parameterCount += 4;
}


//------------------------------------------------------------------------------
//  ~Achieve(void)
//------------------------------------------------------------------------------
/**
 * Destroys the Achieve command.  (destructor)
 */
//------------------------------------------------------------------------------
Achieve::~Achieve()
{
//   if (goalParm)
//      delete goalParm;
//   if (goalTarget)
//      delete goalTarget;
}

    
//------------------------------------------------------------------------------
//  Achieve(const Achieve& t)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a Achieve command.  (Copy constructor)
 *
 * @return A reference to this instance.
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
   goalParm                (NULL),
   targeterNameID          (t.targeterNameID),
   goalNameID              (t.goalNameID),
   goalID                  (t.goalID),
   toleranceID             (t.toleranceID)
{
   parameterCount = t.parameterCount;
}


//------------------------------------------------------------------------------
//  Achieve& operator=(const Achieve& t)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Achieve command.
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
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Achieve.
 *
 * @return clone of the Achieve.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Achieve::Clone(void) const
{
   return (new Achieve(*this));
}

//loj: 11/22/04 added
//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//---------------------------------------------------------------------------
std::string Achieve::GetParameterText(const Integer id) const
{
   if (id == targeterNameID)
      return "TargeterName";
        
   if (id == goalNameID)
      return "Goal";
        
   if (id == goalID)
      return "GoalValue";
        
   if (id == toleranceID)
      return "Tolerance";
    
   return GmatCommand::GetParameterText(id);
}


Integer Achieve::GetParameterID(const std::string &str) const
{
   if (str == "TargeterName")
      return targeterNameID;
        
   if (str == "Goal")
      return goalNameID;
        
   if (str == "GoalValue")
      return goalID;
        
   if (str == "Tolerance")
      return toleranceID;
    
   return GmatCommand::GetParameterID(str);
}


Gmat::ParameterType Achieve::GetParameterType(const Integer id) const
{
   if (id == targeterNameID)
      return Gmat::STRING_TYPE;
        
   if (id == goalNameID)
      return Gmat::STRING_TYPE;
        
   if (id == goalID)
//      return Gmat::REAL_TYPE;
      return Gmat::STRING_TYPE;
        
   if (id == toleranceID)
      return Gmat::REAL_TYPE;
    
   return GmatCommand::GetParameterType(id);
}


std::string Achieve::GetParameterTypeString(const Integer id) const
{
   if (id == targeterNameID)
      return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
        
   if (id == goalNameID)
      return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
        
   if (id == goalID)
//      return PARAM_TYPE_STRING[Gmat::REAL_TYPE];
      return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
        
   if (id == toleranceID)
      return PARAM_TYPE_STRING[Gmat::REAL_TYPE];
    
   return GmatCommand::GetParameterTypeString(id);
}


Real Achieve::GetRealParameter(const Integer id) const
{
//   if (id == goalID)
//      return goal;
        
   if (id == toleranceID)
      return tolerance;
    
   return GmatCommand::GetRealParameter(id);
}


Real Achieve::SetRealParameter(const Integer id, const Real value)
{
//   if (id == goalID) {
//      goal = value;
//      return goal;
//   }
        
   if (id == toleranceID) {
      tolerance = value;
      return tolerance;
   }
    
   return GmatCommand::SetRealParameter(id, value);
}


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
      return true;
   }

   return GmatCommand::SetStringParameter(id, value);
}


// Multiple variables specified on the same line are not allowed in build 2 or 3
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
//  void InterpretAction(void)
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
bool Achieve::InterpretAction(void)
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
   }

   // Find the value
   loc = end + 1;
    
   // Goal can be either a parameter or a number; ConstructGoal determines this.
   Real value;
   if (ConstructGoal(&str[loc]))
      // It's a parameter; just set dummy value here -- gets reset on execution
      value = 54321.0;    
   else
      value = atof(&str[loc]);
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
         goalTarget->SetRefObjectName(Gmat::SPACECRAFT, parmObj); //loj: 9/13/04 added

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
      MessageInterface::ShowMessage("Built parameter %s for object %s with CS %s",
         paramType.c_str(), paramObj.c_str(), parmSystem.c_str());
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
//  bool Initialize(void)
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Achieveer.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Achieve::Initialize()
{
   bool retval = GmatCommand::Initialize();

   if (targeter == NULL)
      throw CommandException("Targeter not initialized for Achieve command\n  \""
                             + generatingString + "\"\n");
//    // Achieve specific initialization goes here:
//    
//    if (objectMap->find(targeterName) == objectMap->end()) {
//        std::string errorString = "Target command cannot find targeter \"";
//        errorString += targeterName;
//        errorString += "\"";
//        throw CommandException(errorString);
//    }
//
//    targeter = (Solver *)((*objectMap)[targeterName]);
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
//  bool Execute(void)
//------------------------------------------------------------------------------
/**
 * Achieve the variables defined for this Achieveing loop.
 *
 * This method (will eventually) runs the Achieveer state machine in order to
 * determine the variable values needed to achieve the user specified 
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
      targeter->UpdateSolverGoal(goalId, val);
   }

   // Evaluate goal and pass it to the targeter
   if (goalParm != NULL)
      val = goalParm->EvaluateReal();
   else 
      val = goalObject->GetRealParameter(parmId);

   targeter->SetResultValue(goalId, val);
   return retval;
}
