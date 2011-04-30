//$Id$
//------------------------------------------------------------------------------
//                              ConditionPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2011/01/06 based on IfPanel which was developed by Waka Waktola
//
/**
 * This class contains the condition command such as If and While setup window.
 */
//------------------------------------------------------------------------------

#include "ConditionPanel.hpp"
#include "ParameterSelectDialog.hpp"
#include "gmatdefs.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PANEL_SAVE 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ConditionPanel, GmatPanel)
    EVT_GRID_CELL_LEFT_CLICK(ConditionPanel::OnCellLeftClick)
    EVT_GRID_CELL_RIGHT_CLICK(ConditionPanel::OnCellRightClick)
    EVT_GRID_CELL_CHANGE(ConditionPanel::OnCellValueChange)  
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ConditionPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
ConditionPanel::ConditionPanel(wxWindow *parent, GmatCommand *cmd) : GmatPanel(parent)
{
   theCommand = (ConditionalBranch*)cmd;
   
   mNumberOfConditions = 0;
   mNumberOfLogicalOps = 0;
   
   mObjectTypeList.Add("Spacecraft");
   mLhsList.clear();
   mEqualityOpStrings.clear();
   mRhsList.clear();
   mLogicalOpStrings.clear();
   mLhsIsParam.clear();
   mRhsIsParam.clear();
   
   Create();
   Show();
}

//------------------------------------------------------------------------------
// ConditionPanel()
//------------------------------------------------------------------------------
/**
 * A destructor.
 */
