//$Id$
//------------------------------------------------------------------------------
//                            SpacecraftPanel
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
// Author: Monisha Butler
// Created: 2003/09/10
// Modified: 2003/09/29
/**
 * This class contains information needed to setup spacecraft through GUI
 * 
 */
//------------------------------------------------------------------------------
#ifndef SpacecraftPanel_hpp
#define SpacecraftPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "BallisticsMassPanel.hpp"
#include "OrbitPanel.hpp"
#include "TankPanel.hpp"
#include "AttitudePanel.hpp"
#include "ThrusterPanel.hpp"
#include "SpicePanel.hpp"
#include "GuiInterpreter.hpp"
#include "VisualModelPanel.hpp"

#include "gmatdefs.hpp"
#include "Spacecraft.hpp"
#include "FuelTank.hpp"

class SpacecraftPanel: public GmatPanel
{
public:
   SpacecraftPanel(wxWindow *parent, const wxString &scName);
   ~SpacecraftPanel();   
       
   virtual bool RefreshObjects(Gmat::ObjectType type = Gmat::UNKNOWN_OBJECT);

private:    
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();

   void OnPageChange(wxCommandEvent &event);

   Spacecraft *theSpacecraft;
   Spacecraft *currentSpacecraft;
   
   wxStaticText *description1;
   wxStaticText *description2;
   wxStaticText *description3;
   wxStaticText *description4;
   wxStaticText *description5;
   wxStaticText *description6;
   
   wxTextCtrl *textCtrl1;
   wxTextCtrl *textCtrl2;
   wxTextCtrl *textCtrl3;
   wxTextCtrl *textCtrl4;
   wxTextCtrl *textCtrl5;
   wxTextCtrl *textCtrl6;
   
   wxStaticText *label1;
   wxStaticText *label2;
   wxStaticText *label3;
   wxStaticText *label4;
   wxStaticText *label5;
   wxStaticText *label6;
   
   wxNotebook *spacecraftNotebook;
   wxNotebook *actuatorNotebook;

   wxPanel *actuators;
   wxPanel *sensors;
   
   BallisticsMassPanel *theBallisticMassPanel;
   OrbitPanel *theOrbitPanel;
   TankPanel *theTankPanel;
   ThrusterPanel *theThrusterPanel;
   AttitudePanel *theAttitudePanel;
   VisualModelPanel *theVisualModelPanel;
   SpicePanel    *theSpicePanel;
   
   // IDs for the controls and the menu commands
   enum
   {
      ID_TEXT = 30000,
      ID_COMBO,
      ID_TEXTCTRL,
      ID_NOTEBOOK,
      ID_BUTTON,
      ID_ELEMENT_TAG1,
      ID_ELEMENT_TAG2,
      ID_ELEMENT_TAG3,
      ID_ELEMENT_TAG4,
      ID_ELEMENT_TAG5,
      ID_ELEMENT_TAG6,
      
      ID_ELEMENT_VALUE1,
      ID_ELEMENT_VALUE2,
      ID_ELEMENT_VALUE3,
      ID_ELEMENT_VALUE4,
      ID_ELEMENT_VALUE5,
      ID_ELEMENT_VALUE6,
      
      ID_STATIC_COORD,
      ID_STATIC_ORBIT,
      ID_STATIC_ELEMENT,
      
      ID_CB_BODY,
      ID_CB_FRAME,
      ID_CB_EPOCH,
      ID_CB_STATE,
      
      ID_EPOCH_VALUE,
      
      ID_BUTTON_OK,
      ID_BUTTON_APPLY,
      ID_BUTTON_CANCEL,
      ID_BUTTON_HELP
   };
};
#endif

