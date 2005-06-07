//$Header$
//------------------------------------------------------------------------------
//                              ParameterSelectDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Implements ParameterSelectDialog class. This class shows dialog window where a
 * user parameter can be selected.
 */
//------------------------------------------------------------------------------

#include "ParameterSelectDialog.hpp"
#include "ParameterCreateDialog.hpp"
#include "ParameterInfo.hpp"            // for GetDepObjectType()
#include "MessageInterface.hpp"

//#define DEBUG_PARAM_SELECT_DIALOG 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ParameterSelectDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, ParameterSelectDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_COMBOBOX(ID_COMBOBOX, ParameterSelectDialog::OnComboBoxChange)
   EVT_BUTTON(ID_BUTTON, ParameterSelectDialog::OnButtonClick)
   EVT_BUTTON(CREATE_VARIABLE, ParameterSelectDialog::OnCreateVariable)
   EVT_LISTBOX(USER_PARAM_LISTBOX, ParameterSelectDialog::OnSelectUserParam)
   EVT_LISTBOX(PROPERTY_LISTBOX, ParameterSelectDialog::OnSelectProperty)
END_EVENT_TABLE()

   
//------------------------------------------------------------------------------
// ParameterSelectDialog(wxWindow *parent,
//                       bool showArrayAndString = false,
//                       bool showSysVars = true,
//                       bool canSelectMultiVars = false,
//                       bool canSelectWholeObject = false,
//                       const wxString &ownerType = "Spacecraft")
//------------------------------------------------------------------------------
ParameterSelectDialog::ParameterSelectDialog(wxWindow *parent,
                                             bool showArrayAndString,
                                             bool showSysVars,
                                             bool canSelectMultiVars,
                                             bool canSelectWholeObject,
                                             const wxString &ownerType)
   : GmatDialog(parent, -1, wxString(_T("ParameterSelectDialog")))
{
   mIsParamSelected = false;
   mCanClose = true;
   mUseUserParam = false;
   mShowArrayAndString = showArrayAndString;
   mShowSysVars = showSysVars;
   mCanSelectMultiVars = canSelectMultiVars;
   mCanSelectWholeObject = canSelectWholeObject;
   mOwnerType = ownerType;
   
   mParamNameArray.Clear();

   Create();
   ShowData();
}


//------------------------------------------------------------------------------
// ~ParameterSelectDialog()
//------------------------------------------------------------------------------
ParameterSelectDialog::~ParameterSelectDialog()
{
   #if DEBUG_PARAM_SELECT_DIALOG
   MessageInterface::ShowMessage
      ("ParameterSelectDialog::~ParameterSelectDialog() Unregister Spacecraft:%d\n",
       mObjectComboBox);
   #endif
   
   theGuiManager->UnregisterComboBox("Spacecraft", mObjectComboBox);
   theGuiManager->UnregisterComboBox("CoordinateSystem", mCoordSysComboBox);
   
}


//------------------------------------------------------------------------------
//void SetParamNameArray(const wxArrayString &paramNames)
//------------------------------------------------------------------------------
void ParameterSelectDialog::SetParamNameArray(const wxArrayString &paramNames)
{
   mParamNameArray = paramNames;

   #if DEBUG_PARAM_SELECT_DIALOG
   MessageInterface::ShowMessage
      ("SetParamNameArray() param count=%d\n", mParamNameArray.GetCount());
   #endif
   
   // show selected parameter
   for (unsigned int i=0; i<mParamNameArray.GetCount(); i++)
   {
      mVarSelectedListBox->Append(mParamNameArray[i]);
      #if DEBUG_PARAM_SELECT_DIALOG
      MessageInterface::ShowMessage("param=%s\n", mParamNameArray[i].c_str());
      #endif
   }
}


//------------------------------------------------------------------------------
// virtual void OnOK()
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnOK(wxCommandEvent &event)
{
   SaveData();
   
   if (mCanClose)
      Close();
}


