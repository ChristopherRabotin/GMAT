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
#include "GmatMainFrame.hpp"      // for namespace GmatMenu
#include "ColorTypes.hpp"         // for namespace GmatColor::
#include "MessageInterface.hpp"

//#define DEBUG_TRAJ_FRAME 1
//#define DEBUG_MDI_CHILD_FRAME_CLOSE 1

BEGIN_EVENT_TABLE(MdiChildTrajFrame, GmatMdiChildFrame)
   EVT_MENU(GmatPlot::MDI_GL_CHILD_QUIT, MdiChildTrajFrame::OnQuit)
   EVT_MENU(GmatPlot::MDI_GL_CHANGE_TITLE, MdiChildTrajFrame::OnChangeTitle)
   EVT_MENU(GmatPlot::MDI_GL_CLEAR_PLOT, MdiChildTrajFrame::OnClearPlot)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_DEFAULT_VIEW, MdiChildTrajFrame::OnShowDefaultView)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_OPTION_PANEL, MdiChildTrajFrame::OnShowOptionDialog)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_WIRE_FRAME, MdiChildTrajFrame::OnDrawWireFrame)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_EQUATORIAL_PLANE, MdiChildTrajFrame::OnDrawXyPlane)
   EVT_ACTIVATE(MdiChildTrajFrame::OnActivate)
   EVT_SIZE(MdiChildTrajFrame::OnPlotSize)
   EVT_MOVE(MdiChildTrajFrame::OnMove)
   EVT_CLOSE(MdiChildTrajFrame::OnPlotClose) 
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// MdiChildTrajFrame(wxMDIParentFrame *parent, const wxString& title, ...)
//------------------------------------------------------------------------------
MdiChildTrajFrame::MdiChildTrajFrame(wxMDIParentFrame *parent,
                                     const wxString& plotName, const wxString& title,
                                     const wxPoint& pos, const wxSize& size,
                                     const long style)
   : GmatMdiChildFrame(parent, plotName, title, GmatTree::OUTPUT_OPENGL_PLOT, -1,
                       pos, size, style | wxNO_FULL_REPAINT_ON_RESIZE)
{
   mCanvas = (TrajPlotCanvas *) NULL;
   mPlotName = plotName;
   mPlotTitle = plotName;
   
   mOverlapPlot = false;
   
   // add Earth as default body
   mBodyNames.Add("Earth");
   mBodyColors.push_back(GmatColor::GREEN32);
   
   MdiGlPlot::mdiChildren.Append(this);
   
   // use this if we want option dialog to be modeless
   mOptionDialog = (OpenGlOptionDialog*)NULL;
   
   // Give it an icon
#ifdef __WXMSW__
   SetIcon(wxIcon(_T("chrt_icn")));
#else
   SetIcon(wxIcon( mondrian_xpm ));
#endif
   
   
   // Create GLCanvas
   int width, height;
   GetClientSize(&width, &height);
   TrajPlotCanvas *canvas =
      new TrajPlotCanvas(this, -1, wxPoint(0, 0), wxSize(width, height), plotName);
   
   mCanvas = canvas;
   
   // this should work for MDI frames as well as for normal ones
   SetSizeHints(100, 100);
   GmatAppData::GetMainFrame()->theMdiChildren->Append(this);
}


