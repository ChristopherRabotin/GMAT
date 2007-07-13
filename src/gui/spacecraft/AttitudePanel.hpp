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
#include "CoordinateConverter.hpp"
#include "CoordinateSystem.hpp"

#include "gmatwxdefs.hpp"
#include "GuiItemManager.hpp"
#include "GmatPanel.hpp"
#include "MessageInterface.hpp"  // *************

class AttitudePanel: public wxPanel
{
public:
   AttitudePanel(GmatPanel *scPanel, wxWindow *parent, Spacecraft *spacecraft);
   ~AttitudePanel();
   void SaveData();
   void LoadData();

   bool IsDataChanged() { return dataChanged; };
   bool CanClosePanel() { return canClose;    };
   
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
   wxComboBox *stateRateTypeComboBox;

   // objects needed
   GmatPanel      *theScPanel;
   Spacecraft     *theSpacecraft;
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   Attitude       *theAttitude;
   
   // coordinate system and converter data
   CoordinateConverter cc;
   CoordinateSystem    *attCS;
   CoordinateSystem    *toCS;
   CoordinateSystem    *fromCS;
   
   // arrays of strings needed as input to the combo boxes
   StringArray modelArray;
   StringArray eulerSeqArray;
   StringArray stateTypeArray;
   StringArray stateRateTypeArray;
   //StringArray coordSysArray;
   //StringArray kinematicArray;

   // those strings as wxString arrays for the combo boxes
   wxString *attitudeModelArray; 
   wxString *eulerSequenceArray;
   wxString *stateArray;
   wxString *stateRateArray;
   
   // string versions of values in text boxes
   wxString *cosineMatrix[9];
   wxString *quaternion[4];
   wxString *eulerAngles[3];
   wxString *angVel[3];
   wxString *eulerAngleRates[3];
   
   // string versions of current data on combo boxes
   std::string      attitudeModel;
   std::string      attCoordSystem;
   std::string      eulerSequence;
   std::string      attStateType;
   std::string      attRateStateType;
   //std::string      attitudeType;   // for later use - Kinematic, etc.

   // flags for data modification
   bool dataChanged;
   bool canClose;
   
   // local values of attitude data
   // NOTE - only currently-displayed representations will be up-to-date;
   // conversions occur on state (or state rate) type changes
   UnsignedIntArray seq;
   Rmatrix33        mat;
   Rvector          q;
   Rvector3         ea;
   Rvector3         av;
   Rvector3         ear;
   Real             epoch;
   
   /// modification flags
   bool             stateTypeModified;
   bool             rateStateTypeModified;
   bool             stateModified;
   bool             stateRateModified;
   bool             csModified;
   bool             seqModified;
   bool             modelModified;

   bool             matModified[9];
   bool             qModified[4];
   bool             eaModified[3];
   bool             avModified[3];
   bool             earModified[3];

   void Create();
   void DisplayEulerAngles();
   void DisplayQuaternion();
   void DisplayDCM();
   void DisplayEulerAngleRates();
   void DisplayAngularVelocity();
   
   void UpdateEulerAngles();
   void UpdateQuaternion();
   void UpdateCosineMatrix();
   void UpdateEulerAngleRates();
   void UpdateAngularVelocity();
      
   bool IsStateModified(const std::string which = "Both");
   void ResetStateFlags(const std::string which = "Both",
                        bool discardEdits = false);
   bool ValidateState(const std::string which = "Both");
   
   wxString ToString(Real rval); // ??
   
   // Event Handling
   DECLARE_EVENT_TABLE();
   /// when user types in a new state value
   void OnStateTextUpdate(wxCommandEvent &event);
   /// when user types in a new state rate value
   void OnStateRateTextUpdate(wxCommandEvent &event);
   /// when user selects state type from combo box
   void OnStateTypeSelection(wxCommandEvent &event);
   /// when user selects state rate type from combo box
   void OnStateTypeRateSelection(wxCommandEvent &event);
   /// when user selects an euler sequence from comb box
   void OnEulerSequenceSelection(wxCommandEvent &event);
   /// when user selects a reference coordinate system from combo box
   void OnCoordinateSystemSelection(wxCommandEvent &event);
   /// when user selects a attitude model from combo box
   void OnAttitudeModelSelection(wxCommandEvent &event);
   
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 50000,
      ID_TEXTCTRL_STATE,
      ID_TEXTCTRL_STATE_RATE,
      ID_CB_STATE,
      ID_CB_STATE_RATE,
      ID_CB_SEQ,
      ID_CB_COORDSYS,
      ID_CB_MODEL,
   };
   
   // IDs for state type
   enum StateType
   {
      EULER_ANGLES = 0,
      QUATERNION,
      DCM,
      StateTypeCount,
   };
   
   // IDs for state rate type
   enum RateStateType
   {
      EULER_ANGLE_RATES = 0,
      ANGULAR_VELOCITY,
      StateRateTypeCount,
   };
      
   static const std::string STATE_TEXT[StateTypeCount];
   static const std::string STATE_RATE_TEXT[StateRateTypeCount];
   
   static const Integer STARTUP_STATE_TYPE_SELECTION;
   static const Integer STARTUP_RATE_STATE_TYPE_SELECTION;
   
};
#endif

