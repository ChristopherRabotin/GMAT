#ifndef SQPSETUPPANEL_HPP_
#define SQPSETUPPANEL_HPP_

#include "gmatwxdefs.hpp"

#include <wx/variant.h>

// base includes
#include "GuiInterpreter.hpp"
#include "GmatPanel.hpp"
#include "Solver.hpp"

class SQPSetupPanel : public GmatPanel
{
public:
    // constructors
    SQPSetupPanel(wxWindow *parent, const wxString &name);
    ~SQPSetupPanel();  
       
private:     
    
    wxStaticText *displayStaticText;
    wxComboBox *displayComboBox;
    
    wxCheckBox *checkBox1;
    wxCheckBox *checkBox2;
    wxCheckBox *checkBox3;

    wxStaticText *specStaticText1;
    wxStaticText *specStaticText2;
    wxStaticText *specStaticText3;
    wxStaticText *specStaticText4;
    wxStaticText *specStaticText5;
    wxStaticText *specStaticText6;
    wxStaticText *specStaticText7;
    
    wxTextCtrl *specTextCtrl1;
    wxTextCtrl *specTextCtrl2;
    wxTextCtrl *specTextCtrl3;
    wxTextCtrl *specTextCtrl4;
    wxTextCtrl *specTextCtrl5;
    wxTextCtrl *specTextCtrl6;
    wxTextCtrl *specTextCtrl7;
    
    GuiInterpreter *theGuiInterpreter;
    Solver *theSolver;
    // pointer to the SQP solver; waiting on implementation
   
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
    
    static const wxString DISPLAY_SCHEMES[4];

    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 55000,
        ID_TEXTCTRL,
        ID_BUTTON,
        ID_COMBOBOX,
        ID_CHECKBOX,
        ID_NOTEBOOK
    };
};

#endif /*SQPSETUPPANEL_HPP_*/
