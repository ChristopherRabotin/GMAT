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
 * Declares PlotInterface class.
 */
//------------------------------------------------------------------------------
#ifndef PlotInterface_hpp
#define PlotInterface_hpp

#include "gmatdefs.hpp"
#include "Rvector.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"

class PlotInterface
{

public:
   // for OpenGL Plot
   static bool CreateGlPlotWindow(const std::string &plotName,
                                  const std::string &oldName,
                                  bool drawEcPlane, bool drawEqPlane,
                                  bool drawWireFrame, bool drawAxes, bool drawGrid,
                                  bool drawESLines, bool overlapPlot,
                                  bool usevpInfo, bool usepm,
                                  Integer numPtsToRedraw);
   
   static void SetGlSolarSystem(const std::string &plotName, SolarSystem *ss);
   
   static void SetGlObject(const std::string &plotName,
                           const StringArray &objNames,
                           const UnsignedIntArray &objOrbitColors,
                           const std::vector<SpacePoint*> &objArray);
   
   static void SetGlCoordSystem(const std::string &plotName,
                                CoordinateSystem *viewCs,
                                CoordinateSystem *viewUpCs);
   
   static void SetGlViewOption(const std::string &plotName,
                               SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                               SpacePoint *vdObj, Real vsFactor,
                               const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                               const Rvector3 &vdVec, const std::string &upAxis,
                               bool usevpRefVec, bool usevpVec, bool usevdVec,
                               bool useFixedFov, Real fov);
   
   static void SetGlDrawOrbitFlag(const std::string &plotName,
                                  const std::vector<bool> &drawArray);
   
   static void SetGlShowObjectFlag(const std::string &plotName,
                                   const std::vector<bool> &showArray);
   
   static void SetGlUpdateFrequency(const std::string &plotName, Integer updFreq);
   
   static bool IsThere(const std::string &plotName);
   
   static bool DeleteGlPlot(const std::string &plotName);
   static bool RefreshGlPlot(const std::string &plotName);
   static bool SetGlEndOfRun(const std::string &plotName);
   
   static bool UpdateGlPlot(const std::string &plotName,
                            const std::string &oldName,
                            const StringArray &scNames, const Real &time,
                            const RealArray &posX, const RealArray &posY,
                            const RealArray &posZ, const RealArray &velX,
                            const RealArray &velY, const RealArray &velZ,
                            const UnsignedIntArray &scColors, bool solving,
                            Integer solverOption, bool updateCanvas);
   
   static bool TakeGlAction(const std::string &plotName,
                            const std::string &action);
   
   // for XY plot
   static bool CreateTsPlotWindow(const std::string &plotName,
                                  const std::string &oldName,
                                  const std::string &plotTitle,
                                  const std::string &xAxisTitle,
                                  const std::string &yAxisTitle,
                                  bool drawGrid = false);
   static bool DeleteTsPlot(const std::string &plotName);
   static bool AddTsPlotCurve(const std::string &plotName, int curveIndex,
                              int yOffset, Real yMin, Real yMax,
                              const std::string &curveTitle,
                              UnsignedInt penColor);
   static bool DeleteAllTsPlotCurves(const std::string &plotName,
                                     const std::string &oldName);
   static bool DeleteTsPlotCurve(const std::string &plotName, int curveIndex);
   static void ClearTsPlotData(const std::string &plotName);
   static void TsPlotPenUp(const std::string &plotName);
   static void TsPlotPenDown(const std::string &plotName);
   
   static void SetTsPlotTitle(const std::string &plotName,
                              const std::string &plotTitle);
   static void ShowTsPlotLegend(const std::string &plotName);
   static bool RefreshTsPlot(const std::string &plotName);
   static bool UpdateTsPlot(const std::string &plotName,
                            const std::string &oldName,
                            const Real &xval, const Rvector &yvals,
                            const std::string &plotTitle,
                            const std::string &xAxisTitle,
                            const std::string &yAxisTitle,
                            bool updateCanvas, bool drawGrid);
   

   
private:

   PlotInterface();
   ~PlotInterface();

};

#endif
