//$Id$
//------------------------------------------------------------------------------
//                              FormationSetupPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/02/12 
// Modified: 
//    2010.03.22 Thomas Grubb 
//      - Added AVL_LISTBOX window id to use with Available ListBox double-click
/**
 * Declares FormationSetupPanel class. This class allows user to setup OpenGL Plot.
 */
//------------------------------------------------------------------------------
#ifndef FormationSetupPanel_hpp
#define FormationSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"

class FormationSetupPanel: public GmatPanel
{
public:
   FormationSetupPanel(wxWindow *parent, const wxString &formationName);
   ~FormationSetupPanel();
   
protected:
   
   std::string mFormationName;
   wxArrayString mSoExcList;
   
   wxListBox *mSoAvailableListBox;
   wxListBox *mSoSelectedListBox;

   void OnAddSpaceObject(wxCommandEvent& event);
   void OnRemoveSpaceObject(wxCommandEvent& event);
   void OnClearSpaceObject(wxCommandEvent& event);

   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ADD_BUTTON = 99000,
      REMOVE_BUTTON,
      CLEAR_BUTTON,
      SEL_LISTBOX,
      AVL_LISTBOX
   };
};
#endif
