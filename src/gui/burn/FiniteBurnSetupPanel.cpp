//$Id$
//------------------------------------------------------------------------------
//                              FiniteBurnSetupPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: LaMont Ruley
// Created: 2004/03/04
//
/**
 * This class contains the Finite Burn Setup window.
 */
//------------------------------------------------------------------------------

#include "FiniteBurnSetupPanel.hpp"
#include "MessageInterface.hpp"
#include <wx/config.h>

//#define DEBUG_FINITEBURN_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FiniteBurnSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_COMBOBOX(ID_COMBOBOX, FiniteBurnSetupPanel::OnComboBoxChange)
   EVT_BUTTON(ID_BUTTON, FiniteBurnSetupPanel::OnButtonClick)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// FiniteBurnSetupPanel(wxWindow *parent, const wxString &burnName)
//------------------------------------------------------------------------------
/**
 * Constructs FiniteBurnSetupPanel object.
 *
 * @param <parent> input parent.
 * @param <burnName> input burn name.
 *
 * @note Creates the Finite burn GUI
 */
//------------------------------------------------------------------------------
FiniteBurnSetupPanel::FiniteBurnSetupPanel(wxWindow *parent, 
   const wxString &burnName):GmatPanel(parent)
{
   theBurn = 
      (FiniteBurn*)theGuiInterpreter->GetConfiguredObject(burnName.c_str());
   
   thrusterSelected = "";
   canClose = true;

   Create();
//   Show();
   
   EnableUpdate(false);
}

//------------------------------------------------------------------------------
// ~FiniteBurnSetupPanel()
//------------------------------------------------------------------------------
/**
 * Destroys FiniteBurnSetupPanel object.
 *
 */
//------------------------------------------------------------------------------
FiniteBurnSetupPanel::~FiniteBurnSetupPanel()
{
   theGuiManager->UnregisterComboBox("Thruster", mThrusterComboBox);
   theGuiManager->UnregisterListBox("Thruster", availableThrusterListBox,
                                    &mExcludedThrusterList);
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnFrameComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void FiniteBurnSetupPanel::OnComboBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mThrusterComboBox)
   {
      thrusterSelected = mThrusterComboBox->GetStringSelection().c_str();
      if (thrusterSelected == "No Thruster Selected")
         thrusterSelected = "";
      
      // remove "No Thruster Selected" once thruster is selected
      int pos = mThrusterComboBox->FindString("No Thruster Selected");
      if (pos != wxNOT_FOUND)
         mThrusterComboBox->Delete(pos);
      
      EnableUpdate(true);
   }
}


//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates the panel for the Finite burn data
 */
