//$Header$
//------------------------------------------------------------------------------
//                             PlotInterface
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/12/18
//
/**
 * Implements PlotInterface class.
 * This class updates OpenGL canvas, XY plot window
 */
//------------------------------------------------------------------------------
#if !defined __CONSOLE_APP__

#include "gmatwxdefs.hpp"
#include "gmatwxrcs.hpp"
#include "GmatAppData.hpp"
#include <wx/settings.h>    // for wxSYS_SCREEN_Y
// for OpenGL plot
#include "MdiParentGlFrame.hpp"
#include "MdiGlPlotData.hpp"
#include "MdiChildTrajFrame.hpp"
// for XY plot
#include "MdiParentXyFrame.hpp"
#include "MdiXyPlotData.hpp"
#include "MdiChildXyFrame.hpp"

#endif

#include <iostream>                // for cout, endl
#include "PlotInterface.hpp"       // for PlotInterface functions
#include "Rvector.hpp"
#include "MessageInterface.hpp"    // for ShowMessage()

//#define DEBUG_PLOTIF_GL 1
//#define DEBUG_PLOTIF_XY 1
//#define DEBUG_PLOTIF_XY_UPDATE 1
//#define DEBUG_RENAME 1

//---------------------------------
//  static data
//---------------------------------

//---------------------------------
//  public functions
//---------------------------------

//------------------------------------------------------------------------------
//  PlotInterface()
//------------------------------------------------------------------------------
PlotInterface::PlotInterface()
{
}

//------------------------------------------------------------------------------
//  ~PlotInterface()
//------------------------------------------------------------------------------
PlotInterface::~PlotInterface()
{
}

//------------------------------------------------------------------------------
//  bool CreateGlPlotWindow(const std::string &plotName, const std::string &oldName,
//                          const std::string &csName, bool drawWireFrame = flase,
//                          bool overlapPlot = false, SolarSystem *solarSystem = NULL)
//------------------------------------------------------------------------------
/*
 * Creates OpenGlPlot window
 *
 * @param <plotName> plot name
 */
//------------------------------------------------------------------------------
bool PlotInterface::CreateGlPlotWindow(const std::string &plotName,
                                       const std::string &oldName,
                                       const std::string &csName,
                                       bool drawWireFrame, bool overlapPlot,
                                       SolarSystem *solarSystem)
{    
#if defined __CONSOLE_APP__
   return true;
#else

   //-------------------------------------------------------
   // create MDI parent frame if NULL
   //-------------------------------------------------------
   if (MdiGlPlot::mdiParentGlFrame == NULL)
   {
      #if DEBUG_PLOTIF_GL
      MessageInterface::ShowMessage("PlotInterface::CreateGlPlotWindow() "
                                    "Creating MdiGlPlot::mdiParentGlFrame\n");
      #endif
      
      int screenX = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
      int screenY = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
      
      MdiGlPlot::mdiParentGlFrame =
         new MdiParentGlFrame((wxFrame *)NULL, -1, _T("MDI OpenGL Window"),
                              wxPoint(screenX-560, screenY-515), wxSize(550, 480),
                              wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);
      // Give it an icon
      #ifdef __WXMSW__
         MdiGlPlot::mdiParentGlFrame->SetIcon(wxIcon(_T("mdi_icn")));
      #else
         MdiGlPlot::mdiParentGlFrame->SetIcon(wxIcon( mondrian_xpm ));
      #endif
   }
   
   //-------------------------------------------------------
   // check if new MDI child frame is needed
   //-------------------------------------------------------
   bool createNewFrame = true;
   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      wxString currPlotName =
         ((MdiChildTrajFrame*)MdiGlPlot::mdiChildren[i])->GetPlotName();

      if (currPlotName.IsSameAs(plotName.c_str()))
      {
         createNewFrame = false;
         break;
      }
      else if (currPlotName.IsSameAs(oldName.c_str()))
      {
         // change plot name (loj: 11/19/04 - added)
         ((MdiChildTrajFrame*)MdiGlPlot::mdiChildren[i])->
            SetPlotName(wxString(plotName.c_str()));
         createNewFrame = false;
         break;
      }
   }
   
   //-------------------------------------------------------
   // create MDI child frame if not exist
   //-------------------------------------------------------
   if (createNewFrame)
   {
      #if DEBUG_PLOTIF_GL
      MessageInterface::ShowMessage("PlotInterface::CreateGlPlotWindow() "
                                    "Creating MdiChildTrajFrame\n");
      #endif
      
      MdiGlPlot::mdiParentGlFrame->mainSubframe =
         new MdiChildTrajFrame(MdiGlPlot::mdiParentGlFrame, true,
                               wxString(plotName.c_str()),
                               wxString(plotName.c_str()),
                               wxPoint(-1, -1), wxSize(-1, -1),
                               wxDEFAULT_FRAME_STYLE, wxString(csName.c_str()),
                               solarSystem);
      
      ++MdiGlPlot::numChildren;

      //----------------------------------------------------
      // initialize GL
      //----------------------------------------------------
      if (!MdiGlPlot::mdiParentGlFrame->mainSubframe->mCanvas->InitGL())
      {
         wxMessageDialog msgDialog(MdiGlPlot::mdiParentGlFrame,
                                   _T("InitGL() failed"), _T("CreateGlPlotWindow"));
         msgDialog.ShowModal();
         return false;
      }
   }
   
   MdiGlPlot::mdiParentGlFrame->mainSubframe->SetDrawWireFrame(drawWireFrame);
   MdiGlPlot::mdiParentGlFrame->mainSubframe->SetOverlapPlot(overlapPlot);
   MdiGlPlot::mdiParentGlFrame->mainSubframe->
      SetDesiredCoordSystem(wxString(csName.c_str()));
   
   MdiGlPlot::mdiParentGlFrame->Show(true);
   MdiGlPlot::mdiParentGlFrame->UpdateUI();

   return true;
