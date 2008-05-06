//$Id$
//------------------------------------------------------------------------------
//                                   MathTree
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Allison Greene
// Created: 2006/04/17
//
/**
 * Defines the MathTree base class used for Math in Scripts.
 */
//------------------------------------------------------------------------------
#include "MathTree.hpp"
#include "MathFunction.hpp"
#include "MathElement.hpp"
#include "FunctionRunner.hpp"
#include "StringUtil.hpp"            // for GetArrayIndex()
#include "InterpreterException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_MATH_TREE 1
//#define DEBUG_MATH_TREE_INIT 1
//#define DEBUG_MATH_WRAPPERS
//#define DEBUG_RENAME
//#define DEBUG_FUNCTION

//------------------------------------------------------------------------------
//  MathTree()
//------------------------------------------------------------------------------
/**
 * Constructs the MathTree object (default constructor).
 */
//------------------------------------------------------------------------------
MathTree::MathTree(const std::string &typeStr, const std::string &nomme) :
   GmatBase(Gmat::MATH_TREE, typeStr, nomme),
   theTopNode(NULL)
{
}


//------------------------------------------------------------------------------
//  ~MathTree(void)
//------------------------------------------------------------------------------
/**
 * Destroys the MathTree object (destructor).
 */
//------------------------------------------------------------------------------
MathTree::~MathTree()
{
   // Need to delete from the bottom nodes
   if (theTopNode)
   {
      //MessageInterface::ShowMessage
      //   ("==> MathTree::~MathTree() deleting theTopNode:%s, %s\n", GetTypeName().c_str(),
      //    GetName().c_str());
      delete theTopNode;
   }
}


//------------------------------------------------------------------------------
//  MathTree(const MathTree &mt)
//------------------------------------------------------------------------------
/**
 * Constructs the MathTree object (copy constructor).
 * 
 * @param <mn> Object that is copied
 */
//------------------------------------------------------------------------------
MathTree::MathTree(const MathTree &mt) :
   GmatBase           (mt),
   theTopNode         (mt.theTopNode),
   theObjectMap       (NULL),
   theGlobalObjectMap (NULL)
{
}


