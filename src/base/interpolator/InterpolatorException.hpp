//$Id$
//------------------------------------------------------------------------------
//                               InterpolatorException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number #####
//
// Author: Linda Jun (NASA/GSFC)
// Created: 2009/09/28
//
//------------------------------------------------------------------------------

#ifndef InterpolatorException_hpp
#define InterpolatorException_hpp

#include "BaseException.hpp"

/**
 * Exceptions thrown by the Interpolators
 */
class GMAT_API InterpolatorException : public BaseException
{
public:
   // class constructor
   InterpolatorException(std::string details = "");
   // class destructor
   ~InterpolatorException();
};

#endif // InterpolatorException_hpp

