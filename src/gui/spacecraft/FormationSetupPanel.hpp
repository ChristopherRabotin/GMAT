//$Header$
//------------------------------------------------------------------------------
//                              FormationSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/02/12
/**
 * Declares FormationSetupPanel class. This class allows user to setup OpenGL Plot.
 */
//------------------------------------------------------------------------------
#ifndef FormationSetupPanel_hpp
#define FormationSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "ReportFile.hpp"

class FormationSetupPanel: public GmatPanel
{
public:
    FormationSetupPanel(wxWindow *parent, const wxString &formationName);
   
protected:
   
    wxBoxSizer *mVarBoxSizer;
    wxListBox *mVarListBox;
    wxListBox *mVarSelectedListBox;

    void OnAddSpacecraft(wxCommandEvent& event);
    void OnRemoveSpacecraft(wxCommandEvent& event);
    void OnClearSpacecraft(wxCommandEvent& event);

    // methods inherited from GmatPanel
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
    
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        ADD_VAR_BUTTON = 99000,
        REMOVE_VAR_BUTTON,
        CLEAR_VAR_BUTTON,
        VAR_SEL_LISTBOX
    };
};
#endif