//------------------------------------------------------------------------------
//  MathTree& operator=(const MathTree &mt)
//------------------------------------------------------------------------------
/**
 * Sets one MathTree object to match another (assignment operator).
 * 
 * @param <mt> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
MathTree& MathTree::operator=(const MathTree &mt)
{
   if (this == &mt)
      return *this;
    
   GmatBase::operator=(mt);
   
   theTopNode         = mt.theTopNode;
   theObjectMap       = NULL;
   theGlobalObjectMap = NULL;
   
   return *this;
}


//------------------------------------------------------------------------------
// const StringArray& GetGmatFunctionNames()
//------------------------------------------------------------------------------
const StringArray& MathTree::GetGmatFunctionNames()
{
   #ifdef DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("MathTree::GetGmatFunctionNames() returning %d GmatFunctions\n",
       theGmatFunctionNames.size());
   #endif
   
   return theGmatFunctionNames;
}


//------------------------------------------------------------------------------
// void SetGmatFunctionNames(StringArray funcList)
//------------------------------------------------------------------------------
void MathTree::SetGmatFunctionNames(StringArray funcList)
{
   theGmatFunctionNames = funcList;
}


//------------------------------------------------------------------------------
// std::vector<Function*> GetFunctions() const
//------------------------------------------------------------------------------
std::vector<Function*> MathTree::GetFunctions() const
{
   return theFunctions;
}


//------------------------------------------------------------------------------
// void SetFunction(Function *function)
//------------------------------------------------------------------------------
void MathTree::SetFunction(Function *function)
{
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathTree::SetFunction() function=%p\n", function);
   #endif
   
   if (theTopNode == NULL)
      return;
   
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("   Calling SetFunctionToRunner() theTopNode=%s, %s\n",
       theTopNode->GetTypeName().c_str(), theTopNode->GetName().c_str());
   #endif
   
   SetFunctionToRunner(theTopNode, function);
   
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage("MathTree::SetFunction() returning\n");
   #endif
}


//------------------------------------------------------------------------------
// MathNode* GetTopNode()
//------------------------------------------------------------------------------
MathNode* MathTree::GetTopNode()
{
   return theTopNode;
}


//------------------------------------------------------------------------------
// void SetTopNode(MathNode *node)
//------------------------------------------------------------------------------
void MathTree::SetTopNode(MathNode *node)
{
   theTopNode = node;
}


//------------------------------------------------------------------------------
// void SetMathWrappers(std::map<std::string, ElementWrapper*> *wrapperMap)
//------------------------------------------------------------------------------
void MathTree::SetMathWrappers(std::map<std::string, ElementWrapper*> *wrapperMap)
{
   if (theTopNode == NULL)
      return;
   
   theWrapperMap = wrapperMap;
   
   #ifdef DEBUG_MATH_WRAPPERS
   MessageInterface::ShowMessage
      ("MathTree::SetMathWrappers() theWrapperMap=%p, theTopNode=%s, %s\n",
       theWrapperMap, theTopNode->GetTypeName().c_str(), theTopNode->GetName().c_str());
   #endif
   
   SetMathElementWrappers(theTopNode);
}


//------------------------------------------------------------------------------
// void Evaluate() const
//------------------------------------------------------------------------------
Real MathTree::Evaluate()
{
   #ifdef DEBUG_MATH_TREE
   MessageInterface::ShowMessage
      ("MathTree::Evaluate() theTopNode=%s, %s\n", theTopNode->GetTypeName().c_str(),
       theTopNode->GetName().c_str());
   #endif
   return theTopNode->Evaluate();
}


//------------------------------------------------------------------------------
// Rmatrix *MatrixEvaluate()
//------------------------------------------------------------------------------
Rmatrix MathTree::MatrixEvaluate()
{
   #ifdef DEBUG_MATH_TREE
   MessageInterface::ShowMessage
      ("MathTree::MatrixEvaluate() theTopNode=%s, %s\n", theTopNode->GetTypeName().c_str(),
       theTopNode->GetName().c_str());
   #endif
   return theTopNode->MatrixEvaluate();
}


//------------------------------------------------------------------------------
// bool Initialize(std::map<std::string, GmatBase *> *objectMap,
//                 std::map<std::string, GmatBase *> *globalObjectMap )
//------------------------------------------------------------------------------
bool MathTree::Initialize(std::map<std::string, GmatBase *> *objectMap,
                          std::map<std::string, GmatBase *> *globalObjectMap )
{
   if (theTopNode == NULL)
      return true;

   theObjectMap       = objectMap;
   theGlobalObjectMap = globalObjectMap;

   #ifdef DEBUG_MATH_TREE
   MessageInterface::ShowMessage
      ("MathTree::Initialize() theTopNode=%s, %s\n", theTopNode->GetTypeName().c_str(),
       theTopNode->GetName().c_str());
   #endif
   
   return InitializeParameter(theTopNode);
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount) const
//------------------------------------------------------------------------------
void MathTree::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount) 
{
    theTopNode->GetOutputInfo(type, rowCount, colCount);
}


//---------------------------------------------------------------------------
// virtual bool RenameRefObject(const Gmat::ObjectType type,
//                 const std::string &oldName, const std::string &newName)
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
bool MathTree::RenameRefObject(const Gmat::ObjectType type,
                               const std::string &oldName,
                               const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("MathTree::RenameRefObject() oldName=<%s>, newName=<%s>\n", oldName.c_str(),
       newName.c_str());
   #endif
   
   if (theTopNode)
      RenameParameter(theTopNode, type, oldName, newName);
   
   return true;
}


//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
const StringArray& MathTree::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   theAllParamArray.clear();
   
   if (theTopNode)
      CreateParameterNameArray(theTopNode);
   
   return theAllParamArray;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the MathTree.
 *
 * @return clone of the MathTree.
 *
 */
