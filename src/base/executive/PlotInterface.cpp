//$Id$
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
#include "MdiGlPlotData.hpp"       // for OpenGL plot
#include "MdiChildTrajFrame.hpp"   // for OpenGL plot
#include "MdiTsPlotData.hpp"       // for XY plot
#include "MdiChildTsFrame.hpp"     // for XY plot

#endif

#include "PlotInterface.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PLOTIF_GL 1
//#define DEBUG_PLOTIF_GL_CREATE 1
//#define DEBUG_PLOTIF_GL_DELETE 1
//#define DEBUG_PLOTIF_GL_UPDATE 1
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
//  bool CreateGlPlotWindow(const std::string &plotName, ...)
//------------------------------------------------------------------------------
/*
 * Creates OpenGlPlot window
 *
 * @param <plotName> plot name
 * @param <oldName>  old plot name, this is needed for renaming plot
 * @param <drawEcPlane>  true if draw ecliptic plane
 * @param <drawXyPlane>  true if draw XY plane
 * @param <drawWirePlane>  true if draw wire frame
 * @param <drawAxes>  true if draw axes
 * @param <drawGrid>  true if draw grid
 * @param <drawSunLine>  true if draw earth sun lines
 * @param <overlapPlot>  true if overlap plot without clearing the plot
 * @param <usevpInfo>  true if use viewpoint info to draw plot
 * @param <usepm>  true if use perspective projection mode
 * @param <numPtsToRedraw>  number of points to redraw during the run
 */
//------------------------------------------------------------------------------
bool PlotInterface::CreateGlPlotWindow(const std::string &plotName,
                                       const std::string &oldName,
                                       bool drawEcPlane, bool drawXyPlane,
                                       bool drawWireFrame, bool drawAxes,
                                       bool drawGrid, bool drawSunLine,
                                       bool overlapPlot, bool usevpInfo, bool usepm,
                                       Integer numPtsToRedraw)
{    
#if defined __CONSOLE_APP__
   return true;
#else

   //-------------------------------------------------------
   // check if new MDI child frame is needed
   //-------------------------------------------------------
   bool createNewFrame = true;
   wxString currPlotName;
   MdiChildTrajFrame *frame = NULL;
   
   #if DEBUG_PLOTIF_GL_CREATE
   MessageInterface::ShowMessage
      ("PlotInterface::CreateGlPlotWindow() MdiGlPlot::numChildren=%d, "
       "plotName=%s\n   oldName=%s\n", MdiGlPlot::numChildren,
       plotName.c_str(), oldName.c_str());
   #endif
   
   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildTrajFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());
      
      if (frame)
         currPlotName = frame->GetPlotName();
      else
         break;
      
      #if DEBUG_PLOTIF_GL_CREATE
      MessageInterface::ShowMessage
         ("PlotInterface::CreateGlPlotWindow() currPlotName[%d]=%s, addr=%p\n",
          i, currPlotName.c_str(), frame);
      #endif
      
      if (currPlotName.IsSameAs(plotName.c_str()))
      {
         createNewFrame = false;
         break;
      }
      else if (currPlotName.IsSameAs(oldName.c_str()))
      {
         // change plot name
         frame->SetPlotName(wxString(plotName.c_str()));
         createNewFrame = false;
         break;
      }
   }
   
   //-------------------------------------------------------
   // create MDI child frame if not exist
   //-------------------------------------------------------
   if (createNewFrame)
   {
      #if DEBUG_PLOTIF_GL_CREATE
      MessageInterface::ShowMessage
         ("PlotInterface::CreateGlPlotWindow() Creating MdiChildTrajFrame "
          "%s\n", plotName.c_str());
      #endif
      
      frame =
         new MdiChildTrajFrame(GmatAppData::Instance()->GetMainFrame(), 
                               wxString(plotName.c_str()),
                               wxString(plotName.c_str()),
                               wxPoint(-1, -1), wxSize(-1, -1),
                               wxDEFAULT_FRAME_STYLE);
      
      if (frame)
         frame->Show();
      else
         return false;
      
      #if __WXMAC__  
         frame->SetSize(499, 349);
      #endif
      
      #if DEBUG_PLOTIF_GL_CREATE
      MessageInterface::ShowMessage
         ("PlotInterface::CreateGlPlotWindow() frame->GetPlotName()=%s\n",
          frame->GetPlotName().c_str());
      #endif
      
      GmatAppData::Instance()->GetMainFrame()->Tile();
      
      ++MdiGlPlot::numChildren;
   }
   else
   {
      #if DEBUG_PLOTIF_GL_CREATE
      MessageInterface::ShowMessage
         ("PlotInterface::CreateGlPlotWindow() PlotName:%s already exist.\n",
          plotName.c_str());
      #endif
   }
   
   #if DEBUG_PLOTIF_GL_CREATE
   MessageInterface::ShowMessage
      ("PlotInterface::CreateGlPlotWindow() setting view options for %s\n",
       frame->GetPlotName().c_str());
   #endif
   
   frame->SetDrawXyPlane(drawXyPlane);
   frame->SetDrawEcPlane(drawEcPlane);
   frame->SetDrawWireFrame(drawWireFrame);
   frame->SetDrawAxes(drawAxes);
   frame->SetDrawGrid(drawGrid);
   frame->SetDrawSunLine(drawSunLine);
   
   frame->SetOverlapPlot(overlapPlot);
   frame->SetUseInitialViewDef(usevpInfo);
   frame->SetUsePerspectiveMode(usepm);
   frame->SetNumPointsToRedraw(numPtsToRedraw);
   
   #if DEBUG_PLOTIF_GL_CREATE
   MessageInterface::ShowMessage
      ("PlotInterface::CreateGlPlotWindow() returning true\n");
   #endif
   
   return true;
