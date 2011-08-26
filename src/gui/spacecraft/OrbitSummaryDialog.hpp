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
 * parameters
 */
//------------------------------------------------------------------------------


#include "GmatDialog.hpp"

#ifndef OrbitSummaryDialog_hpp
#define OrbitSummaryDialog_hpp

class OrbitSummaryDialog : public GmatDialog
{
public:
   // constructors
   OrbitSummaryDialog(wxWindow *parent, wxString &summary);
   ~OrbitSummaryDialog();

private:
   wxTextCtrl *orbitSummary;
   wxString orbitSummaryString;

   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();

   DECLARE_EVENT_TABLE();

   enum
   {
	  ID_TEXTCTRL = 8000
   };
};

#endif //OrbitSummaryDialog_hpp
