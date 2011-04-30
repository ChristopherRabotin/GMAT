//$Id$
//------------------------------------------------------------------------------
//                            StopConditionException
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
#ifndef StopConditionException_hpp
#define StopConditionException_hpp

#include "BaseException.hpp"

class GMAT_API StopConditionException : public BaseException
{
   public:
      StopConditionException(const std::string& message = 
                         "Error occurred in StopCondition class") 
         : BaseException(message) {};
};
#endif
