//$Header$
//------------------------------------------------------------------------------
//                              TogglePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/10/20
//
/**
 * This class contains the Toggle setup window.
 */
//------------------------------------------------------------------------------

#include "TogglePanel.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_TOGGLE_PANEL 1


//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(TogglePanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_RADIOBUTTON(ID_RADIOBUTTON, TogglePanel::OnRadioButtonChange)
   EVT_CHECKLISTBOX(ID_CHECKLISTBOX, TogglePanel::OnCheckListBoxChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// TogglePanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs TogglePanel object.
 *
 * @param <parent> input parent.
 */
//------------------------------------------------------------------------------
TogglePanel::TogglePanel(wxWindow *parent, GmatCommand *cmd)
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
// ~TogglePanel()
//------------------------------------------------------------------------------
TogglePanel::~TogglePanel()
{
   theGuiManager->UnregisterCheckListBox("Subscriber", mSubsCheckListBox);
}


//---------------------------------
// protected methods
//---------------------------------

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates the panel for the Maneuver Command
 */
//------------------------------------------------------------------------------
void TogglePanel::Create()
{
   int bsize = 3;
    
   // create object label
   wxStaticText *objectLabel =
      new wxStaticText(this, ID_TEXT, wxT("Select Subscribers to Toggle"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   // create subscriber check list box
   mSubsCheckListBox =
      theGuiManager->GetSubscriberCheckListBox(this, ID_CHECKLISTBOX, wxSize(150,-1));
   
   // On or Off button
   mOnRadioButton =
      new wxRadioButton(this, ID_RADIOBUTTON, wxT("On"),
                        wxDefaultPosition, wxDefaultSize, 0);
   
   mOffRadioButton =
      new wxRadioButton(this, ID_RADIOBUTTON, wxT("Off"),
                        wxDefaultPosition, wxDefaultSize, 0);
   
   // create sizers
   wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *pageSizer = new wxBoxSizer(wxHORIZONTAL);
   
   buttonSizer->Add(mOnRadioButton, 0, wxALIGN_CENTER | wxALL, bsize);
   buttonSizer->Add(mOffRadioButton, 0, wxALIGN_CENTER | wxALL, bsize);
   
   pageSizer->Add(objectLabel, 0, wxALIGN_CENTER | wxALL, bsize);
   pageSizer->Add(mSubsCheckListBox, 0, wxALIGN_CENTER | wxALL, bsize);
   pageSizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxALL, bsize);

   // add to middle sizer
   theMiddleSizer->Add(pageSizer, 0, wxALIGN_CENTRE|wxALL, bsize);     

}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void TogglePanel::LoadData()
{
   // Set the pointer for the "Show Script" button
   mObject = theCommand;
   
   // get subscriber names 
   StringArray subNames = theCommand->GetRefObjectNameArray(Gmat::SUBSCRIBER);
   int subsize = subNames.size();
   
   std::string toggleState = theCommand->GetStringParameter
      (theCommand->GetParameterID("ToggleState"));
   
   #if DEBUG_TOGGLE_PANEL
   for (unsigned int i=0; i<subNames.size(); i++)
      MessageInterface::ShowMessage
         ("TogglePanel::LoadData() subName[%d]=%s, toggleState=%s\n", i,
          subNames[i].c_str(), toggleState.c_str());
   #endif
   
   // Initialize check list box
   std::string name;
   std::string subName;
   for (UnsignedInt i=0; i<mSubsCheckListBox->GetCount(); i++)
   {
      name = mSubsCheckListBox->GetString(i).c_str();
      for (int j=0; j<subsize; j++)
      {
         subName = subNames[j];
         if (name == subName)
         {
            mSubsCheckListBox->Check(i);
            break;
         }
      }
      
      #if DEBUG_TOGGLE_PANEL
      MessageInterface::ShowMessage("   name=%s\n", name.c_str());
      #endif
      
   }

   if (toggleState == "On")
      mOnRadioButton->SetValue(true);
   else
      mOffRadioButton->SetValue(true);
   
   #if DEBUG_TOGGLE_PANEL
   MessageInterface::ShowMessage
      ("TogglePanel::LoadData() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void TogglePanel::SaveData()
{
   int count = mSubsCheckListBox->GetCount();
   bool toggleOn = mOnRadioButton->GetValue();
   std::string onOff = toggleOn ? "On" : "Off";
   int checkedCount = 0;
   
   canClose = true;
   
   //-----------------------------------------------------------------
   // check for number of subscribers checked
   //-----------------------------------------------------------------
   for (int i=0; i<count; i++)
      if (mSubsCheckListBox->IsChecked(i))
         checkedCount++;

   if (checkedCount == 0)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_,
          "Please select one or more subscribers to toggle on or off.");
      canClose = false;
      return;
   }

   
   //-----------------------------------------------------------------
   // save values to base, base code should do any range checking
   //-----------------------------------------------------------------
   
   theCommand->TakeAction("Clear");
   
   std::string subName;
   for (int i=0; i<count; i++)
   {
      if (mSubsCheckListBox->IsChecked(i))
      {
         subName = mSubsCheckListBox->GetString(i);
         theCommand->SetStringParameter(theCommand->GetParameterID("Subscriber"), subName);
         
         #if DEBUG_TOGGLE_PANEL
         MessageInterface::ShowMessage
            ("TogglePanel::SaveData() subName=%s, onOff=%s\n", subName.c_str(),
             onOff.c_str());
         #endif
      }
   }
   
   theCommand->SetStringParameter(theCommand->GetParameterID("ToggleState"), onOff);
   
}

//---------------------------------
// event handling
//---------------------------------

//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void TogglePanel::OnComboBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnRadioButtonChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void TogglePanel::OnRadioButtonChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnCheckListBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void TogglePanel::OnCheckListBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}


// //------------------------------------------------------------------------------
// // void OnSelectSubscriber(wxCommandEvent& event)
// //------------------------------------------------------------------------------
// void TogglePanel::OnSelectSubscriber(wxCommandEvent& event)
// {
//    ShowOption();
//    EnableUpdate(true);
// }

