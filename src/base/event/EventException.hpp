//$Id$
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
// Created: 2011/09/01
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
/**
 * Exception class used by the Event Location subsystem.
 */
//------------------------------------------------------------------------------


#ifndef EventException_hpp
#define EventException_hpp

#include "BaseException.hpp"
#include "gmatdefs.hpp"          // For GMAT_API

/**
 * Exception class used to report issues with event location.
 */
class GMAT_API EventException : public BaseException
{
public:
   EventException(const std::string &details = "");
   virtual ~EventException();
   EventException(const EventException &be);
};

#endif // EventException_hpp
