//$Header$
//------------------------------------------------------------------------------
//                              PropagateCommandPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/08/29
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
// Modified: 2004/05/06 by Allison Greene to inherit from GmatPanel
/**
 * This class contains the Propagate Command Setup window.
 */
//------------------------------------------------------------------------------

// gui includes
#include <wx/variant.h>

#include "PropagateCommandPanel.hpp"
#include "ParameterSelectDialog.hpp"
#include "SpacecraftSelectDialog.hpp"
#include "MessageInterface.hpp"

#define DEBUG_PROPCMD_PANEL 0

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(PropagateCommandPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
    
   EVT_TEXT(ID_TEXTCTRL, PropagateCommandPanel::OnTextChange)
   EVT_BUTTON(ID_BUTTON, PropagateCommandPanel::OnButton)
   EVT_COMBOBOX(ID_COMBO, PropagateCommandPanel::OnComboSelection)
   EVT_GRID_CELL_LEFT_CLICK(PropagateCommandPanel::OnCellLeftClick)
   EVT_GRID_CELL_RIGHT_CLICK(PropagateCommandPanel::OnCellRightClick)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// PropagateCommandPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
PropagateCommandPanel::PropagateCommandPanel( wxWindow *parent, const wxString &propName,
                                              GmatCommand *cmd )
   : GmatPanel(parent)
{
#if DEBUG_PROPCMD_PANEL
   MessageInterface::ShowMessage
      ("PropagateCommandPanel::PropagateCommandPanel() entered\n");
#endif
   
   propNameString = propName;
   theCommand = cmd;
   thePropagateCommand = (Propagate *)theCommand;

   // Default values
   mStopCondCount = 0;
   numOfProp = 0;
   numOfModes = 1;
   numOfEqualities = 6;
   mTempPropCount = 0;
   mCurrStopRow = 0;
   
   for (int i=0; i<MAX_PROP_ROW; i++)
   {
      mTempProp[i].isChanged = false;
      mTempProp[i].propName = "";
      mTempProp[i].scNames = "";
      mTempProp[i].scNameList.Clear();
      mTempProp[i].propSetupPtr = NULL;
   }

   for (int i=0; i<MAX_STOPCOND_ROW; i++)
   {
      tempStopCond[i].isChanged = false;
      tempStopCond[i].name = "";
      tempStopCond[i].desc = "";
      tempStopCond[i].varName = "";
      tempStopCond[i].typeName = "";
      tempStopCond[i].relOpStr = "=";
      tempStopCond[i].goal = 0.0;
      tempStopCond[i].tol = 0.0;
      tempStopCond[i].repeat = 1;
      tempStopCond[i].stopCondPtr = NULL;
   }
   
   Create();
   Show();
   updateButton->Disable();
   theApplyButton->Disable();
}

//------------------------------------------------------------------------------
// ~PropagateCommandPanel()
//------------------------------------------------------------------------------
PropagateCommandPanel::~PropagateCommandPanel()
{
}

//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void PropagateCommandPanel::Create()
{
   //MessageInterface::ShowMessage("PropagateCommandPanel::Create() entered\n");
   int bsize = 3; // bordersize
   
   //wxBoxSizer *theMiddleBoxSizer = new wxBoxSizer(wxVERTICAL);
   mMiddleBoxSizer = new wxBoxSizer(wxVERTICAL);
    
   // wxGrid
   propGrid =
      new wxGrid( this, -1, wxDefaultPosition, wxSize(700,100), wxWANTS_CHARS );
   
   propGrid->CreateGrid( MAX_PROP_ROW, 2, wxGrid::wxGridSelectRows );
   propGrid->SetColSize(0, 200);
   propGrid->SetColSize(1, 488);
   propGrid->SetColLabelValue(0, _T("Propagator"));
   propGrid->SetColLabelValue(1, _T("Spacecraft List"));
   propGrid->SetRowLabelSize(0);
   propGrid->EnableEditing(false);
   
   stopCondGrid =
      new wxGrid( this, -1, wxDefaultPosition, wxSize(700,120), wxWANTS_CHARS);
   
   stopCondGrid->CreateGrid( MAX_STOPCOND_ROW, 2, wxGrid::wxGridSelectRows);
   stopCondGrid->SetColSize(0, 200);
   stopCondGrid->SetColSize(1, 488);
   stopCondGrid->SetColLabelValue(0, _T("Name"));
   stopCondGrid->SetColLabelValue(1, _T("Condition"));
   stopCondGrid->SetRowLabelSize(0);
   stopCondGrid->EnableEditing(false);

   // wxString
   wxString strArray1[] = 
   {
      wxT("None")
   };
   
   wxString logicalOpArray[] =
   {
      wxT("="),
      wxT(">"),
      wxT("<"),
      wxT(">="),
      wxT("<="),
      wxT("!=")
   };

   // wxStaticText
   synchStaticText =
      new wxStaticText(this, -1, wxT("Synchronization Mode"),
                        wxDefaultPosition, wxDefaultSize, 0);

   //loj: 7/28/04 changed wxDefaultSize to wxSize(40, -1)
   nameStaticText =
      new wxStaticText(this, -1, wxT("Name"), 
                        wxDefaultPosition, wxSize(40, -1), wxALIGN_RIGHT);
   varStaticText =
      new wxStaticText(this, -1, wxT("Variable"), 
                        wxDefaultPosition, wxSize(40, -1), wxALIGN_RIGHT);
   repeatStaticText =
      new wxStaticText(this, -1, wxT("Repeat"), 
                        wxDefaultPosition, wxSize(40, -1), wxALIGN_RIGHT);
   tolStaticText =
      new wxStaticText(this, -1, wxT("Tolerance"), 
                        wxDefaultPosition, wxDefaultSize, 0);

   // wxTextCtrl
   stopNameTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(250,-1), 0);
   varNameTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(250,-1), 0);
   goalTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(150,-1), 0);
   repeatTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(80,-1), 0);
   toleranceTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(80,-1), 0);

   // wxButton
   //loj: 7/28/04 changed wxDefaultSize to wxSize(60, -1)
   viewButton =
      new wxButton(this, ID_BUTTON, wxT("View"),
                    wxDefaultPosition, wxSize(75, -1), 0);
   updateButton =
      new wxButton(this, ID_BUTTON, wxT("Update"),
                    wxDefaultPosition, wxDefaultSize, 0);
   deleteButton =
      new wxButton(this, ID_BUTTON, wxT("Delete"),
                    wxDefaultPosition, wxDefaultSize, 0);
   
   // wxComboBox
   synchComboBox =
      new wxComboBox(this, ID_COMBO, wxT(strArray1[0]), wxDefaultPosition,
                      wxSize(200,-1), numOfModes, strArray1,
                      wxCB_DROPDOWN|wxCB_READONLY);
   equalityComboBox =
      new wxComboBox(this, ID_COMBO, wxT(logicalOpArray[0]), wxDefaultPosition,
                      wxSize(50,-1), numOfEqualities, logicalOpArray,
                      wxCB_DROPDOWN|wxCB_READONLY);
   
   // wx*Sizer    
   wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
   
   wxStaticBox *propStaticBox =
      new wxStaticBox(this, -1, wxT("Propagators and Spacecraft"));
   wxStaticBox *stopCondStaticBox =
      new wxStaticBox(this, -1, wxT("Stopping Conditions"));
   wxStaticBox *stopDetailStaticBox =
      new wxStaticBox(this, -1, wxT("Stopping Condition Details"));
   
   wxStaticBoxSizer *propSizer =
      new wxStaticBoxSizer(propStaticBox, wxVERTICAL);
   //wxStaticBoxSizer *stopSizer =
   mStopSizer =
      new wxStaticBoxSizer(stopCondStaticBox, wxVERTICAL);
   wxStaticBoxSizer *detailSizer =
      new wxStaticBoxSizer(stopDetailStaticBox, wxVERTICAL);    
   
   wxBoxSizer *item9 = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *stopNameSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *item11 = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *item12 = new wxBoxSizer(wxHORIZONTAL);
   //wxBoxSizer *item13 = new wxBoxSizer(wxHORIZONTAL);

   //loj: 6/25/04 try wxALIGN_CENTER_VERTICAL to wxALIGN_CENTER
   propSizer->Add(synchStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   propSizer->Add(synchComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   propSizer->Add(propGrid, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   stopNameSizer->Add(nameStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   stopNameSizer->Add(stopNameTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   stopNameSizer->Add(75, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   item11->Add(varStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   item11->Add(varNameTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   item11->Add(viewButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   item11->Add(equalityComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   item11->Add(goalTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   item12->Add(repeatStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   item12->Add(repeatTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   item12->Add(tolStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   item12->Add(toleranceTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   item12->Add(updateButton, 0, wxALIGN_RIGHT|wxALL, bsize);
   item12->Add(deleteButton, 0, wxALIGN_RIGHT|wxALL, bsize);
   
   item9->Add(stopNameSizer, 0, wxALIGN_LEFT|wxALL, bsize);    
   item9->Add(item11, 0, wxALIGN_LEFT|wxALL, bsize);
   item9->Add(item12, 0, wxALIGN_LEFT|wxALL, bsize);

   detailSizer->Add(item9, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   mStopSizer->Add(stopCondGrid, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   mStopSizer->Add(detailSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   mMiddleBoxSizer->Add(propSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   mMiddleBoxSizer->Add(mStopSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   panelSizer->Add(mMiddleBoxSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   theMiddleSizer->Add(panelSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   // waw: Future Implementation
   synchComboBox->Enable(false);
   deleteButton->Enable(true);
   equalityComboBox->Enable(false);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void PropagateCommandPanel::LoadData()
{
#if DEBUG_PROPCMD_PANEL
   MessageInterface::ShowMessage("PropagateCommandPanel::LoadData() entered\n");
#endif
   
   //----------------------------------
   // propagator
   //----------------------------------
   propID = thePropagateCommand->GetParameterID("Propagator");
   propSetupName = thePropagateCommand->GetStringParameter(propID);

    //loj: 3/2/04 only 1 PropSetup for build2
   numOfProp = 1;  // waw: TBD
   mTempPropCount = numOfProp;
   
   //loj: when multiple spacecraft is handled, move the block inside the for loop
   scID = thePropagateCommand->GetParameterID("Spacecraft");
   scList = thePropagateCommand->GetStringArrayParameter(scID);
   numOfSC = scList.size();

#if DEBUG_PROPCMD_PANEL
   MessageInterface::ShowMessage
      ("PropagateCommandPanel::LoadData() numOfSC=%d\n",
       numOfSC);
#endif

   for (int i=0; i<numOfProp; i++)
   {
      mTempProp[i].propName = wxT(propSetupName.c_str());
      mTempProp[i].propSetupPtr = theGuiInterpreter->GetPropSetup(propSetupName);
      
      for (int j=0; j<numOfSC; j++)
      {
         mTempProp[i].scNameList.Add(scList[j].c_str());
#if DEBUG_PROPCMD_PANEL
         MessageInterface::ShowMessage
            ("PropagateCommandPanel::LoadData() scNameList[%d]=%s\n",
             j, mTempProp[i].scNameList[j].c_str());
#endif
      }
      
      if (numOfSC > 0)
      {
         for (int j=0; j<numOfSC-1; j++)
         {
            mTempProp[i].scNames += scList[j].c_str();
            mTempProp[i].scNames += ", ";
         }

         mTempProp[i].scNames += scList[numOfSC-1].c_str();
      }
   }

//     //----------------------------------
//     // single stopping condition
//     //----------------------------------
//     theStopCondBase = thePropagateCommand->GetObject(Gmat::STOP_CONDITION);
//     theStopCond = (StopCondition *)theStopCondBase;
//     mStopCondCount = 1;  // temp value

   //----------------------------------
   // stopping conditions
   //----------------------------------
   ObjectArray &stopArray =
      thePropagateCommand->GetRefObjectArray(Gmat::STOP_CONDITION);
   mStopCondCount = stopArray.size();
   
   stopCondGrid->SelectRow(0);
   Parameter *stopParam;

#if DEBUG_PROPCMD_PANEL
   MessageInterface::ShowMessage
      ("PropagateCommandPanel::LoadData() mStopCondCount=%d\n",
       mStopCondCount);
#endif

   for (int i=0; i<mStopCondCount; i++)
   {
      theStopCond = (StopCondition *)stopArray[i];
      
#if DEBUG_PROPCMD_PANEL
      MessageInterface::ShowMessage
         ("PropagateCommandPanel::LoadData() stopArray[%d]=%s\n",
          i, theStopCond->GetName().c_str());
#endif

      // StopCondition created from the script might not have been configured (unnamed)
      if (theStopCond != NULL)
      {
         tempStopCond[i].stopCondPtr = theStopCond;
         tempStopCond[i].name = wxT(theStopCond->GetName().c_str());
         stopParam = theStopCond->GetStopParameter();
         tempStopCond[i].varName = wxT(stopParam->GetName().c_str());
         tempStopCond[i].typeName = wxT(theStopCond->GetTypeName().c_str());
         tempStopCond[i].goal = theStopCond->GetRealParameter("Goal");
         tempStopCond[i].tol = theStopCond->GetRealParameter("Tol");
         tempStopCond[i].repeat = theStopCond->GetIntegerParameter("Repeat");
         wxString str = FormatStopCondDesc(tempStopCond[i].varName,
                                           tempStopCond[i].relOpStr,
                                           tempStopCond[i].goal);
         tempStopCond[i].desc = str;
      }
   }

   mCurrStopRow = 0;
   DisplayPropagator();
   DisplayStopCondition(mCurrStopRow);
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void PropagateCommandPanel::SaveData()
{
#if DEBUG_PROPCMD_PANEL
   MessageInterface::ShowMessage
      ("PropagateCommandPanel::SaveData() mTempPropCount=%d\n", mTempPropCount);
#endif
   
   if (mTempPropCount > 1)
   {
      
      //for (int i=1; i<numOfProp; i++)
      for (int i=1; i<mTempPropCount; i++)
      {
         //loj: 7/26/04 check for PropSetup name first
         if (mTempProp[0].propName != mTempProp[i].propName)
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_, "Synchronized mode has not been implemented. "
                "The first propagator in the list will be used");
            numOfProp = 1;
            break;
         }
      }
   }
   
   //-------------------------------------------------------
   // Saving propagator
   //-------------------------------------------------------
   for (int i=0; i<numOfProp; i++)
   {
      if (mTempProp[i].isChanged)
      {
         // Propagate::SetObject(PROP_SETUP) just sets propName
         thePropagateCommand->SetObject(std::string(mTempProp[i].propName.c_str()),
                                        Gmat::PROP_SETUP);
         
         //---------------------------------------
         // Saving spacecraft
         //---------------------------------------
         numOfSC = mTempProp[i].scNameList.Count();
         
         //if (numOfSC > 1)
         //{
         //   MessageInterface::PopupMessage
         //      (Gmat::WARNING_, "Multiple spacecraft has not been implemented. "
         //       "The first spacecraft in the list will be used");
         //   numOfSC = 1;
         //}
         
#if DEBUG_PROPCMD_PANEL
         MessageInterface::ShowMessage
            ("PropagateCommandPanel::SaveData() numOfSC=%d\n", numOfSC);
#endif
         // Propagate::SetObject(SPACECRAFT) adds to the list, so clear first
         thePropagateCommand->ClearObject(Gmat::SPACECRAFT);
         
         for (int j=0; j<numOfSC; j++)
         {
            thePropagateCommand->
               SetObject(std::string(mTempProp[i].scNameList[j].c_str()),
                         Gmat::SPACECRAFT);
#if DEBUG_PROPCMD_PANEL
            MessageInterface::ShowMessage
               ("PropagateCommandPanel::SaveData() scNameList[%d]=%s\n",
                j, mTempProp[i].scNameList[j].c_str());
#endif
         }
      }
   }
   
   //loj: 6/15/04 added
   if (updateButton->IsEnabled())
      UpdateStopCondition();
   
   //-------------------------------------------------------
   // Saving stopping condition
   //-------------------------------------------------------

   int stopCount = 0;
   for (int i=0; i<MAX_STOPCOND_ROW; i++)
   {
      if (tempStopCond[i].desc != "")
         stopCount++;
   }
   
#if DEBUG_PROPCMD_PANEL
   MessageInterface::ShowMessage
      ("PropagateCommandPanel::SaveData() stopCount=%d\n", stopCount);
#endif

   //loj: 6/28/04 Do we need to show message?
//     if (stopCount == 0)
//        MessageInterface::PopupMessage
//           (Gmat::ERROR_, "There is no Stopping condition specified for "
//            "Propagate command. Please add one or more stopping conditions\n");
   
   //---------------------------------------------
   // if any stoppping condition removed,
   // clear Propagate StopCondition array and
   // set flag to add as new
   //---------------------------------------------
   if (stopCount < mStopCondCount)
   {
      thePropagateCommand->ClearObject(Gmat::STOP_CONDITION);
      for (int i=0; i<stopCount; i++)
         tempStopCond[i].isChanged = true;
   }
   
   mStopCondCount = stopCount;

   //---------------------------------------------
   // Set stopCondition.
   //---------------------------------------------
   for (int i=0; i<mStopCondCount; i++)
   {
      if (tempStopCond[i].isChanged)
      {
#if DEBUG_PROPCMD_PANEL
         MessageInterface::ShowMessage
            ("PropagateCommandPanel::SaveData() tempStopCond[%d].isChanged=%d\n", i,
             tempStopCond[i].isChanged);
#endif
         if (tempStopCond[i].stopCondPtr != NULL)
         {
            tempStopCond[i].stopCondPtr->
               SetName(std::string(tempStopCond[i].name.c_str()));
                
            tempStopCond[i].stopCondPtr->
               SetStringParameter("StopVar", tempStopCond[i].varName.c_str());
                
            tempStopCond[i].stopCondPtr->
               SetRealParameter("Goal", tempStopCond[i].goal);
                
            tempStopCond[i].stopCondPtr->
               SetRealParameter("Tol", tempStopCond[i].tol);
                
            tempStopCond[i].stopCondPtr->
               SetIntegerParameter("Repeat", tempStopCond[i].repeat);

            //loj: 6/25/04 use SetRefObject()
            thePropagateCommand->
               SetRefObject(tempStopCond[i].stopCondPtr, Gmat::STOP_CONDITION,
                            "", i);

#if DEBUG_PROPCMD_PANEL
            StopCondition *stopCond = (StopCondition*)thePropagateCommand->
               GetRefObject(Gmat::STOP_CONDITION, tempStopCond[i].stopCondPtr->GetName(),
                            i);
            MessageInterface::ShowMessage
               ("PropagateCommandPanel::SaveData() name=%s, stopVarTYpe=%s, goal = %f\n",
                stopCond->GetName().c_str(),
                stopCond->GetStringParameter("StopVar").c_str(),
                stopCond->GetRealParameter("Goal"));
#endif
         }
      }
   }
}

//------------------------------------------------------------------------------
// void DisplayPropagator()
//------------------------------------------------------------------------------
void PropagateCommandPanel::DisplayPropagator()
{
#if DEBUG_PROPCMD_PANEL
   MessageInterface::ShowMessage
      ("PropagateCommandPanel::DisplayPropagator() entered\n");
#endif

   //loj: 3/2/04 added
   for (int i=0; i<mTempPropCount; i++)
   {
      propGrid->SetCellValue(i, PROP_NAME_COL, mTempProp[i].propName);
      propGrid->SetCellValue(i, PROP_SCS_COL, mTempProp[i].scNames);
   }

   propGrid->SelectRow(0);    
}

//------------------------------------------------------------------------------
// void DisplayStopCondition(int selRow)
//------------------------------------------------------------------------------
void PropagateCommandPanel::DisplayStopCondition(int selRow)
{
#if DEBUG_PROPCMD_PANEL
   MessageInterface::ShowMessage
      ("PropagateCommandPanel::DisplayStopCondition() selRow=%d\n", selRow);
#endif
   
   //----- for stopCondGrid - show all
   //for (int i=0; i<mStopCondCount; i++) //loj: 6/28/04
   for (int i=0; i<MAX_STOPCOND_ROW; i++)
   {
      stopCondGrid->SetCellValue(i, STOPCOND_NAME_COL, tempStopCond[i].name);
      stopCondGrid->SetCellValue(i, STOPCOND_DESC_COL, tempStopCond[i].desc);        
   }

   //----- for detailed stop condition - show first row
   stopNameTextCtrl->SetValue(tempStopCond[selRow].name); //loj: 6/16/04 changed from AppendText
   varNameTextCtrl->SetValue(tempStopCond[selRow].varName);
   equalityComboBox->SetSelection(equalityComboBox->
                                  FindString(tempStopCond[selRow].relOpStr));
   repeatTextCtrl->SetValue(wxVariant((long)(tempStopCond[selRow].repeat)));
   goalTextCtrl->SetValue(wxVariant(tempStopCond[selRow].goal));
   toleranceTextCtrl->SetValue(wxVariant(tempStopCond[selRow].tol));
    
   stopCondGrid->SelectRow(selRow);
   varNameTextCtrl->Disable();
}

//------------------------------------------------------------------------------
// void UpdateStopCondition()
//------------------------------------------------------------------------------
void PropagateCommandPanel::UpdateStopCondition()
{
   //loj: 3/2/04 added - assume single selection
   //wxArrayInt stopRows = stopCondGrid->GetSelectedRows();
   //int stopRow = stopRows[0];

   int stopRow = mCurrStopRow;
   
#if DEBUG_PROPCMD_PANEL
   MessageInterface::ShowMessage
      ("PropagateCommandPanel::UpdateStopCondition() stopRow=%d\n", stopRow);
#endif
   
   wxString oldStopName = tempStopCond[stopRow].name;
   tempStopCond[stopRow].name = stopNameTextCtrl->GetValue();
   tempStopCond[stopRow].varName = varNameTextCtrl->GetValue();
   tempStopCond[stopRow].goal = atof(goalTextCtrl->GetValue().c_str());
   tempStopCond[stopRow].tol = atof(toleranceTextCtrl->GetValue().c_str());
   tempStopCond[stopRow].repeat = atoi(repeatTextCtrl->GetValue().c_str());
            
   wxString str = FormatStopCondDesc(tempStopCond[stopRow].varName,
                                     tempStopCond[stopRow].relOpStr,
                                     tempStopCond[stopRow].goal);
   tempStopCond[stopRow].desc = str;
   
   //loj: 3/31/04 create StopCondition
   //-----------------------------------------------------------------
   // create StopCondition if new StopCondition
   //-----------------------------------------------------------------
   //loj: 6/28/04 if (!oldStopName.IsSameAs(tempStopCond[stopRow].name))
   if (oldStopName.IsSameAs(""))
   {
      StopCondition *stopCond = theGuiInterpreter->
         CreateStopCondition("StopCondition",
                             std::string(tempStopCond[stopRow].name.c_str()));
      tempStopCond[stopRow].stopCondPtr = stopCond;

      if (stopCond == NULL)
      {
         MessageInterface::ShowMessage
            ("PropagateCommandPanel::UpdateStopCondition() Unable to create "
             "StopCondition: name=%s\n", tempStopCond[stopRow].name.c_str());
      }
   }
   else
   {
      tempStopCond[stopRow].stopCondPtr->
         SetName(std::string(tempStopCond[stopRow].name.c_str()));
   }
   
   stopCondGrid->SetCellValue(stopRow, STOPCOND_NAME_COL,
                              tempStopCond[stopRow].name);
   stopCondGrid->SetCellValue(stopRow, STOPCOND_DESC_COL,
                              tempStopCond[stopRow].desc);
   tempStopCond[stopRow].isChanged = true;
        
   updateButton->Enable(false);
   theApplyButton->Enable(true);
}

//------------------------------------------------------------------------------
// void RemoveStopCondition()
//------------------------------------------------------------------------------
void PropagateCommandPanel::RemoveStopCondition()
{
#if DEBUG_PROPCMD_PANEL
   MessageInterface::ShowMessage
      ("PropagateCommandPanel::RemoveStopCondition() mCurrStopRow=%d\n",
       mCurrStopRow);
#endif

   //---------------------------------------------
   // Delete stopCondition pointer
   //---------------------------------------------

   if (tempStopCond[mCurrStopRow].stopCondPtr != NULL)
   {
      delete tempStopCond[mCurrStopRow].stopCondPtr;
      tempStopCond[mCurrStopRow].stopCondPtr = NULL;
   }
   
   // go through the table and move upward if name is not blank
   for (int i=mCurrStopRow; i<MAX_STOPCOND_ROW; i++)
   {
      if (tempStopCond[i+1].name != "")
      {
         // move upward
         tempStopCond[i].isChanged = tempStopCond[i+1].isChanged;
         tempStopCond[i].name = tempStopCond[i+1].name;
         tempStopCond[i].desc = tempStopCond[i+1].desc;
         tempStopCond[i].varName = tempStopCond[i+1].varName;
         tempStopCond[i].typeName = tempStopCond[i+1].typeName;
         tempStopCond[i].relOpStr = tempStopCond[i+1].relOpStr;
         tempStopCond[i].goal = tempStopCond[i+1].goal;
         tempStopCond[i].tol = tempStopCond[i+1].tol;
         tempStopCond[i].repeat = tempStopCond[i+1].repeat;
         tempStopCond[i].stopCondPtr = tempStopCond[i+1].stopCondPtr;
      }
      else
      {
         ClearStopCondition(i);
         break;
      }
   }

   (mCurrStopRow > 0) ? mCurrStopRow = mCurrStopRow-1 : mCurrStopRow = 0;

   DisplayStopCondition(mCurrStopRow);
}

//------------------------------------------------------------------------------
// void ClearStopCondition(int selRow)
//------------------------------------------------------------------------------
void PropagateCommandPanel::ClearStopCondition(int selRow)
{
#if DEBUG_PROPCMD_PANEL
   MessageInterface::ShowMessage
      ("PropagateCommandPanel::ClearStopCondition() selRow=%d\n", selRow);
#endif
   
   tempStopCond[selRow].isChanged = false;
   tempStopCond[selRow].name = "";
   tempStopCond[selRow].desc = "";
   tempStopCond[selRow].varName = "";
   tempStopCond[selRow].typeName = "";
   tempStopCond[selRow].relOpStr = "=";
   tempStopCond[selRow].goal = 0.0;
   tempStopCond[selRow].tol = 0.0;
   tempStopCond[selRow].repeat = 1;
   tempStopCond[selRow].stopCondPtr = NULL;
}

//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagateCommandPanel::OnTextChange(wxCommandEvent& event)
{
   if (stopNameTextCtrl->IsModified() || varNameTextCtrl->IsModified() ||
       goalTextCtrl->IsModified()     || repeatTextCtrl->IsModified()  ||
       toleranceTextCtrl->IsModified())
   {
      ActivateUpdateButton();
   }
}

//------------------------------------------------------------------------------
// void OnComboSelection(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagateCommandPanel::OnComboSelection(wxCommandEvent& event)
{
   //loj: assume single selection
   wxArrayInt stopRows = stopCondGrid->GetSelectedRows();
   int stopRow = stopRows[0];
    
   if (event.GetEventObject() == synchComboBox )
   {
      theApplyButton->Enable(true);
   }
   else if (event.GetEventObject() == equalityComboBox )
   {
      wxString str = FormatStopCondDesc(tempStopCond[stopRow].varName,
                                        tempStopCond[stopRow].relOpStr,
                                        tempStopCond[stopRow].goal);
      tempStopCond[stopRow].desc = str;
      tempStopCond[stopRow].isChanged = true;
      updateButton->Enable(true);
      theApplyButton->Enable(true);
   }
   else
      event.Skip();
}

//------------------------------------------------------------------------------
// void OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagateCommandPanel::OnButton(wxCommandEvent& event)
{
   if (event.GetEventObject() == viewButton )
   {
      // show dialog to select parameter
      ParameterSelectDialog paramDlg(this);
      paramDlg.ShowModal();

      if (paramDlg.IsParamSelected())
      {
         newParamName = paramDlg.GetParamName();
         varNameTextCtrl->SetValue(newParamName);

         //loj: 6/15/04
         // if stopping condition parameter changed, give it a new name
         stopNameTextCtrl->SetValue("StopOn" + newParamName);

         // if Apoapsis or Periapsis, disable goal
         if (newParamName.Contains(".Periapsis") ||
             newParamName.Contains(".Apoapsis"))
         {
            goalTextCtrl->Disable();
            //equalityComboBox->Disable();
         }
         else
         {
            goalTextCtrl->Enable();
            //equalityComboBox->Enable();
         }
      
         updateButton->Enable(true);
         theApplyButton->Enable(true);
      }
   }
   else if (event.GetEventObject() == updateButton )
   {
      UpdateStopCondition();
      theApplyButton->Enable(true);
   }
   else if (event.GetEventObject() == deleteButton )
   {
      RemoveStopCondition();
      theApplyButton->Enable(true);
   }
   else
      event.Skip();
}

//------------------------------------------------------------------------------
// void OnCellLeftClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void PropagateCommandPanel::OnCellLeftClick(wxGridEvent& event)
{
   int row = event.GetRow();
    
   if (event.GetEventObject() == propGrid)
   {
      propGrid->SelectRow(row);
   }
   else if (event.GetEventObject() == stopCondGrid)
   {
      stopCondGrid->SelectRow(row);
      
      if (updateButton->IsEnabled())
         UpdateStopCondition();

      mCurrStopRow = row;
      ShowDetailedStopCond(row);
      updateButton->Disable();
   }
}

//------------------------------------------------------------------------------
// void OnCellRightClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void PropagateCommandPanel::OnCellRightClick(wxGridEvent& event)
{    
   int row = event.GetRow();
   int col = event.GetCol();

   if (row <= mTempPropCount)
   {
      if (event.GetEventObject() == propGrid)
      {
         propGrid->SelectRow(row);
         if (col == 0)
         {
            //----------------------------------
            // select propagator
            //----------------------------------
            StringArray &list =
               theGuiInterpreter->GetListOfConfiguredItems(Gmat::PROP_SETUP);
            int size = list.size();
            wxString *choices = new wxString[size];
        
            for (int i=0; i<size; i++)
            {
               choices[i] = list[i].c_str();
            }
        
            wxSingleChoiceDialog dialog(this, _T("Available Propagator: \n"),
                                        _T("PropagtorSelectDialog"), size, choices);
            dialog.SetSelection(0);

            if (dialog.ShowModal() == wxID_OK)
            {
               if (dialog.GetStringSelection() != propGrid->GetCellValue(row, col))
               {
                  if (mTempPropCount <= row)
                     mTempPropCount = row + 1;
                  
#if DEBUG_PROPCMD_PANEL
                  MessageInterface::ShowMessage
                     ("PropagateCommandPanel::OnCellRightClick() mTempPropCount=%d\n",
                      mTempPropCount);
#endif
                  propGrid->SetCellValue(row, col, dialog.GetStringSelection());
                  mTempProp[row].isChanged = true;
                  mTempProp[row].propName = dialog.GetStringSelection();
                  theApplyButton->Enable(true);
               }
            }
         }
         else if (col == 1)
         {
            //----------------------------------
            // select spacecraft
            //----------------------------------
            wxArrayString scExcList;
            for (int i=0; i<mTempPropCount; i++)
            {
               numOfSC = mTempProp[i].scNameList.Count();
            
               for (int j=0; j<numOfSC; j++)
                  scExcList.Add(mTempProp[i].scNameList[j]);
            }
         
            SpacecraftSelectDialog scDlg(this, mTempProp[row].scNameList, scExcList);
            scDlg.ShowModal();
         
            if (scDlg.IsSpacecraftSelected())
            {
               wxArrayString &names = scDlg.GetSpacecraftNames();
               mTempProp[row].isChanged = true;
               mTempProp[row].scNames = "";
               numOfSC = names.GetCount();
            
#if DEBUG_PROPCMD_PANEL
               MessageInterface::ShowMessage
                  ("PropagateCommandPanel::OnCellRightClick() numOfSC=%d\n", numOfSC);
#endif
            
               mTempProp[row].scNameList.Clear();
               for (int j=0; j<numOfSC; j++)
               {
                  mTempProp[row].scNameList.Add(names[j]);
#if DEBUG_PROPCMD_PANEL
                  MessageInterface::ShowMessage
                     ("PropagateCommandPanel::OnCellRightClick() scNameList[%d]=%s\n",
                      j, mTempProp[row].scNameList[j].c_str());
#endif
               }
            
               for(Integer j=0; j < numOfSC-1; j++)
               {
                  mTempProp[row].scNames += names[j].c_str();
                  mTempProp[row].scNames += ", ";
               }

               mTempProp[row].scNames += names[numOfSC-1].c_str();
            
               propGrid->SetCellValue(row, col, mTempProp[row].scNames);
               
               DisplayStopCondition(0);
               theApplyButton->Enable(true);
            }
         }
      }
   }
}

//------------------------------------------------------------------------------
// void ShowDetailedStopCond(int stopRow)
//------------------------------------------------------------------------------
void PropagateCommandPanel::ShowDetailedStopCond(int stopRow)
{
   stopNameTextCtrl->SetValue(tempStopCond[stopRow].name);
   varNameTextCtrl->SetValue(tempStopCond[stopRow].varName);
   equalityComboBox->SetSelection(equalityComboBox->
                                  FindString(tempStopCond[stopRow].relOpStr));
   repeatTextCtrl->SetValue(wxVariant((long)(tempStopCond[stopRow].repeat)));
   goalTextCtrl->SetValue(wxVariant(tempStopCond[stopRow].goal));
   toleranceTextCtrl->SetValue(wxVariant(tempStopCond[stopRow].tol));
   
   // if param name containes Apoapsis or Periapsis, disable goal
   if (tempStopCond[stopRow].varName.Contains(".Periapsis") ||
       tempStopCond[stopRow].varName.Contains(".Apoapsis"))
   {
      goalTextCtrl->Disable();
      //equalityComboBox->Disable();
   }
   else
   {
      goalTextCtrl->Enable();
      //equalityComboBox->Enable();
   }      
}

//------------------------------------------------------------------------------
// void ActivateUpdateButton()
//------------------------------------------------------------------------------
void PropagateCommandPanel::ActivateUpdateButton()
{
   //loj: assume single selection
   wxArrayInt stopRows = stopCondGrid->GetSelectedRows();
   int stopRow = stopRows[0];

   tempStopCond[stopRow].isChanged = true;
   updateButton->Enable(true);
   theApplyButton->Enable(true); //loj: 6/15/04 uncommented this line
}

//------------------------------------------------------------------------------
// wxString FormatStopCondDesc(const wxString &varName, const wxString &relOpStr,
//                             Real &goal)
//------------------------------------------------------------------------------
wxString PropagateCommandPanel::FormatStopCondDesc(const wxString &varName,
                                                   const wxString &relOpStr,
                                                   Real &goal)
{
   wxString goalStr;
   wxString desc;
   wxString opStr = relOpStr;
   
   goalStr.Printf("%.9f", goal);

   if (varName.Contains("Apoapsis") || varName.Contains("Periapsis"))
   {
      opStr = "";
      goalStr = "";
   }
   
   desc = varName + " " + opStr + " " + goalStr;
    
   return desc;
}

