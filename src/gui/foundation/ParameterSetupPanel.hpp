//$Header$
//------------------------------------------------------------------------------
//                              ParameterSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/02/02
/**
 * Declares ParameterSetupPanel class. This class allows user to setup XY Plot.
 */
//------------------------------------------------------------------------------
#ifndef ParameterSetupPanel_hpp
#define ParameterSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

class ParameterSetupPanel: public GmatPanel
{
public:
    ParameterSetupPanel(wxWindow *parent, const wxString &subscriberName);
   
protected:
    wxWindow *theParent;
    GuiItemManager *theGuiManager; //loj: move this to GmatPanel later
    Subscriber *theSubscriber;
    wxString *theParamList;
    
    wxBoxSizer *pageBoxSizer;
    wxFlexGridSizer *paramGridSizer;
    
    wxBoxSizer *objectBoxSizer;
    wxBoxSizer *propertyBoxSizer;
    wxBoxSizer *propertButtonsBoxSizer;
    wxBoxSizer *paramBoxSizer;
    wxBoxSizer *paramButtonsBoxSizer;
    wxBoxSizer *trashBoxSizer;
    wxBoxSizer *bottomSizer;
    
    wxStaticText *titleScText;
    wxStaticText *titlePropertyText;
    wxStaticText *titleParamText;
    wxStaticText *titleTrashText;
    wxStaticText *emptyText;
    
    wxListBox *paramListBox;
    wxListBox *propertyListBox;
    wxListBox *trashListBox;
    
    wxButton *addPropertyButton;
    wxButton *removeParamButton;
    wxButton *restoreParamButton;
    wxButton *removeAllParamButton;
    wxButton *restoreAllParamButton;

    wxButton *okButton;
    wxButton *applyButton;
    wxButton *cancelButton;
    wxButton *helpButton;
    wxButton *scriptButton;

    wxComboBox *scComboBox;

    void OnAddProperty(wxCommandEvent& event);
    void OnRemoveParameter(wxCommandEvent& event);
    void OnRestoreParameter(wxCommandEvent& event);
    void OnRemoveAllParameters(wxCommandEvent& event);
    void OnRestoreAllParameters(wxCommandEvent& event);
    void OnScComboBoxEvent(wxCommandEvent& event);

    // methods inherited from GmatPanel
    virtual void Create(wxWindow *parent);
    virtual void LoadData();
    virtual void SaveData();
    virtual void OnHelp();
    virtual void OnScript();
    
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        PARAMETER_TEXT = 9000,
        PARAMETER_LISTBOX,
        PARAMETER_ADD_PROPERTY,
        PARAMETER_REMOVE,
        PARAMETER_RESTORE,
        PARAMETER_REMOVE_ALL,
        PARAMETER_RESTORE_ALL,
        PARAMETER_SC_COMBOBOX
    };
};
#endif
