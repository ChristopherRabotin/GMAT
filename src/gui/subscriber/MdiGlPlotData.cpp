//$Id$
//------------------------------------------------------------------------------
//                              MdiGlPlotData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2003/11/25
/**
 * Initializes plot events and static data for opengl plot.
 */
//------------------------------------------------------------------------------
#include "MdiGlPlotData.hpp"

wxList MdiGlPlot::mdiChildren;
int    MdiGlPlot::numChildren = 0;
bool   MdiGlPlot::usePresetSize = false;

