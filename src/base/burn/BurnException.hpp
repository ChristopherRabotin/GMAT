//$Id$
//------------------------------------------------------------------------------
//                               BurnException
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
 * Exception class used by the Burn hierarchy.
 */
//------------------------------------------------------------------------------


#ifndef BurnException_hpp
#define BurnException_hpp

#include "BaseException.hpp"
#include "gmatdefs.hpp"          // For GMAT_API

class GMAT_API BurnException : public BaseException
{
public:
   BurnException(const std::string &details = "");
   virtual ~BurnException();
   BurnException(const BurnException &be);
};

#endif // BurnException_hpp
