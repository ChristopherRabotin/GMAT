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

#include "ColorTypes.hpp"         // for namespace GmatColor::
#include "MessageInterface.hpp"

//#define DEBUG_CHILDTRAJ_FRAME 1

BEGIN_EVENT_TABLE(MdiChildTrajFrame, wxMDIChildFrame)
   EVT_MENU(GmatPlot::MDI_GL_CHILD_QUIT, MdiChildTrajFrame::OnQuit)
   EVT_MENU(GmatPlot::MDI_GL_CHANGE_TITLE, MdiChildTrajFrame::OnChangeTitle)
   EVT_MENU(GmatPlot::MDI_GL_CLEAR_PLOT, MdiChildTrajFrame::OnClearPlot)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_DEFAULT_VIEW, MdiChildTrajFrame::OnShowDefaultView)
   EVT_MENU(GmatPlot::MDI_GL_ZOOM_IN, MdiChildTrajFrame::OnZoomIn)
   EVT_MENU(GmatPlot::MDI_GL_ZOOM_OUT, MdiChildTrajFrame::OnZoomOut)
   
   EVT_MENU(GmatPlot::MDI_GL_SHOW_OPTION_PANEL, MdiChildTrajFrame::OnShowOptionDialog)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_WIRE_FRAME, MdiChildTrajFrame::OnDrawWireFrame)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_EQUATORIAL_PLANE, MdiChildTrajFrame::OnDrawEqPlane)
   
   EVT_MENU(GmatPlot::MDI_GL_VIEW_ADD_BODY, MdiChildTrajFrame::OnAddBody)

   EVT_MENU_RANGE(GmatPlot::MDI_GL_VIEW_GOTO_SUN, GmatPlot::MDI_GL_VIEW_GOTO_MOON,
                  MdiChildTrajFrame::OnGotoStdBody)
   
   EVT_MENU(GmatPlot::MDI_GL_VIEW_GOTO_OTHER_BODY, MdiChildTrajFrame::OnGotoOtherBody)
   EVT_MENU(GmatPlot::MDI_GL_HELP_VIEW, MdiChildTrajFrame::OnHelpView)

   EVT_ACTIVATE(MdiChildTrajFrame::OnActivate)
   EVT_SIZE(MdiChildTrajFrame::OnSize)
   EVT_MOVE(MdiChildTrajFrame::OnMove)
   EVT_CLOSE(MdiChildTrajFrame::OnClose)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// MdiChildTrajFrame(wxMDIParentFrame *parent, const wxString& title, ...)
