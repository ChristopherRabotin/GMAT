//$Header$
//------------------------------------------------------------------------------
//                                 Propagate
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/09/23
//
/**
 * Implementation for the Propagate command class
 */
//------------------------------------------------------------------------------

#include "Propagate.hpp"

//------------------------------------------------------------------------------
//  Propagate(void)
//------------------------------------------------------------------------------
/**
 * Constructs the Propagate Command (default constructor).
 */
//------------------------------------------------------------------------------
Propagate::Propagate(void) :
    Command                     ("Propagate"),
    propName                    (""),
    propCoupled                 (true),
    interruptCheckFrequency     (30),
    prop                        (NULL),
    // Set the parameter IDs
    propCoupledID               (parameterCount),
    interruptCheckFrequencyID   (parameterCount+1)
{
    // Increase the number of parms by the 2 new ones
    parameterCount += 2;
}


//------------------------------------------------------------------------------
//  ~Propagate(void)
//------------------------------------------------------------------------------
/**
 * Destroys the Propagate Command.
 */
//------------------------------------------------------------------------------
Propagate::~Propagate(void)
{    
}


//------------------------------------------------------------------------------
//  Propagate(void)
//------------------------------------------------------------------------------
/**
 * Constructs the Propagate Command based on another instance (copy constructor).
 *
 * @param <p> Original we are copying
 */
//------------------------------------------------------------------------------
Propagate::Propagate(const Propagate &p) :
    Command                     (p),
    propName                    (p.propName),
    propCoupled                 (true),
    interruptCheckFrequency     (30),
    prop                        (NULL),
    // Set the parameter IDs
    propCoupledID               (p.propCoupledID),
    interruptCheckFrequencyID   (p.interruptCheckFrequencyID)
{
    // Increase the number of parms by the 2 new ones
    parameterCount = p.parameterCount;
    initialized = false;
}


//------------------------------------------------------------------------------
//  Propagate& operator=(const Propagate &p)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Propagate Command.
 *
 * @return reference to this copy
 */
//------------------------------------------------------------------------------
Propagate& Propagate::operator=(const Propagate &p)
{
    if (&p == this)
        return *this;

    // Call the base assignment operator
    Command::operator=(p);
    initialized = false;
    
    return *this;
}


bool Propagate::SetObject(const std::string &name, const Gmat::ObjectType type,
                          const std::string &associate,
                          const Gmat::ObjectType associateType)
{
    switch (type) {
        case Gmat::SPACECRAFT:
            satName.push_back(name);
            return true;

        case Gmat::PROP_SETUP:
            propName = name;
            return true;

        // Keep the compiler from whining
        default:
            break;
    }

    // Not handled here -- invoke the next higher SetObject call
    return Command::SetObject(name, type, associate, associateType);
}


bool Propagate::SetObject(GmatBase *obj, const Gmat::ObjectType type)
{
    switch (type) {
        case Gmat::STOP_CONDITION:
            stopWhen.push_back((StopCondition *)obj);
            return true;
            
        // Keep the compiler from whining
        default:
            break;
    }

    // Not handled here -- invoke the next higher SetObject call
    return Command::SetObject(obj, type);
}


// Parameter accessor methods
std::string Propagate::GetParameterText(const Integer id) const
{
    if (id == propCoupledID)
        return "PropagateCoupled";

    if (id == interruptCheckFrequencyID)
        return "InterruptFrequency";

    return Command::GetParameterText(id);
}


Integer Propagate::GetParameterID(const std::string &str) const
{
    if (str == "PropagateCoupled")
        return propCoupledID;

    if (str == "InterruptFrequency")
        return interruptCheckFrequencyID;

    return Command::GetParameterID(str);
}


Gmat::ParameterType Propagate::GetParameterType(const Integer id) const
{
    if (id == propCoupledID)
        return Gmat::BOOLEAN_TYPE;

    if (id == interruptCheckFrequencyID)
        return Gmat::INTEGER_TYPE;

    return Command::GetParameterType(id);
}


std::string Propagate::GetParameterTypeString(const Integer id) const
{
    if (id == propCoupledID)
        return PARAM_TYPE_STRING[Gmat::BOOLEAN_TYPE];

    if (id == interruptCheckFrequencyID)
        return PARAM_TYPE_STRING[Gmat::INTEGER_TYPE];

    return Command::GetParameterTypeString(id);
}


Integer Propagate::GetIntegerParameter(const Integer id) const
{
    if (id == interruptCheckFrequencyID)
        return interruptCheckFrequency;

    return Command::GetIntegerParameter(id);
}


Integer Propagate::SetIntegerParameter(const Integer id, const Integer value)
{
    if (id == interruptCheckFrequencyID) {
        if (value >= 0)
            interruptCheckFrequency = value;
        return interruptCheckFrequency;
    }

    return Command::SetIntegerParameter(id, value);
}


bool Propagate::GetBooleanParameter(const Integer id) const
{
    if (id == propCoupledID)
        return propCoupled;

    return Command::GetBooleanParameter(id);
}


bool Propagate::SetBooleanParameter(const Integer id, const bool value)
{
    if (id == propCoupledID) {
        propCoupled = value;
        return propCoupled;
    }

    return Command::SetBooleanParameter(id, value);
}


bool Propagate::Initialize(void)
{
    return false;
}

/**
 * Propagate the assigned members to the desired stopping condition
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
bool Propagate::Execute(void)
{
    if (initialized == false)
        return false;

    return true;
}

