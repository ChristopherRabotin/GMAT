//$Header$
//------------------------------------------------------------------------------
//                              PropagateCommandPanel
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

#ifndef PropagateCommandPanel_hpp
#define PropagateCommandPanel_hpp

#include <wx/variant.h>

#include "gmatwxdefs.hpp"
//loj: 3/1/04 commented out for build2
//#include "ViewTextFrame.hpp"
//#include "DocViewFrame.hpp"
//#include "TextEditView.hpp"
//#include "TextDocument.hpp"
//#include "MdiTextEditView.hpp"
//#include "MdiDocViewFrame.hpp"
#include "GmatAppData.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "Command.hpp"
#include "Propagate.hpp"
#include "StopCondition.hpp"
#include "GmatPanel.hpp"

class PropagateCommandPanel : public GmatPanel
{
public:
    // constructors
    PropagateCommandPanel( wxWindow *parent, const wxString &propName, 
                           GmatCommand *cmd);
    ~PropagateCommandPanel();  

private:

    static const int MAX_PROP_ROW = 10;
    static const int MAX_STOPCOND_ROW = 20;
    static const int PROP_NAME_COL = 0;
    static const int PROP_SCS_COL = 1;
    static const int STOPCOND_NAME_COL = 0;
    static const int STOPCOND_DESC_COL = 1;

    struct PropType
    {
        bool isChanged;
        wxString propName;
        wxString scNames;
        PropSetup *propSetupPtr;
    };
    
    struct StopCondType
    {
        bool           isChanged;
        wxString       name;
        wxString       desc;
        wxString       varName;
        wxString       typeName;
        wxString       relOpStr;
        Real           goal;
        Real           tol;
        Integer        repeat;
        StopCondition *stopCondPtr;
    };
    
    wxStaticText *synchStaticText;
    wxStaticText *varStaticText;
    wxStaticText *repeatStaticText;
    wxStaticText *tolStaticText;
    wxStaticText *nameStaticText;
        
    wxTextCtrl *nameTextCtrl;
    wxTextCtrl *variableTextCtrl;
    wxTextCtrl *goalTextCtrl;
    wxTextCtrl *toleranceTextCtrl;
    wxTextCtrl *repeatTextCtrl;

    wxButton *updateButton;
    wxButton *deleteButton;
    wxButton *viewButton;

    wxComboBox *synchComboBox;
    wxComboBox *equalityComboBox;

    wxGrid *propGrid;
    wxGrid *stopCondGrid;

    //loj: 3/1/04 commented out for build2
//      wxDocManager *mDocManager;
//      wxDocTemplate *mDocTemplate;
//      ViewTextFrame *mTextFrame;
    
    Integer numOfModes;
    Integer numOfEqualities;
    Integer numOfProp;
    Integer numOfSC;
    Integer propID;
    Integer scID;
    Integer numOfStopCond;
    Integer currentRowStopCond;
    Integer currentRowProp;
    Integer currentRow;
    PropType tempProp[MAX_PROP_ROW];
    StopCondType tempStopCond[MAX_STOPCOND_ROW];
        
    std::string propSetupName;
    wxString propNameString;
    wxString scListString;
    wxString newParamName;
    StringArray scList;
    
    GuiInterpreter *theGuiInterpreter;
    GmatCommand    *theCommand;
    Propagate      *thePropagateCommand;
    GmatBase       *theStopCondBase;
    StopCondition  *theStopCond;
    PropSetup      *thePropSetup;
    
    //std::vector<StopCondition *> theStopCondVector;

    // methods inherited from GmatPanel
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
    //loj: 2/27/04 commented out
    //virtual void OnHelp();
    //virtual void OnScript();

    // Layout & data handling methods
    void Initialize();
    void Setup(wxWindow *parent);
    void DisplayPropagator();
    void DisplayStopCondition();
    void UpdateStopCondition();
    void ClearDetailedStopCond();
    void ShowDetailedStopCond(int row);
    wxString FormatStopCondDesc(const wxString &varName,
                                const wxString &relOpStr,
                                Real &goal);
    
    // Text control event method
    void OnTextUpdate(wxCommandEvent& event);
    void OnTextMaxLen(wxCommandEvent& event);
    
    // Combobox event method
    void OnComboSelection(wxCommandEvent& event);

    // Button event methods
    void OnButton(wxCommandEvent& event);
    //loj: 3/1/04 commented out for build2
    //void CreateScript();
    //wxMenuBar* CreateScriptWindowMenu(const std::string &docType);

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

#endif // PropagateCommandPanel_hpp

