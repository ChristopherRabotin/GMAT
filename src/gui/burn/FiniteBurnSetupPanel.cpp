//$Id$
//------------------------------------------------------------------------------
//                              FiniteBurnSetupPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
      (FiniteBurn*) theGuiInterpreter->GetConfiguredObject(burnName.c_str());
   
   thrusterSelected = "";
   canClose = true;
   
   Create();
   Show();
   
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
 *
 * @note Creates the panel for the Finite burn data
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
      bsize = 2; // border size
      
      // create sizers
      wxFlexGridSizer *pageSizer = new wxFlexGridSizer(4, 2, bsize, bsize);
      
      // Thrusters
      wxStaticText *thrusterLabel = new wxStaticText(this, ID_TEXT,
         wxT(GUI_ACCEL_KEY"Thruster"), wxDefaultPosition, wxDefaultSize, 0);
      mThrusterComboBox =
         theGuiManager->GetThrusterComboBox(this, ID_COMBOBOX, wxSize(150,-1));
      mThrusterComboBox->SetToolTip(pConfig->Read(_T("ThrusterHint")));
      
      // add to page sizer    
      pageSizer->Add(thrusterLabel, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(mThrusterComboBox, 0, wxALIGN_LEFT | wxALL, bsize);
      
      // add page sizer to middle sizer
      theMiddleSizer->Add(pageSizer, 0, wxALIGN_CENTRE|wxALL, 5);
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
      
      // load thruster
      Integer id = theBurn->GetParameterID("Thrusters");
      StringArray thrusters = theBurn->GetStringArrayParameter(id);
      
      #if DEBUG_FINITEBURN_PANEL
      MessageInterface::ShowMessage
         ("   # of configured thrusters = %d\n", thrusters.size() );
      #endif
      
      if (thrusters.empty())
      {
         if (theGuiManager->GetNumThruster() > 0)
         {
            #if DEBUG_FINITEBURN_PANEL
            MessageInterface::ShowMessage("   Inserting No Thruster Selected\n");
            #endif
            mThrusterComboBox->Insert("No Thruster Selected", 0);
            mThrusterComboBox->SetSelection(0);
         }
      }
      else
      {
         thrusterSelected = thrusters[0].c_str(); 
         mThrusterComboBox->SetValue(thrusterSelected.c_str());
      }
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