#endif
} //CreateGlPlotWindow()


//------------------------------------------------------------------------------
// void SetGlSolarSystem(const std::string &plotName, SolarSystem *ss)
//------------------------------------------------------------------------------
void PlotInterface::SetGlSolarSystem(const std::string &plotName, SolarSystem *ss)
{
#if defined __CONSOLE_APP__
   return;
#else
   
   #if DEBUG_PLOTIF_GL
   MessageInterface::ShowMessage
      ("PlotInterface::SetGlSolarSystem() SolarSystem=%p\n", ss);
   #endif
   
   wxString owner = wxString(plotName.c_str());
   
   MdiChildTrajFrame *frame = NULL;
   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildTrajFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());
      
      if (frame->GetPlotName().IsSameAs(owner.c_str()))
      {            
         frame->SetSolarSystem(ss);
      }
   }
   
#endif
}


//------------------------------------------------------------------------------
// void SetGlObject(const std::string &plotName,  ...
//------------------------------------------------------------------------------
void PlotInterface::SetGlObject(const std::string &plotName,
                                const StringArray &objNames,
                                const UnsignedIntArray &objOrbitColors,
                                const std::vector<SpacePoint*> &objArray)
{
#if defined __CONSOLE_APP__
   return;
#else
   
   #if DEBUG_PLOTIF_GL
   MessageInterface::ShowMessage
      ("PlotInterface::SetGlObject() plotName:%s\n", plotName.c_str());
   #endif
   
   wxString owner = wxString(plotName.c_str());
   
   MdiChildTrajFrame *frame = NULL;
   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildTrajFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());
      
      if (frame->GetPlotName().IsSameAs(owner.c_str()))
      {            
         frame->SetGlObject(objNames, objOrbitColors, objArray);
      }
   }
   
#endif
}


//------------------------------------------------------------------------------
// static void SetGlCoordSystem(const std::string &plotName, ...
//------------------------------------------------------------------------------
void PlotInterface::SetGlCoordSystem(const std::string &plotName,
                                     CoordinateSystem *viewCs,
                                     CoordinateSystem *viewUpCs)
{
#if defined __CONSOLE_APP__
   return;
#else

   #if DEBUG_PLOTIF_GL
   MessageInterface::ShowMessage
      ("PlotInterface::SetGlCoordSystem() plotName:%s\n", plotName.c_str());
   #endif
   
   wxString owner = wxString(plotName.c_str());
   MdiChildTrajFrame *frame = NULL;
   
   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildTrajFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());
      
      if (frame->GetPlotName().IsSameAs(owner.c_str()))
      {
         frame->SetGlCoordSystem(viewCs, viewUpCs);
      }
   }
