//$Id: EventException.hpp 9513 2011-04-30 21:23:06Z djcinsb $
//------------------------------------------------------------------------------
//                               EventException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2004/1/13
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
/**
 * Exception class used by the Located Event subsystem.
 */
//------------------------------------------------------------------------------


#ifndef EventException_hpp
#define EventException_hpp

#include "BaseException.hpp"
#include "gmatdefs.hpp"          // For GMAT_API

class GMAT_API EventException : public BaseException
{
public:
   EventException(const std::string &details = "");
   virtual ~EventException();
   EventException(const EventException &be);
};

#endif // EventException_hpp
