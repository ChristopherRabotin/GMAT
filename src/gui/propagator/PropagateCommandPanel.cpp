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

#include "gmatwxdefs.hpp"
#include "ViewTextFrame.hpp"
#include "DocViewFrame.hpp"
#include "TextEditView.hpp"
#include "TextDocument.hpp"
#include "MdiTextEditView.hpp"
#include "MdiDocViewFrame.hpp"
#include "PropagateCommandPanel.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"
#include "Command.hpp"
#include "Propagate.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(PropagateCommandPanel, wxPanel)
    EVT_BUTTON(ID_BUTTON, PropagateCommandPanel::OnButton)
    EVT_COMBOBOX(ID_COMBO, PropagateCommandPanel::OnComboSelection)
    EVT_TEXT(ID_TEXTCTRL, PropagateCommandPanel::OnTextUpdate)
    EVT_TEXT_MAXLEN(ID_TEXTCTRL, PropagateCommandPanel::OnTextMaxLen)
    EVT_GRID_CELL_RIGHT_CLICK(PropagateCommandPanel::OnCellRightClick)
    EVT_GRID_CELL_LEFT_CLICK(PropagateCommandPanel::OnCellLeftClick)
    EVT_GRID_CELL_LEFT_DCLICK(PropagateCommandPanel::OnCellLeftDoubleClick)
    EVT_GRID_CELL_CHANGE(PropagateCommandPanel::OnCellValueChanged)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// PropagateCommandPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
PropagateCommandPanel::PropagateCommandPanel( wxWindow *parent, const wxString &propName )
    : wxPanel(parent)
{
    propNameString = propName;

    //loj: moved from Initialize()
    // For testing
    GuiInterpreter *theGuiInterpreter = GmatAppData::GetGuiInterpreter(); 
    Command *theCommand = theGuiInterpreter->GetCommand(propName.c_str());
    //thePropagateCommand = theCommand->GetObject(Gmat::COMMAND, "Propagate");
    
    Initialize();
    Setup(this);
}

void PropagateCommandPanel::Initialize()
{
    numOfModes = 1;
    numOfProps = 10;
    numOfConds = 10;
    numOfEqualities = 6;
    
}

