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
// Created: 2004/02/10
/**
 * Implements ParameterSetupPanel class.
 */
//------------------------------------------------------------------------------
#include "ParameterSetupPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "GuiItemManager.hpp"

#include "MessageInterface.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(ParameterSetupPanel, wxPanel)
    EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
    EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
    EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
    EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
    EVT_BUTTON(PARAMETER_ADD_PROPERTY, ParameterSetupPanel::OnAddProperty)
    EVT_BUTTON(PARAMETER_REMOVE, ParameterSetupPanel::OnRemoveParameter)
    EVT_BUTTON(PARAMETER_RESTORE, ParameterSetupPanel::OnRestoreParameter)
    EVT_BUTTON(PARAMETER_REMOVE_ALL, ParameterSetupPanel::OnRemoveAllParameters)
    EVT_BUTTON(PARAMETER_RESTORE_ALL, ParameterSetupPanel::OnRestoreAllParameters)
    EVT_BUTTON(PARAMETER_SC_COMBOBOX, ParameterSetupPanel::OnScComboBoxEvent)
    
    EVT_COMBOBOX(PARAMETER_SC_COMBOBOX, ParameterSetupPanel::OnScComboBoxEvent)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
//------------------------------------------------------------------------------
// ParameterSetupPanel(wxWindow *parent, const wxString &subscriberName)
//------------------------------------------------------------------------------
/**
 * Constructs ParameterSetupPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the ParameterSetupPanel GUI
 */
