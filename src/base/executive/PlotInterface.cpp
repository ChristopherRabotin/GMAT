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
 * Implements PlotInterface class.
 * This class updates OpenGL canvas, XY plot window
 */
//------------------------------------------------------------------------------
#if !defined __CONSOLE_APP__
#include "gmatwxdefs.hpp"
#include "gmatwxrcs.hpp"
#include "GmatAppData.hpp"
// for OpenGL plot
#include "MdiParentGlFrame.hpp"
#include "MdiGlPlotData.hpp"
#include "MdiChildTrajFrame.hpp"
// for XY plot
#include "MdiParentXyFrame.hpp"
#include "MdiXyPlotData.hpp"
#include "MdiChildXyFrame.hpp"
#endif

#include <iostream>                // for cout, endl
#include "PlotInterface.hpp"       // for PlotInterface functions
#include "Rvector.hpp"
#include "MessageInterface.hpp"    // for ShowMessage()

//---------------------------------
//  static data
//---------------------------------

//---------------------------------
//  public functions
//---------------------------------

//------------------------------------------------------------------------------
//  PlotInterface()
//------------------------------------------------------------------------------
PlotInterface::PlotInterface()
{
}

//------------------------------------------------------------------------------
//  ~PlotInterface()
//------------------------------------------------------------------------------
PlotInterface::~PlotInterface()
{
}

//------------------------------------------------------------------------------
//  bool CreateGlPlotWindow(bool canvasOnly)
//------------------------------------------------------------------------------
/*
 * Creates OpenGL plow window
 *
 * @param <canvasOnly> if true, it does not create MDI parent OpenGL plot window
 */
//------------------------------------------------------------------------------
bool PlotInterface::CreateGlPlotWindow(bool canvasOnly)
{    
#if !defined __CONSOLE_APP__
    if (!canvasOnly)
    {
        MdiGlPlot::mdiParentGlFrame =
            new MdiParentGlFrame((wxFrame *)NULL, -1, _T("MDI OpenGL Window"),
                                 //wxPoint(300, 200), wxSize(600, 500),
                                 wxPoint(600, 515), wxSize(600, 500),
                                 wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);
        // Give it an icon
#ifdef __WXMSW__
        MdiGlPlot::mdiParentGlFrame->SetIcon(wxIcon(_T("mdi_icn")));
#else
        MdiGlPlot::mdiParentGlFrame->SetIcon(wxIcon( mondrian_xpm ));
#endif
    }
    
    // create a frame, containing a opengl canvas
    MdiGlPlot::mdiParentGlFrame->mainSubframe =
        new MdiChildTrajFrame(MdiGlPlot::mdiParentGlFrame, true,
                              _T("Main Canvas Frame"),
                              wxPoint(-1, -1), wxSize(-1, -1),
                              wxDEFAULT_FRAME_STYLE);
    
    ++MdiGlPlot::numChildren;
    
    MdiGlPlot::mdiParentGlFrame->mainSubframe->SetTitle(_T("Main 3D Plot"));

    // initialize GL
    if (!MdiGlPlot::mdiParentGlFrame->mainSubframe->mCanvas->InitGL())
    {
        wxMessageDialog msgDialog(MdiGlPlot::mdiParentGlFrame,
                                  _T("InitGL() failed"), _T("CreateGlPlotWindow"));
        msgDialog.ShowModal();
        return false;
    }

    MdiGlPlot::mdiParentGlFrame->UpdateUI();
    return true;
    
#endif
    return false;
}

//------------------------------------------------------------------------------
//  bool UpdateGlSpacecraft(const Real &time, const Real &posX,
//                          const Real &posY, const Real &posZ, bool updateCanvas)
//------------------------------------------------------------------------------
/*
 * Updates OpenGL plow window
 */