#endif
   
}


//------------------------------------------------------------------------------
// void SetGlViewOption(const std::string &plotName, SpacePoint *vpRefObj, ...
//------------------------------------------------------------------------------
void PlotInterface::SetGlViewOption(const std::string &plotName,
                                    SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                                    SpacePoint *vdObj, Real vsFactor,
                                    const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                                    const Rvector3 &vdVec, const std::string &upAxis,
                                    bool usevpRefVec, bool usevpVec, bool usevdVec,
                                     bool useFixedFov, Real fov)
{
#if defined __CONSOLE_APP__
   return;
#else
   
   #if DEBUG_PLOTIF_GL
   MessageInterface::ShowMessage
      ("PlotInterface::SetGlViewOption() plotName:%s\n", plotName.c_str());
   #endif
   
   wxString owner = wxString(plotName.c_str());
   MdiChildTrajFrame *frame = NULL;
   
   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildTrajFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());
      
      if (frame->GetPlotName().IsSameAs(owner.c_str()))
      {
         #if DEBUG_PLOTIF_GL
         MessageInterface::ShowMessage
            ("PlotInterface::SetGlViewOption() vpRefObj=%d, vsFactor=%f\n",
             vpRefObj, vsFactor);
         #endif
         
         frame->SetGlViewOption(vpRefObj, vpVecObj, vdObj, vsFactor, vpRefVec,
                                vpVec, vdVec, upAxis, usevpRefVec,usevpVec,
                                usevdVec, useFixedFov, fov);
      }
   }
#endif
}


//------------------------------------------------------------------------------
// void SetGlDrawOrbitFlag(const std::string &plotName, ...
//------------------------------------------------------------------------------
void PlotInterface::SetGlDrawOrbitFlag(const std::string &plotName,
                                       const std::vector<bool> &drawArray)
{
#if defined __CONSOLE_APP__
   return;
#else
   
   #if DEBUG_PLOTIF_GL
   MessageInterface::ShowMessage
      ("PlotInterface::SetGlDrawOrbitFlag() plotName:%s\n", plotName.c_str());
   #endif
   
   wxString owner = wxString(plotName.c_str());
   MdiChildTrajFrame *frame = NULL;
   
   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildTrajFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());
      
      if (frame->GetPlotName().IsSameAs(owner.c_str()))
      {
         frame->SetGlDrawOrbitFlag(drawArray);
      }
   }
#endif
}


//------------------------------------------------------------------------------
// void SetGlShowObjectFlag(const std::string &plotName, ...
//------------------------------------------------------------------------------
void PlotInterface::SetGlShowObjectFlag(const std::string &plotName,
                                        const std::vector<bool> &showArray)
{
#if defined __CONSOLE_APP__
   return;
#else
   
   #if DEBUG_PLOTIF_GL
   MessageInterface::ShowMessage
      ("PlotInterface::SetGlShowObjectFlag() plotName:%s\n", plotName.c_str());
   #endif
   
   wxString owner = wxString(plotName.c_str());
   MdiChildTrajFrame *frame = NULL;
   
   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildTrajFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());
      
      if (frame->GetPlotName().IsSameAs(owner.c_str()))
      {
         frame->SetGlShowObjectFlag(showArray);
      }
   }
#endif
}


//------------------------------------------------------------------------------
// void SetGlUpdateFrequency(const std::string &plotName, Integer updFreq)
//------------------------------------------------------------------------------
void PlotInterface::SetGlUpdateFrequency(const std::string &plotName,
                                         Integer updFreq)
{
#if defined __CONSOLE_APP__
   return;
#else
   wxString owner = wxString(plotName.c_str());
   MdiChildTrajFrame *frame = NULL;
   
   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildTrajFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());
      
      if (frame->GetPlotName().IsSameAs(owner.c_str()))
      {
         frame->SetGlUpdateFrequency(updFreq);
      }
   }
