//$Id$
//------------------------------------------------------------------------------
//                              AttitudeException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan/GSFC
// Created: 2006.02.16
//
/**
 * This class provides an exception class for the Attitude classes.
 */
//------------------------------------------------------------------------------
#ifndef AttitudeException_hpp
#define AttitudeException_hpp

#include "gmatdefs.hpp"
#include "BaseException.hpp" // inherited class's header file

class GMAT_API AttitudeException : public BaseException
{
public:

   AttitudeException(std::string details = "");
   ~AttitudeException();
};
#endif /*AttitudeException_hpp*/
