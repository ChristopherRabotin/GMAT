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
#include "RgbColor.hpp"
#include "MessageInterface.hpp"

#include "wx/colordlg.h"   // for wxColourDialog

#define DEBUG_PARAM_PANEL 0

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ParameterSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   
   EVT_BUTTON(ID_COLOR_BUTTON, ParameterSetupPanel::OnColorButtonClick)
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
   Show();
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void ParameterSetupPanel::Create()
{
   int bsize = 3; // border size
   
   // wxString
   wxString strArray3[] = { wxT("") };
   wxString strArray4[] = { wxT("") };
   wxString strArray5[] = { wxT("") };

   //wxStaticText
   wxStaticText *cbodyStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Central Body"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *coordStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Coordinate System"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *rbodyStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Reference Body"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *epochStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Reference Epoch"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *indexStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Index"),
                       wxDefaultPosition, wxDefaultSize, 0);
    
   wxStaticText *nameStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Name"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *emptyStaticText =
      new wxStaticText(this, ID_TEXT, wxT("  "),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *equalSignStaticText =
      new wxStaticText(this, ID_TEXT, wxT("="),
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *expStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Expression"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *colorStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Color"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   // wxTextCtrl
   nameTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                 wxDefaultPosition, wxSize(150,20), 0);
   expTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                 wxDefaultPosition, wxSize(300,20), 0);
   epochTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                  wxDefaultPosition, wxSize(80,20), 0);
   indexTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                  wxDefaultPosition, wxSize(80,20), 0);
    
   // wxButton
   mColorButton =
      new wxButton(this, ID_COLOR_BUTTON, wxT(""),
                   wxDefaultPosition, wxSize(25, 20), 0);
   mColorButton->SetBackgroundColour(mColor);
   
   // wxComboBox
   cbodyComboBox = new wxComboBox(this, ID_COMBO, wxT(""), wxDefaultPosition,
                                  wxSize(100,-1), 1, strArray3, wxCB_DROPDOWN);
   coordComboBox = new wxComboBox(this, ID_COMBO, wxT(""), wxDefaultPosition,
                                  wxSize(100,-1), 1, strArray4, wxCB_DROPDOWN);
   rbodyComboBox = new wxComboBox(this, ID_COMBO, wxT(""), wxDefaultPosition,
                                  wxSize(100,-1), 1, strArray5, wxCB_DROPDOWN);
    
    
   // wxSizers
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   wxFlexGridSizer *top1FlexGridSizer = new wxFlexGridSizer(3, 0, 0);
   wxFlexGridSizer *detailsBoxSizer = new wxFlexGridSizer(5, 0, 0);
   wxStaticBox *detailStaticBox = new wxStaticBox(this, -1, wxT("Details"));
   wxStaticBoxSizer *detailsStaticBoxSizer =
      new wxStaticBoxSizer(detailStaticBox, wxVERTICAL);
    
   // Add to wx*Sizers
   top1FlexGridSizer->Add(nameStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   top1FlexGridSizer->Add(expStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
    
   top1FlexGridSizer->Add(nameTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(equalSignStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(expTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);

   // detail
   detailsBoxSizer->Add(cbodyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(coordStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(rbodyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(colorStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   detailsBoxSizer->Add(cbodyComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(coordComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(rbodyComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(mColorButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   detailsBoxSizer->Add(20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(epochStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(indexStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   detailsBoxSizer->Add(20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(epochTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(indexTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);

   detailsStaticBoxSizer->Add(detailsBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

   pageBoxSizer->Add(top1FlexGridSizer, 0, wxALIGN_TOP|wxALL, bsize);
   pageBoxSizer->Add(detailsStaticBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ParameterSetupPanel::LoadData()
{
   mParam = theGuiInterpreter->GetParameter(std::string(theVarName.c_str()));

   if (mParam != NULL)
   {
      
#if DEBUG_PARAM_PANEL
   MessageInterface::ShowMessage
      ("ParameterSetupPanel::LoadData() paramName=%s\n", mParam->GetName().c_str());
#endif
   
      std::string varDesc = mParam->GetStringParameter("Description");
      nameTextCtrl->SetValue(theVarName);
      expTextCtrl->SetValue(varDesc.c_str());

      UnsignedInt intColor = mParam->GetUnsignedIntParameter("Color");
      RgbColor color(intColor);
      mColor.Set(color.Red(), color.Green(), color.Blue());
      mColorButton->SetBackgroundColour(mColor);
   }
   
   nameTextCtrl->Disable();
   expTextCtrl->Disable();
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ParameterSetupPanel::SaveData()
{
   if (mIsColorChanged)
   {
      mIsColorChanged = false;
      RgbColor color(mColor.Red(), mColor.Green(), mColor.Blue());
      mParam->SetUnsignedIntParameter("Color", color.GetIntColor());
   }
}

//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSetupPanel::OnTextUpdate(wxCommandEvent& event)
{
   if (event.GetEventObject() == epochTextCtrl)  
   {
      theApplyButton->Enable(true);       
   }
   else if (event.GetEventObject() == indexTextCtrl)
   {
      theApplyButton->Enable(false);
   } 
   else
      event.Skip();
}
    
//------------------------------------------------------------------------------
// void OnComboSelection(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSetupPanel::OnComboSelection(wxCommandEvent& event)
{
   theApplyButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnColorButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSetupPanel::OnColorButtonClick(wxCommandEvent& event)
{    
   wxColourData data;
   data.SetColour(mColor);

   wxColourDialog dialog(this, &data);
   //dialog.CenterOnParent();
   dialog.Center();
   
   if (dialog.ShowModal() == wxID_OK)
   {
      mColor = dialog.GetColourData().GetColour();
      mColorButton->SetBackgroundColour(mColor);
      mIsColorChanged = true;
      theApplyButton->Enable();
   }
}
