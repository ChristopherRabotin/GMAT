//$Header$
//------------------------------------------------------------------------------
//                              TsPlotCurve
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Darrel Conway
// Created: 2005/05/20
/**
 * Declares TsPlotCurve for TsPlotCanvas's.
 *
 * @note The TsPlotCanvas subsystem is a generic plotting package developed by
 *       Thinking Systems using company resources.  Because the package was
 *       developed independently from the GMAT project, it uses native C++
 *       data types (e.g. int rather than Integer, double rather than Real), and
 *       in that respect will look different from other pieces of teh GMAT code
 *       base.
 */
//------------------------------------------------------------------------------


#include "TsPlotCurve.hpp"
#include "LinearInterpolator.hpp"
#include "MessageInterface.hpp"


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
