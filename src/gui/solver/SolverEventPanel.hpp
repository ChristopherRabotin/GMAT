//$Header$
//------------------------------------------------------------------------------
//                           SolverEventPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/12/16
//
/**
 * This class contains the Solver Event window.
 */
//------------------------------------------------------------------------------

#ifndef SolverEventPanel_hpp
#define SolverEventPanel_hpp

// gui includes
#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/button.h>

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"

class SolverEventPanel : public wxPanel
{
public:
    // constructor
    SolverEventPanel(wxWindow *parent, const wxString &name);
       
private:   
    wxStaticText *item6;
    wxStaticText *item8;
    wxStaticText *item10;
    
    wxButton *scriptButton;
    wxButton *correctionsButton;
    wxButton *okButton;
    wxButton *applyButton;
    wxButton *cancelButton ;
    wxButton *helpButton;
    
    wxComboBox *item7;
    wxComboBox *item9;
    wxComboBox *item11;
    
    GuiInterpreter *theGuiInterpreter;
   
    // Layout & data handling methods
    void Setup(wxWindow *parent);
    void Initialize();
    void GetData();
    void SetData();
    
    // Text control event method
    void OnTextUpdate(wxCommandEvent& event);
    
    // Combobox event method
    void OnComboSelection(wxCommandEvent& event);
    
    // Button event methods
    void OnButton(wxCommandEvent& event);

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 51000,
        ID_TEXTCTRL,
        ID_COMBO,
        ID_BUTTON,
    };
};

#endif // SolverEvent_hpp

