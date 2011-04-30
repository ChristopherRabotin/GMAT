//$Id$
//------------------------------------------------------------------------------
//                            BallisticsMassPanel
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
// Author: Allison Greene
// Created: 2004/04/01
/**
 * This class contains information needed to setup users spacecraft ballistics
 * and mass through GUI
 * 
 */
//------------------------------------------------------------------------------
#ifndef BallisticsMassPanel_hpp
#define BallisticsMassPanel_hpp

#include "gmatwxdefs.hpp"
#include "Spacecraft.hpp"
#include "GmatPanel.hpp"


class BallisticsMassPanel: public wxPanel
{
public:
   BallisticsMassPanel(GmatPanel *scPanel, wxWindow *parent, Spacecraft *spacecraft);
   ~BallisticsMassPanel();
   
   void SaveData();
   void LoadData();
   
   bool IsDataChanged() { return dataChanged; }
   bool CanClosePanel() { return canClose; }
   
private:
   bool dataChanged;
   bool canClose;
   
   void Create();
    
   // Event Handling
   DECLARE_EVENT_TABLE();
   void OnTextChange(wxCommandEvent &event);
    
   Spacecraft *theSpacecraft;

   GmatPanel *theScPanel;
   
   wxTextCtrl *dryMassTextCtrl;
   wxTextCtrl *dragCoeffTextCtrl;
   wxTextCtrl *reflectCoeffTextCtrl;
   wxTextCtrl *dragAreaTextCtrl;
   wxTextCtrl *srpAreaTextCtrl;

   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 30100,
      ID_TEXTCTRL,
   };
};
#endif

