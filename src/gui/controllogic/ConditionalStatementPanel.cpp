//$Header$
//------------------------------------------------------------------------------
//                              ConditionalStatementPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2004/05/17
/**
 * This class contains the Conditional Statement Setup window.
 */
//------------------------------------------------------------------------------
#include "ConditionalStatementPanel.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GmatAppData.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ConditionalStatementPanel, GmatPanel)
    EVT_GRID_CELL_LEFT_CLICK(ConditionalStatementPanel::OnCellLeftClick)
    EVT_GRID_CELL_RIGHT_CLICK(ConditionalStatementPanel::OnCellRightClick)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ConditionalStatementPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
ConditionalStatementPanel::ConditionalStatementPanel( wxWindow *parent, 
                           const wxString &condition)
    : GmatPanel(parent)
{
   conditionStatement = condition;
   Create();
   Show();
}

ConditionalStatementPanel::~ConditionalStatementPanel()
{
}

//-------------------------------
// private methods
//-------------------------------
void ConditionalStatementPanel::Create()
{
    Setup(this);    
}

void ConditionalStatementPanel::Setup( wxWindow *parent)
{
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    conditionGrid = new wxGrid( parent, ID_GRID, wxDefaultPosition, 
                     wxSize(454,238), wxWANTS_CHARS );
    conditionGrid->CreateGrid( 10, 4, wxGrid::wxGridSelectCells );
    conditionGrid->SetRowLabelSize(0);
    conditionGrid->SetDefaultCellAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
    conditionGrid->EnableEditing(false);
        
    conditionGrid->SetColLabelValue(0, _T(""));
    conditionGrid->SetColSize(0, 60);
    conditionGrid->SetColLabelValue(1, _T("LHS"));
    conditionGrid->SetColSize(1, 165);
    conditionGrid->SetColLabelValue(2, _T("Condition"));
    conditionGrid->SetColSize(2, 60);
    conditionGrid->SetColLabelValue(3, _T("RHS"));
    conditionGrid->SetColSize(3, 165);
    conditionGrid->SetCellValue(0, 0, conditionStatement);
        
    item0->Add( conditionGrid, 0, wxALIGN_CENTER|wxALL, 5 );

    theMiddleSizer->Add(item0, 0, wxGROW, 5);
   
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ConditionalStatementPanel::LoadData()
{
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ConditionalStatementPanel::SaveData()
{
}


//loj: 3/1/04 added for build2
//------------------------------------------------------------------------------
// void OnCellLeftClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void ConditionalStatementPanel::OnCellLeftClick(wxGridEvent& event)
{
    int row = event.GetRow();
    
    if (event.GetEventObject() == conditionGrid)
    {
        conditionGrid->SelectRow(row);
    }
}

//------------------------------------------------------------------------------
// void OnCellRightClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void ConditionalStatementPanel::OnCellRightClick(wxGridEvent& event)
{
    int row = event.GetRow();
    int col = event.GetCol();
    
    if (event.GetEventObject() == conditionGrid)
    {
      // AND/OR/ENDIF 
      if (col == 0)
      {
         if (row > 0)
         {
            wxString strArray[] =
            {
               wxT("AND"),
               wxT("OR"),
               wxT("END")
            };
         
            wxSingleChoiceDialog dialog(this, _T("Condition: \n"),
                                        _T("ConditionDialog"), 3, strArray);
            dialog.SetSelection(0);

            if (dialog.ShowModal() == wxID_OK)
            {
              if (dialog.GetStringSelection() != conditionGrid->GetCellValue(row, col))
              {
                conditionGrid->SetCellValue(row, col, dialog.GetStringSelection());
                theApplyButton->Enable(true);
              }   
            }   
         }   
      }
      // LHS
      else if (col == 1)
      {
      }
      // condition
      else if (col == 2)
      {
         wxString strArray[] =
         {
            wxT("="),
            wxT(">"),
            wxT("<"),
            wxT(">="),
            wxT("<="),
            wxT("!=")
         };
         
         wxSingleChoiceDialog dialog(this, _T("Condition: \n"),
                                        _T("WhileConditionDialog"), 6, strArray);
         dialog.SetSelection(0);

         if (dialog.ShowModal() == wxID_OK)
         {
            if (dialog.GetStringSelection() != conditionGrid->GetCellValue(row, col))
            {
               conditionGrid->SetCellValue(row, col, dialog.GetStringSelection());
               theApplyButton->Enable(true);
            }
         }   
      }
      // RHS
      else if (col == 3)
      {
      }
      else
      {
         // error
      }  
    }       
}


