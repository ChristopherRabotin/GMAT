//$Id$
//------------------------------------------------------------------------------
//                              MdiChildTrajFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2003/11/25
/**
 * Declares MdiChildTrajFrame for opengl plot.
 */
//------------------------------------------------------------------------------
#ifndef MdiChildTrajFrame_hpp
#define MdiChildTrajFrame_hpp

#include "GmatMdiChildFrame.hpp"
#include "TrajPlotCanvas.hpp"
#include "OpenGlOptionDialog.hpp"

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

class MdiChildTrajFrame: public GmatMdiChildFrame
{
public:
   MdiChildTrajFrame(wxMDIParentFrame *parent, const wxString& plotName,
                     const wxString& title, const wxPoint& pos,
                     const wxSize& size, const long style);
   virtual ~MdiChildTrajFrame();
   
   // getters
   wxString GetPlotName() { return mPlotName; }
   bool GetOverlapPlot() { return mOverlapPlot; }
   TrajPlotCanvas* GetGlCanvas() { return mCanvas; }
   wxGLContext* GetGLContext() { return mCanvas->GetGLContext(); }
   OpenGlOptionDialog* GetOptionDialog() { return mOptionDialog; }
   bool GetUseViewPointInfo();
   bool GetDrawWireFrame();
   bool GetDrawXyPlane();
   bool GetDrawEcPlane();
   bool GetDrawSunLine();
   bool GetDrawAxes();
   bool GetDrawGrid();
   UnsignedInt GetXyPlaneColor();
   UnsignedInt GetSunLineColor();
   Integer GetAnimationUpdateInterval();
   Integer GetAnimationFrameIncrement();
   
   // setters
   void SetSolarSystem(SolarSystem *ss);
   void SetGLContext(wxGLContext *glContext = NULL);
   void SetUserInterrupt();
   
   void SetPlotName(const wxString &name);
   void ResetShowViewOption();
   void SetOverlapPlot(bool overlap);
   void SetUseInitialViewDef(bool flag);
   void SetUsePerspectiveMode(bool flag);
   void SetAnimationUpdateInterval(Integer value);
   void SetAnimationFrameIncrement(Integer value);
   void SetDrawWireFrame(bool flag);
   void SetDrawXyPlane(bool flag);
   void SetDrawEcPlane(bool flag);
   void SetDrawSunLine(bool flag);
   void SetDrawAxes(bool flag);
   void SetDrawGrid(bool flag);
   void SetXyPlaneColor(UnsignedInt color);
   void SetSunLineColor(UnsignedInt color);
   void SetObjectColors(const wxStringColorMap &objectColorMap);
   void SetShowObjects(const wxStringBoolMap &showObjMap);
   void SetShowOrbitNormals(const wxStringBoolMap &showOrbNormMap);
   void SetNumPointsToRedraw(Integer numPoints);
   
   // actions
   void DrawInOtherCoordSystem(const wxString &csName);
   void RedrawPlot(bool viewAnimation);
   
   // menu actions
   void OnClearPlot(wxCommandEvent& event);
   void OnChangeTitle(wxCommandEvent& event);
   void OnShowDefaultView(wxCommandEvent& event);

   // from GL option dialog
   void OnShowOptionDialog(wxCommandEvent& event);
   void OnDrawWireFrame(wxCommandEvent& event);
   void OnDrawXyPlane(wxCommandEvent& event);
   
   void OnQuit(wxCommandEvent& event);
   
   void OnActivate(wxActivateEvent& event);
   void OnPlotSize(wxSizeEvent& event);
   void OnMove(wxMoveEvent& event);
   void OnPlotClose(wxCloseEvent &event);
   void OnClose(wxCloseEvent &event);
   
   // drawing
   void SetGlObject(const StringArray &objNames,
                    const UnsignedIntArray &objOrbitColors,
                    const std::vector<SpacePoint*> &objArray);
   
   void SetGlCoordSystem(CoordinateSystem *viewCs,
                         CoordinateSystem *viewUpCs);
   
   void SetGlViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                        SpacePoint *vdObj, Real vsFactor,
                        const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                        const Rvector3 &vdVec, const std::string &upAxis,
                        bool usevpRefVec, bool usevpVec, bool usevdVec,
                        bool useFixedFov, Real fov);
   
   void SetGlDrawOrbitFlag(const std::vector<bool> &drawArray);
   void SetGlShowObjectFlag(const std::vector<bool> &showArray);
   void SetGlUpdateFrequency(Integer updFreq);
   
   void UpdatePlot(const StringArray &scNames, const Real &time,
                   const RealArray &posX, const RealArray &posY,
                   const RealArray &posZ, const RealArray &velX,
                   const RealArray &velY, const RealArray &velZ,
                   const UnsignedIntArray &scColors, bool solving,
                   Integer solverOption, bool updateCanvas);
   void TakeAction(const std::string &action);
   void RefreshPlot();
   void SetEndOfRun();
   void DeletePlot();
   
   // OptionDialog
   void EnableAnimation(bool enable);
   
protected:
   
   TrajPlotCanvas *mCanvas;
   OpenGlOptionDialog *mOptionDialog;
   
   wxString mPlotName;
   wxString mPlotTitle;
   bool mOverlapPlot;
   
   wxArrayString mBodyNames;
   UnsignedIntArray mBodyColors;
   
   void CheckFrame();
   
   DECLARE_EVENT_TABLE();
};
#endif
