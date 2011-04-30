//$Id$
//------------------------------------------------------------------------------
//                                  Determinant
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Allison Greene
// Created: 2006/04/20
//
/**
 * Implements Determinant class.
 */
//------------------------------------------------------------------------------

#include "Determinant.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_DETERMINANT 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Determinant()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Determinant::Determinant(const std::string &nomme)
   : MathFunction("Determinant", nomme)
{
}


//------------------------------------------------------------------------------
// ~Determinant()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Determinant::~Determinant()
{
}


//------------------------------------------------------------------------------
//  Determinant(const Determinant &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Determinant object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Determinant::Determinant(const Determinant &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Determinant operation.
 *
 * @return clone of the Determinant operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Determinant::Clone() const
{
   return (new Determinant(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Determinant::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   type = Gmat::REAL_TYPE;
   rowCount = 1;
   colCount = 1;

   #if DEBUG_DETERMINANT
   MessageInterface::ShowMessage
      ("Determinant::GetOutputInfo() type=%d, rowCount=%d, colCount=%d\n",
       type, rowCount, colCount);
   #endif
}


//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Determinant::ValidateInputs()
{
   if (leftNode == NULL)
      throw MathException("Determinant() - Missing input arguments.\n");
   
   Integer type1, row1, col1; // Left node

   #if DEBUG_DETERMINANT
   MessageInterface::ShowMessage
      ("Determinant::ValidateInputs() left=%s, %s\n",
       leftNode->GetTypeName().c_str(), leftNode->GetName().c_str());
   #endif
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   return true;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Determinant of left node
 *
 */
//------------------------------------------------------------------------------
Real Determinant::Evaluate()
{
   Integer type, rowCount, colCount;
   leftNode->GetOutputInfo(type, rowCount, colCount);
   
   if (type == Gmat::RMATRIX_TYPE)
   {
      return (leftNode->MatrixEvaluate()).Determinant();
   }
   else
   {
      return leftNode->Evaluate();
   }
}


