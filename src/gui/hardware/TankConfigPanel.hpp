//$Header:
//------------------------------------------------------------------------------
//                            TankConfigPanel
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
#ifndef TankConfigPanel_hpp
#define TankConfigPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"

#include "gmatdefs.hpp"
#include "FuelTank.hpp"

class TankConfigPanel: public GmatPanel
{
public:
    TankConfigPanel(wxWindow *parent, const wxString &tankName);
    ~TankConfigPanel();
    
private: 
    // methods inherited from GmatPanel
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
    
    // Event Handling
    DECLARE_EVENT_TABLE();
    void DisplayData();
    void OnTextChange();
    
    FuelTank* theFuelTank;
    
    Integer tankCount;
    Integer currentTank;

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

