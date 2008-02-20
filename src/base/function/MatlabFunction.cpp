//$Id$
//------------------------------------------------------------------------------
//                                  MatlabFunction
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Allison Greene
// Created: 2004/09/22
//
/**
 * Definition for the MatlabFunction class.
 */
//------------------------------------------------------------------------------


#include "MatlabFunction.hpp"

//------------------------------------------------------------------------------
// MatlabFunction(std::string &nomme)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <nomme> name of the MatlabFunction.
 *
 */
MatlabFunction::MatlabFunction(const std::string &nomme) :
     Function("MatlabFunction", nomme)
{
}




//------------------------------------------------------------------------------
// ~MatlabFunction()
//------------------------------------------------------------------------------
/**
 * Destructor.
 *
 */
MatlabFunction::~MatlabFunction()
{
}

//------------------------------------------------------------------------------
// MatlabFunction(const MatlabFunction &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> object being copied
 */
MatlabFunction::MatlabFunction(const MatlabFunction &copy) :
   Function      (copy)
{
}

//------------------------------------------------------------------------------
// MatlabFunction& MatlabFunction::operator=(const MatlabFunction &right)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 *
 * @param <right> object being copied
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
MatlabFunction& MatlabFunction::operator=(const MatlabFunction& right)
{
    if (this == &right)
        return *this;

    Function::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the MatlabFunction.
 *
 * @return clone of the MatlabFunction.
 */
//------------------------------------------------------------------------------
GmatBase* MatlabFunction::Clone() const
{
   return (new MatlabFunction(*this));
}


//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void MatlabFunction::Copy(const GmatBase* orig)
{
   operator=(*((MatlabFunction *)(orig)));
}

