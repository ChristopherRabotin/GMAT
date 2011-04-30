//$Id$
//------------------------------------------------------------------------------
//                              CompareReportPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Linda Jun
// Created: 2004/02/02
//
/**
 * Declares CompareReportPanel class.
 */
//------------------------------------------------------------------------------

#ifndef CompareReportPanel_hpp
#define CompareReportPanel_hpp

#include "gmatwxdefs.hpp"

class CompareReportPanel : public wxPanel
{
public:
   // constructors
   CompareReportPanel( wxWindow *parent, const wxString &name);

   void AppendText(const wxString &text);
   void SetMaxLength(unsigned long len)
      { mTextCtrl->SetMaxLength(len); };
   int GetNumberOfLines()
      { return mTextCtrl->GetNumberOfLines(); };
   void ClearText()
      { mTextCtrl->Clear(); };
   wxTextCtrl *GetTextCtrl()
      { return mTextCtrl; };
   
   void OnClose(wxCommandEvent &event);

protected:

   // member data
   wxString mName;
   wxTextCtrl *mTextCtrl;
   
   wxButton *theCloseButton;
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {  
      ID_BUTTON_CLOSE = 8050,
      ID_TEXTCTRL,
   };

};

#endif // CompareReportPanel_hpp
