//$Header$
//------------------------------------------------------------------------------
//                              PropagateCommandPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/08/29
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
//
/**
 * This class contains the Propagator Setup window.
 */
//------------------------------------------------------------------------------

// gui includes
#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/grid.h>
//loj: 3/1/04 commented out for build2
//#include <wx/docview.h>
#include <wx/menu.h>
#include <wx/variant.h>

#include "gmatwxdefs.hpp"
//loj: 3/1/04 commented out for build2
//#include "ViewTextFrame.hpp"
//#include "DocViewFrame.hpp"
//#include "TextEditView.hpp"
//#include "TextDocument.hpp"
//#include "MdiTextEditView.hpp"
//#include "MdiDocViewFrame.hpp"
#include "GmatMainNotebook.hpp"
#include "PropagateCommandPanel.hpp"
#include "ParameterSelectDialog.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"
#include "Command.hpp"
#include "MessageInterface.hpp"
#include "StopCondition.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(PropagateCommandPanel, wxPanel)
    EVT_BUTTON(ID_BUTTON, PropagateCommandPanel::OnButton)
    EVT_COMBOBOX(ID_COMBO, PropagateCommandPanel::OnComboSelection)
    //EVT_TEXT(ID_TEXTCTRL, PropagateCommandPanel::OnTextUpdate) //loj: 3/3/04 if added runtime error
    EVT_TEXT_ENTER(ID_TEXTCTRL, PropagateCommandPanel::OnTextUpdate)
    EVT_TEXT_MAXLEN(ID_TEXTCTRL, PropagateCommandPanel::OnTextMaxLen)
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
    : wxPanel(parent)
{
    //MessageInterface::ShowMessage("PropagateCommandPanel::PropagateCommandPanel() entered\n");
    propNameString = propName;
    theCommand = cmd;
    
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
    
    Setup(this);
    
    if (theCommand != NULL)
    {
        Initialize();    
        LoadData();
    }
    
    applyButton->Enable(false);
    updateButton->Enable(false);
}

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
void PropagateCommandPanel::Initialize()
{
    //MessageInterface::ShowMessage("PropagateCommandPanel::Initialize() entered\n");
    theGuiInterpreter = GmatAppData::GetGuiInterpreter();
    thePropagateCommand = (Propagate *)theCommand;
    
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
}

