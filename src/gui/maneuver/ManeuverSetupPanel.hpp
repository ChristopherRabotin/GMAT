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
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"
#include "Command.hpp"
#include "GuiItemManager.hpp"

class ManeuverSetupPanel : public GmatPanel
{
public:
    // constructors
    ManeuverSetupPanel(wxWindow *parent, GmatCommand *cmd);
    
protected:
    // member data
    GuiItemManager *theGuiManager; //loj: move this to GmatPanel later
    GmatCommand *theCommand;

    wxComboBox *burnCB;
    wxComboBox *satCB;
    
    // member functions
    void OnBurnComboBoxChange(wxCommandEvent& event);
    void OnSatComboBoxChange(wxCommandEvent& event);

    // methods inherited from GmatPanel
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 80000,
        
        ID_BURN_COMBOBOX,
        ID_SAT_COMBOBOX
    };
};

#endif // ManeuverSetupPanel_hpp
