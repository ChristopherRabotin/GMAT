//$Header$
//------------------------------------------------------------------------------
//                              TsPlotCurve
//------------------------------------------------------------------------------
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2005/05/20
//
// This code is Copyright (c) 2005, Thinking Systems, Inc.
// Licensed under the Academic Free License version 3.0.
/**
 * Implements the TsPlotCurves used on TsPlotCanvas's.
 */
//------------------------------------------------------------------------------
 

#include "TsPlotCurve.hpp"
//#include "LinearInterpolator.hpp"
#include "MessageInterface.hpp"


// #define DEBUG_PENUP_PENDOWN


//------------------------------------------------------------------------------
// TsPlotCurve(int offsetY, double startY, double endY, double defaultY)
//------------------------------------------------------------------------------
TsPlotCurve::TsPlotCurve(int offsetY, double startY, double endY,
                         const wxString &curveTitle) :
   minX              (1e99),
   maxX              (-1e99),
   minY              (1e99),
   maxY              (-1e99),
   rangeChanged      (false),
   domainChanged     (false),
   lineWidth         (1),
   lineStyle         (wxSOLID),
   penIsDown         (true),
   lastPointPlotted  (0)
{
//   mInterp = new LinearInterpolator();
   ordinate.clear();
   abscissa.clear();
}


//------------------------------------------------------------------------------
// ~TsPlotCurve()
//------------------------------------------------------------------------------
TsPlotCurve::~TsPlotCurve()
{
   ordinate.clear();
   abscissa.clear();

//   delete mInterp;
}


//------------------------------------------------------------------------------
// void AddData(double x, double y)
//------------------------------------------------------------------------------
void TsPlotCurve::AddData(double x, double y)
{
   #ifdef DEBUG_PENUP_PENDOWN
      static Integer counter = 0;
      if (abscissa.size() == 0) 
         counter = 0;
         
      if (abscissa.size() == 80 && counter == 0)
      {
         counter = 1;
         MessageInterface::ShowMessage("Pen up\n");
         PenUp();
      }
      if (counter == 1) 
      {
         ++counter;
         MessageInterface::ShowMessage("Pen down\n");
         PenDown();
      }
   #endif
   
   if (penIsDown)
   {
      if (abscissa.size() == 0) 
      {
         #ifdef DEBUG_FIRST_POINT
            MessageInterface::ShowMessage("Adding initial data: [%lf, %lf]\n", 
               x, y);
         #endif
         
         minX = maxX = x;
         minY = maxY = y;
         rangeChanged = true;
         domainChanged = true;
      }
         
      abscissa.push_back(x);
      ordinate.push_back(y);
      
   
      if (x < minX)
      {
         minX = x;
         domainChanged = true;
      }
      if (x > maxX)
      {
         maxX = x;
         domainChanged = true;
      }
      if (y < minY)
      {
         minY = y;
         rangeChanged = true;
      }
      if (y > maxY)
      {
         maxY = y;
         rangeChanged = true;
      }
   
      #if DEBUG_XY_PLOT_CURVE_ADD
         MessageInterface::ShowMessage
            ("TsPlotCurve::AddData() size = %d, x = %lf, y = %lf\n",
             abscissa.size(), abscissa.back(), ordinate.back());
      #endif
   }
}

////------------------------------------------------------------------------------
//// void SetInterpolator(Interpolator *interp)
////------------------------------------------------------------------------------
//void TsPlotCurve::SetInterpolator(Interpolator *interp)
//{
//   if (mInterp == NULL)
//      mInterp = interp;
//}


//------------------------------------------------------------------------------
// double GetStartX()
//------------------------------------------------------------------------------
double TsPlotCurve::GetMinX()
{
      return minX;
}

//------------------------------------------------------------------------------
// double GetEndX()
//------------------------------------------------------------------------------
double TsPlotCurve::GetMaxX()
{
      return maxX;
}


//------------------------------------------------------------------------------
// virtual double GetY(wxInt32 x)
//------------------------------------------------------------------------------
double TsPlotCurve::GetY(double x)
{
   double yVal = 0.0;
   unsigned int i;
    
   for (i=0; i < abscissa.size(); i++)
   {
      if (abscissa[i] == x)
      {
         yVal = ordinate[i];
         break;
      }
      else if (abscissa[i] > x)
      {
         if (i == 0)
            ++i;
         yVal = ordinate[i-1];
         // interpolate
//         mInterp->Clear();
//         mInterp->AddPoint(mXdata[i-1], &mYdata[i-1]);
//         mInterp->AddPoint(mXdata[i], &mYdata[i]);
//         mInterp->Interpolate(x, &yVal);
         break;
      }
   }
    
   #if DEBUG_XY_PLOT_CURVE_GET
      MessageInterface::ShowMessage
         ("TsPlotCurve::GetY() size = %d, x = %lf, y = %lf\n",
          abscissa.size(), x, yVal);
   #endif
   
   return yVal;
}


//------------------------------------------------------------------------------
// virtual void Clear()
//------------------------------------------------------------------------------
void TsPlotCurve::Clear()
{
   abscissa.clear();
   ordinate.clear();
   penUpIndex.clear();
   linecolor.clear();
   colorChange.clear();
}


//------------------------------------------------------------------------------
// virtual double GetYMin()
//------------------------------------------------------------------------------
double TsPlotCurve::GetMinY()
{
   return minY;
}


//------------------------------------------------------------------------------
// virtual double GetYMax()
//------------------------------------------------------------------------------
double TsPlotCurve::GetMaxY()
{
#if DEBUG_XY_PLOT_CURVE_MAX
   MessageInterface::ShowMessage
      ("TsPlotCurve::GetYMax() size = %d, max=%f\n",
       ordinate.size(), maxY);
#endif
   
   return maxY;
}


//------------------------------------------------------------------------------
// void PenUp()
//------------------------------------------------------------------------------
void TsPlotCurve::PenUp()
{
   penUpIndex.push_back((int)abscissa.size() - 1);
   penIsDown = false;
}

//------------------------------------------------------------------------------
// void PenDown()
//------------------------------------------------------------------------------
void TsPlotCurve::PenDown()
{
   penIsDown = true;
}


//------------------------------------------------------------------------------
// const std::vector<int>* GetPenUpLocations()
//------------------------------------------------------------------------------
const std::vector<int>* TsPlotCurve::GetPenUpLocations()
{
   return &penUpIndex;
}


void TsPlotCurve::SetColour(wxColour rgb)
{
   linecolor.push_back(rgb);
   colorChange.push_back((int)abscissa.size());
}

void TsPlotCurve::SetWidth(int w)
{
   lineWidth = w;
}

int TsPlotCurve::GetWidth()
{
   return lineWidth;
}


/**
 * Line styles defined in wxWidgets
 *    wxSOLID  Solid style.
 *    wxTRANSPARENT  No pen is used.
 *    wxDOT    Dotted style.
 *    wxLONG_DASH    Long dashed style.
 *    wxSHORT_DASH   Short dashed style.
 *    wxDOT_DASH  Dot and dash style.
 *    wxSTIPPLE   Use the stipple bitmap.
 *    wxUSER_DASH    Use the user dashes: see wxPen::SetDashes.
 */
void TsPlotCurve::SetStyle(int ls)
{
   lineStyle = ls;
}


int TsPlotCurve::GetStyle()
{
   return lineStyle;
}
