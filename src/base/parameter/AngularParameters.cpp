//$Header$
//------------------------------------------------------------------------------
//                             File: AngularParameters.cpp
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/03/18
//
/**
 * Implements Angular related parameter classes.
 *   SemilatusRectum
 */
//------------------------------------------------------------------------------
#include "AngularParameters.hpp"

//==============================================================================
//                              SemilatusRectum
//==============================================================================
/**
 * Implements Semilatus Rectum parameter class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SemilatusRectum(const std::string &name, GmatBase *obj,
//                 const std::string &desc, const std::string &unit)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 */
//------------------------------------------------------------------------------
SemilatusRectum::SemilatusRectum(const std::string &name, GmatBase *obj,
                                 const std::string &desc, const std::string &unit)
    : OrbitReal(name, "SemilatusRectum", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// SemilatusRectum(const SemilatusRectum &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
SemilatusRectum::SemilatusRectum(const SemilatusRectum &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const SemilatusRectum& operator=(const SemilatusRectum &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const SemilatusRectum&
SemilatusRectum::operator=(const SemilatusRectum &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~SemilatusRectum()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
SemilatusRectum::~SemilatusRectum()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool SemilatusRectum::Evaluate()
{
    mRealValue = OrbitData::GetAngularReal("SemilatusRectum");    
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}
