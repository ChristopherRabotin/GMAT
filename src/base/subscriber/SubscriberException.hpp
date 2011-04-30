//$Id$
//------------------------------------------------------------------------------
//                            SubscriberException
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
// Created: 2006/03/10
//
/**
 * Defines subscriber exception.
 */
//------------------------------------------------------------------------------
#ifndef SubscriberException_hpp
#define SubscriberException_hpp

#include "BaseException.hpp"

class GMAT_API SubscriberException : public BaseException
{
public:
   SubscriberException(const std::string& details = "") 
      : BaseException("Subscriber Exception: ", details) {};
};
#endif
