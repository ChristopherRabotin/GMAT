//$Header$
//------------------------------------------------------------------------------
//                              VariableCreatePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/12/10
// Modified: 2004/05/06 by Allison Greene to inherit from GmatPanel
/**
 * This class contains the Variable Create window.
 */
//------------------------------------------------------------------------------

#include "gmatwxdefs.hpp"
#include "ViewTextFrame.hpp"
#include "DocViewFrame.hpp"
#include "TextEditView.hpp"
#include "TextDocument.hpp"
#include "MdiTextEditView.hpp"
#include "MdiDocViewFrame.hpp"
#include "GmatAppData.hpp"
//#include "GmatMainNotebook.hpp"
#include "VariableCreatePanel.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GmatAppData.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(VariableCreatePanel, GmatPanel)
//    EVT_BUTTON(ID_BUTTON, VariableCreatePanel::OnButton)
    EVT_COMBOBOX(ID_COMBO, VariableCreatePanel::OnComboSelection)
    EVT_TEXT(ID_TEXTCTRL, VariableCreatePanel::OnTextUpdate)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// VariableCreatePanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
VariableCreatePanel::VariableCreatePanel( wxWindow *parent, const wxString &name )
    : GmatPanel(parent)
{
    Create();
    Show();
}

VariableCreatePanel::~VariableCreatePanel()
{
}

//-------------------------------
// private methods
//-------------------------------
void VariableCreatePanel::Create()
{
    Initialize();
    Setup(this);
}

void VariableCreatePanel::LoadData()
{
    // explicitly disable apply button
    // it is turned on in each of the panels
    theApplyButton->Disable();
}

void VariableCreatePanel::SaveData()
{   
    // explicitly disable apply button
    // it is turned on in each of the panels
    theApplyButton->Disable();
}


void VariableCreatePanel::Initialize()
{
}

