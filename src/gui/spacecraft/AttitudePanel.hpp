//$Header$
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
// Author: Waka Waktola (and heavily modified by Wendy C. Shoan)
// Created: 2006/03/01 (2007.06.12)
/**
 * This class contains information needed to setup users spacecraft attitude
 * parameters.
 */
//------------------------------------------------------------------------------
#ifndef AttitudePanel_hpp
#define AttitudePanel_hpp

#include "gmatdefs.hpp"
#include "Attitude.hpp"
#include "GuiInterpreter.hpp"
#include "Spacecraft.hpp"

#include "gmatwxdefs.hpp"
#include "GuiItemManager.hpp"
#include "GmatPanel.hpp"

class AttitudePanel: public wxPanel
{
public:
   AttitudePanel(GmatPanel *scPanel, wxWindow *parent, Spacecraft *spacecraft);
   ~AttitudePanel();
   void SaveData();
   void LoadData();

   bool IsDataChanged() { return dataChanged; }
   
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

   wxStaticText *attUnits1;
   wxStaticText *attUnits2;
   wxStaticText *attUnits3;

   wxStaticText *rateUnits1;
   wxStaticText *rateUnits2;
   wxStaticText *rateUnits3;

   wxComboBox *config1ComboBox;
   wxComboBox *config2ComboBox;
   wxComboBox *config3ComboBox;
   wxComboBox *config4ComboBox;
   wxComboBox *stateTypeComboBox;
   wxComboBox *stateTypeRateComboBox;

   GmatPanel *theScPanel;
   
   StringArray modelArray;
   StringArray coordSysArray;
   StringArray kinematicArray;
   StringArray eulerAngleArray;
   StringArray stateTypeArray;
   StringArray stateTypeRateArray;
   
   void Create();
   void DisplayEulerAngles();
   void DisplayQuaternion();
   void DisplayDCM();
   void DisplayEulerAngleRates();
   void DisplayAngularVelocity();
   
   wxString *attitudeModelArray; 
   wxString *cosineMatrix[9];
   wxString *eulerAngles[3];
   wxString *quaternion[4];
   wxString *eulerAngleRates[3];
   wxString *angVel[3];
   
   bool dontUpdate;  // true while writing to textCtrls
   bool dataChanged;
   
   // Event Handling
   DECLARE_EVENT_TABLE();
   void OnStateTypeTextUpdate(wxCommandEvent &event);
   void OnStateTypeRateTextUpdate(wxCommandEvent &event);
   void OnConfigurationSelection(wxCommandEvent &event);
   void OnStateTypeSelection(wxCommandEvent &event);
   void OnStateTypeRateSelection(wxCommandEvent &event);
   void OnEulerSequenceSelection(wxCommandEvent &event);
   void OnAttitudeModelSelection(wxCommandEvent &event);
   
   void CalculateFromEulerAngles();
   void CalculateFromQuaternion();
   void CalculateFromCosineMatrix();
   
   void CalculateFromEulerAngleRates();
   void CalculateFromAngularVelocity();

   bool IsStateModified();
   void ResetStateFlags(bool discardEdits = false);
   
   Spacecraft     *theSpacecraft;
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   Attitude       *theAttitude;
   
   std::string      attStateType;
   std::string      attRateStateType;
   std::string      attitudeModel;
   std::string      attitudeType;   // for later use - Kinematic, etc.
   std::string      attCoordSystem;
   std::string      eulerSequence;

   std::string      loadedAttStateType;
   std::string      loadedAttRateStateType;
   std::string      loadedAttitudeModel;
   //std::string      loadedAttitudeType;   // for later use - Kinematic, etc.
   std::string      loadedAttCoordSystem;
   std::string      loadedEulerSequence;

   UnsignedIntArray seq;
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 50000,
      ID_TEXTCTRL_ST,
      ID_TEXTCTRL_STR,
      ID_CB_CONFIG,
      ID_CB_ST,
      ID_CB_STR,
      ID_BUTTON,
      ID_CB_EAS,
      ID_CB_COORDSYS,
      ID_CB_MODE
   };
   
   enum StateType
   {
      EULER_ANGLES= 0,
      QUATERNION,
      DCM,
      StateTypeCount,
   };
   
   enum RateStateType
   {
      EULER_ANGLE_RATES= 0,
      ANGULAR_VELOCITY,
      RateStateTypeCount,
   };
};
#endif

