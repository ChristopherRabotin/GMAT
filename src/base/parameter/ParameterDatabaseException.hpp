//$Id$
//------------------------------------------------------------------------------
//                            ParameterDatabaseException
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
// Created: 2003/09/18
//
/**
 * Defines parameter database exception.
 */
//------------------------------------------------------------------------------
#ifndef ParameterDatabaseException_hpp
#define ParameterDatabaseException_hpp

#include "BaseException.hpp"

class GMAT_API ParameterDatabaseException : public BaseException
{
   public:
      ParameterDatabaseException(const std::string& message = 
                                 "Error occurred in ParameterDabase class") 
         : BaseException(message) {};
};
#endif
