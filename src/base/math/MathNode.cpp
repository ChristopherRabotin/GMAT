//$Id$
//------------------------------------------------------------------------------
//                                   MathNode
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
#include "MessageInterface.hpp"

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
   isNumber        (false),
   isFunction      (false),
   isFunctionInput (false),
   elementType     (Gmat::REAL_TYPE),
   realValue       (REAL_PARAMETER_UNDEFINED)
{
   objectTypes.push_back(Gmat::MATH_NODE);
   objectTypeNames.push_back("MathNode");   
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
   //MessageInterface::ShowMessage
   //   ("==> MathNode::~MathNode() deleting %s, %s\n", GetTypeName().c_str(),
   //    GetName().c_str());
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
   isNumber        (mn.isNumber),
   isFunction      (mn.isFunction),
   isFunctionInput (mn.isFunction),
   elementType     (mn.elementType),
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
   
   isNumber = mn.isNumber;
   isFunction = mn.isFunction;
   isFunctionInput = mn.isFunctionInput;
   elementType = mn.elementType;
   realValue = mn.realValue;
   matrix = mn.matrix;
   
   return *this;
}


//------------------------------------------------------------------------------
// void SetRealValue(Real val)
//------------------------------------------------------------------------------
void MathNode::SetRealValue(Real val)
{
   elementType = Gmat::REAL_TYPE;
   realValue = val;
}

//------------------------------------------------------------------------------
// void SetMatrixValue(const Rmatrix &mat)
//------------------------------------------------------------------------------
void MathNode::SetMatrixValue(const Rmatrix &mat)
{
   elementType = Gmat::RMATRIX_TYPE;
   Integer theRowCount = mat.GetNumRows();
   Integer theColCount = mat.GetNumColumns();
   
   if (!matrix.IsSized())
      matrix.SetSize(theRowCount, theColCount);
   
   matrix = mat;
   
   #if DEBUG_MATRIX_NODE
   std::stringstream ss("");
   ss << matarix;
   MessageInterface::ShowMessage
      ("MathNode::SetMatrixValue() theReturnType=%d, theRowCount=%d, "
       "theColCount=%d, matarix=\n%s\n", theReturnType, theRowCount, theColCount,
       ss.str().c_str());
   #endif
}


//---------------------------------------------------------------------------
// virtual void SetMathWrappers(WrapperMap *wrapperMap)
//---------------------------------------------------------------------------
void MathNode::SetMathWrappers(WrapperMap *wrapperMap)
{
   // do nothing here
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
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
bool MathNode::RenameRefObject(const Gmat::ObjectType type,
                               const std::string &oldName,
                               const std::string &newName)
{
   // There are no renamable objects
   return true;
}

