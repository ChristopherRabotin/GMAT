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

#include "CelestialBodyOrientationPanel.hpp"
#include "SolarSystem.hpp"
#include "Planet.hpp"
#include "GmatBaseException.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"
#include "A1Mjd.hpp"
#include <wx/config.h>
#include <fstream>

//#define DEBUG_CB_ORIENTATION_PANEL
//#define DEBUG_CB_ORIENT_SAVE

// event tables for wxMac/Widgets
BEGIN_EVENT_TABLE(CelestialBodyOrientationPanel, wxPanel)
   EVT_COMBOBOX(ID_COMBO_BOX_ROTATION_DATA_SOURCE, CelestialBodyOrientationPanel::OnRotationDataSourceComboBoxChange)
   EVT_TEXT(ID_TEXT_CTRL_NUTATION_UPDATE_INTERVAL, CelestialBodyOrientationPanel::OnNutationUpdateIntervalTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_SPIN_AXIS_RA_CONSTANT, CelestialBodyOrientationPanel::OnSpinAxisRAConstantTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_SPIN_AXIS_RA_RATE, CelestialBodyOrientationPanel::OnSpinAxisRARateTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_SPIN_AXIS_DEC_CONSTANT, CelestialBodyOrientationPanel::OnSpinAxisDECConstantTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_SPIN_AXIS_DEC_RATE, CelestialBodyOrientationPanel::OnSpinAxisDECRateTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_ROTATION_CONSTANT, CelestialBodyOrientationPanel::OnRotationConstantTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_ROTATION_RATE, CelestialBodyOrientationPanel::OnRotationRateTextCtrlChange)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

CelestialBodyOrientationPanel::CelestialBodyOrientationPanel(GmatPanel *cbPanel, 
                               wxWindow *parent, CelestialBody *body) :
   wxPanel                       (parent),
   dataChanged                   (false),
   canClose                      (true),
   theBody                       (body), 
   rotationDataSource            (""),
   nutationUpdateInterval        (60.0),
   spinAxisRAConstant            (0.0),
   spinAxisRARate                (0.0),
   spinAxisDECConstant           (0.0),
   spinAxisDECRate               (0.0),
   rotationConstant              (0.0),
   rotationRate                  (0.0),
   rotationDataSourceChanged     (false),
   nutationUpdateIntervalChanged (false),
   spinAxisRAConstantChanged     (false),
   spinAxisRARateChanged         (false),
   spinAxisDECConstantChanged    (false),
   spinAxisDECRateChanged        (false),
   rotationConstantChanged       (false),
   rotationRateChanged           (false),
   isEarth                       (false),
   isLuna                        (false),
   theCBPanel                    (cbPanel),
   userDef                       (false)
{
   guiManager     = GuiItemManager::GetInstance();
//   guiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   
   Create();
}

CelestialBodyOrientationPanel::~CelestialBodyOrientationPanel()
{
   // nothing to do ... la la la la la ...
}

