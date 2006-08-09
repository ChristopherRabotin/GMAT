//$Header$
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
#include "MessageInterface.hpp"
#include "Parameter.hpp"
#include "Variable.hpp"
#include "Array.hpp"
#include "MathParser.hpp"
#include "StringUtil.hpp"
/// @todo Rework command so it doesn't need the Moderator!!!
#include "Moderator.hpp" 

//#define DEBUG_RENAME 1
//#define DEBUG_PARM_ASSIGNMENT
//#define DEBUG_ARRAY_INTERPRETING
//#define DEBUG_ASSIGNMENT_INIT 1
//#define DEBUG_EQUATION 1

//------------------------------------------------------------------------------
//  Assignment()
//------------------------------------------------------------------------------
/**
 * Constructs the Assignment command (default constructor).
 */
//------------------------------------------------------------------------------
Assignment::Assignment  () :
   GmatCommand          ("GMAT"),
   ownerName            (""),
   parmName             (""),
   parmOwner            (NULL),
   rhsObject            (NULL),
   rhsType              (NUMBER),
   rhsParmName          (""),
   objToObj             (false),
   parmID               (-1),
   parmType             (Gmat::UNKNOWN_PARAMETER_TYPE),
   value                ("Not_Set"),
   mathTree             (NULL),
   row                  (0),
   col                  (0),
   rowObj               (NULL),
   colObj               (NULL),
   lrow                 (""),
   lcol                 (""),
   lrowObj              (NULL),
   lcolObj              (NULL),
   rowIndex             (0),
   colIndex             (0),
   isLhsArray           (false)
{
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
   ownerName            (a.ownerName),
   parmName             (a.parmName),
   parmOwner            (a.parmOwner),
   rhsObject            (a.rhsObject),
   rhsType              (a.rhsType),
   rhsParmName          (a.rhsParmName),
   objToObj             (a.objToObj),
   parmID               (a.parmID),
   parmType             (a.parmType),
   value                (a.value),
   mathTree             (a.mathTree),
   row                  (a.row),
   col                  (a.col),
   rowObj               (NULL),
   colObj               (NULL),
   lrow                 (a.lrow),
   lcol                 (a.lcol),
   lrowObj              (NULL),
   lcolObj              (NULL),
   rowIndex             (a.rowIndex),
   colIndex             (a.colIndex),
   isLhsArray           (a.isLhsArray)
{
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
        
   ownerName   = a.ownerName;
   parmName    = a.parmName;
   parmOwner   = a.parmOwner;
   rhsObject   = a.rhsObject;
   rhsType     = a.rhsType;
   rhsParmName = a.rhsParmName;
   objToObj    = a.objToObj;
   parmID      = a.parmID;
   parmType    = a.parmType;
   value       = a.value;
   mathTree    = a.mathTree;
   row         = a.row;
   col         = a.col;
   rowObj      = NULL;
   colObj      = NULL;
   lrow        = a.lrow;
   lcol        = a.lcol;
   lrowObj     = NULL;
   lcolObj     = NULL;
   rowIndex    = a.rowIndex;
   colIndex     =a.colIndex;
   isLhsArray  = a.isLhsArray;

   return *this;
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
   #ifdef DEBUG_PARM_ASSIGNMENT
      MessageInterface::ShowMessage(
         "Assignment::InterpretAction entered\n");
   #endif

   Integer end = generatingString.find("%");
   std::string genString = generatingString.substr(0, end);
   
   #ifdef DEBUG_ARRAY_INTERPRETING
      MessageInterface::ShowMessage(
         "Comment-free generating string is %s\n", genString.c_str());
   #endif

   Integer loc = genString.find("GMAT", 0) + 4;
   const char *str = genString.c_str();
   while (str[loc] == ' ')
      ++loc;

   std::string lhs, rhs;
   UnsignedInt eqloc = genString.find("=", loc), lend, rstart;

   // Check to be sure equal sign is in place
   if (eqloc == std::string::npos)
      throw CommandException("Assignment string does not set value\n");
   
   lend = eqloc;
   rstart = eqloc;
   while ((str[lend] == '=') || (str[lend] == ' '))
      --lend;
   while ((str[rstart] == '=') || (str[rstart] == ' '))
      ++rstart;
   lhs = genString.substr(loc, lend - loc + 1);
   rhs = genString.substr(rstart);
   isLhsArray = false;
   
   if (lhs.find("(", 0) != std::string::npos)
      isLhsArray = true;

   if ((lhs.find("[", 0) != std::string::npos) ||
       (lhs.find("]", 0) != std::string::npos) ||
       (rhs.find("[", 0) != std::string::npos) ||
       (rhs.find("]", 0) != std::string::npos))
      throw CommandException("Syntax error in the assignment \"" +
                             generatingString + "\n");
   
   // Strip off trailing spaces and semicolon from RHS
   end = rhs.find(";");
   rhs = rhs.substr(0, end);
   lend = rhs.length() - 1;
   while (rhs[lend] == ' ')
      --lend;
   rhs = rhs.substr(0, lend + 1);
   
   #ifdef DEBUG_ARRAY_INTERPRETING
      MessageInterface::ShowMessage(
         "Left side is \"%s\"\nRight side is \"%s\"\nLeft side %s an array\n",
         lhs.c_str(), rhs.c_str(), (isLhsArray ? "is" : "is not"));
   #endif


   // Check if rhs is equation
   MathParser mp = MathParser();
   bool isRhsEquation = false;
   if (mp.IsEquation(rhs))
      isRhsEquation = true;
   
   end = genString.find(".", loc);
   
   if ((end == (Integer)std::string::npos) && !isLhsArray && !isRhsEquation)
   {
      // Script line is set to make one object the same as another, or to set a
      // value on a variable
      ownerName = lhs;
      value = rhs;
      objToObj = true;      
      return true;
   }
   
   
   // Parse RHS if equation
   if (isRhsEquation)
   {
      #if DEBUG_EQUATION
      MessageInterface::ShowMessage
         ("Assignment::InterpretAction() %s is an equation\n", rhs.c_str());
      #endif
      
      MathNode *topNode = mp.Parse(rhs);
      
      #if DEBUG_EQUATION
      if (topNode)
         MessageInterface::ShowMessage
            ("topNode=%s\n", topNode->GetTypeName().c_str());
      #endif
      
      mathTree = new MathTree("MathTree", rhs);
      mathTree->SetTopNode(topNode);
      ownerName = lhs;
      
      ////return true;
   }
   else
   {
      UnsignedInt dot = rhs.find(".", 0);
      // if rhs is a system parameter
      if (dot != rhs.npos)
      {
         Real rval;
         if (!GmatStringUtil::ToDouble(rhs, &rval))
         {
            // Get an instance if this is a Parameter
            Moderator *mod = Moderator::Instance();

            #ifdef DEBUG_PARM_ASSIGNMENT
            MessageInterface::ShowMessage
               ("=====> Assignment::InterpretAction() Creating RHS as parameter: %s\n",
                rhs.c_str());
            #endif
            
            std::string parmObj, parmType, parmDep;
            GmatStringUtil::ParseParameter(rhs, parmType, parmObj, parmDep);
            try
            {
               mod->CreateParameter(parmType, rhs, parmObj, parmDep);
            }
            catch (BaseException &ex)
            {
               #ifdef DEBUG_PARM_ASSIGNMENT
                  MessageInterface::ShowMessage
                     ("=====> Assignment::InterpretAction() Creating RHS not a "
                     "parameter; trying object member data: '%s' on '%s'\n", 
                     parmType.c_str(), parmObj.c_str());
               #endif
                             
               //throw;
            }
         }
      }
   }
   
   // Parse array handling elements
   if (isLhsArray)
   {
      // Break apart the array text into owner, row, and column
      UnsignedInt paren, comma, closeparen, temp;
      paren = lhs.find("(");
      comma = lhs.find(",");
      closeparen = lhs.find(")");

      if ((comma == std::string::npos) || (closeparen == std::string::npos))
         throw CommandException("Syntax error in the assignment \"" +
            generatingString +
            "\"\nArray assignments must specify row and column, separated by" +
            " a comma, in parentheses.\n");

      ownerName = lhs.substr(0, paren);
      lrow      = lhs.substr(paren + 1, comma - (paren + 1));
      lcol      = lhs.substr(comma + 1, closeparen - (comma + 1));
      
      // Trim the white space
      temp = 0;
      while (ownerName[temp] == ' ')
         ++temp;
      ownerName = ownerName.substr(temp);
      temp = ownerName.length();
      while (ownerName[temp] == ' ')
         --temp;
      ownerName = ownerName.substr(0, temp);

      temp = 0;
      while (lrow[temp] == ' ')
         ++temp;
      lrow = lrow.substr(temp);
      temp = lrow.length();
      while (lrow[temp] == ' ')
         --temp;
      lrow = lrow.substr(0, temp+1);

      temp = 0;
      while (lcol[temp] == ' ')
         ++temp;
      lcol = lcol.substr(temp);
      temp = lcol.length();
      while (lcol[temp] == ' ')
         --temp;
      lcol = lcol.substr(0, temp+1);

      value = rhs;

      #ifdef DEBUG_ARRAY_INTERPRETING
         MessageInterface::ShowMessage(
            "\n   Array is \"%s\", row \"%s\", column \"%s\"\n",
            ownerName.c_str(), lrow.c_str(), lcol.c_str());
      #endif

      return true;
   }

   
   end = lhs.find(".", 0);
   std::string component = lhs.substr(0, end);
   if (component == "")
      throw CommandException("Assignment string does not identify object\n");
   
   ownerName = component;
   loc = end + 1;

   component = lhs.substr(loc);
   if (component == "")
      throw CommandException("Assignment string does not identify parameter\n");
   
   /// @todo A hack for variables -- needs testing!
   if (ownerName != component)
      parmName = component;
   else
      parmName = "";
   
   // Strip off trailing white space, if any
   unsigned n = parmName.length() - 1;
   while ((parmName[n] == ' ') || (parmName[n] == '\t'))
      --n;
   parmName = parmName.substr(0, n+1);

   value = rhs;
   
   #ifdef DEBUG_ARRAY_INTERPRETING
      MessageInterface::ShowMessage(
         "\nOwner is \"%s\"\nParameter is \"%s\"\nValue is \"%s\"\n",
         ownerName.c_str(), parmName.c_str(), value.c_str());
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
         ("Assignment::Initialize() entered. ownerName=%s\n", ownerName.c_str());
   #endif
      
   if (GmatCommand::Initialize() == false)
      return false;

   if (objectMap->find(ownerName) != objectMap->end())
      parmOwner = (*objectMap)[ownerName];
   else
      throw CommandException
         ("Assignment command cannot find LHS object \"" +
          ownerName + "\" for line \n   " + generatingString + "\n");
   
   if (parmOwner)
   {
      #ifdef DEBUG_ASSIGNMENT_INIT
      MessageInterface::ShowMessage
         ("Assignment::Initialize() parmOwner=%s, %s, %p\n",
          parmOwner->GetTypeName().c_str(), parmOwner->GetName().c_str(), parmOwner);
      #endif
   }

   if (objToObj)
   {
      if (objectMap->find(value) != objectMap->end())
      {
         rhsObject = (*objectMap)[value];
         return true;
      }
      else
      {
         if (parmOwner->GetTypeName() == "Variable")
         {
            parmName = "Expression";
            objToObj = false;
            #ifdef DEBUG_PARM_ASSIGNMENT
               MessageInterface::ShowMessage(
                  "Assignment::Initialize has owner %s, name %s, and val %s\n",
                  ownerName.c_str(), parmName.c_str(), value.c_str());
            #endif
         }
         else
            throw CommandException("Assignment command cannot find object \"" +
                     value + "\" for line \n   " + generatingString + "\n");
      }
   }
   
   // Find the object
   if (objectMap->find(ownerName) == objectMap->end())
      throw CommandException("Assignment command cannot find object \"" +
                             ownerName + "\" for line \n   " + generatingString + "\n");

   // Initialize RHS MathTree
   if (mathTree != NULL)
   {
      MathNode *topNode = mathTree->GetTopNode();

      #if DEBUG_EQUATION
      MessageInterface::ShowMessage
         ("Assignment::Initialize() Initializing topNode=%s, %s\n",
          topNode->GetTypeName().c_str(), topNode->GetName().c_str());
      #endif
      
      if (mathTree->Initialize(objectMap))
      {
         if (!topNode->ValidateInputs())
            throw CommandException("Failed to validate equation inputs: " +
                                   generatingString + "\n");
      }
      else
      {
         throw CommandException("Failed to initialize equation: " +
                                generatingString + "\n");
      }
   }
   
   
   if (isLhsArray)
   {
      //MessageInterface::ShowMessage("=====> process isLhsArray part\n");
      
      if (parmOwner->GetTypeName() != "Array")
         throw CommandException(
            "Attemping to treat " + parmOwner->GetTypeName() + " named " +
            parmOwner->GetName() + " like an Array object.\n");

      // Build row index
      if (lrow == ":")
         rowIndex = -1;
      else
      {
         if (objectMap->find(lrow) == objectMap->end())
         {
            rowIndex = atoi(lrow.c_str());
            if (rowIndex < 0)
               throw CommandException(
                  "Attempting to use an invalid (negative) row index for " +
                  ownerName + "\n");
            if (rowIndex == 0)
               throw CommandException(
                  "Attempting to use an invalid row index (0 -- arrays are "
                  "indexed from 1) for " + ownerName + "\n");
         }
         else
            lrowObj = (*objectMap)[lrow];
      }
      
      // Build column index
      if (lcol == ":")
         colIndex = -1;
      else
      {
         if (objectMap->find(lcol) == objectMap->end())
         {
            colIndex = atoi(lcol.c_str());
            if (colIndex < 0)
               throw CommandException(
                  "Attempting to use an invalid (negative) column index for " +
                  ownerName + "\n");
            if (colIndex == 0)
               throw CommandException(
                  "Attempting to use an invalid column index (0 -- arrays "
                  "are indexed from 1) for " + ownerName + "\n");
         }
         else
            lcolObj = (*objectMap)[lcol];
      }
   }

   // if RHS is not a equation, initialize
   if (mathTree == NULL)
      return InitializeRHS(value);
   else
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
   #ifdef DEBUG_ARRAY_INTERPRETING
      MessageInterface::ShowMessage("\nAssignment::Execute() entered for " +
         generatingString + "\n");
   #endif
   bool retval = false;

   if (isLhsArray)
   {
      #ifdef DEBUG_ARRAY_INTERPRETING
      MessageInterface::ShowMessage("   Executing lhs array branch\n");
      #endif
      
      if (lrowObj)
      {
         if (lrowObj->GetTypeName() == "Variable")
            rowIndex = (Integer)(((Parameter*)lrowObj)->GetReal());
         else
            throw CommandException
               ("Non-\"Variable\" Objects (" + lrowObj->GetName() +
                ") cannot be used to set row indexes yet.");
      }
      if (lcolObj)
      {
         if (lcolObj->GetTypeName() == "Variable")
            colIndex = (Integer)(((Parameter*)lcolObj)->GetReal());
         else
            throw CommandException
               ("Non-\"Variable\" Objects (" + lcolObj->GetName() +
                ") cannot be used to set column indexes yet.");
      }
      if (rowIndex == -1)
         throw CommandException
            ("Multiple array row elements cannot be set yet.");
      if (colIndex == -1)
         throw CommandException
            ("Multiple array column elements cannot be set yet.");
      
      #if DEBUG_ARRAY_INTERPRETING
      MessageInterface::ShowMessage
         ("   rowIndex=%d, colIndex=%d\n", rowIndex, colIndex);
      #endif
   }
   
   
   // if there is MathTree, evaluate
   if (mathTree != NULL)
   {      
      Integer returnType;
      Integer numRow;
      Integer numCol;
      Parameter *lhsParm = (Parameter*)parmOwner;
      
      MathNode *topNode = mathTree->GetTopNode();
      
      if (topNode)
      {
         #if DEBUG_EQUATION
         MessageInterface::ShowMessage
            ("Assignment::Execute() topNode=%s, %s\n", topNode->GetTypeName().c_str(),
             topNode->GetName().c_str());
         #endif
         
         topNode->GetOutputInfo(returnType, numRow, numCol);
         
         if (returnType == Gmat::REAL_TYPE)
         {
            Real rval = topNode->Evaluate();
            
            #if DEBUG_EQUATION
            MessageInterface::ShowMessage("Assignment::Execute() rval=%f\n", rval);
            #endif
            
            // set lhsParm here
            if (lhsParm->GetTypeName() == "Variable")
            {
               lhsParm->SetReal(rval);
            }
            else if (lhsParm->GetTypeName() == "Array")
            {
               parmOwner->SetRealParameter("SingleValue", rval, rowIndex-1,
                                           colIndex-1);
            }
            else
            {
               parmID    = parmOwner->GetParameterID(parmName);
               parmType  = parmOwner->GetParameterType(parmID);

               #if DEBUG_EQUATION
               MessageInterface::ShowMessage
                  ("Assignment::Execute() parmName=%s, lhsParm=%s, TypeName=%s, "
                   "parmID=%d, parmType=%d\n", parmName.c_str(),
                   lhsParm->GetName().c_str(), lhsParm->GetTypeName().c_str(),
                   parmID, parmType);
               #endif
               
               if (parmType == Gmat::REAL_TYPE)
               {
                  parmOwner->SetRealParameter(parmID, rval);
               }               
               else
               {
                  throw CommandException
                     ("Expects LHS type to be a Variable or Array or Real "
                      "parameter. Object type: " + lhsParm->GetTypeName() +
                      "Object name: " + lhsParm->GetName() + "\n");
               }
            }
         }
         else
         {
            Rmatrix rmat(numRow, numCol);
            rmat = topNode->MatrixEvaluate();

            #if DEBUG_EQUATION
            MessageInterface::ShowMessage
               ("Assignment::Execute() numRow=%d, numCol=%d, rmat=\n%s\n",
                numRow, numCol, rmat.ToString().c_str());
            #endif
            
            // set lhsParm here
            if (lhsParm->GetTypeName() == "Array")
            {
               #if DEBUG_EQUATION
               Array *arr = (Array*)lhsParm;
               MessageInterface::ShowMessage
                  ("Assignment::Execute() LHS Array numRow=%d, numCol=%d\n",
                   arr->GetRowCount(), arr->GetColCount());
               #endif
               
               lhsParm->SetRmatrix(rmat);
            }
            else
               throw CommandException("Expects LHS type to be an Array, but it's " +
                                      lhsParm->GetTypeName());
         }
         
         BuildCommandSummary(true);
         return true;
      }
      else
      {
         throw CommandException("RHS is a equation, but top node is NULL\n");
      }
   }

   
   // Get the parameter ID and ID type
   try
   {
      #ifdef DEBUG_ARRAY_INTERPRETING
         MessageInterface::ShowMessage("   In the try clause\n");
      #endif
      if (parmOwner == NULL)
         //throw CommandException("Parameter Owner Not Initialized\n");
         throw CommandException("Cannot find LHS Parameter: " + ownerName + "\n");
      
      if (isLhsArray)
      {
         parmOwner->SetRealParameter("SingleValue", EvaluateRHS(), rowIndex-1,
            colIndex-1);

         BuildCommandSummary(true);
         return true;
      }

      if (objToObj)
      {
         #ifdef DEBUG_ARRAY_INTERPRETING
            MessageInterface::ShowMessage(
               "   Executing object to object branch\n");
         #endif

         if (!rhsObject)
            throw CommandException("Assignment command cannot find object \"" +
               value + "\"");
         if (parmOwner->GetTypeName() != rhsObject->GetTypeName())
            throw CommandException("Mismatched object types between \"" +
               parmOwner->GetName() + "\" and \"" + rhsObject->GetName() +
               "\"");
         parmOwner->Copy(rhsObject);
         
         BuildCommandSummary(true);
         return true;
      }
      
      // Once inline equation handling is implemented, variables will do this:
      // if (parmOwner->GetTypeName() == "Variable")
      //    return ((Variable*)parmOwner)->EvaluateReal();
      
      #ifdef DEBUG_PARM_ASSIGNMENT
         MessageInterface::ShowMessage("   Executing parameter setting\n");
      #endif

      bool isVariable = false;
      if (parmOwner->GetTypeName() == "Variable")
      {
         isVariable = true;
         parmType = Gmat::REAL_TYPE;
      }
      else
      {
         parmID    = parmOwner->GetParameterID(parmName);
         parmType  = parmOwner->GetParameterType(parmID);
      }

      #ifdef DEBUG_PARM_ASSIGNMENT
      if (!isVariable)
         MessageInterface::ShowMessage("Assignment::Execute Parameter %s has "
            "type %s\n", parmName.c_str(),
            parmOwner->GetParameterTypeString(parmID).c_str());
      #endif

      switch (parmType)
      {
         case Gmat::INTEGER_TYPE:
            parmOwner->SetIntegerParameter(parmID, (Integer)EvaluateRHS());
            retval = true;
            break;
               
         case Gmat::REAL_TYPE:
            #ifdef DEBUG_PARM_ASSIGNMENT
            if (!isVariable)
               MessageInterface::ShowMessage("Setting %s on %s to %lf\n", 
                  parmOwner->GetParameterText(parmID).c_str(), 
                  parmOwner->GetName().c_str(), EvaluateRHS());
            else
               MessageInterface::ShowMessage("Setting %s to %lf\n", 
                  parmOwner->GetName().c_str(), EvaluateRHS());
            #endif
            if (isVariable)
               ((Variable *)parmOwner)->SetReal(EvaluateRHS());
            else
               parmOwner->SetRealParameter(parmID, EvaluateRHS());
            retval = true;
            break;
               
         case Gmat::STRING_TYPE:
         case Gmat::STRINGARRAY_TYPE:
            #ifdef DEBUG_PARM_ASSIGNMENT
               MessageInterface::ShowMessage("Assignment::Execute setting "
                  "string to %s\n", value.c_str());
            #endif
            parmOwner->SetStringParameter(parmID, value);
            retval = true;
            break;
               
         case Gmat::BOOLEAN_TYPE:
            bool tf;
            if (value == "true")
               tf = true;
            else
               tf = false;
            parmOwner->SetBooleanParameter(parmID, tf);
            retval = true;
            break;
   
         default:
            break;
      }

       // "Add" parameters could also mean to set reference objects
      if (parmName == "Add")
      {
         if (objectMap->find(value) != objectMap->end())
         {
            GmatBase *obj = (*objectMap)[value];
            if (obj)
               parmOwner->SetRefObject(obj, obj->GetType(), value);
         }
      }
   }
   catch (BaseException& ex)
   {
      if (parmOwner == NULL)
         throw;
      // Could be an action rather than a parameter
      if (!parmOwner->TakeAction(parmName, value))
         throw;
       
      retval = true;
   }

   #ifdef DEBUG_PARM_ASSIGNMENT
      MessageInterface::ShowMessage("Assignment::Execute finished\n");
   #endif
   
   BuildCommandSummary(true);
   
   return retval;
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
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Assignment::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   MessageInterface::ShowMessage
      ("   value=%s\n", value.c_str());
   #endif
   
   if (ownerName == oldName)
      ownerName = newName;

   if (parmName == oldName)
      parmName = newName;

   if (rhsParmName == oldName)
      rhsParmName = newName;

   // Since parameter name is composed of spacecraftName.dep.paramType or
   // burnName.dep.paramType, check the type first
   if (type == Gmat::SPACECRAFT || type == Gmat::BURN ||
       type == Gmat::COORDINATE_SYSTEM || type == Gmat::CALCULATED_POINT)
   {
      if (value.find(oldName) != value.npos)
         value = GmatStringUtil::Replace(value, oldName, newName);
   }
   
   if (mathTree != NULL)      
      mathTree->RenameRefObject(type, oldName, newName);
   
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
   std::string gen = prefix + "GMAT " + ownerName;
   if (parmName != "")
      gen += "." + parmName;

   // Could be setting an array element
   if ((lrow != "") || (lcol != ""))
   {
      // Handle row and column separately, in case we allow ommission of one in
      // the future.
      gen += "(";
      if (lrow != "")
      {
         gen += lrow;
         if (lcol != "")
            gen += ", ";
      }
      if (lcol != "")
         gen += lcol;
      gen += ")";
   }
   
   gen += " = " + value + ";";
   
   #ifdef DEBUG_ASSIGNMENT_SCRIPTING
      MessageInterface::ShowMessage("Assignment command generator is \"%s\"\n",
         gen.c_str());
   #endif
   generatingString = gen;

   return generatingString;
}


//------------------------------------------------------------------------------
// bool InitializeRHS(const std::string &rhs)
//------------------------------------------------------------------------------
/**
 * Initializes the objects used on the right side of the assignemnt.
 *
 * @param <rhs> The string on the right side of the equals sign.
 *
 * @return true on success, false on failure.
 *
 * @todo Handle RHS Variables and Parameters
 */
//------------------------------------------------------------------------------
bool Assignment::InitializeRHS(const std::string &rhs)
{
   #ifdef DEBUG_PARM_ASSIGNMENT
      MessageInterface::ShowMessage("Assignment::InitializeRHS(%s) entered\n",
         rhs.c_str());
   #endif
   
   std::string chunk, kind, subchunk;
   Integer start = 0, end;

   // Skip leading spaces
   while (rhs[start] == ' ')
      ++start;
   end = start;
   // Find the next delimiter
   while ((rhs[end] != ';')  && (rhs[end] != ' ') &&
          (rhs[end] != '.')  && (rhs[end] != '=') &&
          (rhs[end] != '(')  && (rhs[end] != ',') &&
          (rhs[end] != '\t') && (end < (Integer)rhs.length()))
      ++end;

   #ifdef DEBUG_PARM_ASSIGNMENT
      MessageInterface::ShowMessage("   length = %d, end = %d\n",
         rhs.length(), end);
   #endif

   chunk = rhs.substr(start, end - start);
   if (end < (Integer)rhs.length())
      subchunk = rhs.substr(end);
   else
      subchunk = "";

   if (subchunk[0] == '.')
      subchunk = subchunk.substr(1);

   #ifdef DEBUG_PARM_ASSIGNMENT
      MessageInterface::ShowMessage(
         "'%s' was broken into '%s' and '%s'\n", rhs.c_str(), chunk.c_str(),
         subchunk.c_str());
   #endif
   
   // Code for inline equations.  Right now the parameter database doesn't know
   // about Parameters on the right side of the =, so this doesn't work yet
   // if (parmOwner->GetTypeName() == "Variable")
   // {
   //    parmOwner->SetStringParameter("Expression", rhs);
   // }
   // else
   
   // Handle RHS system parameter
   std::string name = chunk;
   if (rhs.find('.') != rhs.npos)
   {
      //if (!isalpha(name[0]))
      if (isalpha(name[0]))
         name = rhs;
   }

   #ifdef DEBUG_PARM_ASSIGNMENT
      MessageInterface::ShowMessage("Locating the object %s\n", name.c_str());
   #endif
   
   //if (objectMap->find(chunk) != objectMap->end())
   if (objectMap->find(name) != objectMap->end())
   {
      //rhsObject = (*objectMap)[chunk];
      rhsObject = (*objectMap)[name];
      kind = rhsObject->GetTypeName();

      #ifdef DEBUG_PARM_ASSIGNMENT
         MessageInterface::ShowMessage(
            "Assignment RHS object is a %s named %s\n", kind.c_str(),
            rhsObject->GetName().c_str());
      #endif

      // Determine what type of object handling is needed
      if (kind == "Array")
      {
         rhsType = ARRAY_ELEMENT;
         std::string rowStr, colStr = "1";
         
         unsigned openParen, comma, closeParen, index;
         openParen  = subchunk.find("(");
         comma      = subchunk.find(",");
         closeParen = subchunk.find(")");
         
         if ((openParen == std::string::npos) ||
             (closeParen == std::string::npos))
            throw CommandException(
                      "Assignment::InitializeRHS encountered mismatched "
                      "parenthesis\n   Command text is \"" +
                      (GetGeneratingString()) + "\"");
         if (comma != std::string::npos)
         {
            colStr = subchunk.substr(comma+1, closeParen - (comma+1));
            closeParen = comma;
         }
         rowStr = subchunk.substr(openParen+1, closeParen - (openParen+1));
            
         // Trim whitespace
         index = 0;
         while ((rowStr[index] == ' ') && (index < rowStr.length()))
            ++index;
         rowStr = rowStr.substr(index);
         index = rowStr.length() - 1;
         while ((rowStr[index] == ' ') && (index >= 0))
            --index;
         rowStr = rowStr.substr(0, index+1);

         index = 0;
         while ((colStr[index] == ' ') && (index < colStr.length()))
            ++index;
         colStr = colStr.substr(index);
         index = colStr.length() - 1;
         while ((colStr[index] == ' ') && (index >= 0))
            --index;
         colStr = colStr.substr(0, index+1);

         #ifdef DEBUG_PARM_ASSIGNMENT
            MessageInterface::ShowMessage(
               "   Array element given by ('%s', '%s')\n", rowStr.c_str(),
               colStr.c_str());
         #endif

         if (objectMap->find(rowStr) == objectMap->end())
         {
            row = atoi(rowStr.c_str()) - 1;
         }
         else
            throw CommandException(
               "Assignment command cannot handle dynamic row indices yet.");

         if (objectMap->find(colStr) == objectMap->end())
         {
            col = atoi(colStr.c_str()) - 1;
         }
         else
            throw CommandException(
               "Assignment command cannot handle dynamic column indices yet.");
      }
      else if (kind == "Variable")
      {
         rhsType = VARIABLE;
      }
      else if (rhsObject->GetType() == Gmat::PARAMETER)
      {
         //throw CommandException(
         //   "Assignment command cannot handle Parameters yet.\n");
         rhsType = PARAMETER;
      }
      else
      {
         #ifdef DEBUG_PARM_ASSIGNMENT
            MessageInterface::ShowMessage("%s is an object parm\n",
               subchunk.c_str());
         #endif
         rhsParmName = subchunk;
         rhsType = OBJECT_PARM;
      }
   }
   else
   {
      Real rval;
      if (GmatStringUtil::ToDouble(rhs, &rval))
      {
         #ifdef DEBUG_PARM_ASSIGNMENT
         MessageInterface::ShowMessage(
            "Assignment RHS object is the number %le\n", atof(rhs.c_str()));
         #endif
         rhsType = NUMBER;
      }
      else
      {
         if (rhs != "On" && rhs != "Off")
            throw CommandException
               ("Assignment command cannot find RHS parameter: " + rhs + "\n");
      }
   }

   return true;
}


//------------------------------------------------------------------------------
// Real EvaluateRHS()
//------------------------------------------------------------------------------
/**
 * Initializes the objects used on the right side of the assignemnt.
 *
 * @return The floating point number corresponding to the right hand side.
 *
 * @todo Handle RHS Variables and Parameters
 *
 * @tod0 Determine how to handle strings in Assignment command.
 */
//------------------------------------------------------------------------------
Real Assignment::EvaluateRHS()
{
   //MessageInterface::ShowMessage
   //   ("===> Assignment::EvaluateRHS() rhsType=%d\n", rhsType);
   
   // RHS could be a parameter, an array element, a variable, or just a number
   switch (rhsType)
   {
      case VARIABLE:
      case PARAMETER:
      {
         Real value = ((Parameter*)rhsObject)->EvaluateReal();

         #ifdef DEBUG_PARM_ASSIGNMENT
            MessageInterface::ShowMessage("Evaluating: the %s named %s = %lf\n", 
               rhsObject->GetTypeName().c_str(), rhsObject->GetName().c_str(),
               value);
         #endif

         return value;
      }

      case ARRAY_ELEMENT:
         if (rowObj != NULL)
            if (rowObj->GetType() == Gmat::PARAMETER)
               row = (Integer)((Parameter*)rowObj)->EvaluateReal();
         if (colObj != NULL)
            if (colObj->GetType() == Gmat::PARAMETER)
               col = (Integer)((Parameter*)colObj)->EvaluateReal();
         #ifdef DEBUG_PARM_ASSIGNMENT
            MessageInterface::ShowMessage(
               "   Getting 'SingleValue' for (%d, %d)\n", row, col);
         #endif
         return rhsObject->GetRealParameter("SingleValue", row, col);

      case OBJECT_PARM:
         #ifdef DEBUG_PARM_ASSIGNMENT
            MessageInterface::ShowMessage(
               "   Getting '%s' for %s\n", rhsParmName.c_str(),
               rhsObject->GetName().c_str());
         #endif
            return rhsObject->GetRealParameter(rhsParmName);
      default:
         break;
   }
   
   #ifdef DEBUG_PARM_ASSIGNMENT
      MessageInterface::ShowMessage("%s is just a number, right?\n", value.c_str());
   #endif
   
   // It's just a number
   return atof(value.c_str());
}