//------------------------------------------------------------------------------
MdiChildTrajFrame::MdiChildTrajFrame(wxMDIParentFrame *parent, bool isMainFrame,
                                     const wxString& plotName, const wxString& title,
                                     const wxPoint& pos, const wxSize& size,
                                     const long style, const wxString &csName,
                                     SolarSystem *solarSys)
   : wxMDIChildFrame(parent, -1, title, pos, size,
                     style | wxNO_FULL_REPAINT_ON_RESIZE)
{
   mCanvas = (TrajPlotCanvas *) NULL;
   mIsMainFrame = isMainFrame;
   mPlotName = plotName;
   mPlotTitle = plotName;

   mOverlapPlot = false;
   
   // add Sun, Earth, Luan as default body
   mBodyNames.Add("Sun");
   mBodyNames.Add("Earth");
   mBodyNames.Add("Luna");
   mBodyColors.push_back(GmatColor::YELLOW32);
   mBodyColors.push_back(GmatColor::GREEN32);
   mBodyColors.push_back(GmatColor::L_BROWN32);
   
   MdiGlPlot::mdiChildren.Append(this);

   // use this if we want option dialog to be modeless
   mOptionDialog = (OpenGlOptionDialog*)NULL;
   
   // Give it an icon
#ifdef __WXMSW__
   SetIcon(wxIcon(_T("chrt_icn")));
#else
   SetIcon(wxIcon( mondrian_xpm ));
#endif
   
   // File menu
   wxMenu *fileMenu = new wxMenu;

   fileMenu->Append(GmatPlot::MDI_GL_OPEN_TRAJECTORY_FILE, _T("&Open Trajectory File"));
   fileMenu->Append(GmatPlot::MDI_GL_QUIT, _T("&Exit"));

   // Plot menu
   wxMenu *plotMenu = new wxMenu;

   plotMenu->Append(GmatPlot::MDI_GL_CLEAR_PLOT, _T("Clear Plot"));
   plotMenu->Append(GmatPlot::MDI_GL_CHILD_QUIT, _T("&Close"), _T("Close this window"));
   plotMenu->AppendSeparator();
   plotMenu->Append(GmatPlot::MDI_GL_CHANGE_TITLE, _T("Change &title..."));

   // View menu
   mViewMenu = new wxMenu;
   mViewMenu->Append(GmatPlot::MDI_GL_SHOW_DEFAULT_VIEW, _T("Default\tCtrl-R"),
                    _("Reset to default view"));
   mViewMenu->Append(GmatPlot::MDI_GL_ZOOM_IN, _T("Zoom &in\tCtrl-I"), _("Zoom in"));
   mViewMenu->Append(GmatPlot::MDI_GL_ZOOM_OUT, _T("Zoom &out\tCtrl-O"), _("Zoom out"));
   mViewMenu->AppendSeparator();

   // View Option submenu
   mViewMenu->Append(GmatPlot::MDI_GL_SHOW_OPTION_PANEL,
                     _T("Show View Option Dialog"),
                     _T("Show view option dialog"), wxITEM_CHECK);
   
   mViewOptionMenu = new wxMenu;
   wxMenuItem *item =
      new wxMenuItem(mViewMenu, GmatPlot::MDI_GL_VIEW_OPTION, _T("Option"),
                     _T("Show bodies in wire frame"), wxITEM_NORMAL, mViewOptionMenu);
   mViewOptionMenu->Append(GmatPlot::MDI_GL_SHOW_WIRE_FRAME,
                          _T("Show Wire Frame"),
                          _T("Show bodies in wire frame"), wxITEM_CHECK);
   mViewOptionMenu->Append(GmatPlot::MDI_GL_SHOW_EQUATORIAL_PLANE,
                          _T("Show Equatorial Plane"),
                          _T("Show equatorial plane lines"), wxITEM_CHECK);
   
   mViewOptionMenu->Check(GmatPlot::MDI_GL_SHOW_EQUATORIAL_PLANE, true);
   
   mViewMenu->Append(item);
   mViewMenu->AppendSeparator();

   // View Add Body menu
   mViewMenu->Append(GmatPlot::MDI_GL_VIEW_ADD_BODY, _T("Add Body"), _T("Add bodies"));
   
   // View Goto Body menu
   mViewMenu->Append(GmatPlot::MDI_GL_VIEW_GOTO_BODY, _T("Go to Body"), CreateGotoBodyMenu());
   
   // Help menu
   wxMenu *helpMenu = new wxMenu;
   helpMenu->Append(GmatPlot::MDI_GL_HELP_VIEW, _T("View"), _T("View mouse control"));

   // menu bar
   wxMenuBar *menuBar = new wxMenuBar;

   menuBar->Append(fileMenu, _T("&File"));
   menuBar->Append(plotMenu, _T("&Plot"));
   menuBar->Append(mViewMenu, _T("&View"));
   menuBar->Append(helpMenu, _T("&Help"));

   // Associate the menu bar with the frame
   SetMenuBar(menuBar);

   // status bar
   //CreateStatusBar();
   //SetStatusText(title);

    // Create GLCanvas
   int width, height;
   GetClientSize(&width, &height);
   TrajPlotCanvas *canvas =
      new TrajPlotCanvas(this, -1, wxPoint(0, 0), wxSize(width, height),
                         csName, solarSys);
   
   mCanvas = canvas;
   
   // this should work for MDI frames as well as for normal ones
   SetSizeHints(100, 100);
}

