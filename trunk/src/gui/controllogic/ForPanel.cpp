//$Header$
//------------------------------------------------------------------------------
//                              ForPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2004/05/17
/**
 * This class contains the For Conditional Statement Setup window.
 */
//------------------------------------------------------------------------------
#include "ForPanel.hpp"
#include "ForDialog.hpp"
#include "ParameterSelectDialog.hpp"

//#define DEBUG_FOR_PANEL_LOAD 1
//#define DEBUG_FOR_PANEL_SAVE 1
//#define DEBUG_FOR_PANEL_CELL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ForPanel, GmatPanel)
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
    Setup(this);    
}

//------------------------------------------------------------------------------
// void Setup( wxWindow *parent)
//------------------------------------------------------------------------------
void ForPanel::Setup( wxWindow *parent)
{
   // wxGrid
#if __WXMAC__
   conditionGrid =
      new wxGrid(this, ID_GRID, wxDefaultPosition, wxSize(630,65), wxWANTS_CHARS);
#else
   conditionGrid =
      //loj: new wxGrid(this, ID_GRID, wxDefaultPosition, wxSize(503,50), wxWANTS_CHARS);
      new wxGrid(this, ID_GRID, wxDefaultPosition, wxSize(518,65), wxWANTS_CHARS);
#endif
   
   conditionGrid->CreateGrid(1, 4, wxGrid::wxGridSelectCells);
   conditionGrid->SetSelectionMode(wxGrid::wxGridSelectCells);
   
   //loj: 03/21/07 conditionGrid->EnableEditing(false);
   
   //conditionGrid->EnableEditing(true);
   //conditionGrid->EnableCellEditControl(true);
   //conditionGrid->ShowCellEditControl();
   
   conditionGrid->EnableDragColSize(false);
   conditionGrid->EnableDragRowSize(false);
   conditionGrid->EnableDragGridSize(false);
 
 #if __WXMAC__  
   conditionGrid->SetColSize(0, 155);
   conditionGrid->SetColSize(1, 155);
   conditionGrid->SetColSize(2, 155);
   conditionGrid->SetColSize(3, 155);
#else
   conditionGrid->SetColSize(0, 125);
   conditionGrid->SetColSize(1, 125);
   conditionGrid->SetColSize(2, 125);
   conditionGrid->SetColSize(3, 125);
#endif

   conditionGrid->SetColLabelValue(0, _T("Index"));
   conditionGrid->SetColLabelValue(1, _T("Start"));
   conditionGrid->SetColLabelValue(2, _T("Increment"));
   conditionGrid->SetColLabelValue(3, _T("End"));
   
   conditionGrid->SetRowLabelSize(0);
   
   conditionGrid->SetScrollbars(0, 0, 0, 0, 0, 0, FALSE);   
   
   Integer bsize = 5;
   
   // wx*Sizers
   wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
   item0->Add( conditionGrid, 0, wxALIGN_CENTER|wxALL, bsize);

   theMiddleSizer->Add(item0, 0, wxGROW, bsize);
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
// void OnCellRightClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void ForPanel::OnCellRightClick(wxGridEvent& event)
{
   Integer row = event.GetRow();
   Integer col = event.GetCol();
   
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
