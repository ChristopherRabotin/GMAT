//$Header$
//------------------------------------------------------------------------------
//                              GmatPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/02
//
/**
 * Declares GmatPanel class.
 */
//------------------------------------------------------------------------------

#ifndef GmatPanel_hpp
#define GmatPanel_hpp

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"

#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include "wx/notebook.h"
#include <wx/button.h>
#include <wx/grid.h>
#include "wx/radiobut.h"

class GmatPanel : public wxPanel
{
public:
    // constructors
    GmatPanel( wxWindow *parent);
    
protected:
    // member functions
    virtual void Create(wxWindow *parent) = 0;
    virtual void LoadData() = 0;
    virtual void SaveData() = 0;
    
    virtual void OnApply();
    virtual void OnOK();
    virtual void OnCancel();
    virtual void OnHelp();
    virtual void OnScript();

    // member data
    GuiInterpreter *theGuiInterpreter;

    wxFlexGridSizer *thePanelSizer;
    wxGridSizer *theTopSizer;
    wxBoxSizer *theMiddleSizer;
    wxBoxSizer *theBottomSizer;

    wxButton *theOkButton;
    wxButton *theApplyButton;
    wxButton *theCancelButton;
    wxButton *theHelpButton;
    wxButton *theScriptButton;
    
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        ID_BUTTON_OK = 8000,
        ID_BUTTON_APPLY,
        ID_BUTTON_CANCEL,
        ID_BUTTON_HELP,
        ID_BUTTON_SCRIPT,
    };

};

#endif // GmatPanel_hpp
