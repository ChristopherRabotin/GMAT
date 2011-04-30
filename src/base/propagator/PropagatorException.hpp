//$Id$
//------------------------------------------------------------------------------
//                            PropagatorException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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

class GMAT_API PropagatorException : public BaseException
{
   public:
      PropagatorException(const std::string& message = "Unspecified",
            // Change to this when the repeated instances are fixed:
   //         Gmat::MessageType mt = Gmat::ERROR_);
            // Change to this if it's problematic:
            Gmat::MessageType mt = Gmat::GENERAL_)
         : BaseException("Propagator Exception: ", message, mt) {};
};

#endif
