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
 * Initializes plot events and static data for XY plot.
 */
//------------------------------------------------------------------------------
#include "MdiXyPlotData.hpp"

MdiParentXyFrame* MdiXyPlot::mdiParentXyFrame = (MdiParentXyFrame *) NULL;
wxList MdiXyPlot::mdiChildren = 0;
int MdiXyPlot::numChildren = 0;
