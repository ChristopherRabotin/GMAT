//$Header$
//------------------------------------------------------------------------------
//                              PropagatePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/08/29
// Modified:
//    2004/05/06 Allison Greene: inherit from GmatPanel
//    2004/10/20 Linda Jun: rename from PropgateCommandPanel
/**
 * This class contains the Propagate Command Setup window.
 */
//------------------------------------------------------------------------------

#include <wx/variant.h>
#include "PropagatePanel.hpp"
#include "ParameterSelectDialog.hpp"
#include "SpaceObjectSelectDialog.hpp"
#include "PropagatorSelectDialog.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PROPAGATE_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(PropagatePanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
   
   EVT_BUTTON(ID_BUTTON_CANCEL, PropagatePanel::OnCancel)
   EVT_GRID_CELL_LEFT_DCLICK(PropagatePanel::OnCellLeftDoubleClick)
   EVT_GRID_CELL_RIGHT_CLICK(PropagatePanel::OnCellRightClick)
   EVT_GRID_CELL_CHANGE(PropagatePanel::OnCellValueChange)
   EVT_CHECKBOX(ID_CHECKBOX, PropagatePanel::OnCheckBoxChange)
   EVT_COMBOBOX(ID_COMBOBOX, PropagatePanel::OnComboBoxChange)
   EVT_KEY_DOWN(PropagatePanel::OnKeyDown)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// PropagatePanel(wxWindow *parent, GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
PropagatePanel::PropagatePanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::PropagatePanel() entered\n");
   #endif
   
   thePropCmd = (Propagate *)cmd;

   // Default values
   mPropModeChanged = false;
   mPropChanged     = false;
   mStopCondChanged = false;
   
   mPropGridSelected     = false;
   mStopCondGridSelected = false;

   canClose = true;

   mPropModeCount = 1;
   mPropCount     = 0;
   mTempPropCount = 0;
   mStopCondCount = 0;
   mCurrPropRow   = 0;
   mCurrStopRow   = 0;
//   numOfEqualities = 6;
   
   for (Integer i=0; i<MAX_PROP_ROW; i++)
   {
      mTempProp[i].isChanged = false;
      mTempProp[i].propName = "";
      mTempProp[i].soNames = "";
      mTempProp[i].soNameList.Clear();
   }

   for (Integer i=0; i<MAX_STOPCOND_ROW; i++)
   {
      mTempStopCond[i].isChanged = false;
      mTempStopCond[i].name = "";
      mTempStopCond[i].desc = "";
      mTempStopCond[i].varName = "";
      mTempStopCond[i].typeName = "";
      mTempStopCond[i].relOpStr = "";
      mTempStopCond[i].goalStr = "";
      mTempStopCond[i].tol = BaseStopCondition::STOP_COND_TOL;
      mTempStopCond[i].repeat = 1;
      mTempStopCond[i].stopCondPtr = NULL;
   }
   
   mObjectTypeList.Add("Spacecraft");
   
   Create();
   Show();
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// void OnCancel()
//------------------------------------------------------------------------------
/**
 * Close page.
 */
//------------------------------------------------------------------------------
void PropagatePanel::OnCancel(wxCommandEvent &event)
{
   mRemovedStopCondList.clear();
   GmatAppData::GetMainFrame()->CloseActiveChild();
}


//------------------------------------------------------------------------------
// ~PropagatePanel()
//------------------------------------------------------------------------------
PropagatePanel::~PropagatePanel()
{
   mObjectTypeList.Clear();
}

