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

//------------------------------------------------------------------------------
// BodyId GetBodyId(const wxString &bodyName)
//------------------------------------------------------------------------------
int GmatPlot::GetBodyId(const wxString &bodyName)
{
   for (int i=0; i<MAX_BODIES; i++)
      if (bodyName.IsSameAs(BODY_NAME[i].c_str()))
         return i;

   return GmatPlot::UNKNOWN_BODY;
}

