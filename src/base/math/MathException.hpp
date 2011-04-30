//$Id$
//------------------------------------------------------------------------------
//                               MathException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Allison Greene
// Created: 2006/03/27
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
/**
 * Exception class used by the Math hierarchy.
 */
//------------------------------------------------------------------------------
#ifndef MATHEXCEPTION_H
#define MATHEXCEPTION_H

#include "BaseException.hpp"

class GMAT_API MathException : public BaseException
{
public:
   
   MathException(std::string details = "");
   virtual ~MathException();
   MathException(const MathException &be);
};

#endif // MATHEXCEPTION_H
