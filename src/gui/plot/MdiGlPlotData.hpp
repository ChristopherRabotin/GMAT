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
    enum GlEventType
    {
        MDI_GL_QUIT = 100,
        MDI_GL_OPEN_TRAJECTORY_FILE,
        MDI_GL_CHILD_ZOOM_IN,
        MDI_GL_CHILD_ZOOM_OUT,
        MDI_GL_CLEAR_PLOT,
        MDI_GL_CHANGE_TITLE,
        MDI_GL_CHILD_QUIT,
        MDI_GL_SHOW_DEFAULT_VIEW,
        MDI_GL_ZOOM_IN,
        MDI_GL_ZOOM_OUT,
        MDI_GL_VIEW_OPTION,
        MDI_GL_SHOW_WIRE_FRAME,
        MDI_GL_SHOW_EQUATORIAL_PLANE,
        MDI_GL_HELP_VIEW
    };
}

struct MdiGlPlot
{
    static MdiParentGlFrame *mdiParentGlFrame;
    static wxList mdiChildren;
    static int numChildren;
};

#endif
