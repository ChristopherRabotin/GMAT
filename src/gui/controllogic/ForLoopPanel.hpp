//$Header$
//------------------------------------------------------------------------------
//                              ForLoopPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2004/05/17
/**
 * This class contains the Conditional Statement Setup window.
 */
//------------------------------------------------------------------------------
#ifndef ForLoopPanel_hpp
#define ForLoopPanel_hpp

// gui includes
#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"

// base includes
#include "gmatdefs.hpp"
#include "Command.hpp"
#include "For.hpp"

class ForLoopPanel : public GmatPanel
{
public:
    // constructors
    ForLoopPanel(wxWindow *parent, GmatCommand *cmd);
    ~ForLoopPanel();  

private:
    For *theForCommand;
    
    Real mForName;
    Real mStartValue;
    Real mStepSize;
    Real mEndValue;    
    
    wxStaticText *indexStaticText;
    wxStaticText *startStaticText;
    wxStaticText *stepStaticText;
    wxStaticText *endStaticText;
   
    wxTextCtrl *indexTextCtrl;
    wxTextCtrl *startTextCtrl;
    wxTextCtrl *stepTextCtrl;
    wxTextCtrl *endTextCtrl;
   
    // methods inherited from GmatPanel
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();

    // Layout & data handling methods
    void Setup(wxWindow *parent);
    
    void OnTextUpdate(wxCommandEvent& event); 
               
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXTCTRL = 46000,
        ID_TEXT 
    };
};

#endif // ForLoopPanel_hpp
