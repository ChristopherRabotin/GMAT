//$Header$
//------------------------------------------------------------------------------
//                              MdiParentGlFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2003/11/25
/**
 * Declares MdiParentGlFrame for opengl plot.
 */
//------------------------------------------------------------------------------
#ifndef MdiParentGlFrame_hpp
#define MdiParentGlFrame_hpp

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "wx/wx.h"
#include "wx/mdi.h"

class MdiChildTrajFrame;

// Define a new frame
class MdiParentGlFrame : public wxMDIParentFrame
{
public:
    wxTextCtrl *textWindow;
    MdiChildTrajFrame *subframe;
    MdiChildTrajFrame *mainSubframe;

    MdiParentGlFrame(wxWindow *parent, const wxWindowID id,
                     const wxString& title, const wxPoint& pos,
                     const wxSize& size, const long style);

    void UpdateUI();
    void OnSize(wxSizeEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnOpenTrajectoryFile(wxCommandEvent& event);
    void OnZoomIn(wxCommandEvent& event);
    void OnZoomOut(wxCommandEvent& event);

protected:
    void InitToolBar(wxToolBar* toolBar);
    
    DECLARE_EVENT_TABLE()
};
#endif
