//$Id$
//------------------------------------------------------------------------------
//                           StateVectorException
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
// Author:  Joey Gurganus
// Created: 2005/06/07
//
/**
 * Declares exception for the StateVector class.
 */
//------------------------------------------------------------------------------
#ifndef StateVectorException_hpp
#define StateVectorException_hpp

#include "BaseException.hpp"

class GMAT_API StateVectorException : public BaseException
{
public:
   StateVectorException(const std::string& message = 
                    "Error occurred in StateVector class") 
      : BaseException(message) {};
};
#endif
