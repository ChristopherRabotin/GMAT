//$Header: 
//------------------------------------------------------------------------------
//                              CoordSystemConfigPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/10/26
/**
 * This class contains the Coordinate System Setup Panel.
 */
//------------------------------------------------------------------------------
#ifndef CoordSystemConfigPanel_hpp
#define CoordSystemConfigPanel_hpp

// gui includes
#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"

// base includes
#include "gmatdefs.hpp"
#include "Command.hpp"
#include "For.hpp"

class CoordSystemConfigPanel : public GmatPanel
{
public:
    // constructors
    CoordSystemConfigPanel(wxWindow *parent);
    ~CoordSystemConfigPanel();  

private:
    wxStaticText *nameStaticText;
    wxStaticText *originStaticText;
    wxStaticText *typeStaticText;
    wxStaticText *primaryStaticText;
    wxStaticText *formatStaticText;
    wxStaticText *secondaryStaticText;
    wxStaticText *epochStaticText;
    
    wxTextCtrl *nameTextCtrl;
    
    wxComboBox *originComboBox;
    wxComboBox *typeComboBox;
    wxComboBox *primaryComboBox;
    wxComboBox *formatComboBox;
    wxComboBox *secondaryComboBox;
    wxComboBox *epochComboBox;
    
    // methods inherited from GmatPanel
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();

    // Layout & data handling methods
    void Setup(wxWindow *parent);
    
    void OnTextUpdate(wxCommandEvent& event); 
    void OnComboUpdate(wxCommandEvent& event);
    
    // event handling
    DECLARE_EVENT_TABLE();

    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXTCTRL = 46000,
        ID_COMBO,
        ID_TEXT,
    };
};

#endif // CoordSystemConfigPanel_hpp
