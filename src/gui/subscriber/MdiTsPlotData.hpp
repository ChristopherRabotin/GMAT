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
// Created: 2004/01/20
/**
 * Declares plot events and static data for XY plot.
 */
//------------------------------------------------------------------------------
#ifndef MdiTsPlotData_hpp
#define MdiTsPlotData_hpp

#include "gmatwxdefs.hpp"

namespace GmatPlot
{
   enum TsEventType
   {
      MDI_TS_QUIT = 600, //Do not change. Higher number doesn't work!!
      MDI_TS_OPEN_PLOT_FILE,
      MDI_TS_CLEAR_PLOT,
      MDI_TS_CHANGE_TITLE,
      MDI_TS_CHILD_QUIT,
      MDI_TS_SHOW_DEFAULT_VIEW,
      MDI_TS_VIEW_OPTION,
      MDI_TS_DRAW_GRID,
      MDI_TS_DRAW_DOTTED_LINE,
      MDI_TS_HELP_VIEW
   };
}

struct MdiTsPlot
{
   static wxList mdiChildren;
   static int    numChildren;
   static bool   usePresetSize;
};

#endif
