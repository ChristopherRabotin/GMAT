//$Header$
//------------------------------------------------------------------------------
//                              ConditionalStatementPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/09/08
// Modified: 2004/05/06 by Allison Greene to inherit from GmatPanel
/**
 * This class contains the Propagator command setup window.
 */
//------------------------------------------------------------------------------

#ifndef ConditionalStatementPanel_hpp
#define ConditionalStatementPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GmatPanel.hpp"

class ConditionalStatementPanel : public GmatPanel
{
public:
    // constructors
    ConditionalStatementPanel( wxWindow *parent, const wxString &condition);
    ~ConditionalStatementPanel();  

private:
    wxString conditionStatement;
    wxGrid *conditionGrid;

    // methods inherited from GmatPanel
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
    //loj: 2/27/04 commented out
    //virtual void OnHelp();
    //virtual void OnScript();

    // Layout & data handling methods
    void Setup(wxWindow *parent);

    // Grid table event methods
    void OnCellLeftClick(wxGridEvent& event);
    void OnCellRightClick(wxGridEvent& event);
                
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 44000,
        ID_TEXTCTRL,
        ID_COMBO,
        ID_BUTTON,
        ID_CHECKBOX,
        ID_GRID,
        MENU_INSERT_P,
        MENU_DELETE_P,
        MENU_CLEAR_P
    };
};

#endif // ConditionalStatementPanel_hpp