#endif
}


//------------------------------------------------------------------------------
//  bool IsThere(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Checks if OpenGlPlot exist.
 */
//------------------------------------------------------------------------------
bool PlotInterface::IsThere(const std::string &plotName)
{    
#if defined __CONSOLE_APP__
   return true;
#else

   if (GmatAppData::Instance()->GetMainFrame() != NULL)
   {
      wxString owner = wxString(plotName.c_str());
      
      MdiChildTrajFrame *frame  = NULL;
      for (int i=0; i<MdiGlPlot::numChildren; i++)
      {
         frame = (MdiChildTrajFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());
            
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            return true;
         }
      }
   }

   return false;
#endif
}


//------------------------------------------------------------------------------
//  bool DeleteGlPlot(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Deletes OpenGlPlot by plot name.
 *
 * @param <plotName> name of plot to be deleted
 */
//------------------------------------------------------------------------------
bool PlotInterface::DeleteGlPlot(const std::string &plotName)
{    
#if defined __CONSOLE_APP__
   return true;
#else
   
   GmatAppData *gmatAppData = GmatAppData::Instance();
   
   if (gmatAppData->GetMainFrame() != NULL)
   {
      #if DEBUG_PLOTIF_GL_DELETE
      MessageInterface::ShowMessage
         ("PlotInterface::DeleteGlPlot() plotName=%s\n", plotName.c_str());
      #endif
      
      wxString owner = wxString(plotName.c_str());
      MdiChildTrajFrame *frame = NULL;
      
      for (int i=0; i<MdiGlPlot::numChildren; i++)
      {
         frame = (MdiChildTrajFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());
         
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {            
            gmatAppData->GetMainFrame()->CloseChild(owner, GmatTree::OUTPUT_OPENGL_PLOT);
            gmatAppData->GetMainFrame()->Tile();
            break;
         }
      }
   }
   
   return true;
#endif
}


//------------------------------------------------------------------------------
//  bool RefreshGlPlot(const std::string &plotName)
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

   if (GmatAppData::Instance()->GetMainFrame() != NULL)
   {
      #if DEBUG_PLOTIF_GL
      MessageInterface::ShowMessage
         ("PlotInterface::RefreshGlPlot() plotName=%s\n",plotName.c_str());
      #endif
      
      wxString owner = wxString(plotName.c_str());
      MdiChildTrajFrame *frame = NULL;
      
      for (int i=0; i<MdiGlPlot::numChildren; i++)
      {
         frame = (MdiChildTrajFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());
            
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
// bool SetGlEndOfRun(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Sets end of run flag to OpenGlPlot.
 */
//------------------------------------------------------------------------------
bool PlotInterface::SetGlEndOfRun(const std::string &plotName)
{    
#if defined __CONSOLE_APP__
   return true;
#else
   
   if (GmatAppData::Instance()->GetMainFrame() != NULL)
   {
      #if DEBUG_PLOTIF_GL
         MessageInterface::ShowMessage
            ("PlotInterface::SetGlEndOfRun() plotName=%s\n",plotName.c_str());
      #endif
      wxString owner = wxString(plotName.c_str());
      
      MdiChildTrajFrame *frame = NULL;
      for (int i=0; i<MdiGlPlot::numChildren; i++)
      {
         frame = (MdiChildTrajFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());
            
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            frame->SetEndOfRun();
         }
      }
   }
   
   return true;
#endif
}


//------------------------------------------------------------------------------
//  bool UpdateGlPlot(const std::string &plotName, ...
//------------------------------------------------------------------------------
/*
 * Buffers data and updates OpenGL plow window if updateCanvas is true
 */
//------------------------------------------------------------------------------
bool PlotInterface::UpdateGlPlot(const std::string &plotName,
                                 const std::string &oldName,
                                 const StringArray &scNames, const Real &time,
                                 const RealArray &posX, const RealArray &posY,
                                 const RealArray &posZ, const RealArray &velX,
                                 const RealArray &velY, const RealArray &velZ,
                                 const UnsignedIntArray &scColors, bool solving,
                                 Integer solverOption, bool updateCanvas)
{
#if defined __CONSOLE_APP__
   return true;
#else

   #if DEBUG_PLOTIF_GL_UPDATE
   MessageInterface::ShowMessage
      ("PlotInterface::UpdateGlPlot() entered. time = %f\n", time);
   #endif
   
   bool updated = false;
   wxString owner = wxString(plotName.c_str());

   MdiChildTrajFrame *frame = NULL;
   
   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildTrajFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());
      
      #if DEBUG_PLOTIF_GL_UPDATE
      MessageInterface::ShowMessage
         ("PlotInterface::UpdateGlPlot() frame[%d]->GetPlotName()=%s "
          "owner=%s\n", i, frame->GetPlotName().c_str(), owner.c_str());
      #endif
      
      if (frame)
      {
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            //MessageInterface::ShowMessage
            //   ("PlotInterface::UpdateGlPlot() now updating GL plot...\n");
            frame->UpdatePlot(scNames, time, posX, posY, posZ, velX, velY, velZ,
                              scColors, solving, solverOption, updateCanvas);
            
            updated = true;
         }
      }
   }
   
   return updated;
   
