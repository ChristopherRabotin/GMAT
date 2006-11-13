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
 * This class contains the Propagation configuration window.
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
#include "GravityField.hpp"
#include "HarmonicField.hpp"
#include "PointMassForce.hpp"
#include "SolarRadiationPressure.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "AtmosphereModel.hpp"
#include "MessageInterface.hpp"
#include "GmatPanel.hpp"

class PropagationConfigPanel : public GmatPanel
{
public:

   PropagationConfigPanel(wxWindow *parent, const wxString &propName);
   ~PropagationConfigPanel();  

private:
   
   // Integrator types
   enum IntegratorType
   {
      RKV89 = 0,
      RKN68,
      RKF56,
      PD45,
      PD78,
      BS,
      ABM,
      CW,
      IntegratorCount,
   };

   // Earth gravity field model
   enum EarthGravModelType
   {
      E_NONE_GM = 0,
      JGM2,
      JGM3,
      EGM96,
      E_OTHER,
      EarthGravModelCount,
   };   
   
   // Luna gravity field model
   enum LunaGravModelType
   {
      L_NONE_GM = 0,
      LP165,
      L_OTHER,
      LunaGravModelCount,
   };   
   
   // Venus gravity field model
   enum VenusGravModelType
   {
      V_NONE_GM = 0,
      MGNP180U,
      V_OTHER,
      VenusGravModelCount,
   };   
   
   // Mars gravity field model
   enum MarsGravModelType
   {
      M_NONE_GM = 0,
      MARS50C,
      M_OTHER,
      MarsGravModelCount,
   };   
   
   // Other gravity field model
   enum OthersGravModelType
   {
      O_NONE_GM = 0,
      O_OTHER,
      OthersGravModelCount,
   };
   
   // Earth drag model
   enum EarthDragModelType
   {
      NONE_DM = 0,
      EXPONENTIAL,
      MSISE90,
      JR,
      EarthDragModelCount,
   };
   
   // Magnetic force drag model
   enum MagfModelType
   {
      NONE_MM = 0,
      MagfModelCount,
   };

   // Error Control
   enum ErrorControlType
   {
      NONE_EC = 0,
      RSSSTEP,
      RSSSTATE,
      LARGESTSTEP,
      LARGESTSTATE,
      ErrorControlCount,
   };
   
   struct ForceType
   {
      std::string bodyName;
      std::string gravType;
      std::string dragType;
      std::string magfType;
      wxString gravDegree;
      wxString gravOrder;
      wxString magfDegree;
      wxString magfOrder;
      std::string potFilename;
      PointMassForce *pmf;
      GravityField *gravf;
      DragForce *dragf;
      SolarRadiationPressure *srpf;
      bool useSrp;
      
      ForceType(const std::string &body, const std::string &grav = "None",
                const std::string &drag = "None", const std::string &mag = "None")
         {
            bodyName = body; gravType = grav; dragType = drag; magfType = mag;
            gravDegree = "0"; gravOrder = "0"; magfDegree = "0"; 
            magfOrder = "0"; potFilename = ""; pmf = NULL; gravf = NULL; 
            dragf = NULL; srpf = NULL; useSrp = false;
         }
      
      ForceType& operator= (const ForceType& right)
         {
            if (this == &right)
               return *this;
            
            bodyName = right.bodyName; gravType = right.gravType;
            dragType = right.dragType; magfType = right.magfType; 
            gravDegree = right.gravDegree; gravOrder = right.gravOrder;
            magfDegree = right.magfDegree; magfOrder = right.magfOrder;
            potFilename = right.potFilename; pmf = right.pmf;
            gravf = right.gravf; dragf = right.dragf; srpf = right.srpf;
            useSrp = right.useSrp;
            return *this;
         }
   };
   
