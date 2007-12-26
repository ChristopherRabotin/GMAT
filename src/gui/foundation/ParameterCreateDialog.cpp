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
#include "GmatStaticBoxSizer.hpp"
#include "RgbColor.hpp"
#include "ParameterInfo.hpp"            // for GetDepObjectType()
#include "MessageInterface.hpp"
#include "StringTokenizer.hpp"

#include "wx/colordlg.h"   // for wxColourDialog

//#define DEBUG_PARAM_CREATE_DIALOG 1
//#define DEBUG_PARAM_CREATE_VAR 1
//#define DEBUG_PARAM_CREATE_SAVE 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ParameterCreateDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON, ParameterCreateDialog::OnButton)
   EVT_BUTTON(ID_COLOR_BUTTON, ParameterCreateDialog::OnColorButtonClick)
   EVT_COMBOBOX(ID_COMBO, ParameterCreateDialog::OnComboBoxChange)
   EVT_LISTBOX(ID_PROPERTY_LISTBOX, ParameterCreateDialog::OnSelectProperty)
   EVT_TEXT(ID_TEXTCTRL, ParameterCreateDialog::OnTextUpdate)
END_EVENT_TABLE()

   
//------------------------------------------------------------------------------
// ParameterCreateDialog(wxWindow *parent, int paramType)
//------------------------------------------------------------------------------
/*
 * @param paramType 1 = Variable, 2 = Array, 3 = String
 */
//------------------------------------------------------------------------------
ParameterCreateDialog::ParameterCreateDialog(wxWindow *parent, int paramType)
   : GmatDialog(parent, -1, wxString(_T("ParameterCreateDialog")))
{
   mParamType = paramType;
   mCurrParam = NULL;
   mParamNames.Clear();
   mIsParamCreated = false;
   mCreateVariable = false;
   mCreateString  = false;
   mCreateArray = false;
   mColor.Set(0, 0, 0); // initialize to black
   
   Create();
   ShowData();
}


//------------------------------------------------------------------------------
// ~ParameterCreateDialog()
//------------------------------------------------------------------------------
ParameterCreateDialog::~ParameterCreateDialog()
{
   #if DEBUG_GUI_ITEM_UNREG
   MessageInterface::ShowMessage
      ("ParameterCreateDialog::~ParameterCreateDialog() Unregister Spacecraft:%d\n",
       mObjectListBox);
   #endif

   // Unregister GUI components
   theGuiManager->UnregisterListBox("Spacecraft", mObjectListBox, &mExcludedScList);
   theGuiManager->UnregisterComboBox("CoordinateSystem", mCoordSysComboBox);
}


