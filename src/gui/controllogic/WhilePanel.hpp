//$Header: 
//------------------------------------------------------------------------------
//                              WhilePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: 
// Created: 
// Modified: 
/**
 * 
 */
//------------------------------------------------------------------------------

#ifndef WhilePanel_hpp
#define WhilePanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GmatPanel.hpp"

class WhilePanel : public GmatPanel
{
public:
    // constructors
    WhilePanel( wxWindow *parent );
    ~WhilePanel();  

private:
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
//    void OnCellRightClick(wxGridEvent& event);
    void OnCellDoubleLeftClick(wxGridEvent& event);
                
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 44000,
        ID_BUTTON,
        ID_GRID,
        MENU_INSERT_P,
        MENU_DELETE_P,
        MENU_CLEAR_P
    };
};

#endif // WhilePanel_hpp


