//$Header$
//------------------------------------------------------------------------------
//                              VariableCreatePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/12/10
// Modified: 2004/05/06 by Allison Greene to inherit from GmatPanel
/**
 * This class contains the Variable Create window.
 */
//------------------------------------------------------------------------------

#ifndef VariableCreatePanel_hpp
#define VariableCreatePanel_hpp

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
#include "GmatPanel.hpp"

class VariableCreatePanel: public GmatPanel
{
public:
    // constructors
    VariableCreatePanel(wxWindow *parent, const wxString &name); 
    ~VariableCreatePanel(); 
    
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
    
    wxListBox *objListBox;
    wxListBox *propListBox;
    
    wxComboBox *cbodyComboBox;
    wxComboBox *coordComboBox;
    wxComboBox *rbodyComboBox;
    
    wxDocManager *mDocManager;
    wxDocTemplate *mDocTemplate;
    ViewTextFrame *mTextFrame;
 
     // methods inherited from GmatPanel
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
    //loj: 2/27/04 commented out
    //virtual void OnHelp();
    //virtual void OnScript();

    // Layout & data handling methods
    void Setup(wxWindow *parent);
    void Initialize();
//    void CreateScript();

    wxMenuBar* CreateScriptWindowMenu(const std::string &docType);

    // Text control event method
    void OnTextUpdate(wxCommandEvent& event);
    
    // Combobox event method
    void OnComboSelection(wxCommandEvent& event);

    // Button event methods
//    void OnButton(wxCommandEvent& event);

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


