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
//  bool CreatePlotWindow(bool canvasOnly)
//------------------------------------------------------------------------------
bool PlotInterface::CreatePlotWindow(bool canvasOnly)
{    
#if !defined __CONSOLE_APP__
    if (!canvasOnly)
    {
        MdiGlPlot::mdiParentGlFrame =
            new MdiParentGlFrame((wxFrame *)NULL, -1, _T("MDI OpenGL Window"),
                                 wxPoint(300, 200), wxSize(600, 500),
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
    
    ++MdiGlPlot::numChildFrames;
    
    MdiGlPlot::mdiParentGlFrame->mainSubframe->SetTitle(_T("Main 3D Plot"));

    // initialize GL
    if (!MdiGlPlot::mdiParentGlFrame->mainSubframe->mCanvas->InitGL())
    {
        wxMessageDialog msgDialog(MdiGlPlot::mdiParentGlFrame,
                                  _T("InitGL() failed"), _T("CreatePlotWindow"));
        msgDialog.ShowModal();
        return false;
    }

    MdiGlPlot::mdiParentGlFrame->UpdateUI();
    
#endif
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
    if (MdiGlPlot::mdiParentGlFrame != NULL)
    {
        if (MdiGlPlot::numChildFrames > 0 &&
            MdiGlPlot::mdiParentGlFrame->mainSubframe != NULL)
        {
            MdiGlPlot::mdiParentGlFrame->mainSubframe->SetFocus();
            MdiGlPlot::mdiParentGlFrame->mainSubframe->
                UpdateSpacecraft(time, posX, posY, posZ);
        }
        else
        {
            if (!CreatePlotWindow(true))
                return false;
            
            MdiGlPlot::mdiParentGlFrame->mainSubframe->SetFocus();
            MdiGlPlot::mdiParentGlFrame->mainSubframe->
                UpdateSpacecraft(time, posX, posY, posZ);
        }
        
        return true;
    }
    else
    {
        //wxLogWarning("PlotWindow was not created. Creating a new PlotWindow...");
        //wxLog::FlushActive();

        if (!CreatePlotWindow(false))
            return false;
        
        MdiGlPlot::mdiParentGlFrame->Show(true);
        
        // Update spacecraft trajectory (for now just one spacecraft)
        // later there will be UpdateUniverse()
        MdiGlPlot::mdiParentGlFrame->mainSubframe->
            UpdateSpacecraft(time, posX, posY, posZ);
       
        return true;
    }
#endif
    return true;  // Must return something for console apps -- is this what we want?
} // end UpdateSpacecraft()

