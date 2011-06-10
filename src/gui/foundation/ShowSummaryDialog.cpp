//$Id$
//------------------------------------------------------------------------------
//                         ShowSummaryDialog
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2005/03/08
//
/**
 * Implements the dialog used to show the command summary for individual
 * components.
 */
//------------------------------------------------------------------------------


#include "ShowSummaryDialog.hpp"
#include "GmatAppData.hpp"

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

      // This code is flaky -- text width is height dependent??? -- on Linux:
//      GetTextExtent(text, &w, &h);
//      w = (w + 200 > 1000 ? 1000 : w + 200);
//      h = (h > 700 ? 700 : h);
      scriptPanelSize.Set(690, 720);

      SetSize(wxDefaultCoord, wxDefaultCoord, w, h);
   }
   
   // Set additional style wxTE_RICH to Ctrl + mouse scroll wheel to decrease or
   // increase text size on Windows(loj: 2009.02.05)
   theSummary = new wxTextCtrl(this, -1, text, wxPoint(0,0), scriptPanelSize, 
                    wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL | wxTE_RICH);
   theSummary->SetFont( GmatAppData::Instance()->GetFont() );
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