void CelestialBodyOrientationPanel::SaveData()
{
   std::string strval;
   Real        tmpval;
   bool        realsOK   = true;
   bool        stringsOK = true;
   
   // don't do anything if no data has been changed.
   // note that dataChanged will be true if the user modified any combo box or
   // text ctrl, whether or not he/she actually changed the value
   // Note - as of 2009.02.18, the hasBeenModified flag is not yet working correctly 
   if (!dataChanged) return;

   canClose    = true;
   
   if (rotationDataSourceChanged)
   {
      strval = rotationDataSourceComboBox->GetValue();
      rotationDataSource = strval;
   }
   if (isEarth && nutationUpdateIntervalChanged)
   {
      strval = nutationUpdateIntervalTextCtrl->GetValue();
      if (!theCBPanel->CheckReal(tmpval, strval, "Nutation Update Interval", "Real Number >= 0.0", false, true, true, true))
         realsOK = false;
      else 
      {
         nutationUpdateInterval = tmpval;
      }
   }
   if (spinAxisRAConstantChanged)
   {
      strval = spinAxisRAConstantTextCtrl->GetValue();
      if (!theCBPanel->CheckReal(tmpval, strval, "Spin Axis RA Constant", "Real Number"))
         realsOK = false;
      else 
      {
         spinAxisRAConstant = tmpval;
      }
   }
   if (spinAxisRARateChanged)
   {
      strval = spinAxisRARateTextCtrl->GetValue();
      if (!theCBPanel->CheckReal(tmpval, strval, "Spin Axis RA Rate", "Real Number"))
         realsOK = false;
      else 
      {
         spinAxisRARate = tmpval;
      }
   }
   if (spinAxisDECConstantChanged)
   {
      strval = spinAxisDECConstantTextCtrl->GetValue();
      if (!theCBPanel->CheckReal(tmpval, strval, "Spin Axis DEC Constant", "Real Number"))
         realsOK = false;
      else 
      {
         spinAxisDECConstant = tmpval;
      }
   }
   if (spinAxisDECRateChanged)
   {
      strval = spinAxisDECRateTextCtrl->GetValue();
      if (!theCBPanel->CheckReal(tmpval, strval, "Spin Axis DEC Rate", "Real Number"))
         realsOK = false;
      else 
      {
         spinAxisDECRate = tmpval;
      }
   }
   if (rotationConstantChanged)
   {
      strval = rotationConstantTextCtrl->GetValue();
      if (!theCBPanel->CheckReal(tmpval, strval, "Rotation Constant", "Real Number"))
         realsOK = false;
      else 
      {
         rotationConstant = tmpval;
      }
   }
   if (rotationRateChanged)
   {
      strval = rotationRateTextCtrl->GetValue();
      if (!theCBPanel->CheckReal(tmpval, strval, "Rotation Rate", "Real Number"))
         realsOK = false;
      else 
      {
         rotationRate = tmpval;
      }
   }

   
   if (!realsOK)
   {
      std::string errmsg = "Please enter valid Real values before saving data.\n";
      MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
   }

   if (realsOK && stringsOK)
   {
      theBody->SetStringParameter(theBody->GetParameterID("RotationDataSource"), 
                                  rotationDataSource);
      if (isEarth)
      {
         Planet *thePlanet = (Planet*) theBody;
         thePlanet->SetNutationUpdateInterval(nutationUpdateInterval);
      }
      Rvector6 orientation(spinAxisRAConstant, spinAxisRARate,   spinAxisDECConstant,
                           spinAxisDECRate,    rotationConstant, rotationRate);
      theBody->SetOrientationParameters(orientation);
      dataChanged = false;
      ResetChangeFlags(true);
   }
   else
   {
      #ifdef DEBUG_CB_ORIENT_SAVE
      MessageInterface::ShowMessage("in CBOrientPanel::SaveData, realsOK = %s\n",
            (realsOK? "true" : "false"));
      MessageInterface::ShowMessage("                            stringsOK = %s\n",
            (stringsOK? "true" : "false"));
      #endif
      canClose = false;
   }
   #ifdef DEBUG_CB_ORIENT_SAVE
      MessageInterface::ShowMessage("at end of CBOrientPanel::SaveData, canClose = %s\n",
            (canClose? "true" : "false"));
      MessageInterface::ShowMessage("at end of CBOrientPanel::SaveData, isEarth = %s\n",
            (isEarth? "true" : "false"));
   #endif
   
}

