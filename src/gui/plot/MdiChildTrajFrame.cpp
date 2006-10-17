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
 * Implements MdiChildTrajFrame for opengl plot.
 */
//------------------------------------------------------------------------------
#include "gmatwxrcs.hpp"
#include "MdiChildTrajFrame.hpp"
#include "MdiGlPlotData.hpp"
#include "TrajPlotCanvas.hpp"
#include "CelesBodySelectDialog.hpp"
#include "OpenGlOptionDialog.hpp"
#include "GmatAppData.hpp"

#include "ColorTypes.hpp"         // for namespace GmatColor::
#include "MessageInterface.hpp"

//#define DEBUG_CHILDTRAJ_FRAME 1

BEGIN_EVENT_TABLE(MdiChildTrajFrame, GmatMdiChildFrame)
   EVT_MENU(GmatPlot::MDI_GL_CHILD_QUIT, MdiChildTrajFrame::OnQuit)
   EVT_MENU(GmatPlot::MDI_GL_CHANGE_TITLE, MdiChildTrajFrame::OnChangeTitle)
   EVT_MENU(GmatPlot::MDI_GL_CLEAR_PLOT, MdiChildTrajFrame::OnClearPlot)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_DEFAULT_VIEW, MdiChildTrajFrame::OnShowDefaultView)
   EVT_MENU(GmatPlot::MDI_GL_ZOOM_IN, MdiChildTrajFrame::OnZoomIn)
   EVT_MENU(GmatPlot::MDI_GL_ZOOM_OUT, MdiChildTrajFrame::OnZoomOut)

   EVT_MENU(GmatPlot::MDI_GL_SHOW_OPTION_PANEL, MdiChildTrajFrame::OnShowOptionDialog)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_WIRE_FRAME, MdiChildTrajFrame::OnDrawWireFrame)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_EQUATORIAL_PLANE, MdiChildTrajFrame::OnDrawXyPlane)

   EVT_MENU(GmatPlot::MDI_GL_VIEW_ANIMATION, MdiChildTrajFrame::OnViewAnimation)

   EVT_MENU(GmatPlot::MDI_GL_HELP_VIEW, MdiChildTrajFrame::OnHelpView)

   EVT_ACTIVATE(MdiChildTrajFrame::OnActivate)
   EVT_SIZE(MdiChildTrajFrame::OnTrajSize)
   EVT_MOVE(MdiChildTrajFrame::OnMove)
//   EVT_CLOSE(MdiChildTrajFrame::OnClose)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// MdiChildTrajFrame(wxMDIParentFrame *parent, const wxString& title, ...)
//------------------------------------------------------------------------------
MdiChildTrajFrame::MdiChildTrajFrame(wxMDIParentFrame *parent, bool isMainFrame,
                                     const wxString& plotName, const wxString& title,
                                     const wxPoint& pos, const wxSize& size,
                                     const long style, const wxString &csName,
                                     SolarSystem *solarSys)
