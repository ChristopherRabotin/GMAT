//$Header$
//------------------------------------------------------------------------------
//                         ShowScriptDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2005/03/08
//
/**
 * Implements the dialog used to show scripting for individual components. 
 */
//------------------------------------------------------------------------------


#include "GmatDialog.hpp"

#ifndef ShowSummaryDialog_hpp
#define ShowSummaryDialog_hpp


/** The modal dialog used to display scripting for a component. 
 *
 * At user request, this dialog was constructed so that the user can select text 
 * and copy it to the clipboard for pasting elsewhere. 
 */
class ShowSummaryDialog : public GmatDialog
{
public:
   // constructors
   ShowSummaryDialog(wxWindow *parent, wxWindowID id, const wxString& title, 
                    GmatCommand *obj);

protected:
   /// The object that provides the generating string for display.
   GmatCommand *theObject;
   /// The component on the dialog that shows the scripting.
   wxTextCtrl *theSummary;
          
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
};

#endif //ShowScriptDialog_hpp
