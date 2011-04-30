//$Id$
//------------------------------------------------------------------------------
//                              DoWhilePanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: 
// Created: 
/**
 * 
 */
//------------------------------------------------------------------------------

#include "DoWhilePanel.hpp"
#include "ParameterSelectDialog.hpp"
#include "gmatdefs.hpp"
#include "GmatAppData.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(DoWhilePanel, GmatPanel)
    EVT_GRID_CELL_LEFT_CLICK(DoWhilePanel::OnCellLeftClick)
//    EVT_GRID_CELL_RIGHT_CLICK(DoWhilePanel::OnCellRightClick)
    EVT_GRID_CELL_LEFT_DCLICK(DoWhilePanel::OnCellDoubleLeftClick)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// DoWhilePanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
DoWhilePanel::DoWhilePanel( wxWindow *parent ) : GmatPanel(parent)
{
   Create();
   Show();
}

DoWhilePanel::~DoWhilePanel()
{
}

//-------------------------------
// private methods
//-------------------------------
void DoWhilePanel::Create()
{
    Setup(this);    
}

void DoWhilePanel::Setup( wxWindow *parent)
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
    conditionGrid->SetCellValue(0, 0, "Do");
        
    item0->Add( conditionGrid, 0, wxALIGN_CENTER|wxALL, 5 );

    theMiddleSizer->Add(item0, 0, wxGROW, 5);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void DoWhilePanel::LoadData()
{
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void DoWhilePanel::SaveData()
{
}

//------------------------------------------------------------------------------
// void OnCellLeftClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void DoWhilePanel::OnCellLeftClick(wxGridEvent& event)
{
    int row = event.GetRow();
    
    if (event.GetEventObject() == conditionGrid)
    {
        conditionGrid->SelectRow(row);
    }
}

//------------------------------------------------------------------------------
// void OnCellDoubleLeftClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void DoWhilePanel::OnCellDoubleLeftClick(wxGridEvent& event)
{
   if (event.GetEventObject() == conditionGrid)
   {
      int row = event.GetRow();
      int col = event.GetCol();

      if ( col == 0 )
      {
         wxString strArray[] =
         {
            wxT("While")
         };
         
         wxSingleChoiceDialog dialog(this, _T("Condition: \n"),
                                        _T("DoWhileConditionDialog"), 1, strArray);
         dialog.SetSelection(0);

         if (dialog.ShowModal() == wxID_OK)
         {
            if (dialog.GetStringSelection() != conditionGrid->GetCellValue(row, col))
            {
               conditionGrid->SetCellValue(1, 0, dialog.GetStringSelection());
               EnableUpdate(true);
            }   
         }  
      }   
      else if ( (col == 1) || (col == 3))
      {
         // show dialog to select parameter
         ParameterSelectDialog paramDlg(this);
         paramDlg.ShowModal();

         if (paramDlg.IsParamSelected())
         {
            wxString newParamName = paramDlg.GetParamName();
            conditionGrid->SetCellValue(row, col, newParamName);
            EnableUpdate(true);
         }
      }
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
                                        _T("DoWhileConditionDialog"), 6, strArray);
         dialog.SetSelection(0);

         if (dialog.ShowModal() == wxID_OK)
         {
            if (dialog.GetStringSelection() != conditionGrid->GetCellValue(row, col))
            {
               conditionGrid->SetCellValue(row, col, dialog.GetStringSelection());
               EnableUpdate(true);
            }
         }   
      }
   }
}

//------------------------------------------------------------------------------
// void OnCellRightClick(wxGridEvent& event)
//------------------------------------------------------------------------------
//void DoWhilePanel::OnCellRightClick(wxGridEvent& event)
//{
//    int row = event.GetRow();
//    int col = event.GetCol();
//    
//    if (event.GetEventObject() == conditionGrid)
//    {
//      // AND/OR/ENDIF 
//      if (col == 0)
//      {
//         if (row > 0)
//         {
//            wxString strArray[] =
//            {
//               wxT("&"),
//               wxT("|"),
//               wxT("~"),
//               wxT("END")
//            };
//         
//            wxSingleChoiceDialog dialog(this, _T("Condition: \n"),
//                                        _T("ConditionDialog"), 3, strArray);
//            dialog.SetSelection(0);
//
//            if (dialog.ShowModal() == wxID_OK)
//            {
//              if (dialog.GetStringSelection() != conditionGrid->GetCellValue(row, col))
//              {
//                conditionGrid->SetCellValue(row, col, dialog.GetStringSelection());
//                EnableUpdate(true);
//              }   
//            }   
//         }   
//      }
//    }       
//}


