//$Header$
//------------------------------------------------------------------------------
//                           PropagationConfigPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/08/29
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
//
/**
 * This class contains the Propagation Configuration window.
 */
//------------------------------------------------------------------------------

#ifndef PropagationConfigPanel_hpp
#define PropagationConfigPanel_hpp

#include "gmatwxdefs.hpp"

#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/button.h>

//loj: to remove warnings about duplicate defines
//  #define ID_TEXT     10000
//  #define ID_TEXTCTRL 10001
//  #define ID_COMBO    10002
//  #define ID_BUTTON   10003
//  #define ID_CHECKBOX 10004

class PropagationConfigPanel : public wxPanel
{
public:
    // constructors
    PropagationConfigPanel(wxWindow *parent);
   
private:
    wxBoxSizer *item0;
    wxBoxSizer *item1;
    wxBoxSizer *item4;
    wxBoxSizer *item32;
    wxBoxSizer *item68;

    wxStaticBoxSizer *item5;
    wxStaticBoxSizer *item28;
    wxStaticBoxSizer *item30;
    wxStaticBoxSizer *item45;
    wxStaticBoxSizer *item59;
    wxStaticBoxSizer *item64;

    wxFlexGridSizer *item7;
    wxFlexGridSizer *item61;

    wxStaticBox *item6;
    wxStaticBox *item29;
    wxStaticBox *item31;
    wxStaticBox *item37;
    wxStaticBox *item46;
    wxStaticBox *item51;
    wxStaticBox *item60;
    wxStaticBox *item65;
                
    wxStaticText *item2;
    wxStaticText *item8;
    wxStaticText *item10;
    wxStaticText *item12;
    wxStaticText *item14;
    wxStaticText *item16;
    wxStaticText *item18;
    wxStaticText *item20;
    wxStaticText *item22;
    wxStaticText *item24;
    wxStaticText *item26;
    wxStaticText *item38;
    wxStaticText *item40;
    wxStaticText *item42;
    wxStaticText *item47;
    wxStaticText *item52;
    wxStaticText *item54;
    wxStaticText *item56;
                
    wxTextCtrl *item3;
    wxTextCtrl *item11;
    wxTextCtrl *item13;
    wxTextCtrl *item15;
    wxTextCtrl *item17;
    wxTextCtrl *item19;
    wxTextCtrl *item21;
    wxTextCtrl *item23;
    wxTextCtrl *item25;
    wxTextCtrl *item27;
    wxTextCtrl *item34;
    wxTextCtrl *item39;
    wxTextCtrl *item41;
    wxTextCtrl *item43;
    wxTextCtrl *item53;
    wxTextCtrl *item55;
    wxTextCtrl *item57;
    wxTextCtrl *item62;

    wxComboBox *item9;
    wxComboBox *item33;
    wxComboBox *item48;

    wxCheckBox *item66;

    wxButton *item35;
    wxButton *item44;
    wxButton *item49;
    wxButton *item58;
    wxButton *item63;
    wxButton *item67;
    wxButton *item69;
    wxButton *item70;
    wxButton *item71;
   
    void CreateConfigWindow(wxWindow *parent);

    // any class wishing to process wxWindows events must use this macro
    //DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 10000,
        ID_TEXTCTRL,
        ID_COMBO,
        ID_BUTTON,
        ID_CHECKBOX
    };

};

#endif // PropagationConfigPanel_hpp