#endif
}

//------------------------------------------------------------------------------
//  delete DeleteGlPlot()
//------------------------------------------------------------------------------
/*
 * Deletes OpenGlPlot
 */
//------------------------------------------------------------------------------
bool PlotInterface::DeleteGlPlot()
{    
#if defined __CONSOLE_APP__
   return true;
#else

   if (MdiGlPlot::mdiParentGlFrame != NULL)
   {
#if DEBUG_PLOTIF_GL
      MessageInterface::ShowMessage("PlotInterface::DeleteGlPlot() "
                                    "Deleting MdiGlPlot::mdiChildren[]\n");
#endif
      for (int i=0; i<MdiGlPlot::numChildren; i++)
      {
         ((MdiChildTrajFrame*)MdiGlPlot::mdiChildren[i])->DeletePlot();
      }

      MdiGlPlot::mdiParentGlFrame->Hide();
   }

   return true;
#endif
}

//------------------------------------------------------------------------------
//  delete RefreshGlPlot(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Refreshes OpenGlPlot.
 */
//------------------------------------------------------------------------------
bool PlotInterface::RefreshGlPlot(const std::string &plotName)
{    
#if defined __CONSOLE_APP__
   return true;
#else

   if (MdiGlPlot::mdiParentGlFrame != NULL)
   {
#if DEBUG_PLOTIF_GL
      MessageInterface::ShowMessage
         ("PlotInterface::RefreshGlPlot() plotName=%s\n",plotName.c_str());
#endif
      wxString owner = wxString(plotName.c_str());
      
      for (int i=0; i<MdiGlPlot::numChildren; i++)
      {
         MdiChildTrajFrame *frame =
            (MdiChildTrajFrame*)(MdiGlPlot::mdiChildren[i]);
            
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            frame->RefreshPlot();
         }
      }
   }
   
   return true;
#endif
}

//------------------------------------------------------------------------------
//  bool UpdateGlSpacecraft(const std::string &plotName,
//                          const std::string &oldName,
//                          const std::string &csName,
//                          const Real &time, const RealArray &posX,
//                          const RealArray &posY, const RealArray &posZ,
//                          const UnsignedIntArray &orbitColor,
//                          const UnsignedIntArray &targetColor,
//                          bool updateCanvas, bool drawWireFrame = false)
//------------------------------------------------------------------------------
/*
 * Buffers data and updates OpenGL plow window if updateCanvas is true
 */
