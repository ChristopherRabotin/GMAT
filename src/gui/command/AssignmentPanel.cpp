//$Header$
//------------------------------------------------------------------------------
//                              AssignmentPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2004/12/23
/**
 * This class contains the AssignmentPanel Setup window.
 */
//------------------------------------------------------------------------------

#include "AssignmentPanel.hpp"
#include "MessageInterface.hpp"

#define DEBUG_ASSIGNCMD_PANEL 0

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(AssignmentPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)

   EVT_COMBOBOX(ID_COMBO, AssignmentPanel::OnComboChange)
   EVT_GRID_CELL_LEFT_CLICK(AssignmentPanel::OnCellClick)
   EVT_GRID_CELL_RIGHT_CLICK(AssignmentPanel::OnCellClick)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// AssignmentPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
AssignmentPanel::AssignmentPanel( wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   theCommand = (CallFunction *)cmd;
//   theCommand = cmd;

   if (theCommand != NULL)
   {
      Create();
      Show();
   EnableUpdate(false);
//      theApplyButton->Disable();
   }
}

//------------------------------------------------------------------------------
// ~AssignmentPanel()
//------------------------------------------------------------------------------
AssignmentPanel::~AssignmentPanel()
{
}

//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void AssignmentPanel::Create()
{
   //MessageInterface::ShowMessage("AssignmentPanel::Create() entered\n");
   int bsize = 5; // bordersize
   
   wxFlexGridSizer *mflexGridSizer = new wxFlexGridSizer( 3, 0, 0 );
   wxBoxSizer *horizontalSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *outputSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *inputSizer = new wxBoxSizer(wxHORIZONTAL);

   wxStaticText *equalSign =
                     new wxStaticText( this, ID_TEXT, wxT("  =  "),
                     wxDefaultPosition, wxDefaultSize, 0 );

   // wxGrid
   inputGrid =
      new wxGrid( this, -1, wxDefaultPosition, wxSize(200, 23), wxWANTS_CHARS );

   inputGrid->CreateGrid( 1, 1, wxGrid::wxGridSelectRows );
   inputGrid->SetColSize(0, 200);
   inputGrid->SetRowSize(0, 23);
   inputGrid->SetColLabelSize(0);
   inputGrid->SetRowLabelSize(0);
   inputGrid->SetMargins(0, 0);
   inputGrid->SetScrollbars(0, 0, 0, 0, 0, 0, FALSE);
   inputGrid->EnableEditing(false);

   outputGrid =
      new wxGrid( this, -1, wxDefaultPosition, wxSize(200, 23), wxWANTS_CHARS );

   outputGrid->CreateGrid( 1, 1, wxGrid::wxGridSelectRows );
   outputGrid->SetColSize(0, 200);
   outputGrid->SetRowSize(0, 23);
   outputGrid->SetColLabelSize(0);
   outputGrid->SetRowLabelSize(0);
   outputGrid->SetMargins(0, 0);
   outputGrid->SetScrollbars(0, 0, 0, 0, 0, 0, FALSE);
   outputGrid->EnableEditing(false);

   outputSizer->Add(outputGrid, 0, wxALIGN_CENTRE|wxALL, bsize);

   inputSizer->Add(inputGrid, 0, wxALIGN_CENTRE|wxALL, bsize);

   horizontalSizer->Add(equalSign, 0, wxALIGN_CENTRE|wxALL, bsize);

   mflexGridSizer->Add(outputSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mflexGridSizer->Add(horizontalSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mflexGridSizer->Add(inputSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

//   theMiddleSizer->Add(outputSizer, 0, wxALIGN_CENTER|wxALL, bsize);
//   theMiddleSizer->Add(horizontalSizer, 0, wxALIGN_CENTER|wxALL, bsize);
//   theMiddleSizer->Add(mHorizontalSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
//   theMiddleSizer->Add(inputSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   theMiddleSizer->Add(mflexGridSizer, 0, wxALIGN_CENTER|wxALL, bsize);

}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void AssignmentPanel::LoadData()
{

}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void AssignmentPanel::SaveData()
{

}

//------------------------------------------------------------------------------
// void OnCellClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void AssignmentPanel::OnCellClick(wxGridEvent& event)
{
}

//------------------------------------------------------------------------------
// void OnComboChange()
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void AssignmentPanel::OnComboChange(wxCommandEvent &event)
{
    EnableUpdate(true);
}
