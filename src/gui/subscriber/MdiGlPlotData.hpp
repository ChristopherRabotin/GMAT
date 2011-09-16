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
   static const int MAX_BODIES = 20;
   static const int MAX_SCS = 30;
   static const int UNKNOWN_BODY = -1;
   static const int UNKNOWN_COLOR = GmatColor::WHITE32;
   static const unsigned int UNINIT_TEXTURE = 999;

   enum BodyId
   {
      SUN = 0,  MERCURY,  VENUS,   EARTH,
      MARS,     JUPITER,  SATURN,  URANUS,
      NEPTUNE,  PLUTO,    LUNA,    BODY1,
      BODY2,    BODY3,    BODY4,   BODY5,
      BODY6,    BODY7,    BODY8,   BODY9,
   };
   
   struct BodyInfo
   {
      static wxString WX_BODY_NAME[MAX_BODIES];
      static std::string BODY_NAME[MAX_BODIES];
      static unsigned int BODY_COLOR[MAX_BODIES];
   };
   
   std::string& GetBodyName(int bodyId);
   int GetBodyId(const wxString &bodyName);
   unsigned int GetBodyColor(const wxString &bodyName);
   
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
