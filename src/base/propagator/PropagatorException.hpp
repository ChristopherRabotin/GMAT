//$Header$
//------------------------------------------------------------------------------
//                            PropagatorException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CI63P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/10/10
//
/**
 * Defines stopping condition exception.
 */
//------------------------------------------------------------------------------
#ifndef PropagatorException_hpp
#define PropagatorException_hpp

#include "BaseException.hpp"

class PropagatorException : public BaseException
{
   public:
      PropagatorException(const std::string& message = "Unspecified") 
         : BaseException("Propagator Exception: ", message) {};
};

#endif
