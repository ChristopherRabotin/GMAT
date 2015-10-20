//$Id$
//------------------------------------------------------------------------------
//                            PowerSystemPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan
// Created: 2014.05.07
/**
 * This class contains information needed to setup users spacecraft power
 * system through GUI
 *
 */
//------------------------------------------------------------------------------
#ifndef PowerSystemPanel_hpp
#define PowerSystemPanel_hpp

#include "gmatwxdefs.hpp"
#include "Spacecraft.hpp"
#include "GmatPanel.hpp"
#include "GuiItemManager.hpp"
#include "GmatAppData.hpp"


class PowerSystemPanel: public wxPanel
{
public:
   PowerSystemPanel(GmatPanel *scPanel, wxWindow *parent, Spacecraft *spacecraft);
   ~PowerSystemPanel();

   void SaveData();
   void LoadData();

   bool IsDataChanged() { return dataChanged; }
   bool CanClosePanel() { return canClose; }

private:
   bool dataChanged;
   bool canClose;

   bool powerSystemChanged;

   void Create();

   // Event Handling
   DECLARE_EVENT_TABLE();
   void OnComboBoxChange(wxCommandEvent& event);

   Spacecraft     *theSpacecraft;
   GuiItemManager *theGuiManager;
   GuiInterpreter *theGuiInterpreter;

   GmatPanel    *theScPanel;

   wxComboBox   *powerSystemComboBox;

   std::string  thePowerSystem;

   // IDs for the controls and the menu commands
   enum
   {
      ID_TEXT = 30220,
      ID_COMBOBOX
   };
};
#endif