//------------------------------------------------------------------------------
void FiniteBurnSetupPanel::Create()
{
   #if DEBUG_FINITEBURN_PANEL
      MessageInterface::ShowMessage( "FiniteBurnSetupPanel::Create() \n" );
   #endif
   
   Integer bsize;

   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Finite Burn Setup"));

   if (theBurn != NULL)
   {
//      bsize = 2; // border size
//
//      // create sizers
//      wxFlexGridSizer *pageSizer = new wxFlexGridSizer(4, 2, bsize, bsize);
//
//      // Thrusters
//      wxStaticText *thrusterLabel = new wxStaticText(this, ID_TEXT,
//         GUI_ACCEL_KEY"Thruster", wxDefaultPosition, wxDefaultSize, 0);
//      mThrusterComboBox =
//         theGuiManager->GetThrusterComboBox(this, ID_COMBOBOX, wxSize(150,-1));
//      mThrusterComboBox->SetToolTip(pConfig->Read(_T("ThrusterHint")));
//
//      // add to page sizer
//      pageSizer->Add(thrusterLabel, 0, wxALIGN_LEFT | wxALL, bsize);
//      pageSizer->Add(mThrusterComboBox, 0, wxALIGN_LEFT | wxALL, bsize);

      // get the config object
      wxConfigBase *pConfig = wxConfigBase::Get();
      // SetPath() understands ".."
      pConfig->SetPath(wxString("/Spacecraft Thrusters"));

      // wxButton
      selectButton = new wxButton( this, ID_BUTTON, wxString("-"GUI_ACCEL_KEY">"),
                                 wxDefaultPosition, wxDefaultSize, 0 );
      selectButton->SetToolTip(pConfig->Read(_T("AddThrusterHint")));

      removeButton = new wxButton( this, ID_BUTTON, wxString(GUI_ACCEL_KEY"<-"),
                                 wxDefaultPosition, wxDefaultSize, 0 );
      removeButton->SetToolTip(pConfig->Read(_T("RemoveThrusterHint")));

      selectAllButton = new wxButton( this, ID_BUTTON, wxString("=>"),
                                 wxDefaultPosition, wxDefaultSize, 0 );
      selectAllButton->SetToolTip(pConfig->Read(_T("AddAllThrustersHint")));

      removeAllButton = new wxButton( this, ID_BUTTON, wxString("<"GUI_ACCEL_KEY"="),
                                 wxDefaultPosition, wxDefaultSize, 0 );
      removeAllButton->SetToolTip(pConfig->Read(_T("ClearThrustersHint")));


      //causing VC++ error => wxString emptyList[] = {};
      wxArrayString emptyList;

      Integer paramID = theBurn->GetParameterID("Thrusters");
      StringArray thrusterNames = theBurn->GetStringArrayParameter(paramID);

      Integer count = thrusterNames.size();
      for (Integer i = 0; i < count; i++)
         mExcludedThrusterList.Add(thrusterNames[i].c_str());

      availableThrusterListBox =
         theGuiManager->GetThrusterListBox(this, ID_LISTBOX, wxSize(150,200),
                                           &mExcludedThrusterList);
      availableThrusterListBox->SetToolTip(pConfig->Read(_T("AvailableThrustersHint")));

      selectedThrusterListBox =
         new wxListBox(this, ID_LISTBOX, wxDefaultPosition, wxSize(150,200), //0,
                       emptyList, wxLB_SINGLE);
      selectedThrusterListBox->SetToolTip(pConfig->Read(_T("SelectedThrustersHint")));

      Integer bsize = 3; // border size

      // wx*Sizers
      wxBoxSizer *boxSizer1 = new wxBoxSizer( wxVERTICAL );
      wxBoxSizer *boxSizer2 = new wxBoxSizer( wxVERTICAL );
      wxBoxSizer *boxSizer3 = new wxBoxSizer( wxHORIZONTAL );
      wxStaticBox *staticBox1 = new wxStaticBox( this, -1, wxString(GUI_ACCEL_KEY"Available Thrusters") );
      wxStaticBoxSizer *staticBoxSizer1 = new wxStaticBoxSizer( staticBox1, wxHORIZONTAL );
      wxStaticBox *staticBox2 = new wxStaticBox( this, -1, wxString(GUI_ACCEL_KEY"Selected Thrusters") );
      wxStaticBoxSizer *staticBoxSizer2 = new wxStaticBoxSizer( staticBox2, wxHORIZONTAL );

      // Add to wx*Sizers
      boxSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
      boxSizer2->Add(selectButton, 0, wxALIGN_CENTER|wxALL, bsize );
      boxSizer2->Add(removeButton, 0, wxALIGN_CENTER|wxALL, bsize );
      boxSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
      boxSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
      boxSizer2->Add(selectAllButton, 0, wxALIGN_CENTER|wxALL, bsize );
      boxSizer2->Add(removeAllButton, 0, wxALIGN_CENTER|wxALL, bsize );

      staticBoxSizer1->Add( availableThrusterListBox, 0, wxALIGN_CENTER|wxALL, bsize );
      staticBoxSizer2->Add( selectedThrusterListBox, 0, wxALIGN_CENTER|wxALL, bsize );
      
      boxSizer3->Add( staticBoxSizer1, 0, wxALIGN_CENTER|wxALL, bsize);
      boxSizer3->Add( boxSizer2, 0, wxALIGN_CENTER|wxALL, bsize);
      boxSizer3->Add( staticBoxSizer2, 0, wxALIGN_CENTRE|wxALL, bsize);
      
      boxSizer1->Add( boxSizer3, 0, wxALIGN_CENTRE|wxALL, bsize);
      
//      SetAutoLayout( true );
//      SetSizer( boxSizer1 );
      boxSizer1->Fit( this );
      boxSizer1->SetSizeHints( this );
      
      selectButton->Enable(true);
      removeButton->Enable(true);
      selectAllButton->Enable(true);
      removeAllButton->Enable(true);

      // add page sizer to middle sizer
      theMiddleSizer->Add(boxSizer1, 0, wxALIGN_CENTRE|wxALL, 5);
   }
   else
   {
      // show error message
      MessageInterface::ShowMessage
         ("FiniteBurnSetupPanel:Create() theBurn is NULL\n");
   }
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void FiniteBurnSetupPanel::LoadData()
{
   #if DEBUG_FINITEBURN_PANEL
      MessageInterface::ShowMessage( "FiniteBurnSetupPanel::LoadData() \n" );
   #endif
   
   // load data from the core engine 
   try
   {
      // Set object pointer for "Show Script"
      mObject = theBurn;
      
//      // load thruster
//      Integer id = theBurn->GetParameterID("Thrusters");
//      StringArray thrusters = theBurn->GetStringArrayParameter(id);
//
//      #if DEBUG_FINITEBURN_PANEL
//      MessageInterface::ShowMessage
//         ("   # of configured thrusters = %d\n", thrusters.size() );
//      #endif
//
//      if (thrusters.empty())
//      {
//         if (theGuiManager->GetNumThruster() > 0)
//         {
//            #if DEBUG_FINITEBURN_PANEL
//            MessageInterface::ShowMessage("   Inserting No Thruster Selected\n");
//            #endif
//            mThrusterComboBox->Insert("No Thruster Selected", 0);
//            mThrusterComboBox->SetSelection(0);
//         }
//      }
//      else
//      {
//         thrusterSelected = thrusters[0].c_str();
//         mThrusterComboBox->SetValue(thrusterSelected.c_str());
//      }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("FiniteBurnSetupPanel:LoadData() error occurred!\n%s\n",
            e.GetFullMessage().c_str());
   }
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void FiniteBurnSetupPanel::SaveData()
{
   #if DEBUG_FINITEBURN_PANEL
      MessageInterface::ShowMessage( "FiniteBurnSetupPanel::SaveData() \n" );
   #endif
      
   canClose = true;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      // save thrusters
      Integer id = theBurn->GetParameterID("Thrusters");
      theBurn->SetStringParameter(id, thrusterSelected.c_str(), 0);
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
}


void FiniteBurnSetupPanel::OnButtonClick(wxCommandEvent &event)
{

}