//------------------------------------------------------------------------------
bool PlotInterface::UpdateGlSpacecraft(const std::string &plotName,
                                       const std::string &oldName,
                                       const std::string &csName,
                                       const Real &time, const RealArray &posX,
                                       const RealArray &posY, const RealArray &posZ,
                                       const UnsignedIntArray &color,
                                       bool updateCanvas, bool drawWireFrame)
{   
#if defined __CONSOLE_APP__
   return true;
#else

   bool updated = false;
   wxString owner = wxString(plotName.c_str());

   if (MdiGlPlot::mdiParentGlFrame == NULL)
   {
      if (!CreateGlPlotWindow(plotName, oldName, csName, drawWireFrame))
         return false;
   }
   
   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      MdiChildTrajFrame *frame = (MdiChildTrajFrame*)(MdiGlPlot::mdiChildren[i]);
            
      if (frame->GetPlotName().IsSameAs(owner.c_str()))
      {
         frame->UpdateSpacecraft(time, posX, posY, posZ, color, updateCanvas);
         updated = true;
      }
   }

   return updated;
   
#endif
} // end UpdateGlSpacecraft()


//------------------------------------------------------------------------------
//  bool CreateXyPlotWindow(const std::string &plotName,
//                          const std::string &oldName,
//                          const std::string &plotTitle,
//                          const std::string &xAxisTitle,
//                          const std::string &yAxisTitle,
//                          bool drawGrid = false)
//------------------------------------------------------------------------------
/*
 * Creates XyPlot window.
 *
 * @param <plotName> name of plot
 */
//------------------------------------------------------------------------------
bool PlotInterface::CreateXyPlotWindow(const std::string &plotName,
                                       const std::string &oldName,
                                       const std::string &plotTitle,
                                       const std::string &xAxisTitle,
                                       const std::string &yAxisTitle,
                                       bool drawGrid)
{    
#if defined __CONSOLE_APP__
   return true;
#else

   //-------------------------------------------------------
   // create MDI parent XY frame
   //-------------------------------------------------------
   if (MdiXyPlot::mdiParentXyFrame == NULL)
   {
#if DEBUG_PLOTIF_XY
      MessageInterface::ShowMessage("PlotInterface::CreateXyPlotWindow() "
                                    "Creating MdiXyPlot::mdiParentXyFrame\n");
#endif
      int screenX = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
      
      MdiXyPlot::mdiParentXyFrame =
         new MdiParentXyFrame((wxFrame *)NULL, -1, _T("MDI XY Plot Window"),
                              //wxPoint(600, 10), wxSize(600, 500),
                              wxPoint(screenX-560, 15), wxSize(550, 480),
                              wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);
      // Give it an icon
#ifdef __WXMSW__
      MdiXyPlot::mdiParentXyFrame->SetIcon(wxIcon(_T("mdi_icn")));
#else
      MdiXyPlot::mdiParentXyFrame->SetIcon(wxIcon( mondrian_xpm ));
#endif
   }

   //-------------------------------------------------------
   // check if new MDI child frame needed
   //-------------------------------------------------------
   bool createNewFrame = true;
   for (int i=0; i<MdiXyPlot::numChildren; i++)
   {
      wxString currPlotName =
         ((MdiChildXyFrame*)MdiXyPlot::mdiChildren[i])->GetPlotName();

      if (currPlotName.IsSameAs(plotName.c_str()))
      {
         createNewFrame = false;
         break;
      }
      else if (currPlotName.IsSameAs(oldName.c_str()))
      {
#if DEBUG_RENAME
         MessageInterface::ShowMessage
            ("PlotInterface::CreateXyPlotWindow() currPlotName=%s, oldName=%s\n",
             currPlotName.c_str(), oldName.c_str());
#endif
         // change plot name (loj: 11/19/04 - added)
         ((MdiChildXyFrame*)MdiXyPlot::mdiChildren[i])->
            SetPlotName(wxString(plotName.c_str()));
         createNewFrame = false;
         break;
      }
   }
   
   //-------------------------------------------------------
   // create MDI child XY frame
   //-------------------------------------------------------
   if (createNewFrame)
   {
        
#if DEBUG_PLOTIF_XY
      MessageInterface::ShowMessage
         ("PlotInterface::CreateXyPlotWindow() Creating"
          "MdiXyPlot::mdiParentXyFrame->mainSubframe\n"
          "X Axis Title = %s  Y Axis Title = %s\n",
          xAxisTitle.c_str(), yAxisTitle.c_str());
#endif
      
      // create a frame, containing a XY plot canvas
      MdiXyPlot::mdiParentXyFrame->mainSubframe =
         new MdiChildXyFrame(MdiXyPlot::mdiParentXyFrame, true,
                             wxString(plotName.c_str()),
                             wxString(plotTitle.c_str()),
                             wxString(xAxisTitle.c_str()),
                             wxString(yAxisTitle.c_str()),
                             wxPoint(-1, -1), wxSize(500, 350), //loj: 2/27/04 wxSize(-1, -1),
                             wxDEFAULT_FRAME_STYLE);
    
      ++MdiXyPlot::numChildren;

      MdiXyPlot::mdiParentXyFrame->mainSubframe->RedrawCurve();
   }

   MdiXyPlot::mdiParentXyFrame->mainSubframe->SetShowGrid(drawGrid); //loj: 7/20/04 added
   MdiXyPlot::mdiParentXyFrame->mainSubframe->ResetZoom(); //loj: 7/27/04 added
   MdiXyPlot::mdiParentXyFrame->Show(true);
   MdiXyPlot::mdiParentXyFrame->Raise();
   
   return true;
#endif
}

