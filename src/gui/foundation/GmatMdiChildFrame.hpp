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

#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

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
                        const wxString& name = "");
    ~GmatMdiChildFrame();
    wxString GetTitle();
    void OnClose();
   
protected:
    wxString title;
    
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();

    // IDs for the controls and the menu commands
    enum
    {     
    };

};

#endif // GmatMdiChildFrame_hpp
