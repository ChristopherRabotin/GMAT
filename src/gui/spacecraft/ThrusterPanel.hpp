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

#include "gmatdefs.hpp"
#include "Spacecraft.hpp"
#include "Thruster.hpp"

class ThrusterPanel: public wxPanel
{
public:
    ThrusterPanel(wxWindow *parent, Spacecraft *spacecraft,
                  wxButton *theApplyButton);
    ~ThrusterPanel();
    void SaveData();
    void LoadData();
    
private:    
    void Create();
    void DisplayData();
    
    // Event Handling
    DECLARE_EVENT_TABLE();
    void OnSelect(wxCommandEvent &event);
    void OnButtonClick(wxCommandEvent &event);
    
    Spacecraft *theSpacecraft;
    GuiInterpreter *theGuiInterpreter;
    
    wxArrayString availableThrusterArray;
    wxArrayString selectedThrusterArray;
        
    Integer availableThrusterCount;
    Integer selectedThrusterCount;
    
    Integer currentAvailThruster;
    Integer currentSelectedThruster;
    
    wxButton *theApplyButton;
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

