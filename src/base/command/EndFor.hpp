//$Header$
//------------------------------------------------------------------------------
//                             EndFor
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Joey Gurganus 
// Created: 2004/01/29
//
/**
 * Definition for the closing line of a for loop
 */
//------------------------------------------------------------------------------


#ifndef EndFor_hpp
#define EndFor_hpp


#include "Command.hpp"


class GMAT_API EndFor : public GmatCommand
{
public:
    EndFor(void);
    virtual ~EndFor(void);
    
    EndFor(const EndFor& ef);
    EndFor&          operator=(const EndFor& ef);
    
    virtual bool            Initialize(void);
    virtual bool            Execute(void);

    // inherited from GmatBase
    virtual GmatBase* Clone(void) const;

};


#endif // EndFor_hpp
