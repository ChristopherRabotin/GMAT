//$Header:
//------------------------------------------------------------------------------
//                              MSISE90Panel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/04/06
// Modified:
/**
 * This class allows user to edit MSISE90 drag parameters.
 */
//------------------------------------------------------------------------------

#ifndef MSISE90Panel_hpp
#define MSISE90Panel_hpp

#include "gmatwxdefs.hpp"
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "GmatDialog.hpp"

class MSISE90Panel : public GmatDialog
{
public:
    MSISE90Panel(wxWindow *parent, wxString name);
    ~MSISE90Panel();
    
private:   
    wxTextCtrl *msise90Drag1TextCtrl;
    wxTextCtrl *msise90Drag2TextCtrl;
    wxTextCtrl *msise90Drag3TextCtrl;
    wxTextCtrl *msise90Drag4TextCtrl;
    wxTextCtrl *msise90Drag5TextCtrl;
    wxTextCtrl *msise90Drag6TextCtrl;
    
    // Methods inherited from GmatDialog
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
    virtual void ResetData();
    
    // Event-handling Methods
    void OnTextChange();

    DECLARE_EVENT_TABLE();

    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 10003,
        ID_TEXTCTRL
    };
};

#endif

