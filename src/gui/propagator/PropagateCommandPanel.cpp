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
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(PropagateCommandPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
    
   //EVT_TEXT(ID_TEXTCTRL, PropagateCommandPanel::OnTextChange)
   EVT_TEXT_ENTER(ID_TEXTCTRL, PropagateCommandPanel::OnTextChange)
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
   numOfStopCond = 0;
   numOfProp = 0;
   numOfModes = 1;
   numOfEqualities = 6;
    
   for (int i=0; i<MAX_PROP_ROW; i++)
   {
      tempProp[i].isChanged = false;
      tempProp[i].propName = "";
      tempProp[i].scNames = "";
      tempProp[i].propSetupPtr = NULL;
   }
    
   for (int i=0; i<MAX_STOPCOND_ROW; i++)
   {
      tempStopCond[i].isChanged = false;
      tempStopCond[i].name = "";
      tempStopCond[i].varName = "";
      tempStopCond[i].typeName = "";
      tempStopCond[i].relOpStr = "=";
      tempStopCond[i].goal = 0.0;
      tempStopCond[i].tol = 0.0;
      tempStopCond[i].repeat = 1;
   }
    
   Create();
   Show();
   updateButton->Disable();
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
   
   wxBoxSizer *theMiddleBoxSizer = new wxBoxSizer(wxVERTICAL);
    
   // wxGrid
   //loj: 2/13/04 changed wxSize(150,160) to wxSize(100,160) to match 
   //with stopCond width
   propGrid =
      new wxGrid( this, -1, wxDefaultPosition, wxSize(100,160), wxWANTS_CHARS );
   
   propGrid->CreateGrid( MAX_PROP_ROW, 2, wxGrid::wxGridSelectRows );
   propGrid->SetColSize(0, 200);
   //propGrid->SetColSize(1, 500); //loj: 3/1/04 It scrolls back and forth 
   //when I click
   propGrid->SetColSize(1, 470);
   propGrid->SetColLabelValue(0, _T("Propagator"));
   propGrid->SetColLabelValue(1, _T("Spacecraft List"));
   propGrid->SetRowLabelSize(0);
   propGrid->EnableEditing(false);
    
   stopCondGrid =
      new wxGrid( this, -1, wxDefaultPosition, wxSize(100,160), wxWANTS_CHARS );
   
   stopCondGrid->CreateGrid( MAX_STOPCOND_ROW, 2, wxGrid::wxGridSelectRows );
   stopCondGrid->SetColSize(0, 200);
   //stopCondGrid->SetColSize(1, 500); //loj: 3/1/04 It scrolls back and forth 
   //when I click
   stopCondGrid->SetColSize(1, 470);
   stopCondGrid->SetColLabelValue(0, _T("Name"));
   stopCondGrid->SetColLabelValue(1, _T("Condition"));
   stopCondGrid->SetRowLabelSize(0);
   stopCondGrid->EnableEditing(false);

    // wxString
   wxString strArray1[] = 
   {
      wxT("None")
   };
   wxString strArray2[] =
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
      new wxStaticText( this, -1, wxT("Synchronization Mode"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   nameStaticText =
      new wxStaticText( this, -1, wxT("Name"), 
                        wxDefaultPosition, wxDefaultSize, 0 );
   varStaticText =
      new wxStaticText( this, -1, wxT("Variable"), 
                        wxDefaultPosition, wxDefaultSize, 0 );
   repeatStaticText =
      new wxStaticText( this, -1, wxT("Repeat"), 
                        wxDefaultPosition, wxDefaultSize, 0 );
   tolStaticText =
      new wxStaticText( this, -1, wxT("Tolerance"), 
                        wxDefaultPosition, wxDefaultSize, 0 );

   // wxTextCtrl
   stopNameTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(250,-1), 0 );
   varNameTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(250,-1), 0 );
   goalTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(150,-1), 0 );
   repeatTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(80,-1), 0 );
   toleranceTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(80,-1), 0 );

   // wxButton
   viewButton =
      new wxButton( this, ID_BUTTON, wxT("View"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   updateButton =
      new wxButton( this, ID_BUTTON, wxT("Update"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   deleteButton =
      new wxButton( this, ID_BUTTON, wxT("Delete"),
                    wxDefaultPosition, wxDefaultSize, 0 );
    
   // wxComboBox
   synchComboBox =
      new wxComboBox( this, ID_COMBO, wxT(strArray1[0]), wxDefaultPosition,
                      wxSize(200,-1), numOfModes, strArray1,
                      wxCB_DROPDOWN|wxCB_READONLY );
   equalityComboBox =
      new wxComboBox( this, ID_COMBO, wxT(strArray2[0]), wxDefaultPosition,
                      wxSize(50,-1), numOfEqualities, strArray2,
                      wxCB_DROPDOWN|wxCB_READONLY );
       
   // wx*Sizer    
   wxBoxSizer *panelSizer = new wxBoxSizer( wxVERTICAL );
    
   wxStaticBox *propStaticBox =
      new wxStaticBox( this, -1, wxT("Propagators and Spacecraft") );
   wxStaticBox *stopCondStaticBox =
      new wxStaticBox( this, -1, wxT("Stopping Conditions") );
   wxStaticBox *stopDetailStaticBox =
      new wxStaticBox( this, -1, wxT("Stopping Condition Details") );
    
   wxStaticBoxSizer *propSizer =
      new wxStaticBoxSizer( propStaticBox, wxVERTICAL );
   wxStaticBoxSizer *stopSizer =
      new wxStaticBoxSizer( stopCondStaticBox, wxVERTICAL );
   wxStaticBoxSizer *detailSizer =
      new wxStaticBoxSizer( stopDetailStaticBox, wxVERTICAL );    
    
   wxBoxSizer *item9 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *item10 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *item11 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *item12 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *item13 = new wxBoxSizer( wxHORIZONTAL );

   propSizer->Add( synchStaticText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   propSizer->Add( synchComboBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );    
   propSizer->Add( propGrid, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

   item10->Add( nameStaticText, 0, wxALIGN_LEFT|wxALL, 5 );
   item10->Add( stopNameTextCtrl, 0, wxALIGN_LEFT|wxALL, 5 );
   item10->Add( 75, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    
   item11->Add( varStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
   item11->Add( varNameTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
   item11->Add( viewButton, 0, wxALIGN_CENTRE|wxALL, 5 );
   item11->Add( equalityComboBox, 0, wxALIGN_CENTRE|wxALL, 5 );
   item11->Add( goalTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    
   item12->Add( repeatStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
   item12->Add( repeatTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
   item12->Add( 75, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
   item12->Add( 75, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
   item12->Add( 75, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    
   item13->Add( tolStaticText, 0, wxALIGN_RIGHT|wxALL, 5 );
   item13->Add( toleranceTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
   item13->Add( 75, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
   item13->Add( updateButton, 0, wxALIGN_RIGHT|wxALL, 5 );
   item13->Add( deleteButton, 0, wxALIGN_RIGHT|wxALL, 5 );
    
   item9->Add( item10, 0, wxALIGN_CENTRE|wxALL, 5 );    
   item9->Add( item11, 0, wxALIGN_CENTRE|wxALL, 5 );
   item9->Add( item12, 0, wxALIGN_CENTRE|wxALL, 5 );
   item9->Add( item13, 0, wxALIGN_CENTRE|wxALL, 5 );

   detailSizer->Add( item9, 0, wxALIGN_CENTRE|wxALL, 5 );
    
   stopSizer->Add( stopCondGrid, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   stopSizer->Add( detailSizer, 0, wxALIGN_CENTRE|wxALL, 5 );

   theMiddleBoxSizer->Add( propSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, 5 );
   theMiddleBoxSizer->Add( stopSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, 5 );
    
   panelSizer->Add( theMiddleBoxSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, 5 );
    
   theMiddleSizer->Add(panelSizer, 0, wxGROW, 5);
    
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
   MessageInterface::ShowMessage("PropagateCommandPanel::LoadData() entered\n");
    
   //----------------------------------
   // propagator
   //----------------------------------
   propID = thePropagateCommand->GetParameterID("Propagator");
   propSetupName = thePropagateCommand->GetStringParameter(propID);

    //loj: 3/2/04 only 1 PropSetup for build2
   numOfProp = 1;  // waw: TBD

   //loj: when multiple spacecraft is handled, move the block inside the for loop
   scID = thePropagateCommand->GetParameterID("Spacecraft");
   scList = thePropagateCommand->GetStringArrayParameter(scID);
   numOfSC = scList.size();

   for (int i=0; i<numOfProp; i++)
   {
      tempProp[i].propName = wxT(propSetupName.c_str());
      tempProp[i].propSetupPtr = theGuiInterpreter->GetPropSetup(propSetupName);
      
      if (numOfSC > 0)
      {
         for (int j=0; j<numOfSC-1; j++)
         {
            tempProp[i].scNames += scList[j].c_str();
            tempProp[i].scNames += ",";
         }
    
         tempProp[i].scNames += scList[numOfSC-1].c_str();
      }
   }

   //----------------------------------
   // stopping condition
   //----------------------------------
   //loj: when multiple stop condition is handled, move the block inside the for loop
   theStopCondBase = thePropagateCommand->GetObject(Gmat::STOP_CONDITION);
   theStopCond = (StopCondition *)theStopCondBase;
    
   numOfStopCond = 1;  // temp value      

   Parameter *stopParam;
   for (int i=0; i<numOfStopCond; i++)
   {
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
   /*  Waw:
       Display all the stop. cond. elements 
       Methods for this are currently not avaiable.
        
       numOfStopCond = thePropagateCommand->GetStopConditionSize();
       theStopCondVector = thePropagateCommand->GetObject(GMAT::STOP_CONDITION);
   */

   DisplayPropagator();
   DisplayStopCondition();
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void PropagateCommandPanel::SaveData()
{
   //----------------------------------
   // Saving propagator
   //----------------------------------
   //loj: just one propagator for build 2
   for (int i=0; i<numOfProp; i++)
   {
      if (tempProp[i].isChanged)
      {
         theGuiInterpreter->
            GetPropSetup(std::string(tempProp[i].propName.c_str()));

         //loj: save spacecraft list for the future build
      }
   }
    
   // waw: To Do:- future implemention to save multiple propagators
   PropSetup *propSetup = theGuiInterpreter->GetPropSetup(tempProp[0].propName.c_str());
   thePropagateCommand->SetStringParameter(propID, tempProp[0].propName.c_str());
   //thePropagateCommand->SetObject(propSetup, Gmat::PROP_SETUP);
   thePropagateCommand->SetObject(propSetup->GetName(), Gmat::PROP_SETUP);
    
   //----------------------------------
   // Saving spacecraft
   //----------------------------------
   // waw: To Do:- future implemention to save multiple spacecrafts
   Spacecraft *spacecraft = theGuiInterpreter->GetSpacecraft(tempProp[0].scNames.c_str());
   thePropagateCommand->SetStringParameter(scID, tempProp[0].scNames.c_str());
   //thePropagateCommand->SetObject(spacecraft, Gmat::SPACECRAFT);
   thePropagateCommand->SetObject(spacecraft->GetName(), Gmat::SPACECRAFT);
       
   //----------------------------------
   // Saving stopping condition
   //----------------------------------
   StopCondition *stopCond;
   thePropagateCommand->ClearObject(Gmat::STOP_CONDITION);
                
   for (int i=0; i<numOfStopCond; i++)
   {
      if (tempStopCond[i].isChanged)
      {
         // StopCondition created from the script might not have been configured (unnamed)
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

            thePropagateCommand->
               SetObject(tempStopCond[i].stopCondPtr, Gmat::STOP_CONDITION);
                
            stopCond = (StopCondition*)thePropagateCommand->
               GetObject(Gmat::STOP_CONDITION, tempStopCond[i].stopCondPtr->GetName());
                
            //MessageInterface::ShowMessage
            //    ("PropagateCommandPanel::SetData() name=%s, stopVarTYpe=%s, goal = %f\n",
            //     stopCond->GetName().c_str(),
            //     stopCond->GetStringParameter("StopVar").c_str(),
            //     stopCond->GetRealParameter("Goal"));
         }
      }
   }
}

//------------------------------------------------------------------------------
// void DisplayPropagator()
//------------------------------------------------------------------------------
void PropagateCommandPanel::DisplayPropagator()
{
   MessageInterface::ShowMessage("PropagateCommandPanel::DisplayPropagator() entered\n");
    
   //loj: 3/2/04 added
   for (int i=0; i<numOfProp; i++)
   {
      propGrid->SetCellValue(i, PROP_NAME_COL, tempProp[i].propName);
      propGrid->SetCellValue(i, PROP_SCS_COL, tempProp[i].scNames);
   }

   propGrid->SelectRow(0);
    
   //loj: 3/2/04 commented out
   //      propGrid->SetCellValue( 0, 0, wxT(propSetupName.c_str()) );
    
   //      if ( !scList.empty() )
   //      {
   //          /* waw: Future implementation
   //          for (int i = 0; i < numOfSC; i++)
   //          {
   //              scListString.Append(scList.at(i).c_str());
   //              scListString.Append(" ");
   //          }*/
   //          scListString.Append(scList.at(0).c_str());    
   //          propGrid->SetCellValue( 0, 1, scListString );
   //      }
    
}

// Called only once when window is initially displayed
//------------------------------------------------------------------------------
// void DisplayStopCondition()
//------------------------------------------------------------------------------
void PropagateCommandPanel::DisplayStopCondition()
{
   MessageInterface::ShowMessage("PropagateCommandPanel::DisplayStopCondition() entered\n");
    
   //----- for stopCondGrid - show all
   for (int i=0; i<numOfStopCond; i++)
   {
      stopCondGrid->SetCellValue(i, STOPCOND_NAME_COL, tempStopCond[i].name);
      stopCondGrid->SetCellValue(i, STOPCOND_DESC_COL, tempStopCond[i].desc);        
   }
   
   //----- for detailed stop condition - show first row
   stopNameTextCtrl->AppendText(tempStopCond[0].name);
   varNameTextCtrl->SetValue(tempStopCond[0].varName);
   equalityComboBox->SetSelection(equalityComboBox->FindString(tempStopCond[0].relOpStr));
   repeatTextCtrl->SetValue(wxVariant((long)(tempStopCond[0].repeat)));
   goalTextCtrl->SetValue(wxVariant(tempStopCond[0].goal));
   toleranceTextCtrl->SetValue(wxVariant(tempStopCond[0].tol));
    
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
   wxArrayInt rows = stopCondGrid->GetSelectedRows();
   int row = rows[0];

   wxString oldStopName = tempStopCond[row].name;
   tempStopCond[row].name = stopNameTextCtrl->GetValue();
   tempStopCond[row].varName = varNameTextCtrl->GetValue();
   tempStopCond[row].goal = atof(goalTextCtrl->GetValue().c_str());
   tempStopCond[row].tol = atof(toleranceTextCtrl->GetValue().c_str());
   tempStopCond[row].repeat = atoi(repeatTextCtrl->GetValue().c_str());
            
   wxString str = FormatStopCondDesc(tempStopCond[row].varName,
                                     tempStopCond[row].relOpStr,
                                     tempStopCond[row].goal);
   tempStopCond[row].desc = str;
    
    //loj: 3/31/04 create StopCondition
    // create StopCondition if new StopCondition
   if (!oldStopName.IsSameAs(tempStopCond[row].name))
   {
      StopCondition *stopCond = theGuiInterpreter->
         CreateStopCondition("StopCondition",
                             std::string(tempStopCond[row].name.c_str()));
      tempStopCond[row].stopCondPtr = stopCond;

      if (stopCond == NULL)
      {
         MessageInterface::ShowMessage
            ("PropagateCommandPanel::UpdateStopCondition() Unable to create "
             "StopCondition: name=%s\n", tempStopCond[row].name.c_str());
      }
   }
    
   stopCondGrid->SetCellValue(row, STOPCOND_NAME_COL, tempStopCond[row].name);
   stopCondGrid->SetCellValue(row, STOPCOND_DESC_COL, tempStopCond[row].desc);
   tempStopCond[row].isChanged = true;
        
   updateButton->Enable(false);
   theApplyButton->Enable(true);
    
   /* waw: Part of build 3
      numOfStopCond++;
    
      wxString name = stopNameTextCtrl->GetValue();
      if (!name.IsEmpty())
      stopCondGrid->SetCellValue( (numOfStopCond-1), 0, wxT(name) );
        
      wxString cond = varNameTextCtrl->GetValue();
      cond.Append(" ");
      cond.Append(equalityComboBox->GetStringSelection());
      cond.Append(" ");
      cond.Append(goalTextCtrl->GetValue());
      if (!cond.IsEmpty())
      stopCondGrid->SetCellValue( (numOfStopCond-1), 1, wxT(cond) );
        
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
   wxArrayInt rows = stopCondGrid->GetSelectedRows();
   int row = rows[0];
    
   if ( event.GetEventObject() == synchComboBox )
   {
      theApplyButton->Enable(true);
   }
   else if ( event.GetEventObject() == equalityComboBox )
   {
      wxString str = FormatStopCondDesc(tempStopCond[row].varName,
                                        tempStopCond[row].relOpStr,
                                        tempStopCond[row].goal);
      tempStopCond[row].desc = str;
      tempStopCond[row].isChanged = true;
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
   if ( event.GetEventObject() == viewButton )
   {
      // show dialog to select parameter
      ParameterSelectDialog paramDlg(this);
      paramDlg.ShowModal();

      if (paramDlg.IsParamSelected())
      {
         newParamName = paramDlg.GetParamName();
         varNameTextCtrl->SetValue(newParamName);
      }
      updateButton->Enable(true);
   }
   else if ( event.GetEventObject() == updateButton )
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
   }
   else if (event.GetEventObject() == stopCondGrid)
   {
      stopCondGrid->SelectRow(row);
      ShowDetailedStopCond(row);
   }
}

//------------------------------------------------------------------------------
// void OnCellRightClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void PropagateCommandPanel::OnCellRightClick(wxGridEvent& event)
{    
   int row = event.GetRow();
   int col = event.GetCol();
    
   if (event.GetEventObject() == propGrid)
   {
      // select propagator
      if (col == 0)
      {
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
               propGrid->SetCellValue(row, col, dialog.GetStringSelection());
               tempProp[row].isChanged = true;
               tempProp[row].propName = dialog.GetStringSelection(); // waw added 03/31/04
               theApplyButton->Enable(true);
            }
         }
      }
      // select spacecraft
      else if (col == 1)
      {
         StringArray &list =
            theGuiInterpreter->GetListOfConfiguredItems(Gmat::SPACECRAFT);
         int size = list.size();
         wxString *choices = new wxString[size];
        
         for (int i=0; i<size; i++)
         {
            choices[i] = list[i].c_str();
         }
        
         wxSingleChoiceDialog dialog(this, _T("Available Spacecraft: \n"),
                                     _T("SpacecraftSelectDialog"), size, choices);
         dialog.SetSelection(0);

         if (dialog.ShowModal() == wxID_OK)
         {
            if (dialog.GetStringSelection() != propGrid->GetCellValue(row, col))
            {
               propGrid->SetCellValue(row, col, dialog.GetStringSelection());
               tempProp[row].isChanged = true;
               tempProp[row].scNames = dialog.GetStringSelection(); // waw added 03/31/04
               theApplyButton->Enable(true);
            }
         }
      }
      // waw: Commented out until multiple s/c is implemented
      //        // select spacecraft
      //        else if (col == 1)
      //        {
      //            StringArray &list =
      //                theGuiInterpreter->GetListOfConfiguredItems(Gmat::SPACECRAFT);
      //            int size = list.size();
      //            wxString *choices = new wxString[size];
      //        
      //            for (int i=0; i<size; i++)
      //            {
      //                choices[i] = list[i].c_str();
      //            }
      //        
      //            wxArrayInt selections;
      //            size_t count = wxGetMultipleChoices(selections,
      //                                                _T("Available Spacecraft:"),
      //                                                _T("SpacecraftSelectDialog"),
      //                                                size, choices, this);
      //            if (count > 0)
      //            {
      //                wxString sclistString;
      //                for ( size_t i=0; i<count-1; i++ )
      //                {
      //                    sclistString += wxString::Format(wxT("%s, "), 
      //                                                     choices[selections[i]].c_str());
      //                    
      //                }
      //                
      //                sclistString += wxString::Format(wxT("%s"), 
      //                                                 choices[selections[count-1]].c_str());
      //                
      //                if (sclistString != propGrid->GetCellValue(row, col))
      //                {
      //                    propGrid->SetCellValue(row, col, sclistString);
      //                    tempProp[row].scNames = choices[0].c_str(); // waw added 03/31/04
      //                    applyButton->Enable(true);
      //                }
      //            }
      //        }
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
// void ShowDetailedStopCond(int row)
//------------------------------------------------------------------------------
void PropagateCommandPanel::ShowDetailedStopCond(int row)
{
   //loj: 3/1/04 assume there is only stopcondition for build 2
   // so stopCondRow is not used
    
   stopNameTextCtrl->SetValue(tempStopCond[row].name);
   varNameTextCtrl->SetValue(tempStopCond[row].varName);
   equalityComboBox->SetSelection(equalityComboBox->FindString(tempStopCond[row].relOpStr));
   repeatTextCtrl->SetValue(wxVariant((long)(tempStopCond[row].repeat)));
   goalTextCtrl->SetValue(wxVariant(tempStopCond[row].goal));
   toleranceTextCtrl->SetValue(wxVariant(tempStopCond[row].tol));
    
}

//------------------------------------------------------------------------------
// void ActivateUpdateButton()
//------------------------------------------------------------------------------
void PropagateCommandPanel::ActivateUpdateButton()
{
   //loj: assume single selection
   wxArrayInt rows = stopCondGrid->GetSelectedRows();
   int row = rows[0];

   tempStopCond[row].isChanged = true;
   updateButton->Enable(true);
   //applyButton->Enable(true); //loj: why this was commented?
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
   goalStr.Printf("%.9f", goal);
   desc = varName + " " + relOpStr + " " + goalStr;
    
   return desc;
}