//------------------------------------------------------------------------------
//  bool DeleteXyPlot(bool hideFrame)
//------------------------------------------------------------------------------
/*
 * Deletes XyPlot.
 *
 */
//------------------------------------------------------------------------------
bool PlotInterface::DeleteXyPlot(bool hideFrame)
{    
#if defined __CONSOLE_APP__
   return true;
#else

   if (MdiXyPlot::mdiParentXyFrame != NULL)
   {
#if DEBUG_PLOTIF_XY
      MessageInterface::ShowMessage("PlotInterface::DeleteXyPlot()\n");
#endif
      
      for (int i=0; i<MdiXyPlot::numChildren; i++)
      {
         ((MdiChildXyFrame*)MdiXyPlot::mdiChildren[i])->DeletePlot();
      }

      if (hideFrame)
         MdiXyPlot::mdiParentXyFrame->Hide();
   }
    
   return true;
#endif
}

//------------------------------------------------------------------------------
// bool AddXyPlotCurve(const std::string &plotName, int curveIndex,
//                     int yOffset, Real yMin, Real yMax,
//                     const std::string &curveTitle,
//                     UnsignedInt penColor)
//------------------------------------------------------------------------------
/*
 * Adds a plot curve to XY plow window.
 */
//------------------------------------------------------------------------------
bool PlotInterface::AddXyPlotCurve(const std::string &plotName, int curveIndex,
                                   int yOffset, Real yMin, Real yMax,
                                   const std::string &curveTitle,
                                   UnsignedInt penColor)
{
#if defined __CONSOLE_APP__
   return true;
#else

   bool added = false;
   
#if DEBUG_PLOTIF_XY
   MessageInterface::ShowMessage
      ("PlotInterface::AddXyPlotCurve() entered."
       " plotName = " + plotName + " curveTitle = " + 
       curveTitle + "\n");
    
   MessageInterface::ShowMessage
      ("PlotInterface::AddXyPlotCurve() numChildren = %d\n",
       MdiXyPlot::numChildren);
#endif
   
   for (int i=0; i<MdiXyPlot::numChildren; i++)
   {
      MdiChildXyFrame *frame = (MdiChildXyFrame*)(MdiXyPlot::mdiChildren[i]);
        
      //if (frame->GetPlotName() == wxString(plotName.c_str()))
      if (frame->GetPlotName().IsSameAs(plotName.c_str()))
      {
         frame->AddPlotCurve(curveIndex, yOffset, yMin, yMax,
                             wxString(curveTitle.c_str()), penColor);
         added = true;
      }
   }

   return added;
#endif
}

//------------------------------------------------------------------------------
// bool DeleteAllXyPlotCurves(const std::string &plotName,
//                            const std::string &oldName)
//------------------------------------------------------------------------------
/*
 * Deletes all plot curves in XY plow window.
 */
