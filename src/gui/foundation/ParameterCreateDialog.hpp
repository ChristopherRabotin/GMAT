//$Header$
//------------------------------------------------------------------------------
//                              ParameterCreateDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Declares ParameterCreateDialog class. This class shows dialog window where a
 * user parameter can be created.
 * 
 */
//------------------------------------------------------------------------------
#ifndef ParameterCreateDialog_hpp
#define ParameterCreateDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"

class ParameterCreateDialog : public GmatDialog
{
public:
    
    ParameterCreateDialog(wxWindow *parent);

    wxString GetParamName()
        { return mParamName; }
    bool IsParamCreated()
        { return mIsParamCreated; }
    
private:
    wxString mParamName;
    bool mIsParamCreated;
    
    wxTextCtrl *nameTextCtrl;
    wxTextCtrl *expTextCtrl;
    wxTextCtrl *epochTextCtrl;
    wxTextCtrl *indexTextCtrl;

    wxButton *addPropertyButton;
    wxButton *addParamButton;
    
    wxListBox *objectListBox;
    wxListBox *propertyListBox;
    wxListBox *parameterListBox;
    
    wxComboBox *cbodyComboBox;
    wxComboBox *coordComboBox;
    wxComboBox *rbodyComboBox;

    // methods inherited from GmatDialog
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
    virtual void ResetData();

    // event handling
    void OnTextUpdate(wxCommandEvent& event);
    void OnComboSelection(wxCommandEvent& event);
    void OnButton(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
        
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 9200,
        ID_LISTBOX,
        ID_BUTTON,
        ID_COMBO,
        ID_TEXTCTRL
    };
};

#endif
