//$Id$
//------------------------------------------------------------------------------
//                             PlotInterface
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel Conway, based on code written by Linda Jun
// Created: 2008/06/16
//
/**
 * Declares PlotReceiver class.
 */
//------------------------------------------------------------------------------
#ifndef PlotReceiver_hpp
#define PlotReceiver_hpp

#include "gmatdefs.hpp"
#include "Rvector.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"

/**
 * PlotReceiver defines the interfaces used for 3D and XY plot classes.
 */
class PlotReceiver
{
public:
   // for OpenGL Plot
   virtual bool CreateGlPlotWindow(const std::string &plotName,
                           const std::string &oldName,
                           bool drawEcPlane, bool drawEqPlane,
                           bool drawWireFrame, bool drawAxes, bool drawGrid,
                           bool drawESLines, bool overlapPlot,
                           bool usevpInfo, bool usepm,
                           Integer numPtsToRedraw) = 0;
   
   virtual void SetGlSolarSystem(const std::string &plotName, 
                    SolarSystem *ss) = 0;
   
   virtual void SetGlObject(const std::string &plotName,
                    const StringArray &objNames,
                    const UnsignedIntArray &objOrbitColors,
                    const std::vector<SpacePoint*> &objArray) = 0;
   
   virtual void SetGlCoordSystem(const std::string &plotName,
                         CoordinateSystem *viewCs,
                         CoordinateSystem *viewUpCs) = 0;
   
   virtual void SetGlViewOption(const std::string &plotName,
                        SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                        SpacePoint *vdObj, Real vsFactor,
                        const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                        const Rvector3 &vdVec, const std::string &upAxis,
                        bool usevpRefVec, bool usevpVec, bool usevdVec,
                        bool useFixedFov, Real fov) = 0;
   
   virtual void SetGlDrawOrbitFlag(const std::string &plotName,
                           const std::vector<bool> &drawArray) = 0;
   
   virtual void SetGlShowObjectFlag(const std::string &plotName,
                            const std::vector<bool> &showArray) = 0;
   
   virtual void SetGlUpdateFrequency(const std::string &plotName, 
                        Integer updFreq) = 0;
   
   virtual bool IsThere(const std::string &plotName) = 0;
   
   virtual bool DeleteGlPlot(const std::string &plotName) = 0;
   virtual bool RefreshGlPlot(const std::string &plotName) = 0;
   virtual bool SetGlEndOfRun(const std::string &plotName) = 0;
   
   virtual bool UpdateGlPlot(const std::string &plotName,
                     const std::string &oldName,
                     const StringArray &scNames, const Real &time,
                     const RealArray &posX, const RealArray &posY,
                     const RealArray &posZ, const RealArray &velX,
                     const RealArray &velY, const RealArray &velZ,
                     const UnsignedIntArray &scColors, bool solving,
                     Integer solverOption, bool updateCanvas) = 0;
   
   virtual bool TakeGlAction(const std::string &plotName,
                     const std::string &action) = 0;
   
   // for XY plot
   virtual bool CreateTsPlotWindow(const std::string &plotName,
                           const std::string &oldName,
                           const std::string &plotTitle,
                           const std::string &xAxisTitle,
                           const std::string &yAxisTitle,
                           bool drawGrid = false) = 0;
   virtual bool DeleteTsPlot(const std::string &plotName) = 0;
   virtual bool AddTsPlotCurve(const std::string &plotName, int curveIndex,
                       int yOffset, Real yMin, Real yMax,
                       const std::string &curveTitle,
                       UnsignedInt penColor) = 0;
   virtual bool DeleteAllTsPlotCurves(const std::string &plotName,
                              const std::string &oldName) = 0;
   virtual bool DeleteTsPlotCurve(const std::string &plotName, 
                       int curveIndex) = 0;
   virtual void ClearTsPlotData(const std::string &plotName) = 0;
   virtual void TsPlotPenUp(const std::string &plotName) = 0;
   virtual void TsPlotPenDown(const std::string &plotName) = 0;
   
   virtual void SetTsPlotTitle(const std::string &plotName,
                       const std::string &plotTitle) = 0;
   virtual void ShowTsPlotLegend(const std::string &plotName) = 0;
   virtual bool RefreshTsPlot(const std::string &plotName) = 0;
   virtual bool UpdateTsPlot(const std::string &plotName,
                     const std::string &oldName,
                     const Real &xval, const Rvector &yvals,
                     const std::string &plotTitle,
                     const std::string &xAxisTitle,
                     const std::string &yAxisTitle,
                     bool updateCanvas, bool drawGrid) = 0;
   
protected:
   PlotReceiver();
   virtual ~PlotReceiver();
};

#endif
