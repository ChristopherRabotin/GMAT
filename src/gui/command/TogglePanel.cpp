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
//#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(TogglePanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_RADIOBUTTON(ID_RADIOBUTTON, TogglePanel::OnRadioButtonChange)
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
    
   // create subscriber combo box
   mSubsComboBox =
      theGuiManager->GetSubscriberComboBox(this, ID_COMBOBOX, wxSize(150,-1));
   
   // On or Off button
   mOnRadioButton =
      new wxRadioButton(this, ID_RADIOBUTTON, wxT("On"),
                        wxDefaultPosition, wxDefaultSize, 0);
   
   mOffRadioButton =
      new wxRadioButton(this, ID_RADIOBUTTON, wxT("Off"),
                        wxDefaultPosition, wxDefaultSize, 0);
   
   // create sizers
   wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   
   buttonSizer->Add(mOnRadioButton, 0, wxALIGN_CENTER | wxALL, bsize);
   buttonSizer->Add(mOffRadioButton, 0, wxALIGN_CENTER | wxALL, bsize);
   
   pageBoxSizer->Add(mSubsComboBox, 0, wxALIGN_CENTER | wxALL, bsize);
   pageBoxSizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxALL, bsize);

   // add to middle sizer
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);     

}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void TogglePanel::LoadData()
{
   // Set the pointer for the "Show Script" button
   mObject = theCommand;
   
   // get subscriber name (only one subscriber for now)
   // need GetRefObjectNameArray() in Toggle command to show multiple subscribers
   
   wxString subName = theCommand->GetStringParameter
      (theCommand->GetParameterID("Subscriber")).c_str();
   
   mSubsComboBox->SetValue(subName);
   
   std::string toggleState = theCommand->GetStringParameter
      (theCommand->GetParameterID("ToggleState"));
   
   #if DEBUG_TOGGLE_PANEL
   MessageInterface::ShowMessage
      ("TogglePanel::LoadData() subName=%s, toggleState=%s\n",
       subName.c_str(), toggleState.c_str());
   #endif
   
   if (toggleState == "On")
      mOnRadioButton->SetValue(true);
   else
      mOffRadioButton->SetValue(true);
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void TogglePanel::SaveData()
{
   // allow one subscriber for now
   std::string subName = mSubsComboBox->GetValue().c_str();
   bool toggleOn = mOnRadioButton->GetValue();
   std::string onOff = toggleOn ? "On" : "Off";

   #if DEBUG_TOGGLE_PANEL
   MessageInterface::ShowMessage
      ("TogglePanel::SaveData() subName=%s, onOff=%s\n", subName.c_str(),
       onOff.c_str());
   #endif
   
   theCommand->SetStringParameter(theCommand->GetParameterID("Subscriber"), subName);
   theCommand->SetStringParameter(theCommand->GetParameterID("ToggleState"), onOff);
   
}

//---------------------------------
// event handling
//---------------------------------

//------------------------------------------------------------------------------
// void OnRadioButtonChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void TogglePanel::OnRadioButtonChange(wxCommandEvent& event)
{
   theApplyButton->Enable();
}