//------------------------------------------------------------------------------
bool PlotInterface::UpdateGlSpacecraft(const Real &time, const Real &posX,
                                       const Real &posY, const Real &posZ,
                                       bool updateCanvas)
{   
#if !defined __CONSOLE_APP__
    if (MdiGlPlot::mdiParentGlFrame != NULL)
    {
        
       if (MdiGlPlot::numChildren > 0 &&
            MdiGlPlot::mdiParentGlFrame->mainSubframe != NULL)
        {
            MdiGlPlot::mdiParentGlFrame->mainSubframe->SetFocus();
            MdiGlPlot::mdiParentGlFrame->mainSubframe->
                UpdateSpacecraft(time, posX, posY, posZ, updateCanvas);
        }
        else
        {
            if (!CreateGlPlotWindow(true))
                return false;
            
            MdiGlPlot::mdiParentGlFrame->mainSubframe->SetFocus();
            MdiGlPlot::mdiParentGlFrame->mainSubframe->
                UpdateSpacecraft(time, posX, posY, posZ, updateCanvas);
        }
        
        return true;
    }
    else
    {
        //wxLogWarning("MdiParentGlFrame was not created. Creating a new MDI parent/child frame...");
        //wxLog::FlushActive();

        if (!CreateGlPlotWindow(false))
            return false;
        
        MdiGlPlot::mdiParentGlFrame->Show(true);
        
        // Update spacecraft trajectory (for now just one spacecraft)
        // later there will be UpdateUniverse()
        MdiGlPlot::mdiParentGlFrame->mainSubframe->
            UpdateSpacecraft(time, posX, posY, posZ, updateCanvas);
       
        return true;
    }
#endif
    return false;  // Must return something for console apps -- is this what we want?
} // end UpdateGlSpacecraft()


//------------------------------------------------------------------------------
//  bool CreateXyPlotWindow(bool canvasOnly,
//                          const std::string &plotName,
//                          const std::string &plotTitle,
//                          const std::string &xAxisTitle,
//                          const std::string &yAxisTitle)
//------------------------------------------------------------------------------
/*
 * Creates XY plow window
 *
 * @param <plotName> name of plot
 */
//------------------------------------------------------------------------------
bool PlotInterface::CreateXyPlotWindow(bool canvasOnly,
                                       const std::string &plotName,
                                       const std::string &plotTitle,
                                       const std::string &xAxisTitle,
                                       const std::string &yAxisTitle)
{    
#if !defined __CONSOLE_APP__
    if (!canvasOnly)
    {
        if (MdiXyPlot::mdiParentXyFrame == NULL)
        {
            MdiXyPlot::mdiParentXyFrame =
                new MdiParentXyFrame((wxFrame *)NULL, -1, _T("MDI XY Plot Window"),
                                     wxPoint(600, 10), wxSize(600, 500),
                                     //wxPoint(300, 200), wxSize(600, 500),
                                     wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);
            // Give it an icon
#ifdef __WXMSW__
            MdiXyPlot::mdiParentXyFrame->SetIcon(wxIcon(_T("mdi_icn")));
#else
            MdiXyPlot::mdiParentXyFrame->SetIcon(wxIcon( mondrian_xpm ));
#endif
        }
    }

    MessageInterface::ShowMessage("PlotInterface::CreateXyPlotWindow() "
                                  "X Axis Title = %s  Y Axis Title = %s\n",
                                  xAxisTitle.c_str(), yAxisTitle.c_str());
    
    // create a frame, containing a XY plot canvas
    MdiXyPlot::mdiParentXyFrame->mainSubframe =
        new MdiChildXyFrame(MdiXyPlot::mdiParentXyFrame,
                            wxString(plotName.c_str()),
                            wxString(plotTitle.c_str()),
                            wxString(xAxisTitle.c_str()),
                            wxString(yAxisTitle.c_str()),
                            wxPoint(-1, -1), wxSize(500, 350), //loj: 2/27/04 wxSize(-1, -1),
                            wxDEFAULT_FRAME_STYLE);
    
    ++MdiXyPlot::numChildren;
    
    MdiXyPlot::mdiParentXyFrame->Show(true);
    
    return true;
#endif
    return false;

}

