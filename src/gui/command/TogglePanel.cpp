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
   StringArray items;
   int bsize = 3;
    
   // create sizers
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxHORIZONTAL);

   // get subscriber name
   wxString subscriberName = theCommand->GetStringParameter
      (theCommand->GetParameterID("Subscriber")).c_str();
   
   // create subscriber label
   wxStaticText *subscriberLabel =
      new wxStaticText(this, ID_TEXT, wxT(subscriberName),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   // list of subscribers
   mOnRadioButton =
      new wxRadioButton(this, ID_RADIOBUTTON, wxT("On"),
                        wxDefaultPosition, wxDefaultSize, 0);
   
   mOffRadioButton =
      new wxRadioButton(this, ID_RADIOBUTTON, wxT("Off"),
                        wxDefaultPosition, wxDefaultSize, 0);
     
   // add subscriber label and RadioButton to sizer
   pageBoxSizer->Add(subscriberLabel, 0, wxALIGN_CENTER | wxALL, bsize);
   pageBoxSizer->Add(mOnRadioButton, 0, wxALIGN_CENTER | wxALL, bsize);
   pageBoxSizer->Add(mOffRadioButton, 0, wxALIGN_CENTER | wxALL, bsize);
   
   // add to middle sizer
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);     

}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void TogglePanel::LoadData()
{
   // load data from the core engine
   
   std::string toggleState = theCommand->GetStringParameter
      (theCommand->GetParameterID("ToggleState"));
  
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
   if (mOnRadioButton->GetValue() == true)
      theCommand->SetStringParameter(theCommand->GetParameterID("ToggleState"), "On");
   else
      theCommand->SetStringParameter(theCommand->GetParameterID("ToggleState"), "Off");
      
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

