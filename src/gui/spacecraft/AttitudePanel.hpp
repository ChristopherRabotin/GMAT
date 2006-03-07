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
   
   wxStaticText *stateTypeStaticText;
   wxStaticText *stateTypeRate4StaticText;
   
   wxStaticText *col1StaticText;
   wxStaticText *col2StaticText;
   wxStaticText *col3StaticText;
   wxStaticText *st1StaticText;
   wxStaticText *st2StaticText;
   wxStaticText *st3StaticText;
   wxStaticText *st4StaticText;
   
   wxStaticText *str1StaticText;
   wxStaticText *str2StaticText;
   wxStaticText *str3StaticText;
   
   wxTextCtrl *st1TextCtrl;
   wxTextCtrl *st2TextCtrl;
   wxTextCtrl *st3TextCtrl;
   wxTextCtrl *st4TextCtrl;
   wxTextCtrl *st5TextCtrl;
   wxTextCtrl *st6TextCtrl;
   wxTextCtrl *st7TextCtrl;
   wxTextCtrl *st8TextCtrl;
   wxTextCtrl *st9TextCtrl;
   wxTextCtrl *st10TextCtrl;
   
   wxTextCtrl *str1TextCtrl;
   wxTextCtrl *str2TextCtrl;
   wxTextCtrl *str3TextCtrl;
   
   wxComboBox *config1ComboBox;
   wxComboBox *config2ComboBox;
   wxComboBox *config3ComboBox;
   wxComboBox *config4ComboBox;
   wxComboBox *stateTypeComboBox;
   wxComboBox *stateTypeRateComboBox;
   
   wxButton *theApplyButton;
   
   StringArray modeArray;
   StringArray coordSysArray;
   StringArray kinematicArray;
   StringArray eulerAngleArray;
   StringArray stateTypeArray;
   StringArray stateTypeRateArray;
   
   void Create();
   void DisplayEulerAngles();
   void DisplayQuaternions();
   void DisplayDCM();
   void DisplayEulerAngleRates();
   void DisplayAngularVelocity();
   
   // Event Handling
   DECLARE_EVENT_TABLE();
   void OnStateTypeTextUpdate(wxCommandEvent &event);
   void OnStateTypeRateTextUpdate(wxCommandEvent &event);
   void OnConfigurationSelection(wxCommandEvent &event);
   void OnStateTypeSelection(wxCommandEvent &event);
   void OnStateTypeRateSelection(wxCommandEvent &event);
   
   Spacecraft *theSpacecraft;
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 50000,
      ID_TEXTCTRL_ST,
      ID_TEXTCTRL_STR,
      ID_CB_CONFIG,
      ID_CB_ST,
      ID_CB_STR,
      ID_BUTTON
   };
   
   enum StateType
   {
      EULER_ANGLES= 0,
      QUATERNIONS,
      DCM,
      StateTypeCount,
   };
   
   enum RateStateType
   {
      EULER_ANGLES_RATES= 0,
      ANGULAR_VELOCITY,
      RateStateTypeCount,
   };
};
#endif