//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void PropagatePanel::Create()
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage("PropagatePanel::Create() entered\n");
   #endif
   
   Integer bsize = 2; // bordersize
   
   // Propagate Mode
   wxStaticText *synchStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Propagate Mode:  "), 
         wxDefaultPosition, wxDefaultSize, 0);

   StringArray propModes = thePropCmd->GetStringArrayParameter
      (thePropCmd->GetParameterID("AvailablePropModes"));
   
   mPropModeCount = propModes.size();

   wxString *propModeList = new wxString[mPropModeCount];
   for (Integer i=0; i<mPropModeCount; i++)
   {
      propModeList[i] = propModes[i].c_str();
   }
   
   if (strcmp(propModeList[0].c_str(), "") == 0)
      propModeList[0] = "None";

   mPropModeComboBox =
      new wxComboBox(this, ID_COMBOBOX, wxT(propModeList[0].c_str()), 
         wxDefaultPosition, wxSize(150,-1), mPropModeCount, propModeList,
         wxCB_DROPDOWN|wxCB_READONLY);
   
   mPropModeComboBox->Enable(true);

   // Backwards propagate
   backPropCheckBox =
      new wxCheckBox( this, ID_CHECKBOX, wxT("Backwards Propagation"),
                      wxDefaultPosition, wxDefaultSize, 0 );

   // Propagator Grid
   propGrid =
      new wxGrid(this, ID_GRID, wxDefaultPosition, wxSize(700,100), 
         wxWANTS_CHARS);
   
   propGrid->CreateGrid(MAX_PROP_ROW, 2, wxGrid::wxGridSelectCells);

   propGrid->SetColLabelValue(PROP_NAME_COL, _T("Propagator"));
   propGrid->SetColSize(PROP_NAME_COL, 340);
   propGrid->SetColLabelValue(PROP_SOS_COL, _T("Spacecraft List"));
   propGrid->SetColSize(PROP_SOS_COL, 340);

   propGrid->EnableEditing(false);
   propGrid->SetMargins(0, 0);
   propGrid->SetRowLabelSize(0);
   propGrid->SetScrollbars(5, 8, 15, 15);

   // Stopping Condition Grid
   stopCondGrid =
      new wxGrid(this, ID_GRID, wxDefaultPosition, wxSize(700,100), 
         wxWANTS_CHARS);

   stopCondGrid->CreateGrid(MAX_STOPCOND_ROW, 3, wxGrid::wxGridSelectCells);

   stopCondGrid->SetColLabelValue(STOPCOND_PARAM_COL, _T("Parameter"));
   stopCondGrid->SetColSize(STOPCOND_PARAM_COL, 325);
   stopCondGrid->SetColLabelValue(STOPCOND_RELOPER_COL, _T(""));
   stopCondGrid->SetColSize(STOPCOND_RELOPER_COL, 30);
   stopCondGrid->SetColLabelValue(STOPCOND_COND_COL, _T("Condition"));
   stopCondGrid->SetColSize(STOPCOND_COND_COL, 325);
   
   for (Integer i = 0; i < MAX_STOPCOND_ROW; i++)
   {
      stopCondGrid->SetReadOnly(i, STOPCOND_PARAM_COL, true);
      stopCondGrid->SetReadOnly(i, STOPCOND_RELOPER_COL, true);
   }    
   
   stopCondGrid->SetMargins(0, 0);
   stopCondGrid->SetRowLabelSize(0);
   stopCondGrid->SetScrollbars(5, 8, 15, 15);


   //wxSizers
   propModeSizer = new wxFlexGridSizer( 4, 0, 0 );
   mMiddleBoxSizer = new wxBoxSizer(wxVERTICAL);
   mpropSizer = 
      new GmatStaticBoxSizer(wxVERTICAL, this, "Propagators and Spacecraft");
   mStopSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Stopping Conditions");
   
   //Adding objects to sizers
   propModeSizer->Add( synchStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   propModeSizer->Add( mPropModeComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
   propModeSizer->Add( 200, 20, wxALIGN_CENTRE|wxALL, bsize );
   propModeSizer->Add( backPropCheckBox, 0, wxALIGN_LEFT|wxALL, bsize );
   
   mpropSizer->Add(propModeSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   mpropSizer->Add(propGrid, 0, wxALIGN_CENTER|wxALL, bsize);
     
   mStopSizer->Add(stopCondGrid, 0, wxALIGN_CENTER|wxALL, bsize);
   
   mMiddleBoxSizer->Add(mpropSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   mMiddleBoxSizer->Add(mStopSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   theMiddleSizer->Add(mMiddleBoxSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
}


//------------------------------------------------------------------------------
// void MoveUpPropData()
//------------------------------------------------------------------------------
void PropagatePanel::MoveUpPropData()
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::MoveUpPropData() entered\n");
   #endif

   PropType tempProp[MAX_PROP_ROW];
   
   for (Integer i=0; i<MAX_PROP_ROW; i++)
   {
      tempProp[i].isChanged  = mTempProp[i].isChanged;
      tempProp[i].propName   = mTempProp[i].propName;
      tempProp[i].soNames    = mTempProp[i].soNames;
      tempProp[i].soNameList = mTempProp[i].soNameList;
   }
   
   for (Integer i=0; i<MAX_PROP_ROW; i++)
   {
      mTempProp[i].isChanged = false;
      mTempProp[i].propName  = "";
      mTempProp[i].soNames   = "";
      mTempProp[i].soNameList.Clear();
   }
   
   Integer propCount = 0;
   for (Integer i=0; i<MAX_PROP_ROW; i++)
   {
      if (tempProp[i].propName != "")
      {
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::MoveUpPropData() propName = %s\n",
       tempProp[i].propName.c_str());
   #endif
         mTempProp[propCount].isChanged  = tempProp[i].isChanged;
         mTempProp[propCount].propName   = tempProp[i].propName;
         mTempProp[propCount].soNames    = tempProp[i].soNames;
         mTempProp[propCount].soNameList = tempProp[i].soNameList;
         propCount++;
      }
   }

   mTempPropCount = propCount;
   
   for (Integer i=0; i<MAX_PROP_ROW; i++)
   {
      propGrid->SetCellValue(i, PROP_NAME_COL, mTempProp[i].propName);
      propGrid->SetCellValue(i, PROP_SOS_COL, mTempProp[i].soNames);
   }
   
   DisplayPropagator();
}


//------------------------------------------------------------------------------
// void DisplayPropagator()
//------------------------------------------------------------------------------
void PropagatePanel::DisplayPropagator()
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::DisplayPropagator() entered\n");
   #endif

   wxString name;
   for (Integer i=0; i<mTempPropCount; i++)
   {
      propGrid->SetCellValue(i, PROP_NAME_COL, mTempProp[i].propName);
      propGrid->SetCellValue(i, PROP_SOS_COL, mTempProp[i].soNames);
   }
   
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void DisplayStopCondition()
//------------------------------------------------------------------------------
void PropagatePanel::DisplayStopCondition()
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::DisplayStopCondition() \n");
   #endif
   
   //----- for stopCondGrid - show all
   for (Integer i=0; i<MAX_STOPCOND_ROW; i++)
   {
      stopCondGrid->
         SetCellValue( i, STOPCOND_PARAM_COL, mTempStopCond[i].varName );

      if ( mTempStopCond[i].varName.Contains(".Periapsis") ||
           mTempStopCond[i].varName.Contains(".Apoapsis")  ||
           strcmp(mTempStopCond[i].varName, "") == 0 )
      {
	      stopCondGrid->SetCellValue( i, STOPCOND_RELOPER_COL, "" );
	      stopCondGrid->SetCellValue( i, STOPCOND_COND_COL, "" );
		   stopCondGrid->SetReadOnly( i, STOPCOND_COND_COL, true );
      }
      else
      {
	      stopCondGrid->SetCellValue( i, STOPCOND_RELOPER_COL, "=" );
//         stopCondGrid->
//            SetCellValue( i, STOPCOND_RELOPER_COL, mTempStopCond[i].relOpStr );        
         stopCondGrid->
            SetCellValue( i, STOPCOND_COND_COL, mTempStopCond[i].goalStr );
		   stopCondGrid->SetReadOnly( i, STOPCOND_COND_COL, false );
      }   
   }
   
   EnableUpdate( true );
}


//------------------------------------------------------------------------------
// void UpdateStopCondition()
//------------------------------------------------------------------------------
void PropagatePanel::UpdateStopCondition()
{
   Integer stopRow = mCurrStopRow;
   
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::UpdateStopCondition() stopRow = %d\n", stopRow);
   #endif
   
   wxString oldStopName = mTempStopCond[stopRow].name;
   mTempStopCond[stopRow].name = "StopOn" + 
      stopCondGrid->GetCellValue(stopRow, STOPCOND_PARAM_COL);
   mTempStopCond[stopRow].varName = 
      stopCondGrid->GetCellValue(stopRow, STOPCOND_PARAM_COL);
   mTempStopCond[stopRow].relOpStr = 
      stopCondGrid->GetCellValue(stopRow, STOPCOND_RELOPER_COL);

   // if Apoapsis or Periapsis, disable goal
   if ( mTempStopCond[stopRow].varName.Contains(".Periapsis") ||
        mTempStopCond[stopRow].varName.Contains(".Apoapsis") )
   {
      mTempStopCond[stopRow].goalStr = "0.0";
   }
   else
   {
	   mTempStopCond[stopRow].goalStr = 
	      stopCondGrid->GetCellValue(stopRow, STOPCOND_COND_COL);
	}

   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::UpdateStopCondition() "
       "StopCondition: old name = %s\n", oldStopName.c_str());
   MessageInterface::ShowMessage
      ("PropagatePanel::UpdateStopCondition() "
       "StopCondition: name = %s\n", mTempStopCond[stopRow].name.c_str());
   MessageInterface::ShowMessage
      ("PropagatePanel::UpdateStopCondition() "
       "StopCondition: condition = %s\n", mTempStopCond[stopRow].goalStr.c_str());
   #endif
             
   wxString str = FormatStopCondDesc(mTempStopCond[stopRow].varName,
                                     mTempStopCond[stopRow].relOpStr,
                                     mTempStopCond[stopRow].goalStr);
   mTempStopCond[stopRow].desc = str;
   
   //-----------------------------------------------------------------
   // create StopCondition if new StopCondition
   //-----------------------------------------------------------------
   if (oldStopName.IsSameAs(""))
   {
      StopCondition *stopCond = (StopCondition*)theGuiInterpreter->
         CreateStopCondition("StopCondition", mTempStopCond[stopRow].name.c_str());
      mTempStopCond[stopRow].stopCondPtr = stopCond;

      if (stopCond == NULL)
      {
         MessageInterface::ShowMessage
            ("PropagatePanel::UpdateStopCondition() Unable to create "
             "StopCondition: name=%s\n", mTempStopCond[stopRow].name.c_str());
      }
   }
   else
   {
      mTempStopCond[stopRow].stopCondPtr->
         SetName(std::string(mTempStopCond[stopRow].name.c_str()));
   }
   
   mTempStopCond[stopRow].isChanged = true;
   
   mStopCondChanged = true;
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void RemoveStopCondition()
//------------------------------------------------------------------------------
void PropagatePanel::RemoveStopCondition()
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::RemoveStopCondition() mCurrStopRow=%d\n",
       mCurrStopRow);
   #endif
  
   // Add deleted StopCondition pointer to list so it can be removed
   // later in SaveData()
   if (mTempStopCond[mCurrStopRow].stopCondPtr != NULL)
   {    
      mRemovedStopCondList.push_back(mTempStopCond[mCurrStopRow].stopCondPtr);
      
      #if DEBUG_PROPAGATE_PANEL
      MessageInterface::ShowMessage
         ("PropagatePanel::RemoveStopCondition() remove StopCondition addr=%d\n",
          mRemovedStopCondList.back());
      #endif
   }
   
   // go through the table and move upward if name is not blank
   for (Integer i=mCurrStopRow; i<MAX_STOPCOND_ROW; i++)
   {
      if (mTempStopCond[i+1].name != "")
      {
         // move upward
         mTempStopCond[i].isChanged = mTempStopCond[i+1].isChanged;
         mTempStopCond[i].name = mTempStopCond[i+1].name;
         mTempStopCond[i].desc = mTempStopCond[i+1].desc;
         mTempStopCond[i].varName = mTempStopCond[i+1].varName;
         mTempStopCond[i].typeName = mTempStopCond[i+1].typeName;
         mTempStopCond[i].relOpStr = mTempStopCond[i+1].relOpStr;
         mTempStopCond[i].goalStr = mTempStopCond[i+1].goalStr;
         mTempStopCond[i].tol = mTempStopCond[i+1].tol;
         mTempStopCond[i].repeat = mTempStopCond[i+1].repeat;
         mTempStopCond[i].stopCondPtr = mTempStopCond[i+1].stopCondPtr;
      }
      else
      {
         ClearStopCondition(i);
         break;
      }
   }

   mStopCondChanged = true;
   
   DisplayStopCondition();
}


