//$Header$
//------------------------------------------------------------------------------
//                              ImpulsiveBurnSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: LaMont Ruley
// Created: 2004/02/04
//
/**
 * This class contains the Impulsive Burn Setup window.
 */
//------------------------------------------------------------------------------

#ifndef ImpulsiveBurnSetupPanel_hpp
#define ImpulsiveBurnSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "Burn.hpp"

class ImpulsiveBurnSetupPanel : public wxPanel
{
public:
    // constructors
    ImpulsiveBurnSetupPanel( wxWindow *parent, const wxString &burnName);
    
private:
    // member functions
    void CreateBurn(wxWindow *parent, const wxString &burnName);

    void AddVector(wxWindow *parent);
    void OnVectorChange();
    void OnTextChange();
    void UpdateValues();

    void OnScript();
    void OnApply();
    void OnOK();
    void OnCancel();

    // member data
    GuiInterpreter *theGuiInterpreter;
    Burn *theBurn;

    wxButton *scriptButton;
    wxButton *okButton;
    wxButton *applyButton;
    wxButton *cancelButton;
    wxButton *helpButton;

    wxComboBox *vectorCB;
    wxComboBox *coordCB;
    
    wxPanel *vectorPanel;

    wxStaticText *description1;
    wxStaticText *description2;
    wxStaticText *description3;
    wxStaticText *label1;
    wxStaticText *label2;
    wxStaticText *label3;
    
    wxTextCtrl *textCtrl1;
    wxTextCtrl *textCtrl2;
    wxTextCtrl *textCtrl3;

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 81000,
        ID_TEXTCTRL,

        ID_CB_COORD,
        ID_CB_VECTOR,

        ID_STATIC_COORD,
        ID_STATIC_VECTOR,

        ID_BUTTON_SCRIPT,
        ID_BUTTON_OK,
        ID_BUTTON_APPLY,
        ID_BUTTON_CANCEL,
        ID_BUTTON_HELP

    };
};

#endif // ImpulsiveBurnSetupPanel_hpp
