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
   //EVT_TEXT_ENTER(ID_TEXTCTRL, PropagateCommandPanel::OnTextChange)
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
   //MessageInterface::ShowMessage("PropagateCommandPanel::PropagateCommandPanel() entered\n");
   propNameString = propName;
   theCommand = cmd;
   thePropagateCommand = (Propagate *)theCommand;

   // Default values
   mStopCondCount = 0;
   numOfProp = 0;
   numOfModes = 1;
   numOfEqualities = 6;
   tempPropCount = 0;
   
   for (int i=0; i<MAX_PROP_ROW; i++)
   {
      tempProp[i].isChanged = false;
      tempProp[i].stopCondCount = 0;
      tempProp[i].propName = "";
      tempProp[i].scNames = "";
      tempProp[i].scNameList.Clear();
      tempProp[i].propSetupPtr = NULL;
   }

   for (int i=0; i<MAX_PROP_ROW; i++)
   {
      for (int j=0; j<MAX_STOPCOND_ROW; j++)
      {
         tempStopCond[i][j].isChanged = false;
         tempStopCond[i][j].name = "";
         tempStopCond[i][j].varName = "";
         tempStopCond[i][j].typeName = "";
         tempStopCond[i][j].relOpStr = "=";
         tempStopCond[i][j].goal = 0.0;
         tempStopCond[i][j].tol = 0.0;
         tempStopCond[i][j].repeat = 1;
      }
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
      new wxGrid( this, -1, wxDefaultPosition, wxSize(700,100), wxWANTS_CHARS);
   
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
   nameStaticText =
      new wxStaticText(this, -1, wxT("Name"), 
                        wxDefaultPosition, wxDefaultSize, 0);
   varStaticText =
      new wxStaticText(this, -1, wxT("Variable"), 
                        wxDefaultPosition, wxDefaultSize, 0);
   repeatStaticText =
      new wxStaticText(this, -1, wxT("Repeat"), 
                        wxDefaultPosition, wxDefaultSize, 0);
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
   viewButton =
      new wxButton(this, ID_BUTTON, wxT("View"),
                    wxDefaultPosition, wxDefaultSize, 0);
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
   wxBoxSizer *item10 = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *item11 = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *item12 = new wxBoxSizer(wxHORIZONTAL);
   //wxBoxSizer *item13 = new wxBoxSizer(wxHORIZONTAL);

   propSizer->Add(synchStaticText, 0, wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   propSizer->Add(synchComboBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   propSizer->Add(propGrid, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   
   item10->Add(nameStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   item10->Add(stopNameTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   item10->Add(75, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   item11->Add(varStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   item11->Add(varNameTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   item11->Add(viewButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   item11->Add(equalityComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   item11->Add(goalTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   item12->Add(repeatStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   item12->Add(repeatTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   //item12->Add(75, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   //item12->Add(75, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   //item12->Add(75, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   //item13->Add(tolStaticText, 0, wxALIGN_RIGHT|wxALL, bsize);
   //item13->Add(toleranceTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   //item13->Add(75, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   //item13->Add(updateButton, 0, wxALIGN_RIGHT|wxALL, bsize);
   //item13->Add(deleteButton, 0, wxALIGN_RIGHT|wxALL, bsize);
   
   item12->Add(tolStaticText, 0, wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   item12->Add(toleranceTextCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   item12->Add(updateButton, 0, wxALIGN_RIGHT|wxALL, bsize);
   item12->Add(deleteButton, 0, wxALIGN_RIGHT|wxALL, bsize);
    
   item9->Add(item10, 0, wxALIGN_LEFT|wxALL, bsize);    
   item9->Add(item11, 0, wxALIGN_LEFT|wxALL, bsize);
   item9->Add(item12, 0, wxALIGN_LEFT|wxALL, bsize);
   //item9->Add(item13, 0, wxALIGN_CENTRE|wxALL, bsize);

   detailSizer->Add(item9, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   mStopSizer->Add(stopCondGrid, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   mStopSizer->Add(detailSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   mMiddleBoxSizer->Add(propSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   mMiddleBoxSizer->Add(mStopSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   panelSizer->Add(mMiddleBoxSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
    
   theMiddleSizer->Add(panelSizer, 0, wxGROW, bsize);
    
   // waw: Future Implementation
   synchComboBox->Enable(false);
   deleteButton->Enable(false);
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
   tempPropCount = numOfProp;
   
   //loj: when multiple spacecraft is handled, move the block inside the for loop
   scID = thePropagateCommand->GetParameterID("Spacecraft");
   scList = thePropagateCommand->GetStringArrayParameter(scID);
   numOfSC = scList.size();

#if DEBUG_PROPCMD_PANEL
   MessageInterface::ShowMessage("PropagateCommandPanel::LoadData() numOfSC=%d\n",
                                 numOfSC);
#endif

   for (int i=0; i<numOfProp; i++)
   {
      tempProp[i].propName = wxT(propSetupName.c_str());
      tempProp[i].propSetupPtr = theGuiInterpreter->GetPropSetup(propSetupName);
      
      for (int j=0; j<numOfSC; j++)
      {
         tempProp[i].scNameList.Add(scList[j].c_str());
#if DEBUG_PROPCMD_PANEL
         MessageInterface::
            ShowMessage("PropagateCommandPanel::LoadData() scNameList[%d]=%s\n",
                        j, tempProp[i].scNameList[j].c_str());
#endif
      }
      
      if (numOfSC > 0)
      {
         for (int j=0; j<numOfSC-1; j++)
         {
            tempProp[i].scNames += scList[j].c_str();
            tempProp[i].scNames += ", ";
         }

         tempProp[i].scNames += scList[numOfSC-1].c_str();
      }
   

      //----------------------------------
      // stopping condition
      //----------------------------------

      //loj: Should there be a list of StopCondition per propagator for the
      //     propagate command?
      
      theStopCondBase = thePropagateCommand->GetObject(Gmat::STOP_CONDITION);
      theStopCond = (StopCondition *)theStopCondBase;

      //--------------------------------------------------------------
      //loj: 6/16/04
      //@note
      // for multiple stopping conditions, we need a method returning
      // StopCondition array from Propagate command
      // but set to 1 for now
      //--------------------------------------------------------------
      mStopCondCount = 1;  // temp value      

      Parameter *stopParam;
      for (int j=0; j<mStopCondCount; j++)
      {
         // StopCondition created from the script might not have been configured (unnamed)
         if (theStopCond != NULL)
         {
            tempStopCond[i][j].stopCondPtr = theStopCond;
            tempStopCond[i][j].name = wxT(theStopCond->GetName().c_str());
            stopParam = theStopCond->GetStopParameter();
            tempStopCond[i][j].varName = wxT(stopParam->GetName().c_str());
            tempStopCond[i][j].typeName = wxT(theStopCond->GetTypeName().c_str());
            tempStopCond[i][j].goal = theStopCond->GetRealParameter("Goal");
            tempStopCond[i][j].tol = theStopCond->GetRealParameter("Tol");
            tempStopCond[i][j].repeat = theStopCond->GetIntegerParameter("Repeat");
            wxString str = FormatStopCondDesc(tempStopCond[i][j].varName,
                                              tempStopCond[i][j].relOpStr,
                                              tempStopCond[i][j].goal);
            tempStopCond[i][j].desc = str;
         }
      }
   }

   DisplayPropagator();
   DisplayStopCondition(mStopCondCount, 0); // display first propagator stop condition
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void PropagateCommandPanel::SaveData()
{
#if DEBUG_PROPCMD_PANEL
   MessageInterface::ShowMessage
      ("PropagateCommandPanel::SaveData() tempPropCount=%d\n", tempPropCount);
#endif
         
   if (tempPropCount > 1)
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "Synchronized mode has not been implemented. "
          "It will use the first propagator in the list");
   
   //----------------------------------
   // Saving propagator
   //----------------------------------
   for (int i=0; i<numOfProp; i++)
   {
      if (tempProp[i].isChanged)
      {
         // Propagate::SetObject(PROP_SETUP) just sets propName
         thePropagateCommand->SetObject(std::string(tempProp[i].propName.c_str()),
                                        Gmat::PROP_SETUP);
         
         //----------------------------------
         // Saving spacecraft
         //----------------------------------
         numOfSC = tempProp[i].scNameList.Count();
         
         if (numOfSC > 1)
            MessageInterface::PopupMessage
               (Gmat::WARNING_, "Multiple spacecraft has not been implemented. "
                "It will use the first spacecraft in the list");
         
#if DEBUG_PROPCMD_PANEL
         MessageInterface::ShowMessage
            ("PropagateCommandPanel::SaveData() numOfSC=%d\n", numOfSC);
#endif
         // Propagate::SetObject(SPACECRAFT) adds to the list, so clear first
         thePropagateCommand->ClearObject(Gmat::SPACECRAFT);
         //for (int j=0; j<numOfSC; j++) // when multiple spacecraft is ready
         for (int j=0; j<1; j++)
         {
            thePropagateCommand->
               SetObject(std::string(tempProp[i].scNameList[j].c_str()),
                         Gmat::SPACECRAFT);
#if DEBUG_PROPCMD_PANEL
            MessageInterface::ShowMessage
               ("PropagateCommandPanel::SaveData() scNameList[%d]=%s\n",
                j, tempProp[i].scNameList[j].c_str());
#endif
         }
      }

      //loj: 6/15/04 added
      if (updateButton->IsEnabled())
         UpdateStopCondition();
      
      //----------------------------------
      // Saving stopping condition
      //----------------------------------
      StopCondition *stopCond;
      thePropagateCommand->ClearObject(Gmat::STOP_CONDITION);
                
      for (int j=0; j<mStopCondCount; j++)
      {
         if (tempStopCond[i][j].isChanged)
         {
            // StopCondition created from the script might not have been configured (unnamed)
            if (tempStopCond[i][j].stopCondPtr != NULL)
            {
               tempStopCond[i][j].stopCondPtr->
                  SetName(std::string(tempStopCond[i][j].name.c_str()));
                
               tempStopCond[i][j].stopCondPtr->
                  SetStringParameter("StopVar", tempStopCond[i][j].varName.c_str());
                
               tempStopCond[i][j].stopCondPtr->
                  SetRealParameter("Goal", tempStopCond[i][j].goal);
                
               tempStopCond[i][j].stopCondPtr->
                  SetRealParameter("Tol", tempStopCond[i][j].tol);
                
               tempStopCond[i][j].stopCondPtr->
                  SetIntegerParameter("Repeat", tempStopCond[i][j].repeat);

               thePropagateCommand->
                  SetObject(tempStopCond[i][j].stopCondPtr, Gmat::STOP_CONDITION);
                
               stopCond = (StopCondition*)thePropagateCommand->
                  GetObject(Gmat::STOP_CONDITION, tempStopCond[i][j].stopCondPtr->GetName());

#if DEBUG_PROPCMD_PANEL
               MessageInterface::ShowMessage
                  ("PropagateCommandPanel::SetData() name=%s, stopVarTYpe=%s, goal = %f\n",
                   stopCond->GetName().c_str(),
                   stopCond->GetStringParameter("StopVar").c_str(),
                   stopCond->GetRealParameter("Goal"));
#endif
            }
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
   MessageInterface::ShowMessage("PropagateCommandPanel::DisplayPropagator() entered\n");
#endif

   //loj: 3/2/04 added
   for (int i=0; i<tempPropCount; i++)
   {
      propGrid->SetCellValue(i, PROP_NAME_COL, tempProp[i].propName);
      propGrid->SetCellValue(i, PROP_SCS_COL, tempProp[i].scNames);
   }

   propGrid->SelectRow(0);    
}

// Called only once when window is initially displayed
//------------------------------------------------------------------------------
// void DisplayStopCondition(int numStopCond, int propRow)
//------------------------------------------------------------------------------
void PropagateCommandPanel::DisplayStopCondition(int numStopCond, int propRow)
{
#if DEBUG_PROPCMD_PANEL
   MessageInterface::ShowMessage("PropagateCommandPanel::DisplayStopCondition() entered\n");
#endif
   
   //----- for stopCondGrid - show all
   for (int j=0; j<numStopCond; j++)
   {
      stopCondGrid->SetCellValue(j, STOPCOND_NAME_COL, tempStopCond[propRow][j].name);
      stopCondGrid->SetCellValue(j, STOPCOND_DESC_COL, tempStopCond[propRow][j].desc);        
   }
   
   //----- for detailed stop condition - show first row
   stopNameTextCtrl->SetValue(tempStopCond[propRow][0].name); //loj: 6/16/04 changed from AppendText
   varNameTextCtrl->SetValue(tempStopCond[propRow][0].varName);
   equalityComboBox->SetSelection(equalityComboBox->FindString(tempStopCond[propRow][0].relOpStr));
   repeatTextCtrl->SetValue(wxVariant((long)(tempStopCond[propRow][0].repeat)));
   goalTextCtrl->SetValue(wxVariant(tempStopCond[propRow][0].goal));
   toleranceTextCtrl->SetValue(wxVariant(tempStopCond[propRow][0].tol));
    
   stopCondGrid->SelectRow(0);
   varNameTextCtrl->Disable();
}

// Called when new stop cond is created
//------------------------------------------------------------------------------
// void UpdateStopCondition()
//------------------------------------------------------------------------------
void PropagateCommandPanel::UpdateStopCondition()
{
   //MessageInterface::ShowMessage("PropagateCommandPanel::UpdateStopCondition() entered\n");
   //loj: 3/2/04 added - assume single selection
   wxArrayInt propRows = propGrid->GetSelectedRows();
   int propRow = propRows[0];
   wxArrayInt stopRows = stopCondGrid->GetSelectedRows();
   int stopRow = stopRows[0];

   wxString oldStopName = tempStopCond[propRow][stopRow].name;
   tempStopCond[propRow][stopRow].name = stopNameTextCtrl->GetValue();
   tempStopCond[propRow][stopRow].varName = varNameTextCtrl->GetValue();
   tempStopCond[propRow][stopRow].goal = atof(goalTextCtrl->GetValue().c_str());
   tempStopCond[propRow][stopRow].tol = atof(toleranceTextCtrl->GetValue().c_str());
   tempStopCond[propRow][stopRow].repeat = atoi(repeatTextCtrl->GetValue().c_str());
            
   wxString str = FormatStopCondDesc(tempStopCond[propRow][stopRow].varName,
                                     tempStopCond[propRow][stopRow].relOpStr,
                                     tempStopCond[propRow][stopRow].goal);
   tempStopCond[propRow][stopRow].desc = str;
    
   //loj: 3/31/04 create StopCondition
   // create StopCondition if new StopCondition
   if (!oldStopName.IsSameAs(tempStopCond[propRow][stopRow].name))
   {
      StopCondition *stopCond = theGuiInterpreter->
         CreateStopCondition("StopCondition",
                             std::string(tempStopCond[propRow][stopRow].name.c_str()));
      tempStopCond[propRow][stopRow].stopCondPtr = stopCond;

      if (stopCond == NULL)
      {
         MessageInterface::ShowMessage
            ("PropagateCommandPanel::UpdateStopCondition() Unable to create "
             "StopCondition: name=%s\n", tempStopCond[propRow][stopRow].name.c_str());
      }
   }
    
   stopCondGrid->SetCellValue(stopRow, STOPCOND_NAME_COL,
                              tempStopCond[propRow][stopRow].name);
   stopCondGrid->SetCellValue(stopRow, STOPCOND_DESC_COL,
                              tempStopCond[propRow][stopRow].desc);
   tempStopCond[propRow][stopRow].isChanged = true;
        
   updateButton->Enable(false);
   theApplyButton->Enable(true);
    
   /* waw: Part of build 3
      mStopCondCount++;
    
      wxString name = stopNameTextCtrl->GetValue();
      if (!name.IsEmpty())
      stopCondGrid->SetCellValue((mStopCondCount-1), 0, wxT(name));
        
      wxString cond = varNameTextCtrl->GetValue();
      cond.Append(" ");
      cond.Append(equalityComboBox->GetStringSelection());
      cond.Append(" ");
      cond.Append(goalTextCtrl->GetValue());
      if (!cond.IsEmpty())
      stopCondGrid->SetCellValue((mStopCondCount-1), 1, wxT(cond));
        
      updateButton->Enable(false);
   */
}

//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagateCommandPanel::OnTextChange(wxCommandEvent& event)
{
   ActivateUpdateButton();
}

//------------------------------------------------------------------------------
// void OnComboSelection(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagateCommandPanel::OnComboSelection(wxCommandEvent& event)
{
   //loj: assume single selection
   wxArrayInt propRows = propGrid->GetSelectedRows();
   wxArrayInt stopRows = stopCondGrid->GetSelectedRows();
   int propRow = propRows[0];
   int stopRow = stopRows[0];
    
   if (event.GetEventObject() == synchComboBox )
   {
      theApplyButton->Enable(true);
   }
   else if (event.GetEventObject() == equalityComboBox )
   {
      wxString str = FormatStopCondDesc(tempStopCond[propRow][stopRow].varName,
                                        tempStopCond[propRow][stopRow].relOpStr,
                                        tempStopCond[propRow][stopRow].goal);
      tempStopCond[propRow][stopRow].desc = str;
      tempStopCond[propRow][stopRow].isChanged = true;
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
      }

      //loj: 6/15/04
      // if stopping condition parameter changed, give it a new name
      stopNameTextCtrl->SetValue("StopOn" + newParamName);

      // if Apoapsis or Periapsis, disable goal
      if (newParamName.Contains("Periapsis") || newParamName.Contains("Apoapsis"))
      {
         goalTextCtrl->Disable();
         equalityComboBox->Disable();
      }
      else
      {
         goalTextCtrl->Enable();
         equalityComboBox->Enable();
      }
      
      updateButton->Enable(true);
   }
   else if (event.GetEventObject() == updateButton )
   {
      UpdateStopCondition();
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
      if (propGrid->GetCellValue(row, 1) == "")
      {
         // disable stopcondition
         if (propGrid->GetCellValue(row, 1) == "")    
         {
            mMiddleBoxSizer->Show(mStopSizer, false);
         }
      }
      else
      {
         mMiddleBoxSizer->Show(mStopSizer, true);
         DisplayStopCondition(1, row);
      }
   }
   else if (event.GetEventObject() == stopCondGrid)
   {
      wxArrayInt propRows = propGrid->GetSelectedRows();
      int propRow = propRows[0];
      stopCondGrid->SelectRow(row);
      ShowDetailedStopCond(propRow, row);
   }
}

//------------------------------------------------------------------------------
// void OnCellRightClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void PropagateCommandPanel::OnCellRightClick(wxGridEvent& event)
{    
   int row = event.GetRow();
   int col = event.GetCol();

   if (row <= tempPropCount)
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
                  if (tempPropCount <= row)
                     tempPropCount = row + 1;
                  
#if DEBUG_PROPCMD_PANEL
                  MessageInterface::ShowMessage
                     ("PropagateCommandPanel::OnCellRightClick() tempPropCount=%d\n",
                      tempPropCount);
#endif
                  propGrid->SetCellValue(row, col, dialog.GetStringSelection());
                  tempProp[row].isChanged = true;
                  tempProp[row].propName = dialog.GetStringSelection();
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
            for (int i=0; i<tempPropCount; i++)
            {
               numOfSC = tempProp[i].scNameList.Count();
            
               for (int j=0; j<numOfSC; j++)
                  scExcList.Add(tempProp[i].scNameList[j]);
            }
         
            SpacecraftSelectDialog scDlg(this, tempProp[row].scNameList, scExcList);
            scDlg.ShowModal();
         
            if (scDlg.IsSpacecraftSelected())
            {
               wxArrayString &names = scDlg.GetSpacecraftNames();
               tempProp[row].isChanged = true;
               tempProp[row].scNames = "";
               numOfSC = names.GetCount();
            
#if DEBUG_PROPCMD_PANEL
               MessageInterface::ShowMessage
                  ("PropagateCommandPanel::OnCellRightClick() numOfSC=%d\n", numOfSC);
#endif
            
               tempProp[row].scNameList.Clear();
               for (int j=0; j<numOfSC; j++)
               {
                  tempProp[row].scNameList.Add(names[j]);
#if DEBUG_PROPCMD_PANEL
                  MessageInterface::ShowMessage
                     ("PropagateCommandPanel::OnCellRightClick() scNameList[%d]=%s\n",
                      j, tempProp[row].scNameList[j].c_str());
#endif
               }
            
               for(Integer j=0; j < numOfSC-1; j++)
               {
                  tempProp[row].scNames += names[j].c_str();
                  tempProp[row].scNames += ", ";
               }

               tempProp[row].scNames += names[numOfSC-1].c_str();
            
               propGrid->SetCellValue(row, col, tempProp[row].scNames);

               //----------------------------------------
               // future: crete default stopping condition 
               //----------------------------------------
               //StopCondition *stopCond;
               //for(Integer j=0; j < numOfSC-1; j++)
               //{
               //   stopCond =
               //      theGuiInterpreter->CreateDefaultStopCondition(scNameList[i]);
               //   //tempProp[row].stopInfo.stopCondPtr = stopCond;
               //}
               
               //----------------------------------------
               // show warning about stopping condition
               //----------------------------------------
               MessageInterface::PopupMessage
                  (Gmat::WARNING_, "Make sure to set stopping conditions for the "
                   "selected spacecrft(s)");
               
               DisplayStopCondition(1, row);
               theApplyButton->Enable(true);
            }
         }
      }
   }
}

//------------------------------------------------------------------------------
// void ClearDetailedStopCond()
//------------------------------------------------------------------------------
void PropagateCommandPanel::ClearDetailedStopCond()
{
   stopNameTextCtrl->SetValue("");
   varNameTextCtrl->SetValue("");
   goalTextCtrl->SetValue("");
   repeatTextCtrl->SetValue("1");
   toleranceTextCtrl->SetValue("0.0");

}

//------------------------------------------------------------------------------
// void ShowDetailedStopCond(int propRow, int stopRow)
//------------------------------------------------------------------------------
void PropagateCommandPanel::ShowDetailedStopCond(int propRow, int stopRow)
{
   //loj: 3/1/04 assume there is only stopcondition for build 2
   // so stopCondRow is not used
    
   stopNameTextCtrl->SetValue(tempStopCond[propRow][stopRow].name);
   varNameTextCtrl->SetValue(tempStopCond[propRow][stopRow].varName);
   equalityComboBox->SetSelection(equalityComboBox->
                                  FindString(tempStopCond[propRow][stopRow].relOpStr));
   repeatTextCtrl->SetValue(wxVariant((long)(tempStopCond[propRow][stopRow].repeat)));
   goalTextCtrl->SetValue(wxVariant(tempStopCond[propRow][stopRow].goal));
   toleranceTextCtrl->SetValue(wxVariant(tempStopCond[propRow][stopRow].tol));
}

//------------------------------------------------------------------------------
// void ActivateUpdateButton()
//------------------------------------------------------------------------------
void PropagateCommandPanel::ActivateUpdateButton()
{
   //loj: assume single selection
   stopCondGrid->SelectRow(0);
   wxArrayInt propRows = propGrid->GetSelectedRows();
   wxArrayInt stopRows = stopCondGrid->GetSelectedRows();
   int propRow = propRows[0];
   int stopRow = stopRows[0];

   tempStopCond[propRow][stopRow].isChanged = true;
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

