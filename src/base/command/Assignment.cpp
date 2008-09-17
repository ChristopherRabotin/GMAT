//$Id$
//------------------------------------------------------------------------------
//                                Assignment
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/02/03
//
/**
 * Implementation for the Assignment command class.
 * 
 * The assignment class is the Command class that handles commands of the form
 * 
 *     GMAT object.parameter = value;
 *     GMAT variable = parameter;
 *     GMAT variable = equation;
 *
 */
//------------------------------------------------------------------------------


#include "Assignment.hpp"
#include "MathParser.hpp"
#include "StringUtil.hpp"
#include "TextParser.hpp"
#include "NumberWrapper.hpp"
#include "ArrayWrapper.hpp"
#include "FunctionManager.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_RENAME
//#define DEBUG_EVAL_RHS
//#define DEBUG_ASSIGNMENT_IA 1
//#define DEBUG_ASSIGNMENT_INIT 1
//#define DEBUG_ASSIGNMENT_EXEC 1
//#define DEBUG_EQUATION 1
//#define DEBUG_ASSIGNMENT_WRAPPER 1
//#define DEBUG_FUNCTION 1
//#define DEBUG_OBJECT_MAP
//#define DEBUG_ASSIGN_CALLING_FUNCTION

//------------------------------------------------------------------------------
//  Assignment()
//------------------------------------------------------------------------------
/**
 * Constructs the Assignment command (default constructor).
 */
//------------------------------------------------------------------------------
Assignment::Assignment  () :
   GmatCommand          ("GMAT"),
   lhs                  (""),
   rhs                  ("Not_Set"),
   mathTree             (NULL)
{
   lhsWrapper = NULL;
   rhsWrapper = NULL;
   objectTypeNames.push_back("GMAT");
   objectTypeNames.push_back("Assignment");
}


//------------------------------------------------------------------------------
//  ~Assignment()
//------------------------------------------------------------------------------
/**
 * Destroys the Assignment command (destructor).
 */
//------------------------------------------------------------------------------
Assignment::~Assignment()
{
   if (mathTree)
      delete mathTree;
   
   mathTree = NULL;
   
   ClearWrappers();
}


//------------------------------------------------------------------------------
//  Assignment(const Assignment& a)
//------------------------------------------------------------------------------
/**
 * Constructs the Assignment command (copy constructor).
 *
 * @param <a> The instance that is copied.
 */
//------------------------------------------------------------------------------
Assignment::Assignment  (const Assignment& a) :
   GmatCommand          (a),
   mathTree             (a.mathTree)
{
   lhsWrapper = NULL;
   rhsWrapper = NULL;
}


//------------------------------------------------------------------------------
//  Assignment& operator=(const Assignment& a)
//------------------------------------------------------------------------------
/**
 * Assignment assignment operator.
 *
 * @param <a> The instance that is copied.
 *
 * @return This instance, set to match the input instance.
 */
//------------------------------------------------------------------------------
Assignment& Assignment::operator=(const Assignment& a)
{
   if (this == &a)
      return *this;
   
   mathTree   = a.mathTree;
   
   lhsWrapper = NULL;
   rhsWrapper = NULL;
   
   return *this;
}


//------------------------------------------------------------------------------
// MathTree* GetMathTree()
//------------------------------------------------------------------------------
MathTree* Assignment::GetMathTree()
{
   return mathTree;
}

//------------------------------------------------------------------------------
// bool HasAFunction()
//------------------------------------------------------------------------------
bool Assignment::HasAFunction()
{
   const StringArray fNames = GetGmatFunctionNames();
   if (fNames.empty()) return false;
   return true;
}

//------------------------------------------------------------------------------
// const StringArray& GetGmatFunctionNames()
//------------------------------------------------------------------------------
const StringArray& Assignment::GetGmatFunctionNames()
{
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("Assignment::GetGmatFunctionNames() entered\n");
   #endif
   static StringArray functionNames;
   functionNames.clear();
   
   if (mathTree)
   {
      functionNames = mathTree->GetGmatFunctionNames();
      #ifdef DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("Assignment::GetGmatFunctionNames() returning %d GmatFunctions "
          " from the MathTree\n", functionNames.size());
      #endif
      return functionNames;
   }
   else
   {
      #ifdef DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("Assignment::GetGmatFunctionNames() returning 0 GmatFunctions, "
          "It's not a MathTree\n");
      #endif
      return functionNames;
   }
}


