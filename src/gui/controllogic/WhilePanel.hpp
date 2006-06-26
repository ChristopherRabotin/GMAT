//$Header: 
//------------------------------------------------------------------------------
//                              WhilePanel
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

#ifndef WhilePanel_hpp
#define WhilePanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GmatCommand.hpp"
#include "While.hpp"
#include "Parameter.hpp"

class WhilePanel : public GmatPanel
{
public:
    // constructors
    WhilePanel(wxWindow *parent, GmatCommand *cmd);
    ~WhilePanel();  

private:
    wxGrid *conditionGrid;

    static const int MAX_ROW = 10;
    static const int MAX_COL = 4;
    static const int COMMAND_COL = 0;
    static const int LHS_COL = 1;
    static const int COND_COL = 2;
    static const int RHS_COL = 3;
    
    While *theWhileCommand;
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
    void OnCellRightClick(wxGridEvent& event);
    void OnCellValueChange(wxGridEvent& event);
 
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        ID_GRID = 50000,
    };
      
   Integer mNumberOfConditions;
   Integer mNumberOfLogicalOps;
   
   std::vector<bool> mLhsIsParam;
   std::vector<bool> mRhsIsParam;

   wxArrayString mObjectTypeList;
   StringArray mLogicalOpStrings;
   StringArray mLhsList;
   StringArray mEqualityOpStrings;
   StringArray mRhsList;   
};

#endif // WhilePanel_hpp