//------------------------------------------------------------------------------
// ~MdiChildTrajFrame()
//------------------------------------------------------------------------------
MdiChildTrajFrame::~MdiChildTrajFrame()
{   
   mOptionDialog = (OpenGlOptionDialog*)NULL;
   
   MdiGlPlot::mdiChildren.DeleteObject(this);
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
// bool GetDrawEqPlane()
//------------------------------------------------------------------------------
bool MdiChildTrajFrame::GetDrawEqPlane()
{
   if (mCanvas)
      return mCanvas->GetDrawEqPlane();

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
// bool GetDrawEcLine()
//------------------------------------------------------------------------------
bool MdiChildTrajFrame::GetDrawEcLine()
{
   if (mCanvas)
      return mCanvas->GetDrawEcLine();

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
// unsigned int GetEqPlaneColor()
//------------------------------------------------------------------------------
unsigned int MdiChildTrajFrame::GetEqPlaneColor()
{
   if (mCanvas)
      return mCanvas->GetEqPlaneColor();

   return 0;
}

//------------------------------------------------------------------------------
// unsigned int GetEcPlaneColor()
//------------------------------------------------------------------------------
unsigned int MdiChildTrajFrame::GetEcPlaneColor()
{
   if (mCanvas)
      return mCanvas->GetEcPlaneColor();

   return 0;
}

//------------------------------------------------------------------------------
// unsigned int GetEcLineColor()
//------------------------------------------------------------------------------
unsigned int MdiChildTrajFrame::GetEcLineColor()
{
   if (mCanvas)
      return mCanvas->GetEcLineColor();

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
// int GetGotoBodyId()
//------------------------------------------------------------------------------
int MdiChildTrajFrame::GetGotoBodyId()
{
   if (mCanvas)
      return mCanvas->GetGotoBodyId();

   return GmatPlot::UNKNOWN_BODY;
}

//------------------------------------------------------------------------------
// wxString GetDesiredCoordSysName()
//------------------------------------------------------------------------------
wxString MdiChildTrajFrame::GetDesiredCoordSysName()
{
   if (mCanvas)
      return mCanvas->GetDesiredCoordSysName();

   return "Unknown";
}

//------------------------------------------------------------------------------
// CoordinateSystem* GetDesiredCoordSystem()
//------------------------------------------------------------------------------
CoordinateSystem* MdiChildTrajFrame::GetDesiredCoordSystem()
{
   if (mCanvas)
      return mCanvas->GetDesiredCoordSystem();

   return NULL;
}

//------------------------------------------------------------------------------
// void SetPlotName(const wxString &name)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetPlotName(const wxString &name)
{
#if DEBUG_CHILDTRAJ_FRAME
   MessageInterface::ShowMessage("MdiChildTrajFrame::SetPlotName() name=%s\n",
                                 name.c_str());
#endif
   
   mPlotName = name;
   mPlotTitle = name;
   SetTitle(mPlotTitle);
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
// void SetDrawWireFrame(bool flag)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetDrawWireFrame(bool flag)
{
   if (mCanvas)
   {
      mViewOptionMenu->Check(GmatPlot::MDI_GL_SHOW_WIRE_FRAME, flag);
      mCanvas->SetDrawWireFrame(flag);
   }
}

//------------------------------------------------------------------------------
// void SetDrawEqPlane(bool flag)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetDrawEqPlane(bool flag)
{
   if (mCanvas)
   {
      mViewOptionMenu->Check(GmatPlot::MDI_GL_SHOW_EQUATORIAL_PLANE, flag);
      mCanvas->SetDrawEqPlane(flag);
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
// void SetDrawEcLine(bool flag)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetDrawEcLine(bool flag)
{
   if (mCanvas)
   {
      //loj: the event ID is not in the GmatPlot yet
      //mViewOptionMenu->Check(GmatPlot::MDI_GL_SHOW_ECLIPTIC_LINE, flag);
      mCanvas->SetDrawEcLine(flag);
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
// void SetEqPlaneColor(UnsignedInt color)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetEqPlaneColor(UnsignedInt color)
{
   if (mCanvas)
   {
      mCanvas->SetEqPlaneColor(color);
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
// void SetEcLineColor(UnsignedInt color)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetEcLineColor(UnsignedInt color)
{
   if (mCanvas)
   {
      mCanvas->SetEcLineColor(color);
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
// void SetGotoBodyName(const wxString &bodyName)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetGotoBodyName(const wxString &bodyName)
{
   if (mCanvas)
   {
      mCanvas->GotoStdBody(GmatPlot::GetBodyId(bodyName));
   }
}

//------------------------------------------------------------------------------
// void SetDesiredCoordSystem(const wxString &csName)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetDesiredCoordSystem(const wxString &csName)
{
   if (mCanvas)
   {
      mCanvas->SetDesiredCoordSystem(csName);
   }
}

//------------------------------------------------------------------------------
// void SetDesiredCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::SetDesiredCoordSystem(CoordinateSystem *cs)
{
   if (mCanvas)
   {
      mCanvas->SetDesiredCoordSystem(cs);
   }
}

//------------------------------------------------------------------------------
// void DrawInOtherCoordSystem(const wxString &csName)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::DrawInOtherCoordSystem(const wxString &csName)
{
   if (mCanvas)
   {
      mCanvas->DrawInOtherCoordSystem(csName);
   }
}

//------------------------------------------------------------------------------
// void DrawInOtherCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::DrawInOtherCoordSystem(CoordinateSystem *cs)
{
   if (mCanvas)
   {
      mCanvas->DrawInOtherCoordSystem(cs);
   }
}

//------------------------------------------------------------------------------
// void UpdatePlot()
//------------------------------------------------------------------------------
void MdiChildTrajFrame::UpdatePlot()
{
#ifdef DEBUG_CHILDTRAJ_FRAME
   MessageInterface::ShowMessage("MdiChildTrajFrame::UpdatePlot() entered.\n");
#endif

   if (mCanvas)
      mCanvas->UpdatePlot();

   mOptionDialog->SetDistance(mCanvas->GetDistance());
}

//------------------------------------------------------------------------------
// void OnQuit(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
   Close(TRUE);
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
// void OnClearPlot(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnClearPlot(wxCommandEvent& WXUNUSED(event))
{
   if (mCanvas)
      mCanvas->ClearPlot();
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
      
      int x, y, w, h;
      MdiGlPlot::mdiParentGlFrame->GetPosition(&x, &y);
      mOptionDialog->GetSize(&w, &h);
      mOptionDialog->Move(x-w, y);
      mOptionDialog->Show(true); //modeless dialog
   }
   else
   {
      //mOptionDialog->Close();
      mOptionDialog->Hide();
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
// void OnDrawEqPlane(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnDrawEqPlane(wxCommandEvent& event)
{
   if (mCanvas)
      mCanvas->DrawEqPlane(event.IsChecked());

   if (mOptionDialog)
      mOptionDialog->SetDrawEqPlane(event.IsChecked());
}

//loj: 12/16/04 Added
//------------------------------------------------------------------------------
// void OnAddBody(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnAddBody(wxCommandEvent& event)
{
   wxArrayString emptyBodies;
   
   CelesBodySelectDialog bodyDlg(this, mBodyNames, emptyBodies);
   bodyDlg.SetBodyColors(mBodyNames, mBodyColors);
   bodyDlg.ShowModal();
   
   //--------------------------------------------------
   // update body list
   //--------------------------------------------------
   if (bodyDlg.IsBodySelected())
   {
      wxArrayString bodies = bodyDlg.GetBodyNames();
      UnsignedIntArray colors = bodyDlg.GetBodyColors();

      mBodyNames.Clear();
      mBodyColors.clear();
      
      for (unsigned int i=0; i<bodies.GetCount(); i++)
      {
         mBodyNames.Add(bodies[i]);
         mBodyColors.push_back(colors[i]);
      }

#if DEBUG_CHILDTRAJ_FRAME
      MessageInterface::ShowMessage("MdiChildTrajFrame::OnAddOtherBody()\n");
      for (unsigned int i=0; i<mBodyNames.GetCount(); i++)
      {
         MessageInterface::ShowMessage("body name=%s, color=%d\n",
                                       mBodyNames[i].c_str(), mBodyColors[i]);
      }
#endif

      //--------------------------------------------------
      // update ViewGotoBody menu item
      //--------------------------------------------------
      // remove GoTo menu item first
      mViewMenu->Remove(GmatPlot::MDI_GL_VIEW_GOTO_BODY);
   
      // add new menu items
      mViewMenu->Append(GmatPlot::MDI_GL_VIEW_GOTO_BODY, _T("GoTo"),
                        CreateGotoBodyMenu());
      
      if (mCanvas)
         mCanvas->AddBody(mBodyNames, mBodyColors);
   }
}

//------------------------------------------------------------------------------
// void OnGotoStdBody(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnGotoStdBody(wxCommandEvent& event)
{
   if (mCanvas)
   {
      int bodyId = GmatPlot::UNKNOWN_BODY;
   
      if (event.GetId() == GmatPlot::MDI_GL_VIEW_GOTO_SUN)
         bodyId = GmatPlot::SUN;
      else if (event.GetId() == GmatPlot::MDI_GL_VIEW_GOTO_MERCURY)
         bodyId = GmatPlot::MERCURY;
      else if (event.GetId() == GmatPlot::MDI_GL_VIEW_GOTO_VENUS)
         bodyId = GmatPlot::VENUS;
      else if (event.GetId() == GmatPlot::MDI_GL_VIEW_GOTO_EARTH)
         bodyId = GmatPlot::EARTH;
      else if (event.GetId() == GmatPlot::MDI_GL_VIEW_GOTO_MARS)
         bodyId = GmatPlot::MARS;
      else if (event.GetId() == GmatPlot::MDI_GL_VIEW_GOTO_JUPITER)
         bodyId = GmatPlot::JUPITER;
      else if (event.GetId() == GmatPlot::MDI_GL_VIEW_GOTO_SATURN)
         bodyId = GmatPlot::SATURN;
      else if (event.GetId() == GmatPlot::MDI_GL_VIEW_GOTO_URANUS)
         bodyId = GmatPlot::URANUS;
      else if (event.GetId() == GmatPlot::MDI_GL_VIEW_GOTO_NEPTUNE)
         bodyId = GmatPlot::NEPTUNE;
      else if (event.GetId() == GmatPlot::MDI_GL_VIEW_GOTO_PLUTO)
         bodyId = GmatPlot::PLUTO;
      else if (event.GetId() == GmatPlot::MDI_GL_VIEW_GOTO_MOON)
         bodyId = GmatPlot::MOON;

      if (bodyId != GmatPlot::UNKNOWN_BODY)
      {
         mCanvas->GotoStdBody(bodyId);
      
         // set OptionDialog gotobody ComboBox, distance
         if (mOptionDialog)
         {
            mOptionDialog->SetGotoStdBody(bodyId);
            mOptionDialog->SetDistance(mCanvas->GetDistance());
         }
      }
   }
}

//------------------------------------------------------------------------------
// void OnGotoOtherBody(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnGotoOtherBody(wxCommandEvent& event)
{
   //loj: How do I know the body name from the event?
   if (mCanvas)
   {
      mCanvas->GotoOtherBody("TESTBODY");
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
   wxPoint pos1 = event.GetPosition(),
      pos2 = GetPosition();
   wxLogStatus(MdiGlPlot::mdiParentGlFrame,
               wxT("position from event: (%d, %d), from frame (%d, %d)"),
               pos1.x, pos1.y, pos2.x, pos2.y);

   event.Skip();
}

//------------------------------------------------------------------------------
// void OnSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnSize(wxSizeEvent& event)
{
   // VZ: under MSW the size event carries the client size (quite
   //     unexpectedly) *except* for the very first one which has the full
   //     size... what should it really be? TODO: check under wxGTK
   wxSize size1 = event.GetSize(),
      size2 = GetSize(),
      size3 = GetClientSize();
   wxLogStatus(MdiGlPlot::mdiParentGlFrame,
               wxT("size from event: %dx%d, from frame %dx%d, client %dx%d"),
               size1.x, size1.y, size2.x, size2.y, size3.x, size3.y);
    
   event.Skip();
}

//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent& event)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnClose(wxCloseEvent& event)
{    
   MdiGlPlot::numChildren--;
   
   if (mIsMainFrame)
      MdiGlPlot::mdiParentGlFrame->mainSubframe = NULL;
   
   if (MdiGlPlot::numChildren == 0)
      MdiGlPlot::mdiParentGlFrame->subframe = NULL;
   
   MdiGlPlot::mdiParentGlFrame->UpdateUI();
   event.Skip();
}

//------------------------------------------------------------------------------
// void UpdateSpacecraft(const Real &time, const RealArray &posX,
//                       const RealArray &posY, const RealArray &posZ,
//                       const UnsignedIntArray &color,
//                       bool updateCanvas)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::UpdateSpacecraft(const Real &time, const RealArray &posX,
                                         const RealArray &posY, const RealArray &posZ,
                                         const UnsignedIntArray &color,
                                         bool updateCanvas)
{
   if (mCanvas)
   {
      //loj: 8/17/04 commented out so that MainFrame gets user input focus during
      // the run
      //mCanvas->SetFocus();
      
      mCanvas->UpdateSpacecraft(time, posX, posY, posZ, color);

      if (updateCanvas)
         Update();
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
   Update();
}

//------------------------------------------------------------------------------
// void DeletePlot()
//------------------------------------------------------------------------------
void MdiChildTrajFrame::DeletePlot()
{
   // This will call OnClose()
   if (mIsMainFrame)
      MdiGlPlot::mdiParentGlFrame->mainSubframe->Close();
}
//------------------------------------------------------------------------------
// wxMenu* CreateGotoBodyMenu()
//------------------------------------------------------------------------------
wxMenu* MdiChildTrajFrame::CreateGotoBodyMenu()
{
   wxMenu *menu = new wxMenu;

   for (unsigned int i=0; i<mBodyNames.GetCount(); i++)
   {
      if (GetMenuId(mBodyNames[i]) == -1)
         menu->Append(GmatPlot::MDI_GL_VIEW_GOTO_OTHER_BODY, mBodyNames[i]);
      else
         menu->Append(GetMenuId(mBodyNames[i]), mBodyNames[i]);
   }
   
   return menu;
}

//------------------------------------------------------------------------------
// int GetMenuId(const wxString &body)
//------------------------------------------------------------------------------
int MdiChildTrajFrame::GetMenuId(const wxString &body)
{
   for (unsigned int i=0; i<mBodyNames.Count(); i++)
   {
      if (body == "Sun")
         return GmatPlot::MDI_GL_VIEW_GOTO_SUN;
      else if (body == "Mercury")
         return GmatPlot::MDI_GL_VIEW_GOTO_MERCURY;
      else if (body == "Venus")
         return GmatPlot::MDI_GL_VIEW_GOTO_VENUS;
      else if (body == "Earth")
         return GmatPlot::MDI_GL_VIEW_GOTO_EARTH;
      else if (body == "Mars")
         return GmatPlot::MDI_GL_VIEW_GOTO_MARS;
      else if (body == "Jupiter")
         return GmatPlot::MDI_GL_VIEW_GOTO_JUPITER;
      else if (body == "Saturn")
         return GmatPlot::MDI_GL_VIEW_GOTO_SATURN;
      else if (body == "Uranus")
         return GmatPlot::MDI_GL_VIEW_GOTO_URANUS;
      else if (body == "Neptune")
         return GmatPlot::MDI_GL_VIEW_GOTO_NEPTUNE;
      else if (body == "Pluto")
         return GmatPlot::MDI_GL_VIEW_GOTO_PLUTO;
      else if (body == "Luna")
         return GmatPlot::MDI_GL_VIEW_GOTO_MOON;
      else
         MessageInterface::ShowMessage
            ("MdiChildTrajFrame::GetMenuId() menu for %s not supported yet\n",
             body.c_str());
   }

   return -1;
}

