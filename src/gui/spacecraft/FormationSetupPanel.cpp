//$Id$
//------------------------------------------------------------------------------
//                              FormationSetupPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/02/12
// Modified: 
//    2010.03.22 Thomas Grubb 
//      - Read tooltips from configuration file (GMAT.ini)
//      - Added tooltips & accelerator keys
//      - Added GmatStaticBoxSizers group boxes
//      - Added ability to add and remove spacecraft via double-click
/**
 * Implements FormationSetupPanel class.
 */
//------------------------------------------------------------------------------
#include "FormationSetupPanel.hpp"
#include "GmatAppData.hpp"
#include "ResourceTree.hpp"
#include "FormationInterface.hpp"
#include "MessageInterface.hpp"
#include "GmatStaticBoxSizer.hpp"
#include <wx/config.h>

// #define DEBUG_FORMATION 1

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(FormationSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)

   EVT_LISTBOX_DCLICK(AVL_LISTBOX, FormationSetupPanel::OnAddSpaceObject)
   EVT_LISTBOX_DCLICK(SEL_LISTBOX, FormationSetupPanel::OnRemoveSpaceObject)
   EVT_BUTTON(ADD_BUTTON, FormationSetupPanel::OnAddSpaceObject)
   EVT_BUTTON(REMOVE_BUTTON, FormationSetupPanel::OnRemoveSpaceObject)
   EVT_BUTTON(CLEAR_BUTTON, FormationSetupPanel::OnClearSpaceObject)
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
   mFormationName = std::string(formationName.c_str());
   Create();
   Show();
}


