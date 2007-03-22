//$Header$
//------------------------------------------------------------------------------
//                           TargetPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Waka Waktola
// Created: 2003/12/16
//
/**
 * This class contains the Target setup window.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "TargetPanel.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(TargetPanel, GmatPanel)
   EVT_COMBOBOX(ID_COMBO, TargetPanel::OnComboBoxChange)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// TargetPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
TargetPanel::TargetPanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   theCommand = cmd;
   
   if (theCommand != NULL)
   {
      Create();
      Show();
   }
   else
   {
      // show error message
   }
}


//------------------------------------------------------------------------------
// ~TargetPanel()
//------------------------------------------------------------------------------
TargetPanel::~TargetPanel()
{
   theGuiManager->UnregisterComboBox("BoundarySolver", mSolverComboBox);
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void TargetPanel::Create()
{    
   Integer bsize = 5;

   //-------------------------------------------------------
   // Solver ComboBox
   //-------------------------------------------------------
   wxStaticText *solverNameStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Solver Name"), wxDefaultPosition,
                       wxDefaultSize, 0);
   
   mSolverComboBox =
      theGuiManager->GetBoundarySolverComboBox(this, ID_COMBO, wxSize(180,-1));
   
   
   wxFlexGridSizer *pageSizer = new wxFlexGridSizer(2);
   
   pageSizer->Add(solverNameStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   pageSizer->Add(mSolverComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   
   theMiddleSizer->Add(pageSizer, 0, wxGROW, bsize);

}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void TargetPanel::LoadData()
{
   try
   {
      // Set the pointer for the "Show Script" button
      mObject = theCommand;
      
      std::string solverName =
         theCommand->GetStringParameter(theCommand->GetParameterID("Targeter"));
      
      mSolverComboBox->SetValue(solverName.c_str());
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void TargetPanel::SaveData()
{
   try
   {
      std::string solverName = mSolverComboBox->GetValue().c_str();
      theCommand->SetStringParameter(theCommand->GetParameterID("Targeter"),
                                     solverName);
      EnableUpdate(false);
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void TargetPanel::OnComboBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}
