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
             wxButton *theApplyButton);
   ~TankPanel();
   void SaveData();
   void LoadData();
   
private:    
   void Create();
//    void DisplayData();
   
   // Event Handling
   DECLARE_EVENT_TABLE();
//    void OnSelect(wxCommandEvent &event);
   void OnButtonClick(wxCommandEvent &event);
   
   Spacecraft *theSpacecraft;
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;

   wxArrayString mExcludedTankList;
//    wxArrayString availableTanksArray;
//    wxArrayString selectedTanksArray;
   
//    Integer availableTankCount;
//    Integer selectedTankCount;
   
//    Integer currentAvailTank;
//    Integer currentSelectedTank;
   
   wxButton *theApplyButton;
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

