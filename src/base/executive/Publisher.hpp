//$Header$
//------------------------------------------------------------------------------
//                                  Publisher
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: LaMont Ruley
// Created: 2003/10/21
/**
 * Definition for the Publisher class.
 */
//------------------------------------------------------------------------------

#ifndef Publisher_hpp
#define Publisher_hpp

#include "Subscriber.hpp"
#include <list>

class GMAT_API Publisher
{
public:
    // default constructor
    Publisher(void);
    // assignment operator
    Publisher& operator=(const Publisher &right);

    // destructor
    virtual ~Publisher(void);

    bool        Subscribe(Subscriber * s);
    bool        Unsubscribe(Subscriber * s);

    bool        Publish(Real * data, Integer count);
    bool        Publish(char * data, Integer count = 0);
    bool        Publish(Integer * data, Integer count);

protected:
    //    /// List of the subscribers
    std::list<Subscriber*>  subs;
};
#endif // Publisher_hpp