//    : GmatMdiChildFrame(parent, -1, title, pos, size,
//                        style | wxNO_FULL_REPAINT_ON_RESIZE, plotName,
//                        GmatTree::OUTPUT_OPENGL_PLOT)
   : GmatMdiChildFrame(parent, plotName, title, GmatTree::OUTPUT_OPENGL_PLOT, -1,
                       pos, size, style | wxNO_FULL_REPAINT_ON_RESIZE)
{
   mCanvas = (TrajPlotCanvas *) NULL;
   mIsMainFrame = isMainFrame;
   mPlotName = plotName;
   mPlotTitle = plotName;
   
   mOverlapPlot = false;
   
   // add Sun, Earth, Luan as default body
   //mBodyNames.Add("Sun");
   mBodyNames.Add("Earth");
   //mBodyNames.Add("Luna");
   //mBodyColors.push_back(GmatColor::YELLOW32);
   mBodyColors.push_back(GmatColor::GREEN32);
   //mBodyColors.push_back(GmatColor::L_BROWN32);
   
   MdiGlPlot::mdiChildren.Append(this);
   
   // use this if we want option dialog to be modeless
   mOptionDialog = (OpenGlOptionDialog*)NULL;
   
   // Give it an icon
#ifdef __WXMSW__
   SetIcon(wxIcon(_T("chrt_icn")));
#else
   SetIcon(wxIcon( mondrian_xpm ));
#endif

   // Associate the menu bar with the frame
   //SetMenuBar(GmatAppData::GetMainFrame()->CreateMainMenu(GmatTree::OUTPUT_OPENGL_PLOT));

   // status bar
   //CreateStatusBar();
   //SetStatusText(title);
      
   // Create GLCanvas
   int width, height;
   GetClientSize(&width, &height);
   TrajPlotCanvas *canvas =
      new TrajPlotCanvas(this, -1, wxPoint(0, 0), wxSize(width, height),
                         csName, solarSys, plotName); //loj: 1/9/06 added plotName
   
   mCanvas = canvas;
   
   // this should work for MDI frames as well as for normal ones
   SetSizeHints(100, 100);
   GmatAppData::GetMainFrame()->mdiChildren->Append(this);
}


//------------------------------------------------------------------------------
// ~MdiChildTrajFrame()
//------------------------------------------------------------------------------
MdiChildTrajFrame::~MdiChildTrajFrame()
{
   if (mOptionDialog)
      delete mOptionDialog;
   
   mOptionDialog = (OpenGlOptionDialog*)NULL;
   
   #if DEBUG_CHILDTRAJ_FRAME
   MessageInterface::ShowMessage
      ("~MdiChildTrajFrame mPlotName=%s\n", mPlotName.c_str());
   #endif
   
   MdiGlPlot::mdiChildren.DeleteObject(this);
   
   MdiGlPlot::numChildren--;

}


//------------------------------------------------------------------------------
// bool GetUseViewPointInfo()
//------------------------------------------------------------------------------
bool MdiChildTrajFrame::GetUseViewPointInfo()
{
   if (mCanvas)
      return mCanvas->GetUseViewPointInfo();

   return false;
}


//------------------------------------------------------------------------------
// bool GetUsePerspectiveMode()
//------------------------------------------------------------------------------
bool MdiChildTrajFrame::GetUsePerspectiveMode()
{
   if (mCanvas)
      return mCanvas->GetUsePerspectiveMode();

   return false;
}


//------------------------------------------------------------------------------
// bool GetDrawWireFrame()
//------------------------------------------------------------------------------
bool MdiChildTrajFrame::GetDrawWireFrame()
{
   if (mCanvas)
      return mCanvas->GetDrawWireFrame();

   return false;
}


//------------------------------------------------------------------------------
// bool GetRotateAboutXY()
//------------------------------------------------------------------------------
bool MdiChildTrajFrame::GetRotateAboutXY()
{
   if (mCanvas)
      return mCanvas->GetRotateAboutXY();

   return false;
}


//------------------------------------------------------------------------------
// bool GetDrawXyPlane()
//------------------------------------------------------------------------------
bool MdiChildTrajFrame::GetDrawXyPlane()
{
   if (mCanvas)
      return mCanvas->GetDrawXyPlane();

   return false;
}

//------------------------------------------------------------------------------
// bool GetDrawEcPlane()
//------------------------------------------------------------------------------
bool MdiChildTrajFrame::GetDrawEcPlane()
{
   if (mCanvas)
      return mCanvas->GetDrawEcPlane();

   return false;
}


//------------------------------------------------------------------------------
// bool GetDrawESLines()
//------------------------------------------------------------------------------
bool MdiChildTrajFrame::GetDrawESLines()
{
   if (mCanvas)
      return mCanvas->GetDrawESLines();

   return false;
}


