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
 * Initializes plot events and static data for opengl plot.
 */
//------------------------------------------------------------------------------
#include "MdiGlPlotData.hpp"

MdiParentGlFrame* MdiGlPlot::mdiParentGlFrame = (MdiParentGlFrame *) NULL;    
wxList MdiGlPlot::mdiChildren = 0;
int MdiGlPlot::numChildren = 0;

//  const wxString GmatPlot::BODY_NAME[MAX_BODIES] =
//  {
//     "Sun",      "Mercury",  "Venus",   "Earth",
//     "Mars",     "Jupiter",  "Saturn",  "Uranus",
//     "Neptune",  "Pluto",    "Moon",    "Body1",
//     "Body2",    "Body3",    "Body4",   "Body5",
//     "Body6",    "Body7",    "Body8",   "Body9"
//  };

//------------------------------------------------------------------------------
// BodyId GetBodyId(const wxString &bodyName)
//------------------------------------------------------------------------------
int GmatPlot::GetBodyId(const wxString &bodyName)
{
   for (int i=0; i<MAX_BODIES; i++)
      if (bodyName == BODY_NAME[i])
         return i;

   return GmatPlot::UNKNOWN_BODY;
}

