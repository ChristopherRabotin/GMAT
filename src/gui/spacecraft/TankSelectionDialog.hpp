//$Header: 
//------------------------------------------------------------------------------
//                              TankSelectionDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2005/01/26
//
/**
 * Declares TankSelectionDialog class. This class shows dialog window where
 * available tanks can be selected.
 * 
 */
//------------------------------------------------------------------------------
#ifndef TankSelectionDialog_hpp
#define TankSelectionDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"
#include "Spacecraft.hpp"

class TankSelectionDialog : public GmatDialog
{
public:
   TankSelectionDialog(wxWindow *parent, Spacecraft *spacecraft, 
                        StringArray selectedTanks);
   
private:    
   Integer selectedCount;
   Integer availableCount;
   
   StringArray selectedTankNames;
   StringArray availableTankNames;
   
   Spacecraft *theSpacecraft;
   
   wxListBox *availableListBox;
   wxListBox *selectedListBox;
   
   wxButton *selectButton;
   wxButton *removeButton;
   
   wxButton *selectAllButton;
   wxButton *removeAllButton;
   
   // methods inherited from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   // event handling   
   DECLARE_EVENT_TABLE();
   void OnSelect(wxCommandEvent &event);
   void OnButtonClick(wxCommandEvent &event);
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_LISTBOX = 30400,
      ID_BUTTON,
   };
};

#endif

@