void CelestialBodyOrientationPanel::LoadData()
{
   try
   {
//      StringArray ephemSourceList = theBody->GetEphemSourceList();
//      for (unsigned int ii = 0; ii < ephemSourceList.size(); ii++)
//         ephemSourceComboBox-Append((ephemSourceList.at[ii]).c_str());

//      if (isLuna)
//      {
         rotationDataSource = theBody->GetStringParameter(theBody->GetParameterID("RotationDataSource"));
         rotationDataSourceComboBox->SetValue(rotationDataSource.c_str());
//      }
          
      if (isEarth)
      {
         Planet *thePlanet = (Planet*) theBody;
         nutationUpdateInterval = thePlanet->GetNutationUpdateInterval();
         nutationUpdateIntervalStringWX = guiManager->ToWxString(nutationUpdateInterval);
         nutationUpdateIntervalTextCtrl->SetValue(nutationUpdateIntervalStringWX);
      }
      else // this will be grayed out or not visible anyway
         nutationUpdateInterval = 0.0;
        
      Rvector6 orientation = theBody->GetOrientationParameters();
      spinAxisRAConstant   = orientation[0];
      spinAxisRARate       = orientation[1];
      spinAxisDECConstant  = orientation[2];
      spinAxisDECRate      = orientation[3];
      rotationConstant     = orientation[4];
      rotationRate         = orientation[5];
      
      spinAxisRAConstantStringWX = guiManager->ToWxString(orientation[0]);
      spinAxisRAConstantTextCtrl->SetValue(spinAxisRAConstantStringWX);
      spinAxisRARateStringWX = guiManager->ToWxString(orientation[1]);
      spinAxisRARateTextCtrl->SetValue(spinAxisRARateStringWX);
      spinAxisDECConstantStringWX = guiManager->ToWxString(orientation[2]);
      spinAxisDECConstantTextCtrl->SetValue(spinAxisDECConstantStringWX);
      spinAxisDECRateStringWX = guiManager->ToWxString(orientation[3]);
      spinAxisDECRateTextCtrl->SetValue(spinAxisDECRateStringWX);
      rotationConstantStringWX = guiManager->ToWxString(orientation[4]);
      rotationConstantTextCtrl->SetValue(rotationConstantStringWX);
      rotationRateStringWX = guiManager->ToWxString(orientation[5]);
      rotationRateTextCtrl->SetValue(rotationRateStringWX);
      
      // @todo - onoy make those things visible that should be
      ResetChangeFlags();
      
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}



//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
void CelestialBodyOrientationPanel::Create()
{
   int bSize     = 2;
   isEarth       = false;
   isLuna        = false;
   userDef       = theBody->IsUserDefined();
   
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Celestial Body Orientation"));

   GmatStaticBoxSizer  *boxSizer1 = new GmatStaticBoxSizer(wxVERTICAL, this, "Orientation Data");
//   GmatStaticBoxSizer  *boxSizer2 = new GmatStaticBoxSizer(wxVERTICAL, this, "");
    
   // empty the temporary value strings
   rotationDataSourceStringWX     = "";
   nutationUpdateIntervalStringWX = "";
   spinAxisRAConstantStringWX     = "";
   spinAxisRARateStringWX         = "";
   spinAxisDECConstantStringWX    = "";
   spinAxisDECRateStringWX        = "";
   rotationConstantStringWX       = "";
   rotationRateStringWX           = "";
   
//   // rotation data source combo box
//   sourceArray              = theBody->GetRotationDataSourceList();
//   unsigned int ephemListSz = sourceArray.size();
//   sourceArrayWX            = new wxString[ephemListSz];
//   for (unsigned int jj = 0; jj < ephemListSz; jj++)
//      sourceArrayWX[jj] = wxT(sourceArray[jj].c_str());
//   rotationDataSourceStaticText = new wxStaticText(this, ID_TEXT, wxT("Rotation Data Source"),
//                                  wxDefaultPosition, wxSize(-1,-1), 0);
//   rotationDataSourceComboBox   = new wxComboBox(this, ID_COMBO_BOX_ROTATION_DATA_SOURCE, wxT(sourceArrayWX[0]),
//                                  wxDefaultPosition, wxDefaultSize, ephemListSz, sourceArrayWX,
//                                  wxCB_DROPDOWN|wxCB_READONLY);
//   // nutation update interval
//   nutationUpdateIntervalStaticText =  new wxStaticText(this, ID_TEXT, wxT("Nutation Update Interval"),
//                                       wxDefaultPosition, wxSize(-1,-1), 0);
//   nutationUpdateIntervalTextCtrl   = new wxTextCtrl(this, ID_TEXT_CTRL_NUTATION_UPDATE_INTERVAL, wxT(""),
//                                       wxDefaultPosition, wxSize(60,-1), 0);
//   nutationUpdateIntervalUnitsStaticText = new wxStaticText(this, ID_TEXT, wxT("sec"),
//                                           wxDefaultPosition, wxSize(-1,-1), 0);
   
   // orientation data
   spinAxisRAConstantStaticText  = new wxStaticText(this, ID_TEXT,wxT("Spin Axis R"GUI_ACCEL_KEY"A Constant"),
                                   wxDefaultPosition, wxSize(-1,-1), 0);
   spinAxisRAConstantTextCtrl    = new wxTextCtrl(this, ID_TEXT_CTRL_SPIN_AXIS_RA_CONSTANT, wxT(""),
                                   wxDefaultPosition, wxSize(150, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   spinAxisRAConstantTextCtrl->SetToolTip(pConfig->Read(_T("SpinAxisRAConstantHint")));
   spinAxisRAConstantUnitsStaticText = new wxStaticText(this, ID_TEXT, wxT("deg"),
                                       wxDefaultPosition, wxSize(-1,-1), 0);

   spinAxisRARateStaticText      = new wxStaticText(this, ID_TEXT,wxT("Spin Axis RA "GUI_ACCEL_KEY"Rate"),
                                   wxDefaultPosition, wxSize(-1,-1), 0);
   spinAxisRARateTextCtrl        = new wxTextCtrl(this, ID_TEXT_CTRL_SPIN_AXIS_RA_RATE, wxT(""),
                                   wxDefaultPosition, wxSize(150, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   spinAxisRARateTextCtrl->SetToolTip(pConfig->Read(_T("SpinAxisRARateHint")));
   spinAxisRARateUnitsStaticText = new wxStaticText(this, ID_TEXT, wxT("deg/sec"),
                                   wxDefaultPosition, wxSize(-1,-1), 0);

   spinAxisDECConstantStaticText = new wxStaticText(this, ID_TEXT,wxT("Spin Axis "GUI_ACCEL_KEY"DEC Constant"),
                                   wxDefaultPosition, wxSize(-1,-1), 0);
   spinAxisDECConstantTextCtrl   = new wxTextCtrl(this, ID_TEXT_CTRL_SPIN_AXIS_DEC_CONSTANT, wxT(""),
                                   wxDefaultPosition, wxSize(150, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   spinAxisDECConstantTextCtrl->SetToolTip(pConfig->Read(_T("SpinAxisDECConstantHint")));
   spinAxisDECConstantUnitsStaticText = new wxStaticText(this, ID_TEXT, wxT("deg"),
                                        wxDefaultPosition, wxSize(-1,-1), 0);

   spinAxisDECRateStaticText     = new wxStaticText(this, ID_TEXT,wxT("Spin Axis DEC "GUI_ACCEL_KEY"Rate"),
                                   wxDefaultPosition, wxSize(-1,-1), 0);
   spinAxisDECRateTextCtrl       = new wxTextCtrl(this, ID_TEXT_CTRL_SPIN_AXIS_DEC_RATE, wxT(""),
                                   wxDefaultPosition, wxSize(150, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   spinAxisDECRateTextCtrl->SetToolTip(pConfig->Read(_T("SpinAxisDECRateHint")));
   spinAxisDECRateUnitsStaticText = new wxStaticText(this, ID_TEXT, wxT("deg/sec"),
                                    wxDefaultPosition, wxSize(-1,-1), 0);

   rotationConstantStaticText    = new wxStaticText(this, ID_TEXT,wxT("R"GUI_ACCEL_KEY"otation Constant"),
                                   wxDefaultPosition, wxSize(-1,-1), 0);
   rotationConstantTextCtrl      = new wxTextCtrl(this, ID_TEXT_CTRL_ROTATION_CONSTANT, wxT(""),
                                   wxDefaultPosition, wxSize(150, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   rotationConstantTextCtrl->SetToolTip(pConfig->Read(_T("RotationConstantHint")));
   rotationConstantUnitsStaticText = new wxStaticText(this, ID_TEXT, wxT("deg"),
                                     wxDefaultPosition, wxSize(-1,-1), 0);

   rotationRateStaticText    = new wxStaticText(this, ID_TEXT,wxT("Rotation "GUI_ACCEL_KEY"Rate"),
                                   wxDefaultPosition, wxSize(-1,-1), 0);
   rotationRateTextCtrl      = new wxTextCtrl(this, ID_TEXT_CTRL_ROTATION_RATE, wxT(""),
                                   wxDefaultPosition, wxSize(150, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   rotationRateTextCtrl->SetToolTip(pConfig->Read(_T("RotationRateHint")));
   rotationRateUnitsStaticText = new wxStaticText(this, ID_TEXT, wxT("deg/sec"),
                                 wxDefaultPosition, wxSize(-1,-1), 0);
  
   
// sizers
   if (theBody->GetName() == SolarSystem::EARTH_NAME) isEarth = true;
   else if (theBody->GetName() == SolarSystem::MOON_NAME) isLuna = true;

   wxFlexGridSizer *oneFlexGridSizer = new wxFlexGridSizer(3,0,0);

   oneFlexGridSizer->Add(spinAxisRAConstantStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   oneFlexGridSizer->Add(spinAxisRAConstantTextCtrl,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   oneFlexGridSizer->Add(spinAxisRAConstantUnitsStaticText,0, wxALIGN_LEFT|wxALL, bSize);

   oneFlexGridSizer->Add(spinAxisRARateStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   oneFlexGridSizer->Add(spinAxisRARateTextCtrl,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   oneFlexGridSizer->Add(spinAxisRARateUnitsStaticText,0, wxALIGN_LEFT|wxALL, bSize);

   oneFlexGridSizer->Add(spinAxisDECConstantStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   oneFlexGridSizer->Add(spinAxisDECConstantTextCtrl,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   oneFlexGridSizer->Add(spinAxisDECConstantUnitsStaticText,0, wxALIGN_LEFT|wxALL, bSize);

   oneFlexGridSizer->Add(spinAxisDECRateStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   oneFlexGridSizer->Add(spinAxisDECRateTextCtrl,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   oneFlexGridSizer->Add(spinAxisDECRateUnitsStaticText,0, wxALIGN_LEFT|wxALL, bSize);

   oneFlexGridSizer->Add(rotationConstantStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   oneFlexGridSizer->Add(rotationConstantTextCtrl,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   oneFlexGridSizer->Add(rotationConstantUnitsStaticText,0, wxALIGN_LEFT|wxALL, bSize);

   oneFlexGridSizer->Add(rotationRateStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   oneFlexGridSizer->Add(rotationRateTextCtrl,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   oneFlexGridSizer->Add(rotationRateUnitsStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   
   if (isEarth)
   {
      // add nutation update interval for Earth
      nutationUpdateIntervalStaticText =  new wxStaticText(this, ID_TEXT, wxT(GUI_ACCEL_KEY"Nutation Update Interval"),
                                          wxDefaultPosition, wxSize(-1,-1), 0);
      nutationUpdateIntervalTextCtrl   = new wxTextCtrl(this, ID_TEXT_CTRL_NUTATION_UPDATE_INTERVAL, wxT(""),
                                          wxDefaultPosition, wxSize(60,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
      nutationUpdateIntervalTextCtrl->SetToolTip(pConfig->Read(_T("NutationUpdateIntervalHint")));
      nutationUpdateIntervalUnitsStaticText = new wxStaticText(this, ID_TEXT, wxT("sec"),
                                              wxDefaultPosition, wxSize(-1,-1), 0);
      oneFlexGridSizer->Add(nutationUpdateIntervalStaticText, 0,
                                         wxGROW|wxALIGN_LEFT|wxALL, bSize);
      oneFlexGridSizer->Add(nutationUpdateIntervalTextCtrl, 0, 
                                         wxGROW|wxALIGN_LEFT|wxALL, bSize);
      oneFlexGridSizer->Add(nutationUpdateIntervalUnitsStaticText, 0,
                                         wxALIGN_LEFT|wxALL, bSize);
   }
//   else if (isLuna)
//   {
      // add rotation data source combo box for Luna
      sourceArray              = theBody->GetRotationDataSourceList();
      unsigned int ephemListSz = sourceArray.size();
      sourceArrayWX            = new wxString[ephemListSz];
      for (unsigned int jj = 0; jj < ephemListSz; jj++)
         sourceArrayWX[jj] = wxT(sourceArray[jj].c_str());
      rotationDataSourceStaticText = new wxStaticText(this, ID_TEXT, wxT("Rotation Data "GUI_ACCEL_KEY"Source"),
                                     wxDefaultPosition, wxSize(-1,-1), 0);
      rotationDataSourceComboBox   = new wxComboBox(this, ID_COMBO_BOX_ROTATION_DATA_SOURCE, wxT(sourceArrayWX[0]),
                                     wxDefaultPosition, wxDefaultSize, ephemListSz, sourceArrayWX,
                                     wxCB_DROPDOWN|wxCB_READONLY);
      rotationDataSourceComboBox->SetToolTip(pConfig->Read(_T("RotationDataSourceHint")));
      oneFlexGridSizer->Add(rotationDataSourceStaticText, 0,
                                         wxGROW|wxALIGN_LEFT|wxALL, bSize);
      oneFlexGridSizer->Add(rotationDataSourceComboBox, 0, 
                                         wxGROW|wxALIGN_LEFT|wxALL, bSize);
      oneFlexGridSizer->Add(20,20, 0,
                                         wxALIGN_LEFT|wxALL, bSize);
      rotationDataSourceComboBox->Disable();
//   }
      
   // for now, don't let user modify any values for default bodies
   if (!userDef)
   {
      spinAxisRAConstantTextCtrl->Disable();
      spinAxisRARateTextCtrl->Disable();
      spinAxisDECConstantTextCtrl->Disable();
      spinAxisDECRateTextCtrl->Disable();
      rotationConstantTextCtrl->Disable();
      rotationRateTextCtrl->Disable();
   }

   boxSizer1->Add(oneFlexGridSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bSize);
   
   mainBoxSizer    = new GmatStaticBoxSizer(wxVERTICAL, this, "");
   
   mainBoxSizer->Add(boxSizer1, 1, wxALIGN_LEFT|wxGROW, bSize); 
   
   this->SetAutoLayout(true);
   this->SetSizer(mainBoxSizer);
   mainBoxSizer->Fit(this);
   mainBoxSizer->SetSizeHints(this);
}

void CelestialBodyOrientationPanel::ResetChangeFlags(bool discardMods)
{
   rotationDataSourceChanged     = false;
   nutationUpdateIntervalChanged = false;
   spinAxisRAConstantChanged     = false;
   spinAxisRARateChanged         = false;
   spinAxisDECConstantChanged    = false;
   spinAxisDECRateChanged        = false;
   rotationConstantChanged       = false;
   rotationRateChanged           = false;
   
   if (discardMods)
   {
//      rotationDataSourceComboBox->DiscardEdits();
      if (isEarth) nutationUpdateIntervalTextCtrl->DiscardEdits();
      spinAxisRAConstantTextCtrl->DiscardEdits();
      spinAxisRARateTextCtrl->DiscardEdits();
      spinAxisDECConstantTextCtrl->DiscardEdits();
      spinAxisDECRateTextCtrl->DiscardEdits();
      rotationConstantTextCtrl->DiscardEdits();
      rotationRateTextCtrl->DiscardEdits();
   }
}

//Event Handling
void CelestialBodyOrientationPanel::OnRotationDataSourceComboBoxChange(wxCommandEvent &event)
{
   std::string newSrc = (rotationDataSourceComboBox->GetStringSelection()).c_str();
   if (newSrc == rotationDataSource) return;
   rotationDataSourceChanged = true;
   dataChanged               = true;
//   rotationDataSource        = newSrc;
   theCBPanel->EnableUpdate(true);
}

void CelestialBodyOrientationPanel::OnNutationUpdateIntervalTextCtrlChange(wxCommandEvent &event)
{
   if (nutationUpdateIntervalTextCtrl->IsModified())
   {
      nutationUpdateIntervalChanged  = true;
      dataChanged                    = true;
      theCBPanel->EnableUpdate(true);
   }
}

void CelestialBodyOrientationPanel::OnSpinAxisRAConstantTextCtrlChange(wxCommandEvent &event)
{
   if (spinAxisRAConstantTextCtrl->IsModified())
   {
      spinAxisRAConstantChanged    = true;
      dataChanged                  = true;
      theCBPanel->EnableUpdate(true);
   }
}

void CelestialBodyOrientationPanel::OnSpinAxisRARateTextCtrlChange(wxCommandEvent &event)
{
   if (spinAxisRARateTextCtrl->IsModified())
   {
      spinAxisRARateChanged        = true;
      dataChanged                  = true;
      theCBPanel->EnableUpdate(true);
   }
}

void CelestialBodyOrientationPanel::OnSpinAxisDECConstantTextCtrlChange(wxCommandEvent &event)
{
   if (spinAxisDECConstantTextCtrl->IsModified())
   {
      spinAxisDECConstantChanged   = true;
      dataChanged                  = true;
      theCBPanel->EnableUpdate(true);
   }
}

void CelestialBodyOrientationPanel::OnSpinAxisDECRateTextCtrlChange(wxCommandEvent &event)
{
   if (spinAxisDECRateTextCtrl->IsModified())
   {
      spinAxisDECRateChanged       = true;
      dataChanged                  = true;
      theCBPanel->EnableUpdate(true);
   }
}

void CelestialBodyOrientationPanel::OnRotationConstantTextCtrlChange(wxCommandEvent &event)
{
   if (rotationConstantTextCtrl->IsModified())
   {
      rotationConstantChanged      = true;
      dataChanged                  = true;
      theCBPanel->EnableUpdate(true);
   }
}

void CelestialBodyOrientationPanel::OnRotationRateTextCtrlChange(wxCommandEvent &event)
{
   if (rotationRateTextCtrl->IsModified())
   {
      rotationRateChanged          = true;
      dataChanged                  = true;
      theCBPanel->EnableUpdate(true);
   }
}

