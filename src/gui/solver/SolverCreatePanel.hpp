//------------------------------------------------------------------------------
//                           SolverCreatePanel
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

#ifndef SolverCreatePanel_hpp
#define SolverCreatePanel_hpp

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

class SolverCreatePanel : public wxPanel
{
public:
    // constructors
    SolverCreatePanel(wxWindow *parent);
       
private:   
    
    GuiInterpreter *theGuiInterpreter;
   
    // Layout & data handling methods
    void Setup(wxWindow *parent);
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
    };
};

#endif // SolverEvent_hpp
