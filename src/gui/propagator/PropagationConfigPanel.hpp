//$Header$
//------------------------------------------------------------------------------
//                           PropagationConfigPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/08/29
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
//
/**
 * This class contains the Propagation Configuration window.
 */
//------------------------------------------------------------------------------

#ifndef PropagationConfigPanel_hpp
#define PropagationConfigPanel_hpp

// gui includes
#include "gmatwxdefs.hpp"

// base includes
#include "gmatdefs.hpp"
#include "Propagator.hpp"
#include "PropSetup.hpp"
#include "ForceModel.hpp"
#include "DragForce.hpp"
#include "PointMassForce.hpp"
#include "SolarRadiationPressure.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "MessageInterface.hpp"
#include "GmatPanel.hpp"

class PropagationConfigPanel : public GmatPanel
{
public:
   // constructor
   PropagationConfigPanel(wxWindow *parent, const wxString &propName);
   ~PropagationConfigPanel();  

private:

   enum
   {
      RKV89_ID = 0,
      RKN68_ID,
      RKF56_ID,
      IntegratorCount,
   };
    
   wxStaticText *integratorStaticText;
   wxStaticText *setting1StaticText;
   wxStaticText *setting2StaticText;
   wxStaticText *setting3StaticText;
   wxStaticText *setting4StaticText;
   wxStaticText *setting5StaticText;
   wxStaticText *degree1StaticText;
   wxStaticText *order1StaticText;
   wxStaticText *type1StaticText;
   wxStaticText *type2StaticText;
   wxStaticText *degree2StaticText;
   wxStaticText *order2StaticText;
   wxStaticText *type3StaticText;
                
   wxTextCtrl *setting1TextCtrl;
   wxTextCtrl *setting2TextCtrl;
   wxTextCtrl *setting3TextCtrl;
   wxTextCtrl *setting4TextCtrl;
   wxTextCtrl *setting5TextCtrl;
   wxTextCtrl *bodyTextCtrl;
   wxTextCtrl *gravityDegreeTextCtrl;
   wxTextCtrl *gravityOrderTextCtrl;
   wxTextCtrl *magneticDegreeTextCtrl;
   wxTextCtrl *magneticOrderTextCtrl;
   wxTextCtrl *pmEditTextCtrl;

   wxComboBox *integratorComboBox;
   wxComboBox *bodyComboBox;
   wxComboBox *atmosComboBox;
   wxComboBox *gravityTypeComboBox;
   wxComboBox *magneticTypeComboBox;

   wxCheckBox *srpCheckBox;

   wxButton *bodyButton;
   wxButton *searchGravityButton;
   wxButton *setupButton;
   wxButton *searchMagneticButton;
   wxButton *editMassButton;
   wxButton *editPressureButton;
    
   wxString integratorString;
   wxString primaryBodyString;
   wxString gravityFieldString;
   wxString atmosModelString;

   std::string propSetupName;
   std::string newPropName;
    
   wxArrayString primaryBodiesArray;
   wxArrayString savedBodiesArray;
   wxArrayString pointmassBodiesArray;    
   wxArrayString primaryBodiesGravityArray;
   wxArrayString integratorArray;
    
   Integer numOfBodies;
   Integer numOfAtmosTypes;
   Integer numOfForces;
   Integer numOfMagFields;
   Integer numOfGraFields;
    
   bool useSRP;
   bool useDragForce;
   bool isForceModelChanged;
   bool isBodiesChanged;
   bool isIntegratorChanged;

   Propagator                     *thePropagator;
   Propagator                     *newProp;
   PropSetup                      *thePropSetup;
   ForceModel                     *theForceModel;
   SolarRadiationPressure         *theSRP;
   DragForce                      *theDragForce;
   SolarSystem                    *theSolarSystem;
   std::vector<PointMassForce *>  thePMForces;
   std::vector<CelestialBody *>   theBodies;

   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   //loj: 2/27/04 commented out
   //virtual void OnHelp();
   //virtual void OnScript();
      
   // Layout & data handling methods
   void Initialize();
   void Setup(wxWindow *parent);
   void DisplayIntegratorData(bool integratorChanged);
   void DisplayPrimaryBodyData();
   void DisplayForceData();
   void DisplayGravityFieldData();
   void DisplayAtmosphereModelData();
   void DisplayMagneticFieldData();
   void DisplaySRPData();
    
   // Text control event method
   void OnIntegratorTextUpdate();
   void OnGravityTextUpdate(wxCommandEvent& event);
   void OnMagneticTextUpdate(wxCommandEvent& event);
    
   // Checkbox event method
   void OnSRPCheckBoxChange();
    
   // Combobox event method
   void OnIntegratorSelection();
   void OnBodySelection();
   void OnGravitySelection();
   void OnAtmosphereSelection();
    
   // Button event methods
   void OnAddButton();
   void OnGravSearchButton();
   void OnSetupButton();
   void OnMagSearchButton();
   void OnPMEditButton();
   void OnSRPEditButton();

   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
    
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 42000,
      ID_TEXTCTRL,
      ID_TEXTCTRL_PROP,
      ID_TEXTCTRL_GRAV,
      ID_TEXTCTRL_MAGN,
      ID_CHECKBOX,
      ID_CB_INTGR,
      ID_CB_BODY,
      ID_CB_GRAV,
      ID_CB_ATMOS,
      ID_CB_MAG,
      ID_BUTTON_SCRIPT,
      ID_BUTTON_OK,
      ID_BUTTON_APPLY,
      ID_BUTTON_CANCEL,
      ID_BUTTON_HELP,
      ID_BUTTON_ADD_BODY,
      ID_BUTTON_GRAV_SEARCH,
      ID_BUTTON_SETUP,
      ID_BUTTON_MAG_SEARCH,
      ID_BUTTON_PM_EDIT,
      ID_BUTTON_SRP_EDIT 
   };
};

#endif // PropagationConfigPanel_hpp