//------------------------------------------------------------------------------
bool PlotInterface::DeleteAllXyPlotCurves(const std::string &plotName,
                                          const std::string &oldName)
{
#if defined __CONSOLE_APP__
   return true;
#else
   
#if DEBUG_PLOTIF_XY
   MessageInterface::ShowMessage
      ("PlotInterface::DeleteAllPlotCurve() plotName = %s "
       "numChildren = %d\n", plotName.c_str(),
       MdiXyPlot::numChildren);
#endif
   
   for (int i=0; i<MdiXyPlot::numChildren; i++)
   {
      MdiChildXyFrame *frame = (MdiChildXyFrame*)(MdiXyPlot::mdiChildren[i]);
      if (frame->GetPlotName().IsSameAs(plotName.c_str()) ||
          frame->GetPlotName().IsSameAs(oldName.c_str()))
      {
         frame->DeleteAllPlotCurves();
      }
   }

   return true;
#endif
}

//------------------------------------------------------------------------------
// bool DeleteXyPlotCurve(const std::string &plotName, int curveIndex)
//------------------------------------------------------------------------------
/*
 * Deletes a plot curve to XY plow window.
 */
//------------------------------------------------------------------------------
bool PlotInterface::DeleteXyPlotCurve(const std::string &plotName, int curveIndex)
{
#if defined __CONSOLE_APP__
   return true;
#else
   
#if DEBUG_PLOTIF_XY
   MessageInterface::ShowMessage
      ("PlotInterface::DeleteXyPlotCurve() entered plotName = %s "
       "curveIndex = %d\n", plotName.c_str(), curveIndex);
    
   MessageInterface::ShowMessage
      ("PlotInterface::DeleteXyPlotCurve() numChildren = %d\n",
       MdiXyPlot::numChildren);
#endif
   
   for (int i=0; i<MdiXyPlot::numChildren; i++)
   {
      MdiChildXyFrame *frame = (MdiChildXyFrame*)(MdiXyPlot::mdiChildren[i]);

      if (frame->GetPlotName().IsSameAs(plotName.c_str()))
      {
         frame->DeletePlotCurve(curveIndex);
      }
   }

   return true;
#endif
}

//------------------------------------------------------------------------------
// void ClearXyPlotData(const std::string &plotName))
//------------------------------------------------------------------------------
void PlotInterface::ClearXyPlotData(const std::string &plotName)
{
#if defined __CONSOLE_APP__
   return;
#else

   for (int i=0; i<MdiXyPlot::numChildren; i++)
   {
      MdiChildXyFrame *frame = (MdiChildXyFrame*)(MdiXyPlot::mdiChildren[i]);

      if (frame->GetPlotName().IsSameAs(plotName.c_str()))
      {
         frame->ClearPlotData();
      }
   }
#endif
}

//------------------------------------------------------------------------------
// void SetXyPlotTitle(const std::string &plotName, const std::string &plotTitle)
//------------------------------------------------------------------------------
void PlotInterface::SetXyPlotTitle(const std::string &plotName,
                                   const std::string &plotTitle)
{
#if defined __CONSOLE_APP__
   return;
#else
   
#if DEBUG_PLOTIF_XY
   MessageInterface::ShowMessage
      ("PlotInterface::SetXyPlotTitle() plotName = %s "
       "plotTitle = %s\n", plotName.c_str(), plotTitle.c_str());
#endif
   
   for (int i=0; i<MdiXyPlot::numChildren; i++)
   {
      MdiChildXyFrame *frame = (MdiChildXyFrame*)(MdiXyPlot::mdiChildren[i]);

      if (frame->GetPlotName().IsSameAs(plotName.c_str()))
      {
#if DEBUG_PLOTIF_XY
         MessageInterface::ShowMessage
            ("PlotInterface::SetXyPlotTitle() calling "
             " frame->SetPlotTitle() \n");
#endif
         frame->SetPlotTitle(wxString(plotTitle.c_str()));
      }
   }
#endif
}

//------------------------------------------------------------------------------
// void ShowXyPlotLegend(const std::string &plotName)
//------------------------------------------------------------------------------
void PlotInterface::ShowXyPlotLegend(const std::string &plotName)
{
#if defined __CONSOLE_APP__
   return;
#else
      
   for (int i=0; i<MdiXyPlot::numChildren; i++)
   {
      MdiChildXyFrame *frame = (MdiChildXyFrame*)(MdiXyPlot::mdiChildren[i]);

      if (frame->GetPlotName().IsSameAs(plotName.c_str()))
      {
#if DEBUG_PLOTIF_XY
         MessageInterface::ShowMessage
            ("PlotInterface::ShowXyPlotLegend() calling  frame->ShowPlotLegend() \n");
#endif
         frame->ShowPlotLegend();
      }
   }
#endif
}

