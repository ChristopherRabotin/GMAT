//------------------------------------------------------------------------------
//                           DCSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/01/21
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
//
/**
 * This class contains the Solver Create window.
 */
//------------------------------------------------------------------------------

#ifndef DCSetupPanel_hpp
#define DCSetupPanel_hpp

// gui includes
#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/notebook.h>

#include "gmatwxdefs.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"

class DCSetupPanel : public wxPanel
{
public:
    // constructors
    DCSetupPanel(wxWindow *parent);
       
private:   
    // Main notebook
    wxNotebook *solverNotebook;
    
    wxPanel *dcPanel;
    wxPanel *bmPanel;
    wxPanel *qnPanel;
    
    wxButton *okButton;
    wxButton *applyButton;
    wxButton *cancelButton;
    wxButton *helpButton;
    
    // DC 
    wxStaticText *nameStaticText;
    wxStaticText *maxStaticText;
    
    wxTextCtrl *nameTextCtrl;
    wxTextCtrl *maxTextCtrl;
    
    GuiInterpreter *theGuiInterpreter;
   
    // Layout & data handling methods
    void Setup(wxWindow *parent);
    void SetupDC(wxWindow *parent);
    void Initialize();
    void GetData();
    void SetData();
    
    // Text control event method
    void OnTextUpdate(wxCommandEvent& event);
    
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
        ID_NOTEBOOK
    };
};

#endif // SolverEvent_hpp
