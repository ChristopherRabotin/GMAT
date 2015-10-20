//$Id$
//------------------------------------------------------------------------------
//                            PowerSystemConfigPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan
// Created: 2014.05.13
/**
 * This class contains information needed to setup spacecraft power system data.
 *
 */
//------------------------------------------------------------------------------
#ifndef PowerSystemConfigPanel_hpp
#define PowerSystemConfigPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"

class PowerSystemConfigPanel: public GmatPanel
{
public:
   PowerSystemConfigPanel(wxWindow *parent, const wxString &name);
   ~PowerSystemConfigPanel();

   void         LoadData();
   void         SaveData();
   virtual void SaveData(GmatBase *theObject);

   bool IsDataChanged() { return dataChanged; }
   bool CanClosePanel() { return canClose; }

private:

   // local copy of object for verifying changes before commit/apply
   GmatBase *localObject;
   GmatBase *theObject;

   bool     isSolar;

   bool     dataChanged;
//   bool     canClose;

   Real     theEpoch;

   bool isEpochFormatChanged;
   bool isEpochChanged;
   bool isEpochTextChanged;
   bool isInitPowerChanged;
   bool isDecayRateChanged;
   bool isMarginChanged;
   bool isShadowModelChanged;
   bool isBodyListChanged;
   bool isBusCoeffChanged;
   bool isSolarCoeffChanged;

   std::string fromEpochFormat;

   std::string epochStr;
   std::string taiMjdStr;

   std::string errMsgFormat;

   void Create();

   // Event Handling
   DECLARE_EVENT_TABLE();
   void OnComboBoxChange(wxCommandEvent& event);
   void OnTextChange(wxCommandEvent& event);
   void OnBodiesEditButton(wxCommandEvent &event);

   wxString ToString(Real rval);
   void     ResetChangedFlags();

   wxStaticText *epochFormatTxt;
   wxStaticText *epochTxt;
   wxStaticText *initPowerTxt;
   wxStaticText *decayRateTxt;
   wxStaticText *powerMarginTxt;
   wxStaticText *busCoeff1Txt;
   wxStaticText *busCoeff2Txt;
   wxStaticText *busCoeff3Txt;

   wxStaticText *shadowModelTxt;
   wxStaticText *shadowBodiesTxt;

   wxStaticText *solarCoeff1Txt;
   wxStaticText *solarCoeff2Txt;
   wxStaticText *solarCoeff3Txt;
   wxStaticText *solarCoeff4Txt;
   wxStaticText *solarCoeff5Txt;


   wxStaticText *initPowerUnits;
   wxStaticText *decayRateUnits;
   wxStaticText *powerMarginUnits;
   wxStaticText *busCoeff1Units;
   wxStaticText *busCoeff2Units;
   wxStaticText *busCoeff3Units;

   wxStaticText *solarCoeff1Units;
   wxStaticText *solarCoeff2Units;
   wxStaticText *solarCoeff3Units;
   wxStaticText *solarCoeff4Units;
   wxStaticText *solarCoeff5Units;

   wxTextCtrl   *epochTxtCtrl;
   wxTextCtrl   *initPowerTxtCtrl;
   wxTextCtrl   *decayRateTxtCtrl;
   wxTextCtrl   *powerMarginTxtCtrl;
   wxTextCtrl   *busCoeff1TxtCtrl;
   wxTextCtrl   *busCoeff2TxtCtrl;
   wxTextCtrl   *busCoeff3TxtCtrl;

   wxTextCtrl   *shadowBodiesTxtCtrl;

   wxTextCtrl   *solarCoeff1TxtCtrl;
   wxTextCtrl   *solarCoeff2TxtCtrl;
   wxTextCtrl   *solarCoeff3TxtCtrl;
   wxTextCtrl   *solarCoeff4TxtCtrl;
   wxTextCtrl   *solarCoeff5TxtCtrl;


   wxComboBox *epochFormatComboBox;
   wxComboBox *shadowModelComboBox;

   wxButton   *shadowBodiesButton;

   StringArray shadowBodiesList;

   // IDs for the controls and the menu commands
   enum
   {
      ID_TEXT = 30280,
      ID_TEXTCTRL,
      ID_FORMAT_COMBOBOX,
      ID_SHADOWMODEL_COMBOBOX,
      ID_BUTTON_BODIES,
   };
};
#endif

