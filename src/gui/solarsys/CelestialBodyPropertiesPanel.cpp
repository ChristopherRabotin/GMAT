//$Id$
//------------------------------------------------------------------------------
//                                  CelestialBodyPropertiesPanel
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
// Created: 2009.01.14
//
/**
 * This is the panel for the Properties tab on the notebook on the CelestialBody
 * Panel.
 *
 */
//------------------------------------------------------------------------------

#include "CelestialBodyPropertiesPanel.hpp"
#include "GmatBaseException.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"
#include "bitmaps/OpenFolder.xpm"
#include <wx/config.h>
#include <fstream>

//#define DEBUG_CB_PROP_PANEL
//#define DEBUG_CB_PROP_SAVE


//------------------------------------------------------------------------------
// event tables for wxMac/Widgets
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(CelestialBodyPropertiesPanel, wxPanel)
   EVT_BUTTON(ID_BUTTON_BROWSE, CelestialBodyPropertiesPanel::OnBrowseButton)
   EVT_TEXT(ID_TEXT_CTRL_MU, CelestialBodyPropertiesPanel::OnMuTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_EQRAD, CelestialBodyPropertiesPanel::OnEqRadTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_FLAT, CelestialBodyPropertiesPanel::OnFlatTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_TEXTURE, CelestialBodyPropertiesPanel::OnTextureTextCtrlChange)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CelestialBodyPropertiesPanel(GmatPanel *cbPanel, wxWindow *parent,
//                              CelestialBody *body)
//------------------------------------------------------------------------------
/**
 * Creates the panel (default constructor).
 *
 * @param <cbPanel>             pointer to panel on which this one resides
 * @param <parent>              pointer to the parent window
 * @param <body>                body whose data the panel should display
 */
//------------------------------------------------------------------------------
CelestialBodyPropertiesPanel::CelestialBodyPropertiesPanel(GmatPanel *cbPanel, 
                              wxWindow *parent, CelestialBody *body) :
   wxPanel        (parent),
   dataChanged    (false),
   canClose       (true),
   theBody        (body),
   mu             (0.0),
   eqRad          (0.0),
   flat           (0.0),
   textureMap     (""),
   muChanged      (false),
   eqRadChanged   (false),
   flatChanged    (false),
   textureChanged (false),
   theCBPanel     (cbPanel)
{
   guiManager     = GuiItemManager::GetInstance();
   
   Create();
}

//------------------------------------------------------------------------------
// ~CelestialBodyPropertiesPanel()
//------------------------------------------------------------------------------
/**
 * Destroys the panel (destructor).
 *
 */
//------------------------------------------------------------------------------
CelestialBodyPropertiesPanel::~CelestialBodyPropertiesPanel()
{
   // nothing to do ... la la la la la ...
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves the data from the panel widgets to the body object.
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyPropertiesPanel::SaveData()
{
   std::string strval;
   Real        tmpval;
   bool        realsOK   = true;
   bool        stringsOK = true;
   
   
   // don't do anything if no data has been changed.
   // note that dataChanged will be true if the user modified any combo box or
   // text ctrl, whether or not he/she actually changed the value
   #ifdef DEBUG_CB_PROP_SAVE

      MessageInterface::ShowMessage("Entering CBPropPanel::SaveData, dataChanged = %s\n",
         (dataChanged? "true" : "false"));
      MessageInterface::ShowMessage("    muChanged = %s\n",
         (muChanged? "true" : "false"));
      MessageInterface::ShowMessage("    eqRadChanged = %s\n",
         (eqRadChanged? "true" : "false"));
      MessageInterface::ShowMessage("    flatChanged = %s\n",
         (flatChanged? "true" : "false"));
      MessageInterface::ShowMessage("    textureChanged = %s\n",
         (textureChanged? "true" : "false"));
   #endif
   if (!dataChanged) return;
   
   canClose    = true;
   
   if (muChanged)
   {
      strval = muTextCtrl->GetValue();
      if (!theCBPanel->CheckReal(tmpval, strval, "Mu", "Real Number > 0",
            false, true, true, false))
         realsOK = false;
      else 
      {
         mu = tmpval;
      }
   }
   if (eqRadChanged)
   {
      strval = eqRadTextCtrl->GetValue();
      if (!theCBPanel->CheckReal(tmpval, strval, "Equatorial Radius", "Real Number > 0",
            false, true, true, false))
         realsOK = false;
      else 
      {
         eqRad = tmpval;
      }
   }
   if (flatChanged)
   {
      strval = flatTextCtrl->GetValue();
      if (!theCBPanel->CheckReal(tmpval, strval, "Flattening Coefficient", "Real Number >= 0",
            false, true, true, true))
         realsOK = false;
      else 
      {
         flat = tmpval;
      }
   }
   if (!realsOK)
   {
      std::string errmsg = "Please enter valid Real values before saving data.\n";
      MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
   }

   if (textureChanged)
   {
      strval = textureTextCtrl->GetValue();
      #ifdef DEBUG_CB_PROP_PANEL
         MessageInterface::ShowMessage("textureChanged is true : %s\n",
               strval.c_str());       
      #endif
      std::ifstream filename(strval.c_str());
      
      if (!filename)
      {
         std::string errmsg = "File \"" + strval;
         errmsg += "\" does not exist.\n";
         MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
         stringsOK = false;
      }
      else
      {
         textureMap = strval;
         filename.close();
      } 
   }

   if (realsOK && stringsOK)
   {
      #ifdef DEBUG_CB_PROP_PANEL
         MessageInterface::ShowMessage("Reals and Strings are OK - setting them\n");
         MessageInterface::ShowMessage(
               "mu = %12.4f, eqRad = %12.4f, flat = %12.4f, textureMap = %s\n",
               mu, eqRad, flat, textureMap.c_str());
         MessageInterface::ShowMessage("in Properties panel, body pointer is %p\n",
               theBody);
      #endif
      theBody->SetGravitationalConstant(mu);
      theBody->SetEquatorialRadius(eqRad);
      theBody->SetFlattening(flat);
      theBody->SetStringParameter(theBody->GetParameterID("TextureMapFileName"),
                                  textureMap);
      dataChanged = false;
      ResetChangeFlags(true);
   }
   else
   {
      canClose = false;
   }
   #ifdef DEBUG_CB_PROP_SAVE

      MessageInterface::ShowMessage("At end of CBPropPanel::SaveData, canClose = %s\n",
         (canClose? "true" : "false"));
   #endif
   
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the data from the body object to the panel widgets.
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyPropertiesPanel::LoadData()
{
   try
   {
      mu         = theBody->GetGravitationalConstant();
      muString   = guiManager->ToWxString(mu);
      muTextCtrl->SetValue(muString);
      
      eqRad       = theBody->GetEquatorialRadius();
      eqRadString = guiManager->ToWxString(eqRad);
      eqRadTextCtrl->SetValue(eqRadString);
      
      flat       = theBody->GetFlattening();
      flatString = guiManager->ToWxString(flat);
      flatTextCtrl->SetValue(flatString);
      
      textureMap = theBody->GetStringParameter(theBody->GetParameterID("TextureMapFileName"));
      textureTextCtrl->SetValue(textureMap.c_str());
      
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

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates and arranges the widgets on the panel.
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyPropertiesPanel::Create()
{
   int bSize     = 2;
   #if __WXMAC__
   int buttonWidth = 40;
   #else
   int buttonWidth = 25;
   #endif

   wxBitmap openBitmap = wxBitmap(OpenFolder_xpm);

   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Celestial Body Properties"));

   // empty the temporary value strings
   muString      = "";
   eqRadString   = "";
   flatString    = "";
   textureString = "";
   
   // mu
   muStaticText      = new wxStaticText(this, ID_TEXT, wxT(GUI_ACCEL_KEY"Mu"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   muTextCtrl        = new wxTextCtrl(this, ID_TEXT_CTRL_MU, wxT(""),
                       wxDefaultPosition, wxSize(150, -1),0,wxTextValidator(wxGMAT_FILTER_NUMERIC));
   muTextCtrl->SetToolTip(pConfig->Read(_T("MuHint")));
   muUnitsStaticText = new wxStaticText(this, ID_TEXT, wxT("km^3/sec^2"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   // eq. radius
   eqRadStaticText      = new wxStaticText(this, ID_TEXT, wxT("Equatorial "GUI_ACCEL_KEY"Radius"),
                          wxDefaultPosition, wxSize(-1,-1), 0);
   eqRadTextCtrl        = new wxTextCtrl(this, ID_TEXT_CTRL_EQRAD, wxT(""),
                          wxDefaultPosition, wxSize(150, -1),0,wxTextValidator(wxGMAT_FILTER_NUMERIC));
   eqRadTextCtrl->SetToolTip(pConfig->Read(_T("EquatorialRadiusHint")));
   eqRadUnitsStaticText = new wxStaticText(this, ID_TEXT, wxT("km"),
                          wxDefaultPosition, wxSize(-1,-1), 0);
   // flattening
   flatStaticText      = new wxStaticText(this, ID_TEXT, wxT(GUI_ACCEL_KEY"Flattening"),
                         wxDefaultPosition, wxSize(-1,-1), 0);
   flatTextCtrl        = new wxTextCtrl(this, ID_TEXT_CTRL_FLAT, wxT(""),
                         wxDefaultPosition, wxSize(150, -1),0,wxTextValidator(wxGMAT_FILTER_NUMERIC));
   flatTextCtrl->SetToolTip(pConfig->Read(_T("FlatteningHint")));
   flatUnitsStaticText = new wxStaticText(this, ID_TEXT, wxT(""), // unitless
                         wxDefaultPosition, wxSize(-1,-1), 0);
   // texture map
   textureStaticText = new wxStaticText(this, ID_TEXT, wxT("Te"GUI_ACCEL_KEY"xture Map File"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   textureTextCtrl   = new wxTextCtrl(this, ID_TEXT_CTRL_TEXTURE, wxT(""),
                       wxDefaultPosition, wxSize(300,-1), 0);
   textureTextCtrl->SetToolTip(pConfig->Read(_T("TextureMapFileHint")));
   browseButton      = new wxBitmapButton(this, ID_BUTTON_BROWSE, 
                       openBitmap, wxDefaultPosition,
                       wxSize(buttonWidth, 20));
   browseButton->SetToolTip(pConfig->Read(_T("BrowseTextureMapFileHint"), "Browse for file"));
   
   // set the min width for one of the labels for each GmatStaticBoxSizer
   int minLabelSize = muStaticText->GetBestSize().x;
   minLabelSize = (minLabelSize < eqRadStaticText->GetBestSize().x) ? eqRadStaticText->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < flatStaticText->GetBestSize().x) ? flatStaticText->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < textureStaticText->GetBestSize().x) ? textureStaticText->GetBestSize().x : minLabelSize;

   eqRadStaticText->SetMinSize(wxSize(minLabelSize, eqRadStaticText->GetMinHeight()));
   textureStaticText->SetMinSize(wxSize(minLabelSize, textureStaticText->GetMinHeight()));
   
   wxFlexGridSizer *cbPropGridSizer = new wxFlexGridSizer(3,0,0);
   cbPropGridSizer->Add(muStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbPropGridSizer->Add(muTextCtrl,0, wxALIGN_LEFT|wxALL, bSize);
   cbPropGridSizer->Add(muUnitsStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbPropGridSizer->Add(eqRadStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbPropGridSizer->Add(eqRadTextCtrl,0, wxALIGN_LEFT|wxALL, bSize);
   cbPropGridSizer->Add(eqRadUnitsStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbPropGridSizer->Add(flatStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbPropGridSizer->Add(flatTextCtrl,0, wxALIGN_LEFT|wxALL, bSize);
   cbPropGridSizer->Add(flatUnitsStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbPropGridSizer->Add(20,20,0,wxALIGN_LEFT|wxALL, bSize);
   cbPropGridSizer->Add(20,20,0,wxALIGN_LEFT|wxALL, bSize);
   cbPropGridSizer->Add(20,20,0,wxALIGN_LEFT|wxALL, bSize);

   wxFlexGridSizer *cbPropGridSizer2 = new wxFlexGridSizer(3,0,0);
   cbPropGridSizer2->AddGrowableCol(1);
   cbPropGridSizer2->Add(textureStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbPropGridSizer2->Add(textureTextCtrl,0, wxALIGN_LEFT|wxGROW, bSize);
   cbPropGridSizer2->Add(browseButton,0, wxALIGN_CENTRE|wxALL, bSize);
   GmatStaticBoxSizer *optionsSizer    = new GmatStaticBoxSizer(wxVERTICAL, this, "Options");
   optionsSizer->Add(cbPropGridSizer, 0, wxALIGN_LEFT|wxALL, bSize); 
   optionsSizer->Add(cbPropGridSizer2, 0, wxALIGN_LEFT|wxGROW, bSize); 
   
   pageSizer    = new GmatStaticBoxSizer(wxVERTICAL, this, "");
   
   pageSizer->Add(optionsSizer, 1, wxALIGN_LEFT|wxGROW, bSize); 
   
   this->SetAutoLayout(true);
   this->SetSizer(pageSizer);
   pageSizer->Fit(this);
}

//------------------------------------------------------------------------------
// void ResetChangeFlags(bool discardMods)
//------------------------------------------------------------------------------
/**
 * Resets the change flags for the panel.
 *
 * @param <discardMods>    flag indicating whether or not to discard the
 *                         modifications on the widgets
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyPropertiesPanel::ResetChangeFlags(bool discardMods)
{
   muChanged      = false;
   eqRadChanged   = false;
   flatChanged    = false;
   textureChanged = false;
   if (discardMods)
   {
      muTextCtrl->DiscardEdits();
      eqRadTextCtrl->DiscardEdits();
      flatTextCtrl->DiscardEdits();
      textureTextCtrl->DiscardEdits();
   }
   dataChanged = false;
}

//------------------------------------------------------------------------------
//Event Handling
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void OnMuTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the mu text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyPropertiesPanel::OnMuTextCtrlChange(wxCommandEvent &event)
{
   if (muTextCtrl->IsModified())
   {
      muChanged   = true;
      dataChanged = true;
      theCBPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnEqRadTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the equatorial radius
 * text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyPropertiesPanel::OnEqRadTextCtrlChange(wxCommandEvent &event)
{
   if (eqRadTextCtrl->IsModified())
   {
      eqRadChanged   = true;
      dataChanged    = true;
      theCBPanel->EnableUpdate(true);
   }

}

//------------------------------------------------------------------------------
// void OnFlatTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the flattening coefficient
 * text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyPropertiesPanel::OnFlatTextCtrlChange(wxCommandEvent &event)
{
   if (flatTextCtrl->IsModified())
   {
      flatChanged   = true;
      dataChanged   = true;
      theCBPanel->EnableUpdate(true);
   }

}

//------------------------------------------------------------------------------
// void OnTextureTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the texture filename
 * text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyPropertiesPanel::OnTextureTextCtrlChange(wxCommandEvent &event)
{
   if (textureTextCtrl->IsModified())
   {
      textureChanged  = true;
      dataChanged     = true;
      theCBPanel->EnableUpdate(true);
   }

}


//------------------------------------------------------------------------------
// void OnBrowseButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user selects the browse button.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyPropertiesPanel::OnBrowseButton(wxCommandEvent &event)
{
   wxString oldTexture = textureTextCtrl->GetValue();
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString fileName = (dialog.GetPath()).c_str();
      if (!fileName.IsSameAs(oldTexture))
      {
         textureTextCtrl->SetValue(fileName);
         textureChanged = true;
         theCBPanel->EnableUpdate(true);
         dataChanged = true;
      }
   }
}

//------------------------------------------------------------------------------
// wxString ToString(Real rval)
//------------------------------------------------------------------------------
/**
 * Converts the Real input value to a wxString.
 *
 * @param <rval>    real value
 *
 * @return   the wxString representation of the input real value
 *
 */
//------------------------------------------------------------------------------
wxString CelestialBodyPropertiesPanel::ToString(Real rval)
{
   return guiManager->ToWxString(rval);
}
