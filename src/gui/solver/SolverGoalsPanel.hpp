//$Header$
//------------------------------------------------------------------------------
//                           SolverGoalsPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/01/20
//
/**
 * This class contains the Solver Goals window.
 */
//------------------------------------------------------------------------------

#ifndef SolverGoalsPanel_hpp
#define SolverGoalsPanel_hpp

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

class SolverGoalsPanel : public wxPanel
{
public:
    // constructors
    SolverGoalsPanel(wxWindow *parent);

private:             
    wxGrid *goalsGrid;
    
    wxStaticText *descStaticText;
    wxStaticText *solverStaticText;
    wxStaticText *varStaticText;
    wxStaticText *valueStaticText;
    wxStaticText *tolStaticText;
    
    wxTextCtrl *descTextCtrl;
    wxTextCtrl *varTextCtrl;
    wxTextCtrl *valueTextCtrl;
    wxTextCtrl *tolTextCtrl;
    
    wxComboBox *solverComboBox;
    
    wxButton *editButton;
    wxButton *updateButton;
    wxButton *okButton;
    wxButton *applyButton;
    wxButton *cancelButton;
    wxButton *helpButton;
    
    wxString solverString;
    wxString propertyString;
    wxString descriptionString;
    
    Integer numOfGoals;
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
        ID_TEXT = 52000,
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

