//$Header$
//------------------------------------------------------------------------------
//                              ParameterCreateDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Implements ParameterCreateDialog class. This class shows dialog window where a
 * user parameter can be created.
 * 
 */
//------------------------------------------------------------------------------

#include "ParameterCreateDialog.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ParameterCreateDialog, GmatDialog)
    EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
    EVT_BUTTON(ID_BUTTON_APPLY, GmatDialog::OnApply)
    EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
    EVT_BUTTON(ID_BUTTON, ParameterCreateDialog::OnButton)
    EVT_COMBOBOX(ID_COMBO, ParameterCreateDialog::OnComboSelection)
    EVT_TEXT(ID_TEXTCTRL, ParameterCreateDialog::OnTextUpdate)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ParameterCreateDialog(wxWindow *parent)
//------------------------------------------------------------------------------
ParameterCreateDialog::ParameterCreateDialog(wxWindow *parent)
    : GmatDialog(parent, -1, wxString(_T("ParameterCreateDialog")))
{
    mParamName = "";
    mIsParamCreated = false;
    
    Create();
    Show();
}

//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void ParameterCreateDialog::Create()
{
    int borderSize = 2;
    
    // wxString
    wxString strArray3[] = { wxT("") };
    wxString strArray4[] = { wxT("") };
    wxString strArray5[] = { wxT("") };

    //wxStaticText
    wxStaticText *objStaticText =
        new wxStaticText( this, ID_TEXT, wxT("Object"),
                          wxDefaultPosition, wxDefaultSize, 0 );
    wxStaticText *propertyStaticText =
        new wxStaticText( this, ID_TEXT, wxT("Property"),
                          wxDefaultPosition, wxDefaultSize, 0 );
    wxStaticText *paramStaticText =
        new wxStaticText( this, ID_TEXT, wxT("User Variables"),
                          wxDefaultPosition, wxDefaultSize, 0 );
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
    
    // wxButton
    addPropertyButton = new wxButton( this, ID_BUTTON, wxT("Add"),
                                      wxDefaultPosition, wxDefaultSize, 0 );
    addParamButton = new wxButton( this, ID_BUTTON, wxT("Add"),
                                   wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxListBox
    
    objectListBox = 
        theGuiManager->GetObjectListBox(this, wxSize(150, 200));
       
    propertyListBox = 
        theGuiManager->GetParameterListBox(this, wxSize(150, 200),
                                           objectListBox->GetStringSelection(),
                                           theGuiManager->GetNumSpacecraft());
    parameterListBox =
        theGuiManager->GetConfigParameterListBox(this, wxSize(150, 200),
                                                 "");
    

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
    wxFlexGridSizer *objPropertyFlexGridSizer = new wxFlexGridSizer( 4, 0, 0 );
    wxFlexGridSizer *detailsBoxSizer = new wxFlexGridSizer( 5, 0, 0 );
    wxStaticBox *detailStaticBox = new wxStaticBox( this, -1, wxT("Details") );
    wxStaticBoxSizer *detailsStaticBoxSizer = new wxStaticBoxSizer( detailStaticBox, wxVERTICAL );
    
    // Add to wx*Sizers
    top1FlexGridSizer->Add(nameStaticText, 0, wxALIGN_CENTER|wxALL, borderSize);
    top1FlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
    top1FlexGridSizer->Add(expStaticText, 0, wxALIGN_CENTER|wxALL, borderSize);
    
    top1FlexGridSizer->Add(nameTextCtrl, 0, wxALIGN_CENTER|wxALL, borderSize);
    top1FlexGridSizer->Add(equalSignStaticText, 0, wxALIGN_CENTER|wxALL, borderSize);
    top1FlexGridSizer->Add(expTextCtrl, 0, wxALIGN_CENTER|wxALL, borderSize);
                        
    // 1st row
    objPropertyFlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
    objPropertyFlexGridSizer->Add(addPropertyButton, 0, wxALIGN_CENTRE|wxALL, borderSize);
    objPropertyFlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
    objPropertyFlexGridSizer->Add(addParamButton, 0, wxALIGN_CENTER|wxALL, borderSize);
    
    // 2nd row
    objPropertyFlexGridSizer->Add(objStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
    objPropertyFlexGridSizer->Add(propertyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
    objPropertyFlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
    objPropertyFlexGridSizer->Add(paramStaticText, 0, wxALIGN_CENTER|wxALL, borderSize);

    // 3rd row
    objPropertyFlexGridSizer->Add(objectListBox, 0, wxALIGN_CENTER|wxALL, borderSize);
    objPropertyFlexGridSizer->Add(propertyListBox, 0, wxALIGN_CENTER|wxALL, borderSize);
    objPropertyFlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
    objPropertyFlexGridSizer->Add(parameterListBox, 0, wxALIGN_CENTER|wxALL, borderSize);
    
    detailsBoxSizer->Add( cbodyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
    detailsBoxSizer->Add( coordStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
    detailsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, borderSize);
    detailsBoxSizer->Add( rbodyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
    detailsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, borderSize);
    detailsBoxSizer->Add( cbodyComboBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
    detailsBoxSizer->Add( coordComboBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
    detailsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, borderSize);
    detailsBoxSizer->Add( rbodyComboBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
    detailsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, borderSize);
    detailsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, borderSize);
    detailsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, borderSize);
    detailsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, borderSize);
    detailsBoxSizer->Add( epochStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
    detailsBoxSizer->Add( indexStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
    detailsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, borderSize);
    detailsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, borderSize);
    detailsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, borderSize);
    detailsBoxSizer->Add( epochTextCtrl, 0, wxALIGN_CENTRE|wxALL, borderSize);
    detailsBoxSizer->Add( indexTextCtrl, 0, wxALIGN_CENTRE|wxALL, borderSize);

    detailsStaticBoxSizer->Add( detailsBoxSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);    
    
    pageBoxSizer->Add( top1FlexGridSizer, 0, wxALIGN_TOP|wxALL, borderSize);
    pageBoxSizer->Add( objPropertyFlexGridSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
    pageBoxSizer->Add( detailsStaticBoxSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);

    //------------------------------------------------------
    // add to parent sizer
    //------------------------------------------------------
    theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);

}

//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnTextUpdate(wxCommandEvent& event)
{
    if ((nameTextCtrl->GetValue() != "") &&
        (expTextCtrl->GetValue() != ""))
    {
        theOkButton->Enable();
    }
}
    
//------------------------------------------------------------------------------
// void OnComboSelection(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnComboSelection(wxCommandEvent& event)
{
    ;
}

//------------------------------------------------------------------------------
// void OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnButton(wxCommandEvent& event)
{    
    if ( event.GetEventObject() == addPropertyButton )  
    {
        wxString s = objectListBox->GetStringSelection() + "." +
            propertyListBox->GetStringSelection();

        //loj: for build2 donot append
        //expTextCtrl->AppendText(s);
        expTextCtrl->SetValue(s);
    }
    else if ( event.GetEventObject() == addParamButton )  
    {
        expTextCtrl->AppendText(parameterListBox->GetStringSelection());
    }
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ParameterCreateDialog::LoadData()
{
    //loj: for build2, disable the button, disable textctrl
    addParamButton->Disable();
    expTextCtrl->Disable();
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ParameterCreateDialog::SaveData()
{
    std::string objName = std::string(objectListBox->GetStringSelection().c_str());
    std::string varName = std::string(nameTextCtrl->GetValue().c_str());
    std::string varType = std::string(propertyListBox->GetStringSelection().c_str());
    std::string varDesc = std::string(expTextCtrl->GetValue().c_str());

    MessageInterface::ShowMessage("ParameterCreateDialog::SaveData() objName = " +
                                  objName + " varName = " + varName +
                                  "varType = " + varType + "\n");
    if (varName != "" && varDesc != "")
    {
        // if new user variable to create
        if (theGuiInterpreter->GetParameter(varName) == NULL)
        {
            //loj: because "Param" was removed from the name, add back
            Parameter *param = theGuiInterpreter->CreateParameter(varType+"Param", varName);
            param->AddObject(objName);
            param->SetDesc(varDesc);
            MessageInterface::ShowMessage("ParameterCreateDialog::SaveData() user var added\n");
            
            mParamName = wxString(varName.c_str());
            mIsParamCreated = true;
            theGuiManager->UpdateParameter();

            theOkButton->Enable();
        }
    }
}

//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void ParameterCreateDialog::ResetData()
{
    mIsParamCreated = false;
}

