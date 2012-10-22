//$Id$
//------------------------------------------------------------------------------
//                         ViewTextDialog
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun, NASA/GSFC
// Created: 2007/08/22
//
/**
 * Declares the dialog used to show text.
 */
//------------------------------------------------------------------------------
#ifndef ViewTextDialog_hpp
#define ViewTextDialog_hpp

#include "gmatwxdefs.hpp"
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/button.h>

class ViewTextDialog : public wxDialog
{
public:
   // default constructor
   ViewTextDialog(wxWindow *parent, const wxString& title,
                  bool isEditable = false,
                  const wxPoint &pos = wxDefaultPosition,
                  const wxSize &size = wxDefaultSize,
                  long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
   
   // text manipulation
   void AppendText(const wxString& text);
   bool HasTextChanged() { return hasTextChanged; };
   wxString GetText() { return theText->GetValue(); };
   void SetMaxLength(unsigned long len) { theText->SetMaxLength(len); };
   wxTextCtrl *GetTextCtrl() { return theText; };
   
   // event handlers
   virtual void OnButtonClick(wxCommandEvent &event);
   
protected:
   
   bool isTextEditable;
   bool hasTextChanged;
   wxString oldText;
   
   /// The component on the dialog that shows the text.
   wxTextCtrl *theText;
   wxButton *theOkButton;
   wxButton *theCancelButton;
   
   wxBoxSizer *theButtonSizer;
   wxBoxSizer *theMiddleSizer;
   wxBoxSizer *theBottomSizer;
   
   // event handlers
   virtual void OnEnterPressed(wxCommandEvent &event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {
      ID_BUTTON = 8000,
   };
};

#endif //ViewTextDialog_hpp
