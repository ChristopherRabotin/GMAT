//$Header$
//------------------------------------------------------------------------------
//                              ParameterSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Implements ParameterSetupPanel class.
 */
//------------------------------------------------------------------------------

#include "ParameterSetupPanel.hpp"
#include "GmatMainNotebook.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ParameterSetupPanel, GmatPanel)
    EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
    EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
    EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
    EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
    EVT_COMBOBOX(ID_COMBO, ParameterSetupPanel::OnComboSelection)
    EVT_TEXT(ID_TEXTCTRL, ParameterSetupPanel::OnTextUpdate)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ParameterSetupPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
ParameterSetupPanel::ParameterSetupPanel(wxWindow *parent, const wxString &name)
    : GmatPanel(parent)
{
    theVarName = name;
    Create();
    LoadData();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void ParameterSetupPanel::Create()
{   
    // wxString
    wxString strArray3[] = { wxT("") };
    wxString strArray4[] = { wxT("") };
    wxString strArray5[] = { wxT("") };

    //wxStaticText
    wxStaticText *cbodyStaticText =
        new wxStaticText( this, ID_TEXT, wxT("Central Body"),
                          wxDefaultPosition, wxDefaultSize, 0 );
    wxStaticText *coordStaticText =
        new wxStaticText( this, ID_TEXT, wxT("Coordinate System"),
                          wxDefaultPosition, wxDefaultSize, 0 );
    wxStaticText *rbodyStaticText =
        new wxStaticText( this, ID_TEXT, wxT("Reference Body"),
                          wxDefaultPosition, wxDefaultSize, 0 );
    wxStaticText *epochStaticText =
        new wxStaticText( this, ID_TEXT, wxT("Reference Epoch"),
                          wxDefaultPosition, wxDefaultSize, 0 );
    wxStaticText *indexStaticText =
        new wxStaticText( this, ID_TEXT, wxT("Index"),
                          wxDefaultPosition, wxDefaultSize, 0 );
    
    wxStaticText *nameStaticText =
        new wxStaticText( this, ID_TEXT, wxT("Name"),
                          wxDefaultPosition, wxDefaultSize, 0 );
    wxStaticText *emptyStaticText =
        new wxStaticText( this, ID_TEXT, wxT("  "),
                          wxDefaultPosition, wxDefaultSize, 0 );
    wxStaticText *equalSignStaticText =
        new wxStaticText( this, ID_TEXT, wxT("="),
                          wxDefaultPosition, wxDefaultSize, 0 );
    wxStaticText *expStaticText =
        new wxStaticText( this, ID_TEXT, wxT("Expression"),
                          wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxTextCtrl
    nameTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
                                   wxDefaultPosition, wxSize(150,20), 0 );
    expTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
                                  wxDefaultPosition, wxSize(300,20), 0 );
    epochTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
                                    wxDefaultPosition, wxSize(80,20), 0 );
    indexTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
                                    wxDefaultPosition, wxSize(80,20), 0 );
    
    // wxComboBox
    cbodyComboBox = new wxComboBox( this, ID_COMBO, wxT(""), wxDefaultPosition,
                                    wxSize(100,-1), 1, strArray3, wxCB_DROPDOWN );
    coordComboBox = new wxComboBox( this, ID_COMBO, wxT(""), wxDefaultPosition,
                                    wxSize(100,-1), 1, strArray4, wxCB_DROPDOWN );
    rbodyComboBox = new wxComboBox( this, ID_COMBO, wxT(""), wxDefaultPosition,
                                    wxSize(100,-1), 1, strArray5, wxCB_DROPDOWN );
    
    
    // wxSizers
    wxBoxSizer *pageBoxSizer = new wxBoxSizer( wxVERTICAL );
    wxFlexGridSizer *top1FlexGridSizer = new wxFlexGridSizer( 3, 0, 0 );
    wxFlexGridSizer *detailsBoxSizer = new wxFlexGridSizer( 5, 0, 0 );
    wxStaticBox *detailStaticBox = new wxStaticBox( this, -1, wxT("Details") );
    wxStaticBoxSizer *detailsStaticBoxSizer = new wxStaticBoxSizer( detailStaticBox, wxVERTICAL );
    
    // Add to wx*Sizers
    top1FlexGridSizer->Add(nameStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    top1FlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    top1FlexGridSizer->Add(expStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    
    top1FlexGridSizer->Add(nameTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    top1FlexGridSizer->Add(equalSignStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    top1FlexGridSizer->Add(expTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
                            
    detailsBoxSizer->Add( cbodyStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    detailsBoxSizer->Add( coordStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    detailsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    detailsBoxSizer->Add( rbodyStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    detailsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    detailsBoxSizer->Add( cbodyComboBox, 0, wxALIGN_CENTRE|wxALL, 5 );
    detailsBoxSizer->Add( coordComboBox, 0, wxALIGN_CENTRE|wxALL, 5 );
    detailsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    detailsBoxSizer->Add( rbodyComboBox, 0, wxALIGN_CENTRE|wxALL, 5 );
    detailsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    detailsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    detailsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    detailsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    detailsBoxSizer->Add( epochStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    detailsBoxSizer->Add( indexStaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    detailsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    detailsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    detailsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    detailsBoxSizer->Add( epochTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    detailsBoxSizer->Add( indexTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );

    detailsStaticBoxSizer->Add( detailsBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5 );

    pageBoxSizer->Add( top1FlexGridSizer, 0, wxALIGN_TOP|wxALL, 5 );
    pageBoxSizer->Add( detailsStaticBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5 );

    //------------------------------------------------------
    // add to parent sizer
    //------------------------------------------------------
    theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    Show();

}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void ParameterSetupPanel::LoadData()
{
    Parameter *param = theGuiInterpreter->GetParameter(std::string(theVarName.c_str()));

    if (param != NULL)
    {
        std::string varDesc = param->GetDesc();
        nameTextCtrl->SetValue(theVarName);
        expTextCtrl->SetValue(varDesc.c_str());
    }
    
    nameTextCtrl->Disable();
    expTextCtrl->Disable();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void ParameterSetupPanel::SaveData()
{
    // for now nothing is saved. the Detail box is not ready
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void ParameterSetupPanel::OnTextUpdate(wxCommandEvent& event)
{
    if ( event.GetEventObject() == epochTextCtrl )  
    {
        theApplyButton->Enable(true);       
    }
    else if ( event.GetEventObject() == indexTextCtrl )
    {
        theApplyButton->Enable(false);
    } 
    else
        event.Skip();
}
    
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void ParameterSetupPanel::OnComboSelection(wxCommandEvent& event)
{
    theApplyButton->Enable(true);
}