//------------------------------------------------------------------------------
// void ClearStopCondition(Integer selRow)
//------------------------------------------------------------------------------
void PropagatePanel::ClearStopCondition(Integer selRow)
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::ClearStopCondition() selRow=%d\n", selRow);
   #endif
   
   mTempStopCond[selRow].isChanged = false;
   mTempStopCond[selRow].name = "";
   mTempStopCond[selRow].desc = "";
   mTempStopCond[selRow].varName = "";
   mTempStopCond[selRow].typeName = "";
   mTempStopCond[selRow].relOpStr = "";
   mTempStopCond[selRow].goalStr = "";
   mTempStopCond[selRow].tol = 0.0;
   mTempStopCond[selRow].repeat = 1;
   mTempStopCond[selRow].stopCondPtr = NULL;
}


//------------------------------------------------------------------------------
// wxString FormatStopCondDesc(const wxString &varName, const wxString &relOpStr,
//                             const wxString &goalStr)
//------------------------------------------------------------------------------
wxString PropagatePanel::FormatStopCondDesc(const wxString &varName,
                                            const wxString &relOpStr,
                                            const wxString &goalStr)
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::FormatStopCondDesc() entered\n");
   #endif

   wxString goalTempStr = goalStr;
   wxString desc;
   wxString opStr = relOpStr;
   
   if (varName.Contains("Apoapsis") || varName.Contains("Periapsis"))
   {
      opStr = "";
      goalTempStr = "";
   }
   
   desc = varName + " " + opStr + " " + goalTempStr;
    
   return desc;
}


