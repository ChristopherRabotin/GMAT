//$Header$
//------------------------------------------------------------------------------
//                              ParameterSelectDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Declares ParameterSelectDialog class. This class shows dialog window where a
 * user parameter can be selected.
 * 
 */
//------------------------------------------------------------------------------
#ifndef ParameterSelectDialog_hpp
#define ParameterSelectDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"

class ParameterSelectDialog : public GmatDialog
{
public:
    
    ParameterSelectDialog(wxWindow *parent);

    wxString GetParamName()
        { return mParamName; }
    bool IsParamSelected()
        { return mIsParamSelected; }
    
private:
    wxString mParamName;
     bool mIsParamSelected;

    wxButton *addParamButton;
    wxListBox *paramListBox;
    wxListBox *paramSelectedListBox;
    
    // methods inherited from GmatDialog
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
    virtual void ResetData();

    // event handling
    void OnButton(wxCommandEvent& event);
    
    DECLARE_EVENT_TABLE()
        
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 9300,
        ID_LISTBOX,
        ID_BUTTON,
    };
};

#endif
