//$Id$
//------------------------------------------------------------------------------
//                             GuiPlotReceiver
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
 * Declares GuiPlotReceiver class.
 */
//------------------------------------------------------------------------------
#ifndef GuiPlotReceiver_hpp
#define GuiPlotReceiver_hpp

#include "gmatdefs.hpp"
#include "Rvector.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include "PlotReceiver.hpp"

class GuiPlotReceiver : public PlotReceiver
{

public:
   static GuiPlotReceiver* Instance();
   
   // for OpenGL Plot
   virtual bool CreateGlPlotWindow(const std::string &plotName,
                                  const std::string &oldName,
                                  bool drawEcPlane, bool drawEqPlane,
                                  bool drawWireFrame, bool drawAxes, bool drawGrid,
                                  bool drawESLines, bool overlapPlot,
                                  bool usevpInfo, bool usepm,
                                  Integer numPtsToRedraw);
   
   virtual void SetGlSolarSystem(const std::string &plotName, SolarSystem *ss);
   
   virtual void SetGlObject(const std::string &plotName,
                           const StringArray &objNames,
                           const UnsignedIntArray &objOrbitColors,
                           const std::vector<SpacePoint*> &objArray);
   
   virtual void SetGlCoordSystem(const std::string &plotName,
                                CoordinateSystem *viewCs,
                                CoordinateSystem *viewUpCs);
   
   virtual void SetGlViewOption(const std::string &plotName,
                               SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                               SpacePoint *vdObj, Real vsFactor,
                               const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                               const Rvector3 &vdVec, const std::string &upAxis,
                               bool usevpRefVec, bool usevpVec, bool usevdVec,
                               bool useFixedFov, Real fov);
   
   virtual void SetGlDrawOrbitFlag(const std::string &plotName,
                                  const std::vector<bool> &drawArray);
   
   virtual void SetGlShowObjectFlag(const std::string &plotName,
                                   const std::vector<bool> &showArray);
   
   virtual void SetGlUpdateFrequency(const std::string &plotName, Integer updFreq);
   
   virtual bool IsThere(const std::string &plotName);
   
   virtual bool DeleteGlPlot(const std::string &plotName);
   virtual bool RefreshGlPlot(const std::string &plotName);
   virtual bool SetGlEndOfRun(const std::string &plotName);
   
   virtual bool UpdateGlPlot(const std::string &plotName,
                            const std::string &oldName,
                            const StringArray &scNames, const Real &time,
                            const RealArray &posX, const RealArray &posY,
                            const RealArray &posZ, const RealArray &velX,
                            const RealArray &velY, const RealArray &velZ,
                            const UnsignedIntArray &scColors, bool solving,
                            Integer solverOption, bool updateCanvas);
   
   virtual bool TakeGlAction(const std::string &plotName,
                            const std::string &action);
   
   // for XY plot
   virtual bool CreateTsPlotWindow(const std::string &plotName,
                                  const std::string &oldName,
                                  const std::string &plotTitle,
                                  const std::string &xAxisTitle,
                                  const std::string &yAxisTitle,
                                  bool drawGrid = false);
   virtual bool DeleteTsPlot(const std::string &plotName);
   virtual bool AddTsPlotCurve(const std::string &plotName, int curveIndex,
                              int yOffset, Real yMin, Real yMax,
                              const std::string &curveTitle,
                              UnsignedInt penColor);
   virtual bool DeleteAllTsPlotCurves(const std::string &plotName,
                                     const std::string &oldName);
   virtual bool DeleteTsPlotCurve(const std::string &plotName, int curveIndex);
   virtual void ClearTsPlotData(const std::string &plotName);
   virtual void TsPlotPenUp(const std::string &plotName);
   virtual void TsPlotPenDown(const std::string &plotName);
   
   virtual void SetTsPlotTitle(const std::string &plotName,
                              const std::string &plotTitle);
   virtual void ShowTsPlotLegend(const std::string &plotName);
   virtual bool RefreshTsPlot(const std::string &plotName);
   virtual bool UpdateTsPlot(const std::string &plotName,
                            const std::string &oldName,
                            const Real &xval, const Rvector &yvals,
                            const std::string &plotTitle,
                            const std::string &xAxisTitle,
                            const std::string &yAxisTitle,
                            bool updateCanvas, bool drawGrid);
   

   
private:
   GuiPlotReceiver();
   virtual ~GuiPlotReceiver();

   static GuiPlotReceiver* theGuiPlotReceiver;
};

#endif