//------------------------------------------------------------------------------
ConditionPanel::~ConditionPanel()
{
   mObjectTypeList.Clear();
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void ConditionPanel::Create()
{
   wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
   
   conditionGrid = new wxGrid( this, ID_GRID, wxDefaultPosition, 
                               wxDefaultSize, wxWANTS_CHARS );
   
   conditionGrid->CreateGrid( MAX_ROW, MAX_COL, wxGrid::wxGridSelectCells );
   conditionGrid->SetRowLabelSize(0);
   conditionGrid->SetDefaultCellAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
   wxColour gridColor = wxTheColourDatabase->Find("DIM GREY");
   conditionGrid->SetGridLineColour(gridColor);
   
   conditionGrid->SetColLabelValue(COMMAND_COL, _T(""));
   conditionGrid->SetColLabelValue(LHS_SEL_COL, _T(""));
   conditionGrid->SetColLabelValue(LHS_COL, _T("Left Hand Side"));
   conditionGrid->SetColLabelValue(COND_COL, _T("Condition"));
   conditionGrid->SetColLabelValue(RHS_SEL_COL, _T(""));
   conditionGrid->SetColLabelValue(RHS_COL, _T("Right Hand Side"));
   conditionGrid->SetColSize(COMMAND_COL, 60);
   conditionGrid->SetColSize(LHS_SEL_COL, 25);
   conditionGrid->SetColSize(LHS_COL, 165);
#ifdef __WXMAC__
   conditionGrid->SetColSize(COND_COL, 80);
#else
   conditionGrid->SetColSize(COND_COL, 60);
#endif
   conditionGrid->SetColSize(RHS_SEL_COL, 25);
   conditionGrid->SetColSize(RHS_COL, 165);
   conditionGrid->SetCellValue(0, COMMAND_COL, theCommand->GetTypeName().c_str());

   // Set ... for LHS and RHS select columns
   for (Integer i = 0; i < MAX_ROW; i++)
   {
      conditionGrid->SetReadOnly(i, COMMAND_COL, true);
      conditionGrid->SetReadOnly(i, COND_COL, true);
      conditionGrid->SetCellBackgroundColour(i, LHS_SEL_COL, *wxLIGHT_GREY);
      conditionGrid->SetCellBackgroundColour(i, RHS_SEL_COL, *wxLIGHT_GREY);
      conditionGrid->SetCellValue(i, LHS_SEL_COL, _T("  ... "));
      conditionGrid->SetCellValue(i, RHS_SEL_COL, _T("  ... "));
   }
   
   item0->Add( conditionGrid, 0, wxALIGN_CENTER|wxALL, 0 );
   
   theMiddleSizer->Add(item0, 0, wxGROW, 0);
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ConditionPanel::LoadData()
{
   // Set the pointer for the "Show Script" button
   mObject = theCommand;
   
   if (theCommand == NULL)
      return;
   
   try
   {
      Integer paramId;
      
      paramId = theCommand->GetParameterID("NumberOfConditions");
      mNumberOfConditions = theCommand->GetIntegerParameter(paramId);
      
      if (mNumberOfConditions > 0)
      {
         paramId = theCommand->GetParameterID("NumberOfLogicalOperators");
         mNumberOfLogicalOps = theCommand->GetIntegerParameter(paramId); 
         
         paramId = theCommand->GetParameterID("LeftHandStrings");
         mLhsList = theCommand->GetStringArrayParameter(paramId);
         
         paramId = theCommand->GetParameterID("OperatorStrings");
         mEqualityOpStrings = theCommand->GetStringArrayParameter(paramId);
          
         paramId = theCommand->GetParameterID("RightHandStrings");
         mRhsList = theCommand->GetStringArrayParameter(paramId);
         
         paramId = theCommand->GetParameterID("LogicalOperators");
         mLogicalOpStrings = theCommand->GetStringArrayParameter(paramId); 
         
         for (Integer i = 0; i < mNumberOfConditions; i++)
         {
            conditionGrid->SetCellValue(i, LHS_COL, mLhsList[i].c_str()); 
            conditionGrid->SetCellValue(i, COND_COL, mEqualityOpStrings[i].c_str()); 
            conditionGrid->SetCellValue(i, RHS_COL, mRhsList[i].c_str());
            
            char leftChar = (mLhsList.at(i)).at(0);
            if (isalpha(leftChar))
               mLhsIsParam.push_back(true);
            else
               mLhsIsParam.push_back(false);
            
            char rightChar = (mRhsList.at(i)).at(0);
            if (isalpha(rightChar))
               mRhsIsParam.push_back(true);
            else
               mRhsIsParam.push_back(false);
            
            if (i != 0)
               conditionGrid->SetCellValue(i, COMMAND_COL,
                                           mLogicalOpStrings[i-1].c_str());
         }    
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ConditionPanel::SaveData()
{
   canClose = true;
   
   //-----------------------------------------------------------------
   // check for incomplete conditions
   //-----------------------------------------------------------------
   mNumberOfConditions = 0;
   Integer itemMissing = 0;
   mLogicalOpStrings.clear();
   mLhsList.clear();
   mEqualityOpStrings.clear();
   mRhsList.clear();
   
   // Count number of conditions
   for (Integer i=0; i<MAX_ROW; i++)
   {
      itemMissing = 0;
      for (Integer j=0; j<MAX_COL; j++)
      {
         if (conditionGrid->GetCellValue(i, j) == "")
            itemMissing++;
      }
      
      if (itemMissing == 0)
      {
         mNumberOfConditions++;
         mLogicalOpStrings.push_back(conditionGrid->GetCellValue(i, COMMAND_COL).c_str());
         mLhsList.push_back(conditionGrid->GetCellValue(i, LHS_COL).c_str());
         mEqualityOpStrings.push_back(conditionGrid->GetCellValue(i, COND_COL).c_str());
         mRhsList.push_back(conditionGrid->GetCellValue(i, RHS_COL).c_str());         
      }
      else if (itemMissing < 4)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_,
             "Logical operator or parameters are missing in row %d.\n", i+1);
         canClose = false;
         return;
      }
   }
   
   #ifdef DEBUG_PANEL_SAVE
   MessageInterface::ShowMessage
      ("ConditionPanel::SaveData() mNumberOfConditions=%d\n", mNumberOfConditions);
   #endif
   
   if (mNumberOfConditions == 0)
   {
      MessageInterface::PopupMessage
      (Gmat::WARNING_, "Incomplete parameters for If condition.\n"
                       "Updates have not been saved");
      canClose = false;
      return;
   }
   
   //-----------------------------------------------------------------
   // check input values: Number, Variable, Array element, Parameter
   //-----------------------------------------------------------------
   for (UnsignedInt i=0; i<mLhsList.size(); i++)
      CheckVariable(mLhsList[i].c_str(), Gmat::SPACECRAFT, "LHS",
                    "Variable, Array element, plottable Parameter", true);
   
   for (UnsignedInt i=0; i<mRhsList.size(); i++)
      CheckVariable(mRhsList[i].c_str(), Gmat::SPACECRAFT, "RHS",
                    "Variable, Array element, plottable Parameter", true);
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      for (Integer i = 0; i < mNumberOfConditions; i++)
      {
         #ifdef DEBUG_PANEL_SAVE
         MessageInterface::ShowMessage
            ("   i=%d, mLogicalOpStrings='%s', mLhsList='%s', mEqualityOpStrings='%s'\n"
             "   mRhsList='%s'\n", i, mLogicalOpStrings[i].c_str(), mLhsList[i].c_str(),
             mEqualityOpStrings[i].c_str(), mRhsList[i].c_str());
         #endif
         try
         {
            theCommand->SetCondition(mLhsList[i], mEqualityOpStrings[i],
                                     mRhsList[i], i);
            
            if (i > 0)
               theCommand->SetConditionOperator(mLogicalOpStrings[i], i-1);
            
         }
         catch (BaseException &be)
         {
            MessageInterface::PopupMessage(Gmat::ERROR_, be.GetFullMessage());
            canClose = false;
            // We want to go through all conditions and show errors if any
            //return; 
         }
      }
      
      if (canClose)
         if (!theGuiInterpreter->ValidateCommand(theCommand))
            canClose = false;
      
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
   }
}


//------------------------------------------------------------------------------
// void GetNewValue(Integer row, Integer col)
//------------------------------------------------------------------------------
void ConditionPanel::GetNewValue(Integer row, Integer col)
{
   wxString oldStr = conditionGrid->GetCellValue(row, col);
   ParameterSelectDialog paramDlg(this, mObjectTypeList);
   paramDlg.ShowModal();
   
   if (paramDlg.IsParamSelected())
   {
      if (oldStr != paramDlg.GetParamName())
      {
         conditionGrid->SetCellValue(row, col, paramDlg.GetParamName());
         EnableUpdate(true);
      }
   }
}


//------------------------------------------------------------------------------
// void OnCellLeftClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void ConditionPanel::OnCellLeftClick(wxGridEvent& event)
{
   int row = event.GetRow();
   int col = event.GetCol();
   
   conditionGrid->SelectBlock(row, col, row, col);
   conditionGrid->SetGridCursor(row, col);
   
   if (col == LHS_SEL_COL || col == RHS_SEL_COL)
      GetNewValue(row, col + 1);
}


//------------------------------------------------------------------------------
// void OnCellRightClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void ConditionPanel::OnCellRightClick(wxGridEvent& event)
{
   int row = event.GetRow();
   int col = event.GetCol();
      
   if ( (row == 0) && (col == COMMAND_COL) )
      return;
   
   conditionGrid->SelectBlock(row, col, row, col);
   conditionGrid->SetGridCursor(row, col);
   
   if (col == COMMAND_COL)
   {
      wxString oldStr = conditionGrid->GetCellValue(row, col);
      wxString strArray[] = {wxT("&"), wxT("|")};        
      
      wxSingleChoiceDialog dialog(this, _T("Logical Operator Selection:"),
                                        _T("LogicalOperators"), 2, strArray);
      dialog.SetSelection(0);
      
      if (dialog.ShowModal() == wxID_OK)
      {
         if (oldStr != dialog.GetStringSelection())
         {
            conditionGrid->SetCellValue(row, col, dialog.GetStringSelection());
            EnableUpdate(true);
         }
      }   
   }   
   else if (col == LHS_COL)
   {
      GetNewValue(row, col);
   }
   else if (col == COND_COL)
   {
      wxString oldStr = conditionGrid->GetCellValue(row, col);
      wxString strArray[] = {wxT("=="), wxT("~="), wxT(">"), wxT("<"), 
                             wxT(">="), wxT("<=")};        
      
      wxSingleChoiceDialog dialog(this, _T("Relation Operator Selection:"),
                                        _T("RelationalOperators"), 6, strArray);
      dialog.SetSelection(0);
      
      if (dialog.ShowModal() == wxID_OK)
      {
         if (oldStr != dialog.GetStringSelection())
         {
            conditionGrid->SetCellValue(row, col, dialog.GetStringSelection());
            EnableUpdate(true);
         }
      }
   }
   else if (col == RHS_COL)
   {
      GetNewValue(row, col);
   }
}


//------------------------------------------------------------------------------
// void OnCellValueChange(wxGridEvent& event)
//------------------------------------------------------------------------------
void ConditionPanel::OnCellValueChange(wxGridEvent& event)
{
   EnableUpdate(true);
} 
