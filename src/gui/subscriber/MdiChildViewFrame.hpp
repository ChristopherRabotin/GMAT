//$Id$
//------------------------------------------------------------------------------
//                              MdiChildViewFrame
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
// Created: 2010/04/19
/**
 * Declares MdiChildViewFrame for 3D visulaization.
 */
//------------------------------------------------------------------------------
#ifndef MdiChildViewFrame_hpp
#define MdiChildViewFrame_hpp

#include "GmatMdiChildFrame.hpp"
#include "MdiGlPlotData.hpp"      // for wxStringColorMap
#include "ViewCanvas.hpp"
#include "gmatdefs.hpp"
#include "SolarSystem.hpp"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/mdi.h"
#endif

class MdiChildViewFrame: public GmatMdiChildFrame
{
public:
   MdiChildViewFrame(wxMDIParentFrame *parent, const wxString& plotName,
                     const wxString& title, const wxPoint& pos,
                     const wxSize& size, const long style);
   virtual ~MdiChildViewFrame();
   
   // getters
   virtual wxString GetPlotName();
   virtual bool GetOverlapPlot();
   virtual ViewCanvas* GetGlCanvas();
   virtual wxGLContext* GetGLContext();
   virtual bool GetUseViewPointInfo();
   virtual bool GetDrawWireFrame();
   virtual bool GetDrawXyPlane();
   virtual bool GetDrawEcPlane();
   virtual bool GetDrawSunLine();
   virtual bool GetDrawAxes();
   virtual bool GetDrawGrid();
   virtual UnsignedInt GetXyPlaneColor();
   virtual UnsignedInt GetSunLineColor();
   virtual Integer GetAnimationUpdateInterval();
   virtual Integer GetAnimationFrameIncrement();
   
   // setters
   virtual void SetSolarSystem(SolarSystem *ss);
   virtual void SetGLContext(wxGLContext *glContext = NULL);
   virtual void SetUserInterrupt();
   
   virtual void SetPlotName(const wxString &name);
   virtual void ResetShowViewOption();
   virtual void SetOverlapPlot(bool overlap);
   virtual void SetUseInitialViewDef(bool flag);
   virtual void SetUsePerspectiveMode(bool flag);
   virtual void SetAnimationUpdateInterval(Integer value);
   virtual void SetAnimationFrameIncrement(Integer value);
   virtual void SetDrawWireFrame(bool flag);
   virtual void SetDrawStars(bool flag);
   virtual void SetDrawConstellations(bool flag);
   virtual void SetStarCount(int count);
   virtual void SetDrawXyPlane(bool flag);
   virtual void SetDrawEcPlane(bool flag);
   virtual void SetDrawSunLine(bool flag);
   virtual void SetDrawAxes(bool flag);
   virtual void SetDrawGrid(bool flag);
   virtual void SetXyPlaneColor(UnsignedInt color);
   virtual void SetSunLineColor(UnsignedInt color);
   virtual void SetObjectColors(const wxStringColorMap &objectColorMap);
   virtual void SetShowObjects(const wxStringBoolMap &showObjMap);
   virtual void SetNumPointsToRedraw(Integer numPoints);
   
   // actions
   virtual void DrawInOtherCoordSystem(const wxString &csName);
   virtual void RedrawPlot(bool viewAnimation);
   
   // menu actions
   virtual void OnClearPlot(wxCommandEvent& event);
   virtual void OnChangeTitle(wxCommandEvent& event);
   virtual void OnShowDefaultView(wxCommandEvent& event);
   
   // from GL option dialog
   virtual void OnDrawWireFrame(wxCommandEvent& event);
   virtual void OnDrawXyPlane(wxCommandEvent& event);
   
   virtual void OnQuit(wxCommandEvent& event);
   
   virtual void OnActivate(wxActivateEvent& event);
   virtual void OnPlotSize(wxSizeEvent& event);
   virtual void OnMove(wxMoveEvent& event);
   virtual void OnPlotClose(wxCloseEvent &event);
   virtual void OnClose(wxCloseEvent &event);
   
   // drawing
   virtual void SetGlObject(const StringArray &objNames,
                            const UnsignedIntArray &objOrbitColors,
                            const std::vector<SpacePoint*> &objArray);
   
   virtual void SetGlCoordSystem(CoordinateSystem *internalCs,
                                 CoordinateSystem *viewCs,
                                 CoordinateSystem *viewUpCs);
   
   virtual void SetGlViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                                SpacePoint *vdObj, Real vsFactor,
                                const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                                const Rvector3 &vdVec, const std::string &upAxis,
                                bool usevpRefVec, bool usevpVec, bool usevdVec,
                                bool useFixedFov, Real fov);
   
   virtual void SetGlDrawOrbitFlag(const std::vector<bool> &drawArray);
   virtual void SetGlShowObjectFlag(const std::vector<bool> &showArray);
   virtual void SetGlUpdateFrequency(Integer updFreq);
   
   virtual void UpdatePlot(const StringArray &scNames, const Real &time,
                           const RealArray &posX, const RealArray &posY,
                           const RealArray &posZ, const RealArray &velX,
                           const RealArray &velY, const RealArray &velZ,
                           const UnsignedIntArray &scColors, bool solving,
                           Integer solverOption, bool updateCanvas,
                           bool inFunction = false);
   virtual void TakeAction(const std::string &action);
   virtual void RefreshPlot();
   virtual void SetEndOfRun();
   virtual void DeletePlot();
      
protected:
   
   void CheckFrame();
   
   ViewCanvas *mCanvas;
   
   wxString mPlotName;
   wxString mPlotTitle;
   bool mOverlapPlot;
   bool mInFunction;
   
   wxArrayString mBodyNames;
   UnsignedIntArray mBodyColors;
};
#endif
