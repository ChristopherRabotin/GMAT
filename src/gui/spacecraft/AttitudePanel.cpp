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
// Developed further jointly by NASA/GSFC, Thinking Systems, Inc., and 
// Schafer Corp., under AFRL NOVA Contract #FA945104D03990003
//
//
// Author:    Waka Waktola
// Created:   2006/03/01
// Modified:  Wendy C. Shoan (Modified Heavily)
// Date:      2007/06/12
// Modified:  Dunn Idle (added MRPs, comments, renamed variables for clarity)
// Date:      2010/08/15
//
/**
 * This class contains information needed to setup users' spacecraft attitude
 * parameters in the GUI Spacecraft Attitude TAB.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "AttitudePanel.hpp"
#include "AttitudeFactory.hpp"
#include "MessageInterface.hpp"
#include "GmatAppData.hpp"
#include "GmatBaseException.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "StringUtil.hpp"
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"
#include <wx/config.h>

//#define DEBUG_ATTITUDE_PANEL 1
//#define DEBUG_ATTITUDE_SAVE
//#define DEBUG_ATTITUDE_RATE
//#define DEBUG_ATTITUDE_ANG_VEL
//#define DEBUG_ATTITUDE_PANEL_DCM
//#define DEBUG_PRECESSING_SPINNER
//#define DEBUG_NADIR_POINTING

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string AttitudePanel::STATE_TEXT[AttStateTypeCount] =
{
   "EulerAngles",
   "Quaternion",
   "DirectionCosineMatrix",
   "MRPs",                    // Dunn added MRPs
};

const std::string AttitudePanel::STATE_RATE_TEXT[AttStateRateTypeCount] =
{
   "EulerAngleRates",
   "AngularVelocity",
};

// initial selections in combo boxes on set-up (before LoadData)
const Integer AttitudePanel::STARTUP_STATE_TYPE_SELECTION      = EULER_ANGLES;
const Integer AttitudePanel::STARTUP_RATE_STATE_TYPE_SELECTION = EULER_ANGLE_RATES;

const Integer AttitudePanel::ATTITUDE_TEXT_CTRL_WIDTH          = 80;
const Integer AttitudePanel::QUATERNION_TEXT_CTRL_WIDTH        = 148;

const Real    AttitudePanel::EULER_ANGLE_TOLERANCE             = 1.0E-10;
const Real    AttitudePanel::DCM_ORTHONORMALITY_TOLERANCE      = 1.0e-14;

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(AttitudePanel, wxPanel)
   EVT_TEXT(ID_TEXTCTRL_STATE,            AttitudePanel::OnStateTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_STATE_RATE,       AttitudePanel::OnStateRateTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_SPIN_AXIS,        AttitudePanel::OnSpinAxisTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_NUT_REF_VEC,      AttitudePanel::OnNutationRefVectorTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_INIT_PREC_ANGLE,  AttitudePanel::OnInitialPrecessionAngleTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_PRECESSION_RATE,  AttitudePanel::OnPrecessionRateTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_NUTATION_ANGLE,   AttitudePanel::OnNutationAngleTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_INIT_SPIN_ANGLE,  AttitudePanel::OnInitialSpinAngleTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_SPIN_RATE,        AttitudePanel::OnSpinRateTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_REFERENCE_VECTOR,        AttitudePanel::OnReferenceVectorTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_CONSTRAINT_VECTOR,       AttitudePanel::OnConstraintVectorTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_BODY_ALIGNMENT_VECTOR,   AttitudePanel::OnBodyAlignmentVectorTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_BODY_CONSTRAINT_VECTOR,  AttitudePanel::OnBodyConstraintVectorTextUpdate)
   EVT_COMBOBOX(ID_CB_STATE,              AttitudePanel::OnStateTypeSelection)
   EVT_COMBOBOX(ID_CB_STATE_RATE,         AttitudePanel::OnStateTypeRateSelection)
   EVT_COMBOBOX(ID_CB_SEQ,                AttitudePanel::OnEulerSequenceSelection)
   EVT_COMBOBOX(ID_CB_COORDSYS,           AttitudePanel::OnCoordinateSystemSelection)
   EVT_COMBOBOX(ID_CB_MODEL,              AttitudePanel::OnAttitudeModelSelection)
   EVT_COMBOBOX(ID_CB_REFERENCE_BODY,     AttitudePanel::OnReferenceBodySelection)
   EVT_COMBOBOX(ID_CB_MODE_OF_CONSTRAINT, AttitudePanel::OnModeOfConstraintSelection)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// AttitudePanel(GmatPanel *scPanel, xWindow *parent, Spacecraft *spacecraft)
//------------------------------------------------------------------------------
/**
 * Constructs AttitudePanel object (constructor)
 */
//------------------------------------------------------------------------------
AttitudePanel::AttitudePanel(GmatPanel *scPanel, wxWindow *parent,
                             Spacecraft *spacecraft) :
   wxPanel     (parent), 
   theAttitude (NULL),
   attCS       (NULL),
   toCS        (NULL),
   fromCS      (NULL),
   canClose    (true)
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::AttitudePanel() entered\n");
   #endif

   theScPanel        = scPanel;
   theSpacecraft     = spacecraft;
   
   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   theGuiManager     = GuiItemManager::GetInstance();
   
   modelArray.clear();
   eulerSeqArray.clear();
   stateTypeArray.clear();
   stateRateTypeArray.clear();
   
   unsigned int defSeq[3] = {3, 2, 1};  // Dunn changed from 312 to 321
   seq.push_back(defSeq[0]);
   seq.push_back(defSeq[1]);
   seq.push_back(defSeq[2]);
   
   attitudeModel       = "";
   attCoordSystem      = "";
   eulerSequence       = "321";  // Dunn changed from 312 to 321
   attStateType        = "";
   attRateStateType    = "";
   
   stateTypeModified     = false;
   rateStateTypeModified = false;
   stateModified         = false;
   stateRateModified     = false;
   csModified            = false;
   seqModified           = false;
   modelModified         = false;
   
   ResetStateFlags("Both");   
   
   precessingSpinnerDataLoaded  = false;  // LOJ: Added, this flag tells whether data is loaded or not
   spinAxisModified             = false;  // LOJ: Added, this flag is for all 3 components
   nutRefVecModified            = false;  // LOJ: Added, this flag is for all 3 components
   initPrecAngleModified        = false;
   precessionRateModified       = false;
   nutationAngleModified        = false;
   initSpinAngleModified        = false;
   spinRateModified             = false;

   nadirPointingDataLoaded      = false;  // WCS: Added, this flag tells whether data is loaded or not
   attRefBodyModified           = false;
   modeOfConstraintModified     = false;
   refVectorModified            = false;
   constraintVectorModified     = false;
   bodyAlignVectorModified      = false;
   bodyConstraintVectorModified = false;

   dataChanged = false;
   canClose    = true;
   Create();
}

//------------------------------------------------------------------------------
// ~AttitudePanel()
//------------------------------------------------------------------------------
/**
 * Destroys the AttitudePanel object (destructor)
 */
//------------------------------------------------------------------------------
AttitudePanel::~AttitudePanel()
{
   theGuiManager->UnregisterComboBox("CoordinateSystem", coordSysComboBox);
   theGuiManager->UnregisterComboBox("CelestialBody", referenceBodyComboBox);

   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::~AttitudePanel() entered\n");
   #endif
}

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates the widgets for the panel.
 */