//------------------------------------------------------------------------------
GmatBase* MathTree::Clone(void) const
{
   return (new MathTree(*this));
}


//------------------------------------------------------------------------------
// bool InitializeParameter(MathNode *node)
//------------------------------------------------------------------------------
bool MathTree::InitializeParameter(MathNode *node)
{
   if (node == NULL)
      return true;
   
   if (!node->IsFunction())
   {
      if (node->IsNumber())
         return true;
      
      // Now MathElement can have more than one ref objects due to GmatFunction
      // input arguments.
      StringArray refNames = node->GetRefObjectNameArray(Gmat::PARAMETER);
      std::string undefNames;
      
      for (UnsignedInt i=0; i<refNames.size(); i++)
      {
         #ifdef DEBUG_MATH_TREE_INIT
         MessageInterface::ShowMessage
            ("MathTree::InitializeParameter() refNames[%d]=%s\n", refNames[i].c_str());
         #endif
         
         // Handle array index
         Integer row, col;
         std::string newName;
         GmatStringUtil::GetArrayIndex(refNames[i], row, col, newName);
         
         if (theObjectMap->find(newName) != theObjectMap->end())
         {
            node->SetRefObject((*theObjectMap)[newName], Gmat::PARAMETER,
                               newName);
            
            #ifdef DEBUG_MATH_TREE_INIT
            MessageInterface::ShowMessage
               ("MathTree::InitializeParameter() Found %s from theObjectMap\n",
                refNames[i].c_str());
            #endif
         }
         else if (theGlobalObjectMap->find(newName) != theGlobalObjectMap->end())
         {
            node->SetRefObject((*theGlobalObjectMap)[newName], Gmat::PARAMETER,
                               newName);
            
            #ifdef DEBUG_MATH_TREE_INIT
            MessageInterface::ShowMessage
               ("MathTree::InitializeParameter() Found %s from theGlobalObjectMap\n",
                refNames[i].c_str());
            #endif
         }
         else
         {
            #ifdef DEBUG_MATH_TREE_INIT
            MessageInterface::ShowMessage
               ("MathTree::InitializeParameter() Unable to find " + newName +
                " from theObjectMap or theGlobalObjectMap\n");
            #endif
            
            undefNames = undefNames + ", " + newName;
         }
      }
      
      if (undefNames == "")
         return true;
      else
         throw InterpreterException("Undefined variable(s) \"" + undefNames + "\" used");
   }
   else
   {
      MathNode *left = node->GetLeft();
      bool result1 = InitializeParameter(left);
      
      MathNode *right = node->GetRight();
      bool result2 = InitializeParameter(right);
      
      return (result1 && result2);
   }
}


//------------------------------------------------------------------------------
// void SetMathElementWrappers(MathNode *node)
//------------------------------------------------------------------------------
void MathTree::SetMathElementWrappers(MathNode *node)
{
   if (node == NULL)
      return;
   
   if (!node->IsFunction())
   {
      if (node->IsNumber())
         return;
      
      #ifdef DEBUG_MATH_WRAPPERS
      MessageInterface::ShowMessage
         ("   Setting wrappers to '%s'\n", node->GetName().c_str());
      #endif
      
      ((MathElement*)(node))->SetMathWrappers(theWrapperMap);
   }
   else
   {
      MathNode *left = node->GetLeft();
      SetMathElementWrappers(left);
      
      MathNode *right = node->GetRight();
      SetMathElementWrappers(right);
   }
}


//------------------------------------------------------------------------------
// void SetFunctionToRunner(MathNode *node, Function *function)
//------------------------------------------------------------------------------
void MathTree::SetFunctionToRunner(MathNode *node, Function *function)
{
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathTree::SetFunctionToRunner() node=%p, function=%p\n", node, function);
   #endif
   
   if (node == NULL)
      return;
   
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("   node type='%s', name='%s'\n", node->GetTypeName().c_str(),
       node->GetName().c_str());
   #endif
   
   if (!node->IsFunction())
      return;
   
   if (node->IsOfType("FunctionRunner"))
      ((FunctionRunner*)(node))->SetFunction(function);
   
   MathNode *left = node->GetLeft();   
   SetFunctionToRunner(left, function);
   
   MathNode *right = node->GetRight();
   SetFunctionToRunner(right, function);
}


