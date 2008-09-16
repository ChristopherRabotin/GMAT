//$Header$
//------------------------------------------------------------------------------
//                              AssignmentPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Allison Greene
// Created: 2004/12/23
/**
 * This class contains the Assignment Setup window.
 */
//------------------------------------------------------------------------------

#ifndef AssignmentPanel_hpp
#define AssignmentPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "Assignment.hpp"

class AssignmentPanel : public GmatPanel
{
public:
   // constructors
   AssignmentPanel( wxWindow *parent, GmatCommand *cmd);
   ~AssignmentPanel();

private:
   Assignment *theCommand;
   bool mIsTextModified;
   
   wxTextCtrl *mLhsTextCtrl;
   wxTextCtrl *mRhsTextCtrl;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   void OnTextChange(wxCommandEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {
      ID_TEXT = 44000,
      ID_TEXTCTRL,
   };
};

#endif // AssignmentPanel_hpp

