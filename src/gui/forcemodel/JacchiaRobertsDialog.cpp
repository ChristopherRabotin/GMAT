//$Header$
//------------------------------------------------------------------------------
//                              JacchiaRobertsDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/05/18
// Modified:
/**
 * This class allows user to edit Jacchia-Roberts drag parameters.
 */
//------------------------------------------------------------------------------

#include "JacchiaRobertsDialog.hpp"
#include "MessageInterface.hpp"

#include <wx/variant.h>

//------------------------------------------------------------------------------
// Event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(JacchiaRobertsDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, JacchiaRobertsDialog::OnBrowse)
   EVT_TEXT(ID_TEXTCTRL, JacchiaRobertsDialog::OnTextChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// JacchiaRobertsDialog(wxWindow *parent, const wxString name, DragForce *dragForce)
//------------------------------------------------------------------------------
/**
 * Constructs Jacchia-Roberts object.
 *
 * @param <parent> input parent.
 * @param <dragForce> input drag force.
 *
 * @note Creates the Jacchia-Roberts drag dialog
 */
//------------------------------------------------------------------------------
JacchiaRobertsDialog::JacchiaRobertsDialog(wxWindow *parent, DragForce *dragForce)
    : GmatDialog(parent, -1, wxString(_T("JacchiaRobertsDialog")))
{
    if (dragForce != NULL)    
        theForce = dragForce;
       
    Create();
    Show();
}

//------------------------------------------------------------------------------
// ~JacchiaRobertsDialog()
//------------------------------------------------------------------------------
JacchiaRobertsDialog::~JacchiaRobertsDialog()
{
}

//-------------------------------
// public methods
//-------------------------------

//------------------------------------------------------------------------------
// void GetForce()
//------------------------------------------------------------------------------
DragForce* JacchiaRobertsDialog::GetForce()
{
   return theForce;
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
void JacchiaRobertsDialog::Initialize()
{  
// waw: Future build implementation
//    if (theForce == NULL)
//    {
//        MessageInterface::ShowMessage("Error: The MSISE90 Drag Force is NULL.\n");
//        Close();
//    }
}

//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void JacchiaRobertsDialog::Create()
{
    // wxStaticText
    solarFluxStaticText = new wxStaticText( this, ID_TEXT, wxT("Solar Flux"), wxDefaultPosition, wxDefaultSize, 0 );
    fileNameStaticText = new wxStaticText( this, ID_TEXT, wxT("File Name"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxTextCtrl
    solarFluxTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    fileNameTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    
    // wxButton
    browseButton = new wxButton( this, ID_BUTTON, wxT("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxSizer
    wxFlexGridSizer *mainFlexGridSizer = new wxFlexGridSizer( 2, 3, 0, 0 );
    
    mainFlexGridSizer->Add( solarFluxStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    mainFlexGridSizer->Add( solarFluxTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    mainFlexGridSizer->Add( 20, 0, wxALIGN_CENTER|wxALL, 5 );
    mainFlexGridSizer->Add( fileNameStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    mainFlexGridSizer->Add( fileNameTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    mainFlexGridSizer->Add( browseButton, 0, wxALIGN_CENTER|wxALL, 5 );
    
    theMiddleSizer->Add(mainFlexGridSizer, 0, wxALIGN_CENTER|wxALL, 5);
    
    // waw: Future build implementation
    solarFluxTextCtrl->Enable(false);
    fileNameTextCtrl->Enable(false);
    browseButton->Enable(false);
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void JacchiaRobertsDialog::LoadData()
{   
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void JacchiaRobertsDialog::SaveData()
{      
}

//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void JacchiaRobertsDialog::ResetData()
{

}

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void JacchiaRobertsDialog::OnTextChange()
{
    theOkButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnBrowse()
//------------------------------------------------------------------------------
void JacchiaRobertsDialog::OnBrowse()
{
    wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
    
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString filename;
        
        filename = dialog.GetPath().c_str();
        
        fileNameTextCtrl->SetValue(filename); 
    }
    theOkButton->Enable(true);
}

