//$Header$
//------------------------------------------------------------------------------
//                              MdiGlPlotData
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2004/01/20
/**
 * Declares plot events and static data for XY plot.
 */
//------------------------------------------------------------------------------
#ifndef MdiXyPlotData_hpp
#define MdiXyPlotData_hpp

#include "MdiParentXyFrame.hpp"

namespace GmatPlot
{
    enum XyEventType
    {
        MDI_XY_QUIT = 200,
        MDI_XY_OPEN_PLOT_FILE,
        MDI_XY_CLEAR_PLOT,
        MDI_XY_CHANGE_TITLE,
        MDI_XY_CHILD_QUIT,
        MDI_XY_SHOW_DEFAULT_VIEW,
        MDI_XY_VIEW_OPTION,
        MDI_XY_DRAW_GRID,
        MDI_XY_DRAW_DOTTED_LINE,
        MDI_XY_HELP_VIEW
    };
}

struct MdiXyPlot
{
    static MdiParentXyFrame *mdiParentXyFrame;
    static wxList mdiChildren;
    static int numChildren;
};

#endif