#endif
} // end UpdateGlPlot()


//------------------------------------------------------------------------------
// bool TakeGlAction(const std::string &plotName, const std::string &action)
//------------------------------------------------------------------------------
bool PlotInterface::TakeGlAction(const std::string &plotName,
                                 const std::string &action)
{
#if defined __CONSOLE_APP__
   return true;
#else
   #if DEBUG_PLOTIF_GL_CLEAR
   MessageInterface::ShowMessage
      ("PlotInterface::ClearGlSolverData() entered\n");
   #endif
   
   bool retval = false;
   wxString owner = wxString(plotName.c_str());
   
   MdiChildTrajFrame *frame = NULL;
   
   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildTrajFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());
      
      #if DEBUG_PLOTIF_GL_CLEAR
      MessageInterface::ShowMessage
         ("PlotInterface::ClearGlSolverData() frame[%d]->GetPlotName()=%s "
          "owner=%s\n", i, frame->GetPlotName().c_str(), owner.c_str());
      #endif
      
      if (frame)
      {
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            frame->TakeAction(action);           
            retval = true;
         }
      }
   }
   
   return retval;
#endif
}


//------------------------------------------------------------------------------
//  bool CreateTsPlotWindow(const std::string &plotName,
//                          const std::string &oldName,
//                          const std::string &plotTitle,
//                          const std::string &xAxisTitle,
//                          const std::string &yAxisTitle,
//                          bool drawGrid = false)
//------------------------------------------------------------------------------
/*
 * Creates TsPlot window.
 *
 * @param <plotName> name of plot
 */
//------------------------------------------------------------------------------
bool PlotInterface::CreateTsPlotWindow(const std::string &plotName,
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
   // check if new MDI child frame needed
   //-------------------------------------------------------
   bool createNewFrame = true;
   wxString currPlotName;
   MdiChildTsFrame *frame = NULL;
   
   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());
      currPlotName = frame->GetPlotName();

      if (currPlotName.IsSameAs(plotName.c_str()))
      {
         createNewFrame = false;
         break;
      }
      else if (currPlotName.IsSameAs(oldName.c_str()))
      {
         #if DEBUG_RENAME
         MessageInterface::ShowMessage
            ("PlotInterface::CreateTsPlotWindow() currPlotName=%s, oldName=%s\n",
             currPlotName.c_str(), oldName.c_str());
         #endif
         
         // change plot name
         frame->SetPlotName(wxString(plotName.c_str()));
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
         ("PlotInterface::CreateTsPlotWindow() Creating new "
          "MdiChildXyFrame\n   X Axis Title = %s  Y Axis Title = %s\n",
          xAxisTitle.c_str(), yAxisTitle.c_str());
      #endif
      
      // create a frame, containing a XY plot canvas
      frame =
         new MdiChildTsFrame(GmatAppData::Instance()->GetMainFrame(), true,
                             wxString(plotName.c_str()),
                             wxString(plotTitle.c_str()),
                             wxString(xAxisTitle.c_str()),
                             wxString(yAxisTitle.c_str()),
                             wxPoint(-1, -1), wxSize(500, 350),
                             wxDEFAULT_FRAME_STYLE);
      
      frame->Show();
      
      GmatAppData::Instance()->GetMainFrame()->Tile();

      ++MdiTsPlot::numChildren;

      frame->RedrawCurve();
   }
   
   frame->SetShowGrid(drawGrid);
   frame->ResetZoom();
   
   #if DEBUG_PLOTIF_XY
   MessageInterface::ShowMessage
      ("PlotInterface::CreateTsPlotWindow() leaving\n");
   #endif
   
   return true;
