//$Header$
//------------------------------------------------------------------------------
//                         ShowScriptDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2005/03/08
//
/**
 * Implements the dialog used to show scripting for individual components. 
 */
//------------------------------------------------------------------------------


#include "ShowScriptDialog.hpp"


//------------------------------------------------------------------------------
//  ShowScriptDialog(wxWindow *parent, wxWindowID id, const wxString& title, 
//                   GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * FiniteBurn constructor (default constructor).
 * 
 * @param parent Parent wxWindow for the dialog.
 * @param id wxWidget identifier for the dialog.
 * @param title Title for the dialog.
 * @param obj The object that provides the script text.
 */
//------------------------------------------------------------------------------
ShowScriptDialog::ShowScriptDialog(wxWindow *parent, wxWindowID id, 
                                    const wxString& title, GmatBase *obj) :
   GmatDialog(parent, id, title),
   theObject(obj)
{
   Create();
   ShowData();
   theOkButton->Enable(true);
   theCancelButton->Enable(false);
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates and populates the dialog.
 */
//------------------------------------------------------------------------------
void ShowScriptDialog::Create()
{
   wxString text = "Script not yet available for this panel";
   wxSize scriptPanelSize(500, 32);
   if (theObject != NULL) {
      text = theObject->GetGeneratingString().c_str();
      StringArray sar = theObject->GetGeneratingStringArray();
      Integer size = sar.size();
      scriptPanelSize.Set(500, 32 + size * 14);
   }
   
   theScript = new wxTextCtrl(this, -1, text, wxPoint(0,0), scriptPanelSize, 
                      wxTE_MULTILINE | wxTE_READONLY);
   theMiddleSizer->Add(theScript, 0, wxALIGN_CENTRE|wxALL, 3);
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * For the ShowScriptDialog, does nothing..
 */
//------------------------------------------------------------------------------
void ShowScriptDialog::LoadData()
{
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * For the ShowScriptDialog, does nothing..
 */
//------------------------------------------------------------------------------
void ShowScriptDialog::SaveData()
{
}


//------------------------------------------------------------------------------
// void ResetData()
//------------------------------------------------------------------------------
/**
 * For the ShowScriptDialog, does nothing..
 */
//------------------------------------------------------------------------------
void ShowScriptDialog::ResetData()
{
}
