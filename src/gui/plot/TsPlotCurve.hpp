//$Header$
//------------------------------------------------------------------------------
//                              TSPlotCurve
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Darrel Conway
// Created: 2005/05/20
//
// This code is Copyright (c) 2005, Thinking Systems, Inc.
// Licensed under the Academic Free License version 3.0.
/**
 * Declares TsPlotCurve for TsPlotCanvas's.
 *
 * @note The TsPlotCanvas subsystem is a generic plotting package developed by
 *       Thinking Systems using company resources.  Because the package was
 *       developed independently from the GMAT project, it uses native C++
 *       data types (e.g. int rather than Integer, double rather than Real), and
 *       in that respect will look different from other pieces of the GMAT code
 *       base.
 */
//------------------------------------------------------------------------------
#ifndef TsPlotCurve_hpp
#define TsPlotCurve_hpp

#include <wx/wx.h>

// Forward reference for friend class
class TsPlotCanvas;

// Interpolators are currently turned off in this code
//#include "Interpolator.hpp"

#include <vector>


class TsPlotCurve
{
public:
   TsPlotCurve(int offsetY, double startY, double endY,
               const wxString &curveTitle);
   virtual ~TsPlotCurve();
   
   void AddData(double x, double y);
   void SetColour(wxColour rgb);
   
   virtual void   Clear();

   virtual double GetMinX();
   virtual double GetMaxX();
   virtual double GetY(double x);
   virtual double GetMinY();
   virtual double GetMaxY();

   virtual void PenUp();
   virtual void PenDown();
   
   const std::vector<int>* GetPenUpLocations();

protected:
   double minX;
   double maxX;
   double minY; 
   double maxY;
   
   bool   rangeChanged;
   bool   domainChanged;
   
   // Interpolator *mInterp;
   
   /// Independent data
   std::vector<double> abscissa;
   /// Dependent data
   std::vector<double> ordinate;
   /// Location for "pen up" commands
   std::vector<int>    penUpIndex;
   /// Location for "pen up" commands
   std::vector<wxColour> linecolor;
   /// Location for color changes
   std::vector<int>    colorChange;
   /// Flag indicating that the curve is accepting data
   bool penIsDown;

   /// Index of the last point that was plotted
   unsigned int lastPointPlotted;
   
   friend class TsPlotCanvas; 
};

#endif
