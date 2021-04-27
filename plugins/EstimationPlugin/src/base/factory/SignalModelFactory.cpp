//$Id$
//------------------------------------------------------------------------------
//                           SignalModelFactory
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
// Created: Jan 31, 2014
/**
 * 
 */
//------------------------------------------------------------------------------

#include "SignalModelFactory.hpp"

SignalModelFactory::SignalModelFactory()
{
   GmatType::RegisterType("Signal");
}

SignalModelFactory::~SignalModelFactory()
{
}

