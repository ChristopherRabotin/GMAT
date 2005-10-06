//$Header$
//------------------------------------------------------------------------------
//                              GmatMdiChildFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/02
//
/**
 * Declares GmatMdiChildFrame class.
 */
//------------------------------------------------------------------------------

#ifndef GmatMdiChildFrame_hpp
#define GmatMdiChildFrame_hpp

#include "gmatwxdefs.hpp"

class GmatMdiChildFrame : public wxMDIChildFrame
{
public:
    // constructors
    GmatMdiChildFrame( wxMDIParentFrame* parent, 
                        wxWindowID id = -1, 
                        const wxString& title = "", 
                        const wxPoint& pos = wxDefaultPosition, 
                        const wxSize& size = wxDefaultSize, 
                        long style = wxDEFAULT_FRAME_STYLE, 
                        const wxString& name = "",
                        const int type = 20000);
    ~GmatMdiChildFrame();
#ifdef __WXMAC__
    wxString GetTitle();
    void SetTitle(wxString newTitle);
#endif
    int GetDataType();
    void SetDataType(int type) {dataType = type;};
    void OnClose(wxCloseEvent &event);
    wxMenuBar *CreateMenu(int dataType);
    void SetScriptTextCtrl(wxTextCtrl *scriptTC) {theScriptTextCtrl = scriptTC;};
    wxTextCtrl *GetScriptTextCtrl(){return theScriptTextCtrl;};

protected:
#ifdef __WXMAC__
    wxString title;
#endif

    int dataType;
    wxTextCtrl *theScriptTextCtrl;
    wxMenu *mViewMenu;
    wxMenu *mViewOptionMenu;
    
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();

    // IDs for the controls and the menu commands
    enum
    {     
    };

};

#endif // GmatMdiChildFrame_hpp
