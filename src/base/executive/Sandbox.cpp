//$Header$
//------------------------------------------------------------------------------
//                                 Sandbox
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/10/08
//
/**
 * Implementation for the GMAT Sandbox class
 */
//------------------------------------------------------------------------------


#include "Sandbox.hpp"
#include "SandboxException.hpp"


Sandbox::Sandbox(void) :
    solarSys        (NULL),
    publisher       (NULL),
    sequence		(NULL),
    current			(NULL),
    state           (IDLE)
{
}


Sandbox::~Sandbox(void)
{
    if (solarSys)
        delete solarSys;
    if (sequence)
        delete sequence;
    
    // Delete the local objects
}
    
// Setup methods
bool Sandbox::AddObject(GmatBase *obj)
{
    if (state == INITIALIZED)
        state = IDLE;

    std::string name = obj->GetName();
    if (name == "")
        return false;		// No unnamed objects in the Sandbox tables

    // Check to see if the object is already in the map
    if (objectMap.find(name) == objectMap.end())
        // If not, store the new object pointer
        /// @todo Replace copy c'tor call with Clone() -- Build 2 issue
        if (obj->GetType() == Gmat::SPACECRAFT)
            objectMap[name] = new Spacecraft(*((Spacecraft*)obj));
        else
            objectMap[name] = obj;
    
    return true;
}


bool Sandbox::AddCommand(Command *cmd)
{
    if (state == INITIALIZED)
        state = IDLE;

    if (!cmd)
        return false;
        
    if (cmd == sequence)
        return true;
//        throw SandboxException("Adding command that is already in the Sandbox");
    
    if (sequence)
        return sequence->Append(cmd);

    sequence = cmd;
    return true;
}


bool Sandbox::AddSolarSystem(SolarSystem *ss)
{
    if (state == INITIALIZED)
        state = IDLE;

    if (!ss)
        return false;
    solarSys = ss;
    return true;
}


bool Sandbox::SetPublisher(Publisher *pub)
{
    if (state == INITIALIZED)
        state = IDLE;

    if (pub) {
        publisher = pub;
        return true;
    }

    // Initialize off of the singleton
    //publisher = Publisher::Instance();
    if (!publisher)
        return false;
    return true;
}


// Execution methods
bool Sandbox::Initialize(void)
{
    bool rv;

    // Already initialized
    if (state == INITIALIZED)
        return true;

    current = sequence;
    if (!current)
        return false;

    while (current) {
        current->SetObjectMap(&objectMap);
        rv = current->Initialize();
        if (!rv)
            return false;
        current = current->GetNext();
    }
    
    return rv;
}


bool Sandbox::Execute(void)
{
    bool rv = true;

    state = RUNNING;
    
    current = sequence;
    if (!current)
        return false;

    while (current) {
        // First check to see if the run should be interrupted
        if (Interrupt())
            break;
        
        rv = current->Execute();
        if (!rv) {
            std::string str = current->GetTypeName() +
                              " Command failed to run to completion";
            throw SandboxException(str);
        }
        current = current->GetNext();
    }

    return rv;
}


bool Sandbox::Interrupt(void)
{
    // We'll want something like this in build 2, so the user can interrupt
    // a run:
    //
    // Integer interruptType =  moderator->GetUserInterrupt();
    // switch (interruptType) {
    //     case 1:   // Pause
    //         state = PAUSED;
    //         break;
    //
    //     case 2:   // Stop
    //         state = STOPPED;
    //         break;
    //
    //     case 3:   // Reset, which is also the fefault value
    //     default:
    //         state = RESET;
    //         break;
    //
    // }
    
    return false;
}


void Sandbox::Clear(void)
{
}


bool Sandbox::AddSpacecraft(Spacecraft *obj)
{
    return AddObject(obj);
}


bool Sandbox::AddPropSetup(PropSetup *propSetup)

{
    return AddObject(propSetup);
}


bool Sandbox::AddSubscriber(Subscriber *sub)
{
    return  AddObject(sub);
}

