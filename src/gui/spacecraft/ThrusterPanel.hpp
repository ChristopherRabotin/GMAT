//$Header$
//------------------------------------------------------------------------------
//                            ThrusterPanel
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
// Author: Waka Waktola
// Created: 2005/01/06
/**
 * This class contains information needed to setup users spacecraft thruster 
 * parameters.
 */
//------------------------------------------------------------------------------
#ifndef ThrusterPanel_hpp
#define ThrusterPanel_hpp

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

#include "gmatdefs.hpp"
#include "Spacecraft.hpp"
#include "Thruster.hpp"

class ThrusterPanel: public wxPanel
{
public:
   ThrusterPanel(wxWindow *parent, Spacecraft *spacecraft,
                 wxButton *applyButton, wxButton *okButton);
   ~ThrusterPanel();
   void SaveData();
   void LoadData();
    
   bool IsDataChanged() { return dataChanged; }
   
private:
   
   bool dataChanged;
   
   void Create();
    
   // Event Handling
   DECLARE_EVENT_TABLE();
   void OnButtonClick(wxCommandEvent &event);
    
   Spacecraft *theSpacecraft;
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
    
   wxArrayString mExcludedThrusterList;
    
   wxButton *theApplyButton;
   wxButton *theOkButton;
   wxButton *selectButton;
   wxButton *removeButton;
   wxButton *selectAllButton;
   wxButton *removeAllButton;
    
   wxListBox *availableThrusterListBox;
   wxListBox *selectedThrusterListBox;
        
   // IDs for the controls and the menu commands
   enum
   {     
      ID_LISTBOX = 30200,
      ID_BUTTON,
   };
};
#endif

