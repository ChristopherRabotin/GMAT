//$Header: 
//------------------------------------------------------------------------------
//                              IfPanel
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

#ifndef IfPanel_hpp
#define IfPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"

// base includes
#include "gmatdefs.hpp"
#include "Command.hpp"
#include "If.hpp"
#include "Parameter.hpp"

class IfPanel : public GmatPanel
{
public:
    // constructors
    IfPanel(wxWindow *parent, GmatCommand *cmd);
    ~IfPanel();  

private:
    wxGrid *conditionGrid;

    static const int COMMAND_COL = 0;
    static const int LHS_COL = 1;
    static const int COND_COL = 2;
    static const int RHS_COL = 3;
    
    If *theIfCommand;
    Parameter *theParameter;
    
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
    void OnCellDoubleLeftClick(wxGridEvent& event);
    void OnCellValueChange(wxGridEvent& event);
                
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
    
    bool newCommand;
    
   Integer mNumberOfConditions;
   Integer mNumberOfLogicalOps;
   
   StringArray mLhsList;
   StringArray mOpStrings;
   StringArray mRhsList;
   StringArray mLogicalOpStrings;
};

#endif // IfPanel_hpp
