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
 * Implementation for the Assignment command class.
 * 
 * The assignment class is the Command class that handles commands of the form
 * 
 *     GMAT object.parameter = value;
 * 
 * Eventually (currently scheduled for build 4) this class will be extended to 
 * include numerical operators on the right side of the assignment line.
 */
//------------------------------------------------------------------------------


#include "Assignment.hpp"


Assignment::Assignment() :
    GmatCommand         ("GMAT"),
    ownerName       (""),
    parmName        (""),    
    parmOwner       (NULL),
    parmID          (-1),
    parmType        (Gmat::UNKNOWN_PARAMETER_TYPE),
    value           ("Not_Set")
{
}


Assignment::~Assignment()
{
}


Assignment::Assignment(const Assignment& a) :
    GmatCommand         (a),
    ownerName       (a.ownerName),
    parmName        (a.parmName),    
    parmOwner       (a.parmOwner),
    parmID          (a.parmID),
    parmType        (a.parmType),
    value           (a.value)
{
}


Assignment& Assignment::operator=(const Assignment& a)
{
    if (this == &a)
        return *this;
        
    ownerName = a.ownerName;
    parmName  = a.parmName;    
    parmOwner = a.parmOwner;
    parmID    = a.parmID;
    parmType  = a.parmType;
    value     = a.value;
    
    return *this;
}


bool Assignment::Initialize(void)
{
    // Find the object
    if (objectMap->find(ownerName) == objectMap->end())
        throw CommandException("Assignment command cannot find object");
    
    parmOwner = (*objectMap)[ownerName];
    return true;
}


bool Assignment::InterpretAction(void)
{
    /// @todo: Clean up this hack for the Maneuver::InterpretAction method
    // Assignment lines have the form GMAT Sat.Element1 = 7654.321;
    Integer loc = generatingString.find("GMAT", 0) + 4, end;
    const char *str = generatingString.c_str();
    while (str[loc] == ' ')
        ++loc;
    
    end = generatingString.find(".", loc);
    if (end == (Integer)std::string::npos)
        throw CommandException("Assignment string does not identify object");
    
    std::string component = generatingString.substr(loc, end-loc);
    if (component == "")
        throw CommandException("Assignment string does not identify object");
    ownerName = component;
    
    loc = end + 1;
    end = generatingString.find("=", loc);
    if (end == (Integer)std::string::npos)
        throw CommandException("Assignment string does not set value");
    
    Integer strend = end;
    while (str[strend] == ' ')
        --strend;
    component = generatingString.substr(loc, strend-loc-1);
    if (component == "")
        throw CommandException("Assignment string does not identify parameter");
    parmName = component;
    
    loc = end + 1;
    while (str[loc] == ' ')
        ++loc;

    value     = &str[loc];
    
    end = value.find(";");
    value = value.substr(0, end);
    
    return true;
}


/** 
 * The method that is fired to perform the command.
 *
 * Derived classes implement this method to perform their actions on 
 * GMAT objects.
 *
 * @return true if the GmatCommand runs to completion, false if an error 
 *         occurs. 
 */
bool Assignment::Execute(void)
{
    bool retval = false;

    // Get the parameter ID and ID type
    parmID    = parmOwner->GetParameterID(parmName);
    parmType  = parmOwner->GetParameterType(parmID);
    
    if (parmOwner == NULL)
        throw CommandException("Parameter Owner Not Initialized");
    
    switch (parmType) {
        case Gmat::INTEGER_TYPE:
            parmOwner->SetIntegerParameter(parmID, atoi(value.c_str()));
            retval = true;
            break;
            
        case Gmat::REAL_TYPE:
            parmOwner->SetRealParameter(parmID, atof(value.c_str()));
            retval = true;
            break;
            
        case Gmat::STRING_TYPE:
        case Gmat::STRINGARRAY_TYPE:
            parmOwner->SetStringParameter(parmID, value);
            retval = true;
            break;
            
        default:
            break;
    }
    
    // "Add" parameters could also mean to set reference objects
    if (parmName == "Add") {
       if (objectMap->find(value) != objectMap->end())
       {
          GmatBase *obj = (*objectMap)[value];
          if (obj)
             parmOwner->SetRefObject(obj, obj->GetType(), value);
       }
    }
    
    return retval;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Assignment.
 *
 * @return clone of the Assignment.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Assignment::Clone(void) const
{
   return (new Assignment(*this));
}

