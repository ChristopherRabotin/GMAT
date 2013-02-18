//------------------------------------------------------------------------------
//                              Load3ds
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// ** Legal **
//
// Author: Phillip Silvia, Jr.
// Created: 2009/06/24
/**
 * Loads a .3ds file and stores the information in a ModelObject
 */
//------------------------------------------------------------------------------

#ifndef _LOAD_3DS_H
#define _LOAD_3DS_H

#include "ModelObject.hpp"

// Constants
#define LOAD3DS_DEBUG 0

// Functions
extern char Load3DS(ModelObject *p_object, const std::string &p_filename);

#endif
