//$Header$
//------------------------------------------------------------------------------
//                              MdiGlPlotData
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
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

MdiParentGlFrame* MdiPlot::mdiParentGlFrame = (MdiParentGlFrame *) NULL;    
wxList MdiPlot::mdiChildren = 0;
int MdiPlot::gs_nFrames = 0;
