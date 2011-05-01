//$Id: EventException.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                             EventException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2009/08/11
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
/**
 * Exception class used by the Event hierarchy.
 */
//------------------------------------------------------------------------------


#ifndef EventException_hpp
#define EventException_hpp

#include "estimation_defs.hpp"
#include "BaseException.hpp" // inheriting class's header file


class ESTIMATION_API EventException : public BaseException
{
public:
   // class constructor
   EventException(const std::string &details = "");
   // class destructor
   ~EventException();
   // Copy constructor
   EventException(const EventException &ce);
};

#endif // EventException_hpp

