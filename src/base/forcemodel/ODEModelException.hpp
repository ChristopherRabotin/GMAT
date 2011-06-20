//$Id$
//------------------------------------------------------------------------------
//                             ODEModelException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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


#ifndef ODEModelException_hpp
#define ODEModelException_hpp

#include "BaseException.hpp"
#include "gmatdefs.hpp"

class GMAT_API ODEModelException : public BaseException
{
public:
   ODEModelException(const std::string &details = "");
   virtual ~ODEModelException();
   ODEModelException(const ODEModelException &fme);
};

#endif // ODEModelException_hpp
