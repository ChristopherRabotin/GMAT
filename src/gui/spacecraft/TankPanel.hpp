//$Header$
//------------------------------------------------------------------------------
//                            TankPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Waka Waktola
// Created: 2004/11/19
/**
 * This class contains information needed to setup users spacecraft tank 
 * parameters.
 */
//------------------------------------------------------------------------------
#ifndef TankPanel_hpp
#define TankPanel_hpp

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

#include "gmatdefs.hpp"
#include "Spacecraft.hpp"
#include "Hardware.hpp"
#include "FuelTank.hpp"

class TankPanel: public wxPanel
{
public:
   TankPanel(wxWindow *parent, Spacecraft *spacecraft,
             wxButton *applyButton, wxButton *okButton);
   ~TankPanel();
   void SaveData();
   void LoadData();
   
   bool IsDataChanged() { return dataChanged; }
   
private:    
   
   bool dataChanged;
   
   void Create();
   
   // Event Handling
   DECLARE_EVENT_TABLE();
   void OnButtonClick(wxCommandEvent &event);
   
   Spacecraft *theSpacecraft;
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;

   wxArrayString mExcludedTankList;
   
   wxButton *theApplyButton;
   wxButton *theOkButton;
   wxButton *selectButton;
   wxButton *removeButton;
   wxButton *selectAllButton;
   wxButton *removeAllButton;
   
   wxListBox *availableTankListBox;
   wxListBox *selectedTankListBox;
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_LISTBOX = 30200,
      ID_BUTTON,
   };
};
#endif

