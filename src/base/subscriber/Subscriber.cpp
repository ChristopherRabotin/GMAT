//$Header$
//------------------------------------------------------------------------------
//                                  Subscriber
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// *** File Name : Subscriber.cpp
// *** Created   : May 20, 2003
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 595)                  ***
// ***  Under Contract:  P.O.  GSFC S-67573-G                             ***
// ***  Copyright Thinking Systems 2003                                   ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 5/20/2003 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//                             Source code not yet transferred to GSFC
//                           : 10/01/2003 - L. Ruley, Missions Applications Branch
//                             Updated style using GMAT cpp style guide
// **************************************************************************
//
/**
 * Implementation code for the Subscriber base class
 */
//------------------------------------------------------------------------------


#include "Subscriber.hpp" // class's header file


//---------------------------------
//  public methods
//---------------------------------

Subscriber::Subscriber(std::string typeStr, std::string nomme) :
GmatBase        (Gmat::SUBSCRIBER, typeStr, nomme),
data            (NULL),
next            (NULL),
active          (true)
{
}

Subscriber::Subscriber(const Subscriber &copy) :
GmatBase        (copy),
data            (NULL),
next            (NULL),
active          (true)
{
}

Subscriber::~Subscriber(void)
{
}

//loj: 3/8/04 added
bool Subscriber::Initialize()
{
    return true;
}

bool Subscriber::ReceiveData( const char * datastream)
{
    if (!active)        // Not currently processing data
        return true;
        
    data = datastream;
    return true;
}

bool Subscriber::ReceiveData( const char * datastream,  const int len)
{
    if (!active)        // Not currently processing data
        return true;

    data = datastream;
    if (!Distribute(len)) {
        data = NULL;
        return false;
    }

    data = NULL;
    return true;
}


bool Subscriber::ReceiveData(const double * datastream, const int len)
{
    if (!active)        // Not currently processing data
        return true;

    if (len == 0)
        return true;

    if (!Distribute(datastream, len)) {
        return false;
    }

    return true;
}


Subscriber * Subscriber::Next(void)
{
    return next;
}


bool Subscriber::Add(Subscriber * s)
{
    if (next)
        return next->Add(s);

    next = s;
    return true;
}


bool Subscriber::Remove(Subscriber * s, const bool del)
{
    Subscriber *temp = next;

    if (next == s) {
        next = next->Next();
        if (del)
            delete temp;
        return true;
    }
    return false;
}


void Subscriber::Activate(bool state)
{
    active = state;
}

bool Subscriber::IsActive()
{
    return active;
}

bool Subscriber::Distribute(const double * dat, int len)
{
    return true;
}

