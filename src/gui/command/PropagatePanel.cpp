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
#include "StringUtil.hpp"    // for SeparateBy()
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
   EVT_GRID_CELL_RIGHT_CLICK(PropagatePanel::OnCellRightClick)
   EVT_GRID_CELL_CHANGE(PropagatePanel::OnCellValueChange)
   EVT_CHECKBOX(ID_CHECKBOX, PropagatePanel::OnCheckBoxChange)
   EVT_COMBOBOX(ID_COMBOBOX, PropagatePanel::OnComboBoxChange)
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
   MessageInterface::ShowMessage ("PropagatePanel::PropagatePanel() entered\n");
   #endif
   
   thePropCmd = (Propagate *)cmd;

   // Default values
   mPropModeChanged = false;
   mPropChanged     = false;
   mStopCondChanged = false;

   canClose = true;

   mPropModeCount = 1;
   mPropCount     = 0;
   mStopCondCount = 0;
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
//   mRemovedStopCondList.clear();
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
// void DisplayPropagator()
//------------------------------------------------------------------------------
void PropagatePanel::DisplayPropagator()
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::DisplayPropagator() entered\n");
   #endif

   wxString name;
   for (Integer i=0; i<mPropCount; i++)
   {
      propGrid->SetCellValue( i, PROP_NAME_COL, mTempProp[i].propName );
      propGrid->SetCellValue( i, PROP_SOS_COL, mTempProp[i].soNames );
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
//   for (Integer i=0; i<MAX_STOPCOND_ROW; i++)
   for (Integer i=0; i<mStopCondCount; i++)
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
//           SetCellValue( i, STOPCOND_RELOPER_COL, mTempStopCond[i].relOpStr );        
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
void PropagatePanel::UpdateStopCondition(Integer stopRow)
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::UpdateStopCondition() entered\n");
   MessageInterface::ShowMessage
      ("PropagatePanel::UpdateStopCondition() stopRow = %d\n", stopRow);
   #endif
   
   wxString oldStopName = mTempStopCond[stopRow].name;
   mTempStopCond[stopRow].name = "StopOn" + 
      stopCondGrid->GetCellValue( stopRow, STOPCOND_PARAM_COL );
   mTempStopCond[stopRow].varName = 
      stopCondGrid->GetCellValue( stopRow, STOPCOND_PARAM_COL );
   mTempStopCond[stopRow].relOpStr = 
      stopCondGrid->GetCellValue( stopRow, STOPCOND_RELOPER_COL );

   // if Apoapsis or Periapsis, disable goal
   if ( mTempStopCond[stopRow].varName.Contains(".Periapsis") ||
        mTempStopCond[stopRow].varName.Contains(".Apoapsis") )
   {
      mTempStopCond[stopRow].goalStr = "0.0";
   }
   else
   {
      mTempStopCond[stopRow].goalStr = 
         stopCondGrid->GetCellValue( stopRow, STOPCOND_COND_COL );
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
         ( "StopCondition", mTempStopCond[stopRow].name.c_str() );
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
         SetName( std::string( mTempStopCond[stopRow].name.c_str() ) );
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
   #endif

   // Propagate grid
   if (event.GetEventObject() == propGrid)
   {
      if (col == 0)
      {
         //----------------------------------
         // select propagator
         //----------------------------------
//            PropagatorSelectDialog propDlg(this, mTempProp[row].propName);
         PropagatorSelectDialog propDlg(this, "");
         propDlg.ShowModal();
         
         if (propDlg.HasSelectionChanged())
         {
            wxString newPropName = propDlg.GetPropagatorName();
            #if DEBUG_PROPAGATE_PANEL
            MessageInterface::ShowMessage
               ("PropagatePanel::OnCellRightClick() newPropName = %s\n",
                newPropName.c_str());
            #endif
            
            propGrid->SetCellValue(row, col, newPropName);
           
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
         } // soDlg.IsSpaceObjectSelected()
      } // else if (col == 1)
   } // end of if (event.GetEventObject() == propGrid)

   // Stopping Condition grid
   else if (event.GetEventObject() == stopCondGrid)
   {
      // show dialog to select parameter
      ParameterSelectDialog paramDlg(this, mObjectTypeList);
      paramDlg.ShowModal();

      if (col == 0)
      {
         if (paramDlg.IsParamSelected())
         {
            wxString newParamName = paramDlg.GetParamName();
            stopCondGrid->SetCellValue( row,STOPCOND_PARAM_COL,newParamName );
               
            // if Apoapsis or Periapsis, disable goal
            if (newParamName.Contains(".Periapsis") ||
                newParamName.Contains(".Apoapsis"))
            {
               stopCondGrid->SetCellValue( row, STOPCOND_RELOPER_COL, "" );
               stopCondGrid->SetCellValue( row, STOPCOND_COND_COL, "" );
               stopCondGrid->SetReadOnly( row, STOPCOND_COND_COL, true );
            }
            else
            {
               stopCondGrid->SetCellValue( row, STOPCOND_RELOPER_COL, "=" );
               stopCondGrid->SetCellValue( row, STOPCOND_COND_COL, "0.0" );
               stopCondGrid->SetReadOnly( row, STOPCOND_COND_COL, false );
            }
         }
      }
      else if (col == 2)
      {
         if (paramDlg.IsParamSelected())
         {
            wxString newParamName = paramDlg.GetParamName();
            stopCondGrid->SetCellValue( row, STOPCOND_COND_COL, newParamName );
         }
      }
      
      UpdateStopCondition(row);
      EnableUpdate(true);
   } // event.GetEventObject() == stopCondGrid
}

//------------------------------------------------------------------------------
// void OnCellValueChange(wxGridEvent& event)
//------------------------------------------------------------------------------
void PropagatePanel::OnCellValueChange(wxGridEvent& event)
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::OnCellValueChange() entered\n");
   #endif
   Integer row = event.GetRow();
   Integer col = event.GetCol();

   if (event.GetEventObject() == stopCondGrid)
   {
      if (col == 0)
      {
         wxString newParamName = stopCondGrid->
            GetCellValue( row, STOPCOND_PARAM_COL );
            
         // if Apoapsis or Periapsis, disable goal
         if ( newParamName.Contains(".Periapsis") ||
              newParamName.Contains(".Apoapsis") )
         {
            stopCondGrid->SetCellValue( row, STOPCOND_RELOPER_COL, "" );
            stopCondGrid->SetCellValue( row, STOPCOND_COND_COL, "" );
            stopCondGrid->SetReadOnly( row, STOPCOND_COND_COL, true );
         }
         else
         {
            stopCondGrid->SetCellValue( row, STOPCOND_RELOPER_COL, "=" );
            if (stopCondGrid->GetCellValue(row, STOPCOND_COND_COL) == "")
               stopCondGrid->SetCellValue( row, STOPCOND_COND_COL, "0.0" );
            stopCondGrid->SetReadOnly( row, STOPCOND_COND_COL, false );
         }
      }
   }
   
   EnableUpdate(true);
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
         if ( theGuiInterpreter->GetConfiguredObject(soList[j]) )
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
      
///@todo Will be remove when base code is changed -> LTR 1/18/07
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
   
   DisplayPropagator();
   DisplayStopCondition();
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void PropagatePanel::SaveData()
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage("PropagatePanel::SaveData() entered\n");
   #endif

   Integer blankProps = 0;
   canClose = true;

   // count number of propagators
   for (Integer i=0; i<MAX_PROP_ROW; i++)
   {
      if (propGrid->GetCellValue(i, PROP_NAME_COL) == "")
      	++blankProps;
   }
   // check to see if there is at least one propagator
   if (blankProps == MAX_PROP_ROW)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, 
         "Propagate command must have at least one propagator.\n");
      canClose = false;
      return;
   }

   //-----------------------------------------------------------------
   // check input values: Number, Variable, Array element, Parameter
   //-----------------------------------------------------------------
   for (Integer i=0; i<MAX_STOPCOND_ROW; i++)
   {
      if ( (stopCondGrid->GetCellValue(i, STOPCOND_PARAM_COL) != "") ||
           (stopCondGrid->GetCellValue(i, STOPCOND_COND_COL) != "") )
      {
         CheckVariable( stopCondGrid->GetCellValue
            (i, STOPCOND_PARAM_COL).c_str(), Gmat::SPACECRAFT, "Parameter",
            "Variable, Array element, plottable Parameter", true);
         CheckVariable( stopCondGrid->GetCellValue
            (i, STOPCOND_COND_COL).c_str(), Gmat::SPACECRAFT, "Condition",
            "Variable, Array element, plottable Parameter", true);
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
      // Saving propagation mode
      //-------------------------------------------------------
      if (mPropModeChanged)
      {
         #if DEBUG_PROPAGATE_PANEL
         MessageInterface::ShowMessage
            ("PropagatePanel::SaveData() Saving propagation mode \n");
         #endif
         mPropModeChanged = false;
         wxString str = mPropModeComboBox->GetStringSelection();
         if (str.CmpNoCase("None") == 0)
            str = "";
         thePropCmd->SetStringParameter
            (thePropCmd->GetParameterID("PropagateMode"), str.c_str());
      }
           
//      //-------------------------------------------------------
//      // Saving backward propragation
//      //-------------------------------------------------------
//        /// @todo Need to be implemented in base code for Propagate Command
//        //   Integer backPropId = thePropCmd->GetParameterID("BackProp");
//        //   bool backProp = backPropCheckBox->GetValue();
//        //   thePropCmd->SetBooleanParameter(backPropId, backProp);
//      #if DEBUG_PROPAGATE_PANEL
//      MessageInterface::ShowMessage
//         ("PropagatePanel::SaveData() Saving backward propragation \n");
//      #endif
           
      //---------------------------------------
      // Saving propagator and spacecraft
      //---------------------------------------
      #if DEBUG_PROPAGATE_PANEL
      MessageInterface::ShowMessage
         ("PropagatePanel::SaveData() Saving propagator and spacecraft \n");
      #endif

      Integer propId = thePropCmd->GetParameterID("Propagator");
      Integer scId = thePropCmd->GetParameterID("Spacecraft");
      
      // Clear propagator and spacecraft list
      thePropCmd->TakeAction("Clear", "Propagator");
      
      Integer soCount = 0;
      mPropCount = 0;
	   for (Integer i=0; i<MAX_PROP_ROW; i++)
	   {
         #if DEBUG_PROPAGATE_PANEL
         MessageInterface::ShowMessage
            ("PropagatePanel::SaveData() propagator name[%d]=%s\n",
               i,propGrid->GetCellValue(i, PROP_NAME_COL).c_str());
         MessageInterface::ShowMessage
            ("PropagatePanel::SaveData() spacecraft name[%d]=%s\n",
               i,propGrid->GetCellValue(i, PROP_SOS_COL).c_str());
         #endif

	      if ( (propGrid->GetCellValue(i, PROP_NAME_COL) != "") || 
	           (propGrid->GetCellValue(i, PROP_SOS_COL) != "") )
	      {
            mTempProp[mPropCount].propName = 
               propGrid->GetCellValue(i, PROP_NAME_COL).c_str(); 

	         // backward propragation
	         if (backPropCheckBox->IsChecked())
	         {
	            mTempProp[mPropCount].propName.Prepend("-");  
	         }
	         else
	         {
	            wxString name = mTempProp[mPropCount].propName;
	            name.Replace("-", "", false);
	            mTempProp[mPropCount].propName = name; 
	         }
	         #if DEBUG_PROPAGATE_PANEL
	         MessageInterface::ShowMessage
	            ("PropagatePanel::SaveData() propName[%d]=%s\n",
	               mPropCount,mTempProp[mPropCount].propName.c_str());
	         #endif
	         
            // saving propagator
	         thePropCmd->SetStringParameter 
	            ( propId, mTempProp[mPropCount].propName.c_str() );

            // saving spacecraft
            std::string spacecraftStr = 
               propGrid->GetCellValue(i, PROP_SOS_COL).c_str();
            StringArray parts = 
               GmatStringUtil::SeparateBy(spacecraftStr, ",");

            soCount = parts.size();                    
            for (Integer j=0; j<soCount; j++)
            {
               #if DEBUG_PROPAGATE_PANEL
               MessageInterface::ShowMessage
                  ("parts[%d] = %s\n", j, parts[j].c_str());
               #endif
               thePropCmd->SetStringParameter
                  (scId, std::string(parts[j].c_str()), mPropCount);
            }
	         ++mPropCount;
	      } // if
	   } // for MAX_PROP_ROW

      #if DEBUG_PROPAGATE_PANEL
      MessageInterface::ShowMessage
         ("PropagatePanel::SaveData() mPropCount=%d\n", mPropCount);
      #endif

      //---------------------------------------
      // Saving stopping condition
      //---------------------------------------
      thePropCmd->TakeAction("Clear", "StopCondition");
         
      mStopCondCount = 0;
      // count number of stopping conditions
      for (Integer i=0; i<MAX_STOPCOND_ROW; i++)
      {
//         if ( (stopCondGrid->GetCellValue(i, STOPCOND_PARAM_COL) != "") ||
//              (stopCondGrid->GetCellValue(i, STOPCOND_COND_COL) != "") )
         if (stopCondGrid->GetCellValue(i, STOPCOND_PARAM_COL) != "")
         {
            UpdateStopCondition(i);
               
	         #if DEBUG_PROPAGATE_PANEL
	         MessageInterface::ShowMessage
	            ( "PropagatePanel::SaveData() name = %s\n varName = %s\n goal = %s\n tol = %f\n repeat = %d\n",
	             mTempStopCond[i].name.c_str(),
	             mTempStopCond[i].varName.c_str(),
	             mTempStopCond[i].goalStr.c_str(),
	             mTempStopCond[i].tol,
	             mTempStopCond[i].repeat );
	         #endif
            mTempStopCond[mStopCondCount].stopCondPtr->
               SetName(std::string(mTempStopCond[i].name.c_str()));
            
            mTempStopCond[mStopCondCount].stopCondPtr->
               SetStringParameter("StopVar", mTempStopCond[i].varName.c_str());
            
            mTempStopCond[mStopCondCount].stopCondPtr->
               SetStringParameter("Goal", mTempStopCond[i].goalStr.c_str());
            
            mTempStopCond[mStopCondCount].stopCondPtr->
               SetRealParameter("Tol", mTempStopCond[i].tol);
            
            mTempStopCond[mStopCondCount].stopCondPtr->
               SetIntegerParameter("Repeat", mTempStopCond[i].repeat);
            
            thePropCmd->
               SetRefObject(mTempStopCond[mStopCondCount].stopCondPtr, 
               Gmat::STOP_CONDITION,"", mStopCondCount);
            mStopCondCount++;
	         #if DEBUG_PROPAGATE_PANEL
	         MessageInterface::ShowMessage
	            ("PropagatePanel::SaveData() mStopCondCount=%d\n", 
	            mStopCondCount);
	         #endif
         } // STOPCOND_PARAM_COL != ""
      } // for MAX_STOPCOND_ROW
   } // try
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
}
