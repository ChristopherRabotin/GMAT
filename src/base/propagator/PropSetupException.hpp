//$Id$
//------------------------------------------------------------------------------
//                            PropSetupException
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
// Author: Linda Jun
// Created: 2003/10/10
//
/**
 * Defines stopping condition exception.
 */
//------------------------------------------------------------------------------
#ifndef PropSetupException_hpp
#define PropSetupException_hpp

#include "BaseException.hpp"

class GMAT_API PropSetupException : public BaseException
{
public:
   PropSetupException(const std::string& details = "") 
      : BaseException("PropSetup Exception: ", details) {};
};
#endif
