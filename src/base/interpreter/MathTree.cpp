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

//------------------------------------------------------------------------------
//  MathTree()
//------------------------------------------------------------------------------
/**
 * Constructs the MathTree object (default constructor).
 */
//------------------------------------------------------------------------------
MathTree::MathTree(const std::string &typeStr, const std::string &nomme) :
   GmatBase        (Gmat::MATH_TREE, typeStr, nomme),
   topNode(NULL)
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
    topNode         (mt.topNode)
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
// Rmatrix *MatrixEvaluate()
//------------------------------------------------------------------------------
Rmatrix MathTree::MatrixEvaluate()
{
   return topNode->MatrixEvaluate();
}

//------------------------------------------------------------------------------
// void ReportOutputs(Integer &type, Integer &rowCount, Integer &colCount) const
//------------------------------------------------------------------------------
void MathTree::ReportOutputs(Integer &type, 
               Integer &rowCount, Integer &colCount) 
{
    topNode->ReportOutputs(type, rowCount, colCount);
}

//------------------------------------------------------------------------------
// void Evaluate() const
//------------------------------------------------------------------------------
Real MathTree::Evaluate()
{
   return topNode->Evaluate();
}

//------------------------------------------------------------------------------
// bool Initialize(std::map<std::string, GmatBase *> *objectMap)
//------------------------------------------------------------------------------
bool MathTree::Initialize(std::map<std::string, GmatBase *> *objectMap)
{
    if (topNode == NULL)
        return false;
    
    // if node is an element (or a leaf) set the ref object        
    if (topNode->GetTypeName() == "MathElement")
    {
        std::string refName = topNode->GetRefObjectName(Gmat::PARAMETER);
        if (objectMap->find(refName) != objectMap->end())
        {
           topNode->SetRefObject((*objectMap)[refName], Gmat::PARAMETER,
               refName);
           return true;
        }
        else
           return false;
    }
    else 
    {
        // recursively calls initialize for the left and then the right
        // children
        bool result;
        topNode = ((MathFunction *)topNode)->GetLeft();
        result = Initialize(objectMap);
        topNode = ((MathFunction *)topNode)->GetRight();
        result = result & Initialize(objectMap);      
        return result;
    }

    return false;
}