//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles CheckBox event.
 */
//------------------------------------------------------------------------------
void PropagatePanel::OnCheckBoxChange(wxCommandEvent& event)
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::OnCheckBoxChange() entered\n");
   #endif

   if (event.GetEventObject() == backPropCheckBox)
   {
      mPropChanged = true;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagatePanel::OnComboBoxChange(wxCommandEvent& event)
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::OnComboBoxChange() entered\n");
   #endif

//   // Assume single selection
//   wxArrayInt stopRows = stopCondGrid->GetSelectedRows();
    
   if (event.GetEventObject() == mPropModeComboBox)
   {
      mPropModeChanged = true;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void OnKeyDown(wxKeyEvent& event)
//------------------------------------------------------------------------------
void PropagatePanel::OnKeyDown(wxKeyEvent& event)
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::OnKeyDown() entered\n");
   MessageInterface::ShowMessage
      ("PropagatePanel::OnKeyDown() mCurrPropRow=%d, mTempPropCount=%d\n", 
         mCurrPropRow, mTempPropCount);
   #endif

   if ( (event.GetKeyCode() == WXK_DELETE) || 
        (event.GetKeyCode() == WXK_NUMPAD_DELETE) )
   {
	   if (mPropGridSelected)
	   {
	   	if ( mTempProp[mCurrPropRow].propName != "")
	   	{
	         mTempProp[mCurrPropRow].propName = "";
	         
	         if (mCurrPropRow+1 <= mTempPropCount)
	            MoveUpPropData();
	         else
	            mTempPropCount--;

            mPropChanged = true;
	   	}
	   	else
            propGrid->DeselectRow(mCurrPropRow);
	   }
	   else if (mStopCondGridSelected)
	   {
         if (mTempStopCond[mCurrStopRow].name != "")
            RemoveStopCondition();
         else
            stopCondGrid->DeselectRow(mCurrStopRow);
      }
   }
}


//Note: Do not remove this code. It is used in showing stopping condition
//------------------------------------------------------------------------------
// void OnCellLeftDoubleClick(wxGridEvent& event)
//------------------------------------------------------------------------------
/*
 * On this event stopping condition can be added by clicking View button and
 * Update button to make update to the stopping condition grid.
 */
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// void OnCellRightClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void PropagatePanel::OnCellRightClick(wxGridEvent& event)
{    
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::OnCellRightClick() entered\n");
   #endif

   Integer row = event.GetRow();
   Integer col = event.GetCol();

   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::OnCellRightClick() row = %d, col = %d\n", row, col);
   MessageInterface::ShowMessage
      ("PropagatePanel::OnCellRightClick() mTempPropCount=%d\n",mTempPropCount);
   #endif

   // Propagate grid
   if (event.GetEventObject() == propGrid)
   {
      if (row <= mTempPropCount)
      {
         
         if (col == 0)
         {
            //----------------------------------
            // select propagator
            //----------------------------------
            PropagatorSelectDialog propDlg(this, mTempProp[row].propName);
            propDlg.ShowModal();
            
            if (propDlg.HasSelectionChanged())
            {
               wxString newPropName = propDlg.GetPropagatorName();
               
//               if (newPropName == "") // remove propagator
//               {
//                  propGrid->SetCellValue(row, col, "");
//                  mTempProp[row].isChanged = true;
//                  mTempProp[row].propName = "";
//                  if (row+1 < mTempPropCount)
//                     MoveUpPropData();
//                  else
//                     mTempPropCount--;
//               }
//               else // change propagator
//               {
                  propGrid->SetCellValue(row, col, newPropName);
                  mTempProp[row].isChanged = true;
                  mTempProp[row].propName = newPropName;
                  if (mTempPropCount <= row)
                     mTempPropCount = row + 1;
//               }
               
               #if DEBUG_PROPAGATE_PANEL
               MessageInterface::ShowMessage
                  ("PropagatePanel::OnCellRightClick() mTempPropCount=%d\n",
                   mTempPropCount);
               #endif
               
               mPropChanged = true;
               EnableUpdate(true);
            }
         }
         else if (col == 1)
         {
            //----------------------------------
            // select spacecraft
            //----------------------------------
            wxArrayString soExcList;
            Integer soCount = 0;
            for (Integer i=0; i<mTempPropCount; i++)
            {
               soCount = mTempProp[i].soNameList.Count();
            
               for (Integer j=0; j<soCount; j++)
                  soExcList.Add(mTempProp[i].soNameList[j]);
            }
            
            SpaceObjectSelectDialog soDlg(this, mTempProp[row].soNameList, 
               soExcList);
            soDlg.ShowModal();
         
            if (soDlg.IsSpaceObjectSelected())
            {
               wxArrayString &names = soDlg.GetSpaceObjectNames();
               mTempProp[row].isChanged = true;
               mTempProp[row].soNames = "";
               soCount = names.GetCount();
            
               #if DEBUG_PROPAGATE_PANEL
               MessageInterface::ShowMessage
                  ("PropagatePanel::OnCellRightClick() soCount=%d\n",
                   soCount);
               #endif
               
               mTempProp[row].soNameList.Clear();
               for (Integer j=0; j<soCount; j++)
               {
                  mTempProp[row].soNameList.Add(names[j]);
                  
                  #if DEBUG_PROPAGATE_PANEL
                  MessageInterface::ShowMessage
                     ("PropagatePanel::OnCellRightClick() soNameList[%d]=%s\n",
                      j, mTempProp[row].soNameList[j].c_str());
                  #endif
               }
            
               for(Integer j=0; j < soCount-1; j++)
               {
                  mTempProp[row].soNames += names[j].c_str();
                  mTempProp[row].soNames += ", ";
               }

               mTempProp[row].soNames += names[soCount-1].c_str();
               mTempProp[row].soCount = soCount;
               
               propGrid->SetCellValue(row, col, mTempProp[row].soNames);
               
               mPropChanged = true;
               EnableUpdate(true);
            }
         }

         mCurrPropRow = row;
      } // row <= mTempPropCount
   } // end of if (event.GetEventObject() == propGrid)

   // Stopping Condition grid
   else if (event.GetEventObject() == stopCondGrid)
   {
      #if DEBUG_PROPAGATE_PANEL
      MessageInterface::ShowMessage
         ("PropagatePanel::OnCellRightClick() stopCondCnt=%d\n",mStopCondCount);
      #endif

      if (row <= mStopCondCount)
      {
         #if DEBUG_PROPAGATE_PANEL
         Integer stopRow = mCurrStopRow;
         MessageInterface::ShowMessage
            ( "PropagatePanel::OnCellRightClick() stopRow = %d\n", stopRow );
         #endif
         
         if (col == 0)
         {
  
            // show dialog to select parameter
            ParameterSelectDialog paramDlg(this, mObjectTypeList);
            paramDlg.ShowModal();
      
            if (paramDlg.IsParamSelected())
            {
               wxString newParamName = paramDlg.GetParamName();
               stopCondGrid->SetCellValue( row, STOPCOND_PARAM_COL,
                  newParamName );
               mTempStopCond[row].isChanged = true;
                  
		         // if Apoapsis or Periapsis, disable goal
		         if (newParamName.Contains(".Periapsis") ||
		             newParamName.Contains(".Apoapsis"))
		         {
		            stopCondGrid->SetCellValue(row, STOPCOND_RELOPER_COL, "");
		            stopCondGrid->SetCellValue(row, STOPCOND_COND_COL, "");
				      stopCondGrid->SetReadOnly(row, STOPCOND_COND_COL, true);
		         }
		         else
		         {
		            stopCondGrid->SetCellValue(row, STOPCOND_RELOPER_COL, "=");
		            stopCondGrid->SetCellValue(row, STOPCOND_COND_COL, "0.0");
				      stopCondGrid->SetReadOnly(row, STOPCOND_COND_COL, false);
		         }
		      }
                  if (mStopCondCount <= row)
                     mStopCondCount = row + 1;
         } // col == 0
//		   else if (col == 1)
//		   {   
////            wxString ParamName = stopCondGrid->GetCellValue(row, STOPCOND_PARAM_COL);
////            
////	         // if Apoapsis or Periapsis, disable goal
////	         if (ParamName.Contains(".Periapsis") ||
////	             ParamName.Contains(".Apoapsis"))
////	         {
////				   stopCondGrid->SetReadOnly(row, STOPCOND_RELOPER_COL, true);
////	         }
////	         else
////	         {
//			      wxString oldStr = stopCondGrid->GetCellValue(row, col);
//			      wxString strArray[] = {wxT("="), wxT("!="), wxT(">"), wxT("<"), 
//			                             wxT(">="), wxT("<=")};        
//			      
//			      wxSingleChoiceDialog dialog(this, _T("Equality Selection: \n"),
//			                                        _T("IfConditionDialog"), 6, strArray);
//			      dialog.SetSelection(0);
//			      
//			      if (dialog.ShowModal() == wxID_OK)
//			      {
//			         if (oldStr != dialog.GetStringSelection())
//			         {
//			            stopCondGrid->SetCellValue(row, col, dialog.GetStringSelection());
//			            EnableUpdate(true);
//			         }
//			      }
////		      }
//		         
//		   } // col == 1
         else if (col == 2)
         {
            // show dialog to select parameter
            ParameterSelectDialog paramDlg(this, mObjectTypeList);
            paramDlg.ShowModal();
      
            if (paramDlg.IsParamSelected())
            {
               wxString newParamName = paramDlg.GetParamName();
               stopCondGrid->SetCellValue(row, STOPCOND_COND_COL,
                  newParamName);
               mTempStopCond[row].isChanged = true;
		      }
         } // col == 2
         
         mCurrStopRow = row;
			UpdateStopCondition();
         EnableUpdate(true);
      } // row <= mStopCondCount
   } // event.GetEventObject() == stopCondGrid
}

//------------------------------------------------------------------------------
// void OnCellLeftDoubleClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void PropagatePanel::OnCellLeftDoubleClick(wxGridEvent& event)
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::OnCellLeftDoubleClick() entered\n");
   #endif

   Integer row = event.GetRow();
  
   if (event.GetEventObject() == propGrid)
   {
      propGrid->SelectRow(row);
      stopCondGrid->DeselectRow(mCurrStopRow);
      mPropGridSelected    = true;
      mStopCondGridSelected = false;
      mCurrPropRow = row;
   }
   else if (event.GetEventObject() == stopCondGrid)
   {
      stopCondGrid->SelectRow(row);
      propGrid->DeselectRow(mCurrPropRow);
      mPropGridSelected     = false;
      mStopCondGridSelected = true;
      mCurrStopRow = row;
   }
}