//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void ParameterSelectDialog::Create()
{
   #if DEBUG_PARAM_SELECT_DIALOG
   MessageInterface::ShowMessage("ParameterSelectDialog::Create() entered.\n");
   #endif
   
   int bsize = 2;
   wxString emptyList[] = {};
   
   //------------------------------------------------------
   // available variables list (1st column)
   //------------------------------------------------------
   mVarBoxSizer = new wxBoxSizer(wxVERTICAL);
   
   wxButton *createVarButton;
   
   if (mShowSysVars)
   {
      mParamBoxSizer = theGuiManager->
         CreateParameterSizer(this,
                              &mUserParamListBox, USER_PARAM_LISTBOX,
                              &createVarButton, CREATE_VARIABLE,
                              &mObjectComboBox, ID_COMBOBOX,
                              &mPropertyListBox, PROPERTY_LISTBOX,
                              &mCoordSysComboBox, ID_COMBOBOX,
                              &mCentralBodyComboBox, ID_COMBOBOX,
                              &mCoordSysLabel, &mCoordSysSizer,
                              mShowArrayAndString, mOwnerType);
   }
   else
   {
      mParamBoxSizer = theGuiManager->
         CreateUserVarSizer(this, &mUserParamListBox, USER_PARAM_LISTBOX,
                            &createVarButton, CREATE_VARIABLE, mShowArrayAndString);
   }
   
   //-------------------------------------------------------
   // add parameter button (2nd column)
   //-------------------------------------------------------
   mAddParamButton = new wxButton(this, ID_BUTTON, wxT("->"),
                                  wxDefaultPosition, wxSize(20,20), 0);
   
   mRemoveParamButton = new wxButton(this, ID_BUTTON, wxT("<-"),
                                      wxDefaultPosition, wxSize(20,20), 0);
   
   mRemoveAllParamButton = new wxButton(this, ID_BUTTON, wxT("<="),
                                        wxDefaultPosition, wxSize(20,20), 0);
   
   wxBoxSizer *arrowButtonsBoxSizer = new wxBoxSizer(wxVERTICAL);
   arrowButtonsBoxSizer->Add(mAddParamButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsBoxSizer->Add(mRemoveParamButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsBoxSizer->Add(mRemoveAllParamButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-------------------------------------------------------
   // selected parameter list (3rd column)
   //-------------------------------------------------------
   wxStaticBox *selectedStaticBox = new wxStaticBox(this, -1, wxT(""));

   wxStaticText *selectedParamLabel =
      new wxStaticText(this, -1, wxT("Selected"),
                       wxDefaultPosition, wxSize(80,-1), 0);
   
   mVarSelectedListBox = new wxListBox(this, VAR_SEL_LISTBOX, wxDefaultPosition,
                                       wxSize(170,260), 0, emptyList, wxLB_SINGLE);
   
   wxStaticBoxSizer *mVarSelectedBoxSizer =
      new wxStaticBoxSizer(selectedStaticBox, wxVERTICAL);

   mVarSelectedBoxSizer->Add(selectedParamLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   mVarSelectedBoxSizer->Add(mVarSelectedListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-------------------------------------------------------
   // put in the order
   //-------------------------------------------------------
   wxFlexGridSizer *mFlexGridSizer = new wxFlexGridSizer(3, 0, 0);
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);

   mFlexGridSizer->Add(mParamBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(arrowButtonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(mVarSelectedBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   pageBoxSizer->Add(mFlexGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);

}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ParameterSelectDialog::LoadData()
{   
   #if DEBUG_PARAM_SELECT_DIALOG
   MessageInterface::ShowMessage("ParameterSelectDialog::LoadData() entered.\n");
   #endif
   
   if (mShowSysVars)
   {
      mUserParamListBox->Deselect(mUserParamListBox->GetSelection());
      mObjectComboBox->SetSelection(0);
      mPropertyListBox->SetSelection(0);
      
      // show coordinate system or central body
      ShowCoordSystem();
   }
   else
   {
      mUserParamListBox->SetSelection(0);
      mUseUserParam = true;
   }
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ParameterSelectDialog::SaveData()
{
   mCanClose = true;
   mParamNameArray.Clear();
   mIsParamSelected = false;

   if (mVarSelectedListBox->GetCount() > 0)
   {
      mIsParamSelected = true;
      mParamName = mVarSelectedListBox->GetString(0);
      
      for(int i=0; i<mVarSelectedListBox->GetCount(); i++)
      {
         mParamNameArray.Add(mVarSelectedListBox->GetString(i));
      }
   }
}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void ParameterSelectDialog::ResetData()
{
   mIsParamSelected = false;
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mAddParamButton)
   {
      // if only one parameter is allowed
      if (!mCanSelectMultiVars)
         mVarSelectedListBox->Clear();

      // if whole object is selected and not property or user param
      if ((mCanSelectWholeObject) &&  (mShowSysVars) &&
          (mPropertyListBox->GetSelection() == -1) &&
          (mUserParamListBox->GetSelection() == -1))
      {
         wxString newParam = mObjectComboBox->GetStringSelection();
         int found = mVarSelectedListBox->FindString(newParam);

         // if the string wasn't found in the second list, insert it
         if (found == wxNOT_FOUND)
         {
            mVarSelectedListBox->Append(newParam);
            mVarSelectedListBox->SetStringSelection(newParam);
            theOkButton->Enable();
         }

         return;
      }

      // get string in first list
      wxString newParam = FormParamName();
      
      // Create a system paramete if it does not exist
      if (mShowSysVars)
         GetParameter(newParam);
      
      int found = mVarSelectedListBox->FindString(newParam);
      
      // if the string wasn't found in the second list, insert it
      if (found == wxNOT_FOUND)
      {
         mVarSelectedListBox->Append(newParam);
         mVarSelectedListBox->SetStringSelection(newParam);
         theOkButton->Enable();
      }
   }
   else if (event.GetEventObject() == mRemoveParamButton)
   {
      int sel = mVarSelectedListBox->GetSelection();
      mVarSelectedListBox->Delete(sel);

      if (sel-1 < 0)
         mVarSelectedListBox->SetSelection(0);
      else
         mVarSelectedListBox->SetSelection(sel-1);
      
      if (mVarSelectedListBox->GetCount() > 0)
         theOkButton->Enable();
      else
         theOkButton->Disable();
   }
   else if (event.GetEventObject() == mRemoveAllParamButton)
   {
      mVarSelectedListBox->Clear();
      theOkButton->Disable();
   }
}


//------------------------------------------------------------------------------
// void OnCreateVariable(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnCreateVariable(wxCommandEvent& event)
{
   ParameterCreateDialog paramDlg(this);
   paramDlg.ShowModal();

   if (paramDlg.IsParamCreated())
   {
      if (mShowArrayAndString)
      {
         mUserParamListBox->Set(theGuiManager->GetNumUserParameter(),
                                theGuiManager->GetUserParameterList());
      }
      else
      {
         mUserParamListBox->Set(theGuiManager->GetNumUserVariable(),
                                theGuiManager->GetUserVariableList());
      }
   }
}


//------------------------------------------------------------------------------
// void OnSelectUserParam(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnSelectUserParam(wxCommandEvent& event)
{
   if (mShowSysVars)
   {
      // deselect property
      mPropertyListBox->Deselect(mPropertyListBox->GetSelection());
   }
   
   mUseUserParam = true;
}


//------------------------------------------------------------------------------
// void OnSelectProperty(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnSelectProperty(wxCommandEvent& event)
{
   // deselect user param
   mUserParamListBox->Deselect(mUserParamListBox->GetSelection());

   // show coordinate system or central body
   ShowCoordSystem();

   mUseUserParam = false;
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnComboBoxChange(wxCommandEvent& event)
{    
   if (event.GetEventObject() == mObjectComboBox)
   {
      mPropertyListBox->Deselect(mPropertyListBox->GetSelection());
      mUseUserParam = false;
   }
}


//------------------------------------------------------------------------------
// wxString FormParamName()
//------------------------------------------------------------------------------
wxString ParameterSelectDialog::FormParamName()
{
   if (mUseUserParam)
   {
      return mUserParamListBox->GetStringSelection();
   }
   else
   {
      wxString depObj = "";
      
      if (mCoordSysComboBox->IsShown())
         depObj = mCoordSysComboBox->GetStringSelection();
      else if (mCentralBodyComboBox->IsShown())
         depObj = mCentralBodyComboBox->GetStringSelection();

      // error checking
//      if (mPropertyListBox->GetSelection == -1)
//      {
//         throw
//      }

      if (depObj == "")
         return mObjectComboBox->GetStringSelection() + "." + 
            mPropertyListBox->GetStringSelection();
      else
         return mObjectComboBox->GetStringSelection() + "." + depObj + "." +
            mPropertyListBox->GetStringSelection();
   }
}


//------------------------------------------------------------------------------
// Parameter* GetParameter(const wxString &name)
//------------------------------------------------------------------------------
/*
 * @return existing parameter pointer, return newly created parameter pointer
 *         if it does not exist.
 */
//------------------------------------------------------------------------------
Parameter* ParameterSelectDialog::GetParameter(const wxString &name)
{
   Parameter *param = theGuiInterpreter->GetParameter(std::string(name.c_str()));

   // create a parameter if it does not exist
   if (param == NULL)
   {
      #if DEBUG_PARAM_SELECT_DIALOG
      MessageInterface::ShowMessage
         ("ParameterSelectDialog::GetParameter() Creating parameter:<%s>\n",
          name.c_str());
      #endif
      
      std::string paramName(name.c_str());
      std::string objName(mObjectComboBox->GetStringSelection().c_str());
      std::string propName(mPropertyListBox->GetStringSelection().c_str());
      std::string depObjName = "";
      
      if (mCoordSysComboBox->IsShown())
         depObjName = std::string(mCoordSysComboBox->GetStringSelection().c_str());
      else if (mCentralBodyComboBox->IsShown())
         depObjName = std::string(mCentralBodyComboBox->GetStringSelection().c_str());
      
      param = theGuiInterpreter->CreateParameter(propName, paramName);
      param->SetRefObjectName(Gmat::SPACECRAFT, objName);
      
      if (depObjName != "")
         param->SetStringParameter("DepObject", depObjName);

      //loj: 2/10/05 Changed from NeedCoordSystem()
      if (param->IsCoordSysDependent())
         param->SetRefObjectName(Gmat::COORDINATE_SYSTEM, depObjName);
   }
   
   return param;
}


//------------------------------------------------------------------------------
// void ShowCoordSystem()
//------------------------------------------------------------------------------
void ParameterSelectDialog::ShowCoordSystem()
{
   std::string property = std::string(mPropertyListBox->GetStringSelection().c_str());
   GmatParam::DepObject depObj = ParameterInfo::Instance()->GetDepObjectType(property);

   if (depObj == GmatParam::COORD_SYS)
   {
      mCoordSysLabel->Show();
      mCoordSysLabel->SetLabel("Coordinate System");
      
      mCoordSysComboBox->SetSelection(0);
      
      mCoordSysSizer->Remove(mCoordSysComboBox);
      mCoordSysSizer->Remove(mCentralBodyComboBox);
      mCoordSysSizer->Add(mCoordSysComboBox);
      mCoordSysComboBox->Show();
      mCentralBodyComboBox->Hide();
      mParamBoxSizer->Layout();
   }
   else if (depObj == GmatParam::ORIGIN)
   {
      mCoordSysLabel->Show();
      mCoordSysLabel->SetLabel("Central Body");
      
      mCentralBodyComboBox->SetStringSelection("Earth");
      
      mCoordSysSizer->Remove(mCentralBodyComboBox);
      mCoordSysSizer->Remove(mCoordSysComboBox);
      mCoordSysSizer->Add(mCentralBodyComboBox);
      mCentralBodyComboBox->Show();
      mCoordSysComboBox->Hide();
      mParamBoxSizer->Layout();
   }
   else
   {
      mCoordSysSizer->Remove(mCentralBodyComboBox);
      mCoordSysSizer->Remove(mCoordSysComboBox);
      mCoordSysLabel->Hide();
      mCoordSysComboBox->Hide();
      mCentralBodyComboBox->Hide();
      mParamBoxSizer->Layout();
   }
}
