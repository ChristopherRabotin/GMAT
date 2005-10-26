//$Header$
//------------------------------------------------------------------------------
//                              ForDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/04/06
// Modified:
/**
 * This class allows user to edit For Command parameters.
 */
//------------------------------------------------------------------------------

#include "ForDialog.hpp"
#include "MessageInterface.hpp"

#include <wx/variant.h>

//------------------------------------------------------------------------------
// Event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ForDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_TEXT(ID_TEXTCTRL, ForDialog::OnTextChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// ForDialog(wxWindow *parent, const wxString name, DragForce *dragForce)
//------------------------------------------------------------------------------
/**
 * @param <parent> input parent.
 * @param <dragForce> input drag force.
 *
 * @note Creates the ForDialog dialog
 */
//------------------------------------------------------------------------------
ForDialog::ForDialog(wxWindow *parent, For *forCommand, Integer col)
    : GmatDialog(parent, -1, wxString(_T("ForDialog")))
{
    if (forCommand != NULL)    
        theForCommand = forCommand;

    whichParameter = col;
    
    Create();
    ShowData();
}

//------------------------------------------------------------------------------
// ~MSISE90()
//------------------------------------------------------------------------------
ForDialog::~ForDialog()
{
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
void ForDialog::Initialize()
{  
    if (theForCommand == NULL)
    {
        MessageInterface::ShowMessage("Error: The For Command is NULL.\n");
        Close();
    }
}

//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void ForDialog::Create()
{
    // wxStaticText
    forStaticText = new wxStaticText( this, ID_TEXT, wxT("For Parameter"), wxDefaultPosition, wxDefaultSize, 0 );
 
    // wxTextCtrl
    forTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
 
    // wxSizer
    wxBoxSizer *mainPageSizer = new wxBoxSizer(wxHORIZONTAL);

    mainPageSizer->Add(forStaticText, 0, wxALIGN_CENTER|wxALL, 5);
    mainPageSizer->Add(forTextCtrl, 0, wxALIGN_CENTER|wxALL, 5);
    
    theMiddleSizer->Add(mainPageSizer, 0, wxALIGN_CENTER|wxALL, 5);
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ForDialog::LoadData()
{   
    Initialize();
    
    if (whichParameter == START_COL)
       forID = theForCommand->GetParameterID("StartValue");
    else if (whichParameter == INCR_COL)
       forID = theForCommand->GetParameterID("Step");
    else if (whichParameter == END_COL)
       forID = theForCommand->GetParameterID("EndValue");
       
    forTextCtrl->SetValue(wxVariant(theForCommand->GetRealParameter(forID)));
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ForDialog::SaveData()
{
   if (theOkButton->IsEnabled())
   {      
      if (whichParameter == START_COL)
         forID = theForCommand->GetParameterID("StartValue");
      else if (whichParameter == INCR_COL)
         forID = theForCommand->GetParameterID("Step");
      else if (whichParameter == END_COL)
         forID = theForCommand->GetParameterID("EndValue");
       
      theForCommand->SetRealParameter(forID, atof(forTextCtrl->GetValue().c_str()));
   }       
}

//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void ForDialog::ResetData()
{

}

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void ForDialog::OnTextChange(wxCommandEvent &event)
{
    theOkButton->Enable(true);
}