void PropagateCommandPanel::Setup( wxWindow *parent)
{   
    // wxGrid
    propGrid = new wxGrid( parent, ID_GRID, wxDefaultPosition, wxSize(180,160), wxWANTS_CHARS );
    propGrid->CreateGrid( numOfProps, 2, wxGrid::wxGridSelectCells );
    propGrid->SetColSize(0, 150);
    propGrid->SetColSize(1, 585);
    propGrid->SetColLabelValue(0, _T("Propagator"));
    propGrid->SetColLabelValue(1, _T("Spacecraft List"));
    propGrid->SetRowLabelSize(0);
    
    stopCondGrid = new wxGrid( parent, ID_GRID, wxDefaultPosition, wxSize(200,160), wxWANTS_CHARS );
    stopCondGrid->CreateGrid( numOfConds, 3, wxGrid::wxGridSelectCells );
    stopCondGrid->SetColSize(0, 150);
    stopCondGrid->SetColSize(1, 255);
    stopCondGrid->SetColSize(2, 255);
    stopCondGrid->SetColLabelValue(0, _T("Name"));
    stopCondGrid->SetColLabelValue(1, _T("Condition"));
    stopCondGrid->SetColLabelValue(2, _T("Description"));
    stopCondGrid->SetRowLabelSize(0);
    
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
    eventStaticText = new wxStaticText( parent, ID_TEXT, wxT("Event Name"), wxDefaultPosition, wxDefaultSize, 0 );
    synchStaticText = new wxStaticText( parent, ID_TEXT, wxT("Synchronization Mode"), wxDefaultPosition, wxDefaultSize, 0 );
    descStaticText = new wxStaticText( parent, ID_TEXT, wxT("Description"), wxDefaultPosition, wxDefaultSize, 0 );
    valueStaticText = new wxStaticText( parent, ID_TEXT, wxT("Value"), wxDefaultPosition, wxDefaultSize, 0 );
    varStaticText = new wxStaticText( parent, ID_TEXT, wxT("Variable"), wxDefaultPosition, wxDefaultSize, 0 );
    repeatStaticText = new wxStaticText( parent, ID_TEXT, wxT("Repeat"), wxDefaultPosition, wxDefaultSize, 0 );
    tolStaticText = new wxStaticText( parent, ID_TEXT, wxT("Tolerance"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxTextCtrl
    nameTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
    descriptionTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(300,-1), 0 );
    variableTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(300,-1), 0 );
    valueTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(300,-1), 0 );
    repeatTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    toleranceTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    
    // wxButton
    scriptButton = new wxButton( parent, ID_BUTTON, wxT("Create Script"), wxDefaultPosition, wxDefaultSize, 0 );
    editButton = new wxButton( parent, ID_BUTTON, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
    userDefButton = new wxButton( parent, ID_BUTTON, wxT("User Defined"), wxDefaultPosition, wxDefaultSize, 0 );
    okButton = new wxButton( parent, ID_BUTTON, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    applyButton = new wxButton( parent, ID_BUTTON, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    cancelButton = new wxButton( parent, ID_BUTTON, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    helpButton = new wxButton( parent, ID_BUTTON, wxT("Help"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxComboBox
    synchComboBox = new wxComboBox( parent, ID_COMBO, wxT(strArray1[0]), wxDefaultPosition, wxSize(-1,-1), numOfModes, strArray1, wxCB_DROPDOWN|wxCB_READONLY );
    equalityComboBox = new wxComboBox( parent, ID_COMBO, wxT(strArray2[0]), wxDefaultPosition, wxSize(-1,-1), numOfEqualities, strArray2, wxCB_DROPDOWN|wxCB_READONLY );
    
    // wx*Sizer    
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *item2 = new wxBoxSizer( wxHORIZONTAL );
    
    wxStaticBox *item3 = new wxStaticBox( parent, -1, wxT("Propagators and Spacecraft") );
    wxStaticBox *item4 = new wxStaticBox( parent, -1, wxT("Stopping Conditions") );
    wxStaticBox *item5 = new wxStaticBox( parent, -1, wxT("Stopping Condition Details") );
    
    wxStaticBoxSizer *item6 = new wxStaticBoxSizer( item3, wxVERTICAL );
    wxStaticBoxSizer *item7 = new wxStaticBoxSizer( item4, wxVERTICAL );
    wxStaticBoxSizer *item8 = new wxStaticBoxSizer( item5, wxVERTICAL );    
    
    wxFlexGridSizer *item9  = new wxFlexGridSizer( 5, 0, 1 );
    wxFlexGridSizer *item10 = new wxFlexGridSizer( 5, 0, 1 );
    wxFlexGridSizer *item11 = new wxFlexGridSizer( 5, 0, 1 );
    wxFlexGridSizer *item12 = new wxFlexGridSizer( 5, 0, 1 );
    
    // Add to wx*Sizers     
    item1->Add( eventStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    item1->Add( nameTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    item1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item1->Add( scriptButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    item6->Add( synchStaticText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    item6->Add( synchComboBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );    
    item6->Add( propGrid, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    
    item9->Add( descStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    item9->Add( descriptionTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    item9->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item9->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item9->Add( valueStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    item10->Add( varStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    item10->Add( variableTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    item10->Add( editButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    item10->Add( equalityComboBox, 0, wxALIGN_CENTRE|wxALL, 5 );
    item10->Add( valueTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );

    item11->Add( repeatStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    item11->Add( tolStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    item11->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item11->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item11->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    item12->Add( repeatTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( toleranceTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( userDefButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    item8->Add( item9, 0, wxALIGN_CENTRE|wxALL, 5 );
    item8->Add( item10, 0, wxALIGN_CENTRE|wxALL, 5 );
    item8->Add( item11, 0, wxALIGN_CENTRE|wxALL, 5 );
    item8->Add( item12, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    item7->Add( stopCondGrid, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    item7->Add( item8, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    item2->Add( okButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    item2->Add( applyButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    item2->Add( cancelButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    item2->Add( helpButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    item0->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    item0->Add( item7, 0, wxALIGN_CENTRE|wxALL, 5 );
    item0->Add( item2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    parent->SetAutoLayout( true );
    parent->SetSizer( item0 );
    
    item0->Fit( parent );
    item0->SetSizeHints( parent );
    
    // waw: Future Implementation
    userDefButton->Enable(false);
    helpButton->Enable(false);
    
    applyButton->Enable(false);
    
    //GetData();
}

void PropagateCommandPanel::GetData()
{
    nameTextCtrl->AppendText(propNameString);
}

void PropagateCommandPanel::SetData()
{

}

void PropagateCommandPanel::OnTextUpdate(wxCommandEvent& event)
{
    if ( event.GetEventObject() == nameTextCtrl )         
        applyButton->Enable(true);
    else if ( event.GetEventObject() == valueTextCtrl )           
        applyButton->Enable(true);
    else if ( event.GetEventObject() == repeatTextCtrl )
        applyButton->Enable(true);
    else if ( event.GetEventObject() == toleranceTextCtrl )
        applyButton->Enable(true);
    else if ( event.GetEventObject() == descriptionTextCtrl )
        applyButton->Enable(true);
    else
        event.Skip();
}

void PropagateCommandPanel::OnTextMaxLen(wxCommandEvent& event)
{
    wxLogMessage(_T("You can't enter more characters into this control."));
}

void PropagateCommandPanel::OnComboSelection(wxCommandEvent& event)
{    
    if ( event.GetEventObject() == synchComboBox )         
        applyButton->Enable(true);
    else if ( event.GetEventObject() == equalityComboBox )
        applyButton->Enable(true);
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
    else if ( event.GetEventObject() == userDefButton )           
        ;
    else if ( event.GetEventObject() == okButton )  
    {
        SetData();         
    }
    else if ( event.GetEventObject() == applyButton )
    {
        SetData();
        applyButton->Enable(false);
    }       
    else if ( event.GetEventObject() == cancelButton )
    {
        Close(this);
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
#if !defined __WXMAC__
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

void PropagateCommandPanel::OnCellLeftClick(wxGridEvent& event)
{
    //ev.GetRow()
    //ev.GetCol();

    // you must call event skip if you want default grid processing
    // (cell highlighting etc.)
    event.Skip();
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