   wxStaticText *integratorStaticText;
   wxStaticText *initialStepSizeStaticText;
   wxStaticText *unitsInitStepSizeStaticText;
   wxStaticText *accuracyStaticText;
   wxStaticText *unitsMinStepStaticText;
   wxStaticText *unitsMaxStepStaticText;
   wxStaticText *minStepStaticText;
   wxStaticText *maxStepStaticText;
   wxStaticText *maxStepAttemptStaticText;
   wxStaticText *minIntErrorStaticText;
   wxStaticText *nomIntErrorStaticText;
   wxStaticText *errorCtrlStaticText;
   wxStaticText *pointMassStaticText;
   wxStaticText *centralBodyStaticText;
   wxStaticText *degree1StaticText;
   wxStaticText *order1StaticText;
   wxStaticText *potFileStaticText;
   wxStaticText *type1StaticText;
   wxStaticText *type2StaticText;
   wxStaticText *type3StaticText;
   wxStaticText *degree2StaticText;
   wxStaticText *order2StaticText;
   wxStaticText *type4StaticText;
   
   wxTextCtrl *initialStepSizeTextCtrl;
   wxTextCtrl *accuracyTextCtrl;
   wxTextCtrl *minStepTextCtrl;
   wxTextCtrl *maxStepTextCtrl;
   wxTextCtrl *maxStepAttemptTextCtrl;
   wxTextCtrl *minIntErrorTextCtrl;
   wxTextCtrl *nomIntErrorTextCtrl;
   wxTextCtrl *bodyTextCtrl;
   wxTextCtrl *gravityDegreeTextCtrl;
   wxTextCtrl *gravityOrderTextCtrl;
   wxTextCtrl *potFileTextCtrl;
   wxTextCtrl *magneticDegreeTextCtrl;
   wxTextCtrl *magneticOrderTextCtrl;
   wxTextCtrl *pmEditTextCtrl;
   
   wxComboBox *integratorComboBox;
   wxComboBox *originComboBox;
   wxComboBox *bodyComboBox;
   wxComboBox *gravComboBox;
   wxComboBox *atmosComboBox;
   wxComboBox *magfComboBox;
   wxComboBox *errorComboBox;
   
   wxButton *bodyButton;
   wxButton *searchGravityButton;
   wxButton *dragSetupButton;
   wxButton *searchMagneticButton;
   wxButton *editPmfButton;
   
   wxCheckBox *srpCheckBox;
   
   wxString integratorString;
   wxString primaryBodyString;
   wxString gravityFieldString;
   wxString atmosModelString;
   
   wxBoxSizer *leftBoxSizer;
   
   std::string propSetupName;
   std::string thePropagatorName;
   std::string currentBodyName;
   std::string gravTypeName;
   std::string dragTypeName;
   std::string propOriginName;
   std::string errorControlTypeName;
   
   StringArray propagatorTypeArray;
   StringArray earthGravModelArray;
   StringArray lunaGravModelArray;
   StringArray venusGravModelArray;
   StringArray marsGravModelArray;
   StringArray othersGravModelArray;
   StringArray dragModelArray;
   StringArray magfModelArray;
   StringArray errorControlArray;
   
   std::map<std::string, std::string> theFileMap;
   
   wxArrayString primaryBodiesArray;
   wxArrayString secondaryBodiesArray;
   wxArrayString integratorArray;
   
   Integer numOfBodies;
   Integer numOfForces;
   Integer currentBodyId;
   
   /// normalized harmonic coefficients
   Real               Cbar[361][361];
   /// normalized harmonic coefficients
   Real               Sbar[361][361];
   /// coefficient drifts per year
   Real               dCbar[17][17];
   /// coefficient drifts per year
   Real               dSbar[17][17];
      
   bool useDragForce;
   bool isForceModelChanged;
   bool isDegOrderChanged;
   bool isPotFileChanged;
   bool isMagfTextChanged;
   bool isIntegratorChanged;
   bool isIntegratorDataChanged;
   bool isOriginChanged;