#endif
}

//------------------------------------------------------------------------------
//  bool DeleteTsPlot(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Deletes TsPlot by plot name.
 *
 * @param <plotName> name of plot to be deleted
 */
//------------------------------------------------------------------------------
bool PlotInterface::DeleteTsPlot(const std::string &plotName)
{    
#if defined __CONSOLE_APP__
   return true;
#else
   
   GmatAppData *gmatAppData = GmatAppData::Instance();
   
   if (gmatAppData->GetMainFrame() != NULL)
   {
      #if DEBUG_PLOTIF_XY
         MessageInterface::ShowMessage("PlotInterface::DeleteTsPlot()\n");
      #endif

      wxString owner = wxString(plotName.c_str());
      MdiChildTsFrame *frame = NULL;
      
      for (int i=0; i<MdiTsPlot::numChildren; i++)
      {
         frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());
         
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            gmatAppData->GetMainFrame()->CloseChild(owner, GmatTree::OUTPUT_XY_PLOT);
            gmatAppData->GetMainFrame()->Tile();
            break;
         }
      }
   }
   
   return true;
#endif
}

//------------------------------------------------------------------------------
// bool AddTsPlotCurve(const std::string &plotName, int curveIndex,
//                     int yOffset, Real yMin, Real yMax,
//                     const std::string &curveTitle,
//                     UnsignedInt penColor)
//------------------------------------------------------------------------------
/*
 * Adds a plot curve to XY plow window.
 */
//------------------------------------------------------------------------------
bool PlotInterface::AddTsPlotCurve(const std::string &plotName, int curveIndex,
                                   int yOffset, Real yMin, Real yMax,
                                   const std::string &curveTitle,
                                   UnsignedInt penColor)
{
#if defined __CONSOLE_APP__
   return true;
#else

   UnsignedInt localPenColor = penColor;
   if (penColor == 0) 
      localPenColor = 0xFFFFFF;

   bool added = false;
   
   #if DEBUG_PLOTIF_XY
   MessageInterface::ShowMessage
      ("PlotInterface::AddTsPlotCurve() entered."
       " plotName = " + plotName + " curveTitle = " + 
       curveTitle + "\n");
   
   MessageInterface::ShowMessage
      ("PlotInterface::AddTsPlotCurve() numChildren = %d\n",
       MdiTsPlot::numChildren);
   #endif
   
   MdiChildTsFrame *frame = NULL;
   for (int i = 0; i < MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());
      
      if (frame->GetPlotName().IsSameAs(plotName.c_str()))
      {
         frame->AddPlotCurve(curveIndex, yOffset, yMin, yMax,
                             wxString(curveTitle.c_str()), localPenColor);
         added = true;
      }
   }

   return added;
#endif
}

//------------------------------------------------------------------------------
// bool DeleteAllTsPlotCurves(const std::string &plotName,
//                            const std::string &oldName)
//------------------------------------------------------------------------------
/*
 * Deletes all plot curves in XY plow window.
 */
//------------------------------------------------------------------------------
bool PlotInterface::DeleteAllTsPlotCurves(const std::string &plotName,
                                          const std::string &oldName)
{
#if defined __CONSOLE_APP__
   return true;
#else

   #if DEBUG_PLOTIF_XY
   MessageInterface::ShowMessage
      ("PlotInterface::DeleteAllPlotCurve() plotName = %s "
       "numChildren = %d\n", plotName.c_str(),
       MdiTsPlot::numChildren);
   #endif
   
   MdiChildTsFrame *frame = NULL;
   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());
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
// bool DeleteTsPlotCurve(const std::string &plotName, int curveIndex)
//------------------------------------------------------------------------------
/*
 * Deletes a plot curve to XY plow window.
 */
