//$Header$
//------------------------------------------------------------------------------
//                         ViewTextDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
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
   ViewTextDialog(wxWindow *parent, const wxString& title, int w, int h);
   
   void AppendText(const wxString& text);
   void SetMaxLength(unsigned long len) { theText->SetMaxLength(len); };
   wxTextCtrl *GetTextCtrl() { return theText; };
   
   virtual void OnOK(wxCommandEvent &event);
   
protected:
   
   /// The component on the dialog that shows the text.
   wxTextCtrl *theText;
   wxButton *theOkButton;
   
   wxBoxSizer *theDialogSizer;
   wxBoxSizer *theButtonSizer;
   wxBoxSizer *theMiddleSizer;
   wxBoxSizer *theBottomSizer;
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {
      ID_BUTTON = 8000,
   };
};

#endif //ViewTextDialog_hpp
