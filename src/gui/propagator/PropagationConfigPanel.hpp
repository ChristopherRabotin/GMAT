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

#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/button.h>

#include "gmatwxdefs.hpp"
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "Propagator.hpp"
#include "Integrator.hpp"
#include "RungeKutta89.hpp"
#include "PhysicalModel.hpp"

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

    wxFlexGridSizer *integratorGridSizer;
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
    wxStaticText *setting1StaticText;
    wxStaticText *setting2StaticText;
    wxStaticText *setting3StaticText;
    wxStaticText *setting4StaticText;
    wxStaticText *setting5StaticText;
    wxStaticText *setting6StaticText;
    wxStaticText *setting7StaticText;
    wxStaticText *setting8StaticText;
    wxStaticText *setting9StaticText;
    wxStaticText *item38;
    wxStaticText *item40;
    wxStaticText *item42;
    wxStaticText *item47;
    wxStaticText *item52;
    wxStaticText *item54;
    wxStaticText *item56;
                
    wxTextCtrl *item3;
    wxTextCtrl *setting1TextCtrl;
    wxTextCtrl *setting2TextCtrl;
    wxTextCtrl *setting3TextCtrl;
    wxTextCtrl *setting4TextCtrl;
    wxTextCtrl *setting5TextCtrl;
    wxTextCtrl *setting6TextCtrl;
    wxTextCtrl *setting7TextCtrl;
    wxTextCtrl *setting8TextCtrl;
    wxTextCtrl *setting9TextCtrl;
    wxTextCtrl *item34;
    wxTextCtrl *item39;
    wxTextCtrl *item41;
    wxTextCtrl *item43;
    wxTextCtrl *item53;
    wxTextCtrl *item55;
    wxTextCtrl *item57;
    wxTextCtrl *item62;

    wxComboBox *integratorTypeComboBox;
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
    
    GuiInterpreter *theGuiInterpreter;
    Propagator *thePropagator;
    Integrator *theIntegrator;
    //RungeKutta89 *theRK89;
    PhysicalModel *thePhysicalModel;
   
    void OnButton(wxCommandEvent& event);
    void OnOK();
    void OnApply();
    void OnCancel();
    void Setup(wxWindow *parent);
    void GetData();
    void SetData();

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();
    
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
