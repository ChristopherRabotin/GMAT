//$Id$
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
#include "StringUtil.hpp"               // for SeparateBy()
#include "MessageInterface.hpp"

//#define DEBUG_PROPAGATE_PANEL
//#define DEBUG_PROPAGATE_PANEL_LOAD
//#define DEBUG_RENAME

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(PropagatePanel, GmatPanel)
   EVT_GRID_CELL_RIGHT_CLICK(PropagatePanel::OnCellRightClick)
   EVT_GRID_CELL_CHANGE(PropagatePanel::OnCellValueChange)
   EVT_CHECKBOX(ID_CHECKBOX, PropagatePanel::OnCheckBoxChange)
   EVT_COMBOBOX(ID_COMBOBOX, PropagatePanel::OnComboBoxChange)
   EVT_TEXT(ID_TEXTCTRL, PropagatePanel::OnTextChange)
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
   #ifdef DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage ("PropagatePanel::PropagatePanel() entered\n");
   #endif
   
   thePropCmd = (Propagate *)cmd;
   
   // Default values
   mPropModeChanged = false;
   mPropDirChanged = false;
   mPropSatChanged = false;
   mStopCondChanged = false;
   mStopTolChanged = false;
   canClose = true;

   InitializeData();   
   mObjectTypeList.Add("Spacecraft");
   
   Create();
   Show();
   EnableUpdate(false);
   
   // Listen for Propagator or Spacecraft name change
   theGuiManager->AddToResourceUpdateListeners(this);
}


//------------------------------------------------------------------------------
// ~PropagatePanel()
//------------------------------------------------------------------------------
PropagatePanel::~PropagatePanel()
{
   mObjectTypeList.Clear();
   theGuiManager->RemoveFromResourceUpdateListeners(this);
}


//------------------------------------------------------------------------------
// virtual bool PrepareObjectNameChange()
//------------------------------------------------------------------------------
bool PropagatePanel::PrepareObjectNameChange()
{
   // Save GUI data
   wxCommandEvent event;
   OnApply(event);
   
   return GmatPanel::PrepareObjectNameChange();
}


//------------------------------------------------------------------------------
// virtual void ObjectNameChanged(Gmat::ObjectType type, const wxString &oldName,
//                                const wxString &newName)
//------------------------------------------------------------------------------
/*
 * Reflects resource name change to this panel.
 * By the time this method is called, the base code already changed reference
 * object name, so all we need to do is re-load the data.
 */
//------------------------------------------------------------------------------
void PropagatePanel::ObjectNameChanged(Gmat::ObjectType type,
                                       const wxString &oldName,
                                       const wxString &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("PropagatePanel::ObjectNameChanged() type=%d, oldName=<%s>, "
       "newName=<%s>, mDataChanged=%d\n", type, oldName.c_str(), newName.c_str(),
       mDataChanged);
   #endif
   
   if (type != Gmat::PROP_SETUP && type != Gmat::SPACECRAFT &&
       type != Gmat::PARAMETER)
      return;
   
   // Initialize GUI data and re-load from base
   InitializeData();
   LoadData();
   
   // We don't need to save data if object name changed from the resouce tree
   // while this panel is opened, since base code already has new name
   EnableUpdate(false);
}


