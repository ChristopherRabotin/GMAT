//$Header$
//------------------------------------------------------------------------------
//                              VariableCreatePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/12/10
//
/**
 * This class contains the Variable Create window.
 */
//------------------------------------------------------------------------------

#ifndef VariableCreatePanel_hpp
#define VariableCreatePanel_hpp

#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/docview.h>
#include <wx/menu.h>

//#include <wx/statline.h>
//#include <wx/spinbutt.h>
//#include <wx/spinctrl.h>
//#include <wx/splitter.h>
//#include <wx/listctrl.h>

#include "gmatwxdefs.hpp"
#include "ViewTextFrame.hpp"
#include "DocViewFrame.hpp"
#include "TextEditView.hpp"
#include "TextDocument.hpp"
#include "MdiTextEditView.hpp"
#include "MdiDocViewFrame.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GmatAppData.hpp"

class VariableCreatePanel: public wxPanel
{
public:
    // constructors
    VariableCreatePanel(wxWindow *parent, const wxString &name); 
    
private:   
    wxStaticText *objStaticText;
    wxStaticText *propStaticText;
    wxStaticText *cbodyStaticText;
    wxStaticText *coordStaticText;
    wxStaticText *rbodyStaticText;
    wxStaticText *epochStaticText;
    wxStaticText *indexStaticText;
    
    wxTextCtrl *epochTextCtrl;
    wxTextCtrl *indexTextCtrl;
    
    wxButton *scriptButton;
    wxButton *okButton;
    wxButton *applyButton;
    wxButton *cancelButton;
    wxButton *helpButton;
   
    wxListBox *objListBox;
    wxListBox *propListBox;
    
    wxComboBox *cbodyComboBox;
    wxComboBox *coordComboBox;
    wxComboBox *rbodyComboBox;
    
    wxDocManager *mDocManager;
    wxDocTemplate *mDocTemplate;
    ViewTextFrame *mTextFrame;
 
    // Layout & data handling methods
    void Setup(wxWindow *parent);
    void Initialize();
    void GetData();
    void SetData();
    void CreateScript();

    wxMenuBar* CreateScriptWindowMenu(const std::string &docType);

    // Text control event method
    void OnTextUpdate(wxCommandEvent& event);
    
    // Combobox event method
    void OnComboSelection(wxCommandEvent& event);

    // Button event methods
    void OnButton(wxCommandEvent& event);

    DECLARE_EVENT_TABLE();

    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 54000,
        ID_LISTBOX,
        ID_BUTTON,
        ID_COMBO,
        ID_TEXTCTRL
    };
};

#endif


