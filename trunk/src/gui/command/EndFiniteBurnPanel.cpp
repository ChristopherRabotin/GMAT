//$Header$
//------------------------------------------------------------------------------
//                              EndFiniteBurnPanel
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
 * This class contains the EndFiniteBurn Setup window.
 */
//------------------------------------------------------------------------------

#include "EndFiniteBurnPanel.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_BEGIN_FINITE_BURN_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(EndFiniteBurnPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_COMBOBOX(ID_COMBOBOX, EndFiniteBurnPanel::OnComboBoxChange)
   EVT_CHECKLISTBOX(ID_CHECKLISTBOX, EndFiniteBurnPanel::OnCheckListBoxChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// EndFiniteBurnPanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs EndFiniteBurnPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the maneuver dialog box
 */
//------------------------------------------------------------------------------
EndFiniteBurnPanel::EndFiniteBurnPanel(wxWindow *parent, GmatCommand *cmd)
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
// ~EndFiniteBurnPanel()
//------------------------------------------------------------------------------
EndFiniteBurnPanel::~EndFiniteBurnPanel()
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
void EndFiniteBurnPanel::OnComboBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnCheckListBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void EndFiniteBurnPanel::OnCheckListBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
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
void EndFiniteBurnPanel::Create()
{
   #if DEBUG_BEGIN_FINITE_BURN_PANEL
   MessageInterface::ShowMessage("EndFiniteBurnPanel::Create() command=%s\n",
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
      ("EndFiniteBurnPanel::Create() Calling theGuiManager->"
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
      ("EndFiniteBurnPanel::Create() Calling theGuiManager->GetSpacecraft"
       "CheckListBox()\n");
   #endif
   
   // create spacecraft combo box
   mSatCheckListBox = theGuiManager->
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
void EndFiniteBurnPanel::LoadData()
{
   // Set the pointer for the "Show Script" button
   mObject = theCommand;
   
   try
   {
      // Get FiniteBurn from the command
      std::string burnName = theCommand->GetRefObjectName(Gmat::FINITE_BURN);
      
      #if DEBUG_BEGIN_FINITE_BURN
      MessageInterface::ShowMessage
         ("EndFiniteBurnPanel::LoadData() burnName=<%s>\n", burnName.c_str());
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
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void EndFiniteBurnPanel::SaveData()
{
   canClose = true;
   
   //-----------------------------------------------------------------
   // check empty spacecraft list
   //-----------------------------------------------------------------
   int count = mSatCheckListBox->GetCount();
   wxString scName;
   wxArrayString scList;
   
   for (int i=0; i<count; i++)
   {
      if (mSatCheckListBox->IsChecked(i))
      {
         scName = mSatCheckListBox->GetString(i);
         scList.Add(scName);
      }
   }
   
   if (scList.IsEmpty())
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Please select Spacecraft to end maneuver\n");
      canClose = false;
   }
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      // save finite burn
      wxString burnString = mFiniteBurnComboBox->GetValue();
      theCommand->SetRefObjectName(Gmat::FINITE_BURN, burnString.c_str());
      
      // save spacecrafts
      count = scList.Count();
      theCommand->TakeAction("Clear");
      
      for (int i=0; i<count; i++)
         theCommand->SetRefObjectName(Gmat::SPACECRAFT, scList[i].c_str());
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}