//------------------------------------------------------------------------------
// bool AddXyPlotCurve(const std::string &plotName,
//                     int yOffset, Real yMin, Real yMax,
//                     const std::string &curveTitle,
//                     const std::string &penColor)
//------------------------------------------------------------------------------
/*
 * Adds a plot curve to XY plow window.
 */
//------------------------------------------------------------------------------
bool PlotInterface::AddXyPlotCurve(const std::string &plotName,
                                   int yOffset, Real yMin, Real yMax,
                                   const std::string &curveTitle,
                                   const std::string &penColor)
{
#if !defined __CONSOLE_APP__
    bool added = false;
    
    MessageInterface::ShowMessage("PlotInterface::AddXyPlotCurve() entered."
                                  " plotName = " + plotName + " curveTitle = " + 
                                  curveTitle + "\n");
    
    MessageInterface::ShowMessage("PlotInterface::AddXyPlotCurve() numChildren = %d\n",
                                  MdiXyPlot::numChildren);
    
    for (int i=0; i<MdiXyPlot::numChildren; i++)
    {
        MdiChildXyFrame *frame = (MdiChildXyFrame*)(MdiXyPlot::mdiChildren[i]);
        
        if (frame->GetPlotName() == wxString(plotName.c_str()))
        {
            frame->AddPlotCurve(yOffset, yMin, yMax,
                                wxString(curveTitle.c_str()),
                                wxString(penColor.c_str()));
            added = true;
        }
    }

    return added;
#endif
    return false;
}

//------------------------------------------------------------------------------
// bool UpdateXyPlot(const std::string &plotName,
//                   const Real &xval, const Rvector &yvals,
//                   const std::string &plotTitle,
//                   const std::string &xAxisTitle,
//                   const std::string &yAxisTitle, bool updateCanvas)
//------------------------------------------------------------------------------
/*
 * Updates XY plot curve.
 *
 * @param <plotName> name of xy plot
 * @param <xval> x value
 * @param <yvals> y values, should be in the order of curve added
 */
//------------------------------------------------------------------------------
bool PlotInterface::UpdateXyPlot(const std::string &plotName,
                                 const Real &xval, const Rvector &yvals,
                                 const std::string &plotTitle,
                                 const std::string &xAxisTitle,
                                 const std::string &yAxisTitle,
                                 bool updateCanvas)
{
#if !defined __CONSOLE_APP__
    
    bool updated = false;
    wxString owner = wxString(plotName.c_str());
    
    if (MdiXyPlot::mdiParentXyFrame == NULL)
    {        
        //wxLogWarning("MdiParentXyFrame was not created. Creating a new MDI parent/child frame...");
        //wxLog::FlushActive();
        CreateXyPlotWindow(false, plotName, plotTitle, xAxisTitle, yAxisTitle);
        MessageInterface::ShowMessage("PlotInterface::UpdateXyPlot()" + plotName + " " +
                                      plotTitle + " " + xAxisTitle + " " + yAxisTitle + "\n");
    }

    //loj: assume one canvas for now
    //loj: handle multiple plot canvas later
        
    //MessageInterface::ShowMessage("PlotInterface::UpdateXyPlot() numChildren = %d\n",
    //                              MdiXyPlot::numChildren);
    
    for (int i=0; i<MdiXyPlot::numChildren; i++)
    {
        MdiChildXyFrame *frame = (MdiChildXyFrame*)(MdiXyPlot::mdiChildren[i]);
            
        if (frame->GetPlotName() == owner)
        {
            int numCurves = frame->GetCurveCount();
            
            //MessageInterface::ShowMessage("PlotInterface::UpdateXyPlot() numCurves = %d\n", numCurves);
            for (int j=0; j<numCurves; j++)
            {
                //MessageInterface::ShowMessage("PlotInterface::UpdateXyPlot() yvals[%d] = %f\n", j,yvals(j));
                frame->AddDataPoints(j, xval, yvals(j));
            }

            if (updateCanvas)
                frame->RedrawCurve();
            
            updated = true;
        }
    }
    
    return updated;
    
#endif
    return false;
}