//------------------------------------------------------------------------------
// bool GetDrawAxes()
//------------------------------------------------------------------------------
bool MdiChildTrajFrame::GetDrawAxes()
{
   if (mCanvas)
      return mCanvas->GetDrawAxes();

   return false;
}

//------------------------------------------------------------------------------
// bool GetDrawGrid()
//------------------------------------------------------------------------------
bool MdiChildTrajFrame::GetDrawGrid()
{
   if (mCanvas)
      return mCanvas->GetDrawGrid();

   return false;
}


//------------------------------------------------------------------------------
// UnsignedInt GetXyPlaneColor()
//------------------------------------------------------------------------------
UnsignedInt MdiChildTrajFrame::GetXyPlaneColor()
{
   if (mCanvas)
      return mCanvas->GetXyPlaneColor();

   return 0;
}


//------------------------------------------------------------------------------
// UnsignedInt GetEcPlaneColor()
//------------------------------------------------------------------------------
UnsignedInt MdiChildTrajFrame::GetEcPlaneColor()
{
   if (mCanvas)
      return mCanvas->GetEcPlaneColor();

   return 0;
}

//------------------------------------------------------------------------------
// UnsignedInt GetESLineColor()
//------------------------------------------------------------------------------
UnsignedInt MdiChildTrajFrame::GetESLineColor()
{
   if (mCanvas)
      return mCanvas->GetESLineColor();

   return 0;
}

//------------------------------------------------------------------------------
// float GetDistance()
//------------------------------------------------------------------------------
float MdiChildTrajFrame::GetDistance()
{
   if (mCanvas)
      return mCanvas->GetDistance();

   return 50000;
}


//------------------------------------------------------------------------------
// Integer GetAnimationUpdateInterval()
//------------------------------------------------------------------------------
Integer MdiChildTrajFrame::GetAnimationUpdateInterval()
{
   if (mCanvas)
      return mCanvas->GetAnimationUpdateInterval();

   return 0;
}


//------------------------------------------------------------------------------
// wxString GetGotoObjectName()
//------------------------------------------------------------------------------
wxString MdiChildTrajFrame::GetGotoObjectName()
{
   if (mCanvas)
   {
     return mCanvas->GetGotoObjectName();
   }
   
   return "Unknown";
}


//------------------------------------------------------------------------------
// wxString GetViewCoordSysName()
//------------------------------------------------------------------------------
wxString MdiChildTrajFrame::GetViewCoordSysName()
{
   if (mCanvas)
      return mCanvas->GetViewCoordSysName();

   return "Unknown";
}


//------------------------------------------------------------------------------
// CoordinateSystem* GetViewCoordSystem()
//------------------------------------------------------------------------------
CoordinateSystem* MdiChildTrajFrame::GetViewCoordSystem()
{
   if (mCanvas)
      return mCanvas->GetViewCoordSystem();

   return NULL;
}


// //------------------------------------------------------------------------------
// // const StringArray& GetBodyNamesInUse()
// //------------------------------------------------------------------------------
// const StringArray& MdiChildTrajFrame::GetBodyNamesInUse()
// {
//    static StringArray tempArray;
   
//    if (mCanvas)
//       return mCanvas->GetBodyNamesInUse();

//    return tempArray;
// }


//------------------------------------------------------------------------------
// void SetPlotName(const wxString &name)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetPlotName(const wxString &name)
{
   #if DEBUG_CHILDTRAJ_FRAME
      MessageInterface::ShowMessage
         ("MdiChildTrajFrame::SetPlotName() name=%s\n", name.c_str());
   #endif
   
   mPlotName = name;
   mPlotTitle = name;
   SetTitle(mPlotTitle);
}


