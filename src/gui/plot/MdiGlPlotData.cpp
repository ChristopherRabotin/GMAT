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

MdiParentGlFrame* MdiGlPlot::mdiParentGlFrame = (MdiParentGlFrame *) NULL;    
wxList MdiGlPlot::mdiChildren = 0;
int MdiGlPlot::numChildFrames = 0;
