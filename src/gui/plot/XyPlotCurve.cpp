//$Header$
//------------------------------------------------------------------------------
//                              XyPlotCurve
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2004/01/20
/**
 * Declares XyPlotCurve for XY plot.
 */
//------------------------------------------------------------------------------
#include "XyPlotCurve.hpp"
#include "LinearInterpolator.hpp"

//------------------------------------------------------------------------------
// XyPlotCurve(int offsetY, double startY, double endY, double defaultY)
//------------------------------------------------------------------------------
XyPlotCurve::XyPlotCurve(int offsetY, double startY, double endY, //double defaultY,
                         //const wxString &xAxisTitle, const wxString &yAxisTitle,
                         const wxString &curveTitle)
    : wxPlotCurve(offsetY, startY, endY, curveTitle)
{
    //mDefaultY = defaultY;
    //mXAxisTitle = xAxisTitle;
    //mYAxisTitle = yAxisTitle;
    //mCurveTitle = curveTitle;
    
    //mInterp = NULL;
    mInterp = new LinearInterpolator(); //loj: should I allow user to use his own?
}

//------------------------------------------------------------------------------
// ~XyPlotCurve()
//------------------------------------------------------------------------------
XyPlotCurve::~XyPlotCurve()
{
    delete mInterp;
}

//------------------------------------------------------------------------------
// wxString GetXAxisTitle()
//------------------------------------------------------------------------------
//  wxString XyPlotCurve::GetXAxisTitle()
//  {
//      return mXAxisTitle;
//  }

//------------------------------------------------------------------------------
// wxString GetYAxisTitle()
//------------------------------------------------------------------------------
//  wxString XyPlotCurve::GetYAxisTitle()
//  {
//      return mYAxisTitle;
//  }

//loj: 2/20/04 moved to wxPlotCurve
//  //------------------------------------------------------------------------------
//  // wxString GetCurveTitle()
//  //------------------------------------------------------------------------------
//  wxString XyPlotCurve::GetCurveTitle()
//  {
//      return mCurveTitle;
//  }

//------------------------------------------------------------------------------
// double GetFirstX()
//------------------------------------------------------------------------------
double XyPlotCurve::GetFirstX()
{
    return mFirstX;
}

//------------------------------------------------------------------------------
// void SetFirstX(double x)
//------------------------------------------------------------------------------
void XyPlotCurve::SetFirstX(double x)
{
    mFirstX = x;
}

//------------------------------------------------------------------------------
// void AddData(double x, double y)
//------------------------------------------------------------------------------
void XyPlotCurve::AddData(double x, double y)
{
    //loj: for build2 x has to be time
    //loj: save a1mjd time as (21545xxx) for build 2, X has to be integer
    wxInt32 intTime = (wxInt32)(x * 1000); 
    
    mXdata.push_back(intTime);
    mYdata.push_back(y);
}

//------------------------------------------------------------------------------
// void SetInterpolator(Interpolator *interp)
//------------------------------------------------------------------------------
void XyPlotCurve::SetInterpolator(Interpolator *interp)
{
    if (mInterp == NULL)
        mInterp = interp;
}

//------------------------------------
// methods inherited from wxPlotCurve
//------------------------------------

//------------------------------------------------------------------------------
// virtual wxInt32 GetStartX()
//------------------------------------------------------------------------------
wxInt32 XyPlotCurve::GetStartX()
{
    if (mXdata.size() == 0)
        return 0;
    else
        return mXdata.front();
}

//------------------------------------------------------------------------------
// virtual wxInt32 GetEndX()
//------------------------------------------------------------------------------
wxInt32 XyPlotCurve::GetEndX()
{
    if (mXdata.size() == 0)
        return 0;
    else
        return mXdata.back();
}

//------------------------------------------------------------------------------
// virtual double GetY(wxInt32 x)
//------------------------------------------------------------------------------
double XyPlotCurve::GetY(wxInt32 x)
{
    double yVal = 0.0; // should be undefined?
    unsigned int i;
    
    for (i=0; i<mXdata.size(); i++)
    {
        if (mXdata[i] == x)
        {
            yVal = mYdata[i];
            break;
        }
        else if (mXdata[i] > x)
        {
            // interpolate
            mInterp->Clear();
            mInterp->AddPoint(mXdata[i-1], &mYdata[i-1]);
            mInterp->AddPoint(mXdata[i], &mYdata[i]);
            mInterp->Interpolate(x, &yVal);
            break;
        }
    }
    
    return yVal;
}
