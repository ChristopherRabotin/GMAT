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
 * Initializes plot events and static data for opengl plot.
 */
//------------------------------------------------------------------------------
#include "MdiGlPlotData.hpp"

wxList MdiGlPlot::mdiChildren;
int    MdiGlPlot::numChildren = 0;
bool   MdiGlPlot::usePresetSize = false;

std::string GmatPlot::BodyInfo::BODY_NAME[GmatPlot::MAX_BODIES] =
{
   "Sun",      "Mercury",  "Venus",   "Earth",
   "Mars",     "Jupiter",  "Saturn",  "Uranus",
   "Neptune",  "Pluto",    "Luna",    "Body1",
   "Body2",    "Body3",    "Body4",   "Body5",
   "Body6",    "Body7",    "Body8",   "Body9"
};

wxString GmatPlot::BodyInfo::WX_BODY_NAME[GmatPlot::MAX_BODIES] =
{
   "Sun",      "Mercury",  "Venus",   "Earth",
   "Mars",     "Jupiter",  "Saturn",  "Uranus",
   "Neptune",  "Pluto",    "Luna",    "Body1",
   "Body2",    "Body3",    "Body4",   "Body5",
   "Body6",    "Body7",    "Body8",   "Body9"
};

unsigned int GmatPlot::BodyInfo::BODY_COLOR[GmatPlot::MAX_BODIES] =
{
   GmatColor::YELLOW32, GmatColor::SILVER32, GmatColor::SILVER32, GmatColor::GREEN32,
   GmatColor::SILVER32, GmatColor::SILVER32, GmatColor::SILVER32, GmatColor::SILVER32,
   GmatColor::SILVER32, GmatColor::SILVER32, GmatColor::L_BROWN32, GmatColor::SILVER32,
   GmatColor::SILVER32, GmatColor::SILVER32, GmatColor::SILVER32, GmatColor::SILVER32,
   GmatColor::SILVER32, GmatColor::SILVER32, GmatColor::SILVER32, GmatColor::SILVER32,
};

//------------------------------------------------------------------------------
// std::string& GetBodyName(int bodyId)
//------------------------------------------------------------------------------
std::string& GmatPlot::GetBodyName(int bodyId)
{
   return BodyInfo::BODY_NAME[bodyId];
}

//------------------------------------------------------------------------------
// BodyId GetBodyId(const wxString &bodyName)
//------------------------------------------------------------------------------
int GmatPlot::GetBodyId(const wxString &bodyName)
{
   for (int i=0; i<MAX_BODIES; i++)
      if (bodyName.IsSameAs(BodyInfo::BODY_NAME[i].c_str()))
         return i;

   return GmatPlot::UNKNOWN_BODY;
}

//------------------------------------------------------------------------------
// int GetBodyColor(const wxString &bodyName)
//------------------------------------------------------------------------------
unsigned int GmatPlot::GetBodyColor(const wxString &bodyName)
{
   for (int i=0; i<MAX_BODIES; i++)
      if (bodyName.IsSameAs(BodyInfo::BODY_NAME[i].c_str()))
         return BodyInfo::BODY_COLOR[i];

   return GmatPlot::UNKNOWN_COLOR;
}

