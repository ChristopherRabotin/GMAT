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
#include "StringUtil.hpp"       // for GmatStringUtil::
#include "wx/colordlg.h"        // for wxColourDialog

//#define DEBUG_PARAM_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ParameterSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   
   EVT_BUTTON(ID_COLOR_BUTTON, ParameterSetupPanel::OnColorButtonClick)
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
   mVarName = name;
   mIsStringVar = false;
   mIsColorChanged = false;
   mIsExpChanged = false;
   
   mParam = (Parameter*)theGuiInterpreter->GetObject(std::string(mVarName.c_str()));
   if (mParam->GetTypeName() == "String")
      mIsStringVar = true;
   
   Create();
   Show();
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void ParameterSetupPanel::Create()
{
   int bsize = 3; // border size
   
   //-------------------------------------------------------
   // for Variable Setup
   //-------------------------------------------------------
   
   wxStaticText *nameStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Name"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *emptyStaticText =
      new wxStaticText(this, ID_TEXT, wxT("  "),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *equalSignStaticText =
      new wxStaticText(this, ID_TEXT, wxT("="),
                        wxDefaultPosition, wxDefaultSize, 0);
   mExpStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Expression"),
                       wxDefaultPosition, wxDefaultSize, 0);
   mColorStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Color"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   // wxTextCtrl
   mVarNameTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                 wxDefaultPosition, wxSize(150,20), 0);
   mVarExpTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                 wxDefaultPosition, wxSize(300,20), 0);
   
   // wxButton
   mColorButton =
      new wxButton(this, ID_COLOR_BUTTON, wxT(""),
                   wxDefaultPosition, wxSize(25, 20), 0);
   mColorButton->SetBackgroundColour(mColor);
      
   // wxSizers
   mPageBoxSizer = new wxBoxSizer(wxVERTICAL);
   wxFlexGridSizer *top1FlexGridSizer = new wxFlexGridSizer(3, 0, 0);
   wxBoxSizer *detailsBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   
   wxStaticBox *variableStaticBox;
   
   if (mIsStringVar)
      variableStaticBox = new wxStaticBox(this, -1, wxT("String"));
   else
      variableStaticBox = new wxStaticBox(this, -1, wxT("Variable"));
      
   mVarStaticBoxSizer = new wxStaticBoxSizer(variableStaticBox, wxVERTICAL);
   
   // Add to wx*Sizers
   // for Variable Setup
   // 1st row
   top1FlexGridSizer->Add(nameStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   top1FlexGridSizer->Add(mExpStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
    
   // 1st row
   top1FlexGridSizer->Add(mVarNameTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(equalSignStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(mVarExpTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);

   // detail
   detailsBoxSizer->Add(mColorStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(mColorButton, 0, wxALIGN_CENTRE|wxALL, bsize);

   mVarStaticBoxSizer->Add(top1FlexGridSizer, 0, wxALIGN_TOP|wxALL, bsize);
   mVarStaticBoxSizer->Add(detailsBoxSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mPageBoxSizer->Add(mVarStaticBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(mPageBoxSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);

}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ParameterSetupPanel::LoadData()
{
   mParam = (Parameter*)theGuiInterpreter->GetObject(std::string(mVarName.c_str()));

   // Set the pointer for the "Show Script" button
   mObject = mParam;

   if (mParam != NULL)
   {
      
      #if DEBUG_PARAM_PANEL
      MessageInterface::ShowMessage
         ("ParameterSetupPanel::LoadData() paramName=%s\n", mParam->GetName().c_str());
      #endif
      
      try
      {
         // show expression
         std::string varExp = mParam->GetStringParameter("Expression");
         mVarNameTextCtrl->SetValue(mVarName);
         mVarExpTextCtrl->SetValue(varExp.c_str());

         UnsignedInt intColor = mParam->GetUnsignedIntParameter("Color");
         RgbColor color(intColor);
         mColor.Set(color.Red(), color.Green(), color.Blue());
         mColorButton->SetBackgroundColour(mColor);
      }
      catch (BaseException &e)
      {
         wxLogError(wxT(e.GetMessage().c_str()));
         wxLog::FlushActive();
      }
   }

   if (!mIsStringVar)
   {
      // if expression is just a number, enable editing
      double realVal;
      if (mVarExpTextCtrl->GetValue().ToDouble(&realVal))
         mVarExpTextCtrl->Enable();
      else
         mVarExpTextCtrl->Disable();
   }
   else
   {
      mVarExpTextCtrl->SetValue(mParam->GetStringParameter("Expression").c_str());
      mExpStaticText->SetLabel("Value");
      mColorStaticText->Hide();
      mColorButton->Hide();
   }
   
   mVarNameTextCtrl->Disable();
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ParameterSetupPanel::SaveData()
{
   canClose = true;
   
   if (mIsExpChanged)
   {
      std::string strval = mVarExpTextCtrl->GetValue().c_str();
      Real rval;
      if (GmatStringUtil::ToDouble(strval, &rval))
      {
         mIsExpChanged = false;
         mParam->SetStringParameter("Expression",
                                    std::string(mVarExpTextCtrl->GetValue().c_str()));
      }
      else
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "\"%s\" is not a valid Real number.", strval.c_str());
         canClose = false;
      }
   }
   
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
   if (mVarExpTextCtrl->IsModified())
   {
      mIsExpChanged = true;
      //theApplyButton->Enable();
      EnableUpdate(true);
   }
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
      //theApplyButton->Enable();
      EnableUpdate(true);
   }
}




