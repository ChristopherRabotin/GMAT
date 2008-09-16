//$Header$
//------------------------------------------------------------------------------
//                             ForceModelException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/02/29
//
/**
 * Exceptions thrown in the force models
 */
//------------------------------------------------------------------------------


#ifndef ForceModelException_hpp
#define ForceModelException_hpp

#include "BaseException.hpp"
#include "gmatdefs.hpp"

class GMAT_API ForceModelException : public BaseException
{
public:
   ForceModelException(const std::string &details);
   virtual ~ForceModelException();
   ForceModelException(const ForceModelException &fme);
};

#endif // ForceModelException_hpp
