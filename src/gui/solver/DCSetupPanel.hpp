//$Header$
//------------------------------------------------------------------------------
//                           DCSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/01/21
// Modified: 2004/05/06 by Allison Greene to inherit from GmatPanel
/**
 * This class contains the Solver Create window.
 */
//------------------------------------------------------------------------------

#ifndef DCSetupPanel_hpp
#define DCSetupPanel_hpp

#include "gmatwxdefs.hpp"

#include <wx/variant.h>

// base includes
#include "GuiInterpreter.hpp"
#include "GmatPanel.hpp"
#include "Solver.hpp"
#include "DifferentialCorrector.hpp"

class DCSetupPanel : public GmatPanel
{
public:
    // constructors
    DCSetupPanel(wxWindow *parent, const wxString &name);
    ~DCSetupPanel();  
       
private:     
    wxStaticText *maxStaticText;
    
    wxTextCtrl *maxTextCtrl;
    
    Integer maxIteration;
    Integer maxIterationID;
    
    GuiInterpreter *theGuiInterpreter;
    Solver *theSolver;
    DifferentialCorrector *theDC;
   
    // methods inherited from GmatPanel
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
    //loj: 2/27/04 commented out
    //virtual void OnHelp();
    //virtual void OnScript();
    
    // Layout 
    void Setup(wxWindow *parent);
    
    // Text control event method
    void OnTextUpdate(wxCommandEvent& event);
    
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 55000,
        ID_TEXTCTRL,
        ID_BUTTON,
        ID_NOTEBOOK
    };
};

#endif // SolverEvent_hpp
