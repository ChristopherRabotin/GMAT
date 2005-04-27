//$Header: 
//------------------------------------------------------------------------------
//                              IfPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 
/**
 * This class contains the If condition setup window.
 */
//------------------------------------------------------------------------------

#include "IfPanel.hpp"
#include "ParameterSelectDialog.hpp"
#include "gmatdefs.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(IfPanel, GmatPanel)
    EVT_GRID_CELL_RIGHT_CLICK(IfPanel::OnCellRightClick)
    EVT_GRID_CELL_CHANGE(IfPanel::OnCellValueChange)  
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// IfPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
IfPanel::IfPanel(wxWindow *parent, GmatCommand *cmd) : GmatPanel(parent)
{
   theIfCommand = (If *)cmd;
   
   mNumberOfConditions = 0;
   mNumberOfLogicalOps = 0;
   
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
// IfPanel()
//------------------------------------------------------------------------------
/**
 * A destructor.
 */
//------------------------------------------------------------------------------
IfPanel::~IfPanel()
{
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void IfPanel::Create()
{
    Setup(this);    
}

//------------------------------------------------------------------------------
// void Setup(wxWindow *parent)
//------------------------------------------------------------------------------
void IfPanel::Setup(wxWindow *parent)
{
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    conditionGrid = new wxGrid( parent, ID_GRID, wxDefaultPosition, 
                    wxSize(454,238), wxWANTS_CHARS );
    conditionGrid->CreateGrid( MAX_ROW, MAX_COL, wxGrid::wxGridSelectCells );
    conditionGrid->SetRowLabelSize(0);
    conditionGrid->SetDefaultCellAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
    
    for (Integer i = 0; i < MAX_ROW; i++)
    {
       conditionGrid->SetReadOnly(i, COMMAND_COL, true);
       conditionGrid->SetReadOnly(i, COND_COL, true);
    }    
    //conditionGrid->SetReadOnly(0,0, true);
        
    conditionGrid->SetColLabelValue(0, _T(""));
    conditionGrid->SetColSize(0, 60);
    conditionGrid->SetColLabelValue(1, _T("LHS"));
    conditionGrid->SetColSize(1, 165);
    conditionGrid->SetColLabelValue(2, _T("Condition"));
    conditionGrid->SetColSize(2, 60);
    conditionGrid->SetColLabelValue(3, _T("RHS"));
    conditionGrid->SetColSize(3, 165);
    conditionGrid->SetCellValue(0, COMMAND_COL, "If");
        
    item0->Add( conditionGrid, 0, wxALIGN_CENTER|wxALL, 5 );

    theMiddleSizer->Add(item0, 0, wxGROW, 5);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void IfPanel::LoadData()
{
    // Set the pointer for the "Show Script" button
    mObject = theIfCommand;

    if (theIfCommand != NULL)
    {
       Integer paramId;
       
       paramId = theIfCommand->GetParameterID("NumberOfConditions");
       mNumberOfConditions = theIfCommand->GetIntegerParameter(paramId);
       
       if (mNumberOfConditions > 0)
       {
          paramId = theIfCommand->GetParameterID("NumberOfLogicalOperators");
          mNumberOfLogicalOps = theIfCommand->GetIntegerParameter(paramId); 
                
          paramId = theIfCommand->GetParameterID("LeftHandStrings");
          mLhsList = theIfCommand->GetStringArrayParameter(paramId);

          paramId = theIfCommand->GetParameterID("OperatorStrings");
          mEqualityOpStrings = theIfCommand->GetStringArrayParameter(paramId);
          
          paramId = theIfCommand->GetParameterID("RightHandStrings");
          mRhsList = theIfCommand->GetStringArrayParameter(paramId);
          
          paramId = theIfCommand->GetParameterID("LogicalOperators");
          mLogicalOpStrings = theIfCommand->GetStringArrayParameter(paramId); 
          
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
                conditionGrid->SetCellValue(i, COMMAND_COL, mLogicalOpStrings[i-1].c_str());
          }    
       }   
    }         
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void IfPanel::SaveData()
{
   if ( (mLhsList.empty()) || (mRhsList.empty()) || (mEqualityOpStrings.empty()) )
   {
      MessageInterface::PopupMessage
      (Gmat::WARNING_, "Incomplete parameters for If loop condition.\n"
                       "Updates have not been saved");
      return;
   }      
   
   for (Integer i = 0; i < mNumberOfConditions; i++)
   {
      theIfCommand->SetCondition(mLhsList[i], mEqualityOpStrings[i], mRhsList[i], i);
      
      if (i != 0)
      {
         theIfCommand->SetConditionOperator(mLogicalOpStrings[i-1]);
      } 
                      
      if (mLhsIsParam[i])
      {    
         theParameter = theGuiInterpreter->GetParameter(mLhsList[i].c_str());
         if (theParameter != NULL)
         {
            theIfCommand->SetRefObject(theParameter, Gmat::PARAMETER, mLhsList[i].c_str(), i);
         }    
         else
         {
            MessageInterface::PopupMessage
            (Gmat::WARNING_, "Invalid Parameter Selection.\n"
                             "Updates have not been saved");
            return;
         }    
      }         
      if (mRhsIsParam[i])
      { 
         theParameter = theGuiInterpreter->GetParameter(mRhsList[i].c_str());
         if (theParameter != NULL)
         {
            theIfCommand->SetRefObject(theParameter, Gmat::PARAMETER, mRhsList[i].c_str(), i);
         }    
         else
         {
            MessageInterface::PopupMessage
            (Gmat::WARNING_, "Invalid Parameter Selection.\n"
                             "Updates have not been saved");
            return;
         }    
      }
   }      
}   

//------------------------------------------------------------------------------
// void OnCellRightClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void IfPanel::OnCellRightClick(wxGridEvent& event)
{
   int row = event.GetRow();
   int col = event.GetCol();
      
   if ( (row == COMMAND_COL) && (col == COMMAND_COL) )
      return;
     
   if (col == COMMAND_COL)
   {
      wxString strArray[] = {wxT("&"), wxT("|")};        
      
      wxSingleChoiceDialog dialog(this, _T("Logic Selection: \n"),
                                        _T("IfConditionDialog"), 2, strArray);
      dialog.SetSelection(0);
      
      if (dialog.ShowModal() == wxID_OK)
      {
         wxString logicalString = dialog.GetStringSelection();
         
         if (logicalString != conditionGrid->GetCellValue(row, col))
         {        
            if (mLogicalOpStrings.empty())
            {
                mLogicalOpStrings.push_back(logicalString.c_str());                
            }
            else
            {
                Integer size = mLogicalOpStrings.size();
                
                if (row < size)
                   mLogicalOpStrings[row] = logicalString.c_str();
                else
                   mLogicalOpStrings.push_back(logicalString.c_str());        
            }                         
            conditionGrid->SetCellValue(row, col, logicalString);
            mNumberOfLogicalOps++;
            theApplyButton->Enable(true);
         }
      }   
   }   
   else if (col == LHS_COL)
   {
      ParameterSelectDialog paramDlg(this);
      paramDlg.ShowModal();
      
      if (paramDlg.IsParamSelected())
      {
         wxString newParamName = paramDlg.GetParamName();
         
         if (newParamName != conditionGrid->GetCellValue(row, col))
         { 
            if (mLhsList.empty())
            {
               mLhsList.push_back(newParamName.c_str());
               mLhsIsParam.push_back(true);
            }    
            else
            {
              Integer size = mLhsList.size();
              
              if (row < size)
              {
                 mLhsList[row] = newParamName.c_str();  
                 mLhsIsParam[row] = true;
              }    
              else
              {
                 mLhsList.push_back(newParamName.c_str());   
                 mLhsIsParam.push_back(true);
              }    
            }  
            conditionGrid->SetCellValue(row, col, newParamName);
            theApplyButton->Enable(true);
         }    
      }
   }
   else if (col == COND_COL)
   {   
      wxString strArray[] = {wxT("=="), wxT("~="), wxT(">"), wxT("<"), 
                             wxT(">="), wxT("<=")};        
      
      wxSingleChoiceDialog dialog(this, _T("Equality Selection: \n"),
                                        _T("IfConditionDialog"), 6, strArray);
      dialog.SetSelection(0);
      
      if (dialog.ShowModal() == wxID_OK)
      {
         wxString equalityString = dialog.GetStringSelection();
          
         if (equalityString != conditionGrid->GetCellValue(row, col))
         {
            if (mEqualityOpStrings.empty())
            { 
               mEqualityOpStrings.push_back(equalityString.c_str());
            }    
            else
            { 
                Integer size = mEqualityOpStrings.size();
                
                if (row < size)
                   mEqualityOpStrings[row] = equalityString.c_str();
                else
                   mEqualityOpStrings.push_back(equalityString.c_str());        
            }                         
            conditionGrid->SetCellValue(row, col, equalityString);
            mNumberOfConditions++;
            theApplyButton->Enable(true); 
         }
      }  
   }
   else if (col == RHS_COL)
   {
      ParameterSelectDialog paramDlg(this);
      paramDlg.ShowModal();

      if (paramDlg.IsParamSelected())
      {
         wxString newParamName = paramDlg.GetParamName();
         
         if (newParamName != conditionGrid->GetCellValue(row, col))
         { 
            if (mRhsList.empty())
            {
               mRhsList.push_back(newParamName.c_str());
               mRhsIsParam.push_back(true);
            }    
            else
            {
               Integer size = mRhsList.size();
              
               if (row < size)
               {
                  mRhsList[row] = newParamName.c_str(); 
                  mRhsIsParam[row] = true;
               }     
               else
               {
                  mRhsList.push_back(newParamName.c_str());
                  mRhsIsParam.push_back(true);
               }    
            } 
            conditionGrid->SetCellValue(row, col, newParamName);
            theApplyButton->Enable(true);
         }    
      }
   }      
}      

//------------------------------------------------------------------------------
// void OnCellValueChange(wxGridEvent& event)
//------------------------------------------------------------------------------
void IfPanel::OnCellValueChange(wxGridEvent& event)
{
   int row = event.GetRow();
   int col = event.GetCol();  
   
   wxString temp = conditionGrid->GetCellValue(row, col);

   if (temp.IsEmpty())
      return;
      
   if (col == COMMAND_COL)
   {
       Integer size = mLogicalOpStrings.size();
       Integer index = row - 1;         
       if (index < size)
          conditionGrid->SetCellValue(row, col, mLogicalOpStrings[index].c_str());
       else
          conditionGrid->SetCellValue(row, col, "");       
   }    
   else if (col == LHS_COL)
   {   
      if (mLhsList.empty())
      {
         mLhsList.push_back(temp.c_str());
         mLhsIsParam.push_back(false);
      }    
      else
      {
         Integer size = mLhsList.size();   
         if (row < size)
         {
            mLhsList[row] = temp.c_str();
            mLhsIsParam[row] = false; 
         }    
         else
         {
            mLhsList.push_back(temp.c_str());
            mLhsIsParam.push_back(false);
         }      
      }  
   }    
   else if (col == COND_COL)
   {
       Integer size = mEqualityOpStrings.size();
                
       if (row < size)
          conditionGrid->SetCellValue(row, col, mEqualityOpStrings[row].c_str());
       else
          conditionGrid->SetCellValue(row, col, ""); 
   }    
   else if (col == RHS_COL)
   {
      if (mRhsList.empty())
      {
         mRhsList.push_back(temp.c_str());
         mRhsIsParam.push_back(false);
      }    
      else
      {
         Integer size = mRhsList.size();
         
         if (row < size)
         {  
            mRhsList[row] = temp.c_str(); 
            mRhsIsParam[row] = false; 
         }    
         else
         {  
            mRhsList.push_back(temp.c_str()); 
            mRhsIsParam.push_back(false);
         }   
      } 
   }    
   theApplyButton->Enable(true);
} 