//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void PropagatePanel::Create()
{
   #ifdef DEBUG_PROPAGATE_PANEL
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
      new wxCheckBox(this, ID_CHECKBOX, wxT("Backwards Propagation"),
                      wxDefaultPosition, wxDefaultSize, 0);
   
   // Propagator Grid
   propGrid =
      new wxGrid(this, ID_GRID, wxDefaultPosition, wxSize(700,100), 
         wxWANTS_CHARS);
   
   propGrid->CreateGrid(MAX_PROP_ROW, 2, wxGrid::wxGridSelectCells);
   
   propGrid->SetColLabelValue(PROP_NAME_COL, _T("Propagator"));
   propGrid->SetColSize(PROP_NAME_COL, 340);
   propGrid->SetColLabelValue(PROP_SOS_COL, _T("Spacecraft List"));
   propGrid->SetColSize(PROP_SOS_COL, 340);
   
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
      stopCondGrid->SetReadOnly(i, STOPCOND_RELOPER_COL, true);
      stopCondGrid->SetReadOnly(i, STOPCOND_COND_COL, true);
   }
   
   stopCondGrid->SetMargins(0, 0);
   stopCondGrid->SetRowLabelSize(0);
   stopCondGrid->SetScrollbars(5, 8, 15, 15);
   
   
   wxFlexGridSizer *propModeSizer = new wxFlexGridSizer(4, 0, 0);
   wxBoxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);
   GmatStaticBoxSizer *propSizer = 
      new GmatStaticBoxSizer(wxVERTICAL, this, "Propagators and Spacecraft");
   
   //Adding objects to sizers
   propModeSizer->Add(synchStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   propModeSizer->Add(mPropModeComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   propModeSizer->Add(200, 20, wxALIGN_CENTRE|wxALL, bsize);
   propModeSizer->Add(backPropCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   propSizer->Add(propModeSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   propSizer->Add(propGrid, 0, wxALIGN_CENTER|wxALL, bsize);
   
   // Stop tolerance
   wxStaticText *stopTolStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Stop Tolerance: "), 
                       wxDefaultPosition, wxSize(-1, -1), 0);
   mStopTolTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(150,-1), 0);
   wxBoxSizer *stopTolSizer = new wxBoxSizer(wxHORIZONTAL);
   
   stopTolSizer->Add(stopTolStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   stopTolSizer->Add(mStopTolTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   
   // Stopping conditions
   GmatStaticBoxSizer *stopSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Stopping Conditions");
   
   stopSizer->Add(stopTolSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   stopSizer->Add(stopCondGrid, 0, wxALIGN_CENTER|wxALL, bsize);
   
   pageSizer->Add(propSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   pageSizer->Add(stopSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   theMiddleSizer->Add(pageSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
}


//------------------------------------------------------------------------------
// void InitializeData()
//------------------------------------------------------------------------------
void PropagatePanel::InitializeData()
{
   mPropModeCount = 1;
   mPropCount     = 0;
   mStopCondCount = 0;
   
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
      mTempStopCond[i].relOpStr = "";
      mTempStopCond[i].goalStr = "";
      mTempStopCond[i].stopCondPtr = NULL;
   }
}


//------------------------------------------------------------------------------
// void DisplayPropagator()
//------------------------------------------------------------------------------
void PropagatePanel::DisplayPropagator()
{
   #ifdef DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::DisplayPropagator() entered\n");
   #endif
   
   wxString name;
   for (Integer i=0; i<mPropCount; i++)
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
   #ifdef DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::DisplayStopCondition() \n");
   #endif
   
   //----- for stopCondGrid - show all
   for (Integer i=0; i<mStopCondCount; i++)
   {
      stopCondGrid->
         SetCellValue(i, STOPCOND_PARAM_COL, mTempStopCond[i].varName);

      if (mTempStopCond[i].varName.Contains(".Periapsis") ||
           mTempStopCond[i].varName.Contains(".Apoapsis")  ||
           strcmp(mTempStopCond[i].varName, "") == 0)
      {
              stopCondGrid->SetCellValue(i, STOPCOND_RELOPER_COL, "");
              stopCondGrid->SetCellValue(i, STOPCOND_COND_COL, "");
              stopCondGrid->SetReadOnly(i, STOPCOND_COND_COL, true);
      }
      else
      {
         stopCondGrid->SetCellValue(i, STOPCOND_RELOPER_COL, "=");
         stopCondGrid->
            SetCellValue(i, STOPCOND_COND_COL, mTempStopCond[i].goalStr);
         stopCondGrid->SetReadOnly(i, STOPCOND_COND_COL, false);
      }   
   }
   
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void UpdateStopCondition()
//------------------------------------------------------------------------------
void PropagatePanel::UpdateStopCondition(Integer stopRow)
{
   #ifdef DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::UpdateStopCondition() entered\n");
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
   if (mTempStopCond[stopRow].varName.Contains(".Periapsis") ||
        mTempStopCond[stopRow].varName.Contains(".Apoapsis"))
   {
      mTempStopCond[stopRow].goalStr = "0.0";
   }
   else
   {
      mTempStopCond[stopRow].goalStr = 
         stopCondGrid->GetCellValue(stopRow, STOPCOND_COND_COL);
   }

   #ifdef DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::UpdateStopCondition() "
       "StopCondition: old name = %s\n", oldStopName.c_str());
   MessageInterface::ShowMessage
      ("PropagatePanel::UpdateStopCondition() "
       "StopCondition: name = %s\n", mTempStopCond[stopRow].name.c_str());
   MessageInterface::ShowMessage
      ("PropagatePanel::UpdateStopCondition() "
       "StopCondition: condition = %s\n", 
       mTempStopCond[stopRow].goalStr.c_str());
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
      StopCondition *stopCond = 
         (StopCondition*)theGuiInterpreter-> CreateStopCondition
         ("StopCondition", mTempStopCond[stopRow].name.c_str());
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
// wxString FormatStopCondDesc(const wxString &varName, 
//                             const wxString &relOpStr,
//                             const wxString &goalStr)
//------------------------------------------------------------------------------
wxString PropagatePanel::FormatStopCondDesc(const wxString &varName,
                                            const wxString &relOpStr,
                                            const wxString &goalStr)
{
   #ifdef DEBUG_PROPAGATE_PANEL
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
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagatePanel::OnTextChange(wxCommandEvent& event)
{
   mStopTolChanged = true;
   EnableUpdate(true);
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
   #ifdef DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::OnCheckBoxChange() entered\n");
   #endif

   if (event.GetEventObject() == backPropCheckBox)
   {
      mPropDirChanged = true;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagatePanel::OnComboBoxChange(wxCommandEvent& event)
{
   #ifdef DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::OnComboBoxChange() entered\n");
   #endif
   
   if (event.GetEventObject() == mPropModeComboBox)
   {
      mPropModeChanged = true;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void OnCellRightClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void PropagatePanel::OnCellRightClick(wxGridEvent& event)
{    
   Integer row = event.GetRow();
   Integer col = event.GetCol();
   
   #ifdef DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::OnCellRightClick() row = %d, col = %d\n", row, col);
   #endif
   
   // Propagate grid
   if (event.GetEventObject() == propGrid)
   {
      propGrid->SetGridCursor(row, col);
      
      if (col == 0)
      {
         //----------------------------------
         // select propagator
         //----------------------------------
         PropagatorSelectDialog propDlg(this, "");
         propDlg.ShowModal();
         
         if (propDlg.HasSelectionChanged())
         {
            wxString newPropName = propDlg.GetPropagatorName();
            #ifdef DEBUG_PROPAGATE_PANEL
            MessageInterface::ShowMessage
               ("PropagatePanel::OnCellRightClick() newPropName = %s\n",
                newPropName.c_str());
            #endif
            
            propGrid->SetCellValue(row, col, newPropName);
            mPropSatChanged = true;
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

         for (Integer i=0; i<MAX_PROP_ROW; i++)
         {
            soCount = mTempProp[i].soNameList.Count();
         
            for (Integer j=0; j<soCount; j++)
               soExcList.Add(mTempProp[i].soNameList[j]);
         }
         
         SpaceObjectSelectDialog soDlg(this, mTempProp[row].soNameList, soExcList);
         soDlg.ShowModal();
         
         if (soDlg.HasSelectionChanged())
         {
            wxArrayString &names = soDlg.GetSpaceObjectNames();
            mTempProp[row].isChanged = true;
            mTempProp[row].soNames = "";
            soCount = names.GetCount();
            
            #ifdef DEBUG_PROPAGATE_PANEL
            MessageInterface::ShowMessage
               ("PropagatePanel::OnCellRightClick() new soCount=%d\n", soCount);
            #endif
            
            mTempProp[row].soNameList.Clear();
            for (Integer j=0; j<soCount; j++)
            {
               mTempProp[row].soNameList.Add(names[j]);
               
               #ifdef DEBUG_PROPAGATE_PANEL
               MessageInterface::ShowMessage
                  ("PropagatePanel::OnCellRightClick() soNameList[%d]=%s\n",
                   j, mTempProp[row].soNameList[j].c_str());
               #endif
            }
            
            if (soCount > 0)
            {
               for(Integer j=0; j < soCount-1; j++)
               {
                  mTempProp[row].soNames += names[j].c_str();
                  mTempProp[row].soNames += ", ";
               }
               
               mTempProp[row].soNames += names[soCount-1].c_str();
            }
            
            mTempProp[row].soCount = soCount;
            
            propGrid->SetCellValue(row, col, mTempProp[row].soNames);
            
            mPropSatChanged = true;
            EnableUpdate(true);
         } // soDlg.HasSelectionChanged()
      } // else if (col == 1)
   } // end of if (event.GetEventObject() == propGrid)
   
   // Stopping Condition grid
   else if (event.GetEventObject() == stopCondGrid)
   {
      stopCondGrid->SetGridCursor(row, col);
      
      if (col == 0)
      {
         // show dialog to select parameter
         // we cannot allow Variables
         ParameterSelectDialog paramDlg(this, mObjectTypeList);
         paramDlg.ShowModal();
         
         if (paramDlg.HasSelectionChanged())
         {
            wxString newParamName = paramDlg.GetParamName();
            stopCondGrid->SetCellValue(row,STOPCOND_PARAM_COL,newParamName);
            
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
            
            mStopCondChanged = true;
            UpdateStopCondition(row);
            EnableUpdate(true);
         }
      }
      else if (col == 2)
      {
         wxString paramName = stopCondGrid->GetCellValue(row, STOPCOND_PARAM_COL);
         
         // do nothing if parameter is empty
         if (paramName == "")
            return;
         
         // do nothing if parameter contains Apoapsis or Periapsis
         if (paramName.Contains(".Periapsis") ||
             paramName.Contains(".Apoapsis"))
            return;
         
         // show dialog to select parameter
         // we can allow Variables
         ParameterSelectDialog paramDlg(this, mObjectTypeList);
         paramDlg.ShowModal();
         
         if (paramDlg.HasSelectionChanged())
         {
            wxString newParamName = paramDlg.GetParamName();
            stopCondGrid->SetCellValue(row, STOPCOND_COND_COL, newParamName);
            mStopCondChanged = true;
            UpdateStopCondition(row);
            EnableUpdate(true);
         }
      }
      
   } // event.GetEventObject() == stopCondGrid
}


//------------------------------------------------------------------------------
// void OnCellValueChange(wxGridEvent& event)
//------------------------------------------------------------------------------
void PropagatePanel::OnCellValueChange(wxGridEvent& event)
{
   Integer row = event.GetRow();
   Integer col = event.GetCol();
   
   #ifdef DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::OnCellValueChange() row=%d, col=%d, ", row, col);
   #endif
   
   if (event.GetEventObject() == stopCondGrid)
   {
      wxString paramName = stopCondGrid->GetCellValue(row, STOPCOND_PARAM_COL);
      wxString condValue = stopCondGrid->GetCellValue(row, STOPCOND_COND_COL);
      
      #ifdef DEBUG_PROPAGATE_PANEL
      MessageInterface::ShowMessage
         ("paramName=<%s>, condValue=<%s>\n", paramName.c_str(), condValue.c_str());
      #endif
      
      if (col == STOPCOND_PARAM_COL)
      {
         // if Apoapsis or Periapsis, disable goal
         if (paramName.Contains(".Periapsis") || paramName.Contains(".Apoapsis"))
         {
            stopCondGrid->SetCellValue(row, STOPCOND_RELOPER_COL, "");
            stopCondGrid->SetCellValue(row, STOPCOND_COND_COL, "");
            stopCondGrid->SetReadOnly(row, STOPCOND_COND_COL, true);
         }
         else if (paramName == "")
         {
            // do not show = sign if codition is empty
            if (condValue == "")
               stopCondGrid->SetCellValue(row, STOPCOND_RELOPER_COL, "");
         }
         else
         {
            stopCondGrid->SetCellValue(row, STOPCOND_RELOPER_COL, "=");
            if (stopCondGrid->GetCellValue(row, STOPCOND_COND_COL) == "")
               stopCondGrid->SetCellValue(row, STOPCOND_COND_COL, "0.0");
            stopCondGrid->SetReadOnly(row, STOPCOND_COND_COL, false);
         }
      }
      else if (col == STOPCOND_COND_COL)
      {
         // do not show = sign if pameter and codition is empty
         if (paramName == "" && condValue == "")
            stopCondGrid->SetCellValue(row, STOPCOND_RELOPER_COL, "");
      }
      
      mStopCondChanged = true;
   }
   
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void PropagatePanel::LoadData()
{
   #ifdef DEBUG_PROPAGATE_PANEL_LOAD
   MessageInterface::ShowMessage("PropagatePanel::LoadData() entered\n");
   #endif
   
   // Set the pointer for the "Show Script" button
   mObject = thePropCmd;
   
   //----------------------------------
   // propagation mode
   //----------------------------------
   std::string mode =
      thePropCmd->GetStringParameter(thePropCmd->GetParameterID("PropagateMode"));
   mPropModeComboBox->SetStringSelection(mode.c_str());
   
   //----------------------------------
   // Backwards propagation
   //----------------------------------
   Integer PropDirectionId = thePropCmd->GetParameterID("PropForward");
   bool backProp = !thePropCmd->GetBooleanParameter(PropDirectionId);
   backPropCheckBox->SetValue(backProp);
   
   //----------------------------------
   // propagator
   //----------------------------------
   Integer propId = thePropCmd->GetParameterID("Propagator");
   
   // Get the list of propagators (aka the PropSetups)
   StringArray propNames = thePropCmd->GetStringArrayParameter(propId);
   mPropCount = propNames.size();
   
   StringArray soList;
   wxString name;
   
   Integer scId = thePropCmd->GetParameterID("Spacecraft");

   #ifdef DEBUG_PROPAGATE_PANEL_LOAD
   MessageInterface::ShowMessage
      ("PropagatePanel::LoadData() mPropCount=%d\n", mPropCount);
   #endif
   
   Integer soCount = 0;
   
   for (Integer i=0; i<mPropCount; i++)
   {
      mTempProp[i].propName = wxT(propNames[i].c_str());
      
      // Get the list of spacecraft and formations
      soList = thePropCmd->GetStringArrayParameter(scId, i);
      soCount = soList.size();
      
      #ifdef DEBUG_PROPAGATE_PANEL_LOAD
      MessageInterface::ShowMessage
         ("PropagatePanel::LoadData() propName=%s, soCount=%d\n",
          propNames[i].c_str(), soCount);
      #endif
      
      Integer actualSoCount = 0;
      for (Integer j=0; j<soCount; j++)
      {
         #ifdef DEBUG_PROPAGATE_PANEL_LOAD
         MessageInterface::ShowMessage
            ("PropagatePanel::LoadData() soNameList[%d]=<%s>\n",
             j, soList[j].c_str());
         #endif
         
         // verify space object actually exist
         if (theGuiInterpreter->GetConfiguredObject(soList[j]))
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
      
   } // for (Integer i=0; i<mPropCount; i++)
   
   backPropCheckBox->SetValue(backProp);
   
   //----------------------------------
   // stopping conditions
   //----------------------------------
   
   Real stopTol = thePropCmd->GetRealParameter("StopTolerance");
   mStopTolTextCtrl->SetValue(theGuiManager->ToWxString(stopTol));
   
   ObjectArray &stopArray =
      thePropCmd->GetRefObjectArray(Gmat::STOP_CONDITION);
   mStopCondCount = stopArray.size();
   
   #ifdef DEBUG_PROPAGATE_PANEL_LOAD
   MessageInterface::ShowMessage
      ("PropagatePanel::LoadData() mStopCondCount=%d\n", mStopCondCount);
   #endif
   
   StopCondition  *stopCond;
   for (Integer i=0; i<mStopCondCount; i++)
   {
      stopCond = (StopCondition *)stopArray[i]; 
      
      #ifdef DEBUG_PROPAGATE_PANEL_LOAD
      MessageInterface::ShowMessage
         ("PropagatePanel::LoadData() stopCond=%p, stopArray[%d]=%s\n",
          stopCond, i, stopCond->GetName().c_str());
      #endif
      
      // StopCondition created from the script might not have been 
      // configured (unnamed)
      if (stopCond != NULL)
      {
         mTempStopCond[i].stopCondPtr = stopCond;
         mTempStopCond[i].name = wxT(stopCond->GetName().c_str());
         mTempStopCond[i].varName = 
            wxT(stopCond->GetStringParameter("StopVar").c_str());
         mTempStopCond[i].goalStr = 
            stopCond->GetStringParameter("Goal").c_str();
         wxString str = FormatStopCondDesc(mTempStopCond[i].varName,
                                           mTempStopCond[i].relOpStr,
                                           mTempStopCond[i].goalStr);
         mTempStopCond[i].desc = str;
      }
   }
   
   DisplayPropagator();
   DisplayStopCondition();
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void PropagatePanel::SaveData()
{
   #ifdef DEBUG_PROPAGATE_PANEL_SAVE
   MessageInterface::ShowMessage("PropagatePanel::SaveData() entered\n");
   #endif
   
   canClose = true;
   Integer blankProps = 0;
   wxArrayString emptyProps, emptySos;
   wxString propName, soNames;
   
   //-----------------------------------------------------------------
   // check valid propagators and space objects
   //-----------------------------------------------------------------
   for (Integer i=0; i<MAX_PROP_ROW; i++)
   {
      propName = propGrid->GetCellValue(i, PROP_NAME_COL);
      soNames = propGrid->GetCellValue(i, PROP_SOS_COL);
      
      if (propName == "" && soNames == "")
         ++blankProps;
      else if (propName != "" && soNames == "")
         emptySos.Add(propName);
      else if (propName == "" && soNames != "")
         emptyProps.Add(soNames);
   }
   
   // check to see if there is at least one propagator
   if (blankProps == MAX_PROP_ROW)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_,
          "Propagate command must have at least one propagator.\n");
      canClose = false;
   }
   
   // check to see if there is any missing propagators for space objects
   if (emptyProps.GetCount() > 0)
   {
      for (UnsignedInt i=0; i<emptyProps.GetCount(); i++)
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "Please select a Propagator for "
             "Spacecraft(s) \"%s\"\n", emptyProps[i].c_str());
      
      canClose = false;
   }
   
   // check to see if there is any missing space objects to propagate
   if (emptySos.GetCount() > 0)
   {
      for (UnsignedInt i=0; i<emptySos.GetCount(); i++)
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "Please select Spacecraft(s) for "
             "Propagator \"%s\"\n", emptySos[i].c_str());
      
      canClose = false;
   }
   
   //-----------------------------------------------------------------
   // check input values: Number, Variable, Array element, Parameter
   //-----------------------------------------------------------------
   Real stopTol;
   if (mStopTolChanged)
   {
      CheckReal(stopTol, mStopTolTextCtrl->GetValue().c_str(), "StopTolerance",
                "Real Number > 0", false, true);
   }
   
   if (mStopCondChanged)
   {
      for (Integer i=0; i<MAX_STOPCOND_ROW; i++)
      {
         if ((stopCondGrid->GetCellValue(i, STOPCOND_PARAM_COL) != "") ||
              (stopCondGrid->GetCellValue(i, STOPCOND_COND_COL) != ""))
         {
            CheckVariable(stopCondGrid->GetCellValue
                          (i, STOPCOND_PARAM_COL).c_str(), Gmat::SPACECRAFT, "Parameter",
                          "Variable, Array element, plottable Parameter", true, true);
            
            // check condition if parameter is not Periapsis nor Apoapsis
            wxString paramName = stopCondGrid->GetCellValue(i, STOPCOND_PARAM_COL);
            if (!paramName.Contains(".Periapsis") && !paramName.Contains(".Apoapsis"))
            {
               CheckVariable(stopCondGrid->GetCellValue
                             (i, STOPCOND_COND_COL).c_str(), Gmat::SPACECRAFT, "Condition",
                             "Variable, Array element, plottable Parameter", true, true);
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
      //-------------------------------------------------------
      // Save propagation mode
      //-------------------------------------------------------
      if (mPropModeChanged)
      {
         #ifdef DEBUG_PROPAGATE_PANEL_SAVE
         MessageInterface::ShowMessage
            ("PropagatePanel::SaveData() Save propagation mode \n");
         #endif
         
         mPropModeChanged = false;
         wxString str = mPropModeComboBox->GetStringSelection();
         if (str.CmpNoCase("None") == 0)
            str = "";
         thePropCmd->SetStringParameter
            (thePropCmd->GetParameterID("PropagateMode"), str.c_str());
      }
      
      //---------------------------------------
      // Save propagator and spacecraft
      //---------------------------------------
      if (mPropSatChanged)
      {         
         #ifdef DEBUG_PROPAGATE_PANEL_SAVE
         MessageInterface::ShowMessage
            ("PropagatePanel::SaveData() Save propagator and spacecraft \n");
         #endif
         
         mPropSatChanged = false;
         Integer propId = thePropCmd->GetParameterID("Propagator");
         Integer scId = thePropCmd->GetParameterID("Spacecraft");
         
         // Clear propagator and spacecraft list
         thePropCmd->TakeAction("Clear", "Propagator");
         
         Integer soCount = 0;
         mPropCount = 0;
         for (Integer i=0; i<MAX_PROP_ROW; i++)
         {
            #ifdef DEBUG_PROPAGATE_PANEL_SAVE
            MessageInterface::ShowMessage
               ("PropagatePanel::SaveData() propagator name[%d]=%s\n",
                i,propGrid->GetCellValue(i, PROP_NAME_COL).c_str());
            MessageInterface::ShowMessage
               ("PropagatePanel::SaveData() spacecraft name[%d]=%s\n",
                i,propGrid->GetCellValue(i, PROP_SOS_COL).c_str());
            #endif
            
            
            if ((propGrid->GetCellValue(i, PROP_NAME_COL) != "") || 
                 (propGrid->GetCellValue(i, PROP_SOS_COL) != ""))
            {
               mTempProp[mPropCount].propName = 
                  propGrid->GetCellValue(i, PROP_NAME_COL).c_str(); 
            
               #ifdef DEBUG_PROPAGATE_PANEL_SAVE
               MessageInterface::ShowMessage
                  ("PropagatePanel::SaveData() propName[%d]=%s\n",
                   mPropCount,mTempProp[mPropCount].propName.c_str());
               #endif
               
               // saving propagator
               thePropCmd->SetStringParameter 
                  (propId, mTempProp[mPropCount].propName.c_str());
               
               // saving spacecraft
               std::string spacecraftStr = 
                  propGrid->GetCellValue(i, PROP_SOS_COL).c_str();
               StringArray parts = 
                  GmatStringUtil::SeparateBy(spacecraftStr, ",");
               
               soCount = parts.size();                    
               for (Integer j=0; j<soCount; j++)
               {
                  #ifdef DEBUG_PROPAGATE_PANEL_SAVE
                  MessageInterface::ShowMessage
                     ("parts[%d] = %s\n", j, parts[j].c_str());
                  #endif
                  
                  thePropCmd->SetStringParameter
                     (scId, std::string(parts[j].c_str()), mPropCount);
               }
               ++mPropCount;
            } // if
         } // for MAX_PROP_ROW
      } // if (mPropSatChanged)
      
      
      //---------------------------------------
      // Save the prop direction
      //---------------------------------------
      if (mPropDirChanged)
      {
         mPropDirChanged = false;
         thePropCmd->SetBooleanParameter("PropForward",
                                         !(backPropCheckBox->IsChecked()));
      }
      
      //---------------------------------------
      // Save stop tolerance
      //---------------------------------------
      if (mStopTolChanged)
      {
         mStopTolChanged = false;
         thePropCmd->SetRealParameter("StopTolerance", stopTol);
      }
      
      //---------------------------------------
      // Save stopping condition
      //---------------------------------------
      
      #ifdef DEBUG_PROPAGATE_PANEL_SAVE
      MessageInterface::ShowMessage
         ("PropagatePanel::SaveData() mPropCount=%d\n", mPropCount);
      #endif
      
      if (mStopCondChanged)
      {
         #ifdef DEBUG_PROPAGATE_PANEL_SAVE
         MessageInterface::ShowMessage
            ("PropagatePanel::SaveData() Save stopping conditions\n");
         #endif
         
         mStopCondChanged = false;
         thePropCmd->TakeAction("Clear", "StopCondition");
         
         mStopCondCount = 0;
         // count number of stopping conditions
         for (Integer i=0; i<MAX_STOPCOND_ROW; i++)
         {
            if (stopCondGrid->GetCellValue(i, STOPCOND_PARAM_COL) != "")
            {
               UpdateStopCondition(i);
               
               #ifdef DEBUG_PROPAGATE_PANEL_SAVE
               MessageInterface::ShowMessage
                  ("PropagatePanel::SaveData() name = %s\n varName = %s\n goal = %s\n"
                   " tol = %f\n repeat = %d\n",
                   mTempStopCond[i].name.c_str(),
                   mTempStopCond[i].varName.c_str(),
                   mTempStopCond[i].goalStr.c_str(),
                   mTempStopCond[i].tol,
                   mTempStopCond[i].repeat);
              #endif
               mTempStopCond[mStopCondCount].stopCondPtr->
                  SetName(std::string(mTempStopCond[i].name.c_str()));
               
               mTempStopCond[mStopCondCount].stopCondPtr->
                  SetStringParameter("StopVar", mTempStopCond[i].varName.c_str());
               
               mTempStopCond[mStopCondCount].stopCondPtr->
                  SetStringParameter("Goal", mTempStopCond[i].goalStr.c_str());
               
               thePropCmd->
                  SetRefObject(mTempStopCond[mStopCondCount].stopCondPtr, 
                               Gmat::STOP_CONDITION,"", mStopCondCount);
               mStopCondCount++;
               #ifdef DEBUG_PROPAGATE_PANEL_SAVE
               MessageInterface::ShowMessage
                  ("PropagatePanel::SaveData() mStopCondCount=%d\n", 
                   mStopCondCount);
               #endif
            } // STOPCOND_PARAM_COL != ""
         } // for MAX_STOPCOND_ROW
      } // if (mStopCondChanged)
   } // try
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
}