   Propagator                     *thePropagator;
   PropSetup                      *thePropSetup;
   ForceModel                     *theForceModel;
   PointMassForce                 *thePMF;
   DragForce                      *theDragForce;
   GravityField                   *theGravForce;
   SolarRadiationPressure         *theSRP;
   SolarSystem                    *theSolarSystem;
   CelestialBody                  *theCelestialBody;
   AtmosphereModel                *theAtmosphereModel;
   std::vector<PointMassForce *>  thePMForces;
   std::vector<ForceType*> forceList;
   std::vector<ForceType*> pmForceList;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // Layout & data handling methods
   Integer FindBody(const std::string &bodyName,
                    const std::string &gravType = "None",
                    const std::string &dragType = "None",
                    const std::string &magfType = "None");
   void Initialize();
   void Setup(wxWindow *parent);
   void DisplayIntegratorData(bool integratorChanged);
   void DisplayPrimaryBodyData();
   void DisplayForceData();
   void DisplayGravityFieldData(std::string bodyName);
   void DisplayAtmosphereModelData();
   void DisplayPointMassData();
   void DisplayMagneticFieldData();
   void DisplaySRPData();
   void DisplayErrorControlData();

   // Saving data
   void SaveDegOrder();
   void SavePotFile();
   bool SaveIntegratorData();
   
   // Converting Data
   wxString ToString(Real rval);
   
   // Text control event method
   void OnIntegratorTextUpdate(wxCommandEvent &event);
   void OnGravityTextUpdate(wxCommandEvent& event);
   void OnMagneticTextUpdate(wxCommandEvent& event);
   
   // Checkbox event method
   void OnSRPCheckBoxChange(wxCommandEvent &event);
   
   // Combobox event method
   void OnIntegratorSelection(wxCommandEvent &event);
   void OnBodyComboBoxChange(wxCommandEvent &event);
   void OnOriginComboBoxChange(wxCommandEvent &event);
   void OnBodySelection(wxCommandEvent &event);
   void OnGravitySelection(wxCommandEvent &event);
   void OnAtmosphereSelection(wxCommandEvent &event);
   void OnErrorControlSelection(wxCommandEvent &event);
   
   // Button event methods
   void OnAddBodyButton(wxCommandEvent &event);
   void OnGravSearchButton(wxCommandEvent &event);
   void OnSetupButton(wxCommandEvent &event);
   void OnMagSearchButton(wxCommandEvent &event);
   void OnPMEditButton(wxCommandEvent &event);
   void OnSRPEditButton(wxCommandEvent &event);
   
   // for Debug
   void ShowPropData(const std::string &header);
   void ShowForceList(const std::string &header);
   void ShowForceModel(const std::string &header);
   
   // for reading gravity files
   void ParseDATGravityFile(std::string fname);
   void ParseGRVGravityFile(std::string fname);
   void ParseCOFGravityFile(std::string fname);
   void PrepareGravityArrays();
   
   // Strictly for reading gravity files
   static const Integer GRAV_MAX_DRIFT_DEGREE = 2;
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
    
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 42000,
      ID_TEXTCTRL,
      ID_TEXTCTRL_PROP,
      ID_TEXTCTRL_GRAV,
      ID_TEXTCTRL_MAGF,
      ID_CHECKBOX,
      ID_CB_INTGR,
      ID_CB_BODY,
      ID_CB_ORIGIN,
      ID_CB_GRAV,
      ID_CB_ATMOS,
      ID_CB_MAG,
      ID_CB_ERROR,
      ID_BUTTON_ADD_BODY,
      ID_BUTTON_GRAV_SEARCH,
      ID_BUTTON_SETUP,
      ID_BUTTON_MAG_SEARCH,
      ID_BUTTON_PM_EDIT,
      ID_BUTTON_SRP_EDIT 
   };
};

#endif // PropagationConfigPanel_hpp

