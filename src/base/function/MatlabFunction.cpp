//$Header$
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
#include "MessageInterface.hpp"

#define DEBUG_MATLABFUNCTION 0

//------------------------------------------------------------------------------
// MatlabFunction(std::string &name)
//------------------------------------------------------------------------------
MatlabFunction::MatlabFunction(const std::string &nomme) :
     Function("MatlabFunction", nomme)
{
}




//------------------------------------------------------------------------------
// ~MatlabFunction(void)
//------------------------------------------------------------------------------
MatlabFunction::~MatlabFunction(void)
{
}

//------------------------------------------------------------------------------
// MatlabFunction(const MatlabFunction &rf)
//------------------------------------------------------------------------------
MatlabFunction::MatlabFunction(const MatlabFunction &mf) :
   Function      (mf)
{
}

//------------------------------------------------------------------------------
// MatlabFunction& MatlabFunction::operator=(const MatlabFunction& rf)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
MatlabFunction& MatlabFunction::operator=(const MatlabFunction& mf)
{
    if (this == &mf)
        return *this;

    Function::operator=(mf);

    return *this;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the MatlabFunction.
 *
 * @return clone of the MatlabFunction.
 *
 */
//------------------------------------------------------------------------------
GmatBase* MatlabFunction::Clone(void) const
{
   return (new MatlabFunction(*this));
}

