//$Id$
//------------------------------------------------------------------------------
//                              ManeuverPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: LaMont Ruley
// Created: 2003/12/01
// Modification: 2004/10/20 Renamed from ManeuverSetupPanel
//
/**
 * This class contains the Maneuver Setup window.
 */
//------------------------------------------------------------------------------

#ifndef ManeuverPanel_hpp
#define ManeuverPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"
#include "GmatCommand.hpp"
#include "GuiItemManager.hpp"

class ManeuverPanel : public GmatPanel
{
public:
   // constructors
   ManeuverPanel(wxWindow *parent, GmatCommand *cmd);
   ~ManeuverPanel();
   
protected:
   // member data
   GmatCommand *theCommand;

   wxComboBox *burnCB;
   wxComboBox *satCB;
    
   // member functions
   void OnBurnComboBoxChange(wxCommandEvent& event);
   void OnSatComboBoxChange(wxCommandEvent& event);

   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();

   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
    
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 80000,
        
      ID_BURN_COMBOBOX,
      ID_SAT_COMBOBOX
   };
};

#endif // ManeuverPanel_hpp
