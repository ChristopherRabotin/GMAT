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
    struct Tank
    {
        std::string tankName;
        Real temperature;
        Real refTemperature;
        Real fuelMass;
        Real fuelDensity;
        Real pressure;
        Real volume;
        bool pressureRegulated;
        FuelTank* fuelTank;
      
        Tank(FuelTank* ft, const std::string &name)
        {
            fuelTank = ft;
            tankName = name;
        }
    };    
    
    void Create();
    
    // Event Handling
    DECLARE_EVENT_TABLE();
    void DisplayData();
    void OnTextChange();
    void OnSelect();
    void OnButtonClick(wxCommandEvent &event);
    
    Spacecraft *theSpacecraft;
    std::vector<Tank *> theTanks;
    ObjectArray theFuelTanks;
    StringArray fuelTankNames;
    
    Integer tankCount;
    Integer currentTank;
    
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

