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
#ifndef XyPlotCurve_hpp
#define XyPlotCurve_hpp

#include "gmatwxdefs.hpp"
//#include "wx/plot/plot.h"
#include "XyPlotWindow.hpp" // for wxPlotCurve
#include <vector>
#include "Interpolator.hpp"

class XyPlotCurve: public wxPlotCurve
{
public:
    XyPlotCurve(int offsetY, double startY, double endY,
                const wxString &curveTitle);
    ~XyPlotCurve();
    
    double GetFirstX();
    void SetFirstX(double x);
    void SetEndX(double x);
    void AddData(double x, double y);
    
    void SetInterpolator(Interpolator *interp);
    
    // methods inherited from wxPlotCurve
    virtual wxInt32 GetStartX();
    virtual wxInt32 GetEndX();
    virtual double GetY(wxInt32 x);
    virtual void ClearData(); //loj: 3/10/04 added

    double mFirstX;
    
    Interpolator *mInterp;
    
    std::vector<wxInt32> mXdata;
    std::vector<double>  mYdata;
};

#endif
