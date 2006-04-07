//$Header$
//------------------------------------------------------------------------------
//                                   MathFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Waka Waktola
// Created: 2006/04/04
//
/**
 * Defines the Math functions class for Math in scripts.
 */
//------------------------------------------------------------------------------

#include "MathFunction.hpp"

#if DEBUG_MATHFUNCTION
#include "MessageInterface.hpp"
#endif

//------------------------------------------------------------------------------
//  MathFunction(std::string typeStr, std::string nomme)
//------------------------------------------------------------------------------
/**
 * Constructs the MathFunction object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <nomme>   Name for the object
 */
//------------------------------------------------------------------------------
MathFunction::MathFunction(const std::string &typeStr, const std::string &nomme) :
              MathNode    (typeStr, nomme),
              leftNode    (NULL),
              rightNode   (NULL)
{
}

//------------------------------------------------------------------------------
//  ~MathFunction(void)
//------------------------------------------------------------------------------
/**
 * Destroys the MathFunction object (destructor).
 */
//------------------------------------------------------------------------------
MathFunction::~MathFunction()
{
}

//------------------------------------------------------------------------------
//  MathFunction(const MathFunction &mf)
//------------------------------------------------------------------------------
/**
 * Constructs the MathFunction object (copy constructor).
 * 
 * @param <mf> Object that is copied
 */
//------------------------------------------------------------------------------
MathFunction::MathFunction(const MathFunction &mf) :
              MathNode   (mf),
              leftNode   (mf.leftNode),
              rightNode  (mf.rightNode)
{
}

//------------------------------------------------------------------------------
//  MathFunction& operator=(const MathFunction &mf)
//------------------------------------------------------------------------------
/**
 * Sets one MathFunction object to match another (assignment operator).
 * 
 * @param <mf> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
MathFunction& MathFunction::operator=(const MathFunction &mf)
{
    if (this == &mf)
        return *this;
        
    GmatBase::operator=(mf);

    return *this;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the MathFunction.
 *
 * @return clone of the MathFunction.
 *
 */
//------------------------------------------------------------------------------
GmatBase* MathFunction::Clone(void) const
{
   return (new MathFunction(*this));
}

//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
Real MathFunction::Evaluate()
{
    return MathNode::Evaluate();
}

//------------------------------------------------------------------------------
// bool EvaluateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool MathFunction::EvaluateInputs()
{
	 // @todo: This section may need more implementation.
	 if ( (leftNode) && (rightNode) )
       return true;
    else
       return MathNode::EvaluateInputs();
}

//------------------------------------------------------------------------------
// void ReportOutputs()
//------------------------------------------------------------------------------
void MathFunction::ReportOutputs(Integer &type, 
                                Integer &rowCount, Integer &colCount)
{
}

//------------------------------------------------------------------------------
// bool MatrixEvaluate()
//------------------------------------------------------------------------------
Rmatrix MathFunction::MatrixEvaluate()
{
	return MathNode::MatrixEvaluate();
}

//------------------------------------------------------------------------------
// bool SetChildren(MathNode *leftChild, MathNode *rightChild)()
//------------------------------------------------------------------------------
bool MathFunction::SetChildren(MathNode *leftChild, MathNode *rightChild)
{
	if (leftChild)
	   leftNode = leftChild;
	else
	   leftNode = NULL;
	   
	if (rightChild)
	   rightNode = rightChild;
	else
	   rightNode = NULL;
	   
	return true;
}

//------------------------------------------------------------------------------
// bool GetLeft()
//------------------------------------------------------------------------------
MathNode* MathFunction::GetLeft()
{
	return leftNode;
}

//------------------------------------------------------------------------------
// bool GetRight()
//------------------------------------------------------------------------------
MathNode* MathFunction::GetRight()
{
	return rightNode;
}

