//$Header$
//------------------------------------------------------------------------------
//                                 ViewTextFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/11/03
//
/**
 * Shows text.
 */
//------------------------------------------------------------------------------
#ifndef ViewTextFrame_hpp
#define ViewTextFrame_hpp

#include "gmatwxdefs.hpp"

class ViewTextFrame: public wxFrame
{
public:
   
   // constructors
   ViewTextFrame(wxFrame *frame, const wxString& title, int x, int y, int w, int h,
                 const wxString &mode = "Temporary", const wxString &type = "Output");
   ~ViewTextFrame();
   
   // operations
   void AppendText(const wxString& text);
   void SetMaxLength(unsigned long len)
      { mTextCtrl->SetMaxLength(len); };
   int GetNumberOfLines()
      { return mTextCtrl->GetNumberOfLines(); };
   void ClearText()
      { mTextCtrl->Clear(); };
   void SetWindowMode(const wxString &mode)
      { mWindowMode = mode; };
   wxString GetWindowMode()
      { return mWindowMode; };
   wxTextCtrl *GetTextCtrl()
      { return mTextCtrl; };
    
private:

   wxMenuBar *CreateMainMenu();

   DECLARE_EVENT_TABLE();

   void OnClear(wxCommandEvent& event);
   void OnSaveAs(wxCommandEvent& event);
   void OnExit(wxCommandEvent& event);

   // IDs for the controls and the menu commands
   enum
   {
      VIEW_TEXT_CLEAR = 8000,
      VIEW_TEXT_SAVE_AS,
      VIEW_TEXT_EXIT
   };

   /// just some place to put our messages in
   wxTextCtrl *mTextCtrl;

   /// window mode ("Permanent", "Temporary")
   wxString mWindowMode;
   
   /// text type ("Output", "Script")
   wxString mTextType;
};

#endif
