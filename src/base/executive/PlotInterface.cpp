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

//================================================
//#if !defined __CONSOLE_APP__
//================================================
//------------------------------------------------------------------------------
//  bool CreateGlPlotWindow()
//------------------------------------------------------------------------------
/*
 * Creates OpenGlPlot window
 *
 * @param <canvasOnly> if true, it does not create MDI parent OpenGL plot window
 */
//------------------------------------------------------------------------------
bool PlotInterface::CreateGlPlotWindow()
{    
#if defined __CONSOLE_APP__
    return true;
#else

    //loj: 3/8/04 added if
    if (MdiGlPlot::mdiParentGlFrame == NULL)
    {
        //MessageInterface::ShowMessage("PlotInterface::CreateGlPlotWindow() "
        //                              "Creating MdiGlPlot::mdiParentGlFrame\n");
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
    //loj: 3/8/04 added if
    if (MdiGlPlot::mdiParentGlFrame->mainSubframe == NULL)
    {
        //MessageInterface::ShowMessage("PlotInterface::CreateGlPlotWindow() "
        //                              "Creating MdiGlPlot::mdiParentGlFrame->mainSubframe\n");
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
    }
    
    MdiGlPlot::mdiParentGlFrame->Show(true);
    MdiGlPlot::mdiParentGlFrame->UpdateUI();

    return true;
#endif
}

//loj: 3/8/04 added
//------------------------------------------------------------------------------
//  delete DeleteGlPlot()
//------------------------------------------------------------------------------
/*
 * Deletes OpenGlPlot
 *
 */
//------------------------------------------------------------------------------
bool PlotInterface::DeleteGlPlot()
{    
#if defined __CONSOLE_APP__
    return true;
#else

    if (MdiGlPlot::mdiParentGlFrame != NULL)
    {
        //MessageInterface::ShowMessage("PlotInterface::DeleteGlPlot() "
        //                              "Creating MdiGlPlot::mdiParentGlFrame\n");
        
        for (int i=0; i<MdiGlPlot::numChildren; i++)
        {
            ((MdiChildTrajFrame*)MdiGlPlot::mdiChildren[i])->DeletePlot();
        }

        MdiGlPlot::mdiParentGlFrame->Hide();
    }

    return true;
#endif
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
#if defined __CONSOLE_APP__
    return true;
#else

    if (MdiGlPlot::mdiParentGlFrame != NULL)
    {
                
        if (MdiGlPlot::numChildren > 0 &&
            MdiGlPlot::mdiParentGlFrame->mainSubframe != NULL)
        {
            //MessageInterface::ShowMessage("PlotInterface::UpdateGlSpacecraft()\n");
            //MdiGlPlot::mdiParentGlFrame->mainSubframe->SetFocus();
            MdiGlPlot::mdiParentGlFrame->mainSubframe->
                UpdateSpacecraft(time, posX, posY, posZ, updateCanvas);
        }
        else
        {
            if (!CreateGlPlotWindow())
                return false;
            
            //MdiGlPlot::mdiParentGlFrame->mainSubframe->SetFocus();
            MdiGlPlot::mdiParentGlFrame->mainSubframe->
                UpdateSpacecraft(time, posX, posY, posZ, updateCanvas);
        }
        
        return true;
    }
    else
    {
        //wxLogWarning("MdiParentGlFrame was not created. Creating a new MDI parent/child frame...");
        //wxLog::FlushActive();

        if (!CreateGlPlotWindow())
            return false;
        
        MdiGlPlot::mdiParentGlFrame->Show(true);
        
        // Update spacecraft trajectory (for now just one spacecraft)
        // later there will be UpdateUniverse()
        MdiGlPlot::mdiParentGlFrame->mainSubframe->
            UpdateSpacecraft(time, posX, posY, posZ, updateCanvas);
       
        return true;
    }

    return false;
#endif
} // end UpdateGlSpacecraft()


//------------------------------------------------------------------------------
//  bool CreateXyPlotWindow(const std::string &plotName,
//                          const std::string &plotTitle,
//                          const std::string &xAxisTitle,
//                          const std::string &yAxisTitle)
//------------------------------------------------------------------------------
/*
 * Creates XyPlot window.
 *
 * @param <plotName> name of plot
 */
//------------------------------------------------------------------------------
bool PlotInterface::CreateXyPlotWindow(const std::string &plotName,
                                       const std::string &plotTitle,
                                       const std::string &xAxisTitle,
                                       const std::string &yAxisTitle)
{    
#if defined __CONSOLE_APP__
    return true;
#else

    if (MdiXyPlot::mdiParentXyFrame == NULL)
    {
        //MessageInterface::ShowMessage("PlotInterface::CreateXyPlotWindow() "
        //                              "Creating MdiXyPlot::mdiParentXyFrame\n");
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

    //loj: 3/11/04 added for multiple xyplot frame
    bool createNewFrame = true;
    for (int i=0; i<MdiXyPlot::numChildren; i++)
    {
        wxString currPlotName = ((MdiChildXyFrame*)MdiXyPlot::mdiChildren[i])->GetPlotName();

        if (currPlotName.IsSameAs(plotName.c_str()))
        {
            createNewFrame = false;
            break;
        }
    }
    
    //loj: 3/8/04 added if
    //if (MdiXyPlot::mdiParentXyFrame->mainSubframe == NULL)
    if (createNewFrame)
    {
        
        //MessageInterface::ShowMessage("PlotInterface::CreateXyPlotWindow() Creating"
        //                              "MdiXyPlot::mdiParentXyFrame->mainSubframe\n"
        //                              "X Axis Title = %s  Y Axis Title = %s\n",
        //                              xAxisTitle.c_str(), yAxisTitle.c_str());

        // create a frame, containing a XY plot canvas
        MdiXyPlot::mdiParentXyFrame->mainSubframe =
            new MdiChildXyFrame(MdiXyPlot::mdiParentXyFrame, true,
                                wxString(plotName.c_str()),
                                wxString(plotTitle.c_str()),
                                wxString(xAxisTitle.c_str()),
                                wxString(yAxisTitle.c_str()),
                                wxPoint(-1, -1), wxSize(500, 350), //loj: 2/27/04 wxSize(-1, -1),
                                wxDEFAULT_FRAME_STYLE);
    
        ++MdiXyPlot::numChildren;
        //loj:3/11/04 MdiXyPlot::mdiParentXyFrame->mainSubframe->SetTitle(_T("Main XY Plot"));

        MdiXyPlot::mdiParentXyFrame->mainSubframe->RedrawCurve();
    }
    
    MdiXyPlot::mdiParentXyFrame->Show(true);
    
    return true;
#endif
}

//loj: 3/8/04 added
//------------------------------------------------------------------------------
//  bool DeleteXyPlot(bool hideFrame)
//------------------------------------------------------------------------------
/*
 * Deletes XyPlot.
 *
 */
//------------------------------------------------------------------------------
bool PlotInterface::DeleteXyPlot(bool hideFrame)
{    
#if defined __CONSOLE_APP__
    return true;
#else

    if (MdiXyPlot::mdiParentXyFrame != NULL)
    {
        //MessageInterface::ShowMessage("PlotInterface::DeleteXyPlot()\n");
        
        for (int i=0; i<MdiXyPlot::numChildren; i++)
        {
            ((MdiChildXyFrame*)MdiXyPlot::mdiChildren[i])->DeletePlot();
        }

        if (hideFrame)
            MdiXyPlot::mdiParentXyFrame->Hide();
    }
    
    return true;
#endif
}

//------------------------------------------------------------------------------
// bool AddXyPlotCurve(const std::string &plotName, int curveIndex,
//                     int yOffset, Real yMin, Real yMax,
//                     const std::string &curveTitle,
//                     const std::string &penColor)
//------------------------------------------------------------------------------
/*
 * Adds a plot curve to XY plow window.
 */
//------------------------------------------------------------------------------
bool PlotInterface::AddXyPlotCurve(const std::string &plotName, int curveIndex,
                                   int yOffset, Real yMin, Real yMax,
                                   const std::string &curveTitle,
                                   const std::string &penColor)
{
#if defined __CONSOLE_APP__
    return true;
#else

    bool added = false;
    
    //MessageInterface::ShowMessage("PlotInterface::AddXyPlotCurve() entered."
    //                              " plotName = " + plotName + " curveTitle = " + 
    //                              curveTitle + "\n");
    
    //MessageInterface::ShowMessage("PlotInterface::AddXyPlotCurve() numChildren = %d\n",
    //                              MdiXyPlot::numChildren);
    
    for (int i=0; i<MdiXyPlot::numChildren; i++)
    {
        MdiChildXyFrame *frame = (MdiChildXyFrame*)(MdiXyPlot::mdiChildren[i]);
        
        //if (frame->GetPlotName() == wxString(plotName.c_str()))
        if (frame->GetPlotName().IsSameAs(plotName.c_str()))
        {
            frame->AddPlotCurve(curveIndex, yOffset, yMin, yMax,
                                wxString(curveTitle.c_str()),
                                wxString(penColor.c_str()));
            added = true;
        }
    }

    return added;
#endif
}

//------------------------------------------------------------------------------
// bool DeleteAllXyPlotCurves(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Deletes all plot curves in XY plow window.
 */
//------------------------------------------------------------------------------
bool PlotInterface::DeleteAllXyPlotCurves(const std::string &plotName)
{
#if defined __CONSOLE_APP__
    return true;
#else

    //MessageInterface::ShowMessage("PlotInterface::DeleteAllPlotCurve() plotName = %s "
    //                              "numChildren = %d\n", plotName.c_str(),
    //                              MdiXyPlot::numChildren);
        
    for (int i=0; i<MdiXyPlot::numChildren; i++)
    {
        MdiChildXyFrame *frame = (MdiChildXyFrame*)(MdiXyPlot::mdiChildren[i]);
        if (frame->GetPlotName().IsSameAs(plotName.c_str()))
            frame->DeleteAllPlotCurves();
    }

    return true;
#endif
}

//------------------------------------------------------------------------------
// bool DeleteXyPlotCurve(const std::string &plotName, int curveIndex)
//------------------------------------------------------------------------------
/*
 * Deletes a plot curve to XY plow window.
 */
//------------------------------------------------------------------------------
bool PlotInterface::DeleteXyPlotCurve(const std::string &plotName, int curveIndex)
{
#if defined __CONSOLE_APP__
    return true;
#else

    //MessageInterface::ShowMessage("PlotInterface::DeleteXyPlotCurve() entered "
    //                              " plotName = %s curveIndex = %d\n", plotName.c_str(),
    //                              curveIndex);
    
    //MessageInterface::ShowMessage("PlotInterface::DeleteXyPlotCurve() numChildren = %d\n",
    //                              MdiXyPlot::numChildren);
    
    for (int i=0; i<MdiXyPlot::numChildren; i++)
    {
        MdiChildXyFrame *frame = (MdiChildXyFrame*)(MdiXyPlot::mdiChildren[i]);

        if (frame->GetPlotName().IsSameAs(plotName.c_str()))
        {
            frame->DeletePlotCurve(curveIndex);
        }
    }

    return true;
#endif
}

//------------------------------------------------------------------------------
// void ClearXyPlotData(const std::string &plotName))
//------------------------------------------------------------------------------
void PlotInterface::ClearXyPlotData(const std::string &plotName)
{
#if defined __CONSOLE_APP__
    return;
#else

    for (int i=0; i<MdiXyPlot::numChildren; i++)
    {
        MdiChildXyFrame *frame = (MdiChildXyFrame*)(MdiXyPlot::mdiChildren[i]);

        if (frame->GetPlotName().IsSameAs(plotName.c_str()))
        {
            frame->ClearPlotData();
        }
    }
#endif
}

//------------------------------------------------------------------------------
// void SetXyPlotTitle(const std::string &plotName, const std::string &plotTitle)
//------------------------------------------------------------------------------
void PlotInterface::SetXyPlotTitle(const std::string &plotName,
                                   const std::string &plotTitle)
{
#if defined __CONSOLE_APP__
    return;
#else

    //MessageInterface::ShowMessage("PlotInterface::SetXyPlotTitle() plotName = %s "
    //                              "plotTitle = %s\n", plotName.c_str(), plotTitle.c_str());
    
    for (int i=0; i<MdiXyPlot::numChildren; i++)
    {
        MdiChildXyFrame *frame = (MdiChildXyFrame*)(MdiXyPlot::mdiChildren[i]);

        if (frame->GetPlotName().IsSameAs(plotName.c_str()))
        {
            //MessageInterface::ShowMessage("PlotInterface::SetXyPlotTitle() calling "
            //                              " frame->SetPlotTitle() \n");
            frame->SetPlotTitle(wxString(plotTitle.c_str()));
        }
    }
#endif
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
#if defined __CONSOLE_APP__
    return true;
#else

    bool updated = false;
    wxString owner = wxString(plotName.c_str());
    
    if (MdiXyPlot::mdiParentXyFrame == NULL)
    {        
        //wxLogWarning("MdiParentXyFrame was not created. Creating a new MDI parent/child frame...");
        //wxLog::FlushActive();
        CreateXyPlotWindow(plotName, plotTitle, xAxisTitle, yAxisTitle);
        //MessageInterface::ShowMessage("PlotInterface::UpdateXyPlot()" + plotName + " " +
        //                              plotTitle + " " + xAxisTitle + " " + yAxisTitle + "\n");
    }

        
    //MessageInterface::ShowMessage("PlotInterface::UpdateXyPlot() numChildren = %d\n",
    //                              MdiXyPlot::numChildren);
    
    for (int i=0; i<MdiXyPlot::numChildren; i++)
    {
        MdiChildXyFrame *frame = (MdiChildXyFrame*)(MdiXyPlot::mdiChildren[i]);
            
        if (frame->GetPlotName().IsSameAs(owner.c_str()))
        {
            int numCurves = frame->GetCurveCount();
            
            //MessageInterface::ShowMessage("PlotInterface::UpdateXyPlot() numCurves = %d\n",
            //                              numCurves);
            
            for (int j=0; j<numCurves; j++)
            {
                //MessageInterface::ShowMessage("PlotInterface::UpdateXyPlot() yvals[%d] = %f\n",
                //                              j, yvals(j));
                
                frame->AddDataPoints(j, xval, yvals(j));
            }

            if (updateCanvas)
                frame->RedrawCurve();
            
            updated = true;
        }
    }
    
    return updated;
#endif
}
//================================================
//#endif
//================================================
