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
#include "Publisher.hpp"

//------------------------------------------------------------------------------
//  Propagate(void)
//------------------------------------------------------------------------------
/**
 * Constructs the Propagate Command (default constructor).
 */
//------------------------------------------------------------------------------
Propagate::Propagate(void) :
    GmatCommand                 ("Propagate"),
    propName                    (""),
    propCoupled                 (true),
    interruptCheckFrequency     (30),
    prop                        (NULL),
    // Set the parameter IDs
    propCoupledID               (parameterCount),
    interruptCheckFrequencyID   (parameterCount+1),
    satNameID                   (parameterCount+2),
    propNameID                  (parameterCount+3),
    secondsToProp               (8640.0),
    secondsToPropID				(parameterCount+4)
{
    // Increase the number of parms by the 2 new ones
    parameterCount += 5;
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
    GmatCommand                     (p),
    propName                    (p.propName),
    propCoupled                 (true),
    interruptCheckFrequency     (30),
    baseEpoch                   (0.0),
    prop                        (NULL),
    // Set the parameter IDs
    propCoupledID               (p.propCoupledID),
    interruptCheckFrequencyID   (p.interruptCheckFrequencyID),
    satNameID                   (p.satNameID),
    propNameID                  (p.propNameID),
    secondsToProp               (p.secondsToProp),
    secondsToPropID				(p.secondsToPropID)
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
    GmatCommand::operator=(p);
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
    return GmatCommand::SetObject(name, type, associate, associateType);
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
    return GmatCommand::SetObject(obj, type);
}


GmatBase* Propagate::GetObject(const Gmat::ObjectType type, 
                               const std::string objName)
{
    if (type == Gmat::STOP_CONDITION)
        if (stopWhen.empty())
            return NULL;
        else
            return stopWhen[0];
            
    return GmatCommand::GetObject(type, objName);
}


// Parameter accessor methods
std::string Propagate::GetParameterText(const Integer id) const
{
    if (id == propCoupledID)
        return "PropagateCoupled";

    if (id == interruptCheckFrequencyID)
        return "InterruptFrequency";

    if (id == satNameID)
        return "Spacecraft";
    
    if (id == propNameID)
        return "Propagator";
    
    if (id == stopWhenID)
        return "StoppingConditions";
        
    if (id == secondsToPropID)
        return "ElapsedSeconds";

    return GmatCommand::GetParameterText(id);
}


Integer Propagate::GetParameterID(const std::string &str) const
{
    if (str == "PropagateCoupled")
        return propCoupledID;

    if (str == "InterruptFrequency")
        return interruptCheckFrequencyID;

    if (str == "Spacecraft")
        return satNameID;
    
    if (str == "Propagator")
        return propNameID;
    
    if (str == "ElapsedSeconds")
        return secondsToPropID;
    
    return GmatCommand::GetParameterID(str);
}


Gmat::ParameterType Propagate::GetParameterType(const Integer id) const
{
    if (id == propCoupledID)
        return Gmat::BOOLEAN_TYPE;

    if (id == interruptCheckFrequencyID)
        return Gmat::INTEGER_TYPE;

    if (id == satNameID)
        return Gmat::STRINGARRAY_TYPE;
    
    if (id == propNameID)
        return Gmat::STRING_TYPE;
    
    if (id == secondsToPropID)
        return Gmat::REAL_TYPE;
    
    return GmatCommand::GetParameterType(id);
}


std::string Propagate::GetParameterTypeString(const Integer id) const
{
    if (id == propCoupledID)
        return PARAM_TYPE_STRING[Gmat::BOOLEAN_TYPE];

    if (id == interruptCheckFrequencyID)
        return PARAM_TYPE_STRING[Gmat::INTEGER_TYPE];

    if (id == satNameID)
        return PARAM_TYPE_STRING[Gmat::STRINGARRAY_TYPE];

    if (id == propNameID)
        return PARAM_TYPE_STRING[Gmat::STRING_TYPE];

    if (id == secondsToPropID)
        return PARAM_TYPE_STRING[Gmat::REAL_TYPE];

    return GmatCommand::GetParameterTypeString(id);
}

// Temporary method -- remove once StopConditions are hooked up
Real Propagate::GetRealParameter(const Integer id) const
{
    if (id == secondsToPropID)
    	return secondsToProp;
    	
    return GmatCommand::GetRealParameter(id);
}

// Temporary method -- remove once StopConditions are hooked up
Real Propagate::SetRealParameter(const Integer id, const Real value)
{
    if (id == secondsToPropID) {
    	secondsToProp = value;
    	return secondsToProp;
    }
    	
    return GmatCommand::SetRealParameter(id, value);
}

Integer Propagate::GetIntegerParameter(const Integer id) const
{
    if (id == interruptCheckFrequencyID)
        return interruptCheckFrequency;

    return GmatCommand::GetIntegerParameter(id);
}


Integer Propagate::SetIntegerParameter(const Integer id, const Integer value)
{
    if (id == interruptCheckFrequencyID) {
        if (value >= 0)
            interruptCheckFrequency = value;
        return interruptCheckFrequency;
    }

    return GmatCommand::SetIntegerParameter(id, value);
}


bool Propagate::GetBooleanParameter(const Integer id) const
{
    if (id == propCoupledID)
        return propCoupled;

    return GmatCommand::GetBooleanParameter(id);
}


