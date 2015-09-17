//$Id$
//------------------------------------------------------------------------------
//                             GuiListenerManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Thomas Grubb
// Created: 2015/08/21
//
/**
 * Declares GuiListenerManager class.
 */
//------------------------------------------------------------------------------

#if !defined __CONSOLE_APP__

#include "gmatwxdefs.hpp"
#include "gmatwxrcs.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "MdiTableViewFrame.hpp"
#include "MdiGlPlotData.hpp"            // for 3D Visualization
#include "MdiTsPlotData.hpp"            // for XY plot

#endif

#include "GuiListenerManager.hpp"

//#define DEBUG_TABLE_CREATE 1
//#define DEBUG_PLOT_PERSISTENCE 1

//---------------------------------
//  static data
//---------------------------------
GuiListenerManager* GuiListenerManager::theGuiListenerManager = NULL;
int GuiListenerManager::NumListeners = 0;

//------------------------------------------------------------------------------
// GuiListenerManager* GuiListenerManager::Instance()
//------------------------------------------------------------------------------
/**
 * Method used to initialize and retrieve the GuiListenerManager singleton
 *
 * @return The singleton pointer
 */
//------------------------------------------------------------------------------
GuiListenerManager* GuiListenerManager::Instance()
{
   if (theGuiListenerManager == NULL)
      theGuiListenerManager = new GuiListenerManager();

   return theGuiListenerManager;
}

//------------------------------------------------------------------------------
//  GuiListenerManager()
//------------------------------------------------------------------------------
/**
 * Constructor used to create the singleton
 */
//------------------------------------------------------------------------------
GuiListenerManager::GuiListenerManager()
{
}


//------------------------------------------------------------------------------
//  ~GuiListenerManager()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
GuiListenerManager::~GuiListenerManager()
{
}

ISolverListener* GuiListenerManager::CreateSolverListener(const std::string &tableName,
                                         const std::string &oldName,
                                         Real positionX, Real positionY,
                                         Real width, Real height, bool isMaximized)
{
   //-------------------------------------------------------
   // check if new MDI child frame is needed
   //-------------------------------------------------------
   bool createNewFrame = true;
   wxString currPlotName;
   MdiTableViewFrame *frame = NULL;
   
   #if DEBUG_TABLE_CREATE
   MessageInterface::ShowMessage
      ("GuiListenerManager::CreateListener() Creating MdiTableViewFrame "
         "%s\n", plotName.c_str());
   #endif
      
   Integer x, y, w, h;
   bool isUsingSaved = false;
   ComputePlotPositionAndSize(positionX, positionY,
                              width, height, x, y, w, h,
                              isUsingSaved);
      
   // create a frame, containing a plot canvas
   #if DEBUG_TABLE_CREATE
   MessageInterface::ShowMessage("   Creating MdiTableViewFrame...\n");
   #endif
   frame = new MdiTableViewFrame
      (GmatAppData::Instance()->GetMainFrame(),
         tableName, "Solver Window - " + tableName,
         wxPoint(x, y), wxSize(w, h), wxDEFAULT_FRAME_STYLE);
   if (frame)
   {
      frame->SetSavedConfigFlag(isUsingSaved);
      //frame->SetIsNewFrame(true);
         
      //TGG: Start Hidden so that only currently executing solvers are shown
      //if (GmatGlobal::Instance()->GetGuiMode() == GmatGlobal::MINIMIZED_GUI)
      //   frame->Show(false);
      //else
      //   frame->Show(true);
   }
   else
      return NULL;
      
   #if __WXMAC__
      frame->SetSize(w-1, h-1);
   #else
      if (w != -1 && h != -1)
      {
         // Why not showing the plot without resetting the size?
         // Used SetSize with wxSIZE_NO_ADJUSTMENTS (LOJ: 2012.01.05)
         frame->SetSize(w-1, h-1);
         //frame->SetSize(w+1, h+1);
         frame->SetSize(x, y, w+1, h+1, wxSIZE_NO_ADJUSTMENTS);
      }
   #endif
         
   #if DEBUG_TABLE_CREATE
   MessageInterface::ShowMessage
      ("GuiListenerManager::CreateListener() frame created, frame->GetPlotName()=%s\n",
         frame->GetPlotName().c_str());
   #endif
      
   // Tile plots if TILED_PLOT mode is set from the startup file
   if (GmatGlobal::Instance()->GetPlotMode() == GmatGlobal::TILED_PLOT)
   {
      // Set wxVERTICAL or wxHORIZONTAL based on main frame width and height
      // if run mode is TESTING (LOJ: 2015.07.23)
      wxOrientation tileMode = wxVERTICAL;
      if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING)
      {
         int w, h;
         GmatAppData::Instance()->GetMainFrame()->GetActualClientSize(&w, &h, false);
         if (w < h)
            tileMode = wxHORIZONTAL;
      }
      GmatAppData::Instance()->GetMainFrame()->Tile(tileMode);
   }
   
   #if DEBUG_TABLE_CREATE
   MessageInterface::ShowMessage
      ("GuiListenerManager::CreateListener returning true, there are %d plots.\n",
       MdiGlPlot::numChildren);
   #endif
   
   GmatAppData::Instance()->GetMainFrame()->theMdiChildren->Append(frame);
   return (ISolverListener *) frame;
}