//------------------------------------------------------------------------------
void AttitudePanel::Create()
{ 
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::Create() entered\n");
   #endif
   
   // Need to adjust width of static text because spiceMessage is wrapping and
   // being clipped
   // NOTE - this may need to be adjusted for different platforms
   Integer staticTextWidth = 165;

   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Spacecraft Attitude"));

   // arrays to hold temporary values
   unsigned int x;
   for (x = 0; x < 3; ++x)
   {
      eulerAngles[x]     = new wxString();
      eulerAngleRates[x] = new wxString();
      quaternion[x]      = new wxString();
      MRPs[x]            = new wxString();    // Dunn Added
      angVel[x]          = new wxString();
   }
   quaternion[3] = new wxString();
   for (x = 0; x < 9; ++x)
   {
      cosineMatrix[x]    = new wxString();    // Dunn put all 9 elements here.
   }
   
   q = Rvector(4);
   
   // get list of models and put them into the combo box
   modelArray = theGuiInterpreter->GetListOfFactoryItems(Gmat::ATTITUDE);
   unsigned int modelSz = modelArray.size();
   
   //======================================================================
   //LOJ: Just for testing
   // It should appear automatically when new model is added to factory
   // @todo - Remove this when you integrate with new attitude model
//   modelArray.push_back("PrecessingSpinner");
//   modelSz++;
   //======================================================================
   attitudeModelArray = new wxString[modelSz];
   for (x = 0; x < modelSz; ++x)
      attitudeModelArray[x] = wxT(modelArray[x].c_str());
   
   
   config1StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Attitude "GUI_ACCEL_KEY"Model"),
                        wxDefaultPosition, wxSize(staticTextWidth,20), 0); // wxDefaultSize, 0);
   config1ComboBox = 
      new wxComboBox( this, ID_CB_MODEL, wxT(attitudeModelArray[0]), 
         wxDefaultPosition, wxDefaultSize, modelSz, attitudeModelArray, 
         wxCB_DROPDOWN|wxCB_READONLY );
   config1ComboBox->SetToolTip(pConfig->Read(_T("AttitudeModelHint")));

   // Coordinate System
   config2StaticText =
      new wxStaticText( this, ID_TEXT, wxT(GUI_ACCEL_KEY"Coordinate System"),
         wxDefaultPosition, wxSize(staticTextWidth,20), 0); // wxDefaultSize, 0);
   coordSysComboBox =  theGuiManager->GetCoordSysComboBox(this, ID_CB_COORDSYS,
      wxDefaultSize);
   coordSysComboBox->SetToolTip(pConfig->Read(_T("CoordinateSystemHint")));

   //Euler Angle Sequence
   eulerSeqArray           = Attitude::GetEulerSequenceStrings();
   unsigned int eulerSeqSz = eulerSeqArray.size();
   
   eulerSequenceArray = new wxString[eulerSeqSz];  // Euler sequence types
   unsigned int i;
   Integer ii;
   for (i=0; i<eulerSeqSz; i++)
      eulerSequenceArray[i] = eulerSeqArray[i].c_str();

   config4StaticText =
      new wxStaticText( this, ID_TEXT, wxT(GUI_ACCEL_KEY"Euler Angle Sequence"),
                        wxDefaultPosition, wxSize(staticTextWidth,20), 0); // wxDefaultSize, 0);
   config4ComboBox = new wxComboBox( this, ID_CB_SEQ, wxT(eulerSequenceArray[0]),
                      wxDefaultPosition, wxDefaultSize, 12,
                      eulerSequenceArray, wxCB_DROPDOWN|wxCB_READONLY );
   config4ComboBox->SetToolTip(pConfig->Read(_T("EulerAngleSequenceHint")));

   // State Type
   stateTypeStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Attitude "GUI_ACCEL_KEY"State Type"),
                        wxDefaultPosition, wxDefaultSize, 0);

   for (ii = 0; ii < AttStateTypeCount; ii++)
      stateTypeArray.push_back(STATE_TEXT[ii]);
   
   stateArray = new wxString[AttStateTypeCount];
   for (ii=0; ii<AttStateTypeCount; ii++)
      stateArray[ii] = stateTypeArray[ii].c_str();
   
   stateTypeComboBox = 
      new wxComboBox( this, ID_CB_STATE, wxT(stateArray[STARTUP_STATE_TYPE_SELECTION]),
         wxDefaultPosition, wxSize(180,20), AttStateTypeCount, stateArray,
         wxCB_DROPDOWN|wxCB_READONLY );
   stateTypeComboBox->SetToolTip(pConfig->Read(_T("StateTypeHint")));
   
   st1StaticText =
      new wxStaticText( this, ID_TEXT, wxT(""),
                        wxDefaultPosition, wxDefaultSize, 0);
   st2StaticText =
      new wxStaticText( this, ID_TEXT, wxT(""),
                        wxDefaultPosition, wxDefaultSize, 0);
   st3StaticText =
      new wxStaticText( this, ID_TEXT, wxT(""),
                        wxDefaultPosition, wxDefaultSize, 0);
   st4StaticText =
      new wxStaticText( this, ID_TEXT, wxT(""),
                        wxDefaultPosition, wxDefaultSize, 0);
   st1TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE, wxT(""),
                      wxDefaultPosition, wxSize(ATTITUDE_TEXT_CTRL_WIDTH,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   st2TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE, wxT(""),
                      wxDefaultPosition, wxSize(ATTITUDE_TEXT_CTRL_WIDTH,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   st3TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE, wxT(""),
                      wxDefaultPosition, wxSize(ATTITUDE_TEXT_CTRL_WIDTH,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   st4TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE, wxT(""),
                      wxDefaultPosition, wxSize(ATTITUDE_TEXT_CTRL_WIDTH,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   st5TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE, wxT(""),
                      wxDefaultPosition, wxSize(ATTITUDE_TEXT_CTRL_WIDTH,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   st6TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE, wxT(""),
                      wxDefaultPosition, wxSize(ATTITUDE_TEXT_CTRL_WIDTH,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   st7TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE, wxT(""),
                      wxDefaultPosition, wxSize(ATTITUDE_TEXT_CTRL_WIDTH,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   st8TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE, wxT(""),
                      wxDefaultPosition, wxSize(ATTITUDE_TEXT_CTRL_WIDTH,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   st9TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE, wxT(""),
                      wxDefaultPosition, wxSize(ATTITUDE_TEXT_CTRL_WIDTH,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   st10TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE, wxT(""),
                      wxDefaultPosition, wxSize(ATTITUDE_TEXT_CTRL_WIDTH,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );

   // units for the Euler Angles
   attUnits1 = new wxStaticText( this, ID_TEXT, wxT("degrees"));
   attUnits2 = new wxStaticText( this, ID_TEXT, wxT("degrees"));
   attUnits3 = new wxStaticText( this, ID_TEXT, wxT("degrees"));

   // Rate State Type
   stateRateTypeStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Attitude "GUI_ACCEL_KEY"Rate State Type"),
                        wxDefaultPosition, wxDefaultSize, 0);

   for (ii = 0; ii < AttStateRateTypeCount; ii++)
      stateRateTypeArray.push_back(STATE_RATE_TEXT[ii]);

   stateRateArray = new wxString[AttStateRateTypeCount];
   for (ii=0; ii< AttStateRateTypeCount; ii++)
      stateRateArray[ii] = stateRateTypeArray[ii].c_str();

   stateRateTypeComboBox =
      new wxComboBox( this, ID_CB_STATE_RATE, 
         wxT(stateRateArray[STARTUP_RATE_STATE_TYPE_SELECTION]), wxDefaultPosition, 
         wxSize(180,20), AttStateRateTypeCount, stateRateArray,
         wxCB_DROPDOWN|wxCB_READONLY );                  
   stateRateTypeComboBox->SetToolTip(pConfig->Read(_T("RateStateTypeHint")));
   
   str1StaticText =
      new wxStaticText( this, ID_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
   str2StaticText =
      new wxStaticText( this, ID_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
   str3StaticText =
      new wxStaticText( this, ID_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
   
   str1TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE_RATE, wxT(""), wxDefaultPosition, 
         wxSize(ATTITUDE_TEXT_CTRL_WIDTH,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   str2TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE_RATE, wxT(""), wxDefaultPosition, 
         wxSize(ATTITUDE_TEXT_CTRL_WIDTH,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   str3TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE_RATE, wxT(""), wxDefaultPosition, 
         wxSize(ATTITUDE_TEXT_CTRL_WIDTH,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );

   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage
         ("AttitudePanel::Create() Creating wxTextCtrl objects\n");
   #endif

   rateUnits1 = new wxStaticText( this, ID_TEXT, wxT("deg/sec"));
   rateUnits2 = new wxStaticText( this, ID_TEXT, wxT("deg/sec"));
   rateUnits3 = new wxStaticText( this, ID_TEXT, wxT("deg/sec"));

   // Dunn added initialization of attUnits.  They were already declared in the
   // code but not yet showing up when attitude state type is Euler Angles.
   //
   // This is not working yet.  Dunn needs to learn more wxWidgets commands first.  Wendy fixed it.
   //attUnits1 = new wxStaticText( this, ID_TEXT, wxT(""));
   //attUnits2 = new wxStaticText( this, ID_TEXT, wxT(""));
   //attUnits3 = new wxStaticText( this, ID_TEXT, wxT(""));

   // create the message to be displayed when the user selects "SpiceAttitude"
   spiceMessage =
      new wxStaticText( this, ID_TEXT, wxT("Set data on the SPICE tab."),
                        wxDefaultPosition, wxSize(staticTextWidth,20), 0); //wxDefaultSize, 0);

   //======================================================================
   //LOJ: Shows how to create text label and text ctrl
   //======================================================================
   // Create body spin axis text label
   spinAxisLabel =
      new wxStaticText(this, ID_TEXT, wxT("Body Spin Axis"), wxDefaultPosition, wxDefaultSize, 0);
   // Create body spin axis text ctrl
   spinAxis1TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_SPIN_AXIS, wxT(""), wxDefaultPosition, 
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   spinAxis2TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_SPIN_AXIS, wxT(""), wxDefaultPosition, 
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   spinAxis3TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_SPIN_AXIS, wxT(""), wxDefaultPosition, 
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   
   // Create nutation reference vector text label
   nutRefVecLabel =
      new wxStaticText(this, ID_TEXT, wxT("Nutation Reference Vector"), wxDefaultPosition, wxDefaultSize, 0);
   // Create nutation reference vector text ctrl
   nutRefVec1TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_NUT_REF_VEC, wxT(""), wxDefaultPosition, 
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   nutRefVec2TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_NUT_REF_VEC, wxT(""), wxDefaultPosition, 
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   nutRefVec3TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_NUT_REF_VEC, wxT(""), wxDefaultPosition, 
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   
   //LOJ: @todo - Add angles/rates data here
   //======================================================================
   // Create labels and text ctrls for other angles and rates
   initPrecAngleLabel =
      new wxStaticText(this, ID_TEXT, wxT("Initial Precession Angle"), wxDefaultPosition, wxDefaultSize, 0);
   initPrecAngleUnits =
      new wxStaticText(this, ID_TEXT, wxT("deg"), wxDefaultPosition, wxDefaultSize, 0);

   initPrecAngleTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_INIT_PREC_ANGLE, wxT(""), wxDefaultPosition,
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));

   precessionRateLabel =
      new wxStaticText(this, ID_TEXT, wxT("Precession Rate"), wxDefaultPosition, wxDefaultSize, 0);
   precessionRateUnits =
      new wxStaticText(this, ID_TEXT, wxT("deg/sec"), wxDefaultPosition, wxDefaultSize, 0);
   precessionRateTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_PRECESSION_RATE, wxT(""), wxDefaultPosition,
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));

   nutationAngleLabel =
      new wxStaticText(this, ID_TEXT, wxT("Nutation Angle"), wxDefaultPosition, wxDefaultSize, 0);
   nutationAngleUnits =
      new wxStaticText(this, ID_TEXT, wxT("deg"), wxDefaultPosition, wxDefaultSize, 0);
   nutationAngleTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_NUTATION_ANGLE, wxT(""), wxDefaultPosition,
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));

   initSpinAngleLabel =
      new wxStaticText(this, ID_TEXT, wxT("Initial Spin Angle"), wxDefaultPosition, wxDefaultSize, 0);
   initSpinAngleUnits =
      new wxStaticText(this, ID_TEXT, wxT("deg"), wxDefaultPosition, wxDefaultSize, 0);
   initSpinAngleTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_INIT_SPIN_ANGLE, wxT(""), wxDefaultPosition,
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));

   spinRateLabel =
      new wxStaticText(this, ID_TEXT, wxT("Spin Rate"), wxDefaultPosition, wxDefaultSize, 0);
   spinRateUnits =
      new wxStaticText(this, ID_TEXT, wxT("deg/sec"), wxDefaultPosition, wxDefaultSize, 0);
   spinRateTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_SPIN_RATE, wxT(""), wxDefaultPosition,
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));

   // Now add the NadirPointing widgets
   referenceVectorLabel =
      new wxStaticText(this, ID_TEXT, wxT("Reference Vector"), wxDefaultPosition, wxDefaultSize, 0);
   refVectorXTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_REFERENCE_VECTOR, wxT(""), wxDefaultPosition,
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   refVectorYTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_REFERENCE_VECTOR, wxT(""), wxDefaultPosition,
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   refVectorZTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_REFERENCE_VECTOR, wxT(""), wxDefaultPosition,
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   constraintVectorLabel =
      new wxStaticText(this, ID_TEXT, wxT("Constraint Vector"), wxDefaultPosition, wxDefaultSize, 0);
   constraintVectorXTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_CONSTRAINT_VECTOR, wxT(""), wxDefaultPosition,
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   constraintVectorYTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_CONSTRAINT_VECTOR, wxT(""), wxDefaultPosition,
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   constraintVectorZTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_CONSTRAINT_VECTOR, wxT(""), wxDefaultPosition,
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   bodyAlignVectorLabel =
      new wxStaticText(this, ID_TEXT, wxT("Body Alignment Vector"), wxDefaultPosition, wxDefaultSize, 0);
   bodyAlignVectorXTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_BODY_ALIGNMENT_VECTOR, wxT(""), wxDefaultPosition,
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   bodyAlignVectorYTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_BODY_ALIGNMENT_VECTOR, wxT(""), wxDefaultPosition,
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   bodyAlignVectorZTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_BODY_ALIGNMENT_VECTOR, wxT(""), wxDefaultPosition,
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   bodyConstraintVectorLabel =
      new wxStaticText(this, ID_TEXT, wxT("Body Constraint Vector"), wxDefaultPosition, wxDefaultSize, 0);
   bodyConstraintVectorXTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_BODY_CONSTRAINT_VECTOR, wxT(""), wxDefaultPosition,
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   bodyConstraintVectorYTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_BODY_CONSTRAINT_VECTOR, wxT(""), wxDefaultPosition,
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   bodyConstraintVectorZTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_BODY_CONSTRAINT_VECTOR, wxT(""), wxDefaultPosition,
                     wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));

   // Reference Body must be a Celestial Body
   attRefBodyLabel =
      new wxStaticText( this, ID_TEXT, wxT(GUI_ACCEL_KEY"Reference Body"),
         wxDefaultPosition, wxSize(staticTextWidth,20), 0); // wxDefaultSize, 0);
   referenceBodyComboBox =  theGuiManager->GetCelestialBodyComboBox(this, ID_CB_REFERENCE_BODY,
      wxDefaultSize);
   referenceBodyComboBox->SetToolTip(pConfig->Read(_T("ReferenceBodyHint")));

   StringArray modes = Attitude::GetModesOfConstraint();
   unsigned int modesSz = modes.size();
   modeOfConstraintArray = new wxString[modesSz];
   for (x = 0; x < modesSz; ++x)
      modeOfConstraintArray[x] = wxT(modes[x].c_str());

   modeOfConstraintLabel =
      new wxStaticText( this, ID_TEXT, wxT(GUI_ACCEL_KEY"Mode of Constraint"),
         wxDefaultPosition, wxSize(staticTextWidth,20), 0); // wxDefaultSize, 0);
   modeOfConstraintComboBox =
         new wxComboBox( this, ID_CB_MODE_OF_CONSTRAINT, wxT(modeOfConstraintArray[0]),
            wxDefaultPosition, wxDefaultSize, modesSz, modeOfConstraintArray,
            wxCB_DROPDOWN|wxCB_READONLY );
   modeOfConstraintComboBox->SetToolTip(pConfig->Read(_T("ModeOfConstraintHint")));


   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage(
         "AttitudePanel::Create() Creating wxBoxSizer objects.\n");
   #endif
   
   Integer bsize = 2; // border size
   
   // wx*Sizers
   wxBoxSizer *boxSizer1 = new wxBoxSizer(wxHORIZONTAL);
   //GmatStaticBoxSizer *boxSizer1 = new GmatStaticBoxSizer( wxHORIZONTAL, this, "" );
   GmatStaticBoxSizer *boxSizer2 = new GmatStaticBoxSizer( wxVERTICAL, this, "" );
   //GmatStaticBoxSizer *boxSizer3 = new GmatStaticBoxSizer( wxVERTICAL, this, "" );
   boxSizer3 = new GmatStaticBoxSizer( wxVERTICAL, this, "" );
   
   
   //======================================================================
   //LOJ: Shows how to create sizers and add controls
   //======================================================================   
   // Create 3 column flex grid sizer for body spin axis text ctrl
   wxFlexGridSizer *spinAxisTCSizer = new wxFlexGridSizer(3);
   spinAxisTCSizer->Add(spinAxis1TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   spinAxisTCSizer->Add(spinAxis2TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   spinAxisTCSizer->Add(spinAxis3TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   // Create 3 column flex grid sizer for nutation reference vector text ctrl
   wxFlexGridSizer *nutRefVecTCSizer = new wxFlexGridSizer(3);
   nutRefVecTCSizer->Add(nutRefVec1TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   nutRefVecTCSizer->Add(nutRefVec2TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   nutRefVecTCSizer->Add(nutRefVec3TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   // Create vertical box sizer for body spin axis label and text ctrl
   wxBoxSizer *spinAxisSizer = new wxBoxSizer(wxVERTICAL);
   spinAxisSizer->Add(spinAxisLabel, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   spinAxisSizer->Add(spinAxisTCSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   // Create vertical box sizer for nutation reference vector
   wxBoxSizer *nutRefVecSizer = new wxBoxSizer(wxVERTICAL);
   nutRefVecSizer->Add(nutRefVecLabel, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   nutRefVecSizer->Add(nutRefVecTCSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   // Create static box sizer for all body spin axis items
   GmatStaticBoxSizer *spinAxisGroupSizer
      = new GmatStaticBoxSizer(wxVERTICAL, this, "Vectors");
   spinAxisGroupSizer->Add(spinAxisSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   spinAxisGroupSizer->Add(nutRefVecSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   wxFlexGridSizer *precSpinAnglesRatesSizer = new wxFlexGridSizer(3,5);
   precSpinAnglesRatesSizer->Add(initPrecAngleLabel, 0, wxALIGN_LEFT|wxALL, bsize );
   precSpinAnglesRatesSizer->Add(initPrecAngleTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
   precSpinAnglesRatesSizer->Add(initPrecAngleUnits, 0, wxALIGN_LEFT|wxALL, bsize );
   precSpinAnglesRatesSizer->Add(precessionRateLabel, 0, wxALIGN_LEFT|wxALL, bsize );
   precSpinAnglesRatesSizer->Add(precessionRateTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
   precSpinAnglesRatesSizer->Add(precessionRateUnits, 0, wxALIGN_LEFT|wxALL, bsize );
   precSpinAnglesRatesSizer->Add(nutationAngleLabel, 0, wxALIGN_LEFT|wxALL, bsize );
   precSpinAnglesRatesSizer->Add(nutationAngleTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
   precSpinAnglesRatesSizer->Add(nutationAngleUnits, 0, wxALIGN_LEFT|wxALL, bsize );
   precSpinAnglesRatesSizer->Add(initSpinAngleLabel, 0, wxALIGN_LEFT|wxALL, bsize );
   precSpinAnglesRatesSizer->Add(initSpinAngleTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
   precSpinAnglesRatesSizer->Add(initSpinAngleUnits, 0, wxALIGN_LEFT|wxALL, bsize );
   precSpinAnglesRatesSizer->Add(spinRateLabel, 0, wxALIGN_LEFT|wxALL, bsize );
   precSpinAnglesRatesSizer->Add(spinRateTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
   precSpinAnglesRatesSizer->Add(spinRateUnits, 0, wxALIGN_LEFT|wxALL, bsize );

   // Create static box sizer for all precessing spinner angles and rates
   GmatStaticBoxSizer *precSpinAnglesRatesGroupSizer
      = new GmatStaticBoxSizer(wxVERTICAL, this, "Angles and Rates");
   precSpinAnglesRatesGroupSizer->Add(precSpinAnglesRatesSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   // Create precessing spinner sizer for spin axis and angles/rates
   precessingSpinnerSizer = new wxBoxSizer(wxVERTICAL);
   precessingSpinnerSizer->Add(spinAxisGroupSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   precessingSpinnerSizer->Add(precSpinAnglesRatesGroupSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   //LOJ: - Add nutRefVecGroupSizer here
   //precessingSpinnerSizer->Add(nutRefVecGroupSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   // Create 3 column flex grid sizer for reference vector
   wxFlexGridSizer *refVectorTCSizer = new wxFlexGridSizer(3);
   refVectorTCSizer->Add(refVectorXTextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   refVectorTCSizer->Add(refVectorYTextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   refVectorTCSizer->Add(refVectorZTextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   // Create 3 column flex grid sizer for constraint vector
   wxFlexGridSizer *conVectorTCSizer = new wxFlexGridSizer(3);
   conVectorTCSizer->Add(constraintVectorXTextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   conVectorTCSizer->Add(constraintVectorYTextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   conVectorTCSizer->Add(constraintVectorZTextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   // Create 3 column flex grid sizer for body alignment vector
   wxFlexGridSizer *bodyAlignVectorTCSizer = new wxFlexGridSizer(3);
   bodyAlignVectorTCSizer->Add(bodyAlignVectorXTextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   bodyAlignVectorTCSizer->Add(bodyAlignVectorYTextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   bodyAlignVectorTCSizer->Add(bodyAlignVectorZTextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   // Create 3 column flex grid sizer for body constraint vector
   wxFlexGridSizer *bodyConstraintVectorTCSizer = new wxFlexGridSizer(3);
   bodyConstraintVectorTCSizer->Add(bodyConstraintVectorXTextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   bodyConstraintVectorTCSizer->Add(bodyConstraintVectorYTextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   bodyConstraintVectorTCSizer->Add(bodyConstraintVectorZTextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   // Create vertical box sizer for reference vector label and text ctrl
   wxBoxSizer *refVectorSizer = new wxBoxSizer(wxVERTICAL);
   refVectorSizer->Add(referenceVectorLabel, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   refVectorSizer->Add(refVectorTCSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   // Create vertical box sizer for constraint vector label and text ctrl
   wxBoxSizer *conVectorSizer = new wxBoxSizer(wxVERTICAL);
   conVectorSizer->Add(constraintVectorLabel, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   conVectorSizer->Add(conVectorTCSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   // Create vertical box sizer for body alignment vector label and text ctrl
   wxBoxSizer *bodyAlignVectorSizer = new wxBoxSizer(wxVERTICAL);
   bodyAlignVectorSizer->Add(bodyAlignVectorLabel, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   bodyAlignVectorSizer->Add(bodyAlignVectorTCSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   // Create vertical box sizer for body alignment vector label and text ctrl
   wxBoxSizer *bodyConstraintVectorSizer = new wxBoxSizer(wxVERTICAL);
   bodyConstraintVectorSizer->Add(bodyConstraintVectorLabel, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   bodyConstraintVectorSizer->Add(bodyConstraintVectorTCSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   // Create horizontal box sizer for reference body label and combobox
   wxBoxSizer *referenceBodySizer = new wxBoxSizer(wxHORIZONTAL);
   referenceBodySizer->Add(attRefBodyLabel, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   referenceBodySizer->Add(referenceBodyComboBox, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   // Create horizontal box sizer for mode of constraint label and combobox
   wxBoxSizer *modeOfConstraintSizer = new wxBoxSizer(wxHORIZONTAL);
   modeOfConstraintSizer->Add(modeOfConstraintLabel, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   modeOfConstraintSizer->Add(modeOfConstraintComboBox, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   // Create static box sizer for nadir pointing reference body and mode of constraint
   GmatStaticBoxSizer *nadirPtBodyAndModeGroupSizer
      = new GmatStaticBoxSizer(wxVERTICAL, this, "Body and Mode");
   nadirPtBodyAndModeGroupSizer->Add(referenceBodySizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   nadirPtBodyAndModeGroupSizer->Add(modeOfConstraintSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   // Create static box sizer for all nadir pointing vector items
   GmatStaticBoxSizer *nadirPtVectorsGroupSizer
      = new GmatStaticBoxSizer(wxVERTICAL, this, "Vectors");
   nadirPtVectorsGroupSizer->Add(refVectorSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   nadirPtVectorsGroupSizer->Add(conVectorSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   nadirPtVectorsGroupSizer->Add(bodyAlignVectorSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   nadirPtVectorsGroupSizer->Add(bodyConstraintVectorSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   // Create nadir pointing sizer for vectors
   nadirPointingSizer = new wxBoxSizer(wxVERTICAL);
   nadirPointingSizer->Add(nadirPtBodyAndModeGroupSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   nadirPointingSizer->Add(nadirPtVectorsGroupSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
//   nadirPointingSizer->Add(nadirPtRefBodyAndModeGroupSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   // Add to right box sizer
   boxSizer3->Add(precessingSpinnerSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   boxSizer3->Hide(precessingSpinnerSizer);
   // Add to right box sizer
   boxSizer3->Add(nadirPointingSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   boxSizer3->Hide(nadirPointingSizer);
   //======================================================================
   
   
   
   attitudeSizer = new GmatStaticBoxSizer( wxVERTICAL, this, "Attitude Initial Conditions" );
   attRateSizer = new GmatStaticBoxSizer( wxVERTICAL, this, "Attitude Rate Initial Conditions" );
   
   wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 2, 0, 0 );
   flexGridSizer2 = new wxFlexGridSizer( 5, 0, 0 );   // need 5 to add attUnits
   wxFlexGridSizer *flexGridSizer3 = new wxFlexGridSizer( 3, 0, 0 );
   // Let's make TextCtrl growable, so we can see more numbers when expand
   // Commented out since it doesn't look good on Linux(LOJ: 2010.02.19)
   //flexGridSizer2->AddGrowableCol( 1 );
   //flexGridSizer2->AddGrowableCol( 2 );
   //flexGridSizer2->AddGrowableCol( 3 );
   //flexGridSizer3->AddGrowableCol( 1 );
   
   // Add to wx*Sizers
   flexGridSizer1->Add(config1StaticText, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );
   flexGridSizer1->Add(config1ComboBox, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );        
   flexGridSizer1->Add(config2StaticText, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );
   flexGridSizer1->Add(coordSysComboBox, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );
   flexGridSizer1->Add(config4StaticText, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );
   flexGridSizer1->Add(config4ComboBox, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );

   flexGridSizer1->Add(spiceMessage, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );
   flexGridSizer1->Add(20, 20, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   flexGridSizer2->Add(st1StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st1TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st5TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st8TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );          
   flexGridSizer2->Add(attUnits1, 0, wxALIGN_CENTER|wxALL, bsize );  // Dunn Added
   
   flexGridSizer2->Add(st2StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st2TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st6TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st9TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );        
   flexGridSizer2->Add(attUnits2, 0, wxALIGN_CENTER|wxALL, bsize );  // Dunn Added
   
   flexGridSizer2->Add(st3StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st3TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st7TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st10TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );       
   flexGridSizer2->Add(attUnits3, 0, wxALIGN_CENTER|wxALL, bsize );  // Dunn Added
   
   flexGridSizer2->Add(st4StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st4TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(20, 20, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer2->Add(20, 20, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);        
   flexGridSizer2->Add(20, 20, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);  // for units
   
   flexGridSizer3->Add(str1StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(str1TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(rateUnits1, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(str2StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(str2TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(rateUnits2, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(str3StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(str3TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );        
   flexGridSizer3->Add(rateUnits3, 0, wxALIGN_CENTER|wxALL, bsize );
   
   attitudeSizer->Add(stateTypeStaticText , 0, wxALIGN_LEFT|wxALL, bsize);
   attitudeSizer->Add(stateTypeComboBox , 0, wxALIGN_LEFT|wxALL, bsize);  
   attitudeSizer->Add(flexGridSizer2, 0, wxGROW|wxALIGN_RIGHT|wxALL, bsize);            
   
   attRateSizer->Add(stateRateTypeStaticText , 0, wxALIGN_LEFT|wxALL, bsize);
   attRateSizer->Add(stateRateTypeComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   attRateSizer->Add(flexGridSizer3, 0, wxGROW|wxALIGN_RIGHT|wxALL, bsize);         
   
   boxSizer2->Add(flexGridSizer1, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   
   boxSizer3->Add(attitudeSizer, 0, wxGROW| wxALIGN_CENTER|wxALL, bsize);
   boxSizer3->Add(attRateSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   boxSizer1->Add( boxSizer2, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   boxSizer1->Add( boxSizer3, 1, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   
   this->SetAutoLayout( true );  
   this->SetSizerAndFit( boxSizer1 );
   boxSizer1->Fit( this );
   boxSizer1->SetSizeHints( this );

   std::string initialModel = config1ComboBox->GetValue().c_str();
   DisplayDataForModel(initialModel);
   
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::Create() exiting\n");
   #endif
}    


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the data from the attitude object into the widgets for the panel.
 */
//------------------------------------------------------------------------------
void AttitudePanel::LoadData()
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::LoadData() entered\n");
   #endif
   
   unsigned int x, y;
   bool newAttitude = false;
   // check to see if the spacecraft has an attitude object
   theAttitude = (Attitude*) theSpacecraft->GetRefObject(Gmat::ATTITUDE, "");
   if (theAttitude == NULL)   // no attitude yet
   {
      #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage
         ("   Attitude is NULL, so try to create %s.\n", attitudeModelArray[0].c_str());
      #endif
      
      theAttitude = (Attitude *)theGuiInterpreter->
         CreateObject((attitudeModelArray[0]).c_str(), ""); // Use no name
      // Set new attitude to spacecraft (LOJ: 2009.03.10)
      theSpacecraft->SetRefObject(theAttitude, Gmat::ATTITUDE);
      newAttitude = true;
   }
   if (theAttitude == NULL)
   {
      std::string ex = "ERROR- unable to find or create an attitude object for ";
      ex += theSpacecraft->GetName() + "\n";
      throw GmatBaseException(ex);
   }
   
   try
   {
      #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("   Now retrieve data from the attitude\n");
      #endif
      
      epoch = theAttitude->GetEpoch();
      
      attStateType     = 
         theAttitude->GetStringParameter("AttitudeDisplayStateType");
      attRateStateType = 
         theAttitude->GetStringParameter("AttitudeRateDisplayStateType");
      attitudeModel    = theAttitude->GetAttitudeModelName();
      config1ComboBox->SetValue(wxT(attitudeModel.c_str()));
      
      eulerSequence  = theAttitude->GetStringParameter("EulerAngleSequence");
      seq            = Attitude::ExtractEulerSequence(eulerSequence);
      config4ComboBox->SetValue(wxT(eulerSequence.c_str()));
   
      attCoordSystem = theAttitude->GetStringParameter("AttitudeCoordinateSystem");
      coordSysComboBox->SetValue(wxT(attCoordSystem.c_str()));
      if (!attCS) attCS  = (CoordinateSystem*)theGuiInterpreter->
                     GetConfiguredObject(attCoordSystem);
      
      //DisplayDataForModel(attitudeModel);
      
      //LOJ: Load data from the base attitude object   - do we need these here, or just the DisplayDataForModel below?
      if (attitudeModel == "PrecessingSpinner")
         LoadPrecessingSpinnerData();
      else if (attitudeModel == "NadirPointing")
      {
         LoadNadirPointingData();
         #ifdef DEBUG_ATTITUDE_PANEL
            MessageInterface::ShowMessage("   NadirPointing data loaded\n");
         #endif
      }

      if (attStateType == "EulerAngles")
      {
         Rvector eaVal = theAttitude->GetRvectorParameter("EulerAngles");
         for (x = 0; x < 3; ++x)
         {
            *eulerAngles[x] = theGuiManager->ToWxString(eaVal[x]);
            ea[x]           = eaVal[x];
         }
         DisplayEulerAngles();
      }
      else if (attStateType == "Quaternion")
      {
         Rvector qVal = theAttitude->GetRvectorParameter("Quaternion");
         for (x = 0; x < 4; ++x)
         {
            *quaternion[x] = theGuiManager->ToWxString(qVal[x]);
            q[x]           = qVal[x];
         }
         DisplayQuaternion();
      }
      else if (attStateType == "MRPs")	// Added by Dunn
      {
         Rvector MRPVal = theAttitude->GetRvectorParameter("MRPs");
         for (x = 0; x < 3; ++x)
         {
            *MRPs[x] = theGuiManager->ToWxString(MRPVal[x]);
            mrp[x]   = MRPVal[x];
         }
         DisplayMRPs();
      }
      else // "DirectionCosineMatrix
      {
         Rmatrix matVal = theAttitude->GetRmatrixParameter("DirectionCosineMatrix");
         for (x = 0; x < 3; ++x)
            for (y = 0; y < 3; ++y)
            {
               *cosineMatrix[x*3+y] = theGuiManager->ToWxString(matVal(x,y));
               dcmat(x,y)             = matVal(x,y);
            }
         DisplayDCM();
      }
   
      if (attRateStateType == "EulerAngleRates") 
      {
         Rvector earVal = theAttitude->GetRvectorParameter("EulerAngleRates");
         for (x = 0; x < 3; ++x)
         {
            *eulerAngleRates[x] = theGuiManager->ToWxString(earVal[x]);
            ear[x]              = earVal[x];
         }
         DisplayEulerAngleRates();
      }
      else // AngularVelocity
      {
         Rvector avVal = theAttitude->GetRvectorParameter("AngularVelocity");
         for (x = 0; x < 3; ++x)
         {
            *angVel[x] = theGuiManager->ToWxString(avVal[x]);
            av[x]      = avVal[x];
         }
         DisplayAngularVelocity();
      }

      #ifdef DEBUG_ATTITUDE_PANEL
         MessageInterface::ShowMessage("   calling DisplayDataForModel\n");
      #endif
      DisplayDataForModel(attitudeModel);
      #ifdef DEBUG_ATTITUDE_PANEL
         MessageInterface::ShowMessage("   AFTER DisplayDataForModel\n");
      #endif

      dataChanged = false;
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves the data from the widgets to the attitude object.
 */
//------------------------------------------------------------------------------
void AttitudePanel::SaveData()
{
   #ifdef DEBUG_ATTITUDE_SAVE
      MessageInterface::ShowMessage("AttitudePanel::SaveData() entered\n");
   #endif
   #ifdef DEBUG_ATTITUDE_SAVE
      MessageInterface::ShowMessage("   theAttitude <%p> is of type %s\n",
            theAttitude, (theAttitude->GetTypeName()).c_str());
      MessageInterface::ShowMessage("   modelModified = %s, seqModified = %s\n",
            (modelModified? "true" : "false"),  (seqModified? "true" : "false"));
      MessageInterface::ShowMessage("   csModified = %s, stateTypeModified = %s\n",
            (csModified? "true" : "false"),  (stateTypeModified? "true" : "false"));
      MessageInterface::ShowMessage("   stateModified = %s, rateStateTypeModified = %s\n",
            (stateModified? "true" : "false"),  (rateStateTypeModified? "true" : "false"));
      MessageInterface::ShowMessage("   stateRateModified = %s\n",
            (stateRateModified? "true" : "false"));
      MessageInterface::ShowMessage("   attStateType = %s\n", attStateType.c_str());
   #endif

   if (!ValidateState("Both"))
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, +
         "Please enter valid value(s) before saving the Attitude data\n");
         canClose = false;
     return;
   }
   canClose    = true;
   dataChanged = false;
   
   // if the user selected a different attitude model, we will need to create it
   bool isNewAttitude    = false;
   bool isModelModified  = modelModified;
   Attitude *useAttitude = NULL;
   if (modelModified)  
   {
      try
      {
         #ifdef DEBUG_ATTITUDE_SAVE
            MessageInterface::ShowMessage(
            "   about to create a new attitude of type %s\n", 
            attitudeModel.c_str());
         #endif
         useAttitude = (Attitude *)theGuiInterpreter->
                       CreateObject(attitudeModel, ""); // Use no name
         #ifdef DEBUG_ATTITUDE_SAVE
            MessageInterface::ShowMessage(
            "   Created a new attitude of type %s <%p>\n",
            attitudeModel.c_str(), useAttitude);
         #endif
      }
      catch (BaseException &ex)
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      }
      isNewAttitude = true;
      modelModified = false;
   }
   else useAttitude = theAttitude;
   
   #ifdef DEBUG_ATTITUDE_SAVE
      if (!useAttitude)
        MessageInterface::ShowMessage("   Attitude pointer is NULL\n");
   #endif
      
   //LOJ: Save data to the base attitude object
   if (attitudeModel == "PrecessingSpinner")
   {
      SavePrecessingSpinnerData(useAttitude);
      if (canClose) 
         dataChanged = false;
//      return;
   }
   else if (attitudeModel == "NadirPointing")
   {
      SaveNadirPointingData(useAttitude);
      if (canClose)
         dataChanged = false;
   }
   
   bool canModifyCS    = useAttitude->CSModifyAllowed();
   bool canSetAttitude = useAttitude->SetInitialAttitudeAllowed();
   try
   {
      if (csModified || isNewAttitude)
      {
         #ifdef DEBUG_ATTITUDE_SAVE
            MessageInterface::ShowMessage("   Setting new coordinate system: %s\n",
            attCoordSystem.c_str());
         #endif
         if (canModifyCS)
         {
            useAttitude->SetStringParameter("AttitudeCoordinateSystem",attCoordSystem);
            useAttitude->SetRefObject(attCS, Gmat::COORDINATE_SYSTEM, attCoordSystem);
         }
         csModified = false;
      }

      if (seqModified || isNewAttitude)
      {
         #ifdef DEBUG_ATTITUDE_SAVE
            MessageInterface::ShowMessage("   Setting new sequence: %s\n",
            eulerSequence.c_str());
         #endif
         useAttitude->SetStringParameter("EulerAngleSequence", eulerSequence);

//         // set attitude state and rate as well, to match what the user sees on the screen
//         if (canSetAttitude)
//         {
//            if (attStateType == stateTypeArray[EULER_ANGLES])
//               useAttitude->SetRvectorParameter("EulerAngles", ea);
//            else if (attStateType == stateTypeArray[QUATERNION])
//               useAttitude->SetRvectorParameter("Quaternion", q);
//            else if (attStateType == stateTypeArray[MRPS])
//               useAttitude->SetRvectorParameter("MRPs", mrp);	 // Added by Dunn
//            else
//               useAttitude->SetRmatrixParameter("DirectionCosineMatrix", dcmat);
//
//            if (attRateStateType == stateRateTypeArray[EULER_ANGLE_RATES])
//               useAttitude->SetRvectorParameter("EulerAngleRates", ear);
//            else
//               useAttitude->SetRvectorParameter("AngularVelocity", av);
//         }
         seqModified = false;
      }
      
      if (stateTypeModified || isNewAttitude)
      {
         #ifdef DEBUG_ATTITUDE_SAVE
            MessageInterface::ShowMessage("   Setting new state type to ...%s\n",
            attStateType.c_str());
         #endif
         useAttitude->SetStringParameter("AttitudeDisplayStateType", attStateType);
      }
         
      if (stateModified || isNewAttitude)
      {
         #ifdef DEBUG_ATTITUDE_SAVE
            MessageInterface::ShowMessage("   Setting new state ...\n");
            if (attStateType == stateTypeArray[QUATERNION])
            {
               MessageInterface::ShowMessage(
                     "Quaternion = %12.10f   %12.10f   %12.10f   %12.10f\n",
                     q[0], q[1], q[2], q[3]);
            }
         #endif
         if (canSetAttitude)
         {
            if (attStateType == stateTypeArray[EULER_ANGLES])
               useAttitude->SetRvectorParameter("EulerAngles", ea);
            else if (attStateType == stateTypeArray[QUATERNION])
               useAttitude->SetRvectorParameter("Quaternion", q);
            else if (attStateType == stateTypeArray[MRPS])
               useAttitude->SetRvectorParameter("MRPs", mrp);	// Dunn Added
            else
               useAttitude->SetRmatrixParameter("DirectionCosineMatrix", dcmat);
         }
         stateModified = false;
      }

      if (rateStateTypeModified || isNewAttitude)
      {
         #ifdef DEBUG_ATTITUDE_SAVE
            MessageInterface::ShowMessage("   Setting new rate state type to ...%s\n",
            attRateStateType.c_str());
         #endif
         useAttitude->SetStringParameter("AttitudeRateDisplayStateType", attRateStateType);
      }
   
      if (stateRateModified || isNewAttitude)
      {
         if (canSetAttitude)
         {
            #ifdef DEBUG_ATTITUDE_SAVE
               MessageInterface::ShowMessage("   Setting new state rate ...\n");
               MessageInterface::ShowMessage("   state rate type = %s\n", attRateStateType.c_str());
               MessageInterface::ShowMessage("   ear = %s\n", ear.ToString().c_str());
               MessageInterface::ShowMessage("   av  = %s\n", av.ToString().c_str());
            #endif
            if (attRateStateType == stateRateTypeArray[EULER_ANGLE_RATES])
            {
               useAttitude->SetRvectorParameter("EulerAngleRates", ear);
            }
            else
               useAttitude->SetRvectorParameter("AngularVelocity", av);
         }
         stateRateModified = false;
      }
      
      if (isNewAttitude)
      {
         #ifdef DEBUG_ATTITUDE_SAVE
            MessageInterface::ShowMessage("Setting new attitude model <%p> of type %s on spacecraft %s<%p>\n",
            useAttitude, attitudeModel.c_str(), theSpacecraft->GetName().c_str(), theSpacecraft);
         #endif
         theSpacecraft->SetRefObject(useAttitude, Gmat::ATTITUDE, "");
         // spacecraft deletes the old attitude pointer
         theAttitude = useAttitude;
      }
   }
   catch (BaseException &ex)
   {
      canClose      = false;
      dataChanged   = true;
	   modelModified = isModelModified;
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
   }
   if (canClose) 
   {
		ResetStateFlags("Both", canClose);
		dataChanged = false;
   }
}

//------------------------------------------------------------------------------
// bool IsStateModified(const std::string which = "Both")
//------------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not the state has been modified by the
 * user.
 *
 * @param   which   indicates for which to return the flag - attitude and/or
 *                  rate
 *
 * @return has the specified state(s) been modified?
 */
//------------------------------------------------------------------------------
bool AttitudePanel::IsStateModified(const std::string which)
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::IsStateModified() entered\n");
   #endif
   if ((which == "State")  || (which == "Both"))
   {
      if (attStateType == stateTypeArray[EULER_ANGLES])
      {
         for (Integer ii = 0; ii<3; ii++)
            if (eaModified[ii]) return true;
      }
      else if (attStateType == stateTypeArray[QUATERNION])
      {
         for (Integer ii = 0; ii < 4; ii++)
            if (qModified[ii]) return true;
      }
      else if (attStateType == stateTypeArray[MRPS]) // Dunn Added
      {
         for (Integer ii = 0; ii < 3; ii++)
            if (mrpModified[ii]) return true;
      }
      else if (attStateType == stateTypeArray[DCM])
      {
         for (Integer ii = 0; ii < 9; ii++)
            if (dcmatModified[ii]) return true;
      }
      return false;
   }
   if ((which == "Rate") || (which == "Both"))
   {
      if (attRateStateType == stateRateTypeArray[EULER_ANGLE_RATES])
      {
         for (Integer ii = 0; ii<3; ii++)
            if (earModified[ii]) return true;
      }
      else if (attRateStateType == stateRateTypeArray[ANGULAR_VELOCITY])
      {
         for (Integer ii = 0; ii<3; ii++)
            if (avModified[ii]) return true;
      }
      return false;
   }
   return false;
}


//------------------------------------------------------------------------------
// void ResetStateFlags(const std::string which = "Both",
//                      bool discardEdits = false)
//------------------------------------------------------------------------------
/**
 * Resets the modified flags; on option, discards user edits.
 *
 * @param   which   indicates for which to reset the flags - attitude and/or
 *                  rate
 */
//------------------------------------------------------------------------------
void AttitudePanel::ResetStateFlags(const std::string which,
                                    bool discardEdits)
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::ResetStateFlags() entered\n");
   #endif
   if ((which == "State") || (which == "Both"))
   {
      for (Integer ii = 0; ii < 9; ii++)
         dcmatModified[ii]   = false;
      for (Integer ii = 0; ii < 4; ii++)
         qModified[ii]   = false;
      for (Integer ii = 0; ii < 3; ii++)  // Dunn Added
         mrpModified[ii]   = false;
      for (Integer ii = 0; ii < 3; ii++)
         earModified[ii]  = false;
      if (discardEdits)
      {
         st1TextCtrl->DiscardEdits();
         st2TextCtrl->DiscardEdits();
         st3TextCtrl->DiscardEdits();
         st4TextCtrl->DiscardEdits();
         st5TextCtrl->DiscardEdits();
         st6TextCtrl->DiscardEdits();
         st7TextCtrl->DiscardEdits();
         st8TextCtrl->DiscardEdits();
         st9TextCtrl->DiscardEdits();
         st10TextCtrl->DiscardEdits();
      }
   }
   if ((which == "Rate") || (which == "Both"))
   {
      for (Integer ii = 0; ii < 3; ii++)
      {
         eaModified[ii]   = false;
         avModified[ii]   = false;
      }
      if (discardEdits)
      {
         str1TextCtrl->DiscardEdits();
         str2TextCtrl->DiscardEdits();
         str3TextCtrl->DiscardEdits();
      }
   }

}

//------------------------------------------------------------------------------
// bool ValidateState(const std::string which = "Both",
//                    bool checkForSingularity = false)
//------------------------------------------------------------------------------
/**
 * Validates the state specified.
 *
 * @param   which   indicates which to validate - attitude and/or rate
 *
 * @return is the state specified valid?
 */
//------------------------------------------------------------------------------
bool AttitudePanel::ValidateState(const std::string which,
                                  bool checkForSingularity)
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::ValidateState() entered\n");
      MessageInterface::ShowMessage("which = %s,   checkForSingularity = %s\n",
            which.c_str(), (checkForSingularity? "true" : "false"));
      MessageInterface::ShowMessage("attStateType = %s\n", attStateType.c_str());
   #endif
   bool retval = true;
   std::string strVal;
   Real        tmpVal;
   if ((which == "State") || (which == "Both"))
   {
      if (attStateType == stateTypeArray[EULER_ANGLES])
      {
         if (eaModified[0] || checkForSingularity)
         {
            strVal = st1TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "Euler Angle 1", "Real Number"))
               retval = false;
            else  ea[0] = tmpVal;
         }
         if (eaModified[1] || checkForSingularity)
         {
            strVal = st2TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "Euler Angle 2", "Real Number"))
               retval = false;
            else  ea[1] = tmpVal;
         }
         if (eaModified[2] || checkForSingularity)
         {
            strVal = st3TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "Euler Angle 3", "Real Number"))
               retval = false;
            else  ea[2] = tmpVal;
         }
         if (retval && checkForSingularity)
         {
            Real ea2 = ea[1];
            while (ea2 <= -360.0) ea2 += 360.0;
            while (ea2 >=  360.0) ea2 -= 360.0;
            Real ea2Radians = ea2 * GmatMathConstants::RAD_PER_DEG;
            std::string eseq = config4ComboBox->GetValue().c_str();
            UnsignedIntArray eseqInt = Attitude::ExtractEulerSequence(eseq);
            if (((eseqInt[0] == eseqInt[2]) && GmatMathUtil::Abs(GmatMathUtil::Sin(ea2Radians)) < EULER_ANGLE_TOLERANCE) ||
                ((eseqInt[0] != eseqInt[2]) && GmatMathUtil::Abs(GmatMathUtil::Cos(ea2Radians)) < EULER_ANGLE_TOLERANCE))
            {
               std::stringstream errmsg;
               errmsg << "The attitude defined by the euler angles (";
               errmsg << ea(0) << ", " << ea(1) << ", " << ea(2);
               errmsg << ") is near a singularity.  The allowed values are:\n";
               errmsg << "For a symmetric sequence, EulerAngle2 != 0\n";
               errmsg << "For a non-symmetric sequence, EulerAngle2 != 90\n";
               errmsg << "The tolerance on EulerAngle2 singularity is ";
               errmsg << EULER_ANGLE_TOLERANCE << ".\n";
               MessageInterface::PopupMessage(Gmat::ERROR_, errmsg.str());
               retval = false;
            }
         }
      }
      else if (attStateType == stateTypeArray[QUATERNION])
      {
         #ifdef DEBUG_ATTITUDE_PANEL
            MessageInterface::ShowMessage("   qModified[0] = %s\n",
                  (qModified[0]? "true" : "false"));
            MessageInterface::ShowMessage("   qModified[1] = %s\n",
                  (qModified[1]? "true" : "false"));
            MessageInterface::ShowMessage("   qModified[2] = %s\n",
                  (qModified[2]? "true" : "false"));
            MessageInterface::ShowMessage("   qModified[3] = %s\n",
                  (qModified[3]? "true" : "false"));
         #endif
         if (qModified[0])
         {
            strVal = st1TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "q1", "Real Number"))
               retval = false;
            else  q[0] = tmpVal;
         }
         if (qModified[1])
         {
            strVal = st2TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "q2", "Real Number"))
               retval = false;
            else  q[1] = tmpVal;
         }
         if (qModified[2])
         {
            strVal = st3TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "q3", "Real Number"))
               retval = false;
            else  q[2] = tmpVal;
         }
         if (qModified[3])
         {
            strVal = st4TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "q4", "Real Number"))
               retval = false;
            else  q[3] = tmpVal;
         }
         #ifdef DEBUG_ATTITUDE_PANEL
            MessageInterface::ShowMessage("at end of quaternion check, retval = %s\n",
                  (retval? "true" : "false"));
         #endif
      }
      else if (attStateType == stateTypeArray[MRPS])  // Dunn Added
      {
         if (mrpModified[0])
         {
            strVal = st1TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "MRP 1", "Real Number"))
               retval = false;
            else  mrp[0] = tmpVal;
         }
         if (mrpModified[1]) 
         {
            strVal = st2TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "MRP 2", "Real Number"))
               retval = false;
            else  mrp[1] = tmpVal;
         }
         if (mrpModified[2])
         {
            strVal = st3TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "MRP 3", "Real Number"))
               retval = false;
            else  mrp[2] = tmpVal;
         }
      }
      else if (attStateType == stateTypeArray[DCM])
      {
         if (dcmatModified[0] || checkForSingularity)
         {
            strVal = st1TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "DCM 1,1", "Real Number"))
               retval = false;
            else  dcmat(0,0) = tmpVal;
         }
         if (dcmatModified[1] || checkForSingularity)
         {
            strVal = st5TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "DCM 1,2", "Real Number"))
               retval = false;
            else  dcmat(0,1) = tmpVal;
         }
         if (dcmatModified[2] || checkForSingularity)
         {
            strVal = st8TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "DCM 1,3", "Real Number"))
               retval = false;
            else  dcmat(0,2) = tmpVal;
         }
         if (dcmatModified[3] || checkForSingularity)
         {
            strVal = st2TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "DCM 2,1", "Real Number"))
               retval = false;
            else  dcmat(1,0) = tmpVal;
         }
         if (dcmatModified[4] || checkForSingularity)
         {
            strVal = st6TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "DCM 2,2", "Real Number"))
               retval = false;
            else  dcmat(1,1) = tmpVal;
         }
         if (dcmatModified[5] || checkForSingularity)
         {
            strVal = st9TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "DCM 2,3", "Real Number"))
               retval = false;
            else  dcmat(1,2) = tmpVal;
         }
         if (dcmatModified[6] || checkForSingularity)
         {
            strVal = st3TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "DCM 3,1", "Real Number"))
               retval = false;
            else  dcmat(2,0) = tmpVal;
         }
         if (dcmatModified[7] || checkForSingularity)
         {
            strVal = st7TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "DCM 3,2", "Real Number"))
               retval = false;
            else  dcmat(2,1) = tmpVal;
         }
         if (dcmatModified[8] || checkForSingularity)
         {
            strVal = st10TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "DCM 3,3", "Real Number"))
               retval = false;
            else  dcmat(2,2) = tmpVal;
         }
         #ifdef DEBUG_ATTITUDE_PANEL_DCM
            MessageInterface::ShowMessage("in AttitudePanel::ValidateState()\n");
            MessageInterface::ShowMessage("dcmat = %s\n",dcmat.ToString().c_str());
         #endif
         if (retval && checkForSingularity)
         {
            if (!dcmat.IsOrthogonal(DCM_ORTHONORMALITY_TOLERANCE))
            {
               std::ostringstream errmsg;
               errmsg << "The value [";
               errmsg << dcmat(0,0) << " " << dcmat(0,1) << " " << dcmat(0,2) << " ";
               errmsg << dcmat(1,0) << " " << dcmat(1,1) << " " << dcmat(1,2) << " ";
               errmsg << dcmat(2,0) << " " << dcmat(2,1) << " " << dcmat(2,2);
               errmsg << "] for a cosine matrix is not an allowed value." << std::endl;
               errmsg << "The allowed values are: [orthogonal matrix].\n";
               errmsg << "The tolerance on orthonormality is ";
               errmsg << DCM_ORTHONORMALITY_TOLERANCE << ".\n";
               MessageInterface::PopupMessage(Gmat::ERROR_, errmsg.str());
               retval = false;
            }
            for (unsigned int ii = 0; ii < 3; ii++)
            {
               for (unsigned int jj = 0; jj < 3; jj++)
               {
                  if ((dcmat(ii,jj) < -1.0 - GmatRealConstants::REAL_EPSILON) ||
                      (dcmat(ii,jj) >  1.0 + GmatRealConstants::REAL_EPSILON))
                  {
                     std::ostringstream errmsg;
                     errmsg << "The value \"";
                     errmsg << dcmat(ii,jj);
                     errmsg << "\" for cosine matrix element \"DCM" << ii + 1 << jj + 1 << "\" is not an allowed value.\n";
                     errmsg << "The allowed values are: [-1.0 <= Real Number <= 1.0].\n";
                     MessageInterface::PopupMessage(Gmat::ERROR_, errmsg.str());
                     retval = false;
                  }
               }
            }
         }
      }
   }
   if ((which == "Rate") || (which == "Both"))
   {
      if (attRateStateType == stateRateTypeArray[EULER_ANGLE_RATES])
      {
         if (earModified[0])
         {
            strVal = str1TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "Euler Angle Rate 1", "Real Number"))
               retval = false;
            else  ear[0] = tmpVal;
         }
         if (earModified[1])
         {
            strVal = str2TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "Euler Angle Rate 2", "Real Number"))
               retval = false;
            else  ear[1] = tmpVal;
         }
         if (earModified[2])
         {
            strVal = str3TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "Euler Angle Rate 3", "Real Number"))
               retval = false;
            else  ear[2] = tmpVal;
         }
         #ifdef DEBUG_ATTITUDE_SAVE
                  MessageInterface::ShowMessage(">>>> ear has been set to: %s\n", ear.ToString().c_str());
         #endif
      }
      else if (attRateStateType == stateRateTypeArray[ANGULAR_VELOCITY])
      {
         if (avModified[0])
         {
            strVal = str1TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "Angular Velocity X", "Real Number"))
               retval = false;
            else  av[0] = tmpVal;
         }
         if (avModified[1])
         {
            strVal = str2TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "Angular Velocity Y", "Real Number"))
               retval = false;
            else  av[1] = tmpVal;
         }
         if (avModified[2])
         {
            strVal = str3TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "Angular Velocity Z", "Real Number"))
               retval = false;
            else  av[2] = tmpVal;
         }
         #ifdef DEBUG_ATTITUDE_SAVE
                  MessageInterface::ShowMessage(">>>> av has been set to: %s\n", av.ToString().c_str());
         #endif
      }
   }
   canClose = retval;
   return retval;
}

//------------------------------------------------------------------------------
// void HideInitialAttitudeAndRate()
//------------------------------------------------------------------------------
/**
 * Disables the initial attitude rate widgets.
 *
 */
//------------------------------------------------------------------------------
void AttitudePanel::HideInitialAttitudeAndRate()
{
   // can hide entire sizers here
   boxSizer3->Hide(attitudeSizer); // , true);
   boxSizer3->Hide(attRateSizer); // , true);
   boxSizer3->Layout();
   Refresh();
}

//------------------------------------------------------------------------------
// void ShowInitialAttitudeAndRate()
//------------------------------------------------------------------------------
/**
 * Enables the initial attitude rate widgets.
 *
 */
//------------------------------------------------------------------------------
void AttitudePanel::ShowInitialAttitudeAndRate()
{
   //LOJ: Added to hide precessing spinner data
   boxSizer3->Hide(precessingSpinnerSizer);
   boxSizer3->Hide(nadirPointingSizer);
   
   boxSizer3->Show(attitudeSizer, true);
   boxSizer3->Show(attRateSizer, true);

   // all representations show these three widgets
   st1TextCtrl->Show(true);
   st2TextCtrl->Show(true);
   st3TextCtrl->Show(true);

   if (attStateType == stateTypeArray[EULER_ANGLES])
   {
      st1StaticText->Show(true);
      st2StaticText->Show(true);
      st3StaticText->Show(true);
      st4StaticText->Show(false);

      ResizeTextCtrl1234();  // true);  // add true for wider text box

      st4TextCtrl->Show(false);

      st5TextCtrl->Show(false);
      st6TextCtrl->Show(false);
      st7TextCtrl->Show(false);

      st8TextCtrl->Show(false);
      st9TextCtrl->Show(false);
      st10TextCtrl->Show(false);

      attUnits1->Show(true);
      attUnits2->Show(true);
      attUnits3->Show(true);
   }
   else if (attStateType == stateTypeArray[QUATERNION])
   {
      st1StaticText->Show(true);
      st2StaticText->Show(true);
      st3StaticText->Show(true);
      st4StaticText->Show(true);

      ResizeTextCtrl1234(true);

      st4TextCtrl->Show(true);

      st5TextCtrl->Show(false);
      st6TextCtrl->Show(false);
      st7TextCtrl->Show(false);

      st8TextCtrl->Show(false);
      st9TextCtrl->Show(false);
      st10TextCtrl->Show(false);

      attUnits1->Show(false);
      attUnits2->Show(false);
      attUnits3->Show(false);
   }
   else if (attStateType == stateTypeArray[DCM])
   {
      st1StaticText->Show(false);
      st2StaticText->Show(false);
      st3StaticText->Show(false);
      st4StaticText->Show(false);

      ResizeTextCtrl1234();

      st4TextCtrl->Show(false);

      st5TextCtrl->Show(true);
      st6TextCtrl->Show(true);
      st7TextCtrl->Show(true);

      st8TextCtrl->Show(true);
      st9TextCtrl->Show(true);
      st10TextCtrl->Show(true);

      attUnits1->Show(false);
      attUnits2->Show(false);
      attUnits3->Show(false);
   }
   else if (attStateType == stateTypeArray[MRPS])   // Added by Dunn
   {
      st1StaticText->Show(true);
      st2StaticText->Show(true);
      st3StaticText->Show(true);
      st4StaticText->Show(false);

      ResizeTextCtrl1234();  // true); // add true for wider text box

      st4TextCtrl->Show(false);

      st5TextCtrl->Show(false);
      st6TextCtrl->Show(false);
      st7TextCtrl->Show(false);

      st8TextCtrl->Show(false);
      st9TextCtrl->Show(false);
      st10TextCtrl->Show(false);

      attUnits1->Show(false);
      attUnits2->Show(false);
      attUnits3->Show(false);
   }

   // both rate representations show these three widgets
   str1TextCtrl->Show(true);
   str2TextCtrl->Show(true);
   str3TextCtrl->Show(true);

   boxSizer3->Layout();
   Refresh();
}

//LOJ: Added
//------------------------------------------------------------------------------
// void ShowPrecessingSpinnerData()
//------------------------------------------------------------------------------
void AttitudePanel::ShowPrecessingSpinnerData()
{
   //MessageInterface::ShowMessage("AttitudePanel::ShowPrecessingSpinnerData() entered\n");
   // Hide initial attitude and rate
   HideInitialAttitudeAndRate();
   boxSizer3->Hide(nadirPointingSizer);
   boxSizer3->Show(precessingSpinnerSizer);
   boxSizer3->Layout();
}

//------------------------------------------------------------------------------
// void ShowNadirPointingData()
//------------------------------------------------------------------------------
void AttitudePanel::ShowNadirPointingData()
{
   // Hide initial attitude and rate
   HideInitialAttitudeAndRate();
   boxSizer3->Hide(precessingSpinnerSizer);
   boxSizer3->Show(nadirPointingSizer);
   boxSizer3->Layout();
}

//------------------------------------------------------------------------------
// void DisableAll()
//------------------------------------------------------------------------------
/**
 * Disables the widgets.
 *
 *@note Currently not used
 */
//------------------------------------------------------------------------------
void AttitudePanel::DisableAll()
{
   HideInitialAttitudeAndRate();
//   config1StaticText->Disable();
   config2StaticText->Disable();
//   config3StaticText->Disable();
   config4StaticText->Disable();

   // just want this one disabled, not hidden
   coordSysComboBox->Disable();

   config4ComboBox->Disable();
   stateTypeStaticText->Disable();
   stateTypeComboBox->Disable();
   stateRateTypeStaticText->Disable();
   st1StaticText->Disable();
   st2StaticText->Disable();
   st3StaticText->Disable();
   st1TextCtrl->Disable();
   st2TextCtrl->Disable();
   st3TextCtrl->Disable();

   if (attStateType == STATE_TEXT[QUATERNION])
   {
      st4StaticText->Disable();
      st4TextCtrl->Disable();
   }
   if (attStateType == STATE_TEXT[DCM])
   {
      st5TextCtrl->Disable();
      st6TextCtrl->Disable();
      st7TextCtrl->Disable();
      st8TextCtrl->Disable();
      st9TextCtrl->Disable();
      st10TextCtrl->Disable();
   }
   st1TextCtrl->Disable();
}

//------------------------------------------------------------------------------
// void EnableAll()
//------------------------------------------------------------------------------
/**
 * Enables all the widgets.
 *
 */
//------------------------------------------------------------------------------
void AttitudePanel::EnableAll()
{
//   config1StaticText->Enable();
   config2StaticText->Enable();
//   config3StaticText->Enable();
   config4StaticText->Enable();

   // just want this one enabled - we don't show/hide this one
   coordSysComboBox->Enable();

   config4ComboBox->Enable();
   stateTypeStaticText->Enable();
   stateTypeComboBox->Enable();
   stateRateTypeStaticText->Enable();
   st1StaticText->Enable();
   st2StaticText->Enable();
   st3StaticText->Enable();
   st1TextCtrl->Enable();
   st2TextCtrl->Enable();
   st3TextCtrl->Enable();

   if (attStateType == STATE_TEXT[QUATERNION])
   {
      st4StaticText->Enable();
      st4TextCtrl->Enable();
   }
   if (attStateType == STATE_TEXT[DCM])
   {
      st5TextCtrl->Enable();
      st6TextCtrl->Enable();
      st7TextCtrl->Enable();
      st8TextCtrl->Enable();
      st9TextCtrl->Enable();
      st10TextCtrl->Enable();
   }
   st1TextCtrl->Enable();
}

//------------------------------------------------------------------------------
// void DisplayDataForModel(const std::string &modelType)
//------------------------------------------------------------------------------
/**
 * Displays the data that are allowed to be modified for the specified
 * attitude model type.  Disallowed fields/data are grayed-out.
 *
 * @param modelType   attitude model type
 */
//------------------------------------------------------------------------------
void AttitudePanel::DisplayDataForModel(const std::string &modelType)
{
   // need to create a temporary attitude object in order to query it
   Attitude *tmpAttitude = (Attitude *)theGuiInterpreter->
                           CreateObject(modelType, "", 0);

   //LOJ: Modified to show precessing spinner data
   if (modelType == "PrecessingSpinner")
   {
      if (!precessingSpinnerDataLoaded)
         LoadPrecessingSpinnerData();
      ShowPrecessingSpinnerData();
   }
   else if (modelType == "NadirPointing")
   {
      if (!nadirPointingDataLoaded)
         LoadNadirPointingData();
      ShowNadirPointingData();
   }
   else
   {
      // Show everything that should be shown, then enable it all
      ShowInitialAttitudeAndRate();
      EnableAll();
   }
   if (!tmpAttitude->CSModifyAllowed())
   {
      coordSysComboBox->Disable();
   }
   if (!tmpAttitude->SetInitialAttitudeAllowed())
   {
      HideInitialAttitudeAndRate();
   }
   if (modelType == "SpiceAttitude")
   {
      spiceMessage->Show(true);
   }
   else
   {
      spiceMessage->Show(false);
   }
   delete tmpAttitude;
}

//------------------------------------------------------------------------------
// void ResizeTextCtrl1234(bool forQuaternion = false)
//------------------------------------------------------------------------------
/**
 * Resizes the four text controls.
 *
 * @param forQuaternion flag indicating whether or not these text controls
 *                      are for the quaternion OR we want a wider text box
 *
 */
//------------------------------------------------------------------------------
void AttitudePanel::ResizeTextCtrl1234(bool forQuaternion)
{
   int width;
   if (forQuaternion)
      width = QUATERNION_TEXT_CTRL_WIDTH;
   else
      width = ATTITUDE_TEXT_CTRL_WIDTH;

   // tell the grid sizer the new size of the text boxes
   int w, h;
   st1TextCtrl->GetSize(&w, &h);
   flexGridSizer2->SetItemMinSize(st1TextCtrl, width, h);
   st2TextCtrl->GetSize(&w, &h);
   flexGridSizer2->SetItemMinSize(st2TextCtrl, width, h);
   st3TextCtrl->GetSize(&w, &h);
   flexGridSizer2->SetItemMinSize(st3TextCtrl, width, h);
   st4TextCtrl->GetSize(&w, &h);
   flexGridSizer2->SetItemMinSize(st4TextCtrl, width, h);

   flexGridSizer2->Layout();
}


//------------------------------------------------------------------------------
// wxString ToString(Real rval)
//------------------------------------------------------------------------------
/**
 * Converts a real value to a string.
 *
 * @param rval   real value
 *
 * @return string representation of the input real value
 */
//------------------------------------------------------------------------------
wxString AttitudePanel::ToString(Real rval)
{
   return theGuiManager->ToWxString(rval);
}


//------------------------------------------------------------------------------
// void OnStateTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles case when user updates the state text.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void AttitudePanel::OnStateTextUpdate(wxCommandEvent &event)
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::OnStateTextUpdate() entered\n");
   #endif
   
   if (attStateType == STATE_TEXT[EULER_ANGLES])
   {
      if (st1TextCtrl->IsModified())  eaModified[0]  = true;
      if (st2TextCtrl->IsModified())  eaModified[1]  = true;
      if (st3TextCtrl->IsModified())  eaModified[2]  = true;
   }
   else if (attStateType == STATE_TEXT[QUATERNION])
   {
      if (st1TextCtrl->IsModified())  qModified[0]   = true;
      if (st2TextCtrl->IsModified())  qModified[1]   = true;
      if (st3TextCtrl->IsModified())  qModified[2]   = true;
      if (st4TextCtrl->IsModified())  qModified[3]   = true;
   }
   else if (attStateType == STATE_TEXT[MRPS])   // Dunn Added
   {
      if (st1TextCtrl->IsModified())  mrpModified[0]  = true;
      if (st2TextCtrl->IsModified())  mrpModified[1]  = true;
      if (st3TextCtrl->IsModified())  mrpModified[2]  = true;
   }
   else // DCM
   {
      if (st1TextCtrl->IsModified())  dcmatModified[0] = true;
      if (st2TextCtrl->IsModified())  dcmatModified[3] = true;
      if (st3TextCtrl->IsModified())  dcmatModified[6] = true;
      // not st4TextCtrl - Used for Q4 only
      if (st5TextCtrl->IsModified())  dcmatModified[1] = true;
      if (st6TextCtrl->IsModified())  dcmatModified[4] = true;
      if (st7TextCtrl->IsModified())  dcmatModified[7] = true;
      if (st8TextCtrl->IsModified())  dcmatModified[2] = true;
      if (st9TextCtrl->IsModified())  dcmatModified[5] = true;
      if (st10TextCtrl->IsModified()) dcmatModified[8] = true;
   }
    
   if (IsStateModified("State"))
   {
      stateModified = true;
      dataChanged   = true;
      theScPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnStateRateTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles case when user updates the state rate text.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void AttitudePanel::OnStateRateTextUpdate(wxCommandEvent &event)
{
   //if (!canClose) return;  // ??
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::OnStateRateTextUpdate() entered\n");
   #endif
   
   if (attRateStateType == stateRateTypeArray[EULER_ANGLE_RATES])
   {
      if (str1TextCtrl->IsModified())  earModified[0] = true;
      if (str2TextCtrl->IsModified())  earModified[1] = true;
      if (str3TextCtrl->IsModified())  earModified[2] = true;
   }
   else // ANGULAR_VELOCITY
   {
      if (str1TextCtrl->IsModified())  avModified[0]  = true;
      if (str2TextCtrl->IsModified())  avModified[1]  = true;
      if (str3TextCtrl->IsModified())  avModified[2]  = true;
   }
    
   if (IsStateModified("Rate"))
   {
      stateRateModified = true;
      dataChanged   = true;
      theScPanel->EnableUpdate(true);
   }
}

//LOJ: Added to trigger new value
//------------------------------------------------------------------------------
// void AttitudePanel::OnSpinAxisTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnSpinAxisTextUpdate(wxCommandEvent &event)
{
   spinAxisModified = true;
   dataChanged      = true;
   theScPanel->EnableUpdate(true);
}

//LOJ: Added to trigger new value
//------------------------------------------------------------------------------
// void OnNutationRefVectorTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnNutationRefVectorTextUpdate(wxCommandEvent &event)
{
   nutRefVecModified = true;
   dataChanged       = true;
   theScPanel->EnableUpdate(true);
}

//WCS: Added to trigger new value
//------------------------------------------------------------------------------
// void OnInitialPrecessionAngleTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnInitialPrecessionAngleTextUpdate(wxCommandEvent &event)
{
   initPrecAngleModified = true;
   dataChanged           = true;
   theScPanel->EnableUpdate(true);
}

//WCS: Added to trigger new value
//------------------------------------------------------------------------------
// void OnPrecessionRateTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnPrecessionRateTextUpdate(wxCommandEvent &event)
{
   precessionRateModified = true;
   dataChanged            = true;
   theScPanel->EnableUpdate(true);
}

//WCS: Added to trigger new value
//------------------------------------------------------------------------------
// void OnNutationAngleTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnNutationAngleTextUpdate(wxCommandEvent &event)
{
   nutationAngleModified = true;
   dataChanged           = true;
   theScPanel->EnableUpdate(true);
}

//WCS: Added to trigger new value
//------------------------------------------------------------------------------
// void OnInitialSpinAngleTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnInitialSpinAngleTextUpdate(wxCommandEvent &event)
{
   initSpinAngleModified = true;
   dataChanged           = true;
   theScPanel->EnableUpdate(true);
}

//WCS: Added to trigger new value
//------------------------------------------------------------------------------
// void OnSpinRateTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnSpinRateTextUpdate(wxCommandEvent &event)
{
   spinRateModified = true;
   dataChanged      = true;
   theScPanel->EnableUpdate(true);
}

//WCS: Added to trigger new value
//------------------------------------------------------------------------------
// void OnReferenceVectorTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnReferenceVectorTextUpdate(wxCommandEvent &event)
{
   refVectorModified = true;
   dataChanged       = true;
   theScPanel->EnableUpdate(true);
}

//WCS: Added to trigger new value
//------------------------------------------------------------------------------
// void OnConstraintVectorTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnConstraintVectorTextUpdate(wxCommandEvent &event)
{
   constraintVectorModified = true;
   dataChanged              = true;
   theScPanel->EnableUpdate(true);
}

//WCS: Added to trigger new value
//------------------------------------------------------------------------------
// void OnBodyAlignmentVectorTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnBodyAlignmentVectorTextUpdate(wxCommandEvent &event)
{
   bodyAlignVectorModified = true;
   dataChanged             = true;
   theScPanel->EnableUpdate(true);
}

//WCS: Added to trigger new value
//------------------------------------------------------------------------------
// void OnBodyConstraintVectorTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnBodyConstraintVectorTextUpdate(wxCommandEvent &event)
{
   bodyConstraintVectorModified = true;
   dataChanged                  = true;
   theScPanel->EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnCoordinateSystemSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles case when user selects the coordinate system from the combo box.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void AttitudePanel::OnCoordinateSystemSelection(wxCommandEvent &event)
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::OnCoordinateSystemSelection() entered\n");
   #endif
   std::string newCS = coordSysComboBox->GetValue().c_str();
   if (newCS == attCoordSystem) return;
   // first, validate the state
   if (!ValidateState("Both"))
   {
      coordSysComboBox->SetValue(wxT(attCoordSystem.c_str()));
      MessageInterface::PopupMessage(Gmat::ERROR_, +
         "Please enter valid value(s) before changing the Reference Coordinate System\n");
         return;
   }
   if (!attCS) attCS  = (CoordinateSystem*)theGuiInterpreter->
                        GetConfiguredObject(attCoordSystem);
   fromCS = attCS;
   toCS   = (CoordinateSystem*)theGuiInterpreter->
             GetConfiguredObject(newCS);
             
   // convert things here ***** TBD ********
   
   csModified     = true;
   dataChanged    = true;
   attCoordSystem = newCS;
   attCS          = toCS;
   theScPanel->EnableUpdate(true);
   
   // until know how to convert to new reference coordinate system .........
//   coordSysComboBox->SetValue(wxT(attCoordSystem.c_str()));
//   MessageInterface::PopupMessage(Gmat::WARNING_, +
//      "Conversion of Attitude to a new Reference Coordinate System not yet implemented\n");
   return;
}

//------------------------------------------------------------------------------
// void OnAttitudeModelSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles case when user updates the attitude model.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void AttitudePanel::OnAttitudeModelSelection(wxCommandEvent &event)
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::OnAttitudeModelSelection() entered\n");
   #endif
   // if the user changes the attitude model, we will need to create a new one
    std::string newModel = config1ComboBox->GetValue().c_str();
    if (newModel != attitudeModel)
    {
      modelModified = true;
      dataChanged   = true;
      attitudeModel = newModel;
      theScPanel->EnableUpdate(true);
    }

    DisplayDataForModel(newModel);
}

//------------------------------------------------------------------------------
// void OnReferenceBodySelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles case when user updates the reference body.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void AttitudePanel::OnReferenceBodySelection(wxCommandEvent &event)
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::OnReferenceBodySelection() entered\n");
   #endif
//    std::string newBody   = referenceBodyComboBox->GetValue().c_str();
    attRefBodyModified    = true;
    dataChanged           = true;
    theScPanel->EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnModeOfConstraintSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles case when user updates the mode of constraint.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void AttitudePanel::OnModeOfConstraintSelection(wxCommandEvent &event)
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::OnModeOfConstraintSelection() entered\n");
   #endif
//   std::string newMode      = modeOfConstraintComboBox->GetValue().c_str();
   modeOfConstraintModified = true;
   dataChanged              = true;
   theScPanel->EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnEulerSequenceSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles case when user updates euler sequence selection from the combo box.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void AttitudePanel::OnEulerSequenceSelection(wxCommandEvent &event)
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::OnEulerSequenceSelection() entered\n");
   #endif
   std::string newSeq = config4ComboBox->GetValue().c_str();
   if (newSeq != eulerSequence)
   {
      seqModified   = true;
      dataChanged   = true;
      eulerSequence = newSeq;
      theScPanel->EnableUpdate(true);
      seq = Attitude::ExtractEulerSequence(eulerSequence);
   }
}


//------------------------------------------------------------------------------
// void OnStateTypeSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles case when user selects the state type.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void AttitudePanel::OnStateTypeSelection(wxCommandEvent &event)
{
   bool OK = true;
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::OnStateTypeSelection() entered\n");
   #endif
   std::string newStateType = stateTypeComboBox->GetStringSelection().c_str();
   if (newStateType == attStateType) return;
   if (!ValidateState("State", true))
   {
      stateTypeComboBox->SetValue(wxT(attStateType.c_str()));
      MessageInterface::PopupMessage(Gmat::ERROR_, +
         "Please enter valid value(s) before changing the Attitude State Type\n");
     return;
   }
   
   if (newStateType == stateTypeArray[EULER_ANGLES])
      OK = DisplayEulerAngles();
   else if (newStateType == stateTypeArray[QUATERNION])
      OK = DisplayQuaternion();
   else if (newStateType == stateTypeArray[DCM])
      OK = DisplayDCM();
   else if (newStateType == stateTypeArray[MRPS])   // Added by Dunn
      OK = DisplayMRPs();
      
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("   Now setting attitude state type to %s\n",
      newStateType.c_str());
   #endif
   if (OK)
   {
      attStateType      = newStateType;
      dataChanged       = true;
      stateTypeModified = true;
      theScPanel->EnableUpdate(true);
   }
   else
   {
      stateTypeComboBox->SetValue(attStateType.c_str());
   }
}


//------------------------------------------------------------------------------
// void OnStateTypeRateSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles case when user selects the state rate type.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void AttitudePanel::OnStateTypeRateSelection(wxCommandEvent &event)
{
   bool OK = true;
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::OnStateTypeRateSelection() entered\n");
   #endif
   std::string newStateRateType = 
      stateRateTypeComboBox->GetStringSelection().c_str();
      if (newStateRateType == attRateStateType) return;
      
   if (!ValidateState("Both", true))
   {
      stateRateTypeComboBox->SetValue(wxT(attRateStateType.c_str()));
      MessageInterface::PopupMessage(Gmat::ERROR_, +
         "Please enter valid value(s) before changing the Attitude Rate State Type\n");
     return;
   }
  
   if (newStateRateType == stateRateTypeArray[EULER_ANGLE_RATES])
      OK = DisplayEulerAngleRates();
   else if (newStateRateType == stateRateTypeArray[ANGULAR_VELOCITY])
      OK = DisplayAngularVelocity();
      
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("   Now setting attitude rate state type to %s\n",
      newStateRateType.c_str());
   #endif
   if (OK)
   {
      attRateStateType      = newStateRateType;
      dataChanged           = true;
      rateStateTypeModified = true;
      theScPanel->EnableUpdate(true);
   }
   else
   {
      stateRateTypeComboBox->SetValue(attRateStateType.c_str());
   }
}

//------------------------------------------------------------------------------
// bool DisplayEulerAngles()
//------------------------------------------------------------------------------
/**
 * Displays the Euler angles.
 *
 */
//------------------------------------------------------------------------------
bool AttitudePanel::DisplayEulerAngles()
{
   bool retval = true;
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::DisplayEulerAngles() entered\n");
   #endif
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Spacecraft Attitude"));

   retval = UpdateEulerAngles();
   if (retval)
   {
      stateTypeComboBox->
         SetValue(wxT("Euler Angles"));
      attStateType = STATE_TEXT[EULER_ANGLES];

      st1StaticText->Show(true);
      st2StaticText->Show(true);
      st3StaticText->Show(true);
      st4StaticText->Show(false);

      ResizeTextCtrl1234();  // true); // add true for wider text box

      st1TextCtrl->Enable();
      st1TextCtrl->Show(true);
      st1TextCtrl->SetToolTip(pConfig->Read(_T("EulerAngle1Hint")));
      st2TextCtrl->Enable();
      st2TextCtrl->Show(true);
      st2TextCtrl->SetToolTip(pConfig->Read(_T("EulerAngle2Hint")));
      st3TextCtrl->Enable();
      st3TextCtrl->Show(true);
      st3TextCtrl->SetToolTip(pConfig->Read(_T("EulerAngle3Hint")));
      st4TextCtrl->Disable();
      st4TextCtrl->Show(false);

      st5TextCtrl->Show(false);
      st6TextCtrl->Show(false);
      st7TextCtrl->Show(false);

      st8TextCtrl->Show(false);
      st9TextCtrl->Show(false);
      st10TextCtrl->Show(false);

      attUnits1->Show(true);
      attUnits2->Show(true);
      attUnits3->Show(true);

      st1StaticText->SetLabel(wxT("Euler Angle "GUI_ACCEL_KEY"1"));
      st2StaticText->SetLabel(wxT("Euler Angle "GUI_ACCEL_KEY"2"));
      st3StaticText->SetLabel(wxT("Euler Angle "GUI_ACCEL_KEY"3"));
   
      st1TextCtrl->SetValue(*eulerAngles[0]);
      st2TextCtrl->SetValue(*eulerAngles[1]);
      st3TextCtrl->SetValue(*eulerAngles[2]);
   
      // Dunn Added - Probably not going to use this.  If you disable, you gray out
      // the text.  I'll need to remove the text for everything but EAs.
      //attUnits1->Enable();
      //attUnits2->Enable();
      //attUnits3->Enable();
      attUnits1->SetLabel(wxT("degrees"));
      attUnits2->SetLabel(wxT("degrees"));
      attUnits3->SetLabel(wxT("degrees"));
   
      attitudeSizer->Layout();
      Refresh();
      ResetStateFlags("State", true);
   }
   return retval;
}

//------------------------------------------------------------------------------
// bool DisplayQuaternion()
//------------------------------------------------------------------------------
/**
 * Displays the Quaternion.
 *
 */
//------------------------------------------------------------------------------
bool AttitudePanel::DisplayQuaternion()
{
   bool retval = true;
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::DisplayQuaternion() entered\n");
   #endif
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Spacecraft Attitude"));

   retval = UpdateQuaternion();
   if (retval)
   {
      stateTypeComboBox->
         SetValue(wxT("Quaternion"));
      attStateType = "Quaternion";

      st1StaticText->Show(true);
      st2StaticText->Show(true);
      st3StaticText->Show(true);
      st4StaticText->Show(true);

      ResizeTextCtrl1234(true);

      st1TextCtrl->Show(true);
      st1TextCtrl->Enable(true);
      st1TextCtrl->SetToolTip(pConfig->Read(_T("Quaternion1Hint")));
      st2TextCtrl->Show(true);
      st2TextCtrl->Enable(true);
      st2TextCtrl->SetToolTip(pConfig->Read(_T("Quaternion2Hint")));
      st3TextCtrl->Show(true);
      st3TextCtrl->Enable(true);
      st3TextCtrl->SetToolTip(pConfig->Read(_T("Quaternion3Hint")));
      st4TextCtrl->Show(true);
      st4TextCtrl->Enable(true);
      st4TextCtrl->SetToolTip(pConfig->Read(_T("Quaternion4Hint")));

      st5TextCtrl->Show(false);
      st6TextCtrl->Show(false);
      st7TextCtrl->Show(false);

      st8TextCtrl->Show(false);
      st9TextCtrl->Show(false);
      st10TextCtrl->Show(false);

      st1StaticText->SetLabel(wxT("q"GUI_ACCEL_KEY"1"));
      st2StaticText->SetLabel(wxT("q"GUI_ACCEL_KEY"2"));
      st3StaticText->SetLabel(wxT("q"GUI_ACCEL_KEY"3"));
      st4StaticText->SetLabel(wxT("q"GUI_ACCEL_KEY"4"));  // Dunn changed 4 to c
   
      st1TextCtrl->SetValue(*quaternion[0]);
      st2TextCtrl->SetValue(*quaternion[1]);
      st3TextCtrl->SetValue(*quaternion[2]);
      st4TextCtrl->SetValue(*quaternion[3]);
   
      // Dunn Added - Probably not going to use this.  If you disable, you gray out
      // the text.  I'll need to remove the text for everything but EAs.
      //attUnits1->Disable();
      //attUnits2->Disable();
      //attUnits3->Disable();
      attUnits1->Show(false);
      attUnits2->Show(false);
      attUnits3->Show(false);
   
      attitudeSizer->Layout();
      Refresh();
      ResetStateFlags("State", true);
   }
   return retval;
}

//------------------------------------------------------------------------------
// bool DisplayDCM()
//------------------------------------------------------------------------------
/**
 * Displays the direction cosine matrix.
 *
 * @return  success flag
 *
 */
//------------------------------------------------------------------------------
bool AttitudePanel::DisplayDCM()
{
   bool retval = true;
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::DisplayDCM() entered\n");
   #endif
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Spacecraft Attitude"));

   retval = UpdateCosineMatrix();
   if (retval)
   {
      stateTypeComboBox->
         SetValue(wxT("DirectionCosineMatrix"));
      attStateType = "DirectionCosineMatrix";

      st1StaticText->Show(false);
      st2StaticText->Show(false);
      st3StaticText->Show(false);
      st4StaticText->Show(false);

      ResizeTextCtrl1234();

      st1TextCtrl->Show(true);
      st1TextCtrl->Enable(true);
      st1TextCtrl->SetToolTip(pConfig->Read(_T("DCM1Hint")));
      st2TextCtrl->Show(true);
      st2TextCtrl->Enable(true);
      st2TextCtrl->SetToolTip(pConfig->Read(_T("DCM2Hint")));
      st3TextCtrl->Show(true);
      st3TextCtrl->Enable(true);
      st3TextCtrl->SetToolTip(pConfig->Read(_T("DCM3Hint")));
      st4TextCtrl->Show(false);

      st5TextCtrl->Show(true);
      st5TextCtrl->Enable(true);
      st5TextCtrl->SetToolTip(pConfig->Read(_T("DCM5Hint")));
      st6TextCtrl->Show(true);
      st6TextCtrl->Enable(true);
      st6TextCtrl->SetToolTip(pConfig->Read(_T("DCM6Hint")));
      st7TextCtrl->Show(true);
      st7TextCtrl->Enable(true);
      st7TextCtrl->SetToolTip(pConfig->Read(_T("DCM7Hint")));

      st8TextCtrl->Show(true);
      st8TextCtrl->Enable(true);
      st8TextCtrl->SetToolTip(pConfig->Read(_T("DCM8Hint")));
      st9TextCtrl->Show(true);
      st9TextCtrl->Enable(true);
      st9TextCtrl->SetToolTip(pConfig->Read(_T("DCM9Hint")));
      st10TextCtrl->Show(true);
      st10TextCtrl->Enable(true);
      st10TextCtrl->SetToolTip(pConfig->Read(_T("DCM10Hint")));

      st1StaticText->SetLabel(wxT(""));
      st2StaticText->SetLabel(wxT(""));
      st3StaticText->SetLabel(wxT(""));
      st4StaticText->SetLabel(wxT(""));
   
      st1TextCtrl->SetValue(*cosineMatrix[0]);
      st2TextCtrl->SetValue(*cosineMatrix[3]);
      st3TextCtrl->SetValue(*cosineMatrix[6]);
      st5TextCtrl->SetValue(*cosineMatrix[1]);
      st6TextCtrl->SetValue(*cosineMatrix[4]);
      st7TextCtrl->SetValue(*cosineMatrix[7]);
      st8TextCtrl->SetValue(*cosineMatrix[2]);
      st9TextCtrl->SetValue(*cosineMatrix[5]);
      st10TextCtrl->SetValue(*cosineMatrix[8]);
   
      // Dunn Added - Probably not going to use this.  If you disable, you gray out
      // the text.  I'll need to remove the text for everything but EAs.
      //attUnits1->Disable();
      //attUnits2->Disable();
      //attUnits3->Disable();
      attUnits1->Show(false);
      attUnits2->Show(false);
      attUnits3->Show(false);
   
      attitudeSizer->Layout();
      Refresh();
      ResetStateFlags("State", true);
   }
   return retval;
}

//------------------------------------------------------------------------------
// bool DisplayModifiedRodriguesParameters() - Added by Dunn
//------------------------------------------------------------------------------
/**
 * Displays the modified Rodrigues parameters.
 *
 * @return  success flag
 *
 */
//------------------------------------------------------------------------------
bool AttitudePanel::DisplayMRPs()
{
   bool retval = true;
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::DisplayMRPs() entered\n");
   #endif
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Spacecraft Attitude"));

   retval = UpdateMRPs();
   if (retval)
   {
      stateTypeComboBox->
         SetValue(wxT("MRPs"));
      attStateType = "MRPs";

      st1StaticText->Show(true);
      st2StaticText->Show(true);
      st3StaticText->Show(true);
      st4StaticText->Show(false);

      ResizeTextCtrl1234();   // true); // add true for wider text box

      st1TextCtrl->Show(true);
      st1TextCtrl->Enable(true);
      st1TextCtrl->SetToolTip(pConfig->Read(_T("MRP1Hint")));
      st2TextCtrl->Show(true);
      st2TextCtrl->Enable(true);
      st2TextCtrl->SetToolTip(pConfig->Read(_T("MRP2Hint")));
      st3TextCtrl->Show(true);
      st3TextCtrl->Enable(true);
      st3TextCtrl->SetToolTip(pConfig->Read(_T("MRP3Hint")));
      st4TextCtrl->Show(false);

      st5TextCtrl->Show(false);
      st6TextCtrl->Show(false);
      st7TextCtrl->Show(false);

      st8TextCtrl->Show(false);
      st9TextCtrl->Show(false);
      st10TextCtrl->Show(false);

      st1StaticText->SetLabel(wxT("MRP "GUI_ACCEL_KEY"1"));
      st2StaticText->SetLabel(wxT("MRP "GUI_ACCEL_KEY"2"));
      st3StaticText->SetLabel(wxT("MRP "GUI_ACCEL_KEY"3"));

      st1TextCtrl->SetValue(*MRPs[0]);
      st2TextCtrl->SetValue(*MRPs[1]);
      st3TextCtrl->SetValue(*MRPs[2]);

      // Dunn Added - Probably not going to use this.  If you disable, you gray out
      // the text.  I'll need to remove the text for everything but EAs.
      //attUnits1->Disable();
      //attUnits2->Disable();
      //attUnits3->Disable();
      attUnits1->Show(false);
      attUnits2->Show(false);
      attUnits3->Show(false);

      attitudeSizer->Layout();
      Refresh();
      ResetStateFlags("State", true);
   }
   return retval;
}

//------------------------------------------------------------------------------
// bool DisplayEulerAngleRates()
//------------------------------------------------------------------------------
/**
 * Displays the euler angle rates.
 *
 * @return  success flag
 *
 */
//------------------------------------------------------------------------------
bool AttitudePanel::DisplayEulerAngleRates()
{
   bool retval = true;
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::DisplayEulerAngleRates() entered\n");
   #endif
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Spacecraft Attitude"));

   retval  = UpdateEulerAngleRates();
   if (retval)
   {
      stateRateTypeComboBox->
         SetValue(wxT("EulerAngleRates"));
      attRateStateType = "EulerAngleRates";
      str1StaticText->SetLabel(wxT("Euler Angle Rate "GUI_ACCEL_KEY"1"));
      str2StaticText->SetLabel(wxT("Euler Angle Rate "GUI_ACCEL_KEY"2"));
      str3StaticText->SetLabel(wxT("Euler Angle Rate "GUI_ACCEL_KEY"3"));

      str1TextCtrl->SetToolTip(pConfig->Read(_T("EulerAngleRate1Hint")));
      str2TextCtrl->SetToolTip(pConfig->Read(_T("EulerAngleRate2Hint")));
      str3TextCtrl->SetToolTip(pConfig->Read(_T("EulerAngleRate3Hint")));

      str1TextCtrl->SetValue(*eulerAngleRates[0]);
      str2TextCtrl->SetValue(*eulerAngleRates[1]);
      str3TextCtrl->SetValue(*eulerAngleRates[2]);

      attRateSizer->Layout();
      Refresh();
      ResetStateFlags("Rate", true);
   }
   return retval;
}

//------------------------------------------------------------------------------
// bool DisplayAngularVelocity()
//------------------------------------------------------------------------------
/**
 * Displays the angular velocity.
 *
 * @return  success flag
 *
 */
//------------------------------------------------------------------------------
bool AttitudePanel::DisplayAngularVelocity()
{
   bool retval = true;
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::DisplayAngularVelocity() entered\n");
   #endif
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Spacecraft Attitude"));

   retval = UpdateAngularVelocity();
   if (retval)
   {
      stateRateTypeComboBox->
         SetValue(wxT("AngularVelocity"));
      attRateStateType = "AngularVelocity";
      str1StaticText->SetLabel(wxT("Angular Velocity "GUI_ACCEL_KEY"X"));
      str2StaticText->SetLabel(wxT("Angular Velocity "GUI_ACCEL_KEY"Y"));
      str3StaticText->SetLabel(wxT("Angular Velocity "GUI_ACCEL_KEY"Z"));

      str1TextCtrl->SetToolTip(pConfig->Read(_T("AngularVelocity1Hint")));
      str2TextCtrl->SetToolTip(pConfig->Read(_T("AngularVelocity2Hint")));
      str3TextCtrl->SetToolTip(pConfig->Read(_T("AngularVelocity3Hint")));

      str1TextCtrl->SetValue(*angVel[0]);
      str2TextCtrl->SetValue(*angVel[1]);
      str3TextCtrl->SetValue(*angVel[2]);

      attRateSizer->Layout();
      Refresh();
      ResetStateFlags("Rate", true);
   }
   return retval;
}

//LOJ: Addded
//------------------------------------------------------------------------------
// void LoadPrecessingSpinnerData()
//------------------------------------------------------------------------------
void AttitudePanel::LoadPrecessingSpinnerData()
{
   #ifdef DEBUG_PRECESSING_SPINNER
      MessageInterface::ShowMessage("AttitudePanel::LoadPrecessingSpinnerData() entered\n");
   #endif
   try
   {
      //LOJ: @todo
      // Use theAttitude->GetRealParameter() to get value and set to spin axis text ctrl
      // For example
      //spinAxis1TextCtrl->SetValue(ToString(theAttitude->GetRealParameter("SpinAxis1"));
      
      // Just for testing
      spinAxis1TextCtrl->SetValue(ToString(theAttitude->GetRealParameter("BodySpinAxisX")));
      spinAxis2TextCtrl->SetValue(ToString(theAttitude->GetRealParameter("BodySpinAxisY")));
      spinAxis3TextCtrl->SetValue(ToString(theAttitude->GetRealParameter("BodySpinAxisZ")));

      nutRefVec1TextCtrl->SetValue(ToString(theAttitude->GetRealParameter("NutationReferenceVectorX")));
      nutRefVec2TextCtrl->SetValue(ToString(theAttitude->GetRealParameter("NutationReferenceVectorY")));
      nutRefVec3TextCtrl->SetValue(ToString(theAttitude->GetRealParameter("NutationReferenceVectorZ")));

      initPrecAngleTextCtrl->SetValue(ToString(theAttitude->GetRealParameter("InitialPrecessionAngle")));
      precessionRateTextCtrl->SetValue(ToString(theAttitude->GetRealParameter("PrecessionRate")));
      nutationAngleTextCtrl->SetValue(ToString(theAttitude->GetRealParameter("NutationAngle")));
      initSpinAngleTextCtrl->SetValue(ToString(theAttitude->GetRealParameter("InitialSpinAngle")));
      spinRateTextCtrl->SetValue(ToString(theAttitude->GetRealParameter("SpinRate")));

      precessingSpinnerDataLoaded = true;
   }
   catch (BaseException &be)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, be.GetFullMessage());
   }
}

//------------------------------------------------------------------------------
// void SavePrecessingSpinnerData(Attitude *useAttitude)
//------------------------------------------------------------------------------
void AttitudePanel::SavePrecessingSpinnerData(Attitude *useAttitude)
{
   #ifdef DEBUG_PRECESSING_SPINNER
      MessageInterface::ShowMessage("AttitudePanel::SavePrecessingSpinnerData() entered\n");
   #endif
   canClose = true;
   
   //LOJ: @todo
   // Use theAttitude->SetRealParameter() to set value to attitude object
   // In PrecessingSpinner::SetRealParameter(), do the range checking
   //    if range checking failed, throw an exception so that it can be
   //    caught here
   try
   {
      bool        success = true;
      std::string strVal, strValX, strValY, strValZ;
      Real        theReal, theX, theY, theZ;
      // Set new spin axis value
      if (spinAxisModified)
      {
         strValX = (spinAxis1TextCtrl->GetValue()).c_str();
         strValY = (spinAxis2TextCtrl->GetValue()).c_str();
         strValZ = (spinAxis3TextCtrl->GetValue()).c_str();
         // Check if strings are real numbers and convert wxString to Real
         if ((theScPanel->CheckReal(theX, strValX, "BodySpinAxisX", "Real Number")) &&
             (theScPanel->CheckReal(theY, strValY, "BodySpinAxisY", "Real Number")) &&
             (theScPanel->CheckReal(theZ, strValZ, "BodySpinAxisZ", "Real Number")))
         {
            useAttitude->SetRealParameter("BodySpinAxisX", theX);
            useAttitude->SetRealParameter("BodySpinAxisY", theY);
            useAttitude->SetRealParameter("BodySpinAxisZ", theZ);
            spinAxisModified       = false;
         }
         else
            success = false;
      }

      if (nutRefVecModified)
      {
         strValX = (nutRefVec1TextCtrl->GetValue()).c_str();
         strValY = (nutRefVec2TextCtrl->GetValue()).c_str();
         strValZ = (nutRefVec3TextCtrl->GetValue()).c_str();
         // Check if strings are real numbers and convert wxString to Real
         if ((theScPanel->CheckReal(theX, strValX, "NutationReferenceVectorX", "Real Number")) &&
             (theScPanel->CheckReal(theY, strValY, "NutationReferenceVectorY", "Real Number")) &&
             (theScPanel->CheckReal(theZ, strValZ, "NutationReferenceVectorZ", "Real Number")))
         {
            useAttitude->SetRealParameter("NutationReferenceVectorX", theX);
            useAttitude->SetRealParameter("NutationReferenceVectorY", theY);
            useAttitude->SetRealParameter("NutationReferenceVectorZ", theZ);
            nutRefVecModified      = false;
         }
         else
            success = false;
      }

      if (initPrecAngleModified)
      {
         strVal = (initPrecAngleTextCtrl->GetValue()).c_str();
         if (theScPanel->CheckReal(theReal, strVal, "InitialPrecessionAngle", "Real Number"))
         {
            useAttitude->SetRealParameter("InitialPrecessionAngle", theReal);
            initPrecAngleModified  = false;
         }
         else
            success = false;
      }

      if (precessionRateModified)
      {
         strVal = (precessionRateTextCtrl->GetValue()).c_str();
         if (theScPanel->CheckReal(theReal, strVal, "PrecessionRate", "Real Number"))
         {
            useAttitude->SetRealParameter("PrecessionRate", theReal);
            precessionRateModified = false;
         }
         else
            success = false;
      }

      if (nutationAngleModified)
      {
         strVal = (nutationAngleTextCtrl->GetValue()).c_str();
         if (theScPanel->CheckReal(theReal, strVal, "NutationAngle", "Real Number"))
         {
            useAttitude->SetRealParameter("NutationAngle", theReal);
            nutationAngleModified  = false;
         }
         else
            success = false;
      }

      if (initSpinAngleModified)
      {
         strVal = (initSpinAngleTextCtrl->GetValue()).c_str();
         if (theScPanel->CheckReal(theReal, strVal, "InitialSpinAngle", "Real Number"))
         {
            useAttitude->SetRealParameter("InitialSpinAngle", theReal);
            initSpinAngleModified  = false;
         }
         else
            success = false;
      }

      if (spinRateModified)
      {
         strVal = (spinRateTextCtrl->GetValue()).c_str();
         if (theScPanel->CheckReal(theReal, strVal, "SpinRate", "Real Number"))
         {
            useAttitude->SetRealParameter("SpinRate", theReal);
            spinRateModified       = false;
         }
         else
            success = false;
      }

      if (success)
      {
         // do I need to do something here?
      }
   }
   catch (BaseException &ex)
   {
      canClose    = false;
      dataChanged = true;
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
   }
}

//------------------------------------------------------------------------------
// void LoadNadirPointingData()
//------------------------------------------------------------------------------
void AttitudePanel::LoadNadirPointingData()
{
   #ifdef DEBUG_NADIR_POINTING
      MessageInterface::ShowMessage("AttitudePanel::LoadNadirPointingData() entered\n");
   #endif
   try
   {
      refVectorXTextCtrl->SetValue(ToString(theAttitude->GetRealParameter("ReferenceVectorX")));
      refVectorYTextCtrl->SetValue(ToString(theAttitude->GetRealParameter("ReferenceVectorY")));
      refVectorZTextCtrl->SetValue(ToString(theAttitude->GetRealParameter("ReferenceVectorZ")));
      constraintVectorXTextCtrl->SetValue(ToString(theAttitude->GetRealParameter("ConstraintVectorX")));
      constraintVectorYTextCtrl->SetValue(ToString(theAttitude->GetRealParameter("ConstraintVectorY")));
      constraintVectorZTextCtrl->SetValue(ToString(theAttitude->GetRealParameter("ConstraintVectorZ")));
      bodyAlignVectorXTextCtrl->SetValue(ToString(theAttitude->GetRealParameter("BodyAlignmentVectorX")));
      bodyAlignVectorYTextCtrl->SetValue(ToString(theAttitude->GetRealParameter("BodyAlignmentVectorY")));
      bodyAlignVectorZTextCtrl->SetValue(ToString(theAttitude->GetRealParameter("BodyAlignmentVectorZ")));
      bodyConstraintVectorXTextCtrl->SetValue(ToString(theAttitude->GetRealParameter("BodyConstraintVectorX")));
      bodyConstraintVectorYTextCtrl->SetValue(ToString(theAttitude->GetRealParameter("BodyConstraintVectorY")));
      bodyConstraintVectorZTextCtrl->SetValue(ToString(theAttitude->GetRealParameter("BodyConstraintVectorZ")));

      std::string referenceBody  = theAttitude->GetStringParameter("AttitudeReferenceBody");
      referenceBodyComboBox->SetValue(wxT(referenceBody.c_str()));
      std::string theMode  = theAttitude->GetStringParameter("ModeOfConstraint");
      modeOfConstraintComboBox->SetValue(wxT(theMode.c_str()));

      nadirPointingDataLoaded = true;
   }
   catch (BaseException &be)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, be.GetFullMessage());
   }
}

//------------------------------------------------------------------------------
// void SaveNadirPointingData(Attitude *useAttitude)
//------------------------------------------------------------------------------
void AttitudePanel::SaveNadirPointingData(Attitude *useAttitude)
{
   #ifdef DEBUG_NADIR_POINTING
      MessageInterface::ShowMessage("AttitudePanel::SaveNadirPointingData() entered\n");
   #endif
   canClose = true;

   try
   {
      bool        success = true;
      std::string strValX, strValY, strValZ;
      Real        theX, theY, theZ;
      if (refVectorModified)
      {
         strValX = (refVectorXTextCtrl->GetValue()).c_str();
         strValY = (refVectorYTextCtrl->GetValue()).c_str();
         strValZ = (refVectorZTextCtrl->GetValue()).c_str();
         // Check if strings are real numbers and convert wxString to Real
         if ((theScPanel->CheckReal(theX, strValX, "ReferenceVectorX", "Real Number")) &&
             (theScPanel->CheckReal(theY, strValY, "ReferenceVectorY", "Real Number")) &&
             (theScPanel->CheckReal(theZ, strValZ, "ReferenceVectorZ", "Real Number")))
         {
            useAttitude->SetRealParameter("ReferenceVectorX", theX);
            useAttitude->SetRealParameter("ReferenceVectorY", theY);
            useAttitude->SetRealParameter("ReferenceVectorZ", theZ);
            refVectorModified      = false;
         }
         else
            success = false;
      }
      if (constraintVectorModified)
      {
         strValX = (constraintVectorXTextCtrl->GetValue()).c_str();
         strValY = (constraintVectorYTextCtrl->GetValue()).c_str();
         strValZ = (constraintVectorZTextCtrl->GetValue()).c_str();
         // Check if strings are real numbers and convert wxString to Real
         if ((theScPanel->CheckReal(theX, strValX, "ConstraintVectorX", "Real Number")) &&
             (theScPanel->CheckReal(theY, strValY, "ConstraintVectorY", "Real Number")) &&
             (theScPanel->CheckReal(theZ, strValZ, "ConstraintVectorZ", "Real Number")))
         {
            useAttitude->SetRealParameter("ConstraintVectorX", theX);
            useAttitude->SetRealParameter("ConstraintVectorY", theY);
            useAttitude->SetRealParameter("ConstraintVectorZ", theZ);
            constraintVectorModified      = false;
         }
         else
            success = false;
      }
      if (bodyAlignVectorModified)
      {
         strValX = (bodyAlignVectorXTextCtrl->GetValue()).c_str();
         strValY = (bodyAlignVectorYTextCtrl->GetValue()).c_str();
         strValZ = (bodyAlignVectorZTextCtrl->GetValue()).c_str();
         // Check if strings are real numbers and convert wxString to Real
         if ((theScPanel->CheckReal(theX, strValX, "BodyAlignmentVectorX", "Real Number")) &&
             (theScPanel->CheckReal(theY, strValY, "BodyAlignmentVectorY", "Real Number")) &&
             (theScPanel->CheckReal(theZ, strValZ, "BodyAlignmentVectorZ", "Real Number")))
         {
            useAttitude->SetRealParameter("BodyAlignmentVectorX", theX);
            useAttitude->SetRealParameter("BodyAlignmentVectorY", theY);
            useAttitude->SetRealParameter("BodyAlignmentVectorZ", theZ);
            bodyAlignVectorModified      = false;
         }
         else
            success = false;
      }
      if (bodyConstraintVectorModified)
      {
         strValX = (bodyConstraintVectorXTextCtrl->GetValue()).c_str();
         strValY = (bodyConstraintVectorYTextCtrl->GetValue()).c_str();
         strValZ = (bodyConstraintVectorZTextCtrl->GetValue()).c_str();
         // Check if strings are real numbers and convert wxString to Real
         if ((theScPanel->CheckReal(theX, strValX, "BodyConstraintVectorX", "Real Number")) &&
             (theScPanel->CheckReal(theY, strValY, "BodyConstraintVectorY", "Real Number")) &&
             (theScPanel->CheckReal(theZ, strValZ, "BodyConstraintVectorZ", "Real Number")))
         {
            useAttitude->SetRealParameter("BodyConstraintVectorX", theX);
            useAttitude->SetRealParameter("BodyConstraintVectorY", theY);
            useAttitude->SetRealParameter("BodyConstraintVectorZ", theZ);
            bodyConstraintVectorModified      = false;
         }
         else
            success = false;
      }

      if (attRefBodyModified)
      {
         std::string newBody = referenceBodyComboBox->GetValue().c_str();
         useAttitude->SetStringParameter("AttitudeReferenceBody", newBody);
         attRefBodyModified = false;
      }

      if (modeOfConstraintModified)
      {
         std::string newMode = modeOfConstraintComboBox->GetValue().c_str();
         useAttitude->SetStringParameter("ModeOfConstraint", newMode);
         modeOfConstraintModified = false;
      }

      if (success)
      {
         // do I need to do something here?
      }
   }
   catch (BaseException &ex)
   {
      canClose    = false;
      dataChanged = true;
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
   }

}

//------------------------------------------------------------------------------
// bool UpdateCosineMatrix()
//------------------------------------------------------------------------------
/**
 * Updates the direction cosine matrix.
 *
 * @return  success flag
 *
 */
//------------------------------------------------------------------------------
bool AttitudePanel::UpdateCosineMatrix()
{
   bool retval = true;
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::UpdateCosineMatrix() entered\n");
   #endif
   if (attStateType == stateTypeArray[DCM]) return true;
   try
   {
      if (attStateType == stateTypeArray[QUATERNION])
      {
         dcmat = Attitude::ToCosineMatrix(q);
      }
      else if (attStateType == stateTypeArray[EULER_ANGLES])
      {
         dcmat = Attitude::ToCosineMatrix(ea * GmatMathConstants::RAD_PER_DEG,
                         (Integer) seq[0], (Integer) seq[1], (Integer) seq[2]);
      }
      else if (attStateType == stateTypeArray[MRPS])  // Dunn Added
      {
         q     = Attitude::ToQuaternion(mrp);
         dcmat = Attitude::ToCosineMatrix(q);
      }
      // update string versions of mat values (cosineMatrix)
      unsigned int x, y;
      for (x = 0; x < 3; ++x)
         for (y = 0; y < 3; ++y)
            *cosineMatrix[x*3+y] = theGuiManager->ToWxString(dcmat(x,y));
   }
   catch (BaseException &ex)
   {
      retval = false;
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
   }
   return retval;
}

//------------------------------------------------------------------------------
// bool UpdateQuaternion()
//------------------------------------------------------------------------------
/**
 * Updates the quaternion.
 *
 * @return  success flag
 *
 */
//------------------------------------------------------------------------------
bool AttitudePanel::UpdateQuaternion()
{
   bool retval = true;
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::UpdateQuaternion() entered\n");
   #endif
   if (attStateType == stateTypeArray[QUATERNION]) return true;
   try
   {
      if (attStateType == stateTypeArray[DCM])
      {
         q = Attitude::ToQuaternion(dcmat);
      }
      else if (attStateType == stateTypeArray[EULER_ANGLES])
      {
         q = Attitude::ToQuaternion(ea * GmatMathConstants::RAD_PER_DEG,
                       (Integer) seq[0], (Integer) seq[1], (Integer) seq[2]);
      }
      else if (attStateType == stateTypeArray[MRPS])  // Dunn Added
      {
         q     = Attitude::ToQuaternion(mrp);
      }
      // update string versions of q values
      for (unsigned int x = 0; x < 4; ++x)
         *quaternion[x] = theGuiManager->ToWxString(q[x]);
   }
   catch (BaseException &ex)
   {
      retval = false;
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
   }
   return retval;
}

//------------------------------------------------------------------------------
// bool UpdateEulerAngles()
//------------------------------------------------------------------------------
/**
 * Updates the Euler angles.
 *
 * @return  success flag
 *
 */
//------------------------------------------------------------------------------
bool AttitudePanel::UpdateEulerAngles()
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::UpdateEulerAngles() entered\n");
   #endif
   bool retval = true;
   if (attStateType == stateTypeArray[EULER_ANGLES]) return true;
   try
   {
      if (attStateType == stateTypeArray[DCM])
      {
         ea = Attitude::ToEulerAngles(dcmat,
                        (Integer) seq[0], (Integer) seq[1], (Integer) seq[2])
                        * GmatMathConstants::DEG_PER_RAD;
      }
      else if (attStateType == stateTypeArray[QUATERNION])
      {
         ea = Attitude::ToEulerAngles(q,
                       (Integer) seq[0], (Integer) seq[1], (Integer) seq[2])
                       * GmatMathConstants::DEG_PER_RAD;
      }
      else if (attStateType == stateTypeArray[MRPS])  // Dunn Added
      {
         q  = Attitude::ToQuaternion(mrp);
         ea = Attitude::ToEulerAngles(q,
                       (Integer) seq[0], (Integer) seq[1], (Integer) seq[2])
                       * GmatMathConstants::DEG_PER_RAD;
      }
      // update string versions of ea values
      for (unsigned int x = 0; x < 3; ++x)
         *eulerAngles[x] = theGuiManager->ToWxString(ea[x]);
   }
   catch (BaseException &ex)
   {
      retval = false;
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
   }
   return retval;
}

//------------------------------------------------------------------------------
// bool UpdateMRPs() - Added by Dunn
//------------------------------------------------------------------------------
/**
 * Updates the MRPs.
 *
 * @return  success flag
 *
 */
//------------------------------------------------------------------------------
bool AttitudePanel::UpdateMRPs()
{
#ifdef DEBUG_ATTITUDE_PANEL
   MessageInterface::ShowMessage("AttitudePanel::UpdateMRPs() entered\n");
#endif
   bool retval = true;
   if (attStateType == stateTypeArray[MRPS]) return true;
   try
   {
      if (attStateType == stateTypeArray[DCM])
      {
         q   = Attitude::ToQuaternion(dcmat);
         mrp = Attitude::ToMRPs(q);
      }
      else if (attStateType == stateTypeArray[QUATERNION])
      {
         mrp = Attitude::ToMRPs(q);
      }
      else if (attStateType == stateTypeArray[EULER_ANGLES])
      {
         q   = Attitude::ToQuaternion(ea * GmatMathConstants::RAD_PER_DEG,
               (Integer) seq[0], (Integer) seq[1], (Integer) seq[2]);
         mrp = Attitude::ToMRPs(q);
      }
      // update string versions of mrp values
      for (unsigned int x = 0; x < 3; ++x)
         *MRPs[x] = theGuiManager->ToWxString(mrp[x]);
   }
   catch (BaseException &ex)
   {
      retval = false;
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
   }
   return retval;
}

//------------------------------------------------------------------------------
// bool UpdateAngularVelocity()
//------------------------------------------------------------------------------
/**
 * Updates the angular velocity.
 *
 * @return  success flag
 *
 */
//------------------------------------------------------------------------------
bool AttitudePanel::UpdateAngularVelocity()
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::UpdateAngularVelocity() entered\n");
   #endif
   bool retval = true;
   if (attRateStateType == stateRateTypeArray[ANGULAR_VELOCITY]) return true;
   if (attRateStateType == stateRateTypeArray[EULER_ANGLE_RATES])
   {
      try
      {
         retval = UpdateEulerAngles();
         #ifdef DEBUG_ATTITUDE_PANEL
            MessageInterface::ShowMessage("AttitudePanel::UpdateAngularVelocity() retval from UpdateEulerAngles = %s\n",
                  (retval? "true" : "false"));
         #endif
         if (retval)
         {
            #ifdef DEBUG_ATTITUDE_ANG_VEL
               MessageInterface::ShowMessage(
                     "AttitudePanel::UpdateAngularVelocity() converting ear (%le, %le, %le) to Ang Vel\n",
                     ear[0], ear[1], ear[2]);
               MessageInterface::ShowMessage("   with seq = %d  %d  %d\n",
                     (Integer) seq[0], (Integer) seq[1], (Integer) seq[2]);
            #endif
            av = Attitude::ToAngularVelocity(ear * GmatMathConstants::RAD_PER_DEG,
                           ea * GmatMathConstants::RAD_PER_DEG,
                           (Integer) seq[0], (Integer) seq[1], (Integer) seq[2])
                           * GmatMathConstants::DEG_PER_RAD;
            #ifdef DEBUG_ATTITUDE_ANG_VEL
               MessageInterface::ShowMessage(
                     "AttitudePanel::UpdateAngularVelocity() resulting av = (%le, %le, %le)\n",
                     av[0], av[1], av[2]);
            #endif
            // update string versions of av values
            for (unsigned int x = 0; x < 3; ++x)
               *angVel[x] = theGuiManager->ToWxString(av[x]);
         }
      }
      catch (BaseException &ex)
      {
         retval = false;
         MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      }
   }
   return retval;
}

//------------------------------------------------------------------------------
// bool UpdateEulerAngleRates()
//------------------------------------------------------------------------------
/**
 * Updates the Euler angle rates.
 *
 * @return  success flag
 *
 */
//------------------------------------------------------------------------------
bool AttitudePanel::UpdateEulerAngleRates()
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::UpdateEulerAngleRates() entered\n");
   #endif
   bool retval = true;
   if (attRateStateType == stateRateTypeArray[EULER_ANGLE_RATES]) return true;
   if (attRateStateType == stateRateTypeArray[ANGULAR_VELOCITY])
   {
      try
      {
         retval = UpdateEulerAngles();
         #ifdef DEBUG_ATTITUDE_PANEL
            MessageInterface::ShowMessage("AttitudePanel::UpdateAngularVelocity() retval = %s\n",
                  (retval? "true" : "false"));
         #endif
         if (retval)
         {
            ear = Attitude::ToEulerAngleRates(av * GmatMathConstants::RAD_PER_DEG,
                            ea * GmatMathConstants::RAD_PER_DEG,
                            (Integer) seq[0], (Integer) seq[1], (Integer) seq[2])
                            * GmatMathConstants::DEG_PER_RAD;
            // update string versions of av values
               for (unsigned int x = 0; x < 3; ++x)
                  *eulerAngleRates[x] = theGuiManager->ToWxString(ear[x]);
         }
      }
      catch (BaseException &ex)
      {
         retval = false;
         MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      }
   }
   return retval;
}
