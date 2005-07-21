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

//#define DEBUG_PARAM_CREATE_DIALOG 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ParameterCreateDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, ParameterCreateDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, ParameterCreateDialog::OnButton)
   EVT_BUTTON(ID_COLOR_BUTTON, ParameterCreateDialog::OnColorButtonClick)
   EVT_COMBOBOX(ID_COMBO, ParameterCreateDialog::OnComboBoxChange)
   EVT_LISTBOX(ID_PROPERTY_LISTBOX, ParameterCreateDialog::OnSelectProperty)
   EVT_TEXT(ID_TEXTCTRL, ParameterCreateDialog::OnTextUpdate)
END_EVENT_TABLE()

   
//------------------------------------------------------------------------------
// ParameterCreateDialog(wxWindow *parent)
//------------------------------------------------------------------------------
ParameterCreateDialog::ParameterCreateDialog(wxWindow *parent)
   : GmatDialog(parent, -1, wxString(_T("ParameterCreateDialog")))
{
   mCurrParam = NULL;
   mParamNames.Clear();
   mIsParamCreated = false;
   mCreateVariable = false;
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
      theGuiManager->GetSpacecraftListBox(this, -1, wxSize(135, 85), &mExcludedScList);
   
   mPropertyListBox = 
      theGuiManager->GetPropertyListBox(this, ID_PROPERTY_LISTBOX,
                                        wxSize(135, 85), "Spacecraft");
   
   mUserVarListBox =
      theGuiManager->GetUserVariableListBox(this, -1, wxSize(170, 85), "");
   
   mUserArrayListBox =
      theGuiManager->GetUserArrayListBox(this, -1, wxSize(170, 50), "");
   

   // wxComboBox
   mCoordSysComboBox = theGuiManager->GetCoordSysComboBox(this, ID_COMBO, wxSize(120,-1));
   
   // wxComboBox
   mCentralBodyComboBox = theGuiManager->GetConfigBodyComboBox(this, ID_COMBO, wxSize(100,-1));
   //mCentralBodyComboBox->SetStringSelection("Earth"); // Earth is the default
   
   // wxSizers
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   wxFlexGridSizer *top1FlexGridSizer = new wxFlexGridSizer(3, 0, 0);
   wxFlexGridSizer *objPropertyFlexGridSizer = new wxFlexGridSizer(4, 0, 0);
   mDetailsBoxSizer = new wxBoxSizer(wxHORIZONTAL);   
   
   wxFlexGridSizer *arr1FlexGridSizer = new wxFlexGridSizer(7, 0, 0);
   
   wxStaticBox *variableStaticBox = new wxStaticBox(this, -1, wxT("Variable"));
   wxStaticBoxSizer *variableStaticBoxSizer =
      new wxStaticBoxSizer(variableStaticBox, wxVERTICAL);
   
   wxStaticBox *arrayStaticBox = new wxStaticBox(this, -1, wxT("Array"));
   wxStaticBoxSizer *arrayStaticBoxSizer =
      new wxStaticBoxSizer(arrayStaticBox, wxVERTICAL);

   
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

   //-------------------------------------------------------
   // for String Creation
   //-------------------------------------------------------
   wxStaticText *stringNameLabel =
      new wxStaticText(this, ID_TEXT, wxT("Name"),
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *stringValueLabel =
      new wxStaticText(this, ID_TEXT, wxT("Value"),
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *configStringLabel =
      new wxStaticText(this, ID_TEXT, wxT("Strings"),
                        wxDefaultPosition, wxDefaultSize, 0);
   
   mStringNameTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                        wxDefaultPosition, wxSize(80,20), 0);
   mStringValueTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                     wxDefaultPosition, wxSize(110,20), 0);
   mCreateStringButton = new wxButton(this, ID_BUTTON, wxT("Create"),
                                      wxDefaultPosition, wxDefaultSize, 0);

   mUserStringListBox =
      theGuiManager->GetUserStringListBox(this, -1, wxSize(170, 50), "");
   
   wxStaticBox *stringStaticBox = new wxStaticBox(this, -1, wxT("String"));
   wxStaticBoxSizer *stringStaticBoxSizer =
      new wxStaticBoxSizer(stringStaticBox, wxHORIZONTAL);
   
   wxFlexGridSizer *stringFlexGridSizer = new wxFlexGridSizer(5, 0, 0);

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
   pageBoxSizer->Add(stringStaticBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageBoxSizer->Add(arrayStaticBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-------------------------------------------------------
   // add to parent sizer
   //-------------------------------------------------------
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);

}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ParameterCreateDialog::LoadData()
{
   mCreateVariableButton->Disable();
   mCreateStringButton->Disable();
   mCreateArrayButton->Disable();
   mPropertyListBox->SetSelection(0);

   // show coordinate system or central body
   ShowCoordSystem();
   
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ParameterCreateDialog::SaveData()
{
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
}

//------------------------------------------------------------------------------
// void OnOK()
//------------------------------------------------------------------------------
/**
 * Saves the data and closes the page
 */
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnOK(wxCommandEvent &event)
{
   #if DEBUG_PARAM_CREATE_DIALOG
   MessageInterface::ShowMessage
      ("ParameterCreateDialog::OnOk() mCreateVariableButton->IsEnabled()=%d\n",
       mCreateVariableButton->IsEnabled());
   #endif
   
   if (mCreateVariableButton->IsEnabled())
      mCreateVariable = true;
   
   if (mCreateStringButton->IsEnabled())
      mCreateString = true;
   
   if (mCreateArrayButton->IsEnabled())
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
   mCreateVariableButton->Disable();
   mCreateStringButton->Disable();
   mCreateArrayButton->Disable();

   if (mVarNameTextCtrl->GetValue().Trim() != "" &&
       mVarNameTextCtrl->GetValue().Trim() != " " &&
       mExprTextCtrl->GetValue().Trim() != "")
   {
      mCreateVariableButton->Enable();
      theOkButton->Enable();
   }
   
   if (mStringNameTextCtrl->GetValue().Trim() != "" )
   {
      mCreateStringButton->Enable();
      theOkButton->Enable();
   }
   
   if (mArrNameTextCtrl->GetValue().Trim() != "" &&
       mArrRowTextCtrl->GetValue().Trim() != "" &&
       mArrColTextCtrl->GetValue().Trim() != "")
   {
      mCreateArrayButton->Enable();
      theOkButton->Enable();
   }

}

//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnComboBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mCoordSysComboBox)
   {
   }
   else if (event.GetEventObject() == mCentralBodyComboBox)
   {
      mCurrParam->SetStringParameter
         ("DepObject", std::string(mCentralBodyComboBox->GetStringSelection().c_str()));
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
   // get parameter pointer
   wxString paramName = GetParamName();
   
   // get parameter pointer
   mCurrParam = CreateParameter(paramName);
   
   if (mCurrParam->IsCoordSysDependent())
   {
      mCoordSysLabel->Show();
      mCoordSysLabel->SetLabel("Coordinate System");
      mCoordSysComboBox->SetStringSelection
         (mCurrParam->GetStringParameter("DepObject").c_str());
      mCoordSysComboBox->Show();
      mCentralBodyComboBox->Hide();
      mDetailsBoxSizer->Remove(mCentralBodyComboBox);
      mDetailsBoxSizer->Layout();
   }
   else if (mCurrParam->IsOriginDependent())
   {
      mCoordSysLabel->Show();
      mCoordSysLabel->SetLabel("Central Body");
      mCentralBodyComboBox->SetStringSelection
         (mCurrParam->GetStringParameter("DepObject").c_str());
      mCentralBodyComboBox->Show();
      mCoordSysComboBox->Hide();
      mDetailsBoxSizer->Remove(mCoordSysComboBox);
      mDetailsBoxSizer->Layout();
   }
   else
   {
      mCoordSysLabel->Hide();
      mCoordSysComboBox->Hide();
      mCentralBodyComboBox->Hide();
      mDetailsBoxSizer->Layout();
   }
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
   std::string paramName(name.c_str());
   std::string ownerName(mObjectListBox->GetStringSelection().c_str());
   std::string propName(mPropertyListBox->GetStringSelection().c_str());
   std::string depObjName = "";

   if (mCentralBodyComboBox->IsShown())
   {
      depObjName = std::string(mCentralBodyComboBox->GetStringSelection().c_str());
   }
   // handle CoordinateSystem later, it is not ready yet.
   //else if (mCoordSysComboBox->IsShown())
   //{
   //}
   
   Parameter *param = theGuiInterpreter->GetParameter(paramName);

   // create a parameter if it does not exist
   if (param == NULL)
   {
      param = theGuiInterpreter->CreateParameter(propName, paramName);
      param->SetRefObjectName(Gmat::SPACECRAFT, ownerName);
      
      if (depObjName != "")
         param->SetStringParameter("DepObject", depObjName);
   }

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
   
   wxString paramName = mObjectListBox->GetStringSelection() + "." + depObj + "." +
      mPropertyListBox->GetStringSelection();
   
   return paramName;
}

//------------------------------------------------------------------------------
// void CreateVariable()
//------------------------------------------------------------------------------
void ParameterCreateDialog::CreateVariable()
{
   wxString wxvarName = mVarNameTextCtrl->GetValue().Trim();
   std::string varName = std::string(wxvarName.c_str());
   wxString wxvarExpr = mExprTextCtrl->GetValue().Trim();
   std::string varExpr = std::string(wxvarExpr.c_str());
   
   #if DEBUG_PARAM_CREATE_DIALOG
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
         Real realNum;
         
         param = theGuiInterpreter->CreateParameter("Variable", varName);
         
         param->SetStringParameter("Expression", varExpr);
         
         //loj: 7/15/05 Added check for a number
         // If expresstion is not a number
         if (!wxvarExpr.ToDouble(&realNum))
         {
            // Parse the Parameter
            StringTokenizer st(varExpr, "()*/+-^ ");
            StringArray tokens = st.GetAllTokens();
            
            for (unsigned int i=0; i<tokens.size(); i++)
            {
               #if DEBUG_PARAM_CREATE_DIALOG
               MessageInterface::ShowMessage("token:<%s> \n", tokens[i].c_str());
               #endif
               
               //loj: 7/15/05 Added check for a number
               //if (!isdigit(*tokens[i].c_str()))
               wxString wxtoken = tokens[i].c_str();
               if (!wxtoken.ToDouble(&realNum))
               {
                  // create system parameter if it is NULL
                  if (theGuiInterpreter->GetParameter(tokens[i]) == NULL)
                  {
                     //find objName.depObj.typeName
                     std::string::size_type pos1 = tokens[i].find(".");
                     std::string::size_type pos2 = tokens[i].find_last_of(".");
                     if (pos1 != tokens[i].npos && pos2 != tokens[i].npos)
                     {
                        std::string objName = tokens[i].substr(0, pos1);
                        std::string typeName = tokens[i].substr(pos2+1, tokens[i].npos-pos2);
                        
                        Parameter *sysParam =
                           theGuiInterpreter->CreateParameter(typeName, tokens[i]);
                        sysParam->SetRefObjectName(Gmat::SPACECRAFT, objName);
                        
                        // set dependent object name
                        if (pos2 > pos1)
                        {
                           std::string depObjName = tokens[i].substr(pos1+1, pos2);
                           sysParam->SetStringParameter("DepObject", depObjName);
                        }
                     }
                  }
                  
                  param->SetRefObjectName(Gmat::PARAMETER, tokens[i]);
                  
               }
            }
         }
         
         
         RgbColor color(mColor.Red(), mColor.Green(), mColor.Blue());
         param->SetUnsignedIntParameter("Color", color.GetIntColor());
         
         #if DEBUG_PARAM_CREATE_DIALOG
         MessageInterface::ShowMessage
            ("ParameterCreateDialog::CreateVariable() user var:%s added\n",
             varName.c_str());
         #endif
         
         mParamNames.Add(varName.c_str());
         mIsParamCreated = true;
         theGuiManager->UpdateParameter();
         
         GmatAppData::GetResourceTree()->UpdateVariable();
         mUserVarListBox->Append(varName.c_str());
         
         for (int i=0; i<mUserVarListBox->GetCount(); i++)
         {
            if (mUserVarListBox->GetString(i).IsSameAs(varName.c_str()))
            {
               mUserVarListBox->SetSelection(i);
               break;
            }
         }
         
         //isVarCreated = true;
         // once variable is created cannot revert (delete) for now
         theOkButton->Enable();
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
// void CreateString()
//------------------------------------------------------------------------------
void ParameterCreateDialog::CreateString()
{
   wxString wxstrName = mStringNameTextCtrl->GetValue().Trim();
   std::string strName = std::string(wxstrName);
   std::string strValue = std::string(mStringValueTextCtrl->GetValue().c_str());
   
   // if new user string to create
   if (theGuiInterpreter->GetParameter(strName) == NULL)
   {
      Parameter *param;
      
      param = theGuiInterpreter->CreateParameter("String", strName);
      param->SetStringParameter("Expression", strValue);
      
      mParamNames.Add(strName.c_str());
      mIsParamCreated = true;
      theGuiManager->UpdateParameter();
      
      GmatAppData::GetResourceTree()->UpdateVariable(); //loj: 7/21/05 Added
      mUserStringListBox->Append(strName.c_str());
      
      for (int i=0; i<mUserStringListBox->GetCount(); i++)
      {
         if (mUserStringListBox->GetString(i).IsSameAs(strName.c_str()))
         {
            mUserStringListBox->SetSelection(i);
            break;
         }
      }

      // once string is created cannot revert.
      theOkButton->Enable();
      theCancelButton->Disable();
      
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

         GmatAppData::GetResourceTree()->UpdateVariable(); //loj: 7/21/05 Added
         mUserArrayListBox->Append(arrName.c_str());

         for (int i=0; i<mUserArrayListBox->GetCount(); i++)
         {
            if (mUserArrayListBox->GetString(i).IsSameAs(arrName.c_str()))
            {
               mUserArrayListBox->SetSelection(i);
               break;
            }
         }

         // once array is created cannot revert (delete) for now
         theOkButton->Enable();
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

