//$Header:
//------------------------------------------------------------------------------
//                            TankPanel
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
// Created: 2004/11/19
/**
 * This class contains information needed to setup users spacecraft tank 
 * parameters.
 */
//------------------------------------------------------------------------------
#ifndef TankPanel_hpp
#define TankPanel_hpp

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"

#include "gmatdefs.hpp"
#include "Spacecraft.hpp"
#include "FuelTank.hpp"

class TankPanel: public wxPanel
{
public:
    TankPanel(wxWindow *parent, Spacecraft *spacecraft,
              wxButton *theApplyButton);
    ~TankPanel();
    void SaveData();
    void LoadData();
    
private:
    
    void Create();
    
    // Event Handling
    DECLARE_EVENT_TABLE();
    void OnTextChange();
    
    Spacecraft *theSpacecraft;
    FuelTank *theFuelTank;
    
    Integer tankCount;
    
    wxButton *theApplyButton;
    wxButton *addButton;
    wxButton *removeButton;

    wxTextCtrl *temperatureTextCtrl;
    wxTextCtrl *refTemperatureTextCtrl;
    wxTextCtrl *fuelMassTextCtrl;
    wxTextCtrl *fuelDensityTextCtrl;
    wxTextCtrl *pressureTextCtrl;
    wxTextCtrl *volumeTextCtrl;
    
    wxStaticText *temperatureStaticText;
    wxStaticText *refTemperatureStaticText;
    wxStaticText *fuelMassStaticText;
    wxStaticText *fuelDensityStaticText;
    wxStaticText *pressureStaticText;
    wxStaticText *volumeStaticText;
    wxStaticText *unit1StaticText;
    wxStaticText *unit2StaticText;
    wxStaticText *unit3StaticText;
    wxStaticText *unit4StaticText;
    wxStaticText *unit5StaticText;
    wxStaticText *unit6StaticText;
    
    wxListBox *tankListBox;
        
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 30200,
        ID_TEXTCTRL,
        ID_LISTBOX,
        ID_BUTTON,
    };
};
#endif

