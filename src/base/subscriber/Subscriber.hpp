//$Header$
//------------------------------------------------------------------------------
//                                  Subscriber
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// *** File Name : Subscriber.hpp
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
 * Definition for the Subscriber base class.
 */
//------------------------------------------------------------------------------
#ifndef Subscriber_hpp
#define Subscriber_hpp

#include "GmatBase.hpp"


class GMAT_API Subscriber : public GmatBase
{
public:
    Subscriber(std::string typeStr, std::string nomme);
    Subscriber& operator=(const Subscriber &right);
    virtual ~Subscriber(void);

    virtual bool        Initialize(); //loj: 3/8/04 added
    virtual bool        ReceiveData(const char * datastream);
    virtual bool        ReceiveData(const char * datastream, const Integer len);
    virtual bool        ReceiveData(const Real * datastream, const Integer len = 0);
    Subscriber        * Next(void);
    bool                Add(Subscriber * s);
    bool                Remove(Subscriber * s, const bool del);
    
    void                Activate(bool state = true);
    bool                IsActive();
    
protected:
    const char        * data;
    Subscriber        * next;
    bool                active;

    virtual bool        Distribute(Integer len) = 0;
    virtual bool        Distribute(const Real * dat, Integer len);
private:

};
#endif // Subscribe_hpp

