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
#include "GmatMainFrame.hpp"            // for theMdiChildren->Append


BEGIN_EVENT_TABLE(MdiChild3DViewFrame, MdiChildViewFrame)
   EVT_MENU(GmatPlot::MDI_GL_CHILD_QUIT, MdiChildViewFrame::OnQuit)
   EVT_MENU(GmatPlot::MDI_GL_CHANGE_TITLE, MdiChildViewFrame::OnChangeTitle)
   EVT_MENU(GmatPlot::MDI_GL_CLEAR_PLOT, MdiChildViewFrame::OnClearPlot)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_DEFAULT_VIEW, MdiChildViewFrame::OnShowDefaultView)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_WIRE_FRAME, MdiChildViewFrame::OnDrawWireFrame)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_EQUATORIAL_PLANE, MdiChildViewFrame::OnDrawXyPlane)
   //EVT_ACTIVATE(MdiChildViewFrame::OnActivate)
   //EVT_SIZE(MdiChildViewFrame::OnPlotSize)
   EVT_MOVE(MdiChildViewFrame::OnMove)
   EVT_CLOSE(MdiChildViewFrame::OnPlotClose) 
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


//------------------------------------------------------------------------------
// void SetGl3dDrawingOption(bool drawEcPlane, bool drawXyPlane, ...)
//------------------------------------------------------------------------------
void MdiChild3DViewFrame::SetGl3dDrawingOption(bool drawEcPlane, bool drawXyPlane,
                                               bool drawWireFrame, bool drawAxes,
                                               bool drawGrid, bool drawSunLine,
                                               bool overlapPlot, bool usevpInfo,
                                               bool drawStars, bool drawConstellations,
                                               Integer starCount)
{
   if (mCanvas)
   {
      SetOverlapPlot(overlapPlot);
      mCanvas->SetGl3dDrawingOption(drawEcPlane, drawXyPlane, drawWireFrame,
                                    drawAxes, drawGrid, drawSunLine, usevpInfo,
                                    drawStars, drawConstellations, starCount);
   }
}


//------------------------------------------------------------------------------
// void SetGl3dViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj, ...)
//------------------------------------------------------------------------------
void MdiChild3DViewFrame::SetGl3dViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                                            SpacePoint *vdObj, Real vsFactor,
                                            const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                                            const Rvector3 &vdVec, const std::string &upAxis,
                                            bool usevpRefVec, bool usevpVec, bool usevdVec)
{
   if (mCanvas)
   {
      #ifdef DEBUG_VIEW_FRAME
      MessageInterface::ShowMessage
         ("MdiChild3dViewFrame::SetGl3dViewOption() vsFactor=%f\n", vsFactor);
      #endif
      
      mCanvas->SetGl3dViewOption(vpRefObj, vpVecObj, vdObj, vsFactor, vpRefVec,
                                 vpVec, vdVec, upAxis, usevpRefVec, usevpVec,
                                 usevdVec);
   }
}

