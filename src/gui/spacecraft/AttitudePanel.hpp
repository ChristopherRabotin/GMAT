//$Header: 
//------------------------------------------------------------------------------
//                            AttitudePanel
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
// Created: 2006/03/01
/**
 * This class contains information needed to setup users spacecraft attitude
 * parameters.
 */
//------------------------------------------------------------------------------
#ifndef AttitudePanel_hpp
#define AttitudePanel_hpp

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

#include "gmatdefs.hpp"
#include "Spacecraft.hpp"

class AttitudePanel: public wxPanel
{
public:
   AttitudePanel(wxWindow *parent, Spacecraft *spacecraft,
             wxButton *theApplyButton);
   ~AttitudePanel();
   void SaveData();
   void LoadData();
   
private:    

   wxStaticText *config1StaticText;
   wxStaticText *config2StaticText;
   wxStaticText *config3StaticText;
   wxStaticText *config4StaticText;
   
   wxStaticText *state1StaticText;
   wxStaticText *state24StaticText;
   wxStaticText *stateTypeStaticText;
   wxStaticText *stateTypeRate4StaticText;
   
   wxComboBox *config1ComboBox;
   wxComboBox *config2ComboBox;
   wxComboBox *config3ComboBox;
   wxComboBox *config4ComboBox;
   wxComboBox *stateTypeComboBox;
   wxComboBox *stateTypeRateComboBox;
   
   wxButton *theApplyButton;
   
   void Create();
   
   // Event Handling
   DECLARE_EVENT_TABLE();
   
   Spacecraft *theSpacecraft;
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 50000,
      ID_TEXTCTRL,
      ID_CB,
      ID_BUTTON
   };
};
#endif

