//$Header$
//------------------------------------------------------------------------------
//                             File: OrbitalParameters.cpp
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/03/25
//
/**
 * Implements Orbital related parameter classes.
 *   VelApoapsis, VelPeriapsis, Apoapsis, Periapsis
 */
//------------------------------------------------------------------------------
#include "OrbitalParameters.hpp"
#include "MessageInterface.hpp"

//==============================================================================
//                              VelApoapsis
//==============================================================================
/**
 * Implements Velocity at Apoapsis class.
 */
//------------------------------------------------------------------------------

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// VelApoapsis(const std::string &name, GmatBase *obj,
//             const std::string &desc, const std::string &unit)
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
VelApoapsis::VelApoapsis(const std::string &name, GmatBase *obj,
                         const std::string &desc, const std::string &unit)
    : OrbitReal(name, "VelApoapsis", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// VelApoapsis(const VelApoapsis &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
VelApoapsis::VelApoapsis(const VelApoapsis &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const VelApoapsis& operator=(const VelApoapsis &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const VelApoapsis&
VelApoapsis::operator=(const VelApoapsis &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~VelApoapsis()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
VelApoapsis::~VelApoapsis()
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
bool VelApoapsis::Evaluate()
{
    mRealValue = OrbitData::GetOtherKepReal("VelApoapsis");    
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}

//==============================================================================
//                              VelPeriapsis
//==============================================================================
/**
 * Implements Velocity at Periapsis class.
 */
//------------------------------------------------------------------------------

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// VelPeriapsis(const std::string &name, GmatBase *obj,
//              const std::string &desc, const std::string &unit)
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
VelPeriapsis::VelPeriapsis(const std::string &name, GmatBase *obj,
                           const std::string &desc, const std::string &unit)
    : OrbitReal(name, "VelPeriapsis", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// VelPeriapsis(const VelPeriapsis &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
VelPeriapsis::VelPeriapsis(const VelPeriapsis &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const VelPeriapsis& operator=(const VelPeriapsis &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const VelPeriapsis&
VelPeriapsis::operator=(const VelPeriapsis &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~VelPeriapsis()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
VelPeriapsis::~VelPeriapsis()
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
bool VelPeriapsis::Evaluate()
{
    mRealValue = OrbitData::GetOtherKepReal("VelPeriapsis");    
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}


//==============================================================================
//                              Apoapsis
//==============================================================================
/**
 * Implements Periapsis class.
 */
//------------------------------------------------------------------------------

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Apoapsis(const std::string &name, GmatBase *obj,
//          const std::string &desc, const std::string &unit)
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
Apoapsis::Apoapsis(const std::string &name, GmatBase *obj,
                   const std::string &desc, const std::string &unit)
    : OrbitReal(name, "Apoapsis", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// Apoapsis(const Apoapsis &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
Apoapsis::Apoapsis(const Apoapsis &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const Apoapsis& operator=(const Apoapsis &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const Apoapsis&
Apoapsis::operator=(const Apoapsis &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~Apoapsis()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Apoapsis::~Apoapsis()
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
bool Apoapsis::Evaluate()
{
    Rvector6 cartState = OrbitData::GetCartState();
    if (cartState == Rvector6::RVECTOR6_UNDEFINED)
        return false;

    // compute position and velocity unit vectors
    Rvector3 pos = Rvector3(cartState[0], cartState[1], cartState[2]);
    Rvector3 vel = Rvector3(cartState[3], cartState[4], cartState[5]);
    Rvector3 R = pos.GetUnitVector();
    Rvector3 V = vel.GetUnitVector();
    
    // compute cos(90 - beta) as the dot product of the R and V vectors
    Real rdotv = R*V;
    mRealValue = rdotv;
    
    MessageInterface::ShowMessage("Apoapsis::Evaluate() r=%f,%f,%f, v=%f,%f,%f, r.v=%f\n",
                                  R[0], R[1], R[2], V[0], V[1], V[2], rdotv);
    return true;
}


//==============================================================================
//                              Periapsis
//==============================================================================
/**
 * Implements Periapsis class.
 */
//------------------------------------------------------------------------------

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Periapsis(const std::string &name, GmatBase *obj,
//          const std::string &desc, const std::string &unit)
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
Periapsis::Periapsis(const std::string &name, GmatBase *obj,
                     const std::string &desc, const std::string &unit)
    : OrbitReal(name, "Periapsis", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// Periapsis(const Periapsis &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
Periapsis::Periapsis(const Periapsis &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const Periapsis& operator=(const Periapsis &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const Periapsis&
Periapsis::operator=(const Periapsis &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~Periapsis()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Periapsis::~Periapsis()
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
bool Periapsis::Evaluate()
{
    Rvector6 cartState = OrbitData::GetCartState();
    if (cartState == Rvector6::RVECTOR6_UNDEFINED)
        return false;
    
    // compute position and velocity unit vectors
    Rvector3 pos = Rvector3(cartState[0], cartState[1], cartState[2]);
    Rvector3 vel = Rvector3(cartState[3], cartState[4], cartState[5]);
    Rvector3 R = pos.GetUnitVector();
    Rvector3 V = vel.GetUnitVector();
    
    // compute cos(90 - beta) as the dot product of the R and V vectors
    Real rdotv = R*V;
    mRealValue = rdotv;
    
    return true;
}

