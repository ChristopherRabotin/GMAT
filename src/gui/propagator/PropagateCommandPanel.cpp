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
#include <wx/docview.h>
#include <wx/menu.h>
#include <wx/variant.h>

#include "gmatwxdefs.hpp"
#include "ViewTextFrame.hpp"
#include "DocViewFrame.hpp"
#include "TextEditView.hpp"
#include "TextDocument.hpp"
#include "MdiTextEditView.hpp"
#include "MdiDocViewFrame.hpp"
#include "GmatMainNotebook.hpp"
#include "PropagateCommandPanel.hpp"
#include "ParameterSelectDialog.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"
#include "Command.hpp"
#include "MessageInterface.hpp"
#include "SingleValueStop.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(PropagateCommandPanel, wxPanel)
    EVT_BUTTON(ID_BUTTON, PropagateCommandPanel::OnButton)
    EVT_COMBOBOX(ID_COMBO, PropagateCommandPanel::OnComboSelection)
    EVT_TEXT_ENTER(ID_TEXTCTRL, PropagateCommandPanel::OnTextUpdate)
    EVT_TEXT_MAXLEN(ID_TEXTCTRL, PropagateCommandPanel::OnTextMaxLen)
//    EVT_GRID_CELL_LEFT_CLICK(PropagateCommandPanel::OnCellLeftClick)
//    EVT_GRID_CELL_RIGHT_CLICK(PropagateCommandPanel::OnCellRightClick)
//    EVT_GRID_CELL_LEFT_DCLICK(PropagateCommandPanel::OnCellLeftDoubleClick)
//    EVT_GRID_CELL_CHANGE(PropagateCommandPanel::OnCellValueChanged)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// PropagateCommandPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
//loj: 2/9/04 PropagateCommandPanel::PropagateCommandPanel( wxWindow *parent, const wxString &propName )
PropagateCommandPanel::PropagateCommandPanel( wxWindow *parent, const wxString &propName,
                                              GmatCommand *cmd )
    : wxPanel(parent)
{
    propNameString = propName;
    theCommand = cmd;
    
    // Default values
    numOfStopCond = 0;
    numOfProp = 0;
    numOfModes = 1;
    numOfPropRows = 10;
    numOfCondRows = 20;
    numOfEqualities = 6;
    
    Setup(this);
    
    if (theCommand != NULL)
    {
        Initialize();    
        GetData();
    }
    
    applyButton->Enable(false); //loj: 2/11/04 added
    updateButton->Enable(false);
}

void PropagateCommandPanel::Initialize()
{  
    theGuiInterpreter = GmatAppData::GetGuiInterpreter();
    thePropagateCommand = (Propagate *)theCommand;
    propID = thePropagateCommand->GetParameterID("Propagator");
    propSetupName = thePropagateCommand->GetStringParameter(propID);
    numOfProp = 1;  // waw: TBD

    scID = thePropagateCommand->GetParameterID("Spacecraft");
    scList = thePropagateCommand->GetStringArrayParameter(scID);
    numOfSC = scList.size();
    
    theStopCondBase = thePropagateCommand->GetObject(Gmat::STOP_CONDITION);
    theStopCond = (StopCondition *)theStopCondBase; 
    numOfStopCond = 1;  // temp value      
    
    /*  Waw:
        Display all the stop. cond. elements 
        Methods for this are currently not avaiable.
        
    numOfStopCond = thePropagateCommand->GetStopConditionSize();
    theStopCondVector = thePropagateCommand->GetObject(GMAT::STOP_CONDITION);
    */
}