//------------------------------------------------------------------------------
// void ResetShowViewOption()
//------------------------------------------------------------------------------
void MdiChildTrajFrame::ResetShowViewOption()
{
   #if DEBUG_CHILDTRAJ_FRAME
   MessageInterface::ShowMessage
      ("MdiChildTrajFrame::ResetShowViewOption()\n");
   #endif

    menuBar->Check(GmatPlot::MDI_GL_SHOW_OPTION_PANEL, false);
}


//------------------------------------------------------------------------------
// void SetOverlapPlot(bool overlap)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetOverlapPlot(bool overlap)
{
   mOverlapPlot = overlap;
   
   if (mCanvas)
   {
      if (!overlap)
         mCanvas->ClearPlot();
   }
}


//------------------------------------------------------------------------------
// void SetUseInitialViewDef(bool flag)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetUseInitialViewDef(bool flag)
{
   if (mCanvas)
   {
      mCanvas->SetUseInitialViewDef(flag);
   }
}


//------------------------------------------------------------------------------
// void SetUsePerspectiveMode(bool flag)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetUsePerspectiveMode(bool flag)
{
   if (mCanvas)
   {
      mCanvas->SetUsePerspectiveMode(flag);
   }
}


//------------------------------------------------------------------------------
// void SetAnimationUpdateInterval(nt interval)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetAnimationUpdateInterval(int interval)
{
   if (mCanvas)
   {
      mCanvas->SetAnimationUpdateInterval(interval);
   }
}


//------------------------------------------------------------------------------
// void SetDrawWireFrame(bool flag)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetDrawWireFrame(bool flag)
{
   if (mCanvas)
   {
      //loj: 5/18/05 Why this failing when this frame is closed?
      menuBar->Check(GmatPlot::MDI_GL_SHOW_WIRE_FRAME, flag);
      mCanvas->SetDrawWireFrame(flag);
   }
}


//------------------------------------------------------------------------------
// void SetDrawXyPlane(bool flag)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetDrawXyPlane(bool flag)
{
   if (mCanvas)
   {
      //loj: 5/18/05 Why this failing when this frame is closed?
      menuBar->Check(GmatPlot::MDI_GL_SHOW_EQUATORIAL_PLANE, flag);   
      mCanvas->SetDrawXyPlane(flag);
   }
}


//------------------------------------------------------------------------------
// void SetDrawEcPlane(bool flag)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetDrawEcPlane(bool flag)
{
   if (mCanvas)
   {
      //loj: the event ID is not in the GmatPlot yet
      //mViewOptionMenu->Check(GmatPlot::MDI_GL_SHOW_ECLIPTIC_PLANE, flag);
      mCanvas->SetDrawEcPlane(flag);
   }
}


//------------------------------------------------------------------------------
// void SetDrawESLines(bool flag)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetDrawESLines(bool flag)
{
   if (mCanvas)
   {
      //loj: the event ID is not in the GmatPlot yet
      //mViewOptionMenu->Check(GmatPlot::MDI_GL_SHOW_ECLIPTIC_LINE, flag);
      mCanvas->SetDrawESLines(flag);
   }
}


//------------------------------------------------------------------------------
// void SetDrawAxes(bool flag)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetDrawAxes(bool flag)
{
   if (mCanvas)
   {
      //loj: the event ID is not in the GmatPlot yet
      //mViewOptionMenu->Check(GmatPlot::MDI_GL_SHOW_AXES, flag);
      mCanvas->SetDrawAxes(flag);
   }
}


//------------------------------------------------------------------------------
// void SetDrawGrid(bool flag)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetDrawGrid(bool flag)
{
   if (mCanvas)
   {
      //loj: the event ID is not in the GmatPlot yet
      //mViewOptionMenu->Check(GmatPlot::MDI_GL_SHOW_GRID, flag);
      mCanvas->SetDrawGrid(flag);
   }
}


//------------------------------------------------------------------------------
// void SetRotateAboutXY(bool flag)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetRotateAboutXY(bool flag)
{
   if (mCanvas)
   {
      //loj: the event ID is not in the GmatPlot yet
      //mViewOptionMenu->Check(GmatPlot::MDI_GL_ROTATE_ABOUT_XY, flag);
      mCanvas->SetRotateAboutXY(flag);
   }
}


