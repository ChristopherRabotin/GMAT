//$Header$
//------------------------------------------------------------------------------
//                         ShowSummaryDialog
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


#include "ShowSummaryDialog.hpp"


//------------------------------------------------------------------------------
//  ShowSummaryDialog(wxWindow *parent, wxWindowID id, const wxString& title, 
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
ShowSummaryDialog::ShowSummaryDialog(wxWindow *parent, wxWindowID id, 
                                     const wxString& title, GmatCommand *obj) :
   GmatDialog(parent, id, title, NULL, wxDefaultPosition, wxDefaultSize, 
              wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
   theObject(obj)
{
   Create();
   ShowData();
   theOkButton->Enable(true);
   theDialogSizer->Hide(theCancelButton, true);
   theDialogSizer->Layout();
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates and populates the dialog.
 */
//------------------------------------------------------------------------------
void ShowSummaryDialog::Create()
{
   Integer w, h;
   wxString text = "Summary not yet available for this panel";
   // Find the height of a line of test, to use when sizing the text control
   GetTextExtent(text, &w, &h);

   wxSize scriptPanelSize(500, 32);
   if (theObject != NULL) {
      text = theObject->GetStringParameter("Summary").c_str();
      scriptPanelSize.Set(500, 400);
   }
   
   theSummary = new wxTextCtrl(this, -1, text, wxPoint(0,0), scriptPanelSize, 
                      wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);
   theSummary->SetFont( GmatAppData::GetFont() );
   theMiddleSizer->Add(theSummary, 1, wxGROW|wxALL, 3);
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * For the ShowSummaryDialog, does nothing..
 */
//------------------------------------------------------------------------------
void ShowSummaryDialog::LoadData()
{
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * For the ShowSummaryDialog, does nothing..
 */
//------------------------------------------------------------------------------
void ShowSummaryDialog::SaveData()
{
}


//------------------------------------------------------------------------------
// void ResetData()
//------------------------------------------------------------------------------
/**
 * For the ShowSummaryDialog, does nothing..
 */
//------------------------------------------------------------------------------
void ShowSummaryDialog::ResetData()
{
}
