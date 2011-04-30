//$Id$
//------------------------------------------------------------------------------
//                              CoordinateSystemException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy Shoan
// Created: 2004/12/22
//
/**
 * This class provides an exception class for the CoordinateSystem classes.
 */
//------------------------------------------------------------------------------
#ifndef CoordinateSystemException_hpp
#define CoordinateSystemException_hpp

#include "gmatdefs.hpp"
#include "BaseException.hpp"

class GMAT_API CoordinateSystemException : public BaseException
{
public:

   CoordinateSystemException(std::string details = "");

protected:

private:
};
#endif // CoordinateSystemException_hpp
