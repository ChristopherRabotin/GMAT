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
                                  bool drawWireFrame = false,
                                  bool overlapPlot = false,
                                  SolarSystem *ss = NULL); //loj: 12/14/04 Added
   static bool DeleteGlPlot();
   static bool RefreshGlPlot(const std::string &plotName);
   
   //loj: 7/13/04 handles multiple spacecraft postion and color
   static bool UpdateGlSpacecraft(const std::string &plotName,
                                  const std::string &oldName,
                                  const Real &time, const RealArray &posX,
                                  const RealArray &posY, const RealArray &posZ,
                                  const UnsignedIntArray &color,
                                  bool updateCanvas, bool drawWireFrame = false);

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
