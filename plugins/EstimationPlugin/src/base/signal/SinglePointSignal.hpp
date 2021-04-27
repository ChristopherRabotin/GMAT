//$Id$
//------------------------------------------------------------------------------
//                           SinglePointSignal
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 9, 2014
/**
 * Intermediate class used for signals that include light time
 */
//------------------------------------------------------------------------------

#ifndef SinglePointSignal_hpp
#define SinglePointSignal_hpp

#include "SignalBase.hpp"


/**
 * Intermediate class used for signals that are evaluated at a single endpoint
 *
 * @todo SinglePointSignal classes are not needed in R2014b, the release that
 * first contains this code.  Implement when needed.
 */
class ESTIMATION_API SinglePointSignal: public SignalBase
{
public:
   SinglePointSignal(const std::string &typeStr, const std::string &name);
   virtual ~SinglePointSignal();
   SinglePointSignal(const SinglePointSignal& ps);
   SinglePointSignal& operator=(const SinglePointSignal& ps);
};

#endif /* SinglePointSignal_hpp */