//------------------------------------------------------------------------------
// bool RefreshXyPlot(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Refreshes XY plot curve.
 *
 * @param <plotName> name of xy plot
 */
//------------------------------------------------------------------------------
bool PlotInterface::RefreshXyPlot(const std::string &plotName)
{
#if defined __CONSOLE_APP__
   return true;
#else

   if (MdiXyPlot::mdiParentXyFrame != NULL)
   {        
#if DEBUG_PLOTIF_XY_UPDATE
      MessageInterface::ShowMessage
         ("PlotInterface::RefreshXyPlot() plotName=%s, numChildren=%d\n",
          plotName.c_str(), MdiXyPlot::numChildren);
#endif
      
      wxString owner = wxString(plotName.c_str());
      
      for (int i=0; i<MdiXyPlot::numChildren; i++)
      {
         MdiChildXyFrame *frame =
            (MdiChildXyFrame*)(MdiXyPlot::mdiChildren[i]);
      
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            frame->RedrawCurve();
            //loj: 8/17/04 commented out so that MainFrame gets user input focus
            // during the run
            //frame->SetFocus();
         }
      }
   }
   
   return true;
#endif
}

//------------------------------------------------------------------------------
// bool UpdateXyPlot(const std::string &plotName, const std::string &oldName,
//                   const Real &xval, const Rvector &yvals,
//                   const std::string &plotTitle,
//                   const std::string &xAxisTitle,
//                   const std::string &yAxisTitle, bool updateCanvas,
//                   bool drawGrid)
//------------------------------------------------------------------------------
/*
 * Updates XY plot curve.
 *
 * @param <plotName> name of xy plot
 * @param <xval> x value
 * @param <yvals> y values, should be in the order of curve added
 */
//------------------------------------------------------------------------------
bool PlotInterface::UpdateXyPlot(const std::string &plotName,
                                 const std::string &oldName,
                                 const Real &xval, const Rvector &yvals,
                                 const std::string &plotTitle,
                                 const std::string &xAxisTitle,
                                 const std::string &yAxisTitle,
                                 bool updateCanvas, bool drawGrid)
{
#if defined __CONSOLE_APP__
   return true;
#else

   bool updated = false;
   wxString owner = wxString(plotName.c_str());
    
   if (MdiXyPlot::mdiParentXyFrame == NULL)
   {        
      //wxLogWarning("MdiParentXyFrame was not created. "
      //             "Creating a new MDI parent/child frame...");
      //wxLog::FlushActive();
      CreateXyPlotWindow(plotName, oldName, plotTitle, xAxisTitle, yAxisTitle,
                         drawGrid);
#if DEBUG_PLOTIF_XY_UPDATE
      MessageInterface::ShowMessage
         ("PlotInterface::UpdateXyPlot()" + plotName + " " +
          plotTitle + " " + xAxisTitle + " " + yAxisTitle + "\n");
#endif
   }
        
#if DEBUG_PLOTIF_XY_UPDATE
   MessageInterface::ShowMessage
      ("PlotInterface::UpdateXyPlot() numChildren = %d\n",
       MdiXyPlot::numChildren);
#endif
   
   for (int i=0; i<MdiXyPlot::numChildren; i++)
   {
      MdiChildXyFrame *frame = (MdiChildXyFrame*)(MdiXyPlot::mdiChildren[i]);
      
      if (frame->GetPlotName().IsSameAs(owner.c_str()))
      {
         int numCurves = frame->GetCurveCount();
            
#if DEBUG_PLOTIF_XY_UPDATE
         MessageInterface::ShowMessage
            ("PlotInterface::UpdateXyPlot() numCurves = %d\n",
             numCurves);
#endif            
         for (int j=0; j<numCurves; j++)
         {
#if DEBUG_PLOTIF_XY_UPDATE
            MessageInterface::ShowMessage
               ("PlotInterface::UpdateXyPlot() yvals[%d] = %f\n",
                j, yvals(j));
#endif
            frame->AddDataPoints(j, xval, yvals(j));
         }

         if (updateCanvas)
            frame->RedrawCurve();
            
         updated = true;
      }
   }
   
   return updated;
#endif
}
