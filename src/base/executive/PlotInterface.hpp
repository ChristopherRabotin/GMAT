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

class PlotInterface
{

public:
   // for OpenGL Plot
   static bool CreateGlPlotWindow(const std::string &plotName);
   static bool DeleteGlPlot();
   
   //loj: 5/6/04 added drawWireFrame
   static bool UpdateGlSpacecraft(const std::string &plotName,
                                  const Real &time, const Real &posX,
                                  const Real &posY, const Real &posZ,
                                  const UnsignedInt orbitColor,
                                  const UnsignedInt targetColor,
                                  bool updateCanvas, bool drawWireFrame);

   // for XY plot
   static bool CreateXyPlotWindow(const std::string &plotName,
                                  const std::string &plotTitle,
                                  const std::string &xAxisTitle,
                                  const std::string &yAxisTitle);
   static bool DeleteXyPlot(bool hideFrame); //loj: 3/8/04 added
   static bool AddXyPlotCurve(const std::string &plotName, int curveIndex,
                              int yOffset, Real yMin, Real yMax,
                              const std::string &curveTitle,
                              const std::string &penColor);
   static bool DeleteAllXyPlotCurves(const std::string &plotName);
   static bool DeleteXyPlotCurve(const std::string &plotName, int curveIndex);
   static void ClearXyPlotData(const std::string &plotName);
   static void SetXyPlotTitle(const std::string &plotName,
                              const std::string &plotTitle);
   static bool UpdateXyPlot(const std::string &plotName,
                            const Real &xval, const Rvector &yvals,
                            const std::string &plotTitle,
                            const std::string &xAxisTitle,
                            const std::string &yAxisTitle,
                            bool updateCanvas);
                             
    
private:
    
   PlotInterface();
   ~PlotInterface();

};

#endif
