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
 * This class updates OpenGL canvas.
 */
//------------------------------------------------------------------------------
#if !defined __CONSOLE_APP__
#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "MdiParentGlFrame.hpp"
#include "MdiGlPlotData.hpp"
#include "MdiChildTrajFrame.hpp"
#endif

#include <iostream>                // for cout, endl
#include "PlotInterface.hpp"       // for PlotInterface functions

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
//  bool CreatePlotWindow()
//------------------------------------------------------------------------------
bool PlotInterface::CreatePlotWindow()
{
    MdiPlot::mdiParentGlFrame =
        new MdiParentGlFrame((wxFrame *)NULL, -1, _T("MDI OpenGL Window"),
                             wxPoint(300, 200), wxSize(600, 500),
                             wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);
    // Give it an icon
#ifdef __WXMSW__
    MdiPlot::mdiParentGlFrame->SetIcon(wxIcon(_T("mdi_icn")));
#else
    MdiPlot::mdiParentGlFrame->SetIcon(wxIcon( mondrian_xpm ));
#endif
        
    // create a frame, containing a opengl canvas
    MdiPlot::mdiParentGlFrame->subframe = 
        new MdiChildTrajFrame(MdiPlot::mdiParentGlFrame, _T("OpenGL Canvas Frame"),
                              wxPoint(-1, -1), wxSize(-1, -1),
                              wxDEFAULT_FRAME_STYLE);
        
    wxString title;
    title.Printf(_T("Canvas Frame %d"), ++MdiPlot::gs_nFrames);
    MdiPlot::mdiParentGlFrame->subframe->SetTitle(title);

    // initialize GL
    if (!MdiPlot::mdiParentGlFrame->subframe->mCanvas->InitGL())
    {
        wxMessageDialog msgDialog(MdiPlot::mdiParentGlFrame,
                                  _T("InitGL() failed"), _T("CreatePlotWindow"));
        msgDialog.ShowModal();
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
//  bool UpdateSpacecraft(const Real &time, const Real &posX,
//                        const Real &posY, const Real &posZ)
//------------------------------------------------------------------------------
bool PlotInterface::UpdateSpacecraft(const Real &time, const Real &posX,
                                     const Real &posY, const Real &posZ)
{   
#if !defined __CONSOLE_APP__
    if (MdiPlot::mdiParentGlFrame != NULL)
    {
        MdiPlot::mdiParentGlFrame->Show(true);
        MdiPlot::mdiParentGlFrame->subframe->mCanvas->UpdateSpacecraft(time, posX, posY, posZ);
        return true;
    }
    else
    {
        wxLogWarning("PlotWindow was not created. Creating a new PlotWindow...");
        wxLog::FlushActive();

        if (!CreatePlotWindow())
            return false;
        
        // Update spacecraft trajectory (for now just one spacecraft)
        // later there will be UpdateUniverse()
        MdiPlot::mdiParentGlFrame->subframe->mCanvas->UpdateSpacecraft(time, posX, posY, posZ);
        return true;
    }
#endif

} // end UpdateSpacecraft()

