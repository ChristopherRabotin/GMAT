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

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ForPanel, GmatPanel)
    EVT_GRID_CELL_LEFT_CLICK(ForPanel::OnCellLeftClick)
    EVT_GRID_CELL_RIGHT_CLICK(ForPanel::OnCellRightClick)   
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
      new wxGrid(this, ID_GRID, wxDefaultPosition, wxSize(503,50), wxWANTS_CHARS);
#endif
   
   conditionGrid->CreateGrid(1,4,wxGrid::wxGridSelectCells);
   conditionGrid->SetSelectionMode(wxGrid::wxGridSelectCells);
   
   conditionGrid->EnableEditing(false);
   
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
   // Set the pointer for the "Show Script" button
   mObject = theForCommand;

   if (theForCommand != NULL)
   {
      Integer paramId;
      wxString str;

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

         //loj: 11/21/06 We want to show name as is
         //Parameter *param = (Parameter*)theForCommand->GetRefObject
         //   (Gmat::PARAMETER, mStartString.c_str());
         
         //mStartString = param->GetName().c_str(); 
         conditionGrid->SetCellValue(0, START_COL, mStartString); 
      }
      else
      {    
         paramId = theForCommand->GetParameterID("StartValue");
         mStartValue = theForCommand->GetRealParameter(paramId);
         //str.Printf("%.10f", mStartValue);
         str = theGuiManager->ToWxString(mStartValue);
         conditionGrid->SetCellValue(0, START_COL, str); 
      }    
      
      paramId = theForCommand->GetParameterID("IncrIsparam");
      mIncrIsParam = theForCommand->GetBooleanParameter(paramId);
      //MessageInterface::ShowMessage("===> mIncrIsParam=%d\n", mIncrIsParam);
      
      if (mIncrIsParam)
      {
         paramId = theForCommand->GetParameterID("IncrementName");
         mIncrString = theForCommand->GetStringParameter(paramId).c_str();
         //MessageInterface::ShowMessage("===> mIncrString=%s\n", mIncrString.c_str());
         //loj: 11/21/06 We want to show name as is
         //Parameter *param = (Parameter*)theForCommand->GetRefObject
         //   (Gmat::PARAMETER, mIncrString.c_str());
         //mIncrString = param->GetName().c_str();
         conditionGrid->SetCellValue(0, INCR_COL, mIncrString); 
      }
      else
      {    
         paramId = theForCommand->GetParameterID("Step");
         mIncrValue = theForCommand->GetRealParameter(paramId);
         //str.Printf("%.10f", mIncrValue);
         str = theGuiManager->ToWxString(mIncrValue);
         conditionGrid->SetCellValue(0, INCR_COL, str); 
      }    
      
      paramId = theForCommand->GetParameterID("EndIsParam");
      mEndIsParam = theForCommand->GetBooleanParameter(paramId);
      
      if (mEndIsParam)
      {
         paramId = theForCommand->GetParameterID("EndName");
         mEndString = theForCommand->GetStringParameter(paramId).c_str();
           
         //loj: 11/21/06 We want to show name as is
         //Parameter *param = (Parameter*)theForCommand->GetRefObject
         //   (Gmat::PARAMETER, mEndString.c_str());
         //mEndString = param->GetName().c_str(); 
         conditionGrid->SetCellValue(0, END_COL, mEndString);
      }
      else
      {    
         paramId = theForCommand->GetParameterID("EndValue");
         mEndValue = theForCommand->GetRealParameter(paramId);
         //str.Printf("%.10f", mEndValue);
         str = theGuiManager->ToWxString(mEndValue);
         conditionGrid->SetCellValue(0, END_COL, str);
      }   
   }
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ForPanel::SaveData()
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
// void OnCellLeftClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void ForPanel::OnCellLeftClick(wxGridEvent& event)
{
   Integer row = event.GetRow();
   Integer col = event.GetCol();
   
   if (col == INDEX_COL)
      return;
   
   ForDialog setupDlg(this, theForCommand, col);
   setupDlg.ShowModal();
   
   if (!setupDlg.IsUpdated())
      return;

   // got new value
   
   Real realVal = setupDlg.GetRealValue();
   EnableUpdate(true);
   
   //Integer paramId;
   wxString str;
   
   if (col == START_COL)
   {
      //paramId = theForCommand->GetParameterID("StartValue");
      //mStartValue = theForCommand->GetRealParameter(paramId);
      mStartValue = realVal;
      //str.Printf("%.10f", mStartValue);
      str = theGuiManager->ToWxString(mStartValue);
      conditionGrid->SetCellValue(row, START_COL, str); 
      
      mStartIsParam = false;
      mStartString = conditionGrid->GetCellValue(row, START_COL);
   }
   else if (col == INCR_COL)
   {
      //paramId = theForCommand->GetParameterID("Step");
      //mIncrValue = theForCommand->GetRealParameter(paramId);
      mIncrValue = realVal;
      //str.Printf("%.10f", mIncrValue);
      str =  theGuiManager->ToWxString(mIncrValue);
      conditionGrid->SetCellValue(row, INCR_COL, str); 
      
      mIncrIsParam = false;
      mIncrString = conditionGrid->GetCellValue(row, INCR_COL);
   }
   else if (col == END_COL)
   {
      //paramId = theForCommand->GetParameterID("EndValue");
      //mEndValue = theForCommand->GetRealParameter(paramId);
      mEndValue = realVal;
      //str.Printf("%.10f", mEndValue);
      str =  theGuiManager->ToWxString(mEndValue);
      conditionGrid->SetCellValue(row, END_COL, str);
      
      mEndIsParam = false;
      mEndString = conditionGrid->GetCellValue(row, END_COL);
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
      
      EnableUpdate(true);  
   }  
}     
