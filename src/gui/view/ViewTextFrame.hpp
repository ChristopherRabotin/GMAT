//$Header$
//------------------------------------------------------------------------------
//                                 ViewTextFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/11/03
//
/**
 * Declares show text.
 */
//------------------------------------------------------------------------------
#ifndef ViewTextFrame_hpp
#define ViewTextFrame_hpp

#include "gmatwxdefs.hpp"

class ViewTextFrame: public wxFrame
{
public:
    // constructors
    ViewTextFrame(wxFrame *frame, const wxString& title, int x, int y, int w, int h);

    // operations
    void WriteText(const wxString& text) { mTextCtrl->WriteText(text); }

private:

    wxMenuBar *CreateMainMenu();

    DECLARE_EVENT_TABLE();
    // callbacks
    void OnClose(wxCommandEvent& event);
    void OnClear(wxCommandEvent& event);

    // IDs for the controls and the menu commands
    enum
    {
        VIEW_TEXT_CLEAR = 1,
        VIEW_TEXT_CLOSE
    };
    
    // just some place to put our messages in
    wxTextCtrl *mTextCtrl;
};

#endif
