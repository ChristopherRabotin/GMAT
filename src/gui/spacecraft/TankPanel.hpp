//$Id$
//------------------------------------------------------------------------------
//                            TankPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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

#include "gmatdefs.hpp"
#include "FuelTank.hpp"
#include "Spacecraft.hpp"
#include "Hardware.hpp"

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "GmatPanel.hpp"

class TankPanel: public wxPanel
{
public:
   TankPanel(GmatPanel *scPanel, wxWindow *parent, Spacecraft *spacecraft);
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

   GmatPanel *theScPanel;
   
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

