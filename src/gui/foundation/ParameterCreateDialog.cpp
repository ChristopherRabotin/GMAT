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
#include "RgbColor.hpp"
#include "MessageInterface.hpp"
#include "StringTokenizer.hpp"

#include "wx/colordlg.h"   // for wxColourDialog

#define DEBUG_PARAM_DIALOG 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ParameterCreateDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, ParameterCreateDialog::OnOK)
   //EVT_BUTTON(ID_BUTTON_APPLY, GmatDialog::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, ParameterCreateDialog::OnButton)
   EVT_BUTTON(ID_COLOR_BUTTON, ParameterCreateDialog::OnColorButtonClick)
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
   mColor.Set(0, 0, 0); // initialize to black
   
   Create();
   Show();
   mCreateParamButton->Disable();
}

//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void ParameterCreateDialog::Create()
{
   int bsize = 2;
    
   // wxString
   wxString strArray3[] = { wxT("") };
   wxString strArray4[] = { wxT("") };
   wxString strArray5[] = { wxT("") };

   //wxStaticText
   wxStaticText *objStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Object"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *propertyStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Property"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *paramStaticText =
      new wxStaticText(this, ID_TEXT, wxT("User Variables"),
                       wxDefaultPosition, wxDefaultSize, 0);
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
      new wxStaticText(this, ID_TEXT, wxT("Expression (Available Operators: +-*/^ )"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *colorStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Color"),
                       wxDefaultPosition, wxDefaultSize, 0);
    
   // wxTextCtrl
   mNameTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(150,20), 0);
   mExprTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(300,20), 0);
   mEpochTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(80,20), 0);
   mIndexTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(80,20), 0);
   
   // wxButton
   mCreateParamButton =
      new wxButton(this, ID_BUTTON, wxT("Create"),
                   wxDefaultPosition, wxDefaultSize, 0);
   
   mAddPropertyButton =
      new wxButton(this, ID_BUTTON, wxT("Paste"),
                   wxDefaultPosition, wxDefaultSize, 0);
   mAddParamButton =
      new wxButton(this, ID_BUTTON, wxT("Paste"),
                   wxDefaultPosition, wxDefaultSize, 0);
   mColorButton =
      new wxButton(this, ID_COLOR_BUTTON, wxT(""),
                   wxDefaultPosition, wxSize(25, 20), 0);
   mColorButton->SetBackgroundColour(mColor);
   
   // wxListBox
   wxArrayString emptyArray;
   mObjectListBox = 
      theGuiManager->GetSpacecraftListBox(this, -1, wxSize(150, 150), emptyArray);
   
   mPropertyListBox = 
      theGuiManager->GetParameterListBox(this, -1, wxSize(150, 150),
                                         "Spacecraft"); //loj: 9/22/04 Spacecraft for now
                                         //mObjectListBox->GetStringSelection());