//------------------------------------------------------------------------------
// void SetXyPlaneColor(UnsignedInt color)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetXyPlaneColor(UnsignedInt color)
{
   if (mCanvas)
   {
      mCanvas->SetXyPlaneColor(color);
   }
}


//------------------------------------------------------------------------------
// void SetEcPlaneColor(UnsignedInt color)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetEcPlaneColor(UnsignedInt color)
{
   if (mCanvas)
   {
      mCanvas->SetEcPlaneColor(color);
   }
}


//------------------------------------------------------------------------------
// void SetESLineColor(UnsignedInt color)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetESLineColor(UnsignedInt color)
{
   if (mCanvas)
   {
      mCanvas->SetESLineColor(color);
   }
}


//------------------------------------------------------------------------------
// void SetDistance(float dist)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetDistance(float dist)
{
   if (mCanvas)
      return mCanvas->SetDistance(dist);
}


//------------------------------------------------------------------------------
// void SetGotoObjectName(const wxString &bodyName)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetGotoObjectName(const wxString &bodyName)
{
   if (mCanvas)
   {
      mCanvas->GotoObject(bodyName);
   }
}


//------------------------------------------------------------------------------
// void SetViewCoordSystem(const wxString &csName)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetViewCoordSystem(const wxString &csName)
{
   if (mCanvas)
   {
      mCanvas->SetViewCoordSystem(csName);
   }
}


//------------------------------------------------------------------------------
// void SetNumPointsToRedraw(Integer numPoints)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetNumPointsToRedraw(Integer numPoints)
{
   if (mCanvas)
   {
      mCanvas->SetNumPointsToRedraw(numPoints);
   }
}


//------------------------------------------------------------------------------
// void SetObjectColors(const wxStringColorMap &objectColorMap)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetObjectColors(const wxStringColorMap &objectColorMap)
{
   if (mCanvas)
      mCanvas->SetObjectColors(objectColorMap);
}


//------------------------------------------------------------------------------
// void SetShowObjects(const wxStringBoolMap &showObjMap)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetShowObjects(const wxStringBoolMap &showObjMap)
{
   if (mCanvas)
      mCanvas->SetShowObjects(showObjMap);
}


//------------------------------------------------------------------------------
// void SetShowOrbitNormals(const wxStringBoolMap &showOrbNormMap)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetShowOrbitNormals(const wxStringBoolMap &showOrbNormMap)
{
   if (mCanvas)
      mCanvas->SetShowOrbitNormals(showOrbNormMap);
}


// actions
//------------------------------------------------------------------------------
// void DrawInOtherCoordSystem(const wxString &csName)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::DrawInOtherCoordSystem(const wxString &csName)
{
   if (mCanvas)
   {
      mCanvas->DrawInOtherCoordSystem(csName);
   }
   
   mOptionDialog->SetGotoObjectName(mCanvas->GetGotoObjectName());
}


//------------------------------------------------------------------------------
// void RedrawPlot(bool viewAnimation)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::RedrawPlot(bool viewAnimation)
{
   #ifdef DEBUG_CHILDTRAJ_FRAME
   MessageInterface::ShowMessage("MdiChildTrajFrame::RedrawPlot() entered.\n");
   #endif
   
   if (mCanvas)
      mCanvas->RedrawPlot(viewAnimation);

   mOptionDialog->SetDistance(mCanvas->GetDistance());
   mOptionDialog->SetCoordSysName(mCanvas->GetViewCoordSysName());
   mOptionDialog->SetGotoObjectName(mCanvas->GetGotoObjectName());
}


// menu actions
//------------------------------------------------------------------------------
// void OnClearPlot(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnClearPlot(wxCommandEvent& WXUNUSED(event))
{
   if (mCanvas)
      mCanvas->ClearPlot();
}