//------------------------------------------------------------------------------
// bool RenameParameter(MathNode *node, const Gmat::ObjectType type,
//                      const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
bool MathTree::RenameParameter(MathNode *node, const Gmat::ObjectType type,
                               const std::string &oldName,
                               const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("MathTree::RenameParameter() oldName=<%s>, newName=<%s>\n", oldName.c_str(),
       newName.c_str());
   #endif
   
   if (node == NULL)
      return true;
   
   std::string nodeName = node->GetName();
   
   if (nodeName.find(oldName) != nodeName.npos)
   {
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage("   old nodeName=<%s>\n", nodeName.c_str());
      #endif
   
      nodeName = GmatStringUtil::ReplaceName(nodeName, oldName, newName);
      node->SetName(nodeName);
   
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage("   new nodeName=<%s>\n", nodeName.c_str());
      #endif
   }
   
   if (!node->IsFunction())
   {
      if (!node->IsNumber())
         node->RenameRefObject(type, oldName, newName);
   }
   else
   {
      //loj: ================ try getting node name here
      MathNode *left = node->GetLeft();
      RenameParameter(left, type, oldName, newName);
      
      MathNode *right = node->GetRight();
      RenameParameter(right, type, oldName, newName);
   }
   
   return true;
}


//------------------------------------------------------------------------------
// void CreateParameterNameArray(MathNode *node)
//------------------------------------------------------------------------------
void MathTree::CreateParameterNameArray(MathNode *node)
{
   theParamArray.clear();
   
   if (node == NULL)
      return;
   
   #ifdef DEBUG_MATH_WRAPPERS
   MessageInterface::ShowMessage
      ("MathTree::GetRefObjectNameArray() node=%s\n", node->GetName().c_str());
   #endif
   
   if (!node->IsFunction())
   {
      // if math element is not a number
      if (!node->IsNumber())
      {
         StringArray refs = node->GetRefObjectNameArray(Gmat::PARAMETER);
         for (UnsignedInt i=0; i<refs.size(); i++)
         {
            #ifdef DEBUG_MATH_WRAPPERS
            MessageInterface::ShowMessage("   %d, %s\n", i, refs[i].c_str());
            #endif
            
            // add if not found in the all ref array
            if (find(theAllParamArray.begin(), theAllParamArray.end(), refs[i]) ==
                theAllParamArray.end())
               theAllParamArray.push_back(refs[i]);
         }
      }
   }
   else
   {
      #ifdef __USE_ARGLIST_FOR_REFNAMES__
      // add function input arguments if node is a FunctionRunner
      if (node->IsOfType("FunctionRunner"))
      {
         #ifdef DEBUG_MATH_WRAPPERS
         MessageInterface::ShowMessage
            ("   It is a FunctionRunner of '%s'\n", node->GetName().c_str());
         #endif
         
         StringArray inputs = ((FunctionRunner*)node)->GetInputs();
         for (UnsignedInt i=0; i<inputs.size(); i++)
         {
            #ifdef DEBUG_MATH_WRAPPERS
            MessageInterface::ShowMessage("   inputs[%d]='%s'\n", i, inputs[i].c_str());
            #endif
            
            if (inputs[i] == "")
               continue;
            
            if (find(theAllParamArray.begin(), theAllParamArray.end(), inputs[i]) ==
                theAllParamArray.end())
               theAllParamArray.push_back(inputs[i]);
         }
      }
      #endif
      
      MathNode *left = node->GetLeft();
      CreateParameterNameArray(left);
      
      MathNode *right = node->GetRight();
      CreateParameterNameArray(right);
   }
}

