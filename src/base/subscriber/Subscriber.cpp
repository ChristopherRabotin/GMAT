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

//------------------------------------------------------------------------------
// Subscriber(std::string typeStr, std::string nomme)
//------------------------------------------------------------------------------
Subscriber::Subscriber(std::string typeStr, std::string nomme) :
   GmatBase (Gmat::SUBSCRIBER, typeStr, nomme),
   data (NULL),
   next (NULL),
   active (true),
   isEndOfReceive(false),
   currentProvider(0)
{
   objectTypes.push_back(Gmat::SUBSCRIBER);
   objectTypeNames.push_back("Subscriber");
}

//------------------------------------------------------------------------------
// Subscriber(const Subscriber &copy)
//------------------------------------------------------------------------------
Subscriber::Subscriber(const Subscriber &copy) :
   GmatBase (copy),
   data (NULL),
   next (NULL),
   active (copy.active), //loj: 4/21/05 Changed from true
   isEndOfReceive(false),
   currentProvider(copy.currentProvider)
{
}

//------------------------------------------------------------------------------
// Subscriber& Subscriber::operator=(const Subscriber& sub)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
Subscriber& Subscriber::operator=(const Subscriber& sub)
{
    if (this == &sub)
        return *this;

    GmatBase::operator=(sub);
    
    // is this right?
    data = sub.data;
    next = sub.next;
    active = true;
    isEndOfReceive = false;
    currentProvider = sub.currentProvider;

    return *this;
}

//------------------------------------------------------------------------------
// ~Subscriber(void)
//------------------------------------------------------------------------------
Subscriber::~Subscriber(void)
{
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool Subscriber::Initialize()
{
   isEndOfReceive = false;
   return true;
}

//------------------------------------------------------------------------------
// bool ReceiveData(const char *datastream)
//------------------------------------------------------------------------------
bool Subscriber::ReceiveData(const char *datastream)
{
   if (!active)        // Not currently processing data
      return true;
        
   data = datastream;
   return true;
}

//------------------------------------------------------------------------------
// bool ReceiveData(const char *datastream,  const int len)
//------------------------------------------------------------------------------
bool Subscriber::ReceiveData(const char *datastream,  const int len)
{
   if (!active)        // Not currently processing data
      return true;

   data = datastream;
   if (!Distribute(len))
   {
      data = NULL;
      return false;
   }

   data = NULL;
   return true;
}

//------------------------------------------------------------------------------
// bool ReceiveData(const double *datastream, const int len)
//------------------------------------------------------------------------------
bool Subscriber::ReceiveData(const double *datastream, const int len)
{
   if (!active)        // Not currently processing data
      return true;

   if (len == 0)
      return true;

   if (!Distribute(datastream, len))
   {
      return false;
   }

   return true;
}

//------------------------------------------------------------------------------
// bool FlushData()
//------------------------------------------------------------------------------
bool Subscriber::FlushData()
{
   isEndOfReceive = true;
   Distribute(0);
   Distribute(NULL, 0);
   isEndOfReceive = false;
   return true;
}

//------------------------------------------------------------------------------
// Subscriber* Next(void)
//------------------------------------------------------------------------------
Subscriber* Subscriber::Next(void)
{
    return next;
}

//------------------------------------------------------------------------------
// bool Add(Subscriber *s)
//------------------------------------------------------------------------------
bool Subscriber::Add(Subscriber *s)
{
   if (next)
      return next->Add(s);

   next = s;
   return true;
}

//------------------------------------------------------------------------------
// bool Remove(Subscriber *s, const bool del)
//------------------------------------------------------------------------------
bool Subscriber::Remove(Subscriber *s, const bool del)
{
   Subscriber *temp = next;

   if (next == s)
   {
      next = next->Next();
      if (del)
         delete temp;
      return true;
   }
   return false;
}

//------------------------------------------------------------------------------
// void Activate(bool state)
//------------------------------------------------------------------------------
void Subscriber::Activate(bool state)
{
   active = state;
}

//------------------------------------------------------------------------------
// bool IsActive()
//------------------------------------------------------------------------------
bool Subscriber::IsActive()
{
   return active;
}

//------------------------------------------------------------------------------
// void SetProviderId(Integer id)
//------------------------------------------------------------------------------
void Subscriber::SetProviderId(Integer id)
{
   currentProvider = id;
}

//------------------------------------------------------------------------------
// virtual void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
void Subscriber::SetInternalCoordSystem(CoordinateSystem *cs)
{
   internalCoordSystem = cs;
}

//---------------------------------
//  protected methods
//---------------------------------

//------------------------------------------------------------------------------
// bool Distribute(const double *dat, int len)
//------------------------------------------------------------------------------
bool Subscriber::Distribute(const double *dat, int len)
{
   return true;
}
