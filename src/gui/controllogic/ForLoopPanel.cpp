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
#include "ParameterSelectDialog.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ForLoopPanel, GmatPanel)
    EVT_GRID_CELL_RIGHT_CLICK(ForLoopPanel::OnCellRightClick)
    EVT_GRID_CELL_CHANGE(ForLoopPanel::OnCellValueChange)    
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ForLoopPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
ForLoopPanel::ForLoopPanel(wxWindow *parent, GmatCommand *cmd)
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
   
   Create();
   Show();
}

//------------------------------------------------------------------------------
// ~ForLoopPanel()
//------------------------------------------------------------------------------
/**
 * A destructor.
 */
//------------------------------------------------------------------------------
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
   // wxGrid
   conditionGrid =
      new wxGrid(this, ID_GRID, wxDefaultPosition, wxSize(503,55), wxWANTS_CHARS);
   
   conditionGrid->CreateGrid(1,4,wxGrid::wxGridSelectCells);
   conditionGrid->SetSelectionMode(wxGrid::wxGridSelectCells);
   
   //conditionGrid->EnableEditing(true);
   //conditionGrid->EnableCellEditControl(true);
   //conditionGrid->ShowCellEditControl();
   
   conditionGrid->EnableDragColSize(false);
   conditionGrid->EnableDragRowSize(false);
   conditionGrid->EnableDragGridSize(false);
   
   conditionGrid->SetColSize(0, 125);
   conditionGrid->SetColSize(1, 125);
   conditionGrid->SetColSize(2, 125);
   conditionGrid->SetColSize(3, 125);
   
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
void ForLoopPanel::LoadData()
{
    // Set the pointer for the "Show Script" button
    mObject = theForCommand;

    if (theForCommand != NULL)
    {
       Integer paramId;
       wxString s1, s2, s3;

       paramId = theForCommand->GetParameterID("IndexName");
       mIndexString = theForCommand->GetStringParameter(paramId).c_str();

       if (mIndexString.IsEmpty())
       {
          mIndexString = "Default";
          mIndexIsSet = false;
       } 
       else
       {
          mIndexIsSet = true;  
          
          paramId = theForCommand->GetParameterID("IndexIsParam");
          mIndexIsParam = theForCommand->GetBooleanParameter(paramId);
          
          if (mIndexIsParam)
          {
             Parameter *param = (Parameter*)theForCommand->GetRefObject
                             (Gmat::PARAMETER, mIndexString.c_str());
                
             mIndexString = param->GetName().c_str();
          }  
       }    
       conditionGrid->SetCellValue(0, INDEX_COL, mIndexString); 
    
       paramId = theForCommand->GetParameterID("StartIsParam");
       mStartIsParam = theForCommand->GetBooleanParameter(paramId);
          
       if (mStartIsParam)
       {
           paramId = theForCommand->GetParameterID("StartName");
           mStartString = theForCommand->GetStringParameter(paramId).c_str();
 
           Parameter *param = (Parameter*)theForCommand->GetRefObject
                             (Gmat::PARAMETER, mStartString.c_str());
           
           mStartString = param->GetName().c_str(); 
           conditionGrid->SetCellValue(0, START_COL, mStartString); 
       }
       else
       {    
          paramId = theForCommand->GetParameterID("StartValue");
          mStartValue = theForCommand->GetRealParameter(paramId);
          s1.Printf("%.10f", mStartValue);
          conditionGrid->SetCellValue(0, START_COL, s1); 
       }    
       
       paramId = theForCommand->GetParameterID("IncrIsparam");
       mIncrIsParam = theForCommand->GetBooleanParameter(paramId);

       if (mIncrIsParam)
       {
           paramId = theForCommand->GetParameterID("IncrementName");
           mIncrString = theForCommand->GetStringParameter(paramId).c_str();
           
           Parameter *param = (Parameter*)theForCommand->GetRefObject
                             (Gmat::PARAMETER, mIncrString.c_str());
           mIncrString = param->GetName().c_str();
           conditionGrid->SetCellValue(0, INCR_COL, mIncrString); 
       }
       else
       {    
          paramId = theForCommand->GetParameterID("Step");
          mIncrValue = theForCommand->GetRealParameter(paramId);
          s2.Printf("%.10f", mIncrValue);
          conditionGrid->SetCellValue(0, INCR_COL, s2); 
       }    
       
       paramId = theForCommand->GetParameterID("EndIsParam");
       mEndIsParam = theForCommand->GetBooleanParameter(paramId);
       
       if (mEndIsParam)
       {
           paramId = theForCommand->GetParameterID("EndName");
           mEndString = theForCommand->GetStringParameter(paramId).c_str();
           
           Parameter *param = (Parameter*)theForCommand->GetRefObject
                             (Gmat::PARAMETER, mEndString.c_str());
           mEndString = param->GetName().c_str(); 
           conditionGrid->SetCellValue(0, END_COL, mEndString);
       }
       else
       {    
          paramId = theForCommand->GetParameterID("EndValue");
          mEndValue = theForCommand->GetRealParameter(paramId);
          s3.Printf("%.10f", mEndValue);
          conditionGrid->SetCellValue(0, END_COL, s3);
       }   
    }
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ForLoopPanel::SaveData()
{
    Integer paramId;
    
    if (mIndexIsSet)
    {
       if (mIndexIsParam)
       {    
          paramId = theForCommand->GetParameterID("IndexName");
          theForCommand->SetStringParameter(paramId, mIndexString.c_str());
          Parameter* theParam = theGuiInterpreter->GetParameter(mIndexString.c_str());
          theForCommand->SetRefObject(theParam, Gmat::PARAMETER, mIndexString.c_str());
       }    
    }    
    
    if (mStartIsParam)
    {
       paramId = theForCommand->GetParameterID("StartName");
       theForCommand->SetStringParameter(paramId, mStartString.c_str());
       Parameter* theParam = theGuiInterpreter->GetParameter(mStartString.c_str());
       theForCommand->SetRefObject(theParam, Gmat::PARAMETER, mStartString.c_str());
    }
    else
    {    
       paramId = theForCommand->GetParameterID("StartValue");
       theForCommand->SetRealParameter(paramId, mStartValue);
    }   
    
    if (mIncrIsParam)
    {
       paramId = theForCommand->GetParameterID("IncrementName");
       theForCommand->SetStringParameter(paramId, mIncrString.c_str());
       Parameter* theParam = theGuiInterpreter->GetParameter(mIncrString.c_str());
       theForCommand->SetRefObject(theParam, Gmat::PARAMETER, mIncrString.c_str());
    }
    else
    {        
       paramId = theForCommand->GetParameterID("Step");
       theForCommand->SetRealParameter(paramId, mIncrValue);
    }    
    
    if (mEndIsParam)
    {
       paramId = theForCommand->GetParameterID("EndName");
       theForCommand->SetStringParameter(paramId, mEndString.c_str());
       Parameter* theParam = theGuiInterpreter->GetParameter(mEndString.c_str());
       theForCommand->SetRefObject(theParam, Gmat::PARAMETER, mEndString.c_str());
    }
    else
    {    
       paramId = theForCommand->GetParameterID("EndValue");
       theForCommand->SetRealParameter(paramId, mEndValue);
    }    
}

//------------------------------------------------------------------------------
// void OnCellRightClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void ForLoopPanel::OnCellRightClick(wxGridEvent& event)
{
   int row = event.GetRow();
   int col = event.GetCol();
    
   ParameterSelectDialog paramDlg(this);
   paramDlg.ShowModal();

   if (paramDlg.IsParamSelected())
   {
      wxString newParamName = paramDlg.GetParamName();
      
      if (newParamName == conditionGrid->GetCellValue(row, col))
         return;
               
      conditionGrid->SetCellValue(row, col, newParamName);
      
      if (col == INDEX_COL)
      {
         mIndexString = conditionGrid->GetCellValue(row, INDEX_COL);
         mIndexIsParam = true; 
         mIndexIsSet = true;   
      }    
      else if (col == START_COL)
      {
         mStartString = conditionGrid->GetCellValue(row, START_COL);
         mStartIsParam = true;
      }    
      else if (col == INCR_COL)
      {
         mIncrString = conditionGrid->GetCellValue(row, INCR_COL);
         mIncrIsParam = true;
      }    
      else if (col == END_COL)
      {
         mEndString = conditionGrid->GetCellValue(row, END_COL);
         mEndIsParam = true;
      }  
      theApplyButton->Enable(true);  
   }  
} 
    
//------------------------------------------------------------------------------
// void OnCellValueChange(wxGridEvent& event)
//------------------------------------------------------------------------------
void ForLoopPanel::OnCellValueChange(wxGridEvent& event)
{
   int row = event.GetRow();
   int col = event.GetCol();
  
//   wxString temp = conditionGrid->GetCellValue(row, col);
//   double *value = NULL;
//   
// @todo add check pointer for invalid entries,
// check why ToDouble() returns false in every case.  
//   if ( !temp.ToDouble(value) )
//   {
//      conditionGrid->SetCellValue(row, col, "0.0");
//      return;
//   }    

   if (col == INDEX_COL)
   {
      conditionGrid->SetCellValue(row, INDEX_COL, mIndexString);
   }   
   else if (col == START_COL)
   {
      mStartValue = atof(conditionGrid->GetCellValue(row, col).c_str());
      mStartIsParam = false;
   }    
   else if (col == INCR_COL)
   {
      mIncrValue = atof(conditionGrid->GetCellValue(row, INCR_COL));
      mIncrIsParam = false;
   }    
   else if (col == END_COL)
   {
      mEndValue = atof(conditionGrid->GetCellValue(row, END_COL));
      mEndIsParam = false;
   }    
   theApplyButton->Enable(true);
}    
