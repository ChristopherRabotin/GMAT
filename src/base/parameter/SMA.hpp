//$Header$
//------------------------------------------------------------------------------
//                                   SMA
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
 * Defines a functional semimajor axis parameter class. 
 */
//------------------------------------------------------------------------------


#ifndef SMA_hpp
#define SMA_hpp

#include "Parameter.hpp"
#include "Spacecraft.hpp"
#include <vector>


/**
 * This implementation of the SMA parameter has several useful features:
 * 
 * 1.  It works.
 * 2.  It handles multiple spacecraft, so it won't need to be reworked in 
 *     build 3.
 * 3.  The implementation is simple -- only the methods that are identified as
 *     abstract methods in the base class and the methods that are needed for
 *     the algorithm are implemented.
 * 
 * @todo The implementation is Earth centered only right now, because of 
 * dependence on mu.  The location of the central body is set to (0, 0, 0).
 * Both of these settings are solar system dependent, so they should be 
 * relatively simple to fix.  But not by Monday!
 */ 
class SMA : public Parameter
{
public:
	SMA(const std::string &name);
	virtual ~SMA();

    SMA(const SMA &sma);
    SMA&                    operator=(const SMA &sma);

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
    /// Gravitational constant for the SMA calculation
    Real                    mu;
    /// Central body location
    Real                    *cbLoc;
};

#endif // SMA_hpp
