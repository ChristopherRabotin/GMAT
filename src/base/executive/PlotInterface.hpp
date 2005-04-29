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
 * Declares PlotInterface class.
 */
//------------------------------------------------------------------------------
#ifndef PlotInterface_hpp
#define PlotInterface_hpp

#include "gmatdefs.hpp"
#include "Rvector.hpp"
#include "SolarSystem.hpp"

class PlotInterface
{

public:
   // for OpenGL Plot
   static bool CreateGlPlotWindow(const std::string &plotName,
                                  const std::string &oldName,
                                  const std::string &csName,
                                  SolarSystem *ssPtr,
                                  bool drawEcPlane, bool drawEqPlane,
                                  bool drawWireFrame, bool overlapPlot,
                                  bool usevpInfo, bool usepm);

   static void SetGlViewOption(const std::string &plotName,
                               SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                               SpacePoint *vdObj, Real vsFactor,
                               const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                               const Rvector3 &vdVec, bool usevpRefVec,
                               bool usevpVec, bool usevdVec);

   static bool IsThere(const std::string &plotName);
   
   static bool DeleteGlPlot();
   static bool RefreshGlPlot(const std::string &plotName);
   
   static bool UpdateGlSpacecraft(const std::string &plotName,
                                  const std::string &oldName,
                                  const std::string &csName,
                                  const StringArray &scNameArray,
                                  const Real &time, const RealArray &posX,
                                  const RealArray &posY, const RealArray &posZ,
                                  const UnsignedIntArray &color,
                                  bool updateCanvas);

   // for XY plot
   static bool CreateXyPlotWindow(const std::string &plotName,
                                  const std::string &oldName,
                                  const std::string &plotTitle,
                                  const std::string &xAxisTitle,
                                  const std::string &yAxisTitle,
                                  bool drawGrid = false);
   static bool DeleteXyPlot(bool hideFrame);
   static bool AddXyPlotCurve(const std::string &plotName, int curveIndex,
                              int yOffset, Real yMin, Real yMax,
                              const std::string &curveTitle,
                              UnsignedInt penColor);
   static bool DeleteAllXyPlotCurves(const std::string &plotName,
                                     const std::string &oldName);
   static bool DeleteXyPlotCurve(const std::string &plotName, int curveIndex);
   static void ClearXyPlotData(const std::string &plotName);
   static void SetXyPlotTitle(const std::string &plotName,
                              const std::string &plotTitle);
   static void ShowXyPlotLegend(const std::string &plotName);
   static bool RefreshXyPlot(const std::string &plotName);
   static bool UpdateXyPlot(const std::string &plotName,
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
