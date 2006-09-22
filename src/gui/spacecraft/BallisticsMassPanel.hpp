//$Header$
//------------------------------------------------------------------------------
//                            BallisticsMassPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
//
// **Legal**
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
#include "GuiInterpreter.hpp"
#include "Spacecraft.hpp"

class BallisticsMassPanel: public wxPanel
{
public:
   BallisticsMassPanel(wxWindow *parent,
                       Spacecraft *spacecraft,
                       wxButton *theApplyButton);
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
   wxButton *theApplyButton;

   wxTextCtrl *dryMassTextCtrl;
   wxTextCtrl *dragCoeffTextCtrl;
   wxTextCtrl *reflectCoeffTextCtrl;
   wxTextCtrl *dragAreaTextCtrl;
   wxTextCtrl *srpAreaTextCtrl;

   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 30100,
      ID_TEXT_CTRL,
      ID_TEXTCTRL,
   };
};
#endif

