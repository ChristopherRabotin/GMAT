//$Header$
//------------------------------------------------------------------------------
//                              PropagateCommandPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/09/08
//
/**
 * This class contains the Propagator command setup window.
 */
//------------------------------------------------------------------------------

#ifndef PropagateCommandPanel_hpp
#define PropagateCommandPanel_hpp

// gui includes
#include <wx/window.h>
#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/grid.h>
#include <wx/docview.h>
#include <wx/menu.h>
#include <wx/variant.h>

#include "gmatwxdefs.hpp"
#include "ViewTextFrame.hpp"
#include "DocViewFrame.hpp"
#include "TextEditView.hpp"
#include "TextDocument.hpp"
#include "MdiTextEditView.hpp"
#include "MdiDocViewFrame.hpp"
#include "GmatAppData.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "Command.hpp"
#include "Propagate.hpp"
#include "StopCondition.hpp"

class PropagateCommandPanel : public wxPanel
{
public:
    // constructors
    //PropagateCommandPanel( wxWindow *parent, GmatCommand *propCommand );
    //loj: 2/9/04 PropagateCommandPanel( wxWindow *parent, const wxString &propName );
    PropagateCommandPanel( wxWindow *parent, const wxString &propName, GmatCommand *cmd);
    
private:
    wxStaticText *synchStaticText;
    //wxStaticText *valueStaticText;
    wxStaticText *varStaticText;
    wxStaticText *repeatStaticText;
    wxStaticText *tolStaticText;
    wxStaticText *descStaticText;
        
    wxTextCtrl *valueTextCtrl;
    wxTextCtrl *repeatTextCtrl;
    wxTextCtrl *toleranceTextCtrl;
    wxTextCtrl *descriptionTextCtrl;
    wxTextCtrl *variableTextCtrl;

    wxButton *scriptButton;
    wxButton *userDefButton;
    wxButton *editButton;
    wxButton *okButton;
    wxButton *applyButton;
    wxButton *cancelButton;
    wxButton *helpButton;

    wxComboBox *synchComboBox;
    wxComboBox *equalityComboBox;

    wxGrid *propGrid;
    wxGrid *stopCondGrid;
    
    wxDocManager *mDocManager;
    wxDocTemplate *mDocTemplate;
    ViewTextFrame *mTextFrame;
    
    Integer numOfModes;
    Integer numOfPropRows;
    Integer numOfCondRows;
    Integer numOfEqualities;
    Integer numOfProp;
    Integer numOfSC;
    Integer propID;
    Integer scID;
    
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

    // Layout & data handling methods
    void Initialize();
    void Setup(wxWindow *parent);
    void GetData();
    void SetData();
    
    // Text control event method
    void OnTextUpdate(wxCommandEvent& event);
    void OnTextMaxLen(wxCommandEvent& event);
    
    // Combobox event method
    void OnComboSelection(wxCommandEvent& event);

    // Button event methods
    void OnButton(wxCommandEvent& event);
    void CreateScript();
    wxMenuBar* CreateScriptWindowMenu(const std::string &docType);
    
    // Popup menu event methods
    // waw: Future implementation
    //void ShowContextMenu(const wxPoint& pos);

    // Grid table event methods
    // waw: Future implementation
    //void OnCellValueChanged(wxGridEvent& event);
    //void OnCellRightClick(wxGridEvent& ev, wxMouseEvent& event);
    //void OnCellLeftClick(wxGridEvent& event);
    //void OnCellLeftDoubleClick(wxGridEvent& event);
    //void InsertPropagatorRow();
    //void InsertStopCondRow();
    //void ClearPropagatorTable();
    //void ClearStopCondTable();
    //void DeleteSelectedPropagatorRows();
    //void DeleteSelectedStopCondRows();
                
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

