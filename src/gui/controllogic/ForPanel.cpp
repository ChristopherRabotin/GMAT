//$Id$
//------------------------------------------------------------------------------
//                              ForPanel
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
// Author: Allison Greene
// Created: 2004/05/17
//
/**
 * Implements the For loop setup window.
 */
//------------------------------------------------------------------------------
#include "ForPanel.hpp"
#include "ParameterSelectDialog.hpp"

//#define DEBUG_FOR_PANEL_LOAD 1
//#define DEBUG_FOR_PANEL_SAVE 1
//#define DEBUG_FOR_PANEL_CELL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ForPanel, GmatPanel)
   EVT_GRID_CELL_LEFT_CLICK(ForPanel::OnCellLeftClick)   
   EVT_GRID_CELL_RIGHT_CLICK(ForPanel::OnCellRightClick)   
   EVT_GRID_CELL_CHANGE(ForPanel::OnCellValueChange)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ForPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
ForPanel::ForPanel(wxWindow *parent, GmatCommand *cmd)
    : GmatPanel(parent)
{
   theForCommand = (For *)cmd;
   
   mStartValue = 0;
   mIncrValue = 0;
   mEndValue = 0;   
   
   mIndexIsParam = false;
   mStartIsParam = false;
   mEndIsParam = false;
   mIncrIsParam = false;
   
   mObjectTypeList.Add("Spacecraft");
   
   Create();
   Show();
}

//------------------------------------------------------------------------------
// ~ForPanel()
//------------------------------------------------------------------------------
/**
 * A destructor.
 */
//------------------------------------------------------------------------------
ForPanel::~ForPanel()
{
   mObjectTypeList.Clear();
}

