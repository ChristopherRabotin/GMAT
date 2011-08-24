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
 * Implements the dialog used to show the command summary for individual
 * components.
 */
//------------------------------------------------------------------------------

#include "GuiItemManager.hpp"
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
                    GmatCommand *obj, bool summaryForMission = false, bool physicsOnly = false);
   ~ShowSummaryDialog();

protected:
   /// The object that provides the generating string for display.
   GmatCommand *theObject;
   /// The component on the dialog that shows the scripting.
   wxTextCtrl *theSummary;
          
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();

   // event handling
   void OnComboBoxChange(wxCommandEvent& event);

   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {
      ID_CS_TEXT = 46000,
      ID_COMBOBOX
   };


   bool           isCoordSysModified;
   std::string    currentCoordSysName;
   GuiItemManager *theGuiManager;
   bool           summaryForEntireMission;
   bool           physicsBasedOnly;

   wxComboBox     *coordSysComboBox;

   void BuildValidCoordinateSystemList();
};

#endif //ShowScriptDialog_hpp
