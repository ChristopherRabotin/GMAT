//$Header$
//------------------------------------------------------------------------------
//                              ExponentialDragDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/04/01
// Modified:
/**
 * This class allows user to edit Exponential drag parameters.
 */
//------------------------------------------------------------------------------

#include "ExponentialDragDialog.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// Event tables and other macros for wxWindows
//------------------------------------------------------------------------------

//loj: 10/19/04 removed OnApply
BEGIN_EVENT_TABLE(ExponentialDragDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_TEXT(ID_TEXTCTRL, ExponentialDragDialog::OnTextChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// ExponentialDragDialog(wxWindow *parent, const wxString name)
//------------------------------------------------------------------------------
/**
 * Constructs ExponentialDragPanel object.
 *
 * @param <parent> input parent.
 * @param <name> input drag name.
 *
 * @note Creates the exponential drag panel
 */
//------------------------------------------------------------------------------
ExponentialDragDialog::ExponentialDragDialog(wxWindow *parent, DragForce *dragForce)
   : GmatDialog(parent, -1, wxString(_T("ExponentialDragDialog")))
{    
   if (dragForce != NULL)    
      theForce = dragForce;
       
   Create();
   Show();
}

//------------------------------------------------------------------------------
// ~ExponentialDragDialog()
//------------------------------------------------------------------------------
ExponentialDragDialog::~ExponentialDragDialog()
{
}

//-------------------------------
// public methods
//-------------------------------

//------------------------------------------------------------------------------
// void GetForce()
//------------------------------------------------------------------------------
DragForce* ExponentialDragDialog::GetForce()
{
   return theForce;
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void ExponentialDragDialog::Create()
{
   // wxStaticText
   wxStaticText *expDrag1StaticText = new wxStaticText( this, ID_TEXT, wxT("Parameter 1"), wxDefaultPosition, wxDefaultSize, 0 );
   wxStaticText *expDrag2StaticText = new wxStaticText( this, ID_TEXT, wxT("Parameter 2"), wxDefaultPosition, wxDefaultSize, 0 );
   wxStaticText *expDrag3StaticText = new wxStaticText( this, ID_TEXT, wxT("Parameter 3"), wxDefaultPosition, wxDefaultSize, 0 );
    
   // wxTextCtrl
   expDrag1TextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
   expDrag2TextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
   expDrag3TextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    
   // wxSizer
   wxFlexGridSizer *pageFlexGridSizer = new wxFlexGridSizer( 3, 2, 0, 0 );

   pageFlexGridSizer->Add( expDrag1StaticText, 0, wxALIGN_CENTER|wxALL, 5 );
   pageFlexGridSizer->Add( expDrag1TextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
   pageFlexGridSizer->Add( expDrag2StaticText, 0, wxALIGN_CENTER|wxALL, 5 );
   pageFlexGridSizer->Add( expDrag2TextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
   pageFlexGridSizer->Add( expDrag3StaticText, 0, wxALIGN_CENTER|wxALL, 5 );
   pageFlexGridSizer->Add( expDrag3TextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    
   theMiddleSizer->Add(pageFlexGridSizer, 0, wxALIGN_CENTER|wxALL, 5);
    
   expDrag1TextCtrl->Enable(false);
   expDrag2TextCtrl->Enable(false);
   expDrag3TextCtrl->Enable(false);
    
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ExponentialDragDialog::LoadData()
{

}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ExponentialDragDialog::SaveData()
{

}

//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void ExponentialDragDialog::ResetData()
{

}

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void ExponentialDragDialog::OnTextChange()
{

}