//     mUserParamListBox = 
//        theGuiManager->GetConfigParameterListBox(this, -1, wxSize(150, 200),
//                                                 "");
   //loj: 9/24/04 use GetUserParameterListBox()
   mUserParamListBox =
      theGuiManager->GetUserParameterListBox(this, -1, wxSize(150, 150), "");
    
   
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
   wxFlexGridSizer *objPropertyFlexGridSizer = new wxFlexGridSizer(4, 0, 0);
   wxFlexGridSizer *detailsBoxSizer = new wxFlexGridSizer(5, 0, 0);
   wxStaticBox *detailStaticBox = new wxStaticBox(this, -1, wxT("Details"));
   wxStaticBoxSizer *detailsStaticBoxSizer = new wxStaticBoxSizer(detailStaticBox, wxVERTICAL);
   
   // Add to wx*Sizers
   top1FlexGridSizer->Add(nameStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   top1FlexGridSizer->Add(expStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
    
   top1FlexGridSizer->Add(mNameTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(equalSignStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(mExprTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
                        
   // 1st row
   objPropertyFlexGridSizer->Add(mCreateParamButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   objPropertyFlexGridSizer->Add(mAddPropertyButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   objPropertyFlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   objPropertyFlexGridSizer->Add(mAddParamButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   // 2nd row
   objPropertyFlexGridSizer->Add(objStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   objPropertyFlexGridSizer->Add(propertyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   objPropertyFlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   objPropertyFlexGridSizer->Add(paramStaticText, 0, wxALIGN_CENTER|wxALL, bsize);

   // 3rd row
   objPropertyFlexGridSizer->Add(mObjectListBox, 0, wxALIGN_CENTER|wxALL, bsize);
   objPropertyFlexGridSizer->Add(mPropertyListBox, 0, wxALIGN_CENTER|wxALL, bsize);
   objPropertyFlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   objPropertyFlexGridSizer->Add(mUserParamListBox, 0, wxALIGN_CENTER|wxALL, bsize);

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
   detailsBoxSizer->Add(mEpochTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(mIndexTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);

   detailsStaticBoxSizer->Add(detailsBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);    
    
   pageBoxSizer->Add(top1FlexGridSizer, 0, wxALIGN_TOP|wxALL, bsize);
   pageBoxSizer->Add(objPropertyFlexGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageBoxSizer->Add(detailsStaticBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

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
   if (mNameTextCtrl->GetValue() != "" && mNameTextCtrl->GetValue() != " " &&
       mExprTextCtrl->GetValue() != "" && mExprTextCtrl->GetValue() != " ")
   {
      mCreateParamButton->Enable();
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
   if (event.GetEventObject() == mCreateParamButton)  
   {
      SaveData();
   }
   else if (event.GetEventObject() == mAddPropertyButton)  
   {
      wxString s = mObjectListBox->GetStringSelection() + "." +
         mPropertyListBox->GetStringSelection();

      mExprTextCtrl->AppendText(s);
      //mNameTextCtrl->SetValue(s); //loj: 9/23/04

      if (mNameTextCtrl->GetValue() != "" &&
          mNameTextCtrl->GetValue() != " ")
      {
         mCreateParamButton->Enable();
         theOkButton->Enable();
      }
   }
   else if (event.GetEventObject() == mAddParamButton)  
   {
      mExprTextCtrl->AppendText(mUserParamListBox->GetStringSelection());
   }
}

//------------------------------------------------------------------------------
// void OnColorButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnColorButtonClick(wxCommandEvent& event)
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
   }
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ParameterCreateDialog::LoadData()
{
   //mAddParamButton->Disable();
   //mExprTextCtrl->Disable();
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ParameterCreateDialog::SaveData()
{
   std::string objName = std::string(mObjectListBox->GetStringSelection().c_str());
   std::string varName = std::string(mNameTextCtrl->GetValue().c_str());
   std::string varType = std::string(mPropertyListBox->GetStringSelection().c_str());
   //std::string varDesc = std::string(mExprTextCtrl->GetValue().c_str());
   std::string varExpr = std::string(mExprTextCtrl->GetValue().c_str());

#if DEBUG_PARAM_DIALOG
   MessageInterface::ShowMessage
      ("ParameterCreateDialog::SaveData() objName = " + objName +
       " varName = " + varName + "varType = " + varType + "\n");
#endif
   
   //if (varName != "" && varDesc != "")
   if (varName != "" && varExpr != "")
   {
      // if new user variable to create
      if (theGuiInterpreter->GetParameter(varName) == NULL)
      {
         Parameter *param;
         
         //param = theGuiInterpreter->CreateParameter(varType, varName);
         param = theGuiInterpreter->CreateParameter("Variable", varName);
         //param->SetStringParameter("Description", varDesc); //loj: 9/23/04
         param->SetStringParameter("Expression", varExpr);

         // Parse the Parameter
         StringTokenizer st(varExpr, "()*/+-^ ");
         StringArray tokens = st.GetAllTokens();

         for (unsigned int i=0; i<tokens.size(); i++)
         {
#if DEBUG_PARAM_DIALOG
            MessageInterface::ShowMessage("token:<%s> \n", tokens[i].c_str());
#endif
            // if token does not start with number
            if (!isdigit(*tokens[i].c_str()))
            {
               param->SetRefObjectName(Gmat::PARAMETER, tokens[i]);
            }
         }
         
         RgbColor color(mColor.Red(), mColor.Green(), mColor.Blue());
         param->SetUnsignedIntParameter("Color", color.GetIntColor());
         
#if DEBUG_PARAM_DIALOG
         MessageInterface::ShowMessage
            ("ParameterCreateDialog::SaveData() user var:%s added\n",
             varName.c_str());
#endif
         
         mParamName = wxString(varName.c_str());
         mIsParamCreated = true;
         theGuiManager->UpdateParameter();

         mUserParamListBox->Append(varName.c_str());

         for (int i=0; i<mUserParamListBox->GetCount(); i++)
         {
            if (mUserParamListBox->GetString(i).IsSameAs(varName.c_str()))
            {
               mUserParamListBox->SetSelection(i);
               break;
            }
         }
         
         theOkButton->Enable();
         //loj: once variable is created cannot revert (delete) for now
         theCancelButton->Disable();
      }
      else
      {
         //loj: 7/28/04 added
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "ParameterCreateDialog::SaveData()\nThe variable: %s"
             " cannot be created. It already exists.", varName.c_str());
      }
      
      mCreateParamButton->Disable();
      mExprTextCtrl->SetValue("");
      mNameTextCtrl->SetValue("");
   }
}

//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void ParameterCreateDialog::ResetData()
{
   mIsParamCreated = false;
}

//------------------------------------------------------------------------------
// void OnOK()
//------------------------------------------------------------------------------
/**
 * Saves the data and closes the page
 */
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnOK()
{
#if DEBUG_PARAM_DIALOG
   MessageInterface::ShowMessage
      ("ParameterCreateDialog::OnOk() mCreateParamButton->IsEnabled()=%d\n",
       mCreateParamButton->IsEnabled());
#endif
   
   if (mCreateParamButton->IsEnabled())
      SaveData();
   
   Close();
}