void PropagateCommandPanel::Setup( wxWindow *parent)
{
    //MessageInterface::ShowMessage("PropagateCommandPanel::Setup() entered\n");
    wxBoxSizer *theMiddleBoxSizer = new wxBoxSizer(wxVERTICAL);
    
    // wxGrid
    //loj: 2/13/04 changed wxSize(150,160) to wxSize(100,160) to match with stopCond width
    propGrid = new wxGrid( parent, ID_GRID, wxDefaultPosition, wxSize(100,160), wxWANTS_CHARS );
    propGrid->CreateGrid( MAX_PROP_ROW, 2, wxGrid::wxGridSelectRows );
    propGrid->SetColSize(0, 200);
    //propGrid->SetColSize(1, 500); //loj: 3/1/04 It scrolls back and forth when I click
    propGrid->SetColSize(1, 470);
    propGrid->SetColLabelValue(0, _T("Propagator"));
    propGrid->SetColLabelValue(1, _T("Spacecraft List"));
    propGrid->SetRowLabelSize(0);
    propGrid->EnableEditing(false);
    
    stopCondGrid = new wxGrid( parent, ID_GRID, wxDefaultPosition, wxSize(100,160), wxWANTS_CHARS );
    stopCondGrid->CreateGrid( MAX_STOPCOND_ROW, 2, wxGrid::wxGridSelectRows );
    stopCondGrid->SetColSize(0, 200);
    //stopCondGrid->SetColSize(1, 500); //loj: 3/1/04 It scrolls back and forth when I click
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
    synchStaticText = new wxStaticText( parent, ID_TEXT, wxT("Synchronization Mode"), wxDefaultPosition, wxDefaultSize, 0 );
    nameStaticText = new wxStaticText( parent, ID_TEXT, wxT("Name"), wxDefaultPosition, wxDefaultSize, 0 );
    varStaticText = new wxStaticText( parent, ID_TEXT, wxT("Variable"), wxDefaultPosition, wxDefaultSize, 0 );
    repeatStaticText = new wxStaticText( parent, ID_TEXT, wxT("Repeat"), wxDefaultPosition, wxDefaultSize, 0 );
    tolStaticText = new wxStaticText( parent, ID_TEXT, wxT("Tolerance"), wxDefaultPosition, wxDefaultSize, 0 );
    //condTypeStaticText = new wxStaticText( parent, ID_TEXT, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxTextCtrl
    nameTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(250,-1), 0 );
    variableTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(250,-1), 0 );
    goalTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
    repeatTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    toleranceTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    
    // wxButton
    scriptButton = new wxButton( parent, ID_BUTTON, wxT("View Script"), wxDefaultPosition, wxDefaultSize, 0 );
    viewButton = new wxButton( parent, ID_BUTTON, wxT("View"), wxDefaultPosition, wxDefaultSize, 0 );
    updateButton = new wxButton( parent, ID_BUTTON, wxT("Update"), wxDefaultPosition, wxDefaultSize, 0 );
    deleteButton = new wxButton( parent, ID_BUTTON, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    okButton = new wxButton( parent, ID_BUTTON, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    applyButton = new wxButton( parent, ID_BUTTON, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    cancelButton = new wxButton( parent, ID_BUTTON, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    helpButton = new wxButton( parent, ID_BUTTON, wxT("Help"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxComboBox
    synchComboBox = new wxComboBox( parent, ID_COMBO, wxT(strArray1[0]), wxDefaultPosition, wxSize(200,-1), numOfModes, strArray1, wxCB_DROPDOWN|wxCB_READONLY );
    equalityComboBox = new wxComboBox( parent, ID_COMBO, wxT(strArray2[0]), wxDefaultPosition, wxSize(50,-1), numOfEqualities, strArray2, wxCB_DROPDOWN|wxCB_READONLY );
    //condTypeComboBox = new wxComboBox( parent, ID_COMBO, wxT(strArray3[0]), wxDefaultPosition, wxSize(200,-1), 3, strArray3, wxCB_DROPDOWN|wxCB_READONLY );
    
    // wx*Sizer    
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
    wxFlexGridSizer *item1 = new wxFlexGridSizer(4, 0, 0 );
    wxBoxSizer *item2 = new wxBoxSizer( wxHORIZONTAL );
    
    wxStaticBox *item3 = new wxStaticBox( parent, -1, wxT("Propagators and Spacecraft") );
    wxStaticBox *item4 = new wxStaticBox( parent, -1, wxT("Stopping Conditions") );
    wxStaticBox *item5 = new wxStaticBox( parent, -1, wxT("Stopping Condition Details") );
    
    wxStaticBoxSizer *item6 = new wxStaticBoxSizer( item3, wxVERTICAL );
    wxStaticBoxSizer *item7 = new wxStaticBoxSizer( item4, wxVERTICAL );
    wxStaticBoxSizer *item8 = new wxStaticBoxSizer( item5, wxVERTICAL );    
    
    wxBoxSizer *item9 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *item10 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *item11 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *item12 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *item13 = new wxBoxSizer( wxHORIZONTAL );
    
    // Add to wx*Sizers     
    item1->Add( 200, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item1->Add( 200, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item1->Add( 200, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item1->Add( scriptButton, 0, wxALIGN_RIGHT|wxALL, 5 );
    
    item6->Add( synchStaticText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    item6->Add( synchComboBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );    
    item6->Add( propGrid, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    
    item10->Add( nameStaticText, 0, wxALIGN_LEFT|wxALL, 5 );
    item10->Add( nameTextCtrl, 0, wxALIGN_LEFT|wxALL, 5 );
//      item10->Add( nameStaticText, 0, wxALIGN_CENTRE|wxALL, 5 ); //loj: 3/24/04
//      item10->Add( nameTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    item10->Add( 75, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    item11->Add( varStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    item11->Add( variableTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
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
    
    item8->Add( item9, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    item7->Add( stopCondGrid, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    item7->Add( item8, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    item2->Add( okButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    item2->Add( applyButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    item2->Add( cancelButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    item2->Add( helpButton, 0, wxALIGN_CENTRE|wxALL, 5 );

    theMiddleBoxSizer->Add( item6, 0, wxGROW|wxALIGN_CENTER|wxALL, 5 );
    theMiddleBoxSizer->Add( item7, 0, wxGROW|wxALIGN_CENTER|wxALL, 5 );
    
    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    item0->Add( theMiddleBoxSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, 5 );
    item0->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );

    
    parent->SetAutoLayout( true );
    parent->SetSizer( item0 );
    
    item0->Fit( parent );
    item0->SetSizeHints( parent );
    
    // waw: Future Implementation
    synchComboBox->Enable(false);
    helpButton->Enable(false);
    deleteButton->Enable(false);
    equalityComboBox->Enable(false);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void PropagateCommandPanel::LoadData()
{
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
    
    //loj: 3/2/04 commented out
//      newParamName = variableTextCtrl->GetValue(); 
//      SingleValueStop *newStop = (SingleValueStop *)theStopCondBase;
//      newStop->SetName((nameTextCtrl->GetValue().c_str()));
//      newStop->SetSingleParameter(theGuiInterpreter->GetParameter(newParamName.c_str()));
//      newStop->SetGoal(atof(goalTextCtrl->GetValue()));
//      newStop->SetRepeatCount(atoi(repeatTextCtrl->GetValue().c_str()));
//      newStop->SetTolerance(atof(toleranceTextCtrl->GetValue().c_str()));  
//      thePropagateCommand->SetObject(newStop, Gmat::STOP_CONDITION);
}

//------------------------------------------------------------------------------
// void DisplayPropagator()
//------------------------------------------------------------------------------
void PropagateCommandPanel::DisplayPropagator()
{
    //MessageInterface::ShowMessage("PropagateCommandPanel::DisplayPropagator() entered\n");
    
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
    //MessageInterface::ShowMessage("PropagateCommandPanel::DisplayStopCondition() entered\n");
    
    //loj 3/2/04 added
    //----- for stopCondGrid - show all
    for (int i=0; i<numOfStopCond; i++)
    {
        stopCondGrid->SetCellValue(i, STOPCOND_NAME_COL, tempStopCond[i].name);
        stopCondGrid->SetCellValue(i, STOPCOND_DESC_COL, tempStopCond[i].desc);        
    }

    //----- for detailed stop condition - show first row
    nameTextCtrl->AppendText(tempStopCond[0].name);
    variableTextCtrl->SetValue(tempStopCond[0].varName);
    equalityComboBox->SetSelection(equalityComboBox->FindString(tempStopCond[0].relOpStr));
    repeatTextCtrl->SetValue(wxVariant((long)(tempStopCond[0].repeat)));
    goalTextCtrl->SetValue(wxVariant(tempStopCond[0].goal));
    toleranceTextCtrl->SetValue(wxVariant(tempStopCond[0].tol));
    
    stopCondGrid->SelectRow(0);
    variableTextCtrl->Disable();

    //loj: 3/2/04 commented out
//      if ( theStopCond != NULL )
//      {
//          ParameterPtrArray theParams = theStopCond->GetParameters();
        
//          nameTextCtrl->AppendText(wxT(theStopCond->GetName().c_str()));
//          variableTextCtrl->AppendText(theParams[numOfStopCond-1]->GetName().c_str());
//          goalTextCtrl->AppendText( wxVariant(theStopCond->GetGoal()) );
//          repeatTextCtrl->SetValue( wxVariant((long)(theStopCond->GetRepeatCount())) );
//          toleranceTextCtrl->SetValue( wxVariant(theStopCond->GetTolerance()) );
        
//          wxString name = nameTextCtrl->GetValue();
//          stopCondGrid->SetCellValue( (numOfStopCond-1), 0, wxT(name) );
        
//          wxString cond = variableTextCtrl->GetValue();
//          cond.Append(" ");
//          cond.Append(equalityComboBox->GetStringSelection());
//          cond.Append(" ");
//          cond.Append(goalTextCtrl->GetValue());
//          stopCondGrid->SetCellValue( (numOfStopCond-1), 1, wxT(cond) );

//      }
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
    tempStopCond[row].name = nameTextCtrl->GetValue();
    tempStopCond[row].varName = variableTextCtrl->GetValue();
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

    //loj: 3/2/04 commented out
//      //waw: Temp, to be deleted later
    
//      wxString name = nameTextCtrl->GetValue();
//      if (!name.IsEmpty())
//          stopCondGrid->SetCellValue( rows[0], 0, wxT(name) );
        
//      wxString cond = variableTextCtrl->GetValue();
//      cond.Append(" ");
//      cond.Append(equalityComboBox->GetStringSelection());
//      cond.Append(" ");
//      cond.Append(goalTextCtrl->GetValue());
//      if (!cond.IsEmpty())
//          stopCondGrid->SetCellValue( rows[0], 1, wxT(cond) );
        
    updateButton->Enable(false);
    applyButton->Enable(true);
    
    /* waw: Part of build 3
    numOfStopCond++;
    
    wxString name = nameTextCtrl->GetValue();
    if (!name.IsEmpty())
        stopCondGrid->SetCellValue( (numOfStopCond-1), 0, wxT(name) );
        
    wxString cond = variableTextCtrl->GetValue();
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
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagateCommandPanel::OnTextUpdate(wxCommandEvent& event)
{
    //loj: assume single selection
    wxArrayInt rows = stopCondGrid->GetSelectedRows();
    int row = rows[0];

    tempStopCond[row].isChanged = true;
    updateButton->Enable(true);
    //applyButton->Enable(true); //loj: why this was commented?
}

void PropagateCommandPanel::OnTextMaxLen(wxCommandEvent& event)
{
    wxLogMessage(_T("You can't enter more characters into this control."));
}

void PropagateCommandPanel::OnComboSelection(wxCommandEvent& event)
{
    //loj: assume single selection
    wxArrayInt rows = stopCondGrid->GetSelectedRows();
    int row = rows[0];
    
    if ( event.GetEventObject() == synchComboBox )
    {
        applyButton->Enable(true);
    }
    else if ( event.GetEventObject() == equalityComboBox )
    {
        wxString str = FormatStopCondDesc(tempStopCond[row].varName,
                                          tempStopCond[row].relOpStr,
                                          tempStopCond[row].goal);
        tempStopCond[row].desc = str;
        tempStopCond[row].isChanged = true;
        updateButton->Enable(true);
        applyButton->Enable(true);
    }
    else
        event.Skip();
}

void PropagateCommandPanel::OnButton(wxCommandEvent& event)
{
    if ( event.GetEventObject() == scriptButton )  
    {
        //CreateScript();
        //applyButton->Enable(true);
    }
    else if ( event.GetEventObject() == viewButton ) //loj: 2/25 added for testing ParameterSelectDialog
    {
        // show dialog to select parameter
        ParameterSelectDialog paramDlg(this);
        paramDlg.ShowModal();

        if (paramDlg.IsParamSelected())
        {
            newParamName = paramDlg.GetParamName();
            variableTextCtrl->SetValue(newParamName);
        }
        updateButton->Enable(true);
    }
    else if ( event.GetEventObject() == updateButton )
    {
        UpdateStopCondition();
        applyButton->Enable(true);
    }
    else if ( event.GetEventObject() == okButton )  
    {
        //loj: 2/11/04 added if block
        if (applyButton->IsEnabled())
        {
            SaveData();     
        }
        GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
        gmatMainNotebook->ClosePage();
    }
    else if ( event.GetEventObject() == applyButton )
    {
        SaveData();
        applyButton->Enable(false);
    }       
    else if ( event.GetEventObject() == cancelButton )
    {
        GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
        gmatMainNotebook->ClosePage();
    } 
    else if ( event.GetEventObject() == helpButton )           
        ; 
    else
        event.Skip();
}

//loj: 3/1/04 commented out for build2
//  //------------------------------------------------------------------------------
//  // wxMenuBar* CreateScriptWindowMenu(const std::string &docType)
//  //------------------------------------------------------------------------------
//  wxMenuBar* PropagateCommandPanel::CreateScriptWindowMenu(const std::string &docType)
//  {
//      // Make a menubar
//      wxMenu *fileMenu = new wxMenu;
//      wxMenu *editMenu = (wxMenu *) NULL;
    
//      fileMenu->Append(wxID_NEW, _T("&New..."));
//      fileMenu->Append(wxID_OPEN, _T("&Open..."));

//      if (docType == "sdi")
//      {
//          fileMenu->Append(wxID_CLOSE, _T("&Close"));
//          fileMenu->Append(wxID_SAVE, _T("&Save"));
//          fileMenu->Append(wxID_SAVEAS, _T("Save &As..."));
//          fileMenu->AppendSeparator();
//          fileMenu->Append(wxID_PRINT, _T("&Print..."));
//          fileMenu->Append(wxID_PRINT_SETUP, _T("Print &Setup..."));
//          fileMenu->Append(wxID_PREVIEW, _T("Print Pre&view"));
    
//          editMenu = new wxMenu;
//          editMenu->Append(wxID_UNDO, _T("&Undo"));
//          editMenu->Append(wxID_REDO, _T("&Redo"));
//          editMenu->AppendSeparator();
//          //editMenu->Append(DOCVIEW_CUT, _T("&Cut last segment"));
    
//          docMainFrame->editMenu = editMenu;
//          fileMenu->AppendSeparator();
//      }
    
//      fileMenu->Append(wxID_EXIT, _T("E&xit"));
    
//      // A nice touch: a history of files visited. Use this menu.
//      mDocManager->FileHistoryUseMenu(fileMenu);
    
//      //wxMenu *help_menu = new wxMenu;
//      //help_menu->Append(DOCVIEW_ABOUT, _T("&About"));
    
//      wxMenuBar *menuBar = new wxMenuBar;
    
//      menuBar->Append(fileMenu, _T("&File"));
    
//      if (editMenu)
//          menuBar->Append(editMenu, _T("&Edit"));
    
//      //menuBar->Append(help_menu, _T("&Help"));

//      return menuBar;
//  }

//loj: 3/1/04 commented out for build2
//  void PropagateCommandPanel::CreateScript()
//  {
//      //not MAC mode
//      //----------------------------------------------------------------
//  /* **** NOTE:  it is temporary until it is fixed for Mac  ***************
//  #if !defined __WXMAC__
//  ******************************* */
//  #if 1 
//      // Create a document manager
//      mDocManager = new wxDocManager;

//      // Create a template relating text documents to their views
//      mDocTemplate = 
//          new wxDocTemplate(mDocManager, _T("Text"), _T("*.script"),
//                            _T(""), _T("script"), _T("Text Doc"), _T("Text View"),
//                            CLASSINFO(TextDocument), CLASSINFO(MdiTextEditView));
    
//      // Create the main frame window    
//      mdiDocMainFrame =
//          new MdiDocViewFrame(mDocManager, mdiDocMainFrame, _T("Script Window (MDI)"),
//                              wxPoint(0, 0), wxSize(600, 500),
//                              (wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE));
    
//      // Give it an icon (this is ignored in MDI mode: uses resources)
//      mdiDocMainFrame->SetIcon(wxIcon(_T("doc")));
    
//      // Make a menubar
//      wxMenuBar *menuBar = CreateScriptWindowMenu("mdi");
       
//      // Associate the menu bar with the frame
//      mdiDocMainFrame->SetMenuBar(menuBar);
    
//      mdiDocMainFrame->Centre(wxBOTH);
//      mdiDocMainFrame->Show(TRUE);
//      //----------------------------------------------------------------
//  #else
//      // Create a document manager
//      mDocManager = new wxDocManager;

//      // Create a template relating text documents to their views
//      mDocTemplate = 
//          new wxDocTemplate(mDocManager, _T("Text"), _T("*.script"),
//                            _T(""), _T("script"), _T("Text Doc"), _T("Text View"),
//                            CLASSINFO(TextDocument), CLASSINFO(TextEditView));
    
//      // Create the main frame window    
//      docMainFrame =
//          new DocViewFrame(mDocManager, this, -1, _T("Script Window"),
//                           wxPoint(0, 0), wxSize(600, 500), wxDEFAULT_FRAME_STYLE);
        
//      // Make a menubar
//      wxMenuBar *menuBar = CreateScriptWindowMenu("sdi");
       
//      // Associate the menu bar with the frame
//      docMainFrame->SetMenuBar(menuBar);
    
//      docMainFrame->Centre(wxBOTH);
//      docMainFrame->Show(TRUE);
//  #endif
//  }

//loj: 3/1/04 added for build2
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
    //loj: 3/1/04 added new code
    
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
                    applyButton->Enable(true);
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
                    applyButton->Enable(true);
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
    nameTextCtrl->SetValue("");
    variableTextCtrl->SetValue("");
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
    
    nameTextCtrl->SetValue(tempStopCond[row].name);
    variableTextCtrl->SetValue(tempStopCond[row].varName);
    equalityComboBox->SetSelection(equalityComboBox->FindString(tempStopCond[row].relOpStr));
    repeatTextCtrl->SetValue(wxVariant((long)(tempStopCond[row].repeat)));
    goalTextCtrl->SetValue(wxVariant(tempStopCond[row].goal));
    toleranceTextCtrl->SetValue(wxVariant(tempStopCond[row].tol));
    
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
