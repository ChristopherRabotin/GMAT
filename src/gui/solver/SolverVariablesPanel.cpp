//------------------------------------------------------------------------------
//                           SolverVariablesPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/21/01
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
//
/**
 * This class contains the Solver Variables window.
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
#include "SolverVariablesPanel.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(SolverVariablesPanel, wxPanel)
    EVT_BUTTON(ID_BUTTON, SolverVariablesPanel::OnButton)
    EVT_TEXT(ID_TEXTCTRL, SolverVariablesPanel::OnTextUpdate)
    EVT_COMBOBOX(ID_COMBO, SolverVariablesPanel::OnSolverSelection)
    EVT_GRID_CELL_CHANGE(SolverVariablesPanel::OnCellValueChanged)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// SolverVariablesPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
SolverVariablesPanel::SolverVariablesPanel(wxWindow *parent)
{
    Initialize();
    Setup(this);
    GetData();
}

void SolverVariablesPanel::Initialize()
{
    numOfVars = 20;    // TBD
    nextRow = 0;
}

void SolverVariablesPanel::Setup( wxWindow *parent)
{  
    // wxGrid
    varsGrid = new wxGrid( parent, ID_GRID, wxDefaultPosition, wxSize(600,160), wxWANTS_CHARS );
    varsGrid->CreateGrid( numOfVars, 3, wxGrid::wxGridSelectCells );
    varsGrid->SetColSize(0, 150);
    varsGrid->SetColSize(1, 585);
    varsGrid->SetColSize(2, 585);
    varsGrid->SetColLabelValue(0, _T("Solver"));
    varsGrid->SetColLabelValue(1, _T("Property"));
    varsGrid->SetColLabelValue(2, _T("Description"));
    varsGrid->SetRowLabelSize(0);
       
    // wxStaticText
    descStaticText = new wxStaticText( parent, ID_TEXT, wxT("Description"), wxDefaultPosition, wxDefaultSize, 0 );
    solverStaticText = new wxStaticText( parent, ID_TEXT, wxT("Solver"), wxDefaultPosition, wxDefaultSize, 0 );
    varStaticText = new wxStaticText( parent, ID_TEXT, wxT("Variable"), wxDefaultPosition, wxDefaultSize, 0 );
    pertStaticText = new wxStaticText( parent, ID_TEXT, wxT("Perturbation"), wxDefaultPosition, wxDefaultSize, 0 );
    maxStaticText = new wxStaticText( parent, ID_TEXT, wxT("Max Step"), wxDefaultPosition, wxDefaultSize, 0 );
    lowerStaticText = new wxStaticText( parent, ID_TEXT, wxT("Lower Bound"), wxDefaultPosition, wxDefaultSize, 0 );
    upperStaticText = new wxStaticText( parent, ID_TEXT, wxT("Upper Bound"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxTextCtrl
    descTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(400,-1), 0 );
    varTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(400,-1), 0 );
    pertTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    maxTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    lowerTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    upperTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    
    // wxString
    wxString strArray1[] = 
    {
        wxT("")
    };
    
    // wxComboBox
    solverComboBox = new wxComboBox( parent, ID_COMBO, wxT(strArray1[0]), wxDefaultPosition, wxSize(200,-1), 0, strArray1, wxCB_DROPDOWN|wxCB_READONLY );
    
    // wxButton
    editButton = new wxButton( parent, ID_BUTTON, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
    updateButton = new wxButton( parent, ID_BUTTON, wxT("Update"), wxDefaultPosition, wxDefaultSize, 0 );
    okButton = new wxButton( parent, ID_BUTTON, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    applyButton = new wxButton( parent, ID_BUTTON, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    cancelButton = new wxButton( parent, ID_BUTTON, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    helpButton = new wxButton( parent, ID_BUTTON, wxT("Help"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wx*Sizers
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
    wxStaticBox *item2 = new wxStaticBox( parent, -1, wxT("Variable List") );
    wxStaticBoxSizer *item1 = new wxStaticBoxSizer( item2, wxVERTICAL );
    wxStaticBox *item5 = new wxStaticBox( parent, -1, wxT("Variable Setup") );
    wxStaticBoxSizer *item4 = new wxStaticBoxSizer( item5, wxVERTICAL );
    wxFlexGridSizer *item6 = new wxFlexGridSizer( 3, 0, 0 );
    item6->AddGrowableCol( 1 );
    wxStaticBox *item15 = new wxStaticBox( parent, -1, wxT("Settings") );
    wxStaticBoxSizer *item14 = new wxStaticBoxSizer( item15, wxHORIZONTAL );
    wxGridSizer *item16 = new wxGridSizer( 4, 0, 0 );
    wxBoxSizer *item26 = new wxBoxSizer( wxHORIZONTAL );
    
    // Add to wx*Sizers
    item1->Add( varsGrid, 0, wxALIGN_CENTER|wxALL, 5 );
    
    item6->Add( descStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    item6->Add( descTextCtrl, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    item6->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );
    item6->Add( solverStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    item6->Add( solverComboBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    item6->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );
    item6->Add( varStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    item6->Add( varTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    item6->Add( editButton, 0, wxALIGN_CENTER|wxALL, 5 );    
    
    item16->Add( pertStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( maxStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( lowerStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( upperStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( pertTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( maxTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( lowerTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( upperTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );

    item14->Add( item16, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
    
    item4->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );
    item4->Add( item14, 0, wxALIGN_CENTER|wxALL, 5 );
    item4->Add( updateButton, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item26->Add( okButton, 0, wxALIGN_CENTER|wxALL, 5 );
    item26->Add( applyButton, 0, wxALIGN_CENTER|wxALL, 5 );
    item26->Add( cancelButton, 0, wxALIGN_CENTER|wxALL, 5 );
    item26->Add( helpButton, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );
    item0->Add( item4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    item0->Add( item26, 0, wxALIGN_CENTER|wxALL, 5 );
    
    parent->SetAutoLayout( TRUE );
    parent->SetSizer( item0 );
    
    item0->Fit( parent );
    item0->SetSizeHints( parent );
    
    applyButton->Enable(false);
    
    // Future implementation
    helpButton->Enable(false);
}

void SolverVariablesPanel::GetData()
{    
    return;
}

void SolverVariablesPanel::SetData()
{
    return;
}

void SolverVariablesPanel::OnTextUpdate(wxCommandEvent& event)
{
    if ( event.GetEventObject() == descTextCtrl )  
    {       
        applyButton->Enable(true);
    }
    else if ( event.GetEventObject() == varTextCtrl )
    {      
        applyButton->Enable(true);
    }
    else if ( event.GetEventObject() == pertTextCtrl )  
    {
        applyButton->Enable(true);       
    }
    else if ( event.GetEventObject() == maxTextCtrl )
    {
        applyButton->Enable(true);  
    } 
    else if ( event.GetEventObject() == lowerTextCtrl )
    {
        applyButton->Enable(true);  
    }      
    else if ( event.GetEventObject() == upperTextCtrl )       
    {    
        applyButton->Enable(true); 
    }
    else
        event.Skip();
}

void SolverVariablesPanel::OnSolverSelection()
{
    applyButton->Enable(true);
}

void SolverVariablesPanel::OnButton(wxCommandEvent& event)
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
        
        varsGrid->SetCellValue(nextRow, SOL_COL, solverString);
        varsGrid->SetCellValue(nextRow, PRO_COL, propertyString);
        varsGrid->SetCellValue(nextRow, DES_COL, descriptionString);
        
        nextRow++;
    }
    else if ( event.GetEventObject() == okButton )  
    {
        SetData(); 
        Close(this);        
    }
    else if ( event.GetEventObject() == applyButton )
    {
        SetData();
        applyButton->Enable(false);
    } 
    else if ( event.GetEventObject() == cancelButton )
    {
        Close(this);
    }      
    else if ( event.GetEventObject() == helpButton )           
        ; 
    else
        event.Skip();
}

void SolverVariablesPanel::OnCellValueChanged()
{ 
    applyButton->Enable(true);
}
