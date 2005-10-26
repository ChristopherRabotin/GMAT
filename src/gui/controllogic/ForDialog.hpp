//$Header$
//------------------------------------------------------------------------------
//                              ForDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/04/06
// Modified:
/**
 * This class allows user to edit ForLoop parameters.
 */
//------------------------------------------------------------------------------

#ifndef ForDialog_hpp
#define ForDialog_hpp

#include "gmatwxdefs.hpp"
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "GmatDialog.hpp"

#include "For.hpp"

class ForDialog : public GmatDialog
{
public:
   ForDialog(wxWindow *parent, For *forCommand, Integer col);
   ~ForDialog();
private:   
   wxStaticText *forStaticText;
    
   wxTextCtrl *forTextCtrl;

   For *theForCommand;
    
   Integer forID;
   Integer whichParameter;
   
   static const int START_COL = 1;
   static const int INCR_COL = 2;
   static const int END_COL = 3;

   // Private methods
   void Initialize();
    
   // Methods inherited from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
    
   // Event-handling Methods
   void OnTextChange(wxCommandEvent &event);
   void OnBrowse(wxCommandEvent &event);

   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 45000,
      ID_TEXTCTRL,
   };
};

#endif

