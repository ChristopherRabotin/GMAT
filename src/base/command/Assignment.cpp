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
#include "MessageInterface.hpp"

//#define DEBUG_RENAME
//#define DEBUG_EVAL_RHS
//#define DEBUG_ASSIGNMENT_IA 1
//#define DEBUG_ASSIGNMENT_INIT 1
//#define DEBUG_ASSIGNMENT_EXEC 1
//#define DEBUG_EQUATION 1
//#define DEBUG_ASSIGNMENT_WRAPPER 1
//#define DEBUG_FUNCTION 1

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
// const StringArray& GetGmatFunctions()
//------------------------------------------------------------------------------
const StringArray& Assignment::GetGmatFunctions()
{
   static StringArray emptyArray;
   
   if (mathTree)
      return mathTree->GetGmatFunctionNames();
   else
      return emptyArray;
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
   std::vector<Function*> gf;
   return gf;    // TBD - will need to ask MathTree for all the functions
                 // referred to by FunctionRunners
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
   MessageInterface::ShowMessage("\nAssignment::InterpretAction() entered\n");
   #endif
   
   StringArray chunks = InterpretPreface();
   
   #ifdef DEBUG_ASSIGNMENT_IA
      MessageInterface::ShowMessage("Preface chunks as\n");
      for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
         MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
   #endif
      
   lhs = chunks[0];
   rhs = chunks[1]; 
   
   if (!GmatStringUtil::HasNoBrackets(lhs,true))
      throw CommandException("An assignment command is not allowed to contain brackets, braces, or "
         "parentheses (except to indicate an array element) on the left-hand-side");      
   
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
      
      mathTree = new MathTree("MathTree", rhs);
      mathTree->SetTopNode(topNode);
      mathTree->SetGmatFunctionNames(mp.GetGmatFunctionNames());
   }
   else // if not an equation, check for unexpected commas on the right-hand-side
   {
      // Braces are allowed for lists of names, but brackets shouldn't be allowed
      if ((rhs.find('[') != rhs.npos) || (rhs.find(']') != rhs.npos))
         throw CommandException(
            "An assignment command is not allowed to contain brackets on the right-hand side"); 
      if (!GmatStringUtil::AreAllBracketsBalanced(rhs, "({)}"))
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
   MessageInterface::ShowMessage("Assignment::InterpretAction() leaving\n");
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
      ("Assignment::Initialize() entered for %s\n", generatingString.c_str());
   #endif
   
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
            ("   name=<%s>, wrapper=%p\n", (ewi->first).c_str(), ewi->second);
      #endif
      
      // Set references for the rhs math element wrappers
      for (ewi = mathWrapperMap.begin(); ewi != mathWrapperMap.end(); ++ewi)
      {
         if (SetWrapperReferences(*(ewi->second)) == false)
            return false;
      }
      
      // Initialize mathTree
      MathNode *topNode = mathTree->GetTopNode();
      
//       // Set math Wrapper map
//       mathTree->SetMathWrappers(&mathWrapperMap);
      
      #ifdef DEBUG_ASSIGNMENT_INIT
      MessageInterface::ShowMessage
         ("Assignment::Initialize() Initializing topNode=%s, %s\n",
          topNode->GetTypeName().c_str(), topNode->GetName().c_str());
      #endif
      
      try
      {
         if (mathTree->Initialize(objectMap, globalObjectMap))
         {
            if (!topNode->ValidateInputs())
               throw CommandException
                  ("Failed to validate equation inputs in\n   \"" +
                   generatingString + "\"\n");
         }
         else
         {
            throw CommandException("Failed to initialize equation in\n   \"" +
                                   generatingString + "\"\n");
         }
      }
      catch (BaseException &e)
      {
         CommandException ce;
         ce.SetDetails("%s in \n   \"%s\"\n", e.GetDetails().c_str(),
                      generatingString.c_str());
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
   
   Real rval = -99999.999;
   Integer ival = -99999;
   bool bval = false;
   std::string sval;
   Rmatrix rmat;
   GmatBase *rhsObj = NULL;
   
   Gmat::ParameterType lhsDataType = lhsWrapper->GetDataType();
   Gmat::ParameterType rhsDataType = Gmat::UNKNOWN_PARAMETER_TYPE;
   std::string lhsTypeStr = GmatBase::PARAM_TYPE_STRING[lhsDataType];
   std::string rhsTypeStr = "UnknownDataType";
   Gmat::WrapperDataType lhsWrapperType = lhsWrapper->GetWrapperType();
   Gmat::WrapperDataType rhsWrapperType = Gmat::UNKNOWN_WRAPPER_TYPE;
   
   #ifdef DEBUG_ASSIGNMENT_EXEC
   MessageInterface::ShowMessage
      ("   lhsWrapperType=%d, lhsDataType=%s\n", lhsWrapperType, lhsTypeStr.c_str());
   #endif
   
   try
   {
      // if rhs is not an equation, assign rhs to lhs
      if (mathTree == NULL)
      {
         rhsDataType = rhsWrapper->GetDataType();
         rhsTypeStr = GmatBase::PARAM_TYPE_STRING[rhsDataType];
         rhsWrapperType = rhsWrapper->GetWrapperType();
         
         #ifdef DEBUG_ASSIGNMENT_EXEC
         MessageInterface::ShowMessage
            ("   rhsWrapperType=%d, rhsDataType=%s\n", rhsWrapperType, rhsTypeStr.c_str());
         #endif
         
         // If lhs is String, it must be String Object, so check it first
         // ex) UnknownObj1 = str1
         if (lhsDataType == Gmat::STRING_TYPE && lhsWrapperType == Gmat::STRING)
         {
            CommandException ce;
            ce.SetDetails("Cannot set \"%s\" to unknown object \"%s\"",
                          rhs.c_str(), lhs.c_str());
            throw ce;
         }
         
         switch (rhsDataType)
         {
         case Gmat::BOOLEAN_TYPE:
            bval = rhsWrapper->EvaluateBoolean();
            break;
         case Gmat::INTEGER_TYPE:
            ival = rhsWrapper->EvaluateInteger();
            break;
         case Gmat::REAL_TYPE:
            rval = rhsWrapper->EvaluateReal();
            break;
         case Gmat::RMATRIX_TYPE:
            rmat = rhsWrapper->EvaluateArray();
            break;
         case Gmat::STRING_TYPE:
         case Gmat::ENUMERATION_TYPE:
            sval = rhsWrapper->EvaluateString();
            // Remove enclosing quotes (loj: 2008.03.26)
            sval = GmatStringUtil::RemoveEnclosingString(sval, "'");
            break;
         case Gmat::ON_OFF_TYPE:
            sval = rhsWrapper->EvaluateOnOff();
            break;
         case Gmat::OBJECT_TYPE:
            rhsObj = rhsWrapper->EvaluateObject();
            break;
         default:
            throw CommandException("Unknown RHS data type");
         }
      }
      else
      {
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
               CommandException ce;
               ce.SetDetails("Cannot set type \"%s\" to type \"%s\"",
                             GmatBase::PARAM_TYPE_STRING[returnType].c_str(),
                             lhsTypeStr.c_str());
               throw ce;
            }
            
            switch (returnType)
            {
            case Gmat::REAL_TYPE:
               rhsDataType = Gmat::REAL_TYPE;
               rval = topNode->Evaluate();
               break;
            case Gmat::RMATRIX_TYPE:
               rhsDataType = Gmat::RMATRIX_TYPE;
               rmat.SetSize(numRow, numCol);
               rmat = topNode->MatrixEvaluate();
               break;
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
      }
      
      #if DEBUG_ASSIGNMENT_EXEC
      MessageInterface::ShowMessage
         ("   ==> Now assign \"%s\" to \"%s\", rhsObj=%p\n",
          rhs.c_str(), lhs.c_str(), rhsObj);
      #endif
      
      // Now assign to LHS
      switch (lhsDataType)
      {
      case Gmat::BOOLEAN_TYPE:
         lhsWrapper->SetBoolean(bval);
         break;
      case Gmat::INTEGER_TYPE:
         {
            // Since it always creates NumberWrapper for numbers,
            // check both Integer and Real types
            if (rhsDataType == Gmat::INTEGER_TYPE)
            {
               lhsWrapper->SetInteger(ival);
            }
            else if (rhsDataType == Gmat::REAL_TYPE)
            {
               Integer itempval;
               std::string desc = rhsWrapper->GetDescription();
               if (GmatStringUtil::ToInteger(desc, itempval))
                  lhsWrapper->SetInteger(itempval);
               else
                  throw CommandException("Cannot set Real number to Integer");
            }
            break;
         }
      case Gmat::REAL_TYPE:
         if (rval != -99999.999)
            lhsWrapper->SetReal(rval);
         else
            throw CommandException("Cannot set Non-Real value to Real");
         break;
      case Gmat::RMATRIX_TYPE:
         lhsWrapper->SetArray(rmat);
         break;
      case Gmat::STRING_TYPE:
      case Gmat::ENUMERATION_TYPE:
         // Object to String is needed for Remove for Formation
         if (rhsObj != NULL)
         {
            lhsWrapper->SetString(rhsObj->GetName());
         }
         else if ((rhsDataType == Gmat::STRING_TYPE ||
                   rhsDataType == Gmat::ENUMERATION_TYPE ||
                   rhsDataType == Gmat::ON_OFF_TYPE))
         {
            lhsWrapper->SetString(sval);            
         }
         // We don't want to allow VARIALE to STRING assinment
         else if (rhsDataType == Gmat::REAL_TYPE &&
                  rhsWrapperType != Gmat::VARIABLE)
         {
            lhsWrapper->SetString(rhsWrapper->GetDescription());
         }
         else
         {
            CommandException ce;
            if (rhsObj != NULL)
               ce.SetDetails("Cannot set object of type \"%s\" to an undefined "
                             "object \"%s\"", rhsObj->GetTypeName().c_str(), lhs.c_str());
            else if (lhsWrapperType == Gmat::STRING_OBJECT &&
                     rhsWrapperType == Gmat::VARIABLE)
               ce.SetDetails("Cannot set objet of type \"Variable\" to object of "
                             "type \"String\"");
            else
               ce.SetDetails("Cannot set \"%s\" to an undefined object \"%s\"",
                             rhs.c_str(), lhs.c_str());
            throw ce;
         }
         break;
      case Gmat::ON_OFF_TYPE:
         lhsWrapper->SetOnOff(sval);
         break;
      case Gmat::OBJECT_TYPE:
         if (rhsObj == NULL)
            throw CommandException("Cannot set Non-Object type to object");
         else
            lhsWrapper->SetObject(rhsObj);
         break;
      case Gmat::STRINGARRAY_TYPE:
         if (rhsObj != NULL)
            lhsWrapper->SetString(rhsObj->GetName());
         else
         {
            CommandException ce;
            ce.SetDetails("Cannot set \"%s\" to \"%s\"", rhs.c_str(), lhs.c_str());
            throw ce;            
         }
         break;
      case Gmat::OBJECTARRAY_TYPE:
         // Object to String is needed for Add for Subscribers/Formation
         if (rhsObj != NULL)
            lhsWrapper->SetObject(rhsObj);
         else
         {
            bool errorCond = true;
            
            // Set individually if RHS has more than one object (loj: 2008.04.01)
            StringArray rhsValues;
            TextParser tp;
            if (rhs.find("{") != rhs.npos || rhs.find("}") != rhs.npos)
               rhsValues = tp.SeparateBrackets(rhs, "{}", " ,");
            
            for (UnsignedInt i=0; i<rhsValues.size(); i++)
            {
               GmatBase *obj = FindObject(rhsValues[i]);
               if (obj == NULL)
               {
                  errorCond = true;
                  break;
               }
               
               lhsWrapper->SetObject(obj);
               errorCond = false;
            }
            
            if (errorCond)
            {
               CommandException ce;
               ce.SetDetails("Cannot set \"%s\" to \"%s\"", rhs.c_str(), lhs.c_str());
               throw ce;
            }
         }
         break;
      default:
         throw CommandException("Unknown LHS type");
      }
      
      BuildCommandSummary(true);
      return true;
   }
   catch (BaseException &e)
   {
      // To make error message format consistent, just add "Command Exception:"
      std::string msg = e.GetFullMessage();
      if (msg.find("Exception") == msg.npos && msg.find("exception") == msg.npos)
         msg = "Command Exception: " + msg;
      
      CommandException ce;
      ce.SetMessage("");
      ce.SetDetails("%s in \n   \"%s\"\n", msg.c_str(), generatingString.c_str());
      throw ce;
   }
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
         wrapperObjectNames.push_back(rhs);
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
      ("Assignment::SetElementWrapper() toWrapper=%p, name=%s\n", toWrapper,
       withName.c_str());
   #endif
   
   if (toWrapper == NULL)
      return false;
   
   bool retval = false;
   
   #ifdef DEBUG_ASSIGNMENT_WRAPPER
   MessageInterface::ShowMessage
      ("   Setting wrapper \"%s\" on Assignment \"%s\"\n",
       withName.c_str(), GetGeneratingString(Gmat::NO_COMMENTS).c_str());
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

