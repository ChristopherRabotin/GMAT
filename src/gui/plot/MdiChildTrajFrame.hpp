//$Header$
//------------------------------------------------------------------------------
//                              MdiChildTrajFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2003/11/25
/**
 * Declares MdiChildTrajFrame for opengl plot.
 */
//------------------------------------------------------------------------------
#ifndef MdiChildTrajFrame_hpp
#define MdiChildTrajFrame_hpp

#include "TrajPlotCanvas.hpp"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/mdi.h"
#endif

class MdiChildTrajFrame: public wxMDIChildFrame
{
public:
    TrajPlotCanvas *mCanvas;
    int mViewMenuIndex;

    MdiChildTrajFrame(wxMDIParentFrame *parent, const wxString& title,
                      const wxPoint& pos, const wxSize& size, const long style);
    ~MdiChildTrajFrame();

    void OnChangeTitle(wxCommandEvent& event);
    void OnShowDefaultView(wxCommandEvent& event);
    void OnZoomIn(wxCommandEvent& event);
    void OnZoomOut(wxCommandEvent& event);
    void OnShowWireFrame(wxCommandEvent& event);
    void OnShowEquatorialPlane(wxCommandEvent& event);
    void OnHelpView(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    
    void OnActivate(wxActivateEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMove(wxMoveEvent& event);
    void OnClose(wxCloseEvent& event);

protected:
    
    DECLARE_EVENT_TABLE()
};
#endif
