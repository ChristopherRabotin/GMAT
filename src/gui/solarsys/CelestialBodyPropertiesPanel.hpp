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
// Created: 2009.01.13
//
/**
 * This is the panel for the Properties tab on the notebook on the CelestialBody
 * Panel.
 *
 */
//------------------------------------------------------------------------------


#ifndef CelestialBodyPropertiesPanel_hpp
#define CelestialBodyPropertiesPanel_hpp

#include "gmatdefs.hpp"
#include "CelestialBody.hpp"
#include "gmatwxdefs.hpp"
#include "GuiItemManager.hpp"
#include "GmatPanel.hpp"
#include "GmatStaticBoxSizer.hpp"

class CelestialBodyPropertiesPanel : public wxPanel
{
public:
   CelestialBodyPropertiesPanel(GmatPanel *cbPanel, wxWindow *parent, CelestialBody *body);
   ~CelestialBodyPropertiesPanel();
   
   void SaveData();
   void LoadData();
   
   bool IsDataChanged() { return dataChanged;};
   bool CanClosePanel() { return canClose;};
   
private:
   
   bool           dataChanged;
   bool           canClose;
   
   CelestialBody  *theBody;
   GuiItemManager *guiManager;
   
   Real           mu;
   Real           eqRad;
   Real           flat;
   std::string    textureMap;
   
   bool           muChanged;
   bool           eqRadChanged;
   bool           flatChanged;
   bool           textureChanged;
   
   GmatPanel      *theCBPanel;
   
  
   void     Create();
   void     ResetChangeFlags(bool discardMods = false);
   
   //Event Handling
   DECLARE_EVENT_TABLE();
   void     OnMuTextCtrlChange(wxCommandEvent &event);
   void     OnEqRadTextCtrlChange(wxCommandEvent &event);
   void     OnFlatTextCtrlChange(wxCommandEvent &event);
   void     OnTextureTextCtrlChange(wxCommandEvent &event);
   void     OnBrowseButton(wxCommandEvent &event);
   
   wxString ToString(Real rval);
   
   // wx
   
   wxStaticText *muStaticText;
   wxStaticText *eqRadStaticText;
   wxStaticText *flatStaticText;
   wxStaticText *textureStaticText;

   wxStaticText *muUnitsStaticText;
   wxStaticText *eqRadUnitsStaticText;
   wxStaticText *flatUnitsStaticText;

   wxTextCtrl   *muTextCtrl;
   wxTextCtrl   *eqRadTextCtrl;
   wxTextCtrl   *flatTextCtrl;
   wxTextCtrl   *textureTextCtrl;
   
   wxBitmapButton *browseButton;
   
   /// string versions of current data
   wxString     muString;
   wxString     eqRadString;
   wxString     flatString;
   wxString     textureString;
   
   GmatStaticBoxSizer *pageSizer;
   
   /// IDs for the controls 
   enum
   {
      ID_TEXT = 7100,
      ID_BUTTON_BROWSE,
      ID_TEXT_CTRL_MU,
      ID_TEXT_CTRL_EQRAD,
      ID_TEXT_CTRL_FLAT,
      ID_TEXT_CTRL_TEXTURE,
   };
};
#endif // CelestialBodyPropertiesPanel_hpp