//------------------------------------------------------------------------------
// void SetMathWrappers()
//------------------------------------------------------------------------------
void Assignment::SetMathWrappers()
{
   // Set math Wrapper map
   if (mathTree)
      mathTree->SetMathWrappers(&mathWrapperMap);
}


//------------------------------------------------------------------------------
// virtual void SetFunction(Function *function)
//------------------------------------------------------------------------------
void Assignment::SetFunction(Function *function)
{
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("Assignment::SetFunction() function=%p\n", function);
   #endif
   
   if (mathTree)
      mathTree->SetFunction(function);
   
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage("Assignment::SetFunction() returning\n");
   #endif
}


//------------------------------------------------------------------------------
// std::vector<Function*> GetFunctions()
//------------------------------------------------------------------------------
std::vector<Function*> Assignment::GetFunctions() const
{
   if (mathTree)
      return mathTree->GetFunctions();
   
   std::vector<Function*> gf;
   return gf;
}


//------------------------------------------------------------------------------
//  void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Called by the Sandbox to set the local solar system for the GmatCommand
 * 
 * @param <ss> Pointer to the local solar system
 */
//------------------------------------------------------------------------------
void Assignment::SetSolarSystem(SolarSystem *ss)
{
   GmatCommand::SetSolarSystem(ss);
   
   if (mathTree)
      mathTree->SetSolarSystem(ss);
}


//------------------------------------------------------------------------------
// virtual void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/*
 * @see GmatCommand
 */
//------------------------------------------------------------------------------
void Assignment::SetInternalCoordSystem(CoordinateSystem *cs)
{
   #ifdef DEBUG_ASSIGNMENT_SET
   MessageInterface::ShowMessage("Assignment::SetInternalCoordSystem() cs=<%p>\n", cs);
   #endif
   
   GmatCommand::SetInternalCoordSystem(cs);
   
   if (mathTree)
      mathTree->SetInternalCoordSystem(cs);
}


//------------------------------------------------------------------------------
//  void SetTransientForces(std::vector<PhysicalModel*> *tf)
//------------------------------------------------------------------------------
/**
 * Passes the transient force vector into the commands that need them
 * 
 * @param tf The vector of transient forces
 * 
 * @note The default behavior in the GmatCommands is to ignore the vector.
 */
//------------------------------------------------------------------------------
void Assignment::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
   GmatCommand::SetTransientForces(tf);
   
   if (mathTree)
      mathTree->SetTransientForces(forces);
}


//------------------------------------------------------------------------------
//  void SetObjectMap(ObjectMap *map)
//------------------------------------------------------------------------------
/**
 * Called by the Sandbox to set the local asset store used by the GmatCommand
 * 
 * @param <map> Pointer to the local object map
 */
//------------------------------------------------------------------------------
void Assignment::SetObjectMap(ObjectMap *map)
{
   GmatCommand::SetObjectMap(map);
   
   if (mathTree)
      mathTree->SetObjectMap(map);
}


//------------------------------------------------------------------------------
//  void SetGlobalObjectMap(ObjectMap *map)
//------------------------------------------------------------------------------
/**
 * Called by the Sandbox to set the global asset store used by the GmatCommand
 * 
 * @param <map> Pointer to the global object map
 */
