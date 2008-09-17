//$Id$
//------------------------------------------------------------------------------
//                           OptimizePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun (NASA/GSFC)
// Created: 2007/01/26
//
/**
 * This class contains the Optimize setup window.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "OptimizePanel.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(OptimizePanel, GmatPanel)
   EVT_COMBOBOX(ID_COMBO, OptimizePanel::OnComboBoxChange)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// OptimizePanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
OptimizePanel::OptimizePanel(wxWindow *parent, GmatCommand *cmd)
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
// ~OptimizePanel()
//------------------------------------------------------------------------------
OptimizePanel::~OptimizePanel()
{
   theGuiManager->UnregisterComboBox("Optimizer", mSolverComboBox);
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void OptimizePanel::Create()
{    
   Integer bsize = 5;

   //-------------------------------------------------------
   // Solver ComboBox
   //-------------------------------------------------------
   wxStaticText *solverNameStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Solver Name"), wxDefaultPosition,
                       wxDefaultSize, 0);

   wxStaticText *solverModeStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Solver Mode"), wxDefaultPosition,
                       wxDefaultSize, 0);
   
   mSolverComboBox =
      theGuiManager->GetOptimizerComboBox(this, ID_COMBO, wxSize(180,-1));
   
   StringArray options = theCommand->GetStringArrayParameter("SolveModeOptions");
   wxArrayString theOptions;
   
   for (StringArray::iterator i = options.begin(); i != options.end(); ++i)
      theOptions.Add(i->c_str());
   
   mSolverModeComboBox =
      new wxComboBox(this, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(180,-1),
                     theOptions, wxCB_READONLY);
   
   wxFlexGridSizer *pageSizer = new wxFlexGridSizer(2);
   
   pageSizer->Add(solverNameStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   pageSizer->Add(mSolverComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   pageSizer->Add(solverModeStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   pageSizer->Add(mSolverModeComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   
   theMiddleSizer->Add(pageSizer, 0, wxGROW, bsize);

}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void OptimizePanel::LoadData()
{
   try
   {
      // Set the pointer for the "Show Script" button
      mObject = theCommand;
      
      std::string solverName =
         theCommand->GetStringParameter("SolverName");
      
      mSolverComboBox->SetValue(solverName.c_str());
      
      std::string solverMode =
               theCommand->GetStringParameter("SolveMode");
      mSolverModeComboBox->SetValue(solverMode.c_str());
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void OptimizePanel::SaveData()
{
   try
   {
      std::string solverName = mSolverComboBox->GetValue().c_str();
      std::string solverMode = mSolverModeComboBox->GetValue().c_str();

      theCommand->SetStringParameter(theCommand->GetParameterID("SolverName"),
                                     solverName);
      theCommand->SetStringParameter(theCommand->GetParameterID("SolveMode"), 
            solverMode);

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
void OptimizePanel::OnComboBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}
