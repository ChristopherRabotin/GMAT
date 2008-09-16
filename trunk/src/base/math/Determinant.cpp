//------------------------------------------------------------------------------
//                                  Determinant
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
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
   Integer type1, row1, col1; // Left node

   #if DEBUG_DETERMINANT
   MessageInterface::ShowMessage
      ("Determinant::ValidateInputs() left=%s, %s\n",
       leftNode->GetTypeName().c_str(), leftNode->GetName().c_str());
   #endif
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
      
   if (type1 == Gmat::RMATRIX_TYPE)
      return true;
   else
      return false;
   
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
   return (leftNode->MatrixEvaluate()).Determinant();
}