void VariableCreatePanel::Setup( wxWindow *parent)
{   
    // wxString
    wxString strArray1[] = 
    {
        wxT("")
    };
    wxString strArray2[] = 
    {
        wxT("")
    };
    wxString strArray3[] = 
    {
        wxT("")
    };
    wxString strArray4[] = 
    {
        wxT("")
    };
    wxString strArray5[] = 
    {
        wxT("")
    };
    
    // wxStaticText
    objStaticText = new wxStaticText( parent, ID_TEXT, wxT("Object"), 
         wxDefaultPosition, wxDefaultSize, 0 );
    propStaticText = new wxStaticText( parent, ID_TEXT, wxT("Property"), 
         wxDefaultPosition, wxDefaultSize, 0 );
    cbodyStaticText = new wxStaticText( parent, ID_TEXT, wxT("Central Body"), 
         wxDefaultPosition, wxDefaultSize, 0 );
    coordStaticText = new wxStaticText( parent, ID_TEXT, 
         wxT("Coordinate System"), wxDefaultPosition, wxDefaultSize, 0 );
    rbodyStaticText = new wxStaticText( parent, ID_TEXT, wxT("Reference Body"), 
         wxDefaultPosition, wxDefaultSize, 0 );
    epochStaticText = new wxStaticText( parent, ID_TEXT, wxT("Reference Epoch"), 
         wxDefaultPosition, wxDefaultSize, 0 );
    indexStaticText = new wxStaticText( parent, ID_TEXT, wxT("Index"), 
         wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxTextCtrl
    epochTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), 
         wxDefaultPosition, wxSize(80,-1), 0 );
    indexTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), 
         wxDefaultPosition, wxSize(80,-1), 0 );
      
    // wxListBox
    objListBox = new wxListBox( parent, ID_LISTBOX, wxDefaultPosition, wxSize(200,200), 1, strArray1, wxLB_SINGLE );
    propListBox = new wxListBox( parent, ID_LISTBOX, wxDefaultPosition, wxSize(200,200), 1, strArray2, wxLB_SINGLE );
    
    // wxComboBox
    cbodyComboBox = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strArray3, wxCB_DROPDOWN );
    coordComboBox = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strArray4, wxCB_DROPDOWN );
    rbodyComboBox = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strArray5, wxCB_DROPDOWN );
    
    // wxSizers
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
    wxFlexGridSizer *item5 = new wxFlexGridSizer( 2, 0, 0 );
    item5->AddGrowableRow( 1 );
    wxFlexGridSizer *item12 = new wxFlexGridSizer( 5, 0, 0 );
    wxStaticBox *item11 = new wxStaticBox( parent, -1, wxT("Details") );
    wxStaticBoxSizer *item10 = new wxStaticBoxSizer( item11, wxVERTICAL );
    
    item5->Add( objStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    item5->Add( propStaticText, 0, wxALIGN_CENTRE|wxALL, 5 ); 
    item5->Add( objListBox, 0, wxALIGN_CENTRE|wxALL, 5 );
    item5->Add( propListBox, 0, wxALIGN_CENTRE|wxALL, 5 );

    item0->Add( item5, 0, wxALIGN_CENTRE|wxALL, 5 );

    item12->Add( cbodyStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( coordStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( rbodyStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( cbodyComboBox, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( coordComboBox, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( rbodyComboBox, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( epochStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( indexStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( epochTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    item12->Add( indexTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );

    item10->Add( item12, 0, wxALIGN_CENTRE|wxALL, 5 );

    item0->Add( item10, 0, wxALIGN_CENTRE|wxALL, 5 );

    theMiddleSizer->Add(item0, 0, wxGROW, 5);
}

//void VariableCreatePanel::GetData()
//{
//}
//
//void VariableCreatePanel::SetData()
//{
//}
//
////------------------------------------------------------------------------------
//// wxMenuBar* CreateScriptWindowMenu(const std::string &docType)
////------------------------------------------------------------------------------
//wxMenuBar* VariableCreatePanel::CreateScriptWindowMenu(const std::string &docType)
//{
//    // Make a menubar
//    wxMenu *fileMenu = new wxMenu;
//    wxMenu *editMenu = (wxMenu *) NULL;
//    
//    fileMenu->Append(wxID_NEW, _T("&New..."));
//    fileMenu->Append(wxID_OPEN, _T("&Open..."));
//
//    if (docType == "sdi")
//    {
//        fileMenu->Append(wxID_CLOSE, _T("&Close"));
//        fileMenu->Append(wxID_SAVE, _T("&Save"));
//        fileMenu->Append(wxID_SAVEAS, _T("Save &As..."));
//        fileMenu->AppendSeparator();
//        fileMenu->Append(wxID_PRINT, _T("&Print..."));
//        fileMenu->Append(wxID_PRINT_SETUP, _T("Print &Setup..."));
//        fileMenu->Append(wxID_PREVIEW, _T("Print Pre&view"));
//    
//        editMenu = new wxMenu;
//        editMenu->Append(wxID_UNDO, _T("&Undo"));
//        editMenu->Append(wxID_REDO, _T("&Redo"));
//        editMenu->AppendSeparator();
//        //editMenu->Append(DOCVIEW_CUT, _T("&Cut last segment"));
//    
//        docMainFrame->editMenu = editMenu;
//        fileMenu->AppendSeparator();
//    }
//    
//    fileMenu->Append(wxID_EXIT, _T("E&xit"));
//    
//    // A nice touch: a history of files visited. Use this menu.
//    mDocManager->FileHistoryUseMenu(fileMenu);
//    
//    //wxMenu *help_menu = new wxMenu;
//    //help_menu->Append(DOCVIEW_ABOUT, _T("&About"));
//    
//    wxMenuBar *menuBar = new wxMenuBar;
//    
//    menuBar->Append(fileMenu, _T("&File"));
//    
//    if (editMenu)
//        menuBar->Append(editMenu, _T("&Edit"));
//    
//    //menuBar->Append(help_menu, _T("&Help"));
//
//    return menuBar;
//}
//
//void VariableCreatePanel::CreateScript()
//{
//    //not MAC mode
//    //----------------------------------------------------------------
///* **** NOTE:  it is temporary until it is fixed for Mac  ***************
//#if !defined __WXMAC__
//******************************* */
//#if 1
//    // Create a document manager
//    mDocManager = new wxDocManager;
//
//    // Create a template relating text documents to their views
//    mDocTemplate = 
//        new wxDocTemplate(mDocManager, _T("Text"), _T("*.script"),
//                          _T(""), _T("script"), _T("Text Doc"), _T("Text View"),
//                          CLASSINFO(TextDocument), CLASSINFO(MdiTextEditView));
//    
//    // Create the main frame window    
//    mdiDocMainFrame =
//        new MdiDocViewFrame(mDocManager, mdiDocMainFrame, _T("Script Window (MDI)"),
//                            wxPoint(0, 0), wxSize(600, 500),
//                            (wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE));
//    
//    // Give it an icon (this is ignored in MDI mode: uses resources)
//    mdiDocMainFrame->SetIcon(wxIcon(_T("doc")));
//    
//    // Make a menubar
//    wxMenuBar *menuBar = CreateScriptWindowMenu("mdi");
//       
//    // Associate the menu bar with the frame
//    mdiDocMainFrame->SetMenuBar(menuBar);
//    
//    mdiDocMainFrame->Centre(wxBOTH);
//    mdiDocMainFrame->Show(TRUE);
//    //----------------------------------------------------------------
//#else
//    // Create a document manager
//    mDocManager = new wxDocManager;
//
//    // Create a template relating text documents to their views
//    mDocTemplate = 
//        new wxDocTemplate(mDocManager, _T("Text"), _T("*.script"),
//                          _T(""), _T("script"), _T("Text Doc"), _T("Text View"),
//                          CLASSINFO(TextDocument), CLASSINFO(TextEditView));
//    
//    // Create the main frame window    
//    docMainFrame =
//        new DocViewFrame(mDocManager, this, -1, _T("Script Window"),
//                         wxPoint(0, 0), wxSize(600, 500), wxDEFAULT_FRAME_STYLE);
//        
//    // Make a menubar
//    wxMenuBar *menuBar = CreateScriptWindowMenu("sdi");
//       
//    // Associate the menu bar with the frame
//    docMainFrame->SetMenuBar(menuBar);
//    
//    docMainFrame->Centre(wxBOTH);
//    docMainFrame->Show(TRUE);
//#endif
//}

void VariableCreatePanel::OnTextUpdate(wxCommandEvent& event)
{
    if ( event.GetEventObject() == epochTextCtrl )  
    {
        theApplyButton->Enable(true);       
    }
    else if ( event.GetEventObject() == indexTextCtrl )
    {
        theApplyButton->Enable(false);
    } 
    else
        event.Skip();
}
    
void VariableCreatePanel::OnComboSelection(wxCommandEvent& event)
{
    theApplyButton->Enable(true);
}

//void VariableCreatePanel::OnButton(wxCommandEvent& event)
//{    
//    if ( event.GetEventObject() == scriptButton )  
//    {       
//        CreateScript();
//        theApplyButton->Enable(true);
//    }
//    else if ( event.GetEventObject() == okButton )  
//    {
//        SetData(); 
//        
//        GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
//        gmatMainNotebook->ClosePage();       
//    }
//    else if ( event.GetEventObject() == applyButton )
//    {
//        SetData();
//        applyButton->Enable(false);
//    } 
//    else if ( event.GetEventObject() == cancelButton )
//    {
//        GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
//        gmatMainNotebook->ClosePage();
//    }      
//    else if ( event.GetEventObject() == helpButton )           
//        ; 
//    else
//        event.Skip();
//}
