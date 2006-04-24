//$Header$
//------------------------------------------------------------------------------
//                                   MathNode
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Allison Greene
// Created: 2006/03/27
//
/**
 * Defines the MathNode base class used for Math in Scripts.
 */
//------------------------------------------------------------------------------

#include "MathNode.hpp"

#if DEBUG_MATHNODE
#include "MessageInterface.hpp"
#endif

//------------------------------------------------------------------------------
//  MathNode(std::string typeStr, std::string nomme)
//------------------------------------------------------------------------------
/**
 * Constructs the MathNode object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <nomme>   Name for the object
 */
//------------------------------------------------------------------------------
MathNode::MathNode(const std::string &typeStr, const std::string &nomme) :
    GmatBase        (Gmat::MATH_NODE, typeStr, nomme),
    realValue       (0.0)
{
    matrix = Rmatrix(3, 3);
}

//------------------------------------------------------------------------------
//  ~MathNode(void)
//------------------------------------------------------------------------------
/**
 * Destroys the MathNode object (destructor).
 */
//------------------------------------------------------------------------------
MathNode::~MathNode()
{
}


//------------------------------------------------------------------------------
//  MathNode(const MathNode &mn)
//------------------------------------------------------------------------------
/**
 * Constructs the MathNode object (copy constructor).
 * 
 * @param <mn> Object that is copied
 */
//------------------------------------------------------------------------------
MathNode::MathNode(const MathNode &mn) :
    GmatBase        (mn),
    realValue       (mn.realValue),
    matrix          (mn.matrix)
{
}


//------------------------------------------------------------------------------
//  MathNode& operator=(const MathNode &mn)
//------------------------------------------------------------------------------
/**
 * Sets one MathNode object to match another (assignment operator).
 * 
 * @param <mn> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
MathNode& MathNode::operator=(const MathNode &mn)
{
    if (this == &mn)
        return *this;
        
    GmatBase::operator=(mn);

    return *this;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the MathNode.
 *
 * @return clone of the MathNode.
 *
 */
//------------------------------------------------------------------------------
GmatBase* MathNode::Clone(void) const
{
   return (new MathNode(*this));
}

//------------------------------------------------------------------------------
// Rmatrix *MatrixEvaluate()
//------------------------------------------------------------------------------
Rmatrix MathNode::MatrixEvaluate()
{
//   return matrix;
   throw MathException("No implementation of MatrixEvaluate()\n");    
}

//------------------------------------------------------------------------------
// void ReportOutputs(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void MathNode::ReportOutputs(Integer &type, Integer &rowCount, Integer &colCount)
{
   throw MathException("No implementation of ReportOutputs(...).\n");    
}

//------------------------------------------------------------------------------
// void Evaluate()
//------------------------------------------------------------------------------
Real MathNode::Evaluate()
{
   throw MathException("No implementation of Evaluate().\n");
}

//------------------------------------------------------------------------------
// void EvaluateInputs()
//------------------------------------------------------------------------------
bool MathNode::EvaluateInputs()
{
   throw MathException("No implementation of EvaluateInputs().\n");
}

