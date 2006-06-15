//$Header$
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
#include "StringUtil.hpp"            // for GetArrayIndex()
#include "InterpreterException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_MATH_TREE 1

//------------------------------------------------------------------------------
//  MathTree()
//------------------------------------------------------------------------------
/**
 * Constructs the MathTree object (default constructor).
 */
//------------------------------------------------------------------------------
MathTree::MathTree(const std::string &typeStr, const std::string &nomme) :
   GmatBase        (Gmat::MATH_TREE, typeStr, nomme),
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
    GmatBase        (mt),
    theTopNode      (mt.theTopNode),
    theObjectMap    (NULL)
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
    
    theTopNode   = mt.theTopNode;
    theObjectMap = NULL;
    
    return *this;
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
// void Evaluate() const
//------------------------------------------------------------------------------
Real MathTree::Evaluate()
{
   #if DEBUG_MATH_TREE
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
   #if DEBUG_MATH_TREE
   MessageInterface::ShowMessage
      ("MathTree::MatrixEvaluate() theTopNode=%s, %s\n", theTopNode->GetTypeName().c_str(),
       theTopNode->GetName().c_str());
   #endif
   return theTopNode->MatrixEvaluate();
}


//------------------------------------------------------------------------------
// bool Initialize(std::map<std::string, GmatBase *> *objectMap)
//------------------------------------------------------------------------------
bool MathTree::Initialize(std::map<std::string, GmatBase *> *objectMap)
{
   if (theTopNode == NULL)
      return true;

   theObjectMap = objectMap;

   #if DEBUG_MATH_TREE
   MessageInterface::ShowMessage
      ("MathTree::Initialize() theTopNode=%s, %s\n", theTopNode->GetTypeName().c_str(),
       theTopNode->GetName().c_str());
   #endif
   
   return InitializeParameter(theTopNode);
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount) const
//------------------------------------------------------------------------------
void MathTree::GetOutputInfo(Integer &type, 
               Integer &rowCount, Integer &colCount) 
{
    theTopNode->GetOutputInfo(type, rowCount, colCount);
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
      
      std::string refName = node->GetRefObjectName(Gmat::PARAMETER);

      #if DEBUG_MATH_TREE
      MessageInterface::ShowMessage
         ("MathTree::InitializeParameter() refName=%s\n", refName.c_str());
      #endif

      // Handle array index
      Integer row, col;
      std::string newName;
      GmatStringUtil::GetArrayIndex(refName, row, col, newName);
      
      if (theObjectMap->find(newName) != theObjectMap->end())
      {
         node->SetRefObject((*theObjectMap)[newName], Gmat::PARAMETER,
                            newName);

         #if DEBUG_MATH_TREE
         MessageInterface::ShowMessage
            ("MathTree::InitializeParameter() Found %s from theObjectMap\n",
             refName.c_str());
         #endif
         
         return true;
      }
      else
      {
         //loj: 5/25/06 throw an exception
         throw InterpreterException
            ("MathTree::InitializeParameter() Unable to find " + newName +
             "from theObjectMap\n");
      }
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
