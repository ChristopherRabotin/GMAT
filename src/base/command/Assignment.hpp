//$Header$
//------------------------------------------------------------------------------
//                                Assignment
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/02/03
//
/**
 * Definition of the Assignment command class.
 * 
 * The assignment class is the Command class that handles commands of the form
 * 
 *     GMAT object.parameter = value;
 * 
 * Eventually (currently scheduled for build 4) this class will be extended to 
 * include numerical operators on the right side of the assignment line.
 */
//------------------------------------------------------------------------------


#ifndef Assignment_hpp
#define Assignment_hpp

#include "Command.hpp"

class Assignment : public GmatCommand
{
public:
	Assignment();
	virtual ~Assignment();
    Assignment(const Assignment& a);
    Assignment&         operator=(const Assignment& a);
    
    virtual bool        Initialize(void);
    virtual bool        InterpretAction(void);
    virtual bool        Execute(void);


    // inherited from GmatBase
    virtual GmatBase* Clone(void) const;

protected:
    /// Name of the object that appears on the left side of the equals sign
    std::string         ownerName;
    /// Parameter associated with that object
    std::string         parmName;
    /// Object named by pwnerName
    GmatBase            *parmOwner;
    /// The object used for object assignment
    GmatBase            *rhsObject;
    /// Flag indicating if this is object assignment
    bool                objToObj;
    /// For parameter version, the parm's ID
    Integer             parmID;
    ///  The parm's data type
    Gmat::ParameterType parmType;
    /// The value that is assigned, or the name of the object providing the data
    std::string         value;
};

#endif // Assignment_hpp
