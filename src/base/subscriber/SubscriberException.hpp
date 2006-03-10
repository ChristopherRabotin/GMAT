//$Header$
//------------------------------------------------------------------------------
//                            SubscriberException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
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

class SubscriberException : public BaseException
{
   public:
      SubscriberException(const std::string& message) 
         : BaseException("SubscriberException Thrown: ", message) {};
};
#endif
