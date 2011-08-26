//
//------------------------------------------------------------------------------
//                         OrbitSummaryDialog
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
// Author: Evelyn Hull
// Created: 2011/10/08
//
/**
 * Implements the dialog used to show a summary of the calculated orbit 
 * parameters.
 */
//------------------------------------------------------------------------------

#include "OrbitSummaryDialog.hpp"

BEGIN_EVENT_TABLE(OrbitSummaryDialog, GmatDialog)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// OrbitSummaryDialog()
//------------------------------------------------------------------------------
// A constructor.
//------------------------------------------------------------------------------
OrbitSummaryDialog::OrbitSummaryDialog(wxWindow *parent, wxString &summary)
   : GmatDialog(parent, -1, wxString(_T("Orbit Summary")))
{
   orbitSummaryString = wxT(summary);

   theCancelButton->Show(false);
   theHelpButton->Show(false);

   canClose = true;

   Create();
   ShowData();
}

OrbitSummaryDialog::~OrbitSummaryDialog()
{
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void OrbitSummaryDialog::Create()
{
   orbitSummary = new wxTextCtrl(this, -1, orbitSummaryString, wxPoint(0,0), wxSize(300,200), 
                    wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL | wxTE_RICH);

   theMiddleSizer->Add(orbitSummary, 1, wxGROW|wxALL, 3);
}

void OrbitSummaryDialog::LoadData()
{
}

void OrbitSummaryDialog::SaveData()
{
}

void OrbitSummaryDialog::ResetData()
{
}