//$Header$
//------------------------------------------------------------------------------
//                              BeginFiniteBurnPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
// Author: Linda Jun
// Created: 2006/07/14
//
/**
 * This class contains the BeginFiniteBurn Setup window.
 */
//------------------------------------------------------------------------------

#include "BeginFiniteBurnPanel.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_BEGIN_FINITE_BURN_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(BeginFiniteBurnPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_COMBOBOX(ID_COMBOBOX, BeginFiniteBurnPanel::OnComboBoxChange)
   EVT_CHECKLISTBOX(ID_CHECKLISTBOX, BeginFiniteBurnPanel::OnCheckListBoxChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// BeginFiniteBurnPanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs BeginFiniteBurnPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the maneuver dialog box
 */
//------------------------------------------------------------------------------
BeginFiniteBurnPanel::BeginFiniteBurnPanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)

{
   theCommand = cmd;

   if (theCommand != NULL)
   {
      Create();
      Show();
   }
}


//------------------------------------------------------------------------------
// ~BeginFiniteBurnPanel()
//------------------------------------------------------------------------------
BeginFiniteBurnPanel::~BeginFiniteBurnPanel()
{
   theGuiManager->UnregisterComboBox("FiniteBurn", mFiniteBurnComboBox);
   theGuiManager->UnregisterCheckListBox("Spacecraft", mSatCheckListBox);
}


//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void BeginFiniteBurnPanel::OnComboBoxChange(wxCommandEvent& event)
{
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnCheckListBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void BeginFiniteBurnPanel::OnCheckListBoxChange(wxCommandEvent& event)
{
   theApplyButton->Enable();
}

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 *
 * @note Creates the panel for the Maneuver Command
 */
//------------------------------------------------------------------------------
void BeginFiniteBurnPanel::Create()
{
   #if DEBUG_BEGIN_FINITE_BURN_PANEL
   MessageInterface::ShowMessage("BeginFiniteBurnPanel::Create() command=%s\n",
                                 theCommand->GetTypeName().c_str());
   #endif
   
   StringArray items;
   
   // create sizers
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *burnSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *spacecraftSizer = new wxBoxSizer(wxHORIZONTAL);
   
   //----------------------------------------------------------------------
   // Burns
   //----------------------------------------------------------------------
   // create burn label
   wxStaticText *burnLabel =
      new wxStaticText(this, ID_TEXT,
                       wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0);
   
   #if DEBUG_BEGIN_FINITE_BURN_PANEL
   MessageInterface::ShowMessage
      ("BeginFiniteBurnPanel::Create() Calling theGuiManager->"
       "GetFiniteBurnComboBox()\n");
   #endif
   
   // create burn combo box
   mFiniteBurnComboBox =
      theGuiManager->GetFiniteBurnComboBox(this, ID_COMBOBOX, wxSize(150,-1));
   
   // add burn label and combobox to burn sizer
   burnSizer->Add(burnLabel, 0, wxALIGN_CENTER | wxALL, 5);
   burnSizer->Add(mFiniteBurnComboBox, 0, wxALIGN_CENTER | wxALL, 5);
   
   //----------------------------------------------------------------------
   // Spacecraft
   //----------------------------------------------------------------------
   // create spacecraft label
   wxStaticText *spacecraftLabel =
      new wxStaticText(this, ID_TEXT,
                       wxT("To"), wxDefaultPosition, wxDefaultSize, 0);

   #if DEBUG_BEGIN_FINITE_BURN_PANEL
   MessageInterface::ShowMessage
      ("BeginFiniteBurnPanel::Create() Calling theGuiManager->GetSpacecraft"
       "CheckListBox()\n");
   #endif
   
   // create spacecraft combo box
   mSatCheckListBox = theGuiManager->
      //GetSpacecraftCheckListBox(this, ID_CHECKLISTBOX, wxSize(150,-1));
      GetSpacecraftCheckListBox(this, ID_CHECKLISTBOX, wxSize(150,100));
   
   // add spacecraft label and combobox to spacecraft sizer
   spacecraftSizer->Add(spacecraftLabel, 0, wxALIGN_CENTER | wxALL, 5);
   spacecraftSizer->Add(mSatCheckListBox, 0, wxALIGN_CENTER | wxALL, 5);
   
   // add items to page sizer
   pageBoxSizer->Add(burnSizer, 0, wxGROW | wxALIGN_LEFT | wxALL, 5);
   pageBoxSizer->Add(spacecraftSizer, 0, wxGROW | wxALIGN_RIGHT | wxALL, 5);
   
   // add to middle sizer
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);     
    
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void BeginFiniteBurnPanel::LoadData()
{
   // Set the pointer for the "Show Script" button
   mObject = theCommand;
   
   // Get FiniteBurn from the command
   std::string burnName = theCommand->GetRefObjectName(Gmat::BURN);

   #if DEBUG_BEGIN_FINITE_BURN
   MessageInterface::ShowMessage
      ("BeginFiniteBurnPanel::LoadData() burnName=<%s>\n", burnName.c_str());
   #endif
   
   mFiniteBurnComboBox->SetValue(burnName.c_str());
   
   // Get spacecraft list from the command
   StringArray scNames = theCommand->GetRefObjectNameArray(Gmat::SPACECRAFT);
   int item;
   for (UnsignedInt i=0; i<scNames.size(); i++)
   {
      #if DEBUG_BEGIN_FINITE_BURN
      MessageInterface::ShowMessage("   scName=<%s>\n", scNames[i].c_str());
      #endif
      
      item = mSatCheckListBox->FindString(scNames[i].c_str());
      mSatCheckListBox->Check(item);
   }
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void BeginFiniteBurnPanel::SaveData()
{
   // Get matching EndFiniteBurn command
   
   wxString cmdName = theCommand->GetName().c_str();
   MessageInterface::ShowMessage("===> before cmdName=%s\n", cmdName.c_str());
   cmdName.Replace("Begin", "End");
   MessageInterface::ShowMessage("===> after  cmdName=%s\n", cmdName.c_str());

   GmatCommand *endCommand = NULL;
   GmatCommand *cmd = theCommand->GetNext();
   if (cmd)
   {
      while (cmd)
      {
         MessageInterface::ShowMessage
            ("cmd=%s\n", cmd->GetTypeName().c_str());
         
         if (cmd->GetTypeName() == "EndFiniteBurn")
         {
            endCommand = cmd;
            break;
         }
         
         cmd = cmd->GetNext();
      }
   }

   // Save both BeginFiniteBurn and EndFiniteBurn
   
   // save finite burn
   wxString burnString = mFiniteBurnComboBox->GetValue();
   theCommand->SetRefObjectName(Gmat::BURN, burnString.c_str());
   endCommand->SetRefObjectName(Gmat::BURN, burnString.c_str());
   
   // save spacecrafts
   int count = mSatCheckListBox->GetCount();
   wxString scName;

   theCommand->TakeAction("Clear");
   endCommand->TakeAction("Clear");
   
   for (int i=0; i<count; i++)
   {
      if (mSatCheckListBox->IsChecked(i))
      {
         scName = mSatCheckListBox->GetString(i);
         theCommand->SetRefObjectName(Gmat::SPACECRAFT, scName.c_str());
         endCommand->SetRefObjectName(Gmat::SPACECRAFT, scName.c_str());
      }
   }
}
