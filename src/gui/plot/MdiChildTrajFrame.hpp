//$Header$
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

class MdiChildTrajFrame: public wxMDIChildFrame
{
public:
   TrajPlotCanvas *mCanvas;
   
   bool mIsMainFrame;
   
   MdiChildTrajFrame(wxMDIParentFrame *parent, bool isMainFrame,
                     const wxString& plotName, const wxString& title,
                     const wxPoint& pos, const wxSize& size,
                     const long style, const wxString &csName = "",
                     SolarSystem *solarSys = NULL);
   ~MdiChildTrajFrame();
   
   // getters
   wxString GetPlotName() {return mPlotName;}
   bool GetOverlapPlot() {return mOverlapPlot;}
   bool GetUseViewPointInfo();
   bool GetUsePerspectiveMode();
   bool GetDrawWireFrame();
   bool GetDrawEqPlane();
   bool GetDrawEcPlane();
   bool GetDrawESLine();
   bool GetDrawAxes();
   bool GetRotateAboutXY();
   unsigned int GetEqPlaneColor();
   unsigned int GetEcPlaneColor();
   unsigned int GetESLineColor();
   float GetDistance();
   int   GetAnimationUpdateInterval();
   wxString GetGotoObjectName();
   wxString GetViewCoordSysName();
   CoordinateSystem* GetViewCoordSystem();
   
   // setters
   void SetPlotName(const wxString &name);
   void ResetShowViewOption();
   void SetOverlapPlot(bool overlap);
   void SetUseViewPointInfo(bool flag);
   void SetUsePerspectiveMode(bool flag);
   void SetAnimationUpdateInterval(int interval);
   void SetDrawWireFrame(bool flag);
   void SetDrawEqPlane(bool flag);
   void SetDrawEcPlane(bool flag);
   void SetDrawESLine(bool flag);
   void SetDrawAxes(bool flag);
   void SetRotateAboutXY(bool flag);
   void SetEqPlaneColor(unsigned int color);
   void SetEcPlaneColor(unsigned int color);
   void SetESLineColor(unsigned int color);
   void SetDistance(float dist);
   void SetGotoObjectName(const wxString &bodyName);
   void SetViewCoordSystem(const wxString &csName);
   void SetObjectColors(const wxStringColorMap &objectColorMap);
   void SetShowObjects(const wxStringBoolMap &showObjMap);
   void SetShowOrbitNormals(const wxStringBoolMap &showOrbNormMap);
   
   // actions
   void DrawInOtherCoordSystem(const wxString &csName);
   void RedrawPlot(bool viewAnimation);

   // menu actions
   void OnClearPlot(wxCommandEvent& event);
   void OnChangeTitle(wxCommandEvent& event);
   void OnShowDefaultView(wxCommandEvent& event);
   void OnZoomIn(wxCommandEvent& event);
   void OnZoomOut(wxCommandEvent& event);
   
   void OnShowOptionDialog(wxCommandEvent& event);
   void OnDrawWireFrame(wxCommandEvent& event);
   void OnDrawEqPlane(wxCommandEvent& event);
   
   //void OnAddBody(wxCommandEvent& event);
   //void OnGotoStdBody(wxCommandEvent& event);
   //void OnGotoOtherBody(wxCommandEvent& event);
   void OnViewAnimation(wxCommandEvent& event);
   
   void OnHelpView(wxCommandEvent& event);
   void OnQuit(wxCommandEvent& event);
   
   void OnActivate(wxActivateEvent& event);
   void OnTrajSize(wxSizeEvent& event);
   void OnMove(wxMoveEvent& event);
   void OnClose(wxCloseEvent& event);

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
   
   void SetGlDrawObjectFlag(const std::vector<bool> &drawArray);
   
   void UpdatePlot(const StringArray &scNames, const Real &time,
                   const RealArray &posX, const RealArray &posY,
                   const RealArray &posZ, const RealArray &velX,
                   const RealArray &velY, const RealArray &velZ,
                   const UnsignedIntArray &scColors, bool updateCanvas);
   
   void RefreshPlot();
   void DeletePlot();
   
protected:

   OpenGlOptionDialog *mOptionDialog;
   
   wxString mPlotName;
   wxString mPlotTitle;
   bool mOverlapPlot;

   wxArrayString mBodyNames;
   UnsignedIntArray mBodyColors;

//    wxMenu* CreateGotoBodyMenu();
//    int GetMenuId(const wxString &body);
   
   DECLARE_EVENT_TABLE();
};
#endif
