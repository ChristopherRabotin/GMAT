//$Header$
//------------------------------------------------------------------------------
//                              ManeuverSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: LaMont Ruley
// Created: 2003/12/01
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
//
/**
 * This class contains the Maneuver Setup window.
 */
//------------------------------------------------------------------------------

#ifndef ManeuverSetupPanel_hpp
#define ManeuverSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "Command.hpp"
#include "Burn.hpp"

class ManeuverSetupPanel : public wxPanel
{
public:
    // constructors
    ManeuverSetupPanel(wxWindow *parent, GmatCommand *cmd);
    
private:
    // member functions
    void CreateManeuver(wxWindow *parent);

    void OnScript();
    void OnApply();
    void OnOK();
    void OnCancel();

    // member data
    GuiInterpreter *theGuiInterpreter;
    GmatCommand *theCommand;
    Burn *theBurn;

    wxButton *okButton;
    wxButton *applyButton;
    wxButton *cancelButton;
    wxButton *helpButton;
    wxButton *scriptButton;

    wxComboBox *burnCB;
    wxComboBox *vectorCB;
    wxComboBox *coordCB;
    wxComboBox *satCB;
    
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 80000,
        ID_TEXTCTRL,

        ID_BUTTON_OK,
        ID_BUTTON_APPLY,
        ID_BUTTON_CANCEL,
        ID_BUTTON_HELP,
        ID_BUTTON_SCRIPT,

        ID_COMBO,

        ID_STATIC_VECTOR

    };
};

#endif // ManeuverSetupPanel_hpp
