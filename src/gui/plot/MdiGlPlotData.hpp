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
#include "ColorTypes.hpp"       // for GmatColor::
#include <string>

namespace GmatPlot
{
   static const int MAX_BODIES = 20;
   static const int MAX_SCS = 30;
   static const int UNKNOWN_BODY = -1;
   
   enum BodyId
   {
      SUN = 0,  MERCURY,  VENUS,   EARTH,
      MARS,     JUPITER,  SATURN,  URANUS,
      NEPTUNE,  PLUTO,    MOON,    BODY1,
      BODY2,    BODY3,    BODY4,   BODY5,
      BODY6,    BODY7,    BODY8,   BODY9,
   };
   
   static std::string BODY_NAME[MAX_BODIES] =
   {
      "Sun",      "Mercury",  "Venus",   "Earth",
      "Mars",     "Jupiter",  "Saturn",  "Uranus",
      "Neptune",  "Pluto",    "Luna",    "Body1",
      "Body2",    "Body3",    "Body4",   "Body5",
      "Body6",    "Body7",    "Body8",   "Body9"
   };

   static unsigned int BODY_COLOR[MAX_BODIES] =
   {
      GmatColor::YELLOW32, GmatColor::SILVER32, GmatColor::SILVER32, GmatColor::GREEN32,
      GmatColor::SILVER32, GmatColor::SILVER32, GmatColor::SILVER32, GmatColor::SILVER32,
      GmatColor::SILVER32, GmatColor::SILVER32, GmatColor::L_BROWN32, GmatColor::SILVER32,
      GmatColor::SILVER32, GmatColor::SILVER32, GmatColor::SILVER32, GmatColor::SILVER32,
      GmatColor::SILVER32, GmatColor::SILVER32, GmatColor::SILVER32, GmatColor::SILVER32,
   };
   
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
      MDI_GL_VIEW_ADD_BODY,
      MDI_GL_VIEW_GOTO_BODY,

      MDI_GL_VIEW_GOTO_SUN,
      MDI_GL_VIEW_GOTO_MERCURY,
      MDI_GL_VIEW_GOTO_VENUS,
      MDI_GL_VIEW_GOTO_EARTH,
      MDI_GL_VIEW_GOTO_MARS,
      MDI_GL_VIEW_GOTO_JUPITER,
      MDI_GL_VIEW_GOTO_SATURN,
      MDI_GL_VIEW_GOTO_URANUS,
      MDI_GL_VIEW_GOTO_NEPTUNE,
      MDI_GL_VIEW_GOTO_PLUTO,
      MDI_GL_VIEW_GOTO_MOON,
      MDI_GL_VIEW_GOTO_OTHER_BODY,
      
      MDI_GL_VIEW_OPTION,
      MDI_GL_SHOW_OPTION_PANEL,
      MDI_GL_SHOW_WIRE_FRAME,
      MDI_GL_SHOW_EQUATORIAL_PLANE,
      MDI_GL_HELP_VIEW
   };

   int GetBodyId(const wxString &bodyName);
}

struct MdiGlPlot
{
   static MdiParentGlFrame *mdiParentGlFrame;
   static wxList mdiChildren;
   static int numChildren;
};

#endif