//------------------------------------------------------------------------------
bool PlotInterface::DeleteTsPlotCurve(const std::string &plotName, int curveIndex)
{
#if defined __CONSOLE_APP__
   return true;
#else
   
   #if DEBUG_PLOTIF_XY
   MessageInterface::ShowMessage
      ("PlotInterface::DeleteTsPlotCurve() entered plotName = %s "
       "curveIndex = %d\n", plotName.c_str(), curveIndex);
   
   MessageInterface::ShowMessage
      ("PlotInterface::DeleteTsPlotCurve() numChildren = %d\n",
       MdiTsPlot::numChildren);
   #endif

   MdiChildTsFrame *frame = NULL;
   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());
     
      if (frame->GetPlotName().IsSameAs(plotName.c_str()))
      {
         frame->DeletePlotCurve(curveIndex);
      }
   }

   return true;
#endif
}


//------------------------------------------------------------------------------
// void ClearTsPlotData(const std::string &plotName))
//------------------------------------------------------------------------------
void PlotInterface::ClearTsPlotData(const std::string &plotName)
{
#if defined __CONSOLE_APP__
   return;
#else

   #if DEBUG_PLOTIF_XY
   MessageInterface::ShowMessage
      ("PlotInterface::ClearTsPlotData() numChildren = %d\n",
       MdiTsPlot::numChildren);
   #endif
   
   MdiChildTsFrame *frame = NULL;
   
   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());
      
      if (frame->GetPlotName().IsSameAs(plotName.c_str()))
      {
         frame->ClearPlotData();
      }
   }
#endif
}

//------------------------------------------------------------------------------
// void TsPlotPenUp(const std::string &plotName))
//------------------------------------------------------------------------------
void PlotInterface::TsPlotPenUp(const std::string &plotName)
{
#if defined __CONSOLE_APP__
   return;
#else

   #if DEBUG_PLOTIF_XY
      MessageInterface::ShowMessage
         ("PlotInterface::TsPlotPenUp() numChildren = %d\n",
          MdiTsPlot::numChildren);
   #endif
   
   MdiChildTsFrame *frame = NULL;
   
   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());
      
      if (frame->GetPlotName().IsSameAs(plotName.c_str()))
      {
         frame->PenUp();
      }
   }
#endif
}

//------------------------------------------------------------------------------
// void TsPlotPenDown(const std::string &plotName))
//------------------------------------------------------------------------------
void PlotInterface::TsPlotPenDown(const std::string &plotName)
{
#if defined __CONSOLE_APP__
   return;
#else

   #if DEBUG_PLOTIF_XY
      MessageInterface::ShowMessage
         ("PlotInterface::TsPlotPenUp() numChildren = %d\n",
          MdiTsPlot::numChildren);
   #endif
   
   MdiChildTsFrame *frame = NULL;
   
   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());
      
      if (frame->GetPlotName().IsSameAs(plotName.c_str()))
      {
         frame->PenDown();
      }
   }
#endif
}

//------------------------------------------------------------------------------
// void SetTsPlotTitle(const std::string &plotName, const std::string &plotTitle)
//------------------------------------------------------------------------------
void PlotInterface::SetTsPlotTitle(const std::string &plotName,
                                   const std::string &plotTitle)
{
#if defined __CONSOLE_APP__
   return;
#else
   
   #if DEBUG_PLOTIF_XY
   MessageInterface::ShowMessage
      ("PlotInterface::SetTsPlotTitle() plotName = %s "
       "plotTitle = %s\n", plotName.c_str(), plotTitle.c_str());
   #endif
   
   MdiChildTsFrame *frame = NULL;
   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());
      
      if (frame->GetPlotName().IsSameAs(plotName.c_str()))
      {
         #if DEBUG_PLOTIF_XY
            MessageInterface::ShowMessage
               ("PlotInterface::SetTsPlotTitle() calling "
                " frame->SetPlotTitle() \n");
         #endif
            
         frame->SetPlotTitle(wxString(plotTitle.c_str()));
      }
   }
