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

//---------------------------------
// static data
//---------------------------------
const std::string
MathNode::PARAMETER_TEXT[MathNodeParamCount - GmatBaseParamCount] =
{
   "RealVar",
   "Matrix",
};

const Gmat::ParameterType
MathNode::PARAMETER_TYPE[MathNodeParamCount - GmatBaseParamCount] =
{
   Gmat::REAL_TYPE,
   Gmat::RMATRIX_TYPE,
};


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
    realValue       (0.0),
    matrix          (NULL)
{
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
   parameterCount = MathNodeParamCount;
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
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string MathNode::GetParameterText(const Integer id) const
{
    if (id >= REAL_VAR && id < MathNodeParamCount)
        return PARAMETER_TEXT[id - GmatBaseParamCount];
    else
        return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer MathNode::GetParameterID(const std::string &str) const
{
    for (Integer i = REAL_VAR; i < MathNodeParamCount; i++)
    {
        if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
            return i;
    }

    return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType MathNode::GetParameterType(const Integer id) const
{
    if (id >= REAL_VAR && id < MathNodeParamCount)
        return PARAMETER_TYPE[id - GmatBaseParamCount];
    else
        return GmatBase::GetParameterType(id);

}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string MathNode::GetParameterTypeString(const Integer id) const
{
   if (id >= REAL_VAR && id < MathNodeParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return GmatBase::GetParameterTypeString(id);

}


//------------------------------------------------------------------------------
// Rmatrix *MatrixEvaluate()
//------------------------------------------------------------------------------
Rmatrix *MathNode::MatrixEvaluate()
{
    return matrix;
}

//------------------------------------------------------------------------------
// void ReportOutputs(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void MathNode::ReportOutputs(Integer &type, Integer &rowCount, Integer &colCount)
{
    
}

