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
 * Declares plot events and static data for opengl plot.
 */
//------------------------------------------------------------------------------
#ifndef MdiGlPlotData_hpp
#define MdiGlPlotData_hpp

#include "gmatwxdefs.hpp"
#include "ColorTypes.hpp"       // for GmatColor::
#include "RgbColor.hpp"
#include <string>
#include <map>

typedef std::map<wxString, RgbColor> wxStringColorMap;
typedef std::map<wxString, bool> wxStringBoolMap;

namespace GmatPlot
{
   // Event for OpenGlOptionDialog
   enum GlEventType
   {
      MDI_GL_QUIT = 500, //Do not change. Higher number doesn't work!!
      MDI_GL_OPEN_TRAJECTORY_FILE,
      MDI_GL_CHILD_ZOOM_IN,
      MDI_GL_CHILD_ZOOM_OUT,
      MDI_GL_CLEAR_PLOT,
      MDI_GL_CHANGE_TITLE,
      MDI_GL_CHILD_QUIT,
      MDI_GL_SHOW_DEFAULT_VIEW,
      MDI_GL_ZOOM_IN,
      MDI_GL_ZOOM_OUT,
      MDI_GL_VIEW_ANIMATION,
      
      MDI_GL_VIEW_OPTION,
      MDI_GL_SHOW_OPTION_PANEL,
      MDI_GL_SHOW_WIRE_FRAME,
      MDI_GL_SHOW_EQUATORIAL_PLANE,
      MDI_GL_HELP_VIEW
   };
};


struct MdiGlPlot
{
   static wxList mdiChildren;
   static int    numChildren;
   static bool   usePresetSize;;
};

#endif
