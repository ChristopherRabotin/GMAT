//$Header$
//------------------------------------------------------------------------------
//                                  GmatFunction
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Allison Greene
// Created: 2004/12/16
//
/**
 * Definition for the GmatFunction class.
 */
//------------------------------------------------------------------------------


#include "GmatFunction.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// GmatFunction(std::string &name)
//------------------------------------------------------------------------------
GmatFunction::GmatFunction(const std::string &nomme) :
     Function("GmatFunction", nomme)
{
}




//------------------------------------------------------------------------------
// ~GmatFunction(void)
//------------------------------------------------------------------------------
GmatFunction::~GmatFunction(void)
{
}

//------------------------------------------------------------------------------
// GmatFunction(const GmatFunction &rf)
//------------------------------------------------------------------------------
GmatFunction::GmatFunction(const GmatFunction &gf) :
   Function      (gf)
{
}

//------------------------------------------------------------------------------
// GmatFunction& GmatFunction::operator=(const GmatFunction& rf)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
GmatFunction& GmatFunction::operator=(const GmatFunction& gf)
{
    if (this == &gf)
        return *this;

    Function::operator=(gf);

    return *this;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the GmatFunction.
 *
 * @return clone of the GmatFunction.
 *
 */
//------------------------------------------------------------------------------
GmatBase* GmatFunction::Clone(void) const
{
   return (new GmatFunction(*this));
}

