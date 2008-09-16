//$Header$
//------------------------------------------------------------------------------
//                              CompareReportPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2005/04/18
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
   void OnHelp();

protected:

   // member data
   wxString mName;
   wxTextCtrl *mTextCtrl;
   
   wxButton *theCloseButton;
   wxButton *theHelpButton;

   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
    
   // IDs for the controls and the menu commands
   enum
   {  
      ID_BUTTON_CLOSE = 8050,
      ID_BUTTON_HELP,
      ID_TEXTCTRL,
   };

};

#endif // CompareReportPanel_hpp
