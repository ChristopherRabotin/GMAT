//$Header$
//------------------------------------------------------------------------------
//                           TargetPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/12/16
// Modified:
//    2004/05/06 Allison Greene - to inherit from GmatPanel
//    2004/11/04 Linda Jun - Renamed from TargetPanel
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
   EVT_TEXT(ID_TEXTCTRL, TargetPanel::OnTextUpdate)
   EVT_COMBOBOX(ID_COMBO, TargetPanel::OnComboSelection)
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
      theApplyButton->Disable();
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
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void TargetPanel::Create()
{    
   // wxButton
   correctionsButton =
      new wxButton(this, ID_BUTTON, wxT("Apply Corrections"), wxDefaultPosition,
                   wxSize(117,-1), 0);
   
   // wxStaticText
   wxStaticText *solverNameStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Solver Name"), wxDefaultPosition,
                       wxDefaultSize, 0);
   wxStaticText *convStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Upon Convergence"), wxDefaultPosition,
                       wxDefaultSize, 0);
   wxStaticText *modeStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Mode"), wxDefaultPosition,
                       wxDefaultSize, 0);
   
   // wxString
   wxString strArray2[] = 
   {
      wxT("")
   };
   wxString strArray3[] = 
   {
      wxT("")
   };
   wxString emptyArray[] = 
   {
      wxT("No Solver Available")
   };
   
   // wxComboBox
   StringArray solverNames;
   solverNames = theGuiInterpreter->GetListOfObjects(Gmat::SOLVER);
   int solverCount = solverNames.size();
   wxString *solverArray = new wxString[solverCount];
   for (int i=0; i<solverCount; i++)
   {
      solverArray[i] = solverNames[i].c_str();
   }

   if (solverCount > 0)
   {
      mSolverComboBox =
         new wxComboBox(this, ID_COMBO, wxT(solverArray[0]), wxDefaultPosition,
                        wxSize(180,-1), solverCount, solverArray,
                        wxCB_DROPDOWN|wxCB_READONLY);
   }
   else
   {
      mSolverComboBox =
         new wxComboBox(this, ID_COMBO, wxT(emptyArray[0]), wxDefaultPosition,
                        wxSize(180,-1), 1, emptyArray, wxCB_DROPDOWN|wxCB_READONLY);
   }
   
   mConvComboBox = new wxComboBox(this, ID_COMBO, wxT(""), wxDefaultPosition,
                                  wxSize(180,-1), 1, strArray2, wxCB_DROPDOWN);
   modeComboBox = new wxComboBox(this, ID_COMBO, wxT(""), wxDefaultPosition,
                                 wxSize(180,-1), 1, strArray3, wxCB_DROPDOWN);
   
   // wx*Sizer
   wxBoxSizer *item0 = new wxBoxSizer(wxVERTICAL);
   wxFlexGridSizer *item5 = new wxFlexGridSizer(5, 0, 0);
      
    //First row
   item5->Add(solverNameStaticText, 0, wxALIGN_CENTRE|wxALL, 5);
   item5->Add(mSolverComboBox, 0, wxALIGN_CENTRE|wxALL, 5);
   item5->Add(20, 20, 0, wxALIGN_CENTRE|wxALL, 5);
   item5->Add(convStaticText, 0, wxALIGN_CENTRE|wxALL, 5);
   item5->Add(mConvComboBox, 0, wxALIGN_CENTRE|wxALL, 5);
   //Second row
   item5->Add(modeStaticText, 0, wxALIGN_CENTRE|wxALL, 5);
   item5->Add(modeComboBox, 0, wxALIGN_CENTRE|wxALL, 5);
   item5->Add(20, 20, 0, wxALIGN_CENTRE|wxALL, 5);
   item5->Add(20, 20, 0, wxALIGN_CENTRE|wxALL, 5);
   item5->Add(correctionsButton, 0, wxALIGN_CENTRE|wxALL, 5);

   item0->Add(item5, 0, wxALIGN_CENTRE|wxALL, 5);

   correctionsButton->Enable(false);
   
   theMiddleSizer->Add(item0, 0, wxGROW, 5);

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
      mSolverComboBox->SetStringSelection(solverName.c_str());
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("TargetPanel:LoadData() error occurred!\n%s\n",
          e.GetMessage().c_str());
   }
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void TargetPanel::SaveData()
{
   // explicitly disable apply button
   // it is turned on in each of the panels
   theApplyButton->Disable();
   
   return;
}

//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void TargetPanel::OnTextUpdate(wxCommandEvent& event)
{
   theApplyButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnComboSelection(wxCommandEvent& event)
//------------------------------------------------------------------------------
void TargetPanel::OnComboSelection(wxCommandEvent& event)
{
   theApplyButton->Enable(true);
}
