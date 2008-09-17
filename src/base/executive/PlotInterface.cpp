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
PlotReceiver *PlotInterface::thePlotReceiver = NULL;

//---------------------------------
//  public functions
//---------------------------------


void PlotInterface::SetPlotReceiver(PlotReceiver *pr)
{
   thePlotReceiver = pr;
}


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
   if (thePlotReceiver != NULL)
      return thePlotReceiver->CreateGlPlotWindow(plotName, oldName, drawEcPlane, 
                   drawXyPlane, drawWireFrame, drawAxes, drawGrid, drawSunLine,
                   overlapPlot, usevpInfo, usepm, numPtsToRedraw);
   return false;
}


//------------------------------------------------------------------------------
// void SetGlSolarSystem(const std::string &plotName, SolarSystem *ss)
//------------------------------------------------------------------------------
void PlotInterface::SetGlSolarSystem(const std::string &plotName, SolarSystem *ss)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->SetGlSolarSystem(plotName, ss);
}


//------------------------------------------------------------------------------
// void SetGlObject(const std::string &plotName,  ...
//------------------------------------------------------------------------------
void PlotInterface::SetGlObject(const std::string &plotName,
                                const StringArray &objNames,
                                const UnsignedIntArray &objOrbitColors,
                                const std::vector<SpacePoint*> &objArray)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->SetGlObject(
            plotName, objNames, objOrbitColors, objArray);
}


//------------------------------------------------------------------------------
// static void SetGlCoordSystem(const std::string &plotName, ...
//------------------------------------------------------------------------------
void PlotInterface::SetGlCoordSystem(const std::string &plotName,
                                     CoordinateSystem *viewCs,
                                     CoordinateSystem *viewUpCs)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->SetGlCoordSystem(plotName, viewCs, viewUpCs);
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
   if (thePlotReceiver != NULL)
      thePlotReceiver->SetGlViewOption(plotName, vpRefObj, vpVecObj, vdObj, 
            vsFactor, vpRefVec, vpVec, vdVec, upAxis, usevpRefVec, usevpVec, 
            usevdVec, useFixedFov, fov);
}


//------------------------------------------------------------------------------
// void SetGlDrawOrbitFlag(const std::string &plotName, ...
//------------------------------------------------------------------------------
void PlotInterface::SetGlDrawOrbitFlag(const std::string &plotName,
                                       const std::vector<bool> &drawArray)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->SetGlDrawOrbitFlag(plotName, drawArray);
}


//------------------------------------------------------------------------------
// void SetGlShowObjectFlag(const std::string &plotName, ...
//------------------------------------------------------------------------------
void PlotInterface::SetGlShowObjectFlag(const std::string &plotName,
                                        const std::vector<bool> &showArray)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->SetGlShowObjectFlag(plotName, showArray);
}


//------------------------------------------------------------------------------
// void SetGlUpdateFrequency(const std::string &plotName, Integer updFreq)
//------------------------------------------------------------------------------
void PlotInterface::SetGlUpdateFrequency(const std::string &plotName,
                                         Integer updFreq)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->SetGlUpdateFrequency(plotName, updFreq);
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
   if (thePlotReceiver != NULL)
      return thePlotReceiver->IsThere(plotName);
   
   return false;
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
   if (thePlotReceiver != NULL)
      return thePlotReceiver->DeleteGlPlot(plotName);
   
   return false;
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
   if (thePlotReceiver != NULL)
      return thePlotReceiver->RefreshGlPlot(plotName);
   
   return false;
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
   if (thePlotReceiver != NULL)
      return thePlotReceiver->SetGlEndOfRun(plotName);
   
   return false;
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
   if (thePlotReceiver != NULL)
      return thePlotReceiver->UpdateGlPlot(plotName, oldName, scNames, time,
            posX, posY, posZ, velX, velY, velZ, scColors, solving, solverOption, 
            updateCanvas);

   return false;
} // end UpdateGlPlot()


//------------------------------------------------------------------------------
// bool TakeGlAction(const std::string &plotName, const std::string &action)
//------------------------------------------------------------------------------
bool PlotInterface::TakeGlAction(const std::string &plotName,
                                 const std::string &action)
{
   if (thePlotReceiver != NULL)
      return thePlotReceiver->TakeGlAction(plotName, action);
   
   return false;
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
   if (thePlotReceiver != NULL)
      return thePlotReceiver->CreateTsPlotWindow(plotName, oldName, plotTitle, 
            xAxisTitle, yAxisTitle, drawGrid);
   
   return false;
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
   if (thePlotReceiver != NULL)
      return thePlotReceiver->DeleteTsPlot(plotName);
   
   return false;
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
   if (thePlotReceiver != NULL)
      return thePlotReceiver->AddTsPlotCurve(plotName, curveIndex, yOffset, 
            yMin, yMax, curveTitle, penColor);
   
   return false;
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
   if (thePlotReceiver != NULL)
      return thePlotReceiver->DeleteAllTsPlotCurves(plotName, oldName);
      
   return false;
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
   if (thePlotReceiver != NULL)
      return thePlotReceiver->DeleteTsPlotCurve(plotName, curveIndex);
      
   return false;
}


//------------------------------------------------------------------------------
// void ClearTsPlotData(const std::string &plotName))
//------------------------------------------------------------------------------
void PlotInterface::ClearTsPlotData(const std::string &plotName)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->ClearTsPlotData(plotName);
}

//------------------------------------------------------------------------------
// void TsPlotPenUp(const std::string &plotName))
//------------------------------------------------------------------------------
void PlotInterface::TsPlotPenUp(const std::string &plotName)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->TsPlotPenUp(plotName);
}

//------------------------------------------------------------------------------
// void TsPlotPenDown(const std::string &plotName))
//------------------------------------------------------------------------------
void PlotInterface::TsPlotPenDown(const std::string &plotName)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->TsPlotPenDown(plotName);
}

//------------------------------------------------------------------------------
// void SetTsPlotTitle(const std::string &plotName, const std::string &plotTitle)
//------------------------------------------------------------------------------
void PlotInterface::SetTsPlotTitle(const std::string &plotName,
                                   const std::string &plotTitle)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->SetTsPlotTitle(plotName, plotTitle);
}


//------------------------------------------------------------------------------
// void ShowTsPlotLegend(const std::string &plotName)
//------------------------------------------------------------------------------
void PlotInterface::ShowTsPlotLegend(const std::string &plotName)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->ShowTsPlotLegend(plotName);
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
   if (thePlotReceiver != NULL)
      return thePlotReceiver->RefreshTsPlot(plotName);
      
   return false;
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
   if (thePlotReceiver != NULL)
      return thePlotReceiver->UpdateTsPlot(plotName, oldName, xval, yvals,
            plotTitle, xAxisTitle, yAxisTitle, updateCanvas, drawGrid);
      
   return false;
}