//------------------------------------------------------------------------------
// void OnChangeTitle(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnChangeTitle(wxCommandEvent& WXUNUSED(event))
{
   //#if wxUSE_TEXTDLG
   //static wxString s_title = _T("Canvas Frame");

   wxString title = wxGetTextFromUser(_T("Enter the new title for MDI child"),
                                      _T(""),
                                      mPlotTitle, //s_title,
                                      GetParent()->GetParent());
   if ( !title )
      return;

   mPlotTitle = title;
   //s_title = title;
   //SetTitle(s_title);
   SetTitle(title);
   //#endif
}


//------------------------------------------------------------------------------
// void OnShowDefaultView(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnShowDefaultView(wxCommandEvent& event)
{
   if (mCanvas)
      mCanvas->ShowDefaultView();
}

//------------------------------------------------------------------------------
// void OnZoomIn(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnZoomIn(wxCommandEvent& event)
{
   if (mCanvas)
      mCanvas->ZoomIn();
}

//------------------------------------------------------------------------------
// void OnZoomOut(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnZoomOut(wxCommandEvent& event)
{
   if (mCanvas)
      mCanvas->ZoomOut();
}


//------------------------------------------------------------------------------
// void OnShowOptionDialog(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnShowOptionDialog(wxCommandEvent& event)
{   
   if (event.IsChecked())
   {
      if (mOptionDialog == NULL)
         mOptionDialog = new OpenGlOptionDialog(this, mPlotName, mBodyNames,
                                                mBodyColors);

      mOptionDialog->UpdateObjectList(mCanvas->GetObjectNames(),
                                      mCanvas->GetValidCSNames(),
                                      mCanvas->GetShowObjectMap(),
                                      mCanvas->GetObjectColorMap());
      
      int x, y, newX;
      GmatAppData::GetMainFrame()->GetPosition(&x, &y);
      newX = x-20;
      
      if (newX < 0)
         newX = 5;
     
      mOptionDialog->Move(newX, y+100);
      mOptionDialog->Show(true); // modeless dialog
   }
   else
   {
      mOptionDialog->Hide(); // modeless dialog
   }
}


//------------------------------------------------------------------------------
// void OnDrawWireFrame(wxCommandEvent& event)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnDrawWireFrame(wxCommandEvent& event)
{
   if (mCanvas)
      mCanvas->DrawWireFrame(event.IsChecked());
   
   if (mOptionDialog)
      mOptionDialog->SetDrawWireFrame(event.IsChecked());
}


//------------------------------------------------------------------------------
// void OnDrawXyPlane(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnDrawXyPlane(wxCommandEvent& event)
{
   if (mCanvas)
      mCanvas->DrawXyPlane(event.IsChecked());

   if (mOptionDialog)
      mOptionDialog->SetDrawXyPlane(event.IsChecked());
}


//------------------------------------------------------------------------------
// void OnViewAnimation(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnViewAnimation(wxCommandEvent& event)
{
   //loj: How do I know the body name from the event?
   if (mCanvas)
   {
      wxString strInterval = "10";
      strInterval = wxGetTextFromUser(wxT("Enter Interval (milli-secs): "
                                          "<Press ESC for interrupt>"),
                                      wxT("Update Interval"),
                                      strInterval, this);
      
      // 10/17/2006 - arg:  put in to fix bug 400                                
      if (strInterval == "")
         return; 
         
      long interval;
      strInterval.ToLong(&interval);
      
      // set maximum interval to 100 milli seconds
      if (interval > 100)
         interval = 100;
      
      mCanvas->ViewAnimation(interval);
   }
}


//------------------------------------------------------------------------------
// void OnHelpView(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnHelpView(wxCommandEvent& event)
{
   wxMessageBox(_T("Use Left mouse button to rotate \n"
                   "Right mouse button to zoom \n"
                   "Shift left mouse button to translate"),
                _T("Help View"),
                wxOK | wxICON_INFORMATION, this);
}


