//$Header$
//------------------------------------------------------------------------------
//                              MdiParentXyFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2004/01/21
/**
 * Declares MdiParentXyFrame for XY plot.
 */
//------------------------------------------------------------------------------
#ifndef MdiParentXyFrame_hpp
#define MdiParentXyFrame_hpp

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "wx/wx.h"
#include "wx/mdi.h"

class MdiChildXyFrame;

// Define a new frame
class MdiParentXyFrame : public wxMDIParentFrame
{
public:
    wxTextCtrl *textWindow;
    MdiChildXyFrame *subframe;
    MdiChildXyFrame *mainSubframe;

    MdiParentXyFrame(wxWindow *parent, const wxWindowID id,
                     const wxString& title, const wxPoint& pos,
                     const wxSize& size, const long style);
    ~MdiParentXyFrame();
    
    void OnSize(wxSizeEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnOpenXyPlotFile(wxCommandEvent& event);

protected:
    void InitToolBar(wxToolBar* toolBar);
    
    DECLARE_EVENT_TABLE()
};
#endif