//------------------------------------------------------------------------------
// ~FormationSetupPanel()
//------------------------------------------------------------------------------
FormationSetupPanel::~FormationSetupPanel()
{
   theGuiManager->UnregisterListBox("SpaceObject", mSoAvailableListBox, &mSoExcList);
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
   #ifdef DEBUG_FORMATION
   MessageInterface::ShowMessage("FormationSetupPanel::Create() enters...\n");
   #endif

   Integer bsize = 3; // border size
   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;

   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Formation"));

   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   wxFlexGridSizer *mFlexGridSizer = new wxFlexGridSizer(5, 0, 0);
   
   //------------------------------------------------------
   // available SpaceObject list (1st column)
   //------------------------------------------------------
   GmatStaticBoxSizer *availableBoxSizer = new GmatStaticBoxSizer( wxVERTICAL, this, 
      wxT("Space"GUI_ACCEL_KEY"craft") );
   
   mSoExcList.Add(mFormationName.c_str());
   
   mSoAvailableListBox = 
      theGuiManager->GetSpaceObjectListBox(this, AVL_LISTBOX, wxSize(150, 200),
                                           &mSoExcList, false); //loj: 7/18/05 Added false
   mSoAvailableListBox->SetToolTip(pConfig->Read(_T("AvailableSpacecraftListHint")));
   
   availableBoxSizer->Add(mSoAvailableListBox, 0, wxALIGN_CENTRE|wxALL, bsize);

   //------------------------------------------------------
   // add, remove, clear parameter buttons (2nd column)
   //------------------------------------------------------
   wxButton *addScButton = new wxButton(this, ADD_BUTTON, wxT("--"GUI_ACCEL_KEY">"),
                               wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
   addScButton->SetToolTip(pConfig->Read(_T("AddSpacecraftHint")));
   
   wxButton *removeScButton = new wxButton(this, REMOVE_BUTTON, wxT(GUI_ACCEL_KEY"<--"),
                              wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
   removeScButton->SetToolTip(pConfig->Read(_T("RemoveSpacecraftHint")));
   
   wxButton *clearScButton = new wxButton(this, CLEAR_BUTTON, wxT("<"GUI_ACCEL_KEY"="),
                              wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
   clearScButton->SetToolTip(pConfig->Read(_T("ClearSpacecraftHint")));
   
   wxBoxSizer *arrowButtonsBoxSizer = new wxBoxSizer(wxVERTICAL);
   arrowButtonsBoxSizer->Add(addScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsBoxSizer->Add(removeScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsBoxSizer->Add(clearScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // selected spacecraft list (4th column)
   //------------------------------------------------------
   GmatStaticBoxSizer *mSoSelectedBoxSizer = new GmatStaticBoxSizer( wxVERTICAL, this, 
      wxT("Spacecraft in "GUI_ACCEL_KEY"Formation") );
   
   mSoSelectedListBox =
      new wxListBox(this, SEL_LISTBOX, wxDefaultPosition, wxSize(150,200), //0,
                    emptyList, wxLB_SINGLE);
   mSoSelectedListBox->SetToolTip(pConfig->Read(_T("SelectedSpacecraftListHint")));
   
   mSoSelectedBoxSizer->Add(mSoSelectedListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // put in the order
   //------------------------------------------------------    
   mFlexGridSizer->Add(availableBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(arrowButtonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(mSoSelectedBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   pageBoxSizer->Add(mFlexGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

   #ifdef DEBUG_FORMATION
   MessageInterface::ShowMessage("FormationSetupPanel::Create() exits...\n");
   #endif

}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void FormationSetupPanel::LoadData()
{
   FormationInterface *form = (FormationInterface*)(theGuiInterpreter->GetConfiguredObject(mFormationName));
   StringArray scList = form->GetStringArrayParameter(form->GetParameterID("Add"));

   // Set object pointer for "Show Script"
   mObject = form;

   for (unsigned int i=0; i<scList.size(); i++)
   {
      mSoSelectedListBox->Append(scList[i].c_str());
      mSoExcList.Add(scList[i].c_str());
   }
   
   // Show defaults
   mSoAvailableListBox->SetSelection(0);
   mSoSelectedListBox->SetSelection(0);
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void FormationSetupPanel::SaveData()
{
   FormationInterface *form = (FormationInterface*)(theGuiInterpreter->GetConfiguredObject(mFormationName));
   form->SetBooleanParameter("Clear", true);
   
   int soCount = mSoSelectedListBox->GetCount();
   for (int i=0; i<soCount; i++)
   {
      form->SetStringParameter(form->GetParameterID("Add"),
                               std::string(mSoSelectedListBox->GetString(i).c_str()));
   }

   theGuiManager->UpdateFormation();
   GmatAppData::Instance()->GetResourceTree()->UpdateFormation();
}


//------------------------------------------------------------------------------
// void OnAddSpaceObject(wxCommandEvent& event)
//------------------------------------------------------------------------------
void FormationSetupPanel::OnAddSpaceObject(wxCommandEvent& event)
{
   #ifdef DEBUG_FORMATION
      MessageInterface::ShowMessage
         ("FormationSetupPanel::OnAddSpaceObject() enters...");
   #endif
   
   // get string in first list and then search for it
   // in the second list
   wxString str = mSoAvailableListBox->GetStringSelection();
   int sel = mSoAvailableListBox->GetSelection();
   int found = mSoSelectedListBox->FindString(str);
   
   #ifdef DEBUG_FORMATION
      MessageInterface::ShowMessage
         ("str = \"%s\", sel = %d, found = %d\n", str.c_str(),sel,found);
   #endif
      
   // Check no selection then do nothing
   if (sel == -1)
      return;

   // if the string wasn't found in the second list, insert it
   if (found == wxNOT_FOUND)
   {
      #ifdef DEBUG_FORMATION
         MessageInterface::ShowMessage("string is not found...");
      #endif

      mSoSelectedListBox->Append(str);
      mSoAvailableListBox->Delete(sel);
      mSoSelectedListBox->SetStringSelection(str);
      mSoExcList.Add(str);
      
      mSoAvailableListBox->SetSelection(sel);
      
      EnableUpdate(true);
   }
   
   #ifdef DEBUG_FORMATION
      MessageInterface::ShowMessage
         ("FormationSetupPanel::OnAddSpaceObject() exits...");
   #endif
}


//------------------------------------------------------------------------------
// void OnRemoveSpaceObject(wxCommandEvent& event)
//------------------------------------------------------------------------------
void FormationSetupPanel::OnRemoveSpaceObject(wxCommandEvent& event)
{
   #ifdef DEBUG_FORMATION
   MessageInterface::ShowMessage(
                   "\nFormationSetupPanel::OnRemoveSpaceObject() enters...");
   #endif

   wxString str = mSoSelectedListBox->GetStringSelection();
   int sel = mSoSelectedListBox->GetSelection();
   
   #ifdef DEBUG_FORMATION
      MessageInterface::ShowMessage("\nstr = \"%s\", sel = %d\n",
                                    str.c_str(),sel);
   #endif

   // Check if no selection the do nothing
   if (sel == -1)
      return;

   mSoSelectedListBox->Delete(sel);
   mSoAvailableListBox->Append(str);
   mSoAvailableListBox->SetStringSelection(str);
   mSoExcList.Remove(str.c_str());
   
   if (sel-1 < 0)
      mSoSelectedListBox->SetSelection(0);
   else
      mSoSelectedListBox->SetSelection(sel-1);
   
   EnableUpdate(true);

   #ifdef DEBUG_FORMATION
   MessageInterface::ShowMessage(
                   "\nFormationSetupPanel::OnRemoveSpaceObject() exits...\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnClearSpaceObject(wxCommandEvent& event)
//------------------------------------------------------------------------------
void FormationSetupPanel::OnClearSpaceObject(wxCommandEvent& event)
{
   Integer count = mSoSelectedListBox->GetCount();
   
   if (count == 0)
      return;
   
   for (Integer i=0; i<count; i++)
   {
      mSoAvailableListBox->Append(mSoSelectedListBox->GetString(i));
   }
   
   mSoSelectedListBox->Clear();
   mSoExcList.Empty();
   mSoAvailableListBox->SetSelection(0);
   
   EnableUpdate(true);
}