//------------------------------------------------------------------------------
// void OnQuit(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
   Close(TRUE);
}


//------------------------------------------------------------------------------
// void OnActivate(wxActivateEvent& event)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnActivate(wxActivateEvent& event)
{
   if ( event.GetActive() && mCanvas )
   {
      mCanvas->SetFocus();
   }
}

//------------------------------------------------------------------------------
// void OnMove(wxMoveEvent& event)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnMove(wxMoveEvent& event)
{
   // VZ: here everything is totally wrong under MSW, the positions are
   //     different and both wrong (pos2 is off by 2 pixels for me which seems
   //     to be the width of the MDI canvas border)
   wxPoint pos1 = event.GetPosition();
   wxPoint pos2 = GetPosition();

   //loj: 8/4/05 Commented out
   //wxLogStatus(GmatAppData::GetMainFrame(),
   //            wxT("position from event: (%d, %d), from frame (%d, %d)"),
   //            pos1.x, pos1.y, pos2.x, pos2.y);
   
   event.Skip();
}

//------------------------------------------------------------------------------
// void OnTrajSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnTrajSize(wxSizeEvent& event)
{
   // VZ: under MSW the size event carries the client size (quite
   //     unexpectedly) *except* for the very first one which has the full
   //     size... what should it really be? TODO: check under wxGTK
   wxSize size1 = event.GetSize();
   wxSize size2 = GetSize();
   wxSize size3 = GetClientSize();

   //loj: 8/4/05 commented out
   //wxLogStatus(GmatAppData::GetMainFrame(),
   //            wxT("size from event: %dx%d, from frame %dx%d, client %dx%d"),
   //            size1.x, size1.y, size2.x, size2.y, size3.x, size3.y);
   
   event.Skip();
}


//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent& event)
//------------------------------------------------------------------------------
//void MdiChildTrajFrame::OnClose(wxCloseEvent& event)
//{
//   #if DEBUG_CHILDTRAJ_FRAME
//   MessageInterface::ShowMessage
//      ("MdiChildTrajFrame::OnClose() this->PlotName=%s\n", mPlotName.c_str());
//   #endif
//
////    if (mIsMainFrame)
////    {
////       MessageInterface::ShowMessage
////          ("MdiChildTrajFrame::OnClose() this->PlotName=%s\n", mPlotName.c_str());
//
////       //GmatAppData::GetMainFrame()->trajMainSubframe = NULL;
//
////    }
//
//   //MdiGlPlot::numChildren--; //loj: 5/18/05 moved to destructor
//
////    if (MdiGlPlot::numChildren == 0)
////    {
//// //       GmatAppData::GetMainFrame()->trajMainSubframe = NULL;
////       GmatAppData::GetMainFrame()->trajSubframe = NULL;
////    }
//
//   event.Skip();
//}


//------------------------------------------------------------------------------
// void SetGlObject(const StringArray &objNames, ...
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetGlObject(const StringArray &objNames,
                                    const UnsignedIntArray &objOrbitColors,
                                    const std::vector<SpacePoint*> &objArray)
{
   if (mCanvas)
   {         
      mCanvas->SetGlObject(objNames, objOrbitColors, objArray);
   }
}


//------------------------------------------------------------------------------
// void SetGlObject(CoordinateSystem *viewCs, ...
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetGlCoordSystem(CoordinateSystem *viewCs,
                                         CoordinateSystem *viewUpCs)
{
   if (mCanvas)
   {         
      mCanvas->SetGlCoordSystem(viewCs, viewUpCs);
   }
}


