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
#ifndef OrbitPanel_hpp
#define OrbitPanel_hpp

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"
#include "Spacecraft.hpp"

class OrbitPanel: public wxPanel
{
public:
    OrbitPanel(wxWindow *parent, 
               Spacecraft *spacecraft, 
               wxButton *theApplyButton);
    ~OrbitPanel();
    void SaveData();
    void LoadData();
   
private:

    void Create();
    void AddElements(wxWindow *parent);
    
    // Event Handling
    DECLARE_EVENT_TABLE();
    void OnStateChange();
    void OnEpochChange();
    void OnTextChange();

    Spacecraft *theSpacecraft;
    wxButton *theApplyButton;

    wxStaticText *description1;
    wxStaticText *description2;
    wxStaticText *description3;
    wxStaticText *description4;
    wxStaticText *description5;
    wxStaticText *description6;
    
    wxTextCtrl *textCtrl1;
    wxTextCtrl *textCtrl2;
    wxTextCtrl *textCtrl3;
    wxTextCtrl *textCtrl4;
    wxTextCtrl *textCtrl5;
    wxTextCtrl *textCtrl6;
    
    wxStaticText *label1;
    wxStaticText *label2;
    wxStaticText *label3;
    wxStaticText *label4;
    wxStaticText *label5;
    wxStaticText *label6;
    
    wxPanel *elementsPanel;
    wxComboBox *stateComboBox;
    wxComboBox *dateComboBox;
    wxTextCtrl *epochValue;



    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 30200,
        ID_TEXT_CTRL,
        ID_TEXTCTRL,
        ID_COMBO,
        
        ID_ELEMENT_TAG1,
        ID_ELEMENT_TAG2,
        ID_ELEMENT_TAG3,
        ID_ELEMENT_TAG4,
        ID_ELEMENT_TAG5,
        ID_ELEMENT_TAG6,
        
        ID_ELEMENT_VALUE1,
        ID_ELEMENT_VALUE2,
        ID_ELEMENT_VALUE3,
        ID_ELEMENT_VALUE4,
        ID_ELEMENT_VALUE5,
        ID_ELEMENT_VALUE6,
        
        ID_STATIC_COORD,
        ID_STATIC_ORBIT,
        ID_STATIC_ELEMENT,
        
        ID_CB_BODY,
        ID_CB_FRAME,
        ID_CB_EPOCH,
        ID_CB_STATE,
        
        ID_EPOCH_VALUE,
    };
};
#endif

