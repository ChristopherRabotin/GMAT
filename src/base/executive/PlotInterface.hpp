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
    static bool CreateGlPlotWindow(bool canvasOnly); 
    static bool UpdateGlSpacecraft(const Real &time, const Real &posX,
                                   const Real &posY, const Real &posZ,
                                   bool updateCanvas);

    // for XY plot
    static bool CreateXyPlotWindow(bool canvasOnly,
                                   const std::string &plotName,
                                   const std::string &plotTitle,
                                   const std::string &xAxisTitle,
                                   const std::string &yAxisTitle);
    static bool AddXyPlotCurve(const std::string &plotName, int cuveNum,
                               int yOffset, Real yMin, Real yMax,
                               const std::string &curveTitle,
                               const std::string &penColor);
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
