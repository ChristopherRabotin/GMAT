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
 * Declares plot events and static data for opengl plot.
 */
//------------------------------------------------------------------------------
#ifndef MdiGlPlotData_hpp
#define MdiGlPlotData_hpp

#include "MdiParentGlFrame.hpp"

namespace GmatPlot
{
    enum EventType
    {
        MDI_QUIT = 100,
        MDI_OPEN_TRAJECTORY_FILE,
        MDI_CHANGE_TITLE,
        MDI_CHILD_QUIT,
        MDI_SHOW_DEFAULT_VIEW,
        MDI_ZOOM_IN,
        MDI_ZOOM_OUT,
        MDI_VIEW_OPTION,
        MDI_SHOW_WIRE_FRAME,
        MDI_SHOW_EQUATORIAL_PLANE,
        MDI_HELP_VIEW
    };
}

struct MdiPlot
{
    static MdiParentGlFrame *mdiParentGlFrame;
    static wxList mdiChildren;
    
    static int gs_nFrames;
};

#endif