//------------------------------------------------------------------------------
// bool ComputePlotPositionAndSize(Integer &x, Integer &y, Integer &w, ...)
//------------------------------------------------------------------------------
bool GuiListenerManager::ComputePlotPositionAndSize(Real positionX,
                                                 Real positionY, Real width, Real height,
                                                 Integer &x, Integer &y, Integer &w, Integer &h,
                                                 bool usingSaved)
{
   #ifdef DEBUG_PLOT_PERSISTENCE
   MessageInterface::ShowMessage
      ("ComputePlotPositionAndSize() entered, %s, usingSaved = %d\n   positionX = %12.10f, "
       "positionY = %12.10f, width = %12.10f, height = %12.10f\n", 
       usingSaved, positionX, positionY, width, height);
   #endif
   
   if (GmatGlobal::Instance()->GetPlotMode() == GmatGlobal::TILED_PLOT)
   {
      x = -1;
      y = -1;
      w = -1;
      h = -1;
      return false;
   }
   
   Integer plotCount = NumListeners++;//MdiGlPlot::numChildren + MdiTsPlot::numChildren;
   bool isPresetSizeUsed = false;
   
   Integer screenWidth = 0;
   Integer screenHeight = 0;
   
   #ifdef __WXMAC__
      screenWidth  = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
      screenHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
   #else
      GmatMainFrame *mainFrame = GmatAppData::Instance()->GetMainFrame();
      Integer missionTreeX = 0;
      Integer missionTreeY = 0;
      Integer missionTreeW = 0;
      mainFrame->GetActualClientSize(&screenWidth, &screenHeight, true);
   #endif
   
   #ifdef DEBUG_PLOT_PERSISTENCE
   MessageInterface::ShowMessage("   screen size  : w = %4d, h = %4d\n", screenWidth, screenHeight);
   #ifndef __WXMAC__
      MessageInterface::ShowMessage("   mission tree : w = %4d\n", missionTreeW);
   #endif
   #endif
   
   // if position and size were not saved from an earlier run, figure out the initial values
   if (GmatMathUtil::IsEqual(positionX,0.0) && GmatMathUtil::IsEqual(positionY,0.0) &&
       GmatMathUtil::IsEqual(width,0.0)     && GmatMathUtil::IsEqual(height,0.0))
   {
      
      #ifdef DEBUG_PLOT_PERSISTENCE
      MessageInterface::ShowMessage("   ==> position and size are zero\n");
      #endif
      
      usingSaved = false;
      #ifdef __WXMAC__
         wxSize size = wxGetDisplaySize();
         w = (size.GetWidth() - 239) / 2;
         h = 350;
         Integer hLoc = plotCount % 2;
         Integer vLoc = (Integer)((plotCount) / 2);
         x = 238 + hLoc * w + 1;
         y = 20  + vLoc * (h+10);
      #else
         // If MissionTree is undocked, subtract its width before computing w
         if (mainFrame->IsMissionTreeUndocked(missionTreeX, missionTreeY, missionTreeW))
            screenWidth -= missionTreeW;
         
         int newCount = plotCount + 1;
         
         // compute window size depends on number of active windows
         w = (Integer)((Real)screenWidth * 0.3);
         h = (Integer)((Real)screenHeight * 0.25);
         
         // compute window position
         x = missionTreeW + 5 + (newCount-1) * 20;               
         y = (newCount - 1) * 20;
         
      #endif
   }
   else
   {
      #ifdef DEBUG_PLOT_PERSISTENCE
      MessageInterface::ShowMessage("   ==> position and size are non-zero\n");
      #endif
      
      isPresetSizeUsed = true;
      usingSaved       = true;
      
      // @note
      // Since position and size are truncated to whole integer,
      // actual plot position and size can differ a pixel or so for
      // a consecutive run after plots are closed.
      x = (Integer) (positionX * (Real) screenWidth);
      y = (Integer) (positionY * (Real) screenHeight);
      w = (Integer) (width     * (Real) screenWidth);
      h = (Integer) (height    * (Real) screenHeight);
   }
   
   #ifdef DEBUG_PLOT_PERSISTENCE
   MessageInterface::ShowMessage("   computed     : x = %4d, y = %4d, w = %4d, h = %4d\n", x, y, w, h);
   MessageInterface::ShowMessage
      ("ComputePlotPositionAndSize() returning isPresetSizeUsed = %d\n", isPresetSizeUsed);
   #endif
   
   return isPresetSizeUsed;
}


void GuiListenerManager::ClosingSolverListener()
{
   NumListeners--;
}