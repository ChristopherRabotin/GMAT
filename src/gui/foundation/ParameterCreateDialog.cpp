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

//#define DEBUG_PARAM_DIALOG 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ParameterCreateDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, ParameterCreateDialog::OnOK)
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
   mParamNames.Clear();
   mIsParamCreated = false;
   mCreateVariable = false;
   mCreateArray = false;
   mColor.Set(0, 0, 0); // initialize to black
   
   Create();
   Show();
}

//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void ParameterCreateDialog::Create()
{
   int bsize = 2;
   
   // wxString
   wxString strCoordArray[] = { wxT("") };

   //wxStaticText
   wxStaticText *varNameStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Name"),
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *objStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Object"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *propertyStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Property"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *configVarStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Variables"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *coordStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Coordinate System"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *expStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Expression (Available Operators: +-*/^ )"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *colorStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Color"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *varEqualSignStaticText =
      new wxStaticText(this, ID_TEXT, wxT("="),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   wxStaticText *arrNameStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Name"),
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *arr1RowStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Row"),
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *arr1ColStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Column"),
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *emptyStaticText =
      new wxStaticText(this, ID_TEXT, wxT("  "),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *arrEqualSignStaticText =
      new wxStaticText(this, ID_TEXT, wxT("="),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *arrTimesStaticText =
      new wxStaticText(this, ID_TEXT, wxT(" X"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *configArrStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Arrays"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   // wxTextCtrl
   mVarNameTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                     wxDefaultPosition, wxSize(130,20), 0);
   mExprTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                  wxDefaultPosition, wxSize(280,20), 0);
   
   mArrNameTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                     wxDefaultPosition, wxSize(102,20), 0);
   mArrRowTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                    wxDefaultPosition, wxSize(35,20), 0);
   mArrColTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                    wxDefaultPosition, wxSize(35,20), 0);
   
   // wxButton
   mCreateVariableButton = new wxButton(this, ID_BUTTON, wxT("Create"),
                                        wxDefaultPosition, wxDefaultSize, 0);
   mPastePropertyButton = new wxButton(this, ID_BUTTON, wxT("Paste"),
                                     wxDefaultPosition, wxDefaultSize, 0);
   mPasteUserVarButton = new wxButton(this, ID_BUTTON, wxT("Paste"),
                                  wxDefaultPosition, wxDefaultSize, 0);
   mColorButton =  new wxButton(this, ID_COLOR_BUTTON, wxT(""),
                                wxDefaultPosition, wxSize(25, 20), 0);
   mColorButton->SetBackgroundColour(mColor);
   
   mCreateArrayButton = new wxButton(this, ID_BUTTON, wxT("Create"),
                                     wxDefaultPosition, wxDefaultSize, 0);
   
   
   // wxListBox
   wxArrayString emptyArray;
   mObjectListBox = 
      theGuiManager->GetSpacecraftListBox(this, -1, wxSize(135, 120), emptyArray);
   
   //loj: 10/1/04 changed GetParameterListBox() to GetPropertyListBox()
   mPropertyListBox = 
      theGuiManager->GetPropertyListBox(this, -1, wxSize(135, 120), "Spacecraft");
   
   //loj: 9/24/04 use GetUserVariableListBox()
   mUserVarListBox =
      theGuiManager->GetUserVariableListBox(this, -1, wxSize(135, 120), "");
   
   mUserArrayListBox =
      theGuiManager->GetUserArrayListBox(this, -1, wxSize(135, 50), "");
   


   // wxComboBox
   mCoordComboBox = new wxComboBox(this, ID_COMBO, wxT(""), wxDefaultPosition,
                                  wxSize(100,-1), 1, strCoordArray, wxCB_DROPDOWN);
    
   
   // wxSizers
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   wxFlexGridSizer *top1FlexGridSizer = new wxFlexGridSizer(3, 0, 0);
   wxFlexGridSizer *objPropertyFlexGridSizer = new wxFlexGridSizer(4, 0, 0);
   wxBoxSizer *detailsBoxSizer = new wxBoxSizer(wxHORIZONTAL);   

   wxFlexGridSizer *arr1FlexGridSizer = new wxFlexGridSizer(7, 0, 0);

   wxStaticBox *variableStaticBox = new wxStaticBox(this, -1, wxT("Variable"));
   wxStaticBoxSizer *variableStaticBoxSizer =
      new wxStaticBoxSizer(variableStaticBox, wxVERTICAL);

   wxStaticBox *arrayStaticBox = new wxStaticBox(this, -1, wxT("Array"));
   wxStaticBoxSizer *arrayStaticBoxSizer =
      new wxStaticBoxSizer(arrayStaticBox, wxVERTICAL);

   
   // Add to wx*Sizers
   // for Variable
   top1FlexGridSizer->Add(varNameStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   top1FlexGridSizer->Add(expStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   
   top1FlexGridSizer->Add(mVarNameTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(varEqualSignStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(mExprTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   
   // 1st row
   objPropertyFlexGridSizer->Add(mCreateVariableButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   objPropertyFlexGridSizer->Add(mPastePropertyButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   objPropertyFlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   objPropertyFlexGridSizer->Add(mPasteUserVarButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   // 2nd row
   objPropertyFlexGridSizer->Add(objStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   objPropertyFlexGridSizer->Add(propertyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   objPropertyFlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   objPropertyFlexGridSizer->Add(configVarStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   
   // 3rd row
   objPropertyFlexGridSizer->Add(mObjectListBox, 0, wxALIGN_CENTER|wxALL, bsize);
   objPropertyFlexGridSizer->Add(mPropertyListBox, 0, wxALIGN_CENTER|wxALL, bsize);
   objPropertyFlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   objPropertyFlexGridSizer->Add(mUserVarListBox, 0, wxALIGN_CENTER|wxALL, bsize);
   
   detailsBoxSizer->Add(coordStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(mCoordComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(colorStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(mColorButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   variableStaticBoxSizer->Add(top1FlexGridSizer, 0, wxALIGN_TOP|wxALL, bsize);
   variableStaticBoxSizer->Add(objPropertyFlexGridSizer, 0, wxALIGN_TOP|wxALL, bsize);
   variableStaticBoxSizer->Add(detailsBoxSizer, 0, wxALIGN_LEFT|wxALL, bsize);

   // for Array Creation
   // 1st row
   arr1FlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(arrNameStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(arr1RowStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(arr1ColStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(configArrStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   // 2nd row
   arr1FlexGridSizer->Add(mCreateArrayButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(mArrNameTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(arrEqualSignStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(mArrRowTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(arrTimesStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(mArrColTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(mUserArrayListBox, 0, wxALIGN_CENTRE|wxALL, bsize);

   arrayStaticBoxSizer->Add(arr1FlexGridSizer, 0, wxALIGN_TOP|wxALL, bsize);
   
   pageBoxSizer->Add(variableStaticBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageBoxSizer->Add(arrayStaticBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);

}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ParameterCreateDialog::LoadData()
{
   mCreateVariableButton->Disable();
   mCreateArrayButton->Disable();
}

//------------------------------------------------------------------------------
// void CreateVariable()
//------------------------------------------------------------------------------
void ParameterCreateDialog::CreateVariable()
{
   std::string varName = std::string(mVarNameTextCtrl->GetValue().c_str());
   std::string varExpr = std::string(mExprTextCtrl->GetValue().c_str());

#if DEBUG_PARAM_DIALOG
   MessageInterface::ShowMessage
      ("ParameterCreateDialog::CreateVariable() varName = "  + varName +
       " varExpr = " + varExpr + "\n");
#endif
   
   if (varName != "" && varExpr != "")
   {
      // if new user variable to create
      if (theGuiInterpreter->GetParameter(varName) == NULL)
      {
         Parameter *param;
         
         param = theGuiInterpreter->CreateParameter("Variable", varName);
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
            ("ParameterCreateDialog::CreateVariable() user var:%s added\n",
             varName.c_str());
#endif
         
         mParamNames.Add(varName.c_str());
         mIsParamCreated = true;
         theGuiManager->UpdateParameter();

         mUserVarListBox->Append(varName.c_str());

         for (int i=0; i<mUserVarListBox->GetCount(); i++)
         {
            if (mUserVarListBox->GetString(i).IsSameAs(varName.c_str()))
            {
               mUserVarListBox->SetSelection(i);
               break;
            }
         }
         
         theOkButton->Enable();
         //loj: once variable is created cannot revert (delete) for now
         theCancelButton->Disable();
      }
      else
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "ParameterCreateDialog::CreateVariable()\nThe variable: %s"
             " cannot be created. It already exists.", varName.c_str());
      }

      mCreateVariable = false;
      mCreateVariableButton->Disable();
      mExprTextCtrl->SetValue("");
      mVarNameTextCtrl->SetValue("");
   }
}

//------------------------------------------------------------------------------
// void CreateArray()
//------------------------------------------------------------------------------
void ParameterCreateDialog::CreateArray()
{
   long row, col;

   if (!(mArrRowTextCtrl->GetValue().ToLong(&row)) ||
       !(mArrColTextCtrl->GetValue().ToLong(&col)))
   {
      wxLogError(wxT("Row or Column is not a number"));
      wxLog::FlushActive();
   }
   else
   {
      std::string arrName = std::string(mArrNameTextCtrl->GetValue().c_str());
      
      // if new user array to create
      if (theGuiInterpreter->GetParameter(arrName) == NULL)
      {
         Parameter *param;
         
         param = theGuiInterpreter->CreateParameter("Array", arrName);
         param->SetIntegerParameter("NumRows", row);
         param->SetIntegerParameter("NumCols", col);
         
         mParamNames.Add(arrName.c_str());
         mIsParamCreated = true;
         theGuiManager->UpdateParameter();

         mUserArrayListBox->Append(arrName.c_str());

         for (int i=0; i<mUserArrayListBox->GetCount(); i++)
         {
            if (mUserArrayListBox->GetString(i).IsSameAs(arrName.c_str()))
            {
               mUserArrayListBox->SetSelection(i);
               break;
            }
         }
         
         theOkButton->Enable();
         //loj: once array is created cannot revert (delete) for now
         theCancelButton->Disable();
      }
      else
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "ParameterCreateDialog::CreateArray()\nThe array: %s"
             " cannot be created. It already exists.", arrName.c_str());
      }
      
      mCreateArray = false;
      mCreateArrayButton->Disable();
      mArrNameTextCtrl->SetValue("");
      mArrRowTextCtrl->SetValue("");
      mArrColTextCtrl->SetValue("");
   }
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ParameterCreateDialog::SaveData()
{
   if (mCreateVariable)
      CreateVariable();
   else if (mCreateArray)
      CreateArray();
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
      ("ParameterCreateDialog::OnOk() mCreateVariableButton->IsEnabled()=%d\n",
       mCreateVariableButton->IsEnabled());
#endif
   
   if (mCreateVariableButton->IsEnabled())
      mCreateVariable = true;
   else if (mCreateArrayButton->IsEnabled())
      mCreateArray = true;
   
   SaveData();
   Close();
}

//---------------------------------
// event handling
//---------------------------------

//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnTextUpdate(wxCommandEvent& event)
{
   if (mVarNameTextCtrl->GetValue().Trim() != "" &&
       mVarNameTextCtrl->GetValue().Trim() != " " &&
       mExprTextCtrl->GetValue().Trim() != "")
   {
      mCreateVariableButton->Enable();
      theOkButton->Enable();
   }
   else if (mArrNameTextCtrl->GetValue().Trim() != "" &&
            mArrRowTextCtrl->GetValue().Trim() != "" &&
            mArrColTextCtrl->GetValue().Trim() != "")
   {
      mCreateArrayButton->Enable();
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
   if (event.GetEventObject() == mCreateVariableButton)  
   {
      mCreateVariable = true;
      mCreateArray = false;
      SaveData();
   }
   else if (event.GetEventObject() == mCreateArrayButton)  
   {
      mCreateArray = true;
      mCreateVariable = false;
      SaveData();
   }
   else if (event.GetEventObject() == mPastePropertyButton)  
   {
      wxString s = mObjectListBox->GetStringSelection() + "." +
         mPropertyListBox->GetStringSelection();

      mExprTextCtrl->AppendText(s);
      //mVarNameTextCtrl->SetValue(s); //loj: 9/23/04

      if (mVarNameTextCtrl->GetValue() != "" &&
          mVarNameTextCtrl->GetValue() != " ")
      {
         mCreateVariableButton->Enable();
         theOkButton->Enable();
      }
   }
   else if (event.GetEventObject() == mPasteUserVarButton)  
   {
      mExprTextCtrl->AppendText(mUserVarListBox->GetStringSelection());
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