//------------------------------------------------------------------------------
// ~MdiChildTrajFrame()
//------------------------------------------------------------------------------
MdiChildTrajFrame::~MdiChildTrajFrame()
{
   if (mOptionDialog)
      delete mOptionDialog;
   
   mOptionDialog = (OpenGlOptionDialog*)NULL;
   
   #if DEBUG_MDI_CHILD_FRAME_CLOSE
   MessageInterface::ShowMessage
      ("~MdiChildTrajFrame() mPlotName=%s\n", mPlotName.c_str());
   #endif
   
   MdiGlPlot::mdiChildren.DeleteObject(this);
   MdiGlPlot::numChildren--;
   
   #if DEBUG_MDI_CHILD_FRAME_CLOSE
   MessageInterface::ShowMessage
      ("~MdiChildTrajFrame() exiting MdiGlPlot::numChildren=%d\n",
       MdiGlPlot::numChildren);
   #endif
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
// bool GetDrawWireFrame()
//------------------------------------------------------------------------------
bool MdiChildTrajFrame::GetDrawWireFrame()
{
   if (mCanvas)
      return mCanvas->GetDrawWireFrame();

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
// bool GetDrawSunLine()
//------------------------------------------------------------------------------
bool MdiChildTrajFrame::GetDrawSunLine()
{
   if (mCanvas)
      return mCanvas->GetDrawSunLine();

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
// UnsignedInt GetSunLineColor()
//------------------------------------------------------------------------------
UnsignedInt MdiChildTrajFrame::GetSunLineColor()
{
   if (mCanvas)
      return mCanvas->GetSunLineColor();

   return 0;
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
// Integer GetAnimationFrameIncrement()
//------------------------------------------------------------------------------
Integer MdiChildTrajFrame::GetAnimationFrameIncrement()
{
   if (mCanvas)
      return mCanvas->GetAnimationFrameIncrement();

   return 0;
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetSolarSystem(SolarSystem *ss)
{
   if (mCanvas)
      mCanvas->SetSolarSystem(ss);
}


//------------------------------------------------------------------------------
// void SetGLContext(wxGLContext *glContext = NULL)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetGLContext(wxGLContext *glContext)
{
   if (mCanvas)
      mCanvas->SetGLContext(glContext);
}


//------------------------------------------------------------------------------
// void SetUserInterrupt()
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetUserInterrupt()
{
   if (mCanvas)
      mCanvas->SetUserInterrupt();
}


//------------------------------------------------------------------------------
// void SetPlotName(const wxString &name)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetPlotName(const wxString &name)
{
   #if DEBUG_TRAJ_FRAME
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
   #if DEBUG_TRAJ_FRAME
   MessageInterface::ShowMessage
      ("MdiChildTrajFrame::ResetShowViewOption()\n");
   #endif

    theMenuBar->Check(GmatPlot::MDI_GL_SHOW_OPTION_PANEL, false);
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
      mCanvas->SetUseInitialViewDef(flag);
}


//------------------------------------------------------------------------------
// void SetUsePerspectiveMode(bool flag)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetUsePerspectiveMode(bool flag)
{
   if (mCanvas)
      mCanvas->SetUsePerspectiveMode(flag);
}


//------------------------------------------------------------------------------
// void SetAnimationUpdateInterval(nt value)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetAnimationUpdateInterval(int value)
{
   if (mCanvas)
      mCanvas->SetAnimationUpdateInterval(value);
}


//------------------------------------------------------------------------------
// void SetAnimationFrameIncrement(nt val)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetAnimationFrameIncrement(int value)
{
   if (mCanvas)
      mCanvas->SetAnimationFrameIncrement(value);
   
   if (mOptionDialog)
      mOptionDialog->SetAnimationFrameInc(value);
   
}


//------------------------------------------------------------------------------
// void SetDrawWireFrame(bool flag)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetDrawWireFrame(bool flag)
{
   if (mCanvas)
   {
      theMenuBar->Check(GmatPlot::MDI_GL_SHOW_WIRE_FRAME, flag);
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
      theMenuBar->Check(GmatPlot::MDI_GL_SHOW_EQUATORIAL_PLANE, flag);   
      mCanvas->SetDrawXyPlane(flag);
   }
}


//------------------------------------------------------------------------------
// void SetDrawEcPlane(bool flag)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetDrawEcPlane(bool flag)
{
   if (mCanvas)
      mCanvas->SetDrawEcPlane(flag);
}


//------------------------------------------------------------------------------
// void SetDrawSunLine(bool flag)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetDrawSunLine(bool flag)
{
   if (mCanvas)
      mCanvas->SetDrawSunLine(flag);
}


//------------------------------------------------------------------------------
// void SetDrawAxes(bool flag)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetDrawAxes(bool flag)
{
   if (mCanvas)
      mCanvas->SetDrawAxes(flag);
}


//------------------------------------------------------------------------------
// void SetDrawGrid(bool flag)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetDrawGrid(bool flag)
{
   if (mCanvas)
      mCanvas->SetDrawGrid(flag);
}


//------------------------------------------------------------------------------
// void SetXyPlaneColor(UnsignedInt color)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetXyPlaneColor(UnsignedInt color)
{
   if (mCanvas)
      mCanvas->SetXyPlaneColor(color);
}


//------------------------------------------------------------------------------
// void SetSunLineColor(UnsignedInt color)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetSunLineColor(UnsignedInt color)
{
   if (mCanvas)
      mCanvas->SetSunLineColor(color);
}


//------------------------------------------------------------------------------
// void SetNumPointsToRedraw(Integer numPoints)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetNumPointsToRedraw(Integer numPoints)
{
   if (mCanvas)
      mCanvas->SetNumPointsToRedraw(numPoints);
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


//------------------------------------------------------------------------------
// void DrawInOtherCoordSystem(const wxString &csName)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::DrawInOtherCoordSystem(const wxString &csName)
{
   if (mCanvas)
      mCanvas->DrawInOtherCoordSystem(csName);
}


//------------------------------------------------------------------------------
// void RedrawPlot(bool viewAnimation)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::RedrawPlot(bool viewAnimation)
{
   #ifdef DEBUG_TRAJ_FRAME
   MessageInterface::ShowMessage("MdiChildTrajFrame::RedrawPlot() entered.\n");
   #endif
   
   if (mCanvas)
      mCanvas->RedrawPlot(viewAnimation);
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
   wxString title = wxGetTextFromUser(_T("Enter the new title for MDI child"),
                                      _T(""),
                                      mPlotTitle, //s_title,
                                      GetParent()->GetParent());
   if ( !title )
      return;

   mPlotTitle = title;
   SetTitle(title);
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
// void OnShowOptionDialog(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnShowOptionDialog(wxCommandEvent& event)
{
   #if DEBUG_TRAJ_FRAME
   MessageInterface::ShowMessage
      ("MdiChildTrajFrame::OnShowOptionDialog() entered\n");
   #endif
   
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
// void OnPlotSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnPlotSize(wxSizeEvent& event)
{
   // VZ: under MSW the size event carries the client size (quite
   //     unexpectedly) *except* for the very first one which has the full
   //     size... what should it really be? TODO: check under wxGTK
   wxSize size1 = event.GetSize();
   wxSize size2 = GetSize();
   wxSize size3 = GetClientSize();

   //wxLogStatus(GmatAppData::GetMainFrame(),
   //            wxT("size from event: %dx%d, from frame %dx%d, client %dx%d"),
   //            size1.x, size1.y, size2.x, size2.y, size3.x, size3.y);
   
   event.Skip();
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
   
   //wxLogStatus(GmatAppData::GetMainFrame(),
   //            wxT("position from event: (%d, %d), from frame (%d, %d)"),
   //            pos1.x, pos1.y, pos2.x, pos2.y);
   
   event.Skip();
}


//------------------------------------------------------------------------------
// void OnPlotClose(wxCloseEvent &event)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnPlotClose(wxCloseEvent &event)
{
   #ifdef DEBUG_MDI_CHILD_FRAME_CLOSE
   MessageInterface::ShowMessage("MdiChildTrajFrame::OnPlotClose() entered\n");
   #endif
   
   CheckFrame();
   if (mCanClose)
   {
      // remove from list of frames but do not delete
      GmatAppData::GetMainFrame()->RemoveChild(GetTitle(), mItemType, false);   
      event.Skip();
   }
   else
   {
      event.Veto();
   }
   
   #ifdef DEBUG_MDI_CHILD_FRAME_CLOSE
   MessageInterface::ShowMessage("MdiChildTrajFrame::OnPlotClose() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent &event)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnClose(wxCloseEvent &event)
{
   #ifdef DEBUG_MDI_CHILD_FRAME_CLOSE
   MessageInterface::ShowMessage("MdiChildTrajFrame::OnClose() entered\n");
   #endif
   
   CheckFrame();
   
   if (mCanClose)
   {
      GmatMdiChildFrame::OnClose(event);
      event.Skip();
   }
   else
   {
      event.Veto();
   }
   
   #ifdef DEBUG_MDI_CHILD_FRAME_CLOSE
   MessageInterface::ShowMessage("MdiChildTrajFrame::OnClose() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void SetGlObject(const StringArray &objNames, ...
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetGlObject(const StringArray &objNames,
                                    const UnsignedIntArray &objOrbitColors,
                                    const std::vector<SpacePoint*> &objArray)
{
   if (mCanvas)
      mCanvas->SetGlObject(objNames, objOrbitColors, objArray);
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
      #if DEBUG_TRAJ_FRAME
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
      mCanvas->SetGlDrawOrbitFlag(drawArray);
}


//------------------------------------------------------------------------------
// void SetGlShowObjectFlag(const std::vector<bool> &showArray)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetGlShowObjectFlag(const std::vector<bool> &showArray)
{
   if (mCanvas)
      mCanvas->SetGlShowObjectFlag(showArray);
}


//------------------------------------------------------------------------------
// void SetGlUpdateFrequency(Integer updFreq)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetGlUpdateFrequency(Integer updFreq)
{
   if (mCanvas)
      mCanvas->SetUpdateFrequency(updFreq);
}


//------------------------------------------------------------------------------
// void UpdatePlot(const StringArray &scNames, ...
//------------------------------------------------------------------------------
void MdiChildTrajFrame::UpdatePlot(const StringArray &scNames, const Real &time,
                                   const RealArray &posX, const RealArray &posY,
                                   const RealArray &posZ, const RealArray &velX,
                                   const RealArray &velY, const RealArray &velZ,
                                   const UnsignedIntArray &scColors, bool solving,
                                   Integer solverOption, bool updateCanvas)
{
   if (mCanvas)
   {
      mCanvas->UpdatePlot(scNames, time, posX, posY, posZ, velX, velY, velZ,
                          scColors, solving, solverOption);
      
      if (updateCanvas)
      {
         //MessageInterface::ShowMessage
         //   ("===> MdiChildTrajFrame::UpdatePlot() time=%f\n", time);
         
         //-----------------------------------------------------------
         // Notes:
         // Added mCanvas->Refresh(false) here since Refresh() is
         // removed from the TrajPlotCanvas::UpdatePlot().
         //-----------------------------------------------------------
         mCanvas->Refresh(false);
         Update();
         
         #ifndef __WXMSW__
            ::wxYield();
         #endif
      }
   }
}


//------------------------------------------------------------------------------
// void TakeAction(const std::string &action)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::TakeAction(const std::string &action)
{
   if (mCanvas)
      mCanvas->TakeAction(action);
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
}


//------------------------------------------------------------------------------
// void EnableAnimationButton(bool enable)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::EnableAnimation(bool enable)
{
   if (mOptionDialog)
      mOptionDialog->EnableAnimation(enable);
}


//------------------------------------------------------------------------------
// void CheckFrame()
//------------------------------------------------------------------------------
void MdiChildTrajFrame::CheckFrame()
{
   #ifdef DEBUG_MDI_CHILD_FRAME_CLOSE
   MessageInterface::ShowMessage("MdiChildTrajFrame::CheckFrame() entered\n");
   #endif
   
   if (mCanvas)
   {
      mCanClose = true;

      #ifdef DEBUG_MDI_CHILD_FRAME_CLOSE
      MessageInterface::ShowMessage
         ("MdiChildTrajFrame::OnClose() IsAnimationRunning=%d\n",
          mCanvas->IsAnimationRunning());
      #endif
      
      wxSafeYield();
      
      if (mCanvas->IsAnimationRunning())
      {
         wxMessageBox(_T("The animation is running.\n"
                         "Please stop the animation first."),
                      _T("GMAT Warning"));
         mCanClose = false;
      }
   }
   
   #ifdef DEBUG_MDI_CHILD_FRAME_CLOSE
   MessageInterface::ShowMessage("MdiChildTrajFrame::CheckFrame() exiting\n");
   #endif
   
}


