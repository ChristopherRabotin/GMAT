//$Header$
//------------------------------------------------------------------------------
//                              GmatDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/02
//
/**
 * Declares GmatDialog class. This class provides base class of GMAT Dialogs.
 */
//------------------------------------------------------------------------------

#ifndef GmatDialog_hpp
#define GmatDialog_hpp

#include "gmatwxdefs.hpp"
#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/grid.h>
#include "wx/radiobut.h"

#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

class GmatDialog : public wxDialog
{
public:
    // constructors
    GmatDialog( wxWindow *parent, wxWindowID id, const wxString& title);
    
protected:
    // member functions
    virtual void Show();
    virtual void Create() = 0;
    virtual void LoadData() = 0;
    virtual void SaveData() = 0;
    virtual void ResetData() = 0;
    
    virtual void OnOK();
    virtual void OnCancel();
    virtual void OnHelp();

    // member data
    GuiInterpreter *theGuiInterpreter;
    GuiItemManager *theGuiManager;
    wxWindow *theParent;
    
    wxBoxSizer *theDialogSizer;
    wxStaticBoxSizer *theMiddleSizer;
    wxStaticBoxSizer *theBottomSizer;
    
    wxButton *theOkButton;
    wxButton *theCancelButton;
    wxButton *theHelpButton;
    
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        ID_BUTTON_OK = 8100,
        ID_BUTTON_APPLY,
        ID_BUTTON_CANCEL,
        ID_BUTTON_HELP
    };

};

#endif // GmatDialog_hpp
