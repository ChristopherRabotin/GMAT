//$Header$
//------------------------------------------------------------------------------
//                              CallFunctionPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2004/9/15
/**
 * This class contains the Matlab Command Setup window.
 */
//------------------------------------------------------------------------------

#include "CallFunctionPanel.hpp"
#include "MessageInterface.hpp"

#define DEBUG_PROPCMD_PANEL 0

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CallFunctionPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// CallFunctionPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
CallFunctionPanel::CallFunctionPanel( wxWindow *parent, const wxString &propName)
   : GmatPanel(parent)
{
   Create();
   Show();
   theApplyButton->Disable();
}

//------------------------------------------------------------------------------
// ~CallFunctionPanel()
//------------------------------------------------------------------------------
CallFunctionPanel::~CallFunctionPanel()
{
}

//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void CallFunctionPanel::Create()
{
   //MessageInterface::ShowMessage("CallFunctionPanel::Create() entered\n");
   int bsize = 5; // bordersize
   
   wxGridSizer *mHorizontalSizer = new wxGridSizer( 2, 0, 0 );
   wxBoxSizer *horizontalSizer = new wxBoxSizer(wxHORIZONTAL);

   // wxStaticText
   nameStaticText = new wxStaticText( this, ID_TEXT, wxT("Function Name:  "),
                     wxDefaultPosition, wxDefaultSize, 0 );

   StringArray &list =
         theGuiInterpreter->GetListOfConfiguredItems(Gmat::FUNCTION);
   int size = list.size();
   wxString *choices = new wxString[size];

   for (int i=0; i<size; i++)
   {
        choices[i] = list[i].c_str();
   }

    // combo box for the date type
   functionComboBox = new wxComboBox( this, ID_COMBO, wxT(""),
             wxDefaultPosition, wxSize(130,-1), size, choices,
             wxCB_DROPDOWN | wxCB_READONLY );



   // wxGrid
   inputGrid =
      new wxGrid( this, -1, wxDefaultPosition, wxSize(250, 275), wxWANTS_CHARS );

   inputGrid->CreateGrid( 15, 1, wxGrid::wxGridSelectRows );
   inputGrid->SetColSize(0, 270);
   inputGrid->SetColLabelValue(0, _T("Input"));
   inputGrid->SetRowLabelSize(0);
   inputGrid->EnableEditing(false);

   outputGrid =
      new wxGrid( this, -1, wxDefaultPosition, wxSize(250, 275), wxWANTS_CHARS );

   outputGrid->CreateGrid( 15, 1, wxGrid::wxGridSelectRows );
   outputGrid->SetColSize(0, 270);
   outputGrid->SetColLabelValue(0, _T("Output"));
   outputGrid->SetRowLabelSize(0);
   outputGrid->EnableEditing(false);

   horizontalSizer->Add(nameStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   horizontalSizer->Add(functionComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);

   mHorizontalSizer->Add(inputGrid, 0, wxALIGN_CENTRE|wxALL, bsize);
   mHorizontalSizer->Add(outputGrid, 0, wxALIGN_CENTRE|wxALL, bsize);

   theMiddleSizer->Add(horizontalSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   theMiddleSizer->Add(mHorizontalSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void CallFunctionPanel::LoadData()
{
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void CallFunctionPanel::SaveData()
{
}

