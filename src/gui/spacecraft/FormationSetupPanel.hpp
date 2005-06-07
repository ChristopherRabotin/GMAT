//$Header$
//------------------------------------------------------------------------------
//                              FormationSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/02/12
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
      SEL_LISTBOX
   };
};
#endif
