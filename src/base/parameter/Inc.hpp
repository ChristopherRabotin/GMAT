//$Header$
//------------------------------------------------------------------------------
//                                   Inc
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/17
//
/**
 * Defines a functional inclination parameter class. 
 */
//------------------------------------------------------------------------------


#ifndef Inc_hpp
#define Inc_hpp

#include "Parameter.hpp"
#include "Spacecraft.hpp"
#include <vector>


/**
 * This implementation of the inclination parameter has several useful features:
 * 
 * 1.  It works.
 * 2.  It handles multiple spacecraft, so it won't need to be reworked in 
 *     build 3.
 * 3.  The implementation is simple -- only the methods that are identified as
 *     abstract methods in the base class and the methods that are needed for
 *     the algorithm are implemented.
 * 
 * @todo The implementation places the central body as (0, 0, 0).  This setting
 * is solar system dependent, so it should be relatively simple to fix.  But not 
 * by Monday!
 */ 
class Inc : public Parameter
{
public:
	Inc(const std::string &name);
	virtual ~Inc();

    Inc(const Inc &Inc);
    Inc&                    operator=(const Inc &Inc);

    virtual bool            AddObject(GmatBase *object);
    virtual Integer         GetNumObjects() const;

    virtual bool            Evaluate();
    virtual Real            EvaluateReal();

    virtual bool            Validate();
    
protected:
    /// Array of spacecraft that provide the data
    std::vector<Spacecraft *>    source;
    /// Array of evaluated data
    Real                    *results;
    /// Central body location, so position can be set correctly
    Real                    *cbLoc;
};

#endif // Inc_hpp