#endif
}


//------------------------------------------------------------------------------
// void ShowTsPlotLegend(const std::string &plotName)
//------------------------------------------------------------------------------
void PlotInterface::ShowTsPlotLegend(const std::string &plotName)
{
#if defined __CONSOLE_APP__
   return;
#else
   
//   MdiChildXyFrame *frame = NULL;
//   for (int i=0; i<MdiTsPlot::numChildren; i++)
//   {
//      frame = (MdiChildXyFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());
//
//      if (frame->GetPlotName().IsSameAs(plotName.c_str()))
//      {
//         #if DEBUG_PLOTIF_XY
//            MessageInterface::ShowMessage
//               ("PlotInterface::ShowTsPlotLegend() calling  frame->ShowPlotLegend() \n");
//         #endif
//            
//         frame->ShowPlotLegend();
//      }
//   }
#endif
}


//------------------------------------------------------------------------------
// bool RefreshTsPlot(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Refreshes XY plot curve.
 *
 * @param <plotName> name of xy plot
 */
//------------------------------------------------------------------------------
bool PlotInterface::RefreshTsPlot(const std::string &plotName)
{
#if defined __CONSOLE_APP__
   return true;
#else

   if (GmatAppData::Instance()->GetMainFrame() != NULL)
   {        
      #if DEBUG_PLOTIF_XY_UPDATE
         MessageInterface::ShowMessage
            ("PlotInterface::RefreshTsPlot() plotName=%s, numChildren=%d\n",
             plotName.c_str(), MdiTsPlot::numChildren);
      #endif
      
      wxString owner = wxString(plotName.c_str());

      MdiChildTsFrame *frame = NULL;
      //MessageInterface::ShowMessage("PlotInterface::RefreshTsPlot(1)\n");
      
      for (int i=0; i<MdiTsPlot::numChildren; i++)
      {
         //MessageInterface::ShowMessage("PlotInterface::RefreshTsPlot(2)\n");
         frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());
         //MessageInterface::ShowMessage("PlotInterface::RefreshTsPlot(3)\n");
         if (frame)   
         {         
            if (frame->GetPlotName().IsSameAs(owner.c_str()))
            {
               //MessageInterface::ShowMessage("PlotInterface::RefreshTsPlot(4)\n");
               frame->RedrawCurve();
               //MessageInterface::ShowMessage("PlotInterface::RefreshTsPlot(5)\n");
               #if __WXMAC__  
                  frame->Refresh(true,NULL);
               #endif
            }
         }
      }
   }
   
   return true;
#endif
}


//------------------------------------------------------------------------------
// bool UpdateTsPlot(const std::string &plotName, const std::string &oldName,
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
bool PlotInterface::UpdateTsPlot(const std::string &plotName,
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

   #if DEBUG_PLOTIF_XY_UPDATE
   MessageInterface::ShowMessage
      ("PlotInterface::UpdateTsPlot() numChildren = %d\n",
       MdiTsPlot::numChildren);
   #endif
   
   MdiChildTsFrame *frame = NULL;
   
   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());
      
      if (frame)  //waw: Added null pointer check 03/23/06
      {
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            int numCurves = frame->GetCurveCount();
            #if DEBUG_PLOTIF_XY_UPDATE
               MessageInterface::ShowMessage
               ("PlotInterface::UpdateTsPlot() numCurves = %d\n", numCurves);
            #endif
            
            for (int j=0; j<numCurves; j++)
            {
               #if DEBUG_PLOTIF_XY_UPDATE
                  MessageInterface::ShowMessage
                  ("PlotInterface::UpdateTsPlot() yvals[%d] = %f\n", j, yvals(j));
               #endif
                
               frame->AddDataPoints(j, xval, yvals(j));
            }

            if (updateCanvas)
               frame->RedrawCurve();
            
            updated = true;
         }
      }
   }
   
   return updated;
#endif
}
