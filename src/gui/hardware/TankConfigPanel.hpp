//$Id$
//------------------------------------------------------------------------------
//                            TankConfigPanel
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
// Modified: 
//    2010.02.12 Thomas Grubb 
//       - defined ___USE_OLD_TANK_PANEL__
//       - Added GmatStaticBoxSizers: fuelPropertiesSizer and tankPropertiesSizer
//    2009.05.27 Linda Jun - To derive from GmatBaseSetupPanel
/**
 * This class contains information needed to setup users spacecraft tank 
 * parameters.
 */
//------------------------------------------------------------------------------
#ifndef TankConfigPanel_hpp
#define TankConfigPanel_hpp

// This panel is derived from GmatBaseSetupPanel
// 
//====================================================================
#define __USE_OLD_TANK_PANEL__
#ifndef __USE_OLD_TANK_PANEL__
//====================================================================

#include "gmatwxdefs.hpp"
#include "GmatBaseSetupPanel.hpp"
#include "FuelTank.hpp"

class TankConfigPanel: public GmatBaseSetupPanel
{
public:
   TankConfigPanel(wxWindow *parent, const wxString &name);
   ~TankConfigPanel();
   
};


//====================================================================
#else
//====================================================================


#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "gmatdefs.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "FuelTank.hpp"

class TankConfigPanel: public GmatPanel
{
public:
   TankConfigPanel(wxWindow *parent, const wxString &name);
   ~TankConfigPanel();
   
private: 
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // Event Handling
   DECLARE_EVENT_TABLE();
   void OnTextChange(wxCommandEvent &event);
   void OnComboBoxChange(wxCommandEvent &event);
   
   FuelTank* theFuelTank;
   
   wxTextCtrl *temperatureTextCtrl;
   wxTextCtrl *refTemperatureTextCtrl;
   wxTextCtrl *fuelMassTextCtrl;
   wxTextCtrl *fuelDensityTextCtrl;
   wxTextCtrl *pressureTextCtrl;
   wxTextCtrl *volumeTextCtrl;
   
   wxCheckBox *allowNegativeFuelMassCheckBox;

   wxComboBox *pressureModelComboBox;
   GmatStaticBoxSizer *tankPropertiesSizer;
   GmatStaticBoxSizer *fuelPropertiesSizer;

   // IDs for the controls and the menu commands
   enum
   {
      ID_TEXT = 39000,
      ID_TEXTCTRL,
      ID_LISTBOX,
      ID_BUTTON,
      ID_CHECKBOX,
      ID_COMBOBOX
   };
};

//====================================================================
#endif // __USE_OLD_TANK_PANEL__
//====================================================================

#endif // TankConfigPanel_hpp