//------------------------------------------------------------------------------
// void OnCellValueChange(wxGridEvent& event)
//------------------------------------------------------------------------------
void PropagatePanel::OnCellValueChange(wxGridEvent& event)
{
   Integer row = event.GetRow();
   Integer col = event.GetCol();

   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::OnCellValueChange() entered\n");
   MessageInterface::ShowMessage
      ("PropagatePanel::OnCellValueChange() row=%d, mStopCondCount=%d\n",
      row, mStopCondCount);
   #endif

   if (row <= mStopCondCount)
   {
      if (event.GetEventObject() == stopCondGrid)
      {
         
         if (col == 2)
         {

            wxString temp = stopCondGrid->GetCellValue(row, col);
      
            if (temp.IsEmpty())
               return;
            else
            {
               stopCondGrid->SetCellValue(row, STOPCOND_COND_COL,
                  temp);
               mTempStopCond[row].isChanged = true;
            }
         } // end if (col == 2)
         
         mCurrStopRow = row;
		   UpdateStopCondition();
         EnableUpdate(true);
      }
   }
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void PropagatePanel::LoadData()
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage("PropagatePanel::LoadData() entered\n");
   #endif

   // Set the pointer for the "Show Script" button
   mObject = thePropCmd;
   
   //----------------------------------
   // propagation mode
   //----------------------------------
   std::string mode = thePropCmd->GetStringParameter
      ( thePropCmd->GetParameterID("PropagateMode") );
   mPropModeComboBox->SetStringSelection( mode.c_str() );

   //----------------------------------
   // Backwards propagation
   //----------------------------------
///@todo Need to be implemented in base code for Propagate Command
//   Integer backPropId = thePropCmd->GetParameterID("BackProp");
//   bool backProp = thePropCmd->GetBooleanParameter(backPropId);
//   backPropCheckBox->SetValue(backProp);

   //----------------------------------
   // propagator
   //----------------------------------
   Integer propId = thePropCmd->GetParameterID( "Propagator" );

   // Get the list of propagators (aka the PropSetups)
   StringArray propNames = thePropCmd->GetStringArrayParameter( propId );
   mPropCount = propNames.size();
   
   StringArray soList;
   
   wxString name;
   bool backProp = false;
   
   mTempPropCount = mPropCount;
   
   Integer scId = thePropCmd->GetParameterID( "Spacecraft" );

   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::LoadData() mPropCount=%d\n", mPropCount);
   #endif

   Integer soCount = 0;
   
   for ( Integer i=0; i<mPropCount; i++ )
   {
      mTempProp[i].propName = wxT( propNames[i].c_str() );

      // Get the list of spacecraft and formations
      soList = thePropCmd->GetStringArrayParameter( scId, i );
      soCount = soList.size();
      
	   #if DEBUG_PROPAGATE_PANEL
	   MessageInterface::ShowMessage
	      ("PropagatePanel::LoadData() propName=%s, soCount=%d\n",
	       propNames[i].c_str(), soCount);
	   #endif
   
      Integer actualSoCount = 0;
      for (Integer j=0; j<soCount; j++)
      {
         #if DEBUG_PROPAGATE_PANEL
         MessageInterface::ShowMessage
            ("PropagatePanel::LoadData() soNameList[%d]=<%s>\n",
             j, soList[j].c_str());
         #endif
         
         // verify space object actually exist
//         if ( theGuiInterpreter->GetObject(soList[j]) ||
//              theGuiInterpreter->GetObject(soList[j]) )
         if ( theGuiInterpreter->GetObject(soList[j]) )
         {
            actualSoCount++;
            mTempProp[i].soNameList.Add(soList[j].c_str());
         }
         else
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_, "The SpaceObject:<%s> was not created, "
                "so removed from the display list\n", soList[j].c_str());
         }
      }

      soCount = actualSoCount;
      mTempProp[i].soCount = actualSoCount;
      
      if (soCount > 0)
      {
         for (Integer j=0; j<soCount-1; j++)
         {
            mTempProp[i].soNames += soList[j].c_str();
            mTempProp[i].soNames += ", ";
         }

         mTempProp[i].soNames += soList[soCount-1].c_str();
      }
      
