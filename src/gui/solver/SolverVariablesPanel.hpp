//------------------------------------------------------------------------------
//                           SolverVariablesPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/21/01
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
//
/**
 * This class contains the Solver Variables window.
 */
//------------------------------------------------------------------------------

#ifndef SolverVariablesPanel_hpp
#define SolverVariablesPanel_hpp

// gui includes
#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/grid.h>

#include "gmatwxdefs.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"

class SolverVariablesPanel : public wxPanel
{
public:
    // constructors
    SolverVariablesPanel(wxWindow *parent);

private:             
    wxGrid *varsGrid;
    
    wxStaticText *descStaticText;
    wxStaticText *solverStaticText;
    wxStaticText *varStaticText;
    wxStaticText *pertStaticText;
    wxStaticText *maxStaticText;
    wxStaticText *lowerStaticText;
    wxStaticText *upperStaticText;
    
    wxTextCtrl *descTextCtrl;
    wxTextCtrl *varTextCtrl;
    wxTextCtrl *pertTextCtrl;
    wxTextCtrl *maxTextCtrl;
    wxTextCtrl *lowerTextCtrl;
    wxTextCtrl *upperTextCtrl;
    
    wxButton *editButton;
    wxButton *updateButton;
    wxButton *okButton;
    wxButton *applyButton;
    wxButton *cancelButton;
    wxButton *helpButton;
    
    wxComboBox *solverComboBox;
    
    wxString solverString;
    wxString propertyString;
    wxString descriptionString;
    
    Integer numOfVars;
    Integer nextRow;
        
    // Layout & data handling methods
    void Initialize();
    void Setup(wxWindow *parent);
    void GetData();
    void SetData();
    
    // Grid table event method
    void OnCellValueChanged();
    
    // Text control event method
    void OnTextUpdate(wxCommandEvent& event);    
    
    // Combobox event method
    void OnSolverSelection();
    
    // Button event methods
    void OnButton(wxCommandEvent& event);

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 10000,
        ID_TEXTCTRL,
        ID_BUTTON,
        ID_COMBO,
        ID_GRID,
        SOL_COL,
        PRO_COL,
        DES_COL
    };
};

#endif // SolverGoalsPanel_hpp
