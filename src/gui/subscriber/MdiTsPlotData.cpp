//$Id$
//------------------------------------------------------------------------------
//                              MdiGlPlotData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2003/11/25
/**
 * Initializes plot events and static data for XY plot.
 */
//------------------------------------------------------------------------------
#include "MdiTsPlotData.hpp" 

wxList MdiTsPlot::mdiChildren;
int    MdiTsPlot::numChildren = 0;
bool   MdiTsPlot::usePresetSize = false;
