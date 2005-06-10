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
#include "MdiTsPlotData.hpp" 

//MdiParentXyFrame* MdiXyPlot::mdiParentXyFrame = (MdiParentXyFrame *) NULL;
wxList MdiTsPlot::mdiChildren = 0;
int MdiTsPlot::numChildren = 0;