//------------------------------------------------------------------------------
ParameterSetupPanel::ParameterSetupPanel(wxWindow *parent,
                                         const wxString &subscriberName)
    : GmatPanel(parent)
{
    theParent = parent;
    theSubscriber =
        theGuiInterpreter->GetSubscriber(std::string(subscriberName.c_str()));

    theGuiManager = GuiItemManager::GetInstance();
    theParamList = NULL;

    //MessageInterface::ShowMessage("ParameterSetupPanel{} creating the panel...\n");
    Create(this);
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnAddProperty(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSetupPanel::OnAddProperty(wxCommandEvent& event)
{
    wxString s = scComboBox->GetStringSelection() + "." +
        propertyListBox->GetStringSelection();
    paramListBox->Append(s);    
}

//------------------------------------------------------------------------------
// void OnRemoveParameter(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSetupPanel::OnRemoveParameter(wxCommandEvent& event)
{
    int sel = paramListBox->GetSelection();
    wxString str = paramListBox->GetStringSelection();

    paramListBox->Delete(sel);
    trashListBox->Append(str);
}

//------------------------------------------------------------------------------
// void OnRestoreParameter(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSetupPanel::OnRestoreParameter(wxCommandEvent& event)
{
    int sel = trashListBox->GetSelection();
    wxString str = trashListBox->GetStringSelection();
    
    trashListBox->Delete(sel);
    paramListBox->Append(str);
}

//------------------------------------------------------------------------------
// void OnRemoveAllParameters(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSetupPanel::OnRemoveAllParameters(wxCommandEvent& event)
{
    int count = paramListBox->GetCount();
    wxString str;

    for(int i=0; i<count; i++)
    {
        str = paramListBox->GetString(i);
        trashListBox->Append(str);
    }
    
    paramListBox->Clear();
}

//------------------------------------------------------------------------------
// void OnRestoreAllParameters(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSetupPanel::OnRestoreAllParameters(wxCommandEvent& event)
{
    int count = trashListBox->GetCount();
    wxString str;

    for(int i=0; i<count; i++)
    {
        str = trashListBox->GetString(i);
        paramListBox->Append(str);
    }
    
    trashListBox->Clear();
}

//------------------------------------------------------------------------------
// void OnScComboBoxEvent(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSetupPanel::OnScComboBoxEvent(wxCommandEvent& event)
{    
    theGuiManager->UpdateParameter(scComboBox->GetStringSelection());
    
    propertyListBox->Set(theGuiManager->GetNumParameter(),
                         theGuiManager->GetParameterList());
    
}

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * @param <parent> input parent.
 * @param <scName> input spacecraft name.
 *
 * @note Creates the notebook for spacecraft information
 */
//------------------------------------------------------------------------------
void ParameterSetupPanel::Create(wxWindow *parent)
{
    unsigned int i;
    wxString emptyList[] = {};

    pageBoxSizer = new wxBoxSizer(wxVERTICAL);
    paramGridSizer = new wxFlexGridSizer(6, 0, 0);

    // empty text for spacing line
    emptyText = new wxStaticText(parent, PARAMETER_TEXT, wxT(""),
                                 wxDefaultPosition, wxSize(100, -1), 0);
    
    //------------------------------------------------------
    // select object (1st column)
    //------------------------------------------------------
    
    // spacecraft label
    titleScText = new wxStaticText(parent, PARAMETER_TEXT, wxT("Spacecraft"),
                                   wxDefaultPosition, wxSize(100, -1), 0);
    
    theGuiManager->UpdateSpacecraft();
    
    // get spacecraft ComboBox
    scComboBox = theGuiManager->GetSpacecraftComboBox(parent, PARAMETER_SC_COMBOBOX,
                                                      wxSize(100, -1));
        
    objectBoxSizer = new wxBoxSizer(wxVERTICAL);
    objectBoxSizer->Add(titleScText, 0, wxALIGN_LEFT|wxALL, 5);
    objectBoxSizer->Add(scComboBox, 0, wxALIGN_LEFT|wxALL, 5);
        
    //------------------------------------------------------
    // Property label (2nd column)
    //------------------------------------------------------
    propertyBoxSizer = new wxBoxSizer(wxVERTICAL);
    
    titlePropertyText = new wxStaticText(parent, PARAMETER_TEXT, wxT("Property"),
                                         wxDefaultPosition, wxSize(80,-1), 0);
    
    theGuiManager->UpdateParameter(scComboBox->GetStringSelection());
    
    propertyListBox =
        theGuiManager->GetParameterListBox(parent, wxSize(140,125),
                                           scComboBox->GetStringSelection(),
                                           theGuiManager->GetNumSpacecraft());
    
    propertyBoxSizer->Add(titlePropertyText, 0, wxALIGN_CENTRE|wxALL, 5);
    propertyBoxSizer->Add(propertyListBox, 0, wxALIGN_CENTRE|wxALL, 5);

    
    //------------------------------------------------------
    // add, property buttons (3rd column)
    //------------------------------------------------------
    addPropertyButton = new wxButton(parent, PARAMETER_ADD_PROPERTY, wxT("-->"),
                                     wxDefaultPosition, wxSize(20,20), 0);

    propertButtonsBoxSizer = new wxBoxSizer(wxVERTICAL);
    //loj: 2/10/04 propertButtonsBoxSizer->Add(30, 20, 0, wxALIGN_CENTRE|wxALL, 5);
    propertButtonsBoxSizer->Add(addPropertyButton, 0, wxALIGN_CENTRE|wxALL, 5);
      
    //------------------------------------------------------
    // Parameter box (4nd column)
    //------------------------------------------------------
    titleParamText = new wxStaticText(parent, PARAMETER_TEXT, wxT("Variables"),
                                      wxDefaultPosition, wxSize(80,-1), 0);
    
    theGuiManager->UpdateConfigParameter();
    
    paramListBox =
        theGuiManager->GetConfigParameterListBox(parent, wxSize(140,125));
           
    paramBoxSizer = new wxBoxSizer(wxVERTICAL);
    paramBoxSizer->Add(titleParamText, 0, wxALIGN_CENTRE|wxALL, 5);
    paramBoxSizer->Add(paramListBox, 0, wxALIGN_CENTRE|wxALL, 5);
            
    //------------------------------------------------------
    // add, remove, clear Parameter buttons (5th column)
    //------------------------------------------------------
    removeParamButton = new wxButton(parent, PARAMETER_REMOVE, wxT("-->"),
                                     wxDefaultPosition, wxSize(20,20), 0);

    restoreParamButton = new wxButton(parent, PARAMETER_RESTORE, wxT("<--"),
                                      wxDefaultPosition, wxSize(20,20), 0);
    
    removeAllParamButton = new wxButton(parent, PARAMETER_REMOVE_ALL, wxT("=>"),
                                        wxDefaultPosition, wxSize(20,20), 0);
    
    restoreAllParamButton = new wxButton(parent, PARAMETER_RESTORE_ALL, wxT("<="),
                                         wxDefaultPosition, wxSize(20,20), 0);
    
    paramButtonsBoxSizer = new wxBoxSizer(wxVERTICAL);
    paramButtonsBoxSizer->Add(removeParamButton, 0, wxALIGN_CENTRE|wxALL, 5);
    paramButtonsBoxSizer->Add(restoreParamButton, 0, wxALIGN_CENTRE|wxALL, 5);
    paramButtonsBoxSizer->Add(removeAllParamButton, 0, wxALIGN_CENTRE|wxALL, 5);
    paramButtonsBoxSizer->Add(restoreAllParamButton, 0, wxALIGN_CENTRE|wxALL, 5);
    
    //------------------------------------------------------
    // Trash label (6th column)
    //------------------------------------------------------
    titleTrashText = new wxStaticText(parent, PARAMETER_TEXT, wxT("Trash"),
                                      wxDefaultPosition, wxSize(80,-1), 0);

    trashListBox = new wxListBox(parent, PARAMETER_LISTBOX, wxDefaultPosition,
                                 wxSize(140,125), 0, emptyList, wxLB_SINGLE);
    
    trashBoxSizer = new wxBoxSizer(wxVERTICAL);
    trashBoxSizer->Add(titleTrashText, 0, wxALIGN_CENTRE|wxALL, 5);
    trashBoxSizer->Add(trashListBox, 0, wxALIGN_CENTRE|wxALL, 5);

    //------------------------------------------------------
    // Ok, Apply, Cancel, Help buttons
    //------------------------------------------------------
    //loj: recreate buttons here until GmatPanel works!!
    theOkButton = new wxButton(parent, ID_BUTTON_OK, "OK", 
                               wxDefaultPosition, wxDefaultSize, 0);
    theApplyButton = new wxButton(parent, ID_BUTTON_APPLY, "Apply", 
                                  wxDefaultPosition, wxDefaultSize, 0);
    theCancelButton = new wxButton(parent, ID_BUTTON_CANCEL, "Cancel", 
                                   wxDefaultPosition, wxDefaultSize, 0);
    theHelpButton = new wxButton(parent, ID_BUTTON_HELP, "Help", 
                                 wxDefaultPosition, wxDefaultSize, 0);
    
    bottomSizer = new wxBoxSizer(wxHORIZONTAL);
    
    // adds the buttons to button sizer    
    bottomSizer->Add(theOkButton, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);
    bottomSizer->Add(theApplyButton, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);
    bottomSizer->Add(theCancelButton, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);
    bottomSizer->Add(theHelpButton, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);
    
    //------------------------------------------------------
    // put in the order
    //------------------------------------------------------
    paramGridSizer->Add(objectBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    paramGridSizer->Add(propertyBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    paramGridSizer->Add(propertButtonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    paramGridSizer->Add(paramBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    paramGridSizer->Add(paramButtonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    paramGridSizer->Add(trashBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    
    pageBoxSizer->Add(paramGridSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    pageBoxSizer->Add(bottomSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    
    //------------------------------------------------------
    // add to parent sizer
    //------------------------------------------------------
    theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    Show();
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ParameterSetupPanel::LoadData()
{
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ParameterSetupPanel::SaveData()
{
    // save data
}

//------------------------------------------------------------------------------
// virtual void OnHelp()
//------------------------------------------------------------------------------
void ParameterSetupPanel::OnHelp()
{
    // open the window
    GmatPanel::OnHelp();

    // fill help text
}

//------------------------------------------------------------------------------
// virtual void OnScript()
//------------------------------------------------------------------------------
void ParameterSetupPanel::OnScript()
{
    // open the window
    GmatPanel::OnScript();

    // fill scripts
}