void PropagateCommandPanel::Setup( wxWindow *parent)
{
    //loj: 2/13/04 added
    wxBoxSizer *theMiddleBoxSizer = new wxBoxSizer(wxVERTICAL);
    
    // wxGrid
    //loj: 2/13/04 changed wxSize(150,160) to wxSize(100,160) to match with stopCond width
    propGrid = new wxGrid( parent, ID_GRID, wxDefaultPosition, wxSize(100,160), wxWANTS_CHARS );
    propGrid->CreateGrid( numOfPropRows, 2, wxGrid::wxGridSelectRows );
    propGrid->SetColSize(0, 200);
    //loj: 2/13/04 changed SetColSize(1, 585) to SetColSize(1, 460)
    propGrid->SetColSize(1, 500);
    propGrid->SetColLabelValue(0, _T("Propagator"));
    propGrid->SetColLabelValue(1, _T("Spacecraft List"));
    propGrid->SetRowLabelSize(0);
    propGrid->EnableEditing(false);
    
    stopCondGrid = new wxGrid( parent, ID_GRID, wxDefaultPosition, wxSize(100,160), wxWANTS_CHARS );
    stopCondGrid->CreateGrid( numOfCondRows, 2, wxGrid::wxGridSelectRows );
    stopCondGrid->SetColSize(0, 200);
    stopCondGrid->SetColSize(1, 500);
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
    wxString strArray3[] =
    {
        wxT("Single Value Stop"),
        wxT("Apoapsis Stop"),
        wxT("Periapsis Stop"),
    };
    
    // wxStaticText
    synchStaticText = new wxStaticText( parent, ID_TEXT, wxT("Synchronization Mode"), wxDefaultPosition, wxDefaultSize, 0 );
    nameStaticText = new wxStaticText( parent, ID_TEXT, wxT("Name"), wxDefaultPosition, wxDefaultSize, 0 );
    varStaticText = new wxStaticText( parent, ID_TEXT, wxT("Variable"), wxDefaultPosition, wxDefaultSize, 0 );
    repeatStaticText = new wxStaticText( parent, ID_TEXT, wxT("Repeat"), wxDefaultPosition, wxDefaultSize, 0 );
    tolStaticText = new wxStaticText( parent, ID_TEXT, wxT("Tolerance"), wxDefaultPosition, wxDefaultSize, 0 );
    condTypeStaticText = new wxStaticText( parent, ID_TEXT, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxTextCtrl
    nameTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(250,-1), 0 );
    variableTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(250,-1), 0 );
    valueTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
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
    condTypeComboBox = new wxComboBox( parent, ID_COMBO, wxT(strArray3[0]), wxDefaultPosition, wxSize(200,-1), 3, strArray3, wxCB_DROPDOWN|wxCB_READONLY );
    
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
    
    item10->Add( nameStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    item10->Add( nameTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    item10->Add( condTypeStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    item10->Add( condTypeComboBox, 0, wxALIGN_CENTRE|wxALL, 5 );
    item10->Add( 75, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    //item10->Add( 75, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    //item10->Add( 75, 20, 0, wxALIGN_CENTRE|wxALL, 5 );    
    
    item11->Add( varStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    item11->Add( variableTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    item11->Add( viewButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    item11->Add( equalityComboBox, 0, wxALIGN_CENTRE|wxALL, 5 );
    item11->Add( valueTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    
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
    item0->Add( theMiddleBoxSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, 5 ); //loj: added
    //loj: 2/13/04 item0->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    //loj: 2/13/04 item0->Add( item7, 0, wxALIGN_CENTRE|wxALL, 5 );
    //loj: 2/13/04 item0->Add( item2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
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
    condTypeComboBox->Enable(false);
}

void PropagateCommandPanel::GetData()
{
    DisplayPropagator();
    DisplayStopCondition();
}

void PropagateCommandPanel::SetData()
{     
    // Saving propagator
    Integer id = thePropagateCommand->GetParameterID("ElapsedSeconds");
    thePropagateCommand->SetRealParameter(id, atof(valueTextCtrl->GetValue()));
    
    // Saving stopping condition  
    newParamName = variableTextCtrl->GetValue(); 
    SingleValueStop *newStop = (SingleValueStop *)theStopCondBase; 
    newStop->SetSingleParameter(theGuiInterpreter->GetParameter(newParamName.c_str()));
    thePropagateCommand->SetObject(newStop, Gmat::STOP_CONDITION);
    newStop->SetGoal(atof(valueTextCtrl->GetValue()));
    newStop->SetRepeatCount(atoi(repeatTextCtrl->GetValue().c_str()));
    newStop->SetTolerance(atof(toleranceTextCtrl->GetValue().c_str()));  
}

void PropagateCommandPanel::DisplayPropagator()
{
    propGrid->SetCellValue( 0, 0, wxT(propSetupName.c_str()) );
    
    if ( !scList.empty() )
    {
        /* waw: Future implementation
        for (int i = 0; i < numOfSC; i++)
        {
            scListString.Append(scList.at(i).c_str());
            scListString.Append(" ");
        }*/
        scListString.Append(scList.at(0).c_str());    
        propGrid->SetCellValue( 0, 1, scListString );
    }
}

// Called only once when window is initially displayed
void PropagateCommandPanel::DisplayStopCondition()
{
    if ( theStopCond != NULL )
    {
        ParameterPtrArray theParams = theStopCond->GetParameters();
        
        nameTextCtrl->AppendText(wxT(theStopCond->GetName().c_str()));
        variableTextCtrl->AppendText(theParams[numOfStopCond-1]->GetName().c_str());
        valueTextCtrl->AppendText( wxVariant(theStopCond->GetGoal()) );
        repeatTextCtrl->SetValue( wxVariant((long)(theStopCond->GetRepeatCount())) );
        toleranceTextCtrl->SetValue( wxVariant(theStopCond->GetTolerance()) );
        
        wxString name = nameTextCtrl->GetValue();
        stopCondGrid->SetCellValue( (numOfStopCond-1), 0, wxT(name) );
        
        wxString cond = variableTextCtrl->GetValue();
        cond.Append(" ");
        cond.Append(equalityComboBox->GetStringSelection());
        cond.Append(" ");
        cond.Append(valueTextCtrl->GetValue());
        stopCondGrid->SetCellValue( (numOfStopCond-1), 1, wxT(cond) );
    }
}

// Called when new stop cond is created
void PropagateCommandPanel::UpdateStopCondition()
{
    //waw: Temp, to be deleted later  
    wxString name = nameTextCtrl->GetValue();
    if (!name.IsEmpty())
        stopCondGrid->SetCellValue( (0), 0, wxT(name) );
        
    wxString cond = variableTextCtrl->GetValue();
    cond.Append(" ");
    cond.Append(equalityComboBox->GetStringSelection());
    cond.Append(" ");
    cond.Append(valueTextCtrl->GetValue());
    if (!cond.IsEmpty())
        stopCondGrid->SetCellValue( (0), 1, wxT(cond) );
        
    updateButton->Enable(false);
    
    /* waw: Part of build 3
    numOfStopCond++;
    
    wxString name = nameTextCtrl->GetValue();
    if (!name.IsEmpty())
        stopCondGrid->SetCellValue( (numOfStopCond-1), 0, wxT(name) );
        
    wxString cond = variableTextCtrl->GetValue();
    cond.Append(" ");
    cond.Append(equalityComboBox->GetStringSelection());
    cond.Append(" ");
    cond.Append(valueTextCtrl->GetValue());
    if (!cond.IsEmpty())
        stopCondGrid->SetCellValue( (numOfStopCond-1), 1, wxT(cond) );
        
    updateButton->Enable(false);
    */
}

void PropagateCommandPanel::OnTextUpdate(wxCommandEvent& event)
{
    updateButton->Enable(true);
    applyButton->Enable(true);
}

void PropagateCommandPanel::OnTextMaxLen(wxCommandEvent& event)
{
    wxLogMessage(_T("You can't enter more characters into this control."));
}

void PropagateCommandPanel::OnComboSelection(wxCommandEvent& event)
{                
    if ( event.GetEventObject() == condTypeComboBox )
    {
        wxString type = condTypeComboBox->GetStringSelection();
        
        if (type.Cmp("Single Value Stop") == 0)
        {
            varStaticText->Enable(true);
            variableTextCtrl->Enable(true);
            viewButton->Enable(true);
            equalityComboBox->Enable(true);
            valueTextCtrl->Enable(true);            
        }
        else
        {
            varStaticText->Enable(false);
            variableTextCtrl->Enable(false);
            viewButton->Enable(false);
            equalityComboBox->Enable(false);
            valueTextCtrl->Enable(false);   
        }
        applyButton->Enable(true);
    } 
    else if ( event.GetEventObject() == synchComboBox )           
        applyButton->Enable(true); 
    else if ( event.GetEventObject() == equalityComboBox )           
        updateButton->Enable(true);
    else
        event.Skip();
}

void PropagateCommandPanel::OnButton(wxCommandEvent& event)
{
    if ( event.GetEventObject() == scriptButton )  
    {       
        CreateScript();
        applyButton->Enable(true);
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
            SetData();     
        }
        GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
        gmatMainNotebook->ClosePage();
    }
    else if ( event.GetEventObject() == applyButton )
    {
        SetData();
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

//------------------------------------------------------------------------------
// wxMenuBar* CreateScriptWindowMenu(const std::string &docType)
//------------------------------------------------------------------------------
wxMenuBar* PropagateCommandPanel::CreateScriptWindowMenu(const std::string &docType)
{
    // Make a menubar
    wxMenu *fileMenu = new wxMenu;
    wxMenu *editMenu = (wxMenu *) NULL;
    
    fileMenu->Append(wxID_NEW, _T("&New..."));
    fileMenu->Append(wxID_OPEN, _T("&Open..."));

    if (docType == "sdi")
    {
        fileMenu->Append(wxID_CLOSE, _T("&Close"));
        fileMenu->Append(wxID_SAVE, _T("&Save"));
        fileMenu->Append(wxID_SAVEAS, _T("Save &As..."));
        fileMenu->AppendSeparator();
        fileMenu->Append(wxID_PRINT, _T("&Print..."));
        fileMenu->Append(wxID_PRINT_SETUP, _T("Print &Setup..."));
        fileMenu->Append(wxID_PREVIEW, _T("Print Pre&view"));
    
        editMenu = new wxMenu;
        editMenu->Append(wxID_UNDO, _T("&Undo"));
        editMenu->Append(wxID_REDO, _T("&Redo"));
        editMenu->AppendSeparator();
        //editMenu->Append(DOCVIEW_CUT, _T("&Cut last segment"));
    
        docMainFrame->editMenu = editMenu;
        fileMenu->AppendSeparator();
    }
    
    fileMenu->Append(wxID_EXIT, _T("E&xit"));
    
    // A nice touch: a history of files visited. Use this menu.
    mDocManager->FileHistoryUseMenu(fileMenu);
    
    //wxMenu *help_menu = new wxMenu;
    //help_menu->Append(DOCVIEW_ABOUT, _T("&About"));
    
    wxMenuBar *menuBar = new wxMenuBar;
    
    menuBar->Append(fileMenu, _T("&File"));
    
    if (editMenu)
        menuBar->Append(editMenu, _T("&Edit"));
    
    //menuBar->Append(help_menu, _T("&Help"));

    return menuBar;
}

void PropagateCommandPanel::CreateScript()
{
    //not MAC mode
    //----------------------------------------------------------------
/* **** NOTE:  it is temporary until it is fixed for Mac  ***************
#if !defined __WXMAC__
******************************* */
#if 1 
    // Create a document manager
    mDocManager = new wxDocManager;

    // Create a template relating text documents to their views
    mDocTemplate = 
        new wxDocTemplate(mDocManager, _T("Text"), _T("*.script"),
                          _T(""), _T("script"), _T("Text Doc"), _T("Text View"),
                          CLASSINFO(TextDocument), CLASSINFO(MdiTextEditView));
    
    // Create the main frame window    
    mdiDocMainFrame =
        new MdiDocViewFrame(mDocManager, mdiDocMainFrame, _T("Script Window (MDI)"),
                            wxPoint(0, 0), wxSize(600, 500),
                            (wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE));
    
    // Give it an icon (this is ignored in MDI mode: uses resources)
    mdiDocMainFrame->SetIcon(wxIcon(_T("doc")));
    
    // Make a menubar
    wxMenuBar *menuBar = CreateScriptWindowMenu("mdi");
       
    // Associate the menu bar with the frame
    mdiDocMainFrame->SetMenuBar(menuBar);
    
    mdiDocMainFrame->Centre(wxBOTH);
    mdiDocMainFrame->Show(TRUE);
    //----------------------------------------------------------------
#else
    // Create a document manager
    mDocManager = new wxDocManager;

    // Create a template relating text documents to their views
    mDocTemplate = 
        new wxDocTemplate(mDocManager, _T("Text"), _T("*.script"),
                          _T(""), _T("script"), _T("Text Doc"), _T("Text View"),
                          CLASSINFO(TextDocument), CLASSINFO(TextEditView));
    
    // Create the main frame window    
    docMainFrame =
        new DocViewFrame(mDocManager, this, -1, _T("Script Window"),
                         wxPoint(0, 0), wxSize(600, 500), wxDEFAULT_FRAME_STYLE);
        
    // Make a menubar
    wxMenuBar *menuBar = CreateScriptWindowMenu("sdi");
       
    // Associate the menu bar with the frame
    docMainFrame->SetMenuBar(menuBar);
    
    docMainFrame->Centre(wxBOTH);
    docMainFrame->Show(TRUE);
#endif
}

/*  waw: OnCellLeftClick() is completed for build 3
void PropagateCommandPanel::OnCellLeftClick(wxGridEvent& event)
{        
    currentRowStopCond = event.GetRow();
    
    if ( (currentRowStopCond+1) <= numOfStopCond )
    {
        wxString string = stopCondGrid->GetCellValue(currentRowStopCond, 0);
          
        if ( !string.IsEmpty() )
        {
            nameTextCtrl->Clear();
            variableTextCtrl->Clear();
            valueTextCtrl->Clear();
            repeatTextCtrl->Clear();
            toleranceTextCtrl->Clear();
            
            repeatTextCtrl->SetValue( wxVariant((long)(theStopCond->GetRepeatCount())) );
            toleranceTextCtrl->SetValue( wxVariant(theStopCond->GetTolerance()) ); 
            
            nameTextCtrl->AppendText(stopCondGrid->GetCellValue(currentRowStopCond, 0));
            
            char c = ' ';
            wxString var = stopCondGrid->GetCellValue(currentRowStopCond, 1);
            variableTextCtrl->AppendText(var.BeforeFirst(c));  
            valueTextCtrl->AppendText(var.AfterLast(c));
            
            wxString eq =  var.AfterFirst(c);
            wxString eqVal = eq.BeforeFirst(c);
            
            int pos = equalityComboBox->FindString(eqVal);
            equalityComboBox->SetSelection(pos);            
        }
    }
    else if ( (currentRowStopCond+1) > numOfStopCond )
    {
        nameTextCtrl->Clear();
        variableTextCtrl->Clear();
        valueTextCtrl->Clear();
        
        nameTextCtrl->AppendText("");
        variableTextCtrl->AppendText("");
        valueTextCtrl->AppendText("");       
    }
}

void PropagateCommandPanel::ShowContextMenu(const wxPoint& pos)
{
    wxMenu propMenu(_T("Test popup"));
    
    propMenu.Append(MENU_INSERT_P, wxT("Insert"), wxT(""), wxITEM_NORMAL);
    propMenu.Append(MENU_DELETE_P, wxT("Delete"), wxT(""), wxITEM_NORMAL);
    propMenu.Append(MENU_CLEAR_P, wxT("Clear"), wxT(""), wxITEM_NORMAL);
    
    PopupMenu(&propMenu, pos.x, pos.y);  
}

void PropagateCommandPanel::OnCellRightClick(wxGridEvent& ev, wxMouseEvent& event)
{
    // if ( numOfStopCond < event.GetRow() )
    //ev.GetCol();

    // you must call event skip if you want default grid processing
    // (cell highlighting etc.)
    
    if ( ev.GetEventObject() == propGrid ) 
    {
        ShowContextMenu(ScreenToClient(event.GetPosition()));          
        applyButton->Enable(true);
    }
    else if ( ev.GetEventObject() == stopCondGrid )  
    {
        ShowContextMenu(ScreenToClient(event.GetPosition()));  
        applyButton->Enable(true);
    }        
}

void PropagateCommandPanel::OnCellLeftDoubleClick(wxGridEvent& event)
{

}

void PropagateCommandPanel::OnCellValueChanged(wxGridEvent& event)
{ 
    if ( event.GetEventObject() == propGrid )           
        applyButton->Enable(TRUE);
    else if ( event.GetEventObject() == stopCondGrid )  
        applyButton->Enable(TRUE);
}

void PropagateCommandPanel::InsertPropagatorRow()
{
    propGrid->InsertRows(propGrid->GetGridCursorRow(), 1);
    applyButton->Enable(TRUE);
}

void PropagateCommandPanel::InsertStopCondRow()
{
    stopCondGrid->InsertRows(stopCondGrid->GetGridCursorRow(), 1);
    applyButton->Enable(TRUE);
}

void PropagateCommandPanel::ClearPropagatorTable()
{
        propGrid->ClearGrid();
        applyButton->Enable(TRUE);
}

void PropagateCommandPanel::ClearStopCondTable()
{
    stopCondGrid->ClearGrid();
    applyButton->Enable(TRUE);
}

void PropagateCommandPanel::DeleteSelectedPropagatorRows()
{
    if (propGrid->IsSelection() )
    {
        propGrid->BeginBatch();
        for ( int n = 0; n < propGrid->GetNumberRows(); )
            if (propGrid->IsInSelection( n , 0 ) )
                propGrid->DeleteRows( n, 1 );
                        else
                                n++;
        propGrid->EndBatch();
    }
    applyButton->Enable(TRUE);
}

void PropagateCommandPanel::DeleteSelectedStopCondRows()
{
    if (stopCondGrid->IsSelection() )
    {
        stopCondGrid->BeginBatch();
        for ( int n = 0; n < propGrid->GetNumberRows(); )
            if (stopCondGrid->IsInSelection( n , 0 ) )
                stopCondGrid->DeleteRows( n, 1 );
                        else
                                n++;
        stopCondGrid->EndBatch();
    }
    applyButton->Enable(true);
}
*/