//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void ParameterCreateDialog::Create()
{
   #if DEBUG_PARAM_CREATE_DIALOG
   MessageInterface::ShowMessage("ParameterCreateDialog::Create() entered\n");
   #endif
   
   int bsize = 2;
   
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
   mCoordSysLabel =
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
      wxStaticText *stringNameLabel =
      new wxStaticText(this, ID_TEXT, wxT("Name"),
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *stringValueLabel =
      new wxStaticText(this, ID_TEXT, wxT("Value"),
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *configStringLabel =
      new wxStaticText(this, ID_TEXT, wxT("Strings"),
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
   mStringNameTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                        wxDefaultPosition, wxSize(80,20), 0);
   mStringValueTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                     wxDefaultPosition, wxSize(110,20), 0);
   
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
   mCreateStringButton = new wxButton(this, ID_BUTTON, wxT("Create"),
                                      wxDefaultPosition, wxDefaultSize, 0);
   
   //wxArrayString
   wxArrayString emptyArray;
   
   // wxListBox
   mObjectListBox = 
      theGuiManager->GetSpacecraftListBox(this, -1, wxSize(135, 85), &mExcludedScList);
   mPropertyListBox = 
      theGuiManager->GetPropertyListBox(this, ID_PROPERTY_LISTBOX, wxSize(135, 85), "Spacecraft");
   mUserVarListBox =
      theGuiManager->GetUserVariableListBox(this, -1, wxSize(170, 85), "");
   mUserArrayListBox =
      theGuiManager->GetUserArrayListBox(this, -1, wxSize(170, 50), "");
   mUserStringListBox =
      theGuiManager->GetUserStringListBox(this, -1, wxSize(170, 50), "");
          
   // wxComboBox
   mCoordSysComboBox = theGuiManager->GetCoordSysComboBox(this, ID_COMBO, wxSize(120,-1));
   mCentralBodyComboBox = theGuiManager->GetConfigBodyComboBox(this, ID_COMBO, wxSize(100,-1));
   //mCentralBodyComboBox->SetStringSelection("Earth"); // Earth is the default
   
   // wxSizers
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   mDetailsBoxSizer = new wxBoxSizer(wxHORIZONTAL);   
   
   wxFlexGridSizer *top1FlexGridSizer = new wxFlexGridSizer(3, 0, 0);
   wxFlexGridSizer *objPropertyFlexGridSizer = new wxFlexGridSizer(4, 0, 0);
   wxFlexGridSizer *arr1FlexGridSizer = new wxFlexGridSizer(7, 0, 0);
   wxFlexGridSizer *stringFlexGridSizer = new wxFlexGridSizer(5, 0, 0);
   
   GmatStaticBoxSizer *variableStaticBoxSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Variable");
   
   GmatStaticBoxSizer *arrayStaticBoxSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Array");
   
   GmatStaticBoxSizer *stringStaticBoxSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "String");
   
   // Add to wx*Sizers
   //-------------------------------------------------------
   // for Variable
   //-------------------------------------------------------
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
   
   mDetailsBoxSizer->Add(colorStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   mDetailsBoxSizer->Add(mColorButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   mDetailsBoxSizer->Add(30, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   mDetailsBoxSizer->Add(mCoordSysLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   mDetailsBoxSizer->Add(mCoordSysComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   mDetailsBoxSizer->Add(mCentralBodyComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   variableStaticBoxSizer->Add(top1FlexGridSizer, 0, wxALIGN_TOP|wxALL, bsize);
   variableStaticBoxSizer->Add(objPropertyFlexGridSizer, 0, wxALIGN_TOP|wxALL, bsize);
   variableStaticBoxSizer->Add(mDetailsBoxSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   stringFlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   stringFlexGridSizer->Add(stringNameLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   stringFlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   stringFlexGridSizer->Add(stringValueLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   stringFlexGridSizer->Add(configStringLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   stringFlexGridSizer->Add(mCreateStringButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   stringFlexGridSizer->Add(mStringNameTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   stringFlexGridSizer->Add(varEqualSignStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   stringFlexGridSizer->Add(mStringValueTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   stringFlexGridSizer->Add(mUserStringListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   stringStaticBoxSizer->Add(stringFlexGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-------------------------------------------------------
   // for Array Creation
   //-------------------------------------------------------
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
   pageBoxSizer->Add(stringStaticBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   if (mParamType == 1)
   {
      pageBoxSizer->Show(arrayStaticBoxSizer, false);
      pageBoxSizer->Show(stringStaticBoxSizer, false);
   }
   else if (mParamType == 2)
   {
      pageBoxSizer->Show(variableStaticBoxSizer, false);
      pageBoxSizer->Show(stringStaticBoxSizer, false);
   }
   else if (mParamType == 3)
   {
      pageBoxSizer->Show(variableStaticBoxSizer, false);
      pageBoxSizer->Show(arrayStaticBoxSizer, false);
   }
   
   
   //-------------------------------------------------------
   // add to parent sizer
   //-------------------------------------------------------
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
   
   #if DEBUG_PARAM_CREATE_DIALOG
   MessageInterface::ShowMessage("ParameterCreateDialog::Create() exiting\n");
   #endif
   
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ParameterCreateDialog::LoadData()
{
   #if DEBUG_PARAM_CREATE_DIALOG
   MessageInterface::ShowMessage("ParameterCreateDialog::LoadData() entering\n");
   #endif
   
   mCreateVariableButton->Disable();
   mCreateStringButton->Disable();
   mCreateArrayButton->Disable();
   mPropertyListBox->SetSelection(0);
   
   // show coordinate system or central body
   ShowCoordSystem();
   
   #if DEBUG_PARAM_CREATE_DIALOG
   MessageInterface::ShowMessage("ParameterCreateDialog::LoadData() exiting\n");
   #endif   
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ParameterCreateDialog::SaveData()
{
   if (mCreateVariableButton->IsEnabled())
      mCreateVariable = true;
   
   if (mCreateStringButton->IsEnabled())
      mCreateString = true;
   
   if (mCreateArrayButton->IsEnabled())
      mCreateArray = true;
   
   if (mCreateVariable || mCreateString || mCreateArray)
      EnableUpdate(true);
   
   canClose = true;
   
   #if DEBUG_PARAM_CREATE_SAVE
   MessageInterface::ShowMessage
      ("ParameterCreateDialog::SaveData() canClose=%d, mCreateVariable=%d, "
       "mCreateString=%d, mCreateArray=%d\n", canClose, mCreateVariable,
       mCreateString, mCreateArray);
   #endif
   
   if (mCreateVariable)
      CreateVariable();
   
   if (mCreateString)
      CreateString();
   
   if (mCreateArray)
      CreateArray();
      
}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void ParameterCreateDialog::ResetData()
{
   mIsParamCreated = false;
   mCreateVariable = false;
   mCreateString  = false;
   mCreateArray = false;
}


//---------------------------------
// event handling
//---------------------------------

//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnTextUpdate(wxCommandEvent& event)
{
   mCreateVariableButton->Disable();
   mCreateStringButton->Disable();
   mCreateArrayButton->Disable();

   if (mVarNameTextCtrl->GetValue().Trim() != "" &&
       mVarNameTextCtrl->GetValue().Trim() != " " &&
       mExprTextCtrl->GetValue().Trim() != "")
   {
      mCreateVariableButton->Enable();
      EnableUpdate(true);
   }
   
   if (mStringNameTextCtrl->GetValue().Trim() != "" )
   {
      mCreateStringButton->Enable();
      EnableUpdate(true);
   }
   
   if (mArrNameTextCtrl->GetValue().Trim() != "" &&
       mArrRowTextCtrl->GetValue().Trim() != "" &&
       mArrColTextCtrl->GetValue().Trim() != "")
   {
      mCreateArrayButton->Enable();
      EnableUpdate(true);
   }

}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnComboBoxChange(wxCommandEvent& event)
{
   //loj: 9/26/06
   // This needs to be updated to have ObjectType combobox like
   // in ParameterSelectDialog.
   
   if (event.GetEventObject() == mCoordSysComboBox)
   {
      //mCurrParam->SetStringParameter
      //   ("DepObject", std::string(mCoordSysComboBox->GetStringSelection().c_str()));
   }
   else if (event.GetEventObject() == mCentralBodyComboBox)
   {
      //mCurrParam->SetStringParameter
      //   ("DepObject", std::string(mCentralBodyComboBox->GetStringSelection().c_str()));
   }
}


//------------------------------------------------------------------------------
// void OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnButton(wxCommandEvent& event)
{    
   if (event.GetEventObject() == mCreateVariableButton)  
   {
      mCreateVariable = true;
      mCreateString = false;
      mCreateArray = false;
      SaveData();
   }
   else if (event.GetEventObject() == mCreateStringButton)  
   {
      mCreateVariable = false;
      mCreateString = true;
      mCreateArray = false;
      SaveData();
   }
   else if (event.GetEventObject() == mCreateArrayButton)  
   {
      mCreateArray = true;
      mCreateString = false;
      mCreateVariable = false;
      SaveData();
   }
   else if (event.GetEventObject() == mPastePropertyButton)  
   {
      wxString s = GetParamName();

      mExprTextCtrl->AppendText(s);

      if (mVarNameTextCtrl->GetValue() != "" &&
          mVarNameTextCtrl->GetValue() != " ")
      {
         mCreateVariableButton->Enable();
         EnableUpdate(true);
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

//------------------------------------------------------------------------------
// void OnSelectProperty(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnSelectProperty(wxCommandEvent& event)
{
   ShowCoordSystem();   
}


//------------------------------------------------------------------------------
// void ShowCoordSystem()
//------------------------------------------------------------------------------
void ParameterCreateDialog::ShowCoordSystem()
{
   std::string property = std::string(mPropertyListBox->GetStringSelection().c_str());
   GmatParam::DepObject depObj = ParameterInfo::Instance()->GetDepObjectType(property);
   
   //MessageInterface::ShowMessage
   //   ("===> ShowCoordSystem() property=%s, depObj=%d\n", property.c_str(), depObj);
   
   if (depObj == GmatParam::COORD_SYS)
   {
      mCoordSysLabel->Show();
      mCoordSysLabel->SetLabel("Coordinate System");
      mCoordSysComboBox->Show();
      mCentralBodyComboBox->Hide();
      mDetailsBoxSizer->Remove(mCentralBodyComboBox);
      mDetailsBoxSizer->Layout();
   }
   else if (depObj == GmatParam::ORIGIN)
   {
      mCoordSysLabel->Show();
      mCoordSysLabel->SetLabel("Central Body");
      mCentralBodyComboBox->Show();
      mCoordSysComboBox->Hide();
      mDetailsBoxSizer->Remove(mCoordSysComboBox);
      mDetailsBoxSizer->Layout();
      mDetailsBoxSizer->Layout();
   }
   else
   {
      mCoordSysLabel->Hide();
      mCoordSysComboBox->Hide();
      mCentralBodyComboBox->Hide();
      mDetailsBoxSizer->Layout();
      mDetailsBoxSizer->Layout();
   }
   
   #if DEBUG_PARAM_CREATE_DIALOG
   MessageInterface::ShowMessage("ParameterCreateDialog::ShowCoordSystem() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// Parameter* CreateParameter(const wxString &paramName)
//------------------------------------------------------------------------------
/*
 * @return newly created parameter pointer if it does not exist,
 *         return existing parameter pointer otherwise
 */
//------------------------------------------------------------------------------
Parameter* ParameterCreateDialog::CreateParameter(const wxString &name)
{
   #if DEBUG_PARAM_CREATE_DIALOG
   MessageInterface::ShowMessage
      ("ParameterCreateDialog::CreateParameter() name:%s\n", name.c_str());
   #endif
   
   std::string paramName(name.c_str());
   std::string ownerName(mObjectListBox->GetStringSelection().c_str());
   std::string propName(mPropertyListBox->GetStringSelection().c_str());
   std::string depObjName = "";

   if (mCentralBodyComboBox->IsShown())
   {
      depObjName = std::string(mCentralBodyComboBox->GetStringSelection().c_str());
   }
   else if (mCoordSysComboBox->IsShown())
   {
      depObjName = std::string(mCoordSysComboBox->GetStringSelection().c_str());
   }
   
   Parameter *param = theGuiInterpreter->GetParameter(paramName);
   
   // create a parameter if it does not exist
   if (param == NULL)
   {
      param = theGuiInterpreter->CreateParameter(propName, paramName);
      param->SetRefObjectName(Gmat::SPACECRAFT, ownerName);
      
      if (depObjName != "")
         param->SetStringParameter("DepObject", depObjName);
   }
   
   #if DEBUG_PARAM_CREATE_DIALOG
   MessageInterface::ShowMessage("ParameterCreateDialog::CreateParameter() exiting\n");
   #endif
   
   return param;
}


//------------------------------------------------------------------------------
// wxString GetParamName()
//------------------------------------------------------------------------------
wxString ParameterCreateDialog::GetParamName()
{
   wxString depObj = "";
   
   if (mCentralBodyComboBox->IsShown())
      depObj = mCentralBodyComboBox->GetStringSelection();
   else if (mCoordSysComboBox->IsShown())
      depObj = mCoordSysComboBox->GetStringSelection();
   
   wxString paramName;
   
   if (depObj == "")
   {
      paramName = mObjectListBox->GetStringSelection() + "." + 
         mPropertyListBox->GetStringSelection();
   }
   else
   {
      paramName = mObjectListBox->GetStringSelection() + "." + depObj + "." +
         mPropertyListBox->GetStringSelection();
   }
   
   return paramName;
}


//------------------------------------------------------------------------------
// void CreateVariable()
//------------------------------------------------------------------------------
/*
 * This method creates a variable after going through validation.
 */
//------------------------------------------------------------------------------
void ParameterCreateDialog::CreateVariable()
{
   wxString wxvarName = mVarNameTextCtrl->GetValue().Trim();
   std::string varName = std::string(wxvarName.c_str());
   wxString wxvarExpr = mExprTextCtrl->GetValue().Trim();
   std::string varExpr = std::string(wxvarExpr.c_str());
   Real realNum;
   bool isRealNumber = true;
   
   #if DEBUG_PARAM_CREATE_VAR
   MessageInterface::ShowMessage
      ("ParameterCreateDialog::CreateVariable() varName = "  + varName +
       " varExpr = " + varExpr + "\n");
   #endif
   
   
   // check if it has blank variable name or expression
   if (varName == "" || varExpr == "")
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Variable or expression cannot be blank");
      canClose = false;
      return;
   }
   
   // check if it has valid variable name
   if (!GmatStringUtil::IsValidName(varName))
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Invalid variable name: \"%s.\" Variable name must "
          "start with an alphabet letter", varName.c_str());
      canClose = false;
      return;
   }
   
   // check if rhs is a number
   if (!GmatStringUtil::ToReal(varExpr, realNum))
      isRealNumber = false;
   
   Parameter *param = NULL;
   
   // check if variable name already exist
   if (theGuiInterpreter->GetParameter(varName) != NULL)
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "The variable: %s cannot be created. It already exists.",
          varName.c_str());
      canClose = false;
      return;
   }

   try
   {
      // create a variable if rhs is a number
      if (isRealNumber)
      {
         param = theGuiInterpreter->CreateParameter("Variable", varName);  
         param->SetStringParameter("Expression", varExpr);
      }
      else
      {
         // Parse the Parameter
         StringTokenizer st(varExpr, "()*/+-^ ");
         StringArray tokens = st.GetAllTokens();
         StringArray paramArray;
         
         // Check if unexisting varibles used in expression
         for (unsigned int i=0; i<tokens.size(); i++)
         {
            #if DEBUG_PARAM_CREATE_VAR
            MessageInterface::ShowMessage("   token:<%s> \n", tokens[i].c_str());
            #endif
            
            if (!GmatStringUtil::ToReal(tokens[i], realNum))
            {
               // create system parameter if it is NULL
               if (theGuiInterpreter->GetParameter(tokens[i]) == NULL)
               {
                  // check if it is system parameter
                  std::string type, owner, depObj;
                  GmatStringUtil::ParseParameter(tokens[i], type, owner, depObj);
                  if (theGuiInterpreter->IsParameter(type))
                  {
                     #if DEBUG_PARAM_CREATE_VAR
                     MessageInterface::ShowMessage
                        ("type:%s is a system parameter\n", type.c_str());
                     #endif
                  
                     Parameter *sysParam = 
                        theGuiInterpreter->CreateParameter(type, tokens[i]);
                  
                     // set ref. object name
                     sysParam->SetRefObjectName(sysParam->GetOwnerType(), owner);
                  
                     // set dependent object name
                     if (depObj != "")
                        sysParam->SetStringParameter("DepObject", depObj);
                  
                  }
                  else
                  {
                     MessageInterface::PopupMessage
                        (Gmat::WARNING_, "The variable \"%s\" does not exist. "
                         "It must be created first.", tokens[i].c_str());
                     canClose = false;
                     return;
                  }
               }
            
               // create a variable
               param = theGuiInterpreter->CreateParameter("Variable", varName);
               param->SetStringParameter("Expression", varExpr);
            
               // set parameter names used in expression
               param->SetRefObjectName(Gmat::PARAMETER, tokens[i]);
            
            }
         }      
      }
   
   
      RgbColor color(mColor.Red(), mColor.Green(), mColor.Blue());
      param->SetUnsignedIntParameter("Color", color.GetIntColor());
      
      #if DEBUG_PARAM_CREATE_VAR
      MessageInterface::ShowMessage
         ("ParameterCreateDialog::CreateVariable() The variable \"%s\" added\n",
          varName.c_str());
      #endif
      
      mParamNames.Add(varName.c_str());
      mIsParamCreated = true;
      theGuiManager->UpdateParameter();
      
      GmatAppData::GetResourceTree()->UpdateVariable();
      mUserVarListBox->Append(varName.c_str());
      
      for (unsigned int i=0; i<mUserVarListBox->GetCount(); i++)
      {
         if (mUserVarListBox->GetString(i).IsSameAs(varName.c_str()))
         {
            mUserVarListBox->SetSelection(i);
            break;
         }
      }
      
      // reset values 
      mCreateVariable = false;
      mCreateVariableButton->Disable();
      mExprTextCtrl->SetValue("");
      mVarNameTextCtrl->SetValue("");
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void CreateString()
//------------------------------------------------------------------------------
void ParameterCreateDialog::CreateString()
{
   wxString wxstrName = mStringNameTextCtrl->GetValue().Trim();
   std::string strName = std::string(wxstrName);
   std::string strValue = std::string(mStringValueTextCtrl->GetValue().c_str());

   try
   {
      // if new user string to create
      if (theGuiInterpreter->GetParameter(strName) == NULL)
      {
         Parameter *param;
      
         param = theGuiInterpreter->CreateParameter("String", strName);
         param->SetStringParameter("Expression", strValue);
      
         mParamNames.Add(strName.c_str());
         mIsParamCreated = true;
         theGuiManager->UpdateParameter();
      
         GmatAppData::GetResourceTree()->UpdateVariable();
         mUserStringListBox->Append(strName.c_str());
      
         for (unsigned int i=0; i<mUserStringListBox->GetCount(); i++)
         {
            if (mUserStringListBox->GetString(i).IsSameAs(strName.c_str()))
            {
               mUserStringListBox->SetSelection(i);
               break;
            }
         }

         EnableUpdate(true);
      }
      else
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "ParameterCreateDialog::CreateString()\nThe string: %s"
             " cannot be created. It already exists.", strName.c_str());
      }

      mCreateString = false;
      mCreateStringButton->Disable();
      mStringNameTextCtrl->SetValue("");
      mStringValueTextCtrl->SetValue("");
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
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
      return;
   }

   try
   {
      wxString wxarrName = mArrNameTextCtrl->GetValue().Trim();
      std::string arrName = std::string(wxarrName.c_str());

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
         
         GmatAppData::GetResourceTree()->UpdateVariable();
         mUserArrayListBox->Append(arrName.c_str());
         
         for (unsigned int i=0; i<mUserArrayListBox->GetCount(); i++)
         {
            if (mUserArrayListBox->GetString(i).IsSameAs(arrName.c_str()))
            {
               mUserArrayListBox->SetSelection(i);
               break;
            }
         }
         
         EnableUpdate(true);
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
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}