//-------------------------------
// private methods
//-------------------------------
void ForPanel::Create()
{
   // wxGrid
#if __WXMAC__
   conditionGrid =
      new wxGrid(this, ID_GRID, wxDefaultPosition, wxSize(700,65), wxWANTS_CHARS);
#else
   conditionGrid =
      new wxGrid(this, ID_GRID, wxDefaultPosition, wxSize(600,65), wxWANTS_CHARS);
#endif
   
   conditionGrid->CreateGrid(1, MAX_COL, wxGrid::wxGridSelectCells);
   conditionGrid->SetSelectionMode(wxGrid::wxGridSelectCells);
   conditionGrid->SetDefaultCellAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
 
#if __WXMAC__  
   conditionGrid->SetColSize(INDEX_SEL_COL, 25);
   conditionGrid->SetColSize(INDEX_COL, 155);
   conditionGrid->SetColSize(START_SEL_COL, 25);
   conditionGrid->SetColSize(START_COL, 155);
   conditionGrid->SetColSize(INCR_SEL_COL, 25);
   conditionGrid->SetColSize(INCR_COL, 155);
   conditionGrid->SetColSize(END_SEL_COL, 25);
   conditionGrid->SetColSize(END_COL, 155);
#else
   conditionGrid->SetColSize(INDEX_SEL_COL, 25);
   conditionGrid->SetColSize(INDEX_COL, 125);
   conditionGrid->SetColSize(START_SEL_COL, 25);
   conditionGrid->SetColSize(START_COL, 125);
   conditionGrid->SetColSize(INCR_SEL_COL, 25);
   conditionGrid->SetColSize(INCR_COL, 125);
   conditionGrid->SetColSize(END_SEL_COL, 25);
   conditionGrid->SetColSize(END_COL, 125);
#endif

   conditionGrid->SetColLabelValue(INDEX_SEL_COL, _T(""));
   conditionGrid->SetColLabelValue(INDEX_COL, _T("Index"));
   conditionGrid->SetColLabelValue(START_SEL_COL, _T(""));
   conditionGrid->SetColLabelValue(START_COL, _T("Start"));
   conditionGrid->SetColLabelValue(INCR_SEL_COL, _T(""));
   conditionGrid->SetColLabelValue(INCR_COL, _T("Increment"));
   conditionGrid->SetColLabelValue(END_SEL_COL, _T(""));
   conditionGrid->SetColLabelValue(END_COL, _T("End"));
   
   conditionGrid->SetRowLabelSize(0);
   
   conditionGrid->SetScrollbars(0, 0, 0, 0, 0, 0, FALSE);   
   
   // Set ... before each column for selection
   conditionGrid->SetCellBackgroundColour(0, INDEX_SEL_COL, *wxLIGHT_GREY);
   conditionGrid->SetCellBackgroundColour(0, START_SEL_COL, *wxLIGHT_GREY);
   conditionGrid->SetCellBackgroundColour(0, INCR_SEL_COL, *wxLIGHT_GREY);
   conditionGrid->SetCellBackgroundColour(0, END_SEL_COL, *wxLIGHT_GREY);
   conditionGrid->SetCellValue(0, INDEX_SEL_COL, _T("  ... "));
   conditionGrid->SetCellValue(0, START_SEL_COL, _T("  ... "));
   conditionGrid->SetCellValue(0, INCR_SEL_COL, _T("  ... "));
   conditionGrid->SetCellValue(0, END_SEL_COL, _T("  ... "));
   
   // wx*Sizers
   wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
   item0->Add( conditionGrid, 0, wxALIGN_CENTER|wxALL, 0);
   
   theMiddleSizer->Add(item0, 0, wxGROW, 0);
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ForPanel::LoadData()
{
   #if DEBUG_FOR_PANEL_LOAD
   MessageInterface::ShowMessage("ForPanel::LoadData() entered\n");
   #endif
   
   // Set the pointer for the "Show Script" button
   mObject = theForCommand;
   
   if (theForCommand == NULL)
      return;
      
   try
   {
      Integer paramId;
      wxString str;

      paramId = theForCommand->GetParameterID("IndexName");
      mIndexString = theForCommand->GetStringParameter(paramId).c_str();

      paramId = theForCommand->GetParameterID("StartName");
      mStartString = theForCommand->GetStringParameter(paramId).c_str();

      paramId = theForCommand->GetParameterID("EndName");
      mEndString = theForCommand->GetStringParameter(paramId).c_str();

      paramId = theForCommand->GetParameterID("IncrementName");
      mIncrString = theForCommand->GetStringParameter(paramId).c_str();
      
      conditionGrid->SetCellValue(0, INDEX_COL, mIndexString);
      conditionGrid->SetCellValue(0, START_COL, mStartString); 
      conditionGrid->SetCellValue(0, END_COL, mEndString);
      conditionGrid->SetCellValue(0, INCR_COL, mIncrString); 
      
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ForPanel::SaveData()
{
   #if DEBUG_FOR_PANEL_SAVE
   MessageInterface::ShowMessage("ForPanel::SaveData() entered\n");
   #endif
   
   canClose = true;
   
   //-----------------------------------------------------------------
   // check input values: Number, Variable, Array element, Parameter
   //-----------------------------------------------------------------
   CheckVariable(mIndexString.c_str(), Gmat::SPACECRAFT, "Index",
                 "Variable", false);
   CheckVariable(mStartString.c_str(), Gmat::SPACECRAFT, "Start",
                 "Real Number, Variable, Array element, plottable Parameter", true);
   CheckVariable(mIncrString.c_str(), Gmat::SPACECRAFT, "Increment",
                 "Real Number, Variable, Array element, plottable Parameter", true);
   CheckVariable(mEndString.c_str(), Gmat::SPACECRAFT, "End",
                 "Real Number, Variable, Array element, plottable Parameter", true);
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      Integer paramId;
      paramId = theForCommand->GetParameterID("IndexName");
      theForCommand->SetStringParameter(paramId, mIndexString.c_str());
      
      paramId = theForCommand->GetParameterID("StartName");
      theForCommand->SetStringParameter(paramId, mStartString.c_str());
      
      paramId = theForCommand->GetParameterID("EndName");
      theForCommand->SetStringParameter(paramId, mEndString.c_str());
      
      paramId = theForCommand->GetParameterID("IncrementName");
      theForCommand->SetStringParameter(paramId, mIncrString.c_str());
      
      if (!theGuiInterpreter->ValidateCommand(theForCommand))
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
void ForPanel::GetNewValue(Integer row, Integer col)
{
   ParameterSelectDialog paramDlg(this, mObjectTypeList);
   paramDlg.ShowModal();
   
   if (paramDlg.IsParamSelected())
   {
      wxString newParamName = paramDlg.GetParamName();
      
      if (newParamName == conditionGrid->GetCellValue(row, col))
         return;
      
      conditionGrid->SetCellValue(row, col, newParamName);
      
      if (col == INDEX_COL)
         mIndexString = newParamName;
      else if (col == START_COL)
         mStartString = newParamName;
      else if (col == INCR_COL)
         mIncrString = newParamName;
      else if (col == END_COL)
         mEndString = newParamName;
      
      EnableUpdate(true);
   }  
}


//------------------------------------------------------------------------------
// void OnCellLeftClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void ForPanel::OnCellLeftClick(wxGridEvent& event)
{
   Integer row = event.GetRow();
   Integer col = event.GetCol();
   
   conditionGrid->SelectBlock(row, col, row, col);
   conditionGrid->SetGridCursor(row, col);
   
   if (col == INDEX_SEL_COL || col == START_SEL_COL || col == INCR_SEL_COL ||
       col == END_SEL_COL)
      GetNewValue(row, col + 1);
}


//------------------------------------------------------------------------------
// void OnCellRightClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void ForPanel::OnCellRightClick(wxGridEvent& event)
{
   Integer row = event.GetRow();
   Integer col = event.GetCol();

   GetNewValue(row, col);

   
//    ParameterSelectDialog paramDlg(this, mObjectTypeList);
//    paramDlg.ShowModal();
   
//    if (paramDlg.IsParamSelected())
//    {
//       wxString newParamName = paramDlg.GetParamName();
      
//       if (newParamName == conditionGrid->GetCellValue(row, col))
//          return;
      
//       conditionGrid->SetCellValue(row, col, newParamName);
      
//       if (col == INDEX_COL)
//          mIndexString = newParamName;
//       else if (col == START_COL)
//          mStartString = newParamName;
//       else if (col == INCR_COL)
//          mIncrString = newParamName;
//       else if (col == END_COL)
//          mEndString = newParamName;
      
//       EnableUpdate(true);
//    }  
}     


//------------------------------------------------------------------------------
// void OnCellValueChange(wxGridEvent& event)
//------------------------------------------------------------------------------
void ForPanel::OnCellValueChange(wxGridEvent& event)
{
   Integer row = event.GetRow();
   Integer col = event.GetCol();
   wxString valStr = conditionGrid->GetCellValue(row, col);
   
   #if DEBUG_FOR_PANEL_CELL
   MessageInterface::ShowMessage
      ("ForPanel::OnCellValueChange() row=%d, col=%d, value=%s\n", row, col,
       valStr.c_str());
   #endif
   
   if (col == INDEX_COL)
      mIndexString = valStr;
   else if (col == START_COL)
      mStartString = valStr;
   else if (col == INCR_COL)
      mIncrString = valStr;
   else if (col == END_COL)
      mEndString = valStr;
   
   EnableUpdate(true);  
}
