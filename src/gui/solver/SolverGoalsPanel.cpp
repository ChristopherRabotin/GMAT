//$Header$
//------------------------------------------------------------------------------
//                           SolverGoalsPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/01/20
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
//
/**
 * This class contains the Solver Event window.
 */
//------------------------------------------------------------------------------

// gui includes
#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/grid.h>

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "GmatMainNotebook.hpp"
#include "SolverGoalsPanel.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(SolverGoalsPanel, wxPanel)
    EVT_BUTTON(ID_BUTTON, SolverGoalsPanel::OnButton)
    EVT_TEXT(ID_TEXTCTRL, SolverGoalsPanel::OnTextUpdate)
    EVT_COMBOBOX(ID_COMBO, SolverGoalsPanel::OnSolverSelection)
    EVT_GRID_CELL_CHANGE(SolverGoalsPanel::OnCellValueChanged)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// SolverGoalsPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
SolverGoalsPanel::SolverGoalsPanel(wxWindow *parent)
    : wxPanel(parent)
{
    Initialize();
    Setup(this);
    GetData();
}

void SolverGoalsPanel::Initialize()
{
    numOfGoals = 20;  //TBD
    nextRow = 0;
}

void SolverGoalsPanel::Setup( wxWindow *parent)
{
    // wxGrid
    goalsGrid = new wxGrid( parent, ID_GRID, wxDefaultPosition, wxSize(600,160), wxWANTS_CHARS );
    goalsGrid->CreateGrid( numOfGoals, 3, wxGrid::wxGridSelectCells );
    goalsGrid->SetColSize(0, 150);
    goalsGrid->SetColSize(1, 585);
    goalsGrid->SetColSize(2, 585);
    goalsGrid->SetColLabelValue(0, _T("Solver"));
    goalsGrid->SetColLabelValue(1, _T("Property"));
    goalsGrid->SetColLabelValue(2, _T("Description"));
    goalsGrid->SetRowLabelSize(0);
 
    // wxStaticText
    descStaticText = new wxStaticText( parent, ID_TEXT, wxT("Description"), wxDefaultPosition, wxDefaultSize, 0 );
    solverStaticText = new wxStaticText( parent, ID_TEXT, wxT("Solver"), wxDefaultPosition, wxDefaultSize, 0 );
    varStaticText = new wxStaticText( parent, ID_TEXT, wxT("Variable"), wxDefaultPosition, wxDefaultSize, 0 );
    valueStaticText = new wxStaticText( parent, ID_TEXT, wxT("Desired Value"), wxDefaultPosition, wxDefaultSize, 0 );
    tolStaticText = new wxStaticText( parent, ID_TEXT, wxT("Tol."), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxTextCtrl
    descTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(400,-1), 0 );
    varTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(400,-1), 0 );
    valueTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    tolTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    
    // wxString
    wxString strArray1[] = 
    {
        wxT("")
    };
    
    // wxComboBox
    solverComboBox = new wxComboBox( parent, ID_COMBO, wxT(strArray1[0]), wxDefaultPosition, wxSize(200,-1), 1, strArray1, wxCB_DROPDOWN|wxCB_READONLY );

    // wxButton
    editButton = new wxButton( parent, ID_BUTTON, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
    updateButton = new wxButton( parent, ID_BUTTON, wxT("Update"), wxDefaultPosition, wxDefaultSize, 0 );
    okButton = new wxButton( parent, ID_BUTTON, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    applyButton = new wxButton( parent, ID_BUTTON, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    cancelButton = new wxButton( parent, ID_BUTTON, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    helpButton = new wxButton( parent, ID_BUTTON, wxT("Help"), wxDefaultPosition, wxDefaultSize, 0 );

    // wx*Sizers
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
    wxStaticBox *item2 = new wxStaticBox( parent, -1, wxT("Goals List") );
    wxStaticBoxSizer *item1 = new wxStaticBoxSizer( item2, wxVERTICAL );
    wxStaticBox *item5 = new wxStaticBox( parent, -1, wxT("Goal Setup") );
    wxStaticBoxSizer *item4 = new wxStaticBoxSizer( item5, wxVERTICAL );
    wxFlexGridSizer *item6 = new wxFlexGridSizer( 3, 0, 0 );
    wxStaticBox *item15 = new wxStaticBox( parent, -1, wxT("Settings") );
    wxStaticBoxSizer *item14 = new wxStaticBoxSizer( item15, wxHORIZONTAL );
    wxFlexGridSizer *item16 = new wxFlexGridSizer( 4, 0, 0 );
    wxBoxSizer *item22 = new wxBoxSizer( wxHORIZONTAL );
    
    // Add to wx*Sizers
    item1->Add( goalsGrid, 0, wxALIGN_CENTER|wxALL, 5 );
    
    item6->Add( descStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    item6->Add( descTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    item6->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );
    item6->Add( solverStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    item6->Add( solverComboBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    item6->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );
    item6->Add( varStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    item6->Add( varTextCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    item6->Add( editButton, 0, wxALIGN_CENTER|wxALL, 5 );
        
    item16->Add( valueStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( tolStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( 100, 20, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( 100, 20, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( valueTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( tolTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( 100, 20, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( 100, 20, 0, wxALIGN_CENTER|wxALL, 5 );
    
    item14->Add( item16, 0, wxALIGN_CENTER|wxALL, 5 );
    item4->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );
    item4->Add( item14, 0, wxALIGN_CENTER|wxALL, 5 );
    item4->Add( updateButton, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    
    item22->Add( okButton, 0, wxALIGN_CENTER|wxALL, 5 );
    item22->Add( applyButton, 0, wxALIGN_CENTER|wxALL, 5 );
    item22->Add( cancelButton, 0, wxALIGN_CENTER|wxALL, 5 );
    item22->Add( helpButton, 0, wxALIGN_CENTER|wxALL, 5 );
   
    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );
    item0->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );
    item0->Add( item22, 0, wxALIGN_CENTER|wxALL, 5 );
    
    parent->SetAutoLayout( true );
    parent->SetSizer( item0 );
    item0->Fit( parent );
    item0->SetSizeHints( parent );
    
    applyButton->Enable(false);
    
    // Future implementation
    helpButton->Enable(false);
}

void SolverGoalsPanel::GetData()
{    
    return;
}

void SolverGoalsPanel::SetData()
{
    return;
}

void SolverGoalsPanel::OnTextUpdate(wxCommandEvent& event)
{
    applyButton->Enable(true);
}

void SolverGoalsPanel::OnSolverSelection()
{
    applyButton->Enable(true);
}

void SolverGoalsPanel::OnButton(wxCommandEvent& event)
{
    if ( event.GetEventObject() == editButton )  
    {       
        // Bring up the VariableCreatePanel
        applyButton->Enable(true);
    }
    else if ( event.GetEventObject() == updateButton )
    {      
        solverString = solverComboBox->GetStringSelection();
        propertyString = varTextCtrl->GetValue();
        descriptionString = descTextCtrl->GetValue();
        
        goalsGrid->SetCellValue(nextRow, SOL_COL, solverString);
        goalsGrid->SetCellValue(nextRow, PRO_COL, propertyString);
        goalsGrid->SetCellValue(nextRow, DES_COL, descriptionString);
        
        nextRow++;
    }
    else if ( event.GetEventObject() == okButton )  
    {
        SetData(); 
        GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
        gmatMainNotebook->ClosePage();       
    }
    else if ( event.GetEventObject() == applyButton )
    {
        SetData();
        applyButton->Enable(false);
    } 
    else if ( event.GetEventObject() == cancelButton )
    {
        GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
        gmatMainNotebook->ClosePage();
    }      
    else if ( event.GetEventObject() == helpButton )           
        ; 
    else
        event.Skip();
}

void SolverGoalsPanel::OnCellValueChanged()
{ 
    applyButton->Enable(true);
}