///@todo Will be remove was base code is changed -> LTR 1/18/07
      // Check if backwards propagation
      name = mTempProp[i].propName;
      Integer x = name.Find("-");
      
      if (x == -1)
      {
         backProp = false;
      }
      else
      {
         backProp = true;
         name.Replace("-", "", false);
         mTempProp[i].propName = name; 
      }            
      
   } // for (Integer i=0; i<mPropCount; i++)
   
   backPropCheckBox->SetValue(backProp);

   //----------------------------------
   // stopping conditions
   //----------------------------------
   ObjectArray &stopArray =
      thePropCmd->GetRefObjectArray(Gmat::STOP_CONDITION);
   mStopCondCount = stopArray.size();
      
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::LoadData() mStopCondCount=%d\n",
       mStopCondCount);
   #endif
   
   StopCondition  *stopCond;
   for ( Integer i=0; i<mStopCondCount; i++ )
   {
      stopCond = (StopCondition *)stopArray[i];
      
      #if DEBUG_PROPAGATE_PANEL
      MessageInterface::ShowMessage
         ("PropagatePanel::LoadData() stopArray[%d]=%s\n",
          i, stopCond->GetName().c_str());
      #endif
      
      // StopCondition created from the script might not have been 
      // configured (unnamed)
      if ( stopCond != NULL )
      {
         mTempStopCond[i].stopCondPtr = stopCond;
         mTempStopCond[i].name = wxT(stopCond->GetName().c_str());
         mTempStopCond[i].varName = 
            wxT(stopCond->GetStringParameter("StopVar").c_str());
         mTempStopCond[i].typeName = wxT(stopCond->GetTypeName().c_str());
         mTempStopCond[i].goalStr = 
            stopCond->GetStringParameter("Goal").c_str();
         mTempStopCond[i].tol = stopCond->GetRealParameter("Tol");
         mTempStopCond[i].repeat = stopCond->GetIntegerParameter("Repeat");
         wxString str = FormatStopCondDesc(mTempStopCond[i].varName,
                                           mTempStopCond[i].relOpStr,
                                           mTempStopCond[i].goalStr);
         mTempStopCond[i].desc = str;
      }
   }
   
   mCurrStopRow = 0;
   DisplayPropagator();
   DisplayStopCondition();
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void PropagatePanel::SaveData()
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::SaveData() mTempPropCount=%d, mRemovedStopCondList.size()=%d\n",
      mTempPropCount, mRemovedStopCondList.size());
   #endif
   canClose = true;

   Real realNum;
   std::string msg = "The condition of \"%s\" for parameter \"%s\" is not an"
                     " allowed value. \nThe allowed values are: "
                     "[Real number, Array element, Variable or Object parameter].";

   //-----------------------------------------------------------------
   // check input for Condition - Real number or parameter
   //-----------------------------------------------------------------
   if (mStopCondChanged)
   {
      for (Integer i=0; i<mStopCondCount; i++)
      {
         if (mTempStopCond[i].isChanged)
         {
            if (!wxString(mTempStopCond[i].goalStr.c_str()).ToDouble(&realNum))
            {
               theParameter = 
                  theGuiInterpreter->GetParameter(mTempStopCond[i].goalStr.c_str());
               if (theParameter == NULL)
               {
                  MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
                     mTempStopCond[i].goalStr.c_str(), mTempStopCond[i].varName.c_str());
                  canClose = false;
               }
            }
         }
      }
   }

   if (!canClose)
      return;

   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
	   //---------------------------------------------
	   // Delete any removed stopCondition pointer
	   //---------------------------------------------
	   for (unsigned int i=0; i<mRemovedStopCondList.size(); i++)
	   {
	      #if DEBUG_PROPAGATE_PANEL
	      MessageInterface::ShowMessage
	         ("PropagatePanel::SaveData() deleting StopCondition addr=%d\n",
	          mRemovedStopCondList[i]);
	      #endif
	      
	      delete mRemovedStopCondList[i];
	   }
	   
      mRemovedStopCondList.clear();
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::SaveData() mRemovedStopCondList.size()=%d\n",
       mRemovedStopCondList.size());
   #endif
	   
	   //-------------------------------------------------------
	   // Saving propagation mode
	   //-------------------------------------------------------
	   if (mPropModeChanged)
	   {
	      mPropModeChanged = false;
	      wxString str = mPropModeComboBox->GetStringSelection();
	      if (str.CmpNoCase("None") == 0)
	         str = "";
	      thePropCmd->SetStringParameter
	         (thePropCmd->GetParameterID("PropagateMode"), str.c_str());
	   }
	   
	   //-------------------------------------------------------
	   // Saving backward propragation
	   //-------------------------------------------------------
	///@todo Need to be implemented in base code for Propagate Command
	//   Integer backPropId = thePropCmd->GetParameterID("BackProp");
	//   bool backProp = backPropCheckBox->GetValue();
	//   thePropCmd->SetBooleanParameter(backPropId, backProp);
	   for (Integer i=0; i<mPropCount; i++)
	   {
	         if (backPropCheckBox->IsChecked())
	         {
	            mTempProp[i].propName.Prepend("-");  
	         }
	         else
	         {
	            wxString name = mTempProp[i].propName;
	            name.Replace("-", "", false);
	            mTempProp[i].propName = name; 
	         } 
	
	         #if DEBUG_PROPAGATE_PANEL
	         MessageInterface::ShowMessage
	            ("PropagatePanel::SaveData() propName[%d]=%s\n",
	             i,mTempProp[i].propName.c_str());
	         #endif
	   }
	   
	   Integer soCount = 0;
   
      //---------------------------------------
      // Saving propagator and spacecraft
      //---------------------------------------
      if (mPropChanged)
      {
         mPropChanged = false;

         Integer propId = thePropCmd->GetParameterID("Propagator");
         Integer scId = thePropCmd->GetParameterID("Spacecraft");
         
         // Clear propagator and spacecraft list
         thePropCmd->TakeAction("Clear", "Propagator");
         
         mPropCount = mTempPropCount;
         
         #if DEBUG_PROPAGATE_PANEL
         MessageInterface::ShowMessage
            ("PropagatePanel::SaveData() mPropCount=%d\n", mPropCount);
         #endif
         
         for (Integer i=0; i<mPropCount; i++)
         {
            //---------------------------------------
            // Saving propagator
            //---------------------------------------
            thePropCmd->SetStringParameter
               (propId, std::string(mTempProp[i].propName.c_str()));
            
            //---------------------------------------
            // Saving spacecraft
            //---------------------------------------
            soCount = mTempProp[i].soNameList.Count();
            
            #if DEBUG_PROPAGATE_PANEL
            MessageInterface::ShowMessage
               ("PropagatePanel::SaveData() soCount=%d\n", soCount);
            #endif
            
            for (Integer j=0; j<soCount; j++)
            {
               #if DEBUG_PROPAGATE_PANEL > 1
               MessageInterface::ShowMessage
                  ("PropagatePanel::SaveData() propIndex=%d, soNameList[%d]=%s\n",
                   i, j, mTempProp[i].soNameList[j].c_str());
               #endif
               
               thePropCmd->SetStringParameter
                  (scId, std::string(mTempProp[i].soNameList[j].c_str()), i);
               
            }
         }
      } //if (mPropChanged)
      
      //-------------------------------------------------------
      // Saving stopping condition
      //-------------------------------------------------------
      if (mStopCondChanged)
      {
         mStopCondChanged = false;
         
         Integer stopCount = 0;
         for (Integer i=0; i<MAX_STOPCOND_ROW; i++)
         {
            if (mTempStopCond[i].desc != "")
               stopCount++;
         }
         
         #if DEBUG_PROPAGATE_PANEL
         MessageInterface::ShowMessage
            ("PropagatePanel::SaveData() stopCount=%d\n", stopCount);
         #endif
         
         //---------------------------------------------
         // if any stoppping condition removed,
         // clear Propagate StopCondition array and
         // set flag to add as new
         //---------------------------------------------
         if (stopCount < mStopCondCount)
         {
            thePropCmd->TakeAction("Clear", "StopCondition");
            for (Integer i=0; i<stopCount; i++)
               mTempStopCond[i].isChanged = true;
         }
         
         mStopCondCount = stopCount;
         
         //---------------------------------------------
         // Set stopCondition.
         //---------------------------------------------
         for (Integer i=0; i<mStopCondCount; i++)
         {
            if (mTempStopCond[i].isChanged)
            {
               #if DEBUG_PROPAGATE_PANEL
               MessageInterface::ShowMessage
                  ("PropagatePanel::SaveData() mTempStopCond[%d].isChanged=%d\n", i,
                   mTempStopCond[i].isChanged);
               #endif
               
               if (mTempStopCond[i].stopCondPtr != NULL)
               {
                  mTempStopCond[i].stopCondPtr->
                     SetName(std::string(mTempStopCond[i].name.c_str()));
                  
                  mTempStopCond[i].stopCondPtr->
                     SetStringParameter("StopVar", mTempStopCond[i].varName.c_str());
                  
                  mTempStopCond[i].stopCondPtr->
                     SetStringParameter("Goal", mTempStopCond[i].goalStr.c_str());
                  
                  mTempStopCond[i].stopCondPtr->
                     SetRealParameter("Tol", mTempStopCond[i].tol);
                  
                  mTempStopCond[i].stopCondPtr->
                     SetIntegerParameter("Repeat", mTempStopCond[i].repeat);
                  
                  thePropCmd->
                     SetRefObject(mTempStopCond[i].stopCondPtr, Gmat::STOP_CONDITION,
                                  "", i);
                  
                  #if DEBUG_PROPAGATE_PANEL
                  StopCondition *stopCond = (StopCondition*)thePropCmd->
                     GetRefObject(Gmat::STOP_CONDITION,
                                  mTempStopCond[i].stopCondPtr->GetName(), i);
                  MessageInterface::ShowMessage
                     ("PropagatePanel::SaveData() name=%s, stopVarType=%s, goal = %s\n",
                      stopCond->GetName().c_str(),
                      stopCond->GetStringParameter("StopVar").c_str(),
                      stopCond->GetStringParameter("Goal").c_str());
                 #endif
               }
            }
         }
      }//if (mStopCondChanged)
      
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetMessage());
      canClose = false;
      return;
   }
}
