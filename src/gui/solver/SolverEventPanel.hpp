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

// base include
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"

class SolverEventPanel : public wxPanel
{
public:
    // constructors
    SolverEventPanel(wxWindow *parent);
       
private:   
    wxStaticText *item2;
    wxStaticText *item6;
    wxStaticText *item8;
    wxStaticText *item10;
    
    wxTextCtrl *item3;
    
    wxButton *item4;
    wxButton *item12;
    wxButton *item14;
    wxButton *item15;
    wxButton *item16 ;
    wxButton *item17;
    
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
