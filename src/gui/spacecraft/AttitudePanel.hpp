//$Id$
//------------------------------------------------------------------------------
//                            AttitudePanel
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
#include "GmatStaticBoxSizer.hpp"

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

   GmatStaticBoxSizer *attitudeSizer;
   GmatStaticBoxSizer *attRateSizer;
   
   wxStaticText *config1StaticText;
   wxStaticText *config2StaticText;
   wxStaticText *config3StaticText;
   wxStaticText *config4StaticText;
   
   wxStaticText *stateTypeStaticText;
   wxStaticText *stateTypeRate4StaticText;
   
   wxStaticText *st1StaticText;
   wxStaticText *st2StaticText;
   wxStaticText *st3StaticText;
   wxStaticText *st4StaticText;
   
   wxStaticText *str1StaticText;
   wxStaticText *str2StaticText;
   wxStaticText *str3StaticText;
   
   wxStaticText *spiceMessage;

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
   wxString *MRPs[3]; // Dunn Added
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
   Rmatrix33        dcmat; // Dunn renamed to dcmat
   Rvector          q;
   Rvector3         ea;
   Rvector3         mrp; // Dunn Added
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

   bool             dcmatModified[9]; // Dunn renamed to dcmatModified
   bool             qModified[4];
   bool             eaModified[3];
   bool             mrpModified[3];   // Dunn Added
   bool             avModified[3];
   bool             earModified[3];

   void Create();
   bool DisplayEulerAngles();
   bool DisplayQuaternion();
   bool DisplayDCM();
   bool DisplayMRPs();  // Dunn Added
   bool DisplayEulerAngleRates();
   bool DisplayAngularVelocity();
   
   bool UpdateEulerAngles();
   bool UpdateQuaternion();
   bool UpdateCosineMatrix();
   bool UpdateMRPs();   // Dunn Added
   bool UpdateEulerAngleRates();
   bool UpdateAngularVelocity();
      
   bool IsStateModified(const std::string which = "Both");
   void ResetStateFlags(const std::string which = "Both",
                        bool discardEdits = false);
   bool ValidateState(const std::string which = "Both");
   
   void DisableInitialAttitudeRate();
   void EnableInitialAttitudeRate();
   void DisableAll();
   void EnableAll();
   void DisplaySpiceReminder();

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
   // Dunn added a new enumeration, "MRPs", and changed the name of the count
   // variable at the bottom from StateTypeCount, which shows up for OrbitState
   // as well, to attStateTypeCount.
   enum StateType
   {
      EULER_ANGLES = 0,
      QUATERNION,
      DCM,
      MRPS,
      attStateTypeCount,
   };
   
   // IDs for state rate type
   enum RateStateType
   {
      EULER_ANGLE_RATES = 0,
      ANGULAR_VELOCITY,
      attStateRateTypeCount,  // Dunn renamed from StateRateTypeCount
   };
   
   static const std::string STATE_TEXT[attStateTypeCount];
   static const std::string STATE_RATE_TEXT[attStateRateTypeCount];
   
   static const Integer STARTUP_STATE_TYPE_SELECTION;
   static const Integer STARTUP_RATE_STATE_TYPE_SELECTION;
   
};
#endif

