//$Header$
//------------------------------------------------------------------------------
//                              ForLoopPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2004/05/17
/**
 * This class contains the Conditional Statement Setup window.
 */
//------------------------------------------------------------------------------
#include "ForLoopPanel.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GmatAppData.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ForLoopPanel, GmatPanel)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ForLoopPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
ForLoopPanel::ForLoopPanel( wxWindow *parent)
    : GmatPanel(parent)
{
   Create();
   Show();
}

ForLoopPanel::~ForLoopPanel()
{
}

//-------------------------------
// private methods
//-------------------------------
void ForLoopPanel::Create()
{
    Setup(this);    
}

void ForLoopPanel::Setup( wxWindow *parent)
{
//    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
//
//    wxFlexGridSizer *item1 = new wxFlexGridSizer( 2, 0, 0 );
//
//    wxStaticText *item2 = new wxStaticText( parent, ID_TEXT, wxT("Start"), 
//               wxDefaultPosition, wxDefaultSize, 0 );
//    item1->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );
//
//    wxTextCtrl *item3 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), 
//               wxDefaultPosition, wxSize(80,-1), 0 );
//    item1->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );
//
//    wxStaticText *item4 = new wxStaticText( parent, ID_TEXT, wxT("End"), 
//               wxDefaultPosition, wxDefaultSize, 0 );
//    item1->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );
//
//    wxTextCtrl *item5 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), 
//               wxDefaultPosition, wxSize(80,-1), 0 );
//    item1->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );
//
//    wxStaticText *item6 = new wxStaticText( parent, ID_TEXT, wxT("Step Size"), 
//               wxDefaultPosition, wxDefaultSize, 0 );
//    item1->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );
//
//    wxTextCtrl *item7 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), 
//               wxDefaultPosition, wxSize(80,-1), 0 );
//    item1->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );
//
//    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );
//
//    theMiddleSizer->Add(item0, 0, wxGROW, 5);
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    conditionGrid = new wxGrid( parent, ID_GRID, wxDefaultPosition, 
                     wxSize(455,54), wxWANTS_CHARS );
    conditionGrid->CreateGrid( 1, 4, wxGrid::wxGridSelectCells );
    conditionGrid->SetRowLabelSize(0);
    conditionGrid->SetDefaultCellAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
    conditionGrid->EnableEditing(false);
        
    conditionGrid->SetColLabelValue(0, _T(""));
    conditionGrid->SetColSize(0, 60);
    conditionGrid->SetColLabelValue(1, _T("Start"));
    conditionGrid->SetColSize(1, 165);
    conditionGrid->SetColLabelValue(2, _T("Increment"));
    conditionGrid->SetColSize(2, 60);
    conditionGrid->SetColLabelValue(3, _T("End"));
    conditionGrid->SetColSize(3, 165);
    conditionGrid->SetCellValue(0, 0, "FOR");
    
    item0->Add( conditionGrid, 0, wxALIGN_CENTER|wxALL, 5 );

    theMiddleSizer->Add(item0, 0, wxGROW, 5);
   
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ForLoopPanel::LoadData()
{
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ForLoopPanel::SaveData()
{
}