bool Propagate::SetBooleanParameter(const Integer id, const bool value)
{
    if (id == propCoupledID) {
        propCoupled = value;
        return propCoupled;
    }

    return GmatCommand::SetBooleanParameter(id, value);
}


std::string Propagate::GetStringParameter(const Integer id) const
{
    if (id == propNameID)
        return propName;

    return GmatCommand::GetStringParameter(id);
}


bool Propagate::SetStringParameter(const Integer id, const std::string &value)
{
    if (id == satNameID)
        if (satName.empty())
            satName.push_back(value);
        else                            /// @todo: Generalize for multiple sats
            satName[0] = value;
//        if (value == "")
//            satName.clear();

    if (id == propNameID)
        propName = value;
 
    return GmatCommand::SetStringParameter(id, value);
}


const StringArray& Propagate::GetStringArrayParameter(const Integer id) const
{
    if (id == satNameID)
        return satName;
 
    return GmatCommand::GetStringArrayParameter(id);
}


void Propagate::InterpretAction(void)
{
/// @todo: Clean up this hack for the Propagate::InterpretAction method
    // Sample string:  "Propagate RK89(Sat1, {Duration = 86400.0});"
    Integer loc = generatingString.find("Propagate", 0) + 9, end;
    const char *str = generatingString.c_str();
    while (str[loc] == ' ')
        ++loc;
        
    end = generatingString.find("(", loc);
    if (end == (Integer)std::string::npos)
        throw CommandException("Propagate string does not identify propagator");

    std::string component = generatingString.substr(loc, end-loc);
    SetObject(component, Gmat::PROP_SETUP);

    loc = end + 1;
    end = generatingString.find(",", loc);
    if (end == (Integer)std::string::npos)
        throw CommandException("Propagate string does not identify spacecraft");
    
    component = generatingString.substr(loc, end-loc);
    SetObject(component, Gmat::SPACECRAFT);

    loc = end + 1;
    end = generatingString.find(",", loc);
    if (end != (Integer)std::string::npos)
        throw CommandException("Propagate does not yet support multiple spacecraft");

    end = generatingString.find("{", loc);
    if (end == (Integer)std::string::npos)
        throw CommandException("Propagate does not identify stopping condition");

    end = generatingString.find("Duration", loc);
    if (end == (Integer)std::string::npos)
        throw CommandException("'Duration' is the only supported stopping condition");

    loc = end + 8;
    end = generatingString.find("=", loc);
    if (end == (Integer)std::string::npos)
        throw CommandException("Format: 'Duration = xxx'");

    loc = end + 1;
    secondsToProp = atof(&str[loc]);    
}


bool Propagate::Initialize(void)
{
    GmatCommand::Initialize();
    if (objectMap->find(propName) == objectMap->end())
        throw CommandException("Propagate command cannot find Propagator Setup");
    prop = (PropSetup *)((*objectMap)[propName]);
    if (!prop)
        return false;
    Propagator *p = prop->GetPropagator();
    if (!p)
        throw CommandException("Propagator not set in PropSetup");
        
    // Toss the spacecraft into the force model
    ForceModel *fm = prop->GetForceModel();
    StringArray::iterator scName;
    for (scName = satName.begin(); scName != satName.end(); ++scName) {
        sats.push_back((Spacecraft*)(*objectMap)[*scName]);
        fm->AddSpacecraft((Spacecraft*)(*objectMap)[*scName]);
    }
    
    p->Initialize();
    initialized = true;
    return true;
}


/**
 * Propagate the assigned members to the desired stopping condition
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
bool Propagate::Execute(void)
{
    Real elapsedTime = 0.0;
    if (initialized == false)
        throw CommandException("Propagate Command was not Initialized");

    Propagator *p = prop->GetPropagator();
    ForceModel *fm = prop->GetForceModel();
    fm->SetTime(0.0);
    
    p->Initialize();
    Real *state = fm->GetState();
    Integer dim = fm->GetDimension();
    Real *pubdata = new Real[dim+1];
    GmatBase* sat1 = (*objectMap)[*(satName.begin())];
    Integer epochID = sat1->GetParameterID("Epoch");
    baseEpoch = sat1->GetRealParameter(epochID);

    while (elapsedTime < secondsToProp) {
        if (!p->Step())
            throw CommandException("Propagator Failed to Step");
        // Not at stop condition yet
        if (fm->GetTime() < secondsToProp) {
            elapsedTime = fm->GetTime();
            fm->UpdateSpacecraft();
            /// @todo Update epoch on spacecraft
        }
        else // Passed stop epoch
        {
            fm->UpdateFromSpacecraft();
            fm->SetTime(elapsedTime);
            break;
        }
        // Publish the data here
        pubdata[0] = baseEpoch + fm->GetTime() / 86400.0;
        memcpy(&pubdata[1], state, dim*sizeof(Real));
        publisher->Publish(pubdata, dim+1);
    }
    
    if (secondsToProp - elapsedTime > 0.0) {
        if (!p->Step(secondsToProp - elapsedTime))
            throw CommandException("Propagator Failed to Step fixed interval");
        // Publish the final data point here
        pubdata[0] = baseEpoch + fm->GetTime() / 86400.0;
        
        memcpy(&pubdata[1], state, dim*sizeof(Real));
        publisher->Publish(pubdata, dim+1);
        fm->UpdateSpacecraft(baseEpoch + fm->GetTime() / 86400.0);
    }
    
    delete [] pubdata;

    return true;
}

