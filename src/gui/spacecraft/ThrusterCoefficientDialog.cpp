//$Header: 
//------------------------------------------------------------------------------
//                              ThrusterCoefficientDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2005/01/13
//
/**
 * Implements ThrusterCoefficientDialog class. This class shows dialog window where
 * thruster coefficients can be modified.
 * 
 */
//------------------------------------------------------------------------------

#include "ThrusterCoefficientDialog.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(ThrusterCoefficientDialog, GmatDialog)
   EVT_GRID_SELECT_CELL(ThrusterCoefficientDialog::OnCellValueChange)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ThrusterCoefficientDialog(wxWindow *parent)
//------------------------------------------------------------------------------
ThrusterCoefficientDialog::ThrusterCoefficientDialog(wxWindow *parent)
   : GmatDialog(parent, -1, wxString(_T("ThrusterCoefficientDialog")))
{   
   Create();
   ShowData();
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void ThrusterCoefficientDialog::Create()
{
   coefficientGrid = new wxGrid( this, ID_GRID, wxDefaultPosition, 
                    wxSize(200,315), wxWANTS_CHARS );
   coefficientGrid->CreateGrid( 14, 2, wxGrid::wxGridSelectCells );
   coefficientGrid->SetRowLabelSize(0);
   coefficientGrid->SetDefaultCellAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
   coefficientGrid->EnableEditing(true);
   
   coefficientGrid->SetColLabelValue(0, _T("Coef"));
   coefficientGrid->SetColSize(0, 60);
   coefficientGrid->SetColLabelValue(1, _T("in Newton"));
   coefficientGrid->SetColSize(1, 140);
   
   // wxSizers   
   theMiddleSizer->Add(coefficientGrid, 0, wxALIGN_CENTRE|wxALL, 3);
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ThrusterCoefficientDialog::LoadData()
{
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ThrusterCoefficientDialog::SaveData()
{
}  

//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void ThrusterCoefficientDialog::ResetData()
{
}     

//------------------------------------------------------------------------------
// virtual void OnCellValueChange()
//------------------------------------------------------------------------------
void ThrusterCoefficientDialog::OnCellValueChange()
{
}
@