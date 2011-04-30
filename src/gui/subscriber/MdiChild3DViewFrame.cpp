//$Id$
//------------------------------------------------------------------------------
//                              MdiChild3DViewFrame
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
 * Implements MdiChild3DViewFrame for 3D visualization
 */
//------------------------------------------------------------------------------
#include "gmatwxrcs.hpp"
#include "MdiChild3DViewFrame.hpp"
#include "OrbitViewCanvas.hpp"
#include "GmatAppData.hpp"

//#define DEBUG_TRAJ_FRAME
//#define DEBUG_MDI_CHILD_FRAME_CLOSE

BEGIN_EVENT_TABLE(MdiChild3DViewFrame, MdiChildViewFrame)
   EVT_MENU(GmatPlot::MDI_GL_CHILD_QUIT, MdiChild3DViewFrame::OnQuit)
   EVT_MENU(GmatPlot::MDI_GL_CHANGE_TITLE, MdiChild3DViewFrame::OnChangeTitle)
   EVT_MENU(GmatPlot::MDI_GL_CLEAR_PLOT, MdiChild3DViewFrame::OnClearPlot)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_DEFAULT_VIEW, MdiChild3DViewFrame::OnShowDefaultView)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_WIRE_FRAME, MdiChild3DViewFrame::OnDrawWireFrame)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_EQUATORIAL_PLANE, MdiChild3DViewFrame::OnDrawXyPlane)
   EVT_ACTIVATE(MdiChild3DViewFrame::OnActivate)
   EVT_SIZE(MdiChild3DViewFrame::OnPlotSize)
   EVT_MOVE(MdiChild3DViewFrame::OnMove)
   EVT_CLOSE(MdiChild3DViewFrame::OnPlotClose) 
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// MdiChild3DViewFrame(wxMDIParentFrame *parent, const wxString& title, ...)
//------------------------------------------------------------------------------
MdiChild3DViewFrame::MdiChild3DViewFrame(wxMDIParentFrame *parent,
                                     const wxString& plotName, const wxString& title,
                                     const wxPoint& pos, const wxSize& size,
                                     const long style)
   : MdiChildViewFrame(parent, plotName, title, pos, size, style)
{
   // Create GLCanvas
   int width, height;
   GetClientSize(&width, &height);
   OrbitViewCanvas *canvas =
      new OrbitViewCanvas(this, -1, wxPoint(0, 0), wxSize(width, height), plotName);
   
   mCanvas = canvas;
   
   // this should work for MDI frames as well as for normal ones
   SetSizeHints(100, 100);
   GmatAppData::Instance()->GetMainFrame()->theMdiChildren->Append(this);
}


//------------------------------------------------------------------------------
// ~MdiChild3DViewFrame()
//------------------------------------------------------------------------------
MdiChild3DViewFrame::~MdiChild3DViewFrame()
{   
}

