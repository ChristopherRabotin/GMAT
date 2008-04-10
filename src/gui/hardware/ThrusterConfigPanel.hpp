//$Header$
//------------------------------------------------------------------------------
//                            ThrusterConfigPanel
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
#ifndef ThrusterConfigPanel_hpp
#define ThrusterConfigPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

#include "gmatdefs.hpp"
#include "Spacecraft.hpp"
#include "Thruster.hpp"

class ThrusterConfigPanel: public GmatPanel
{
public:
    ThrusterConfigPanel(wxWindow *parent, const wxString &name);
    ~ThrusterConfigPanel();
    
private:
    // methods inherited from GmatPanel
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
    
    // Event Handling
    DECLARE_EVENT_TABLE();
    void OnTextChange(wxCommandEvent &event);
    void OnComboBoxChange(wxCommandEvent &event);
    void OnButtonClick(wxCommandEvent &event);
    
    std::string thrusterName;
    std::string coordsysName;
    std::string tankName;
    
    Integer tankSize;
    
    Thruster* theThruster;
   
    bool isCoordSysChanged;
    bool isTankChanged;
    bool isTankEmpty;
    
    wxButton *cCoefButton;
    wxButton *kCoefButton;
    
    wxComboBox *coordsysComboBox;
    wxComboBox *tankComboBox;
    
    wxTextCtrl *XTextCtrl;
    wxTextCtrl *YTextCtrl;
    wxTextCtrl *ZTextCtrl;
    wxTextCtrl *scaleFactorTextCtrl;
    
    wxStaticText *coordsysStaticText;
    wxStaticText *tankStaticText;
    wxStaticText *XStaticText;
    wxStaticText *YStaticText;
    wxStaticText *ZStaticText;
    wxStaticText *scaleFactorStaticText;
        
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 30250,
        ID_TEXTCTRL,
        ID_LISTBOX,
        ID_BUTTON,
        ID_COMBOBOX,
    };
};
#endif

