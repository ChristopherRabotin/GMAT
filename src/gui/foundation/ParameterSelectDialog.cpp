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

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ParameterSelectDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, ParameterSelectDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_COMBOBOX(ID_COMBOBOX, ParameterSelectDialog::OnComboBoxChange)
   EVT_BUTTON(ADD_VAR_BUTTON, ParameterSelectDialog::OnAddVariable)
   EVT_BUTTON(CREATE_VARIABLE, ParameterSelectDialog::OnCreateVariable)
   EVT_LISTBOX(USER_PARAM_LISTBOX, ParameterSelectDialog::OnSelectUserParam)
   EVT_LISTBOX(PROPERTY_LISTBOX, ParameterSelectDialog::OnSelectProperty)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ParameterSelectDialog(wxWindow *parent, bool showArray = false)
//------------------------------------------------------------------------------
ParameterSelectDialog::ParameterSelectDialog(wxWindow *parent, bool showArray)
   : GmatDialog(parent, -1, wxString(_T("ParameterSelectDialog")))
{
   mParamName = "";
   mIsParamSelected = false;
   mCanClose = true;
   mUseUserParam = false;
   mShowArray = showArray;
   
   Create();
   ShowData();
}

//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void ParameterSelectDialog::Create()
{
   int bsize = 2;
   wxString emptyList[] = {};

   //------------------------------------------------------
   // available variables list (1st column)
   //------------------------------------------------------
   mVarBoxSizer = new wxBoxSizer(wxVERTICAL);
    
   wxButton *createVarButton;

   mParamBoxSizer =
      theGuiManager->CreateParameterSizer(this, &createVarButton, CREATE_VARIABLE,
                                          &mObjectComboBox, ID_COMBOBOX,
                                          &mUserParamListBox, USER_PARAM_LISTBOX,
                                          &mPropertyListBox, PROPERTY_LISTBOX,
                                          &mCoordSysComboBox, ID_COMBOBOX,
                                          &mCentralBodyComboBox, ID_COMBOBOX,
                                          &mCoordSysLabel, &mCoordSysSizer);
   
   //-------------------------------------------------------
   // add parameter button (2nd column)
   //-------------------------------------------------------
   mAddParamButton = new wxButton(this, ADD_VAR_BUTTON, wxT("-->"),
                                  wxDefaultPosition, wxSize(20,20), 0 );
   
   
   //-------------------------------------------------------
   // selected parameter list (3rd column)
   //-------------------------------------------------------
   wxStaticBox *selectedStaticBox = new wxStaticBox( this, -1, wxT("") );

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
   wxFlexGridSizer *mFlexGridSizer = new wxFlexGridSizer(5, 0, 0);
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);

   mFlexGridSizer->Add(mParamBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(mAddParamButton, 0, wxALIGN_CENTRE|wxALL, bsize);
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
   mUserParamListBox->Deselect(mUserParamListBox->GetSelection());
   mObjectComboBox->SetSelection(0);
   mPropertyListBox->SetSelection(0);
   
   // show coordinate system or central body
   ShowCoordSystem();
}

//------------------------------------------------------------------------------
// virtual void OnOK()
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnOK()
{
   SaveData();
   
   if (mCanClose)
      Close();
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ParameterSelectDialog::SaveData()
{
   mCanClose = true;
   
   //loj: Can we select multiple parameters? If so we need to use wxArrayString
   // for build2, just assume one parameter selection
   if (mVarSelectedListBox->GetCount() > 0)
   {
      mParamName = mVarSelectedListBox->GetString(0); // first selection
      std::string paramName(mParamName.c_str());
      std::string objName(mObjectComboBox->GetStringSelection().c_str());
      std::string propName(mPropertyListBox->GetStringSelection().c_str());

      // create parameter if it is new
      Parameter *param = theGuiInterpreter->GetParameter(paramName);

      if (param == NULL)
      {
         param = theGuiInterpreter->CreateParameter(propName, paramName);
         param->SetRefObjectName(Gmat::SPACECRAFT, objName);
      }

      // check if pararameter is returning single value (plottable)

      if (param->IsPlottable())
      {
         mIsParamSelected = true;
      }
      else
      {
         if (mShowArray)
         {
            mIsParamSelected = true;
         }
         else
         {
            wxLogMessage("Selected parameter:%s is not returning single value. "
                         "Please select another parameter\n", paramName.c_str());
         
            mIsParamSelected = false;
            mCanClose = false;
         }
      }
   }
   else
   {
      mIsParamSelected = false;
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
// void OnAddVariable(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnAddVariable(wxCommandEvent& event)
{
   // empty listbox first, only one parameter is allowed
   mVarSelectedListBox->Clear();

   // get string in first list and then search for it in the second list
   wxString newParam = FormParamName();
   
   // Create a paramete if it does not exist
   GetParameter(newParam);
   
   mVarSelectedListBox->Append(newParam);
   mVarSelectedListBox->SetStringSelection(newParam);

   theOkButton->Enable();
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
      mUserParamListBox->Set(theGuiManager->GetNumUserVariable(),
                             theGuiManager->GetUserVariableList());
   }
}

//------------------------------------------------------------------------------
// void OnSelectUserParam(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnSelectUserParam(wxCommandEvent& event)
{
   // deselect property
   mPropertyListBox->Deselect(mPropertyListBox->GetSelection());

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
      
      if (param->NeedCoordSystem())
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
