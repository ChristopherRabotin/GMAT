//$Header$
//------------------------------------------------------------------------------
//                              PropagationConfigPanel
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

#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/button.h>

#include "PropagationConfigPanel.hpp"
#include "gmatwxdefs.hpp"
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "Propagator.hpp"
#include "Integrator.hpp"
#include "RungeKutta89.hpp"
#include "PhysicalModel.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(PropagationConfigPanel, wxPanel)
    EVT_BUTTON(-1, PropagationConfigPanel::OnButton)
    EVT_BUTTON(-1, PropagationConfigPanel::OnOK)
    EVT_BUTTON(-1, PropagationConfigPanel::OnApply)
    EVT_BUTTON(-1, PropagationConfigPanel::OnCancel)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// PropagationConfigPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
PropagationConfigPanel::PropagationConfigPanel(wxWindow *parent)
    :wxPanel(parent)
{
    Setup(this);
}

void PropagationConfigPanel::Setup(wxWindow *parent)
{
    StringArray propList;
    
    theGuiInterpreter = GmatAppData::GetGuiInterpreter();  
     
    propList = theGuiInterpreter->GetListOfConfiguredItems(Gmat::PROPAGATOR);
    
    if (propList.empty())
    {
        thePropagator = theGuiInterpreter->CreatePropagator("Propagator", "RungeKutta89");
        propList = theGuiInterpreter->GetListOfConfiguredItems(Gmat::PROPAGATOR);
    }
    else
        thePropagator = theGuiInterpreter->GetPropagator(propList[0]);
        
    //theRK89 = new RungeKutta89( "RungeKutta89" );
    //theIntegrator = new Integrator( thePropagator->GetName(), "RungeKutta89" );
    //thePhysicalModel = 
       
    item0 = new wxBoxSizer( wxVERTICAL );
    item1 = new wxBoxSizer( wxHORIZONTAL );

    item2 = new wxStaticText( parent, ID_TEXT, wxT("Propagation Name"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_CENTRE|wxALL, 5 );

    item3 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT("Lunar Tran"), wxDefaultPosition, wxSize(250,-1), 0 );
        
    item1->Add( item3, 0, wxALIGN_CENTRE|wxALL, 5 );
        
    item0->Add( item1, 0, wxALIGN_CENTRE|wxALL, 5 );

    item4 = new wxBoxSizer( wxHORIZONTAL );

    item6 = new wxStaticBox( parent, -1, wxT("Numerical Integrator") );
    item5 = new wxStaticBoxSizer( item6, wxVERTICAL );

    integratorGridSizer = new wxFlexGridSizer( 2, 0, 0 );

    item8 = new wxStaticText( parent, ID_TEXT, wxT("Integrator Type"), wxDefaultPosition, wxDefaultSize, 0 );
    integratorGridSizer->Add( item8, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxString integratorList[] = 
    {
        wxT("RK 8(9)")
    };
        
    integratorTypeComboBox = new wxComboBox( parent, ID_COMBO, wxT("RK 8(9)"), wxDefaultPosition, wxSize(100,-1), 1, integratorList, wxCB_DROPDOWN|wxCB_READONLY );
    integratorGridSizer->Add( integratorTypeComboBox, 0, wxALIGN_CENTRE|wxALL, 5 );

    setting1StaticText = new wxStaticText( parent, ID_TEXT, wxT("Setting 1"), wxDefaultPosition, wxDefaultSize, 0 );
    integratorGridSizer->Add( setting1StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );

    setting1TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    integratorGridSizer->Add( setting1TextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );

    setting2StaticText = new wxStaticText( parent, ID_TEXT, wxT("Setting 2"), wxDefaultPosition, wxDefaultSize, 0 );
    integratorGridSizer->Add( setting2StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );

    setting2TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    integratorGridSizer->Add( setting2TextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );

    setting3StaticText = new wxStaticText( parent, ID_TEXT, wxT("Setting 3"), wxDefaultPosition, wxDefaultSize, 0 );
    integratorGridSizer->Add( setting3StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );

    setting3TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    integratorGridSizer->Add( setting3TextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );

    setting4StaticText = new wxStaticText( parent, ID_TEXT, wxT("Setting 4"), wxDefaultPosition, wxDefaultSize, 0 );
    integratorGridSizer->Add( setting4StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );

    setting4TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    integratorGridSizer->Add( setting4TextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );

    setting5StaticText = new wxStaticText( parent, ID_TEXT, wxT("Setting 5"), wxDefaultPosition, wxDefaultSize, 0 );
    integratorGridSizer->Add( setting5StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );

    setting5TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    integratorGridSizer->Add( setting5TextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );

    setting6StaticText = new wxStaticText( parent, ID_TEXT, wxT("Setting 6"), wxDefaultPosition, wxDefaultSize, 0 );
    integratorGridSizer->Add( setting6StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );

    setting6TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    integratorGridSizer->Add( setting6TextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );

    setting7StaticText = new wxStaticText( parent, ID_TEXT, wxT("Setting 7"), wxDefaultPosition, wxDefaultSize, 0 );
    integratorGridSizer->Add( setting7StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );

    setting7TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    integratorGridSizer->Add( setting7TextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );

    setting8StaticText = new wxStaticText( parent, ID_TEXT, wxT("Setting 8"), wxDefaultPosition, wxDefaultSize, 0 );
    integratorGridSizer->Add( setting8StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );

    setting8TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    integratorGridSizer->Add( setting8TextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );

    setting9StaticText = new wxStaticText( parent, ID_TEXT, wxT("Setting 9"), wxDefaultPosition, wxDefaultSize, 0 );
    integratorGridSizer->Add( setting9StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );

    setting9TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    integratorGridSizer->Add( setting9TextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );

    item5->Add( integratorGridSizer, 0, wxALIGN_CENTRE|wxALL, 5 );

    item4->Add( item5, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item29 = new wxStaticBox( parent, -1, wxT("Force Model") );
    item28 = new wxStaticBoxSizer( item29, wxVERTICAL );

    item31 = new wxStaticBox( parent, -1, wxT("Primary Bodies") );
    item30 = new wxStaticBoxSizer( item31, wxVERTICAL );

    item32 = new wxBoxSizer( wxHORIZONTAL );

    wxString strs33[] = 
    {
        wxT("Earth")
    };
        
    item33 = new wxComboBox( parent, ID_COMBO, wxT("Earth"), wxDefaultPosition, wxSize(100,-1), 1, strs33, wxCB_DROPDOWN|wxCB_READONLY );
    item32->Add( item33, 0, wxALIGN_CENTRE|wxALL, 5 );

    item34 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT("Earth"), wxDefaultPosition, wxSize(150,-1), 0 );
    item32->Add( item34, 0, wxALIGN_CENTRE|wxALL, 5 );

    item35 = new wxButton( parent, ID_BUTTON, wxT("Add Body"), wxDefaultPosition, wxDefaultSize, 0 );
    item32->Add( item35, 0, wxALIGN_CENTRE|wxALL, 5 );

    item30->Add( item32, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item37 = new wxStaticBox( parent, -1, wxT("Gravity Field") );
    wxStaticBoxSizer *item36 = new wxStaticBoxSizer( item37, wxHORIZONTAL );

    item38 = new wxStaticText( parent, ID_TEXT, wxT("Degree"), wxDefaultPosition, wxDefaultSize, 0 );
    item36->Add( item38, 0, wxALIGN_CENTRE|wxALL, 5 );

    item39 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(80,-1), 0 );
    item36->Add( item39, 0, wxALIGN_CENTRE|wxALL, 5 );

    item40 = new wxStaticText( parent, ID_TEXT, wxT("Order"), wxDefaultPosition, wxDefaultSize, 0 );
    item36->Add( item40, 0, wxALIGN_CENTRE|wxALL, 5 );

    item41 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(80,-1), 0 );
    item36->Add( item41, 0, wxALIGN_CENTRE|wxALL, 5 );

    item42 = new wxStaticText( parent, ID_TEXT, wxT("Source"), wxDefaultPosition, wxDefaultSize, 0 );
    item36->Add( item42, 0, wxALIGN_CENTRE|wxALL, 5 );

    item43 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item36->Add( item43, 0, wxALIGN_CENTRE|wxALL, 5 );

    item44 = new wxButton( parent, ID_BUTTON, wxT("Search"), wxDefaultPosition, wxDefaultSize, 0 );
    item36->Add( item44, 0, wxALIGN_CENTRE|wxALL, 5 );

    item30->Add( item36, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item46 = new wxStaticBox( parent, -1, wxT("Environment Model") );
    item45 = new wxStaticBoxSizer( item46, wxHORIZONTAL );

    item47 = new wxStaticText( parent, ID_TEXT, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
    item45->Add( item47, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxString strs48[] = 
    {
        wxT("")
    };
        
    item48 = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(150,-1), 1, strs48, wxCB_DROPDOWN|wxCB_READONLY );
    item45->Add( item48, 0, wxALIGN_CENTRE|wxALL, 5 );

    item49 = new wxButton( parent, ID_BUTTON, wxT("Setup"), wxDefaultPosition, wxDefaultSize, 0 );
    item45->Add( item49, 0, wxALIGN_CENTRE|wxALL, 5 );

    item30->Add( item45, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item51 = new wxStaticBox( parent, -1, wxT("Magnetic Field") );
    wxStaticBoxSizer *item50 = new wxStaticBoxSizer( item51, wxHORIZONTAL );

    item52 = new wxStaticText( parent, ID_TEXT, wxT("Degree"), wxDefaultPosition, wxDefaultSize, 0 );
    item50->Add( item52, 0, wxALIGN_CENTRE|wxALL, 5 );

    item53 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(80,-1), 0 );
    item50->Add( item53, 0, wxALIGN_CENTRE|wxALL, 5 );

    item54 = new wxStaticText( parent, ID_TEXT, wxT("Order"), wxDefaultPosition, wxDefaultSize, 0 );
    item50->Add( item54, 0, wxALIGN_CENTRE|wxALL, 5 );

    item55 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(80,-1), 0 );
    item50->Add( item55, 0, wxALIGN_CENTRE|wxALL, 5 );

    item56 = new wxStaticText( parent, ID_TEXT, wxT("Source"), wxDefaultPosition, wxDefaultSize, 0 );
    item50->Add( item56, 0, wxALIGN_CENTRE|wxALL, 5 );

    item57 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item50->Add( item57, 0, wxALIGN_CENTRE|wxALL, 5 );

    item58 = new wxButton( parent, ID_BUTTON, wxT("Search"), wxDefaultPosition, wxDefaultSize, 0 );
    item50->Add( item58, 0, wxALIGN_CENTRE|wxALL, 5 );

    item30->Add( item50, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item28->Add( item30, 0, wxALIGN_CENTRE|wxALL, 5 );

    item60 = new wxStaticBox( parent, -1, wxT("Point Masses") );
    item59 = new wxStaticBoxSizer( item60, wxVERTICAL );

    item61 = new wxFlexGridSizer( 2, 0, 2 );

    item62 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(405,-1), 0 );
    item61->Add( item62, 0, wxALIGN_CENTRE|wxALL, 5 );

    item63 = new wxButton( parent, ID_BUTTON, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
    item61->Add( item63, 0, wxALIGN_CENTRE|wxALL, 5 );

    item59->Add( item61, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item28->Add( item59, 0, wxALIGN_CENTRE|wxALL, 5 );

    item65 = new wxStaticBox( parent, -1, wxT("Solar Radiation Pressure") );
    item64 = new wxStaticBoxSizer( item65, wxHORIZONTAL );

    item64->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    item66 = new wxCheckBox( parent, ID_CHECKBOX, wxT("Use"), wxDefaultPosition, wxDefaultSize, 0 );
    item64->Add( item66, 0, wxALIGN_CENTRE|wxALL, 5 );

    item64->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    item67 = new wxButton( parent, ID_BUTTON, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
    item64->Add( item67, 0, wxALIGN_CENTRE|wxALL, 5 );

    item28->Add( item64, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item4->Add( item28, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item0->Add( item4, 0, wxALIGN_CENTRE|wxALL, 5 );

    item68 = new wxBoxSizer( wxHORIZONTAL );

    item69 = new wxButton( parent, ID_BUTTON, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item68->Add( item69, 0, wxALIGN_CENTRE|wxALL, 5 );

    item70 = new wxButton( parent, ID_BUTTON, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    item68->Add( item70, 0, wxALIGN_CENTRE|wxALL, 5 );

    item71 = new wxButton( parent, ID_BUTTON, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item68->Add( item71, 0, wxALIGN_CENTRE|wxALL, 5 );

    item0->Add( item68, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    parent->SetAutoLayout(true);
    parent->SetSizer( item0 );
    item0->Fit( parent );
    item0->SetSizeHints( parent );
    
    item35->Enable(false);
    item44->Enable(false);
    item49->Enable(false);
    item58->Enable(false);
    item63->Enable(false);
    item67->Enable(false);    
    item69->Enable(false);
    item70->Enable(false);
    item71->Enable(false);
    
    GetData();
}

void PropagationConfigPanel::GetData()
{
    setting1StaticText->SetLabel("Max Relative Error: ");
    setting2StaticText->SetLabel("Min Relative Error: ");
    setting3StaticText->SetLabel("Max Step Size: ");
    setting4StaticText->SetLabel("Max Step Size During Maneuver: ");
    setting5StaticText->SetLabel("Min Step Size: ");
    setting6StaticText->SetLabel("Initial Step Size: ");
    setting7StaticText->SetLabel("Step Size Fraction (High): ");
    setting8StaticText->SetLabel("Step Size Fraction (Low): ");
    setting9StaticText->SetLabel("Maximum Iterations: ");
    
    setting1TextCtrl->AppendText("1E-10");
    setting2TextCtrl->AppendText("1E-11");
    setting3TextCtrl->AppendText("0.5");
    //setting3TextCtrl->AppendText(theIntegrator->GetParameterTypeString(theIntegrator->maximumStepSize));
    setting4TextCtrl->AppendText("1");
    setting5TextCtrl->AppendText("1E-5");
    //setting5TextCtrl->AppendText(theIntegrator->GetParameterTypeString(theIntegrator->minimumStepSize));
    setting6TextCtrl->AppendText("10");
    setting7TextCtrl->AppendText("0.9");
    setting8TextCtrl->AppendText("0.9");
    setting9TextCtrl->AppendText("20");
}

void PropagationConfigPanel::SetData()
{
    return;
}

void PropagationConfigPanel::OnButton(wxCommandEvent& event)
{
    if ( event.GetEventObject() == item69 )  // OK
    {
        OnOK();
    }
    else if ( event.GetEventObject() == item70 )  // Apply
    {
        OnApply();
    }
    else if ( event.GetEventObject() == item71 )  // Cancel
    {
        OnCancel();
    }
    else
    {
        event.Skip();
    }
}

void PropagationConfigPanel::OnOK()
{
    Close(true);
}

void PropagationConfigPanel::OnApply()
{
    return;
}

void PropagationConfigPanel::OnCancel()
{
    Close(true);
}

