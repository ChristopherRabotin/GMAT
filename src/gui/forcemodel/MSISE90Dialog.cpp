//$Header:
//------------------------------------------------------------------------------
//                              MSISE90Panel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/04/06
// Modified:
/**
 * This class allows user to edit MSISE90 drag parameters.
 */
//------------------------------------------------------------------------------

#include "MSISE90Dialog.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// Event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MSISE90Dialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatDialog::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_TEXT(ID_TEXTCTRL, MSISE90Dialog::OnTextChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// MSISE90Dialog(wxWindow *parent, const wxString name)
//------------------------------------------------------------------------------
/**
 * Constructs MSISE90 object.
 *
 * @param <parent> input parent.
 * @param <name> input drag name.
 *
 * @note Creates the MSISE90 drag panel
 */
//------------------------------------------------------------------------------
MSISE90Dialog::MSISE90Dialog(wxWindow *parent, wxString name)
    : GmatDialog(parent, -1, wxString(_T("MSISE90DragDialog")))
{    
    Create();
    Show();
}

//------------------------------------------------------------------------------
// ~MSISE90()
//------------------------------------------------------------------------------
MSISE90Dialog::~MSISE90Dialog()
{
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void MSISE90Dialog::Create()
{
    // wxStaticText
    wxStaticText *msise90Drag1StaticText = new wxStaticText( this, ID_TEXT, wxT("Parameter 1"), wxDefaultPosition, wxDefaultSize, 0 );
    wxStaticText *msise90Drag2StaticText = new wxStaticText( this, ID_TEXT, wxT("Parameter 2"), wxDefaultPosition, wxDefaultSize, 0 );
    wxStaticText *msise90Drag3StaticText = new wxStaticText( this, ID_TEXT, wxT("Parameter 3"), wxDefaultPosition, wxDefaultSize, 0 );
    wxStaticText *msise90Drag4StaticText = new wxStaticText( this, ID_TEXT, wxT("Parameter 4"), wxDefaultPosition, wxDefaultSize, 0 );
    wxStaticText *msise90Drag5StaticText = new wxStaticText( this, ID_TEXT, wxT("Parameter 5"), wxDefaultPosition, wxDefaultSize, 0 );
    wxStaticText *msise90Drag6StaticText = new wxStaticText( this, ID_TEXT, wxT("Parameter 6"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxTextCtrl
    msise90Drag1TextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    msise90Drag2TextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    msise90Drag3TextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    msise90Drag4TextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );;
    msise90Drag5TextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );;
    msise90Drag6TextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );;
    
    // wxSizer
    wxFlexGridSizer *pageFlexGridSizer = new wxFlexGridSizer( 3, 2, 0, 0 );

    pageFlexGridSizer->Add( msise90Drag1StaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    pageFlexGridSizer->Add( msise90Drag1TextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    pageFlexGridSizer->Add( msise90Drag2StaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    pageFlexGridSizer->Add( msise90Drag2TextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    pageFlexGridSizer->Add( msise90Drag3StaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    pageFlexGridSizer->Add( msise90Drag3TextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    pageFlexGridSizer->Add( msise90Drag4StaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    pageFlexGridSizer->Add( msise90Drag4TextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    pageFlexGridSizer->Add( msise90Drag5StaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    pageFlexGridSizer->Add( msise90Drag5TextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    pageFlexGridSizer->Add( msise90Drag6StaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    pageFlexGridSizer->Add( msise90Drag6TextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    
    theMiddleSizer->Add(pageFlexGridSizer, 0, wxALIGN_CENTER|wxALL, 5);
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void MSISE90Dialog::LoadData()
{

}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void MSISE90Dialog::SaveData()
{

}

//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void MSISE90Dialog::ResetData()
{

}

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void MSISE90Dialog::OnTextChange()
{

}