//------------------------------------------------------------------------------
void Assignment::SetGlobalObjectMap(ObjectMap *map)
{
   #ifdef DEBUG_GLOBAL_OBJECT_MAP
   MessageInterface::ShowMessage
      ("Assignment::SetGlobalObjectMap() entered, map=<%p>\n", map);
   #endif
   
   GmatCommand::SetGlobalObjectMap(map);
   
   if (mathTree)
      mathTree->SetGlobalObjectMap(map);
   
   #ifdef DEBUG_GLOBAL_OBJECT_MAP
   MessageInterface::ShowMessage("Assignment::SetGlobalObjectMap() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// bool InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * Assignment lines have the form
 *
 *    GMAT Sat.Element1 = 7654.321;
 *
 * or
 *
 *    GMAT object1 = object2;
 *
 * This method breaks this line into its constituent pieces.
 *
 * @return true on successful parsing of the command.
 *
 * @todo: Determine if Assignment::InterpretAction can be moved into the
 *        Interpreter
 */
//------------------------------------------------------------------------------
bool Assignment::InterpretAction()
{
   #ifdef DEBUG_ASSIGNMENT_IA
   MessageInterface::ShowMessage
      ("\nAssignment::InterpretAction() entered, currentFunction=<%p>'%s'\n",
       currentFunction,
       currentFunction ? "currentFunction->GetFunctionPathAndName().c_str()" : "NULL");
   #endif
   
   StringArray chunks = InterpretPreface();
   
   #ifdef DEBUG_ASSIGNMENT_IA
      MessageInterface::ShowMessage("Preface chunks as\n");
      for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
         MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
   #endif
      
   lhs = chunks[0];
   rhs = chunks[1]; 
   
   // check if there is single quote in LHS(loj: 2008.07.22)
   if (lhs.find("'") != lhs.npos)
      throw CommandException("An assignment command is not allowed to contain "
                             "single quote on the left-hand-side"); 
   
   if (!GmatStringUtil::HasNoBrackets(lhs,true))
      throw CommandException("An assignment command is not allowed to contain "
         "brackets, braces, or parentheses (except to indicate an array element)"
         " on the left-hand-side");
   
   // check for unexpected commas on the left-hand-side
   Integer commaPos = -1;
   if (lhs.find(',') != lhs.npos)
   {
      GmatStringUtil::GetArrayCommaIndex(lhs, commaPos);
      if (commaPos == -1)
         throw CommandException("Command contains an unexpected comma on left-hand-side");
   }
   
   // it there is still ; then report error since ; should have been removed
   if (rhs.find(";") != rhs.npos)
      throw CommandException("Is there a missing \"\%\" for inline comment?");
   
   // check for common use of ./ (path) in GmatFunction to avoid creating MathTree(loj: 2008.09.08)
   if (rhs.find("./") != rhs.npos)
   {
      if (currentFunction != NULL &&
          (!GmatStringUtil::IsEnclosedWith(rhs, "'")))
         throw CommandException("The string literal \"" + rhs + "\" must be "
                                "enclosed with single quotes");
   }
   
   // Check if rhs is an equation
   MathParser mp = MathParser();
   if (mp.IsEquation(rhs))
   {
      // Parse RHS if equation
      #if DEBUG_EQUATION
      MessageInterface::ShowMessage
         ("Assignment::InterpretAction() %s is an equation\n", rhs.c_str());
      #endif
      
      MathNode *topNode = mp.Parse(rhs);
      
      #if DEBUG_EQUATION
      if (topNode)
         MessageInterface::ShowMessage
            ("   topNode=%s\n", topNode->GetTypeName().c_str());
      #endif
      
      // check if sting has missing start quote (loj: 2008.07.23)
      // it will be an error only if rhs with blank space removed matches with
      // any GmatFunction name without letter case
      std::string str1 = rhs;
      if (GmatStringUtil::EndsWith(str1, "'"))
      {
         #if DEBUG_EQUATION
         MessageInterface::ShowMessage("   <%s> ends with '\n", str1.c_str());
         #endif
         
         str1 = GmatStringUtil::RemoveLastString(str1, "'");
         str1 = GmatStringUtil::RemoveAll(str1, ' ');
         StringArray gmatFnNames = mp.GetGmatFunctionNames();
         bool isError = false;
         for (UnsignedInt i=0; i<gmatFnNames.size(); i++)
         {
            if (GmatStringUtil::ToUpper(str1) == GmatStringUtil::ToUpper(gmatFnNames[i]))
            {
               isError = true;
               break;
            }
         }
         if (isError)
            throw CommandException("Found missing start quote on the right-hand-side"
                                   " of an Assignment command");
      }
      
      mathTree = new MathTree("MathTree", rhs);
      mathTree->SetTopNode(topNode);
      mathTree->SetGmatFunctionNames(mp.GetGmatFunctionNames());
   }
   else // if not an equation, check for unexpected commas on the right-hand-side
   {
      // Braces are allowed for lists of names, but brackets shouldn't be allowed
      // Assignment command should handle something like: 
      // "plot.ViewPointVector = [ 0 0 30000];", so commented out (loj: 2008.06.05)
      //if ((rhs.find('[') != rhs.npos) || (rhs.find(']') != rhs.npos))
      //   throw CommandException(
      //      "An assignment command is not allowed to contain brackets on the right-hand side"); 
      //if (!GmatStringUtil::AreAllBracketsBalanced(rhs, "({)}"))
      if (!GmatStringUtil::AreAllBracketsBalanced(rhs, "[({])}"))
         throw CommandException(
            "Parentheses or braces are unbalanced on the right-hand-side of an assignment command"); 
      
      // We want to allow the following scripts in the Assignment command.
      //    Create Formation Formation1;
      //    GMAT Formation1.Add = {Spacecraft1, Spacecraft2};
      // So commented out (loj: 2008.03.24)
      //if (rhs.find(',') != rhs.npos)
      //{
      //   GmatStringUtil::GetArrayCommaIndex(rhs, commaPos);
      //   if (commaPos == -1)
      //      throw CommandException("Command contains an unexpected comma on right-hand-side");
      //}
   }
   
   #ifdef DEBUG_ASSIGNMENT_IA
   MessageInterface::ShowMessage("Assignment::InterpretAction() returning true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Assignment command.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Assignment::Initialize()
{
   #ifdef DEBUG_ASSIGNMENT_INIT
   MessageInterface::ShowMessage
      ("Assignment::Initialize() entered for %s, It's%s a math equation\n",
       generatingString.c_str(), (mathTree == NULL ? " not" : ""));
   MessageInterface::ShowMessage
      ("   lhsWrapper=<%p><%s>\n   rhsWrapper=<%p><%s>\n",
       lhsWrapper, lhsWrapper ? lhsWrapper->GetDescription().c_str() : "NULL",
       rhsWrapper, rhsWrapper ? rhsWrapper->GetDescription().c_str() : "NULL");
   #endif
   #ifdef DEBUG_OBJECT_MAP
   ShowObjectMaps();
   #endif
   
   if (lhsWrapper == NULL || (mathTree == NULL && rhsWrapper == NULL))
      return false;
   
   if (GmatCommand::Initialize() == false)
      return false;
   
   // if rhs is not an equation, set ref obj on both lhs and rhs wrappers
   if (mathTree == NULL)
   {      
      // Set references for the wrappers   
      if (SetWrapperReferences(*lhsWrapper) == false)
         return false;
      
      if (SetWrapperReferences(*rhsWrapper) == false)
         return false;
   }
   else
   {
      // Set references for the lhs wrapper
      if (SetWrapperReferences(*lhsWrapper) == false)
         return false;
      
      std::map<std::string, ElementWrapper *>::iterator ewi;
      
      #ifdef DEBUG_ASSIGNMENT_INIT
      for (ewi = mathWrapperMap.begin(); ewi != mathWrapperMap.end(); ++ewi)
         MessageInterface::ShowMessage
            ("   name=<%s>, wrapper=<%p>, type=%d\n", (ewi->first).c_str(), ewi->second,
             (ewi->second)->GetWrapperType());
      #endif
      
      // Set references for the rhs math element wrappers
      for (ewi = mathWrapperMap.begin(); ewi != mathWrapperMap.end(); ++ewi)
      {
         if (SetWrapperReferences(*(ewi->second)) == false)
            return false;
      }
      
      // Initialize mathTree
      MathNode *topNode = mathTree->GetTopNode();
      
      #ifdef DEBUG_ASSIGNMENT_INIT
      MessageInterface::ShowMessage
         ("Assignment::Initialize() Initializing topNode=%s, %s\n",
          topNode->GetTypeName().c_str(), topNode->GetName().c_str());
      #endif
      
      std::string fnMsg;
      if (currentFunction != NULL)
      {
         fnMsg = currentFunction->GetFunctionPathAndName();
         fnMsg = "\n(In Function \"" + fnMsg + "\")";
      }
      
      try
      {
         if (mathTree->Initialize(objectMap, globalObjectMap))
         {
            if (!topNode->ValidateInputs())
               throw CommandException("Failed to validate equation inputs in\n   \"" +
                                      generatingString + "\"" + fnMsg);
         }
         else
         {
            throw CommandException("Failed to initialize equation in\n   \"" +
                                   generatingString + "\"" + fnMsg);
         }
      }
      catch (BaseException &e)
      {
         CommandException ce;
         ce.SetDetails("%s in \n   \"%s\"%s\n", e.GetDetails().c_str(),
                      generatingString.c_str(), fnMsg.c_str());
         throw ce;
      }
   }
   
   #ifdef DEBUG_ASSIGNMENT_INIT
   MessageInterface::ShowMessage("Assignment::Initialize() returning true\n");
   #endif
   
   return true;
   
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * The method that is fired to perform the command.
 *
 * Derived classes implement this method to perform their actions on 
 * GMAT objects.
 *
 * @return true if the GmatCommand runs to completion, false if an error 
 *         occurs. 
 */
//------------------------------------------------------------------------------
bool Assignment::Execute()
{
   #ifdef DEBUG_ASSIGNMENT_EXEC
   MessageInterface::ShowMessage
      ("\nAssignment::Execute() entered for \"%s\"\n", generatingString.c_str());
   #endif
   
   if (lhsWrapper == NULL || (rhsWrapper == NULL && mathTree == NULL))
   {
      CommandException ce;
      ce.SetMessage("");
      std::string msg = "Assignment::Execute() failed, LHS or/and RHS wrappers are NULL";
      ce.SetDetails("%s in\n   \"%s\"\n", msg.c_str(), generatingString.c_str());
      throw ce;
   }
   
   ElementWrapper *outWrapper = NULL;
   
   try
   {
      bool retval = false;
      
      // Use ElementWrapper static method SetValue() (loj: 2008.06.20)
      if (mathTree == NULL)
      {
         retval = ElementWrapper::SetValue(lhsWrapper, rhsWrapper, solarSys, objectMap,
                                           globalObjectMap);
      }
      else
      {
         outWrapper = RunMathTree(lhsWrapper);
         retval = ElementWrapper::SetValue(lhsWrapper, outWrapper, solarSys, objectMap,
                                           globalObjectMap);
      }
      
      #ifdef DEBUG_ASSIGNMENT_EXEC
      MessageInterface::ShowMessage("   ElementWrapper::SetValue() returned %d\n", retval);
      #endif
      
      if (!retval)
      {
         if (outWrapper)
            delete outWrapper;
         
         throw CommandException("Assignment::Execute() failed");
      }
   }
   catch (BaseException &e)
   {
      // To make error message format consistent, just add "Command Exception:"
      std::string msg = e.GetFullMessage();
      if (msg.find("Exception") == msg.npos && msg.find("exception") == msg.npos)
         msg = "Command Exception: " + msg;
      
      if (currentFunction != NULL)
      {
         MessageInterface::ShowMessage(msg + "\n");
      }
      else
      {
         if (outWrapper)
            delete outWrapper;
         
         CommandException ce;
         ce.SetMessage("");
         ce.SetDetails("%s in\n   \"%s\"\n", msg.c_str(), generatingString.c_str());
         throw ce;
      }
   }
   
   #ifdef DEBUG_ASSIGNMENT_EXEC
   MessageInterface::ShowMessage("Assignment::Execute() returning true\n");
   #endif
   
   if (outWrapper)
      delete outWrapper;
   
   return true;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
void Assignment::RunComplete()
{
   if (mathTree)
   {
      #ifdef DEBUG_RUN_COMPLETE
      MessageInterface::ShowMessage
         ("RunComplete::RunComplete() calling MathTree::Finalize()\n");
      #endif
      
      mathTree->Finalize();
   }
   
   GmatCommand::RunComplete();
}


//------------------------------------------------------------------------------
//  bool SkipInterrupt()
//------------------------------------------------------------------------------
/**
 * Allows select commands to avoid polling for user interrupts.
 * 
 * @return true if the command can skip polling; false if polling is needed.
 */
bool Assignment::SkipInterrupt()
{
   return true;
}

//------------------------------------------------------------------------------
// void SetCallingFunction();
//------------------------------------------------------------------------------
void Assignment::SetCallingFunction(FunctionManager *fm)
{
   #ifdef DEBUG_ASSIGN_CALLING_FUNCTION
      MessageInterface::ShowMessage("Assignment::SetCallingFunction - fm is %s NULL\n",
            fm? "NOT" : "really");
      MessageInterface::ShowMessage("   and mathTree DOES %s exist\n",
            mathTree? "really" : "NOT");
   #endif
   GmatCommand::SetCallingFunction(fm);
   if (mathTree)
      mathTree->SetCallingFunction(fm);
}


//------------------------------------------------------------------------------
// const StringArray& Vary::GetWrapperObjectNameArray()
//------------------------------------------------------------------------------
/*
 * Returns wrapper object names.
 */
//------------------------------------------------------------------------------
const StringArray& Assignment::GetWrapperObjectNameArray()
{
   #ifdef DEBUG_ASSIGNMENT_WRAPPER
   MessageInterface::ShowMessage
      ("Assignment::GetWrapperObjectNameArray() lhs=<%s>, rhs=<%s>\n",
       lhs.c_str(), rhs.c_str());
   #endif
   
   wrapperObjectNames.clear();
   
   // If rhs is not an equation, just add rhs
   if (mathTree == NULL)
   {
      if (rhs != "")
      {
         wrapperObjectNames.push_back(rhs);
      }
   }
   else
   {
      // Add math node elements to wrapper object names
      StringArray tmpArray = mathTree->GetRefObjectNameArray(Gmat::PARAMETER);
      if (tmpArray.size() > 0)
         wrapperObjectNames.insert(wrapperObjectNames.end(),
                                   tmpArray.begin(), tmpArray.end());
      
      #ifdef DEBUG_ASSIGNMENT_WRAPPER
      MessageInterface::ShowMessage("   Got the following from the MathTree:\n");
      #endif
      for (UnsignedInt i=0; i<wrapperObjectNames.size(); i++)
      {
         mathWrapperMap[wrapperObjectNames[i]] = NULL;
         
         #ifdef DEBUG_ASSIGNMENT_WRAPPER
         MessageInterface::ShowMessage
            ("   Math element %d, %s\n", i, wrapperObjectNames[i].c_str());
         #endif
      }
   }
   
   #ifdef DEBUG_ASSIGNMENT_WRAPPER
   MessageInterface::ShowMessage
      ("Assignment::GetWrapperObjectNameArray() returning %d wrapper elements\n",
       wrapperObjectNames.size());
   for (UnsignedInt i=0; i<wrapperObjectNames.size(); i++)
      MessageInterface::ShowMessage("   '%s'\n", wrapperObjectNames[i].c_str());
   #endif
   
   return wrapperObjectNames;
   
}


//------------------------------------------------------------------------------
// bool SetElementWrapper(ElementWrapper *toWrapper, const std::string &withName)
//------------------------------------------------------------------------------
bool Assignment::SetElementWrapper(ElementWrapper *toWrapper, 
                                   const std::string &withName)
{
   #ifdef DEBUG_ASSIGNMENT_WRAPPER
   MessageInterface::ShowMessage
      ("Assignment::SetElementWrapper() toWrapper=%p, name='%s'\n   lhs='%s'\n   rhs='%s', "
       "mathTree=<%p>\n", toWrapper,withName.c_str(), lhs.c_str(), rhs.c_str(), mathTree);
   #endif
   
   if (toWrapper == NULL)
   {
      #ifdef DEBUG_ASSIGNMENT_WRAPPER
      MessageInterface::ShowMessage
         ("Assignment::SetElementWrapper() returning false, toWrapper is NULL\n");
      #endif
      return false;
   }
   
   bool retval = false;
   
   #ifdef DEBUG_ASSIGNMENT_WRAPPER
   MessageInterface::ShowMessage
      ("   Setting wrapper \"%s\" of data type \"%d\" and of wrapper type \"%d\" "
       "on Assignment\n      \"%s\"\n", withName.c_str(), (Integer) (toWrapper->GetDataType()), 
       (Integer) (toWrapper->GetWrapperType()), GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif
   
   if (withName == lhs)
   {
      // lhs should always be object property wrapper, so check first
      if (withName.find(".") == withName.npos ||
          (withName.find(".") != withName.npos &&
           toWrapper->GetWrapperType() == Gmat::OBJECT_PROPERTY))
      {
         lhsWrapper = toWrapper;
         retval = true;
      }
   }
   
   if (mathTree == NULL)
   {
      if (withName == rhs)
      {
         rhsWrapper = toWrapper;
         retval = true;
      }
   }
   else
   {
      // if name found in the math wrapper map
      if (mathWrapperMap.find(withName) != mathWrapperMap.end())
      {
         // rhs should always be parameter wrapper, so check first
         if (withName.find(".") == withName.npos ||
             (withName.find(".") != withName.npos &&
              toWrapper->GetWrapperType() == Gmat::PARAMETER_OBJECT))
         {
            mathWrapperMap[withName] = toWrapper;
            retval = true;
         }
      }
   }
   
   #ifdef DEBUG_ASSIGNMENT_WRAPPER
   MessageInterface::ShowMessage
      ("Assignment::SetElementWrapper() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// void ClearWrappers()
//------------------------------------------------------------------------------
void Assignment::ClearWrappers()
{
   ElementWrapper* lhsEw = NULL;
   ElementWrapper* rhsEw = NULL;
   
   if (lhsWrapper)
   {
      lhsEw = lhsWrapper;
      lhsWrapper = NULL;
   }
   
   if (rhsWrapper)
   {
      rhsEw = rhsWrapper;
      rhsWrapper = NULL;
   }
   
   if (rhsEw)
      delete rhsEw;
   
   // clear rhs math wrapper map
   std::map<std::string, ElementWrapper *>::iterator ewi;
   for (ewi = mathWrapperMap.begin(); ewi != mathWrapperMap.end(); ++ewi)
   {
      if (ewi->second != NULL)
      {
         // if it is not the same as lhs wrapper, delete
         if (ewi->second != lhsEw)
            delete ewi->second;
         ewi->second = NULL;
      }
   }
   
   if (lhsEw)
      delete lhsEw;
   
   mathWrapperMap.clear();
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames referenced objects.
 *
 * @param <type> Type of the object that is renamed.
 * @param <oldName> The current name for the object.
 * @param <newName> The name the object has when this operation is complete.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool Assignment::RenameRefObject(const Gmat::ObjectType type,
                                 const std::string &oldName,
                                 const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Assignment::RenameRefObject() entered <%s>\n",
       GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   MessageInterface::ShowMessage
      ("   type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type != Gmat::PARAMETER && type != Gmat::SPACECRAFT &&
       type != Gmat::IMPULSIVE_BURN && type != Gmat::COORDINATE_SYSTEM &&
       type != Gmat::CALCULATED_POINT)
   {
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage("===> no replacement needed, just return true\n");
      #endif
      
      return true;
   }
   
   if (lhs.find(oldName) != lhs.npos)
      lhs = GmatStringUtil::ReplaceName(lhs, oldName, newName);
   
   if (rhs.find(oldName) != rhs.npos)
      rhs = GmatStringUtil::ReplaceName(rhs, oldName, newName);
   
   if (lhsWrapper)
      lhsWrapper->RenameObject(oldName, newName);
   
   if (rhsWrapper)
      rhsWrapper->RenameObject(oldName, newName);
   
   if (mathTree)
      mathTree->RenameRefObject(type, oldName, newName);
   
   // Update generatingString
   GetGeneratingString();
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Assignment::RenameRefObject() leaving <%s>\n",
       GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Assignment.
 *
 * @return clone of the Assignment.
 */
//------------------------------------------------------------------------------
GmatBase* Assignment::Clone() const
{
   return (new Assignment(*this));
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//                                            const Integer index) const
//------------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param id The integer ID for the parameter.
 * @param index The index when multiple StringArrays are supported.
 *
 * @return The requested StringArray.
 */
//------------------------------------------------------------------------------
const std::string& Assignment::GetGeneratingString(Gmat::WriteMode mode,
                                                   const std::string &prefix,
                                                   const std::string &useName)
{
   if (mode == Gmat::NO_COMMENTS)
   {
      generatingString = lhs + " = " + rhs + ";";
      return generatingString;
   }
   
   
   std::string gen = prefix + "GMAT " + lhs + " = " + rhs + ";";
   
   #ifdef DEBUG_ASSIGNMENT_SCRIPTING
   MessageInterface::ShowMessage("Assignment command generator is \"%s\"\n",
                                 gen.c_str());
   #endif
   
   generatingString = gen;
   
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}

//---------------------------------
// protected
//---------------------------------

//------------------------------------------------------------------------------
// ElementWrapper* RunMathTree(ElementWrapper *lhsWrapper)
//------------------------------------------------------------------------------
/*
 * Executes MathTree and creates output ElemenetWrapper.
 *
 * @param  lhsWrapper  LHS wrapper pointer
 */
//------------------------------------------------------------------------------
ElementWrapper* Assignment::RunMathTree(ElementWrapper *lhsWrapper)
{
   if (mathTree == NULL)
      return NULL;
   
   Gmat::ParameterType lhsDataType = lhsWrapper->GetDataType();
   ElementWrapper *outWrapper = NULL;
   
   // Evalute math tree
   Integer returnType;   
   Integer numRow;
   Integer numCol;
   
   MathNode *topNode = mathTree->GetTopNode();
   
   if (topNode)
   {
      #if DEBUG_EQUATION
      MessageInterface::ShowMessage
         ("Assignment::Execute() topNode=%s, %s\n", topNode->GetTypeName().c_str(),
          topNode->GetName().c_str());
      #endif
      
      topNode->GetOutputInfo(returnType, numRow, numCol);
      
      #if DEBUG_ASSIGNMENT_EXEC
      MessageInterface::ShowMessage("   returnType=%d\n", returnType);
      #endif
      
      if (lhsDataType != returnType)
      {
         std::string lhsTypeStr = GmatBase::PARAM_TYPE_STRING[lhsDataType];
         CommandException ce;
         ce.SetDetails("Cannot set type \"%s\" to type \"%s\"",
                       GmatBase::PARAM_TYPE_STRING[returnType].c_str(),
                       lhsTypeStr.c_str());
         throw ce;
      }
      
      // @note We need to set description before setting the value to output wrapper
      
      switch (returnType)
      {
      case Gmat::REAL_TYPE:
         {
            #if DEBUG_ASSIGNMENT_EXEC
            MessageInterface::ShowMessage("   Calling topNode->Evaluate()\n");
            #endif
            
            Real rval = -99999.999;
            rval = topNode->Evaluate();
            
            #if DEBUG_ASSIGNMENT_EXEC
            MessageInterface::ShowMessage("   Returned %f\n", rval);
            MessageInterface::ShowMessage("   Creating NumberWrapper for output\n");
            #endif
            
            outWrapper = new NumberWrapper();
            outWrapper->SetDescription(GmatStringUtil::ToString(rval));
            outWrapper->SetReal(rval);
            break;
         }
      case Gmat::RMATRIX_TYPE:
         {
            #if DEBUG_ASSIGNMENT_EXEC
            MessageInterface::ShowMessage("   Calling topNode->MatrixEvaluate()\n");
            #endif
            
            Rmatrix rmat;
            rmat.SetSize(numRow, numCol);
            rmat = topNode->MatrixEvaluate();
            Array *arr = new Array("ArrayOutput");
            arr->SetSize(numRow, numCol);
            arr->SetRmatrix(rmat);
            
            #if DEBUG_ASSIGNMENT_EXEC
            MessageInterface::ShowMessage("   Creating ArrayWrapper for output\n");
            #endif
            
            outWrapper = new ArrayWrapper();
            outWrapper->SetDescription("ArrayOutput");
            outWrapper->SetRefObject(arr);
            break;
         }
      default:
         CommandException ce;
         ce.SetDetails("Cannot set \"%s\" to \"%s\". The return type of "
                       "equation is unknown", rhs.c_str(), lhs.c_str());
         throw ce;
      }
   }
   else
   {
      throw CommandException("RHS is an equation, but top node is NULL\n");
   }
   
   return outWrapper;
   
}