//------------------------------------------------------------------------------
// void SetGlViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj, ...
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetGlViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                                        SpacePoint *vdObj, Real vsFactor,
                                        const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                                        const Rvector3 &vdVec, const std::string &upAxis,
                                        bool usevpRefVec, bool usevpVec, bool usevdVec,
                                        bool useFixedFov, Real fov)
{
   if (mCanvas)
   {
      #if DEBUG_CHILDTRAJ_FRAME
         MessageInterface::ShowMessage
            ("MdiChildTrajFrame::SetGlViewOption() vsFactor=%f\n", vsFactor);
      #endif
         
      mCanvas->SetGlViewOption(vpRefObj, vpVecObj, vdObj, vsFactor, vpRefVec,
                               vpVec, vdVec, upAxis, usevpRefVec, usevpVec,
                               usevdVec, useFixedFov, fov);
   }
}


//------------------------------------------------------------------------------
// void SetGlDrawOrbitFlag(const std::vector<bool> &drawArray)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetGlDrawOrbitFlag(const std::vector<bool> &drawArray)
{
   if (mCanvas)
   {
      mCanvas->SetGlDrawOrbitFlag(drawArray);
   }
}


//------------------------------------------------------------------------------
// void SetGlShowObjectFlag(const std::vector<bool> &showArray)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetGlShowObjectFlag(const std::vector<bool> &showArray)
{
   if (mCanvas)
   {
      mCanvas->SetGlShowObjectFlag(showArray);
   }
}


//------------------------------------------------------------------------------
// void SetGlUpdateFrequency(Integer updFreq)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetGlUpdateFrequency(Integer updFreq)
{
   if (mCanvas)
   {
      mCanvas->SetUpdateFrequency(updFreq);
   }
}


//------------------------------------------------------------------------------
// void UpdatePlot(const StringArray &scNames, ...
//------------------------------------------------------------------------------
void MdiChildTrajFrame::UpdatePlot(const StringArray &scNames, const Real &time,
                                   const RealArray &posX, const RealArray &posY,
                                   const RealArray &posZ, const RealArray &velX,
                                   const RealArray &velY, const RealArray &velZ,
                                   const UnsignedIntArray &scColors,
                                   bool updateCanvas)
{
   if (mCanvas)
   {
      mCanvas->UpdatePlot(scNames, time, posX, posY, posZ, velX, velY, velZ,
                          scColors);
      
      if (updateCanvas)
      {
         //MessageInterface::ShowMessage
         //   ("===> MdiChildTrajFrame::UpdatePlot() time=%f\n", time);
         
         //loj: 9/2/05
         // Added mCanvas->Refresh(false) here since Refresh() is removed from the
         // TrajPlotCanvas::UpdatePlot(). The UpdateFrequency was not actually used
         // since TrajPlotCanvas::UpdatePlot() always called Refresh().
         // The correct use UpdateFrequency will improve performance.
         
         mCanvas->Refresh(false);
         Update();
         
         #ifndef __WXMSW__
            ::wxYield();
         #endif
      }
   }
}


//------------------------------------------------------------------------------
// void RefreshPlot()
//------------------------------------------------------------------------------
/*
 * Activates OnPaint() event
 */
//------------------------------------------------------------------------------
void MdiChildTrajFrame::RefreshPlot()
{
   if (mCanvas)
   {
      mCanvas->SetEndOfData(true);
      mCanvas->Refresh(false);
      Update();
   }
}


//------------------------------------------------------------------------------
// void SetEndOfRun()
//------------------------------------------------------------------------------
/*
 * Sets end of run flag.
 */
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetEndOfRun()
{
   if (mCanvas)
   {
      mCanvas->SetEndOfRun(true);
      mCanvas->Refresh(false);
      Update();
   }
}


//------------------------------------------------------------------------------
// void DeletePlot()
//------------------------------------------------------------------------------
void MdiChildTrajFrame::DeletePlot()
{
   // This will call OnClose()
   Close(TRUE);
   //if (mIsMainFrame)
   //   GmatAppData::GetMainFrame()->trajMainSubframe->Close();
}
