//$Id$
//------------------------------------------------------------------------------
//                         ShowScriptDialog
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2005/03/08
//
/**
 * Implements the dialog used to show scripting for individual components. 
 */
//------------------------------------------------------------------------------


#include "GmatDialog.hpp"

#ifndef ShowScriptDialog_hpp
#define ShowScriptDialog_hpp


/** The modal dialog used to display scripting for a component. 
 *
 * At user request, this dialog was constructed so that the user can select text 
 * and copy it to the clipboard for pasting elsewhere. 
 */
class ShowScriptDialog : public GmatDialog
{
public:
   // constructors
   ShowScriptDialog(wxWindow *parent, wxWindowID id, const wxString& title, 
                    GmatBase *obj, bool isSingleton = false);

protected:
   /// The object that provides the generating string for display.
   GmatBase *theObject;
   /// The component on the dialog that shows the scripting.
   wxTextCtrl *theScript;
   /// Flag used to suppress the "Create" line
   bool showAsSingleton;
          
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
};

#endif //ShowScriptDialog_hpp
