//$Header$
//------------------------------------------------------------------------------
//                              FormationSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/02/12
/**
 * Implements FormationSetupPanel class.
 */
//------------------------------------------------------------------------------
#include "FormationSetupPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"

#include "MessageInterface.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(FormationSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
    
   EVT_BUTTON(ADD_VAR_BUTTON, FormationSetupPanel::OnAddSpacecraft)
   EVT_BUTTON(REMOVE_VAR_BUTTON, FormationSetupPanel::OnRemoveSpacecraft)
   EVT_BUTTON(CLEAR_VAR_BUTTON, FormationSetupPanel::OnClearSpacecraft)

END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
    
//------------------------------------------------------------------------------
// FormationSetupPanel(wxWindow *parent, const wxString &subscriberName)
//------------------------------------------------------------------------------
/**
 * Constructs FormationSetupPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the FormationSetupPanel GUI
 */
//------------------------------------------------------------------------------
FormationSetupPanel::FormationSetupPanel(wxWindow *parent,
                                   const wxString &formationName)
    : GmatPanel(parent)
{
    Create();
    Show();
    theApplyButton->Disable();
}

//-------------------------------
// protected methods
//-------------------------------

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * @param <scName> input spacecraft name.
 *
 * @note Creates the notebook for spacecraft information
 */
//------------------------------------------------------------------------------
void FormationSetupPanel::Create()
{
    //MessageInterface::ShowMessage("FormationSetupPanel::Create() entering...\n");

    Integer bsize = 3; // border size
    wxString emptyList[] = {};

    wxBoxSizer *variablesBoxSizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer *mFlexGridSizer = new wxFlexGridSizer(5, 0, 0);
           
   //------------------------------------------------------
   // available Spacecraft list (1st column)
   //------------------------------------------------------
   mVarBoxSizer = new wxBoxSizer(wxVERTICAL);
    
   wxStaticText *titleAvailable =
      new wxStaticText(this, -1, wxT("Space Objects"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxArrayString empty;
   
   mVarListBox = 
       theGuiManager->GetSpaceObjectListBox(this, -1, wxSize(150, 200), empty);
    
   mVarBoxSizer->Add(titleAvailable, 0, wxALIGN_CENTRE|wxALL, bsize);
   mVarBoxSizer->Add(mVarListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   //------------------------------------------------------
   // add, remove, clear parameter buttons (2nd column)
   //------------------------------------------------------
   wxButton *addScButton = new wxButton(this, ADD_VAR_BUTTON, wxT("-->"),
                              wxDefaultPosition, wxSize(20,20), 0);

   wxButton *removeScButton = new wxButton(this, REMOVE_VAR_BUTTON, wxT("<--"),
                                 wxDefaultPosition, wxSize(20,20), 0);
    
   wxButton *clearScButton = new wxButton(this, CLEAR_VAR_BUTTON, wxT("<="),
                                wxDefaultPosition, wxSize(20,20), 0);
    
   wxBoxSizer *arrowButtonsBoxSizer = new wxBoxSizer(wxVERTICAL);
   arrowButtonsBoxSizer->Add(addScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsBoxSizer->Add(removeScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsBoxSizer->Add(clearScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   //------------------------------------------------------
   // selected spacecraft list (4th column)
   //------------------------------------------------------
   wxStaticText *titleSelected =
      new wxStaticText(this, -1, wxT("Selected Space Objects"),
                       wxDefaultPosition, wxSize(-1,-1), 0);

   mVarSelectedListBox =
      new wxListBox(this, VAR_SEL_LISTBOX, wxDefaultPosition,
                    wxSize(150,200), 0, emptyList, wxLB_SINGLE);
        
   wxBoxSizer *mVarSelectedBoxSizer = new wxBoxSizer(wxVERTICAL);
   mVarSelectedBoxSizer->Add(titleSelected, 0, wxALIGN_CENTRE|wxALL, bsize);
   mVarSelectedBoxSizer->Add(mVarSelectedListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   //------------------------------------------------------
   // put in the order
   //------------------------------------------------------    
   mFlexGridSizer->Add(mVarBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(arrowButtonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(mVarSelectedBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   variablesBoxSizer->Add(mFlexGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

    //------------------------------------------------------
    // add to parent sizer
    //------------------------------------------------------
    theMiddleSizer->Add(variablesBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void FormationSetupPanel::LoadData()
{

}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void FormationSetupPanel::SaveData()
{

}

//------------------------------------------------------------------------------
// void OnAddSpacecraft(wxCommandEvent& event)
//------------------------------------------------------------------------------
void FormationSetupPanel::OnAddSpacecraft(wxCommandEvent& event)
{
   // get string in first list and then search for it
   // in the second list
   wxString s = mVarListBox->GetStringSelection();
   int found = mVarSelectedListBox->FindString(s);
    
   // if the string wasn't found in the second list, insert it
   if (found == wxNOT_FOUND)
   {
      mVarSelectedListBox->Append(s);
      mVarSelectedListBox->SetStringSelection(s);
      theApplyButton->Enable();
   }
}

//------------------------------------------------------------------------------
// void OnRemoveSpacecraft(wxCommandEvent& event)
//------------------------------------------------------------------------------
void FormationSetupPanel::OnRemoveSpacecraft(wxCommandEvent& event)
{
   int sel = mVarSelectedListBox->GetSelection();
   mVarSelectedListBox->Delete(sel);

   if (sel-1 < 0)
   {
      mVarSelectedListBox->SetSelection(0);
   }
   else
   {
      mVarSelectedListBox->SetSelection(sel-1);
   }
   
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnClearSpacecraft(wxCommandEvent& event)
//------------------------------------------------------------------------------
void FormationSetupPanel::OnClearSpacecraft(wxCommandEvent& event)
{
   mVarSelectedListBox->Clear();
   theApplyButton->Enable();
}


