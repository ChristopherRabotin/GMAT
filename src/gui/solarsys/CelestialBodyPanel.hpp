//$Id$
//------------------------------------------------------------------------------
//                              CelestialBodyPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy C. Shoan (original author: Allison Greene 2005.07.06)
// Created: 2009.01.26
//
/**
 * Declares CelestialBodyPanel class.  This panel contains a notebook with
 * three tabs for Properties, Orbit, and Orientation.
 */
//------------------------------------------------------------------------------

#ifndef CelestialBodyPanel_hpp
#define CelestialBodyPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "CelestialBody.hpp"
#include "CelestialBodyPropertiesPanel.hpp"
#include "CelestialBodyOrbitPanel.hpp"
#include "CelestialBodyOrientationPanel.hpp"
#include "GuiInterpreter.hpp"
#include "gmatdefs.hpp"

class CelestialBodyPanel: public GmatPanel
{
public:
   
   // constructors
   CelestialBodyPanel(wxWindow *parent, const wxString &name);
   virtual ~CelestialBodyPanel();
    
private:
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   void OnPageChange(wxCommandEvent &event);
    
   CelestialBody *theCelestialBody;
   CelestialBody *origCelestialBody;
   //Planet *thePlanet;
   wxString      bodyName;
   bool          isUserDefined;
   
   wxNotebook    *cbNotebook;
   
   CelestialBodyPropertiesPanel  *properties;
   CelestialBodyOrbitPanel       *orbit;
   CelestialBodyOrientationPanel *orientation;
   
   
   void EnableAll(bool enable);

   // event handling
   
   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9000,
      ID_NOTEBOOK,
      ID_BUTTON_OK,
      ID_BUTTON_APPLY,
      ID_BUTTON_CANCEL,
      ID_BUTTON_HELP,
   };
   
};

#endif
