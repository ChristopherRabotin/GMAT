//------------------------------------------------------------------------------
//                            LoadPOV
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Phillip Silvia, Jr.
// Created: 2009/10/09
/**
 * This is the utility to loading in the model data from a POV-ray file. 
 * Derived from Dunning Idle 5's code from Odyssey. 
 */
//------------------------------------------------------------------------------

#include "gmatwxdefs.hpp"
#include "ModelObject.hpp"

void LoadPOV(ModelObject *obj, const std::string &modelPath);
