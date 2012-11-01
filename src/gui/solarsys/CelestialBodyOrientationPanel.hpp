//$Id$
//------------------------------------------------------------------------------
//                                  CelestialBodyOrientationPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Wendy C. Shoan
// Created: 2009.01.30
//
/**
 * This is the panel for the Orientation tab on the notebook on the CelestialBody
 * Panel.
 *
 */
//------------------------------------------------------------------------------

#ifndef CelestialBodyOrientationPanel_hpp
#define CelestialBodyOrientationPanel_hpp

#include "gmatdefs.hpp"
#include "CelestialBody.hpp"
#include "gmatwxdefs.hpp"
#include "GuiItemManager.hpp"
#include "GmatPanel.hpp"
#include "GmatStaticBoxSizer.hpp"

class CelestialBodyOrientationPanel : public wxPanel
{
public:
   CelestialBodyOrientationPanel(GmatPanel *cbPanel, wxWindow *parent, CelestialBody *body);
   ~CelestialBodyOrientationPanel();
   
   void SaveData();
   void LoadData();
   
   bool IsDataChanged() { return dataChanged;};
   bool CanClosePanel() { return canClose;};
   
private:
   
   bool           dataChanged;
   bool           canClose;
   
   CelestialBody  *theBody;
   GuiItemManager *guiManager;

   std::string    rotationDataSource;
   Real           nutationUpdateInterval;;
   Real           spinAxisRAConstant;
   Real           spinAxisRARate;
   Real           spinAxisDECConstant;
   Real           spinAxisDECRate;
   Real           rotationConstant;
   Real           rotationRate;
   
   bool           rotationDataSourceChanged;
   bool           nutationUpdateIntervalChanged;
   bool           spinAxisRAConstantChanged;
   bool           spinAxisRARateChanged;
   bool           spinAxisDECConstantChanged;
   bool           spinAxisDECRateChanged;
   bool           rotationConstantChanged;
   bool           rotationRateChanged;
   
   bool           isEarth;
   bool           isLuna;
   
   GmatPanel      *theCBPanel;
   bool           userDef;
   
  
   void     Create();
   void     ResetChangeFlags(bool discardMods = false);
   
   //Event Handling
   DECLARE_EVENT_TABLE();
   void     OnRotationDataSourceComboBoxChange(wxCommandEvent &event);
   void     OnNutationUpdateIntervalTextCtrlChange(wxCommandEvent &event);
   void     OnSpinAxisRAConstantTextCtrlChange(wxCommandEvent &event);
   void     OnSpinAxisRARateTextCtrlChange(wxCommandEvent &event);
   void     OnSpinAxisDECConstantTextCtrlChange(wxCommandEvent &event);
   void     OnSpinAxisDECRateTextCtrlChange(wxCommandEvent &event);
   void     OnRotationConstantTextCtrlChange(wxCommandEvent &event);
   void     OnRotationRateTextCtrlChange(wxCommandEvent &event);
   
   wxString ToString(Real rval);
   
   // wx
   
   wxStaticText *rotationDataSourceStaticText;
   wxStaticText *nutationUpdateIntervalStaticText;
   wxStaticText *spinAxisRAConstantStaticText;
   wxStaticText *spinAxisRARateStaticText;
   wxStaticText *spinAxisDECConstantStaticText;
   wxStaticText *spinAxisDECRateStaticText;
   wxStaticText *rotationConstantStaticText;
   wxStaticText *rotationRateStaticText;

   wxStaticText *nutationUpdateIntervalUnitsStaticText;
   wxStaticText *spinAxisRAConstantUnitsStaticText;
   wxStaticText *spinAxisRARateUnitsStaticText;
   wxStaticText *spinAxisDECConstantUnitsStaticText;
   wxStaticText *spinAxisDECRateUnitsStaticText;
   wxStaticText *rotationConstantUnitsStaticText;
   wxStaticText *rotationRateUnitsStaticText;

   wxTextCtrl   *nutationUpdateIntervalTextCtrl;
   wxTextCtrl   *spinAxisRAConstantTextCtrl;
   wxTextCtrl   *spinAxisRARateTextCtrl;
   wxTextCtrl   *spinAxisDECConstantTextCtrl;
   wxTextCtrl   *spinAxisDECRateTextCtrl;
   wxTextCtrl   *rotationConstantTextCtrl;
   wxTextCtrl   *rotationRateTextCtrl;
   
   wxComboBox   *rotationDataSourceComboBox;
   
   // strings for the combo box
   StringArray  sourceArray;
   
   // wxString arrays for the combo box
   wxString     *sourceArrayWX;
   
   /// string versions of current data
   wxString     rotationDataSourceStringWX;
   wxString     nutationUpdateIntervalStringWX;
   wxString     spinAxisRAConstantStringWX;
   wxString     spinAxisRARateStringWX;
   wxString     spinAxisDECConstantStringWX;
   wxString     spinAxisDECRateStringWX;
   wxString     rotationConstantStringWX;
   wxString     rotationRateStringWX;

   
   GmatStaticBoxSizer *mainBoxSizer;
   
   /// IDs for the controls 
   enum
   {
      ID_TEXT = 7100,
      ID_COMBO_BOX_ROTATION_DATA_SOURCE,
      ID_TEXT_CTRL_NUTATION_UPDATE_INTERVAL,
      ID_TEXT_CTRL_SPIN_AXIS_RA_CONSTANT,
      ID_TEXT_CTRL_SPIN_AXIS_RA_RATE,
      ID_TEXT_CTRL_SPIN_AXIS_DEC_CONSTANT,
      ID_TEXT_CTRL_SPIN_AXIS_DEC_RATE,
      ID_TEXT_CTRL_ROTATION_CONSTANT,
      ID_TEXT_CTRL_ROTATION_RATE,
   };
};
#endif // CelestialBodyOrientationPanel_hpp
