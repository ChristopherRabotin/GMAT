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
 * Implementation for the GmatFunction class.
 */
//------------------------------------------------------------------------------


#include "GmatFunction.hpp"

//------------------------------------------------------------------------------
// GmatFunction(std::string &nomme)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <nomme> function name
 */
GmatFunction::GmatFunction(const std::string &nomme) :
     Function("GmatFunction", nomme)
{
}




//------------------------------------------------------------------------------
// ~GmatFunction()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
GmatFunction::~GmatFunction()
{
}

//------------------------------------------------------------------------------
// GmatFunction(const GmatFunction &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> object being copied
 */
GmatFunction::GmatFunction(const GmatFunction &copy) :
   Function      (copy)
{
}

//------------------------------------------------------------------------------
// GmatFunction& GmatFunction::operator=(const GmatFunction& right)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 *
 * @param <right> object being copied
 *
 * @return reference to this object
 */
GmatFunction& GmatFunction::operator=(const GmatFunction& right)
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
 * Clone of the GmatFunction.
 *
 * @return clone of the GmatFunction.
 *
 */
//------------------------------------------------------------------------------
GmatBase* GmatFunction::Clone() const
{
   return (new GmatFunction(*this));
}

