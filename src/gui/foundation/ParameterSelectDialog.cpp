//$Id$
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
   EVT_COMBOBOX(ID_COMBOBOX, ParameterSelectDialog::OnComboBoxChange)
   EVT_BUTTON(ID_BUTTON, ParameterSelectDialog::OnButtonClick)
   EVT_BUTTON(CREATE_VARIABLE, ParameterSelectDialog::OnCreateVariable)
   EVT_LISTBOX(USER_PARAM_LISTBOX, ParameterSelectDialog::OnSelectUserParam)
   EVT_LISTBOX(PROPERTY_LISTBOX, ParameterSelectDialog::OnSelectProperty)
   EVT_LISTBOX_DCLICK(USER_PARAM_LISTBOX, ParameterSelectDialog::OnDoubleClick)
   EVT_LISTBOX_DCLICK(PROPERTY_LISTBOX, ParameterSelectDialog::OnDoubleClick)
END_EVENT_TABLE()

   
//------------------------------------------------------------------------------
// ParameterSelectDialog(wxWindow *parent,
//                       const wxString &ownerType = "Spacecraft",
//                       int showOption = GuiItemManager::SHOW_PLOTTABLE,
//                       bool showVariable = true,
//                       bool showArray = false,
//                       bool showSysParams = true,
//                       bool canSelectMultiVars = false,
//                       bool canSelectWholeObject = false,
//                       bool createParam = true)
//------------------------------------------------------------------------------
ParameterSelectDialog::ParameterSelectDialog(wxWindow *parent,
                                             const wxArrayString &objectTypeList,
                                             const wxString &objectType,
                                             int showOption,
                                             bool showVariable,
                                             bool showArray,
                                             bool showSysVars,
                                             bool canSelectMultiVars,
                                             bool canSelectWholeObject,
                                             bool createParam)
   : GmatDialog(parent, -1, wxString(_T("ParameterSelectDialog")))
{
   mIsParamSelected = false;
   mCanClose = true;
   mUseUserParam = false;
   mObjectTypeList = objectTypeList;
   mObjectType = objectType;
   mShowOption = showOption;
   mShowVariable = showVariable;
   mShowArray = showArray;
   mShowSysVars = showSysVars;
   mCanSelectMultiVars = canSelectMultiVars;
   mCanSelectWholeObject = canSelectWholeObject;
   mCreateParam = createParam;
   mLastPropertySelection = wxNOT_FOUND;
   mLastUserParamSelection = wxNOT_FOUND;
   
   mParamNameArray.Clear();

   #if DEBUG_PARAM_SELECT_DIALOG
   MessageInterface::ShowMessage
      ("ParameterSelectDialog() mObjectType=%s, mShowOption=%d, mShowVariable=%d, "
       "mShowArray=%d, mShowSysVars=%d, mCanSelectMultiVars=%d, mCanSelectWholeObject=%d, "
       "mCreateParam=%d\n", mObjectType.c_str(), mShowOption, mShowVariable,
       mShowArray, mShowSysVars, mCanSelectMultiVars, mCanSelectWholeObject,
       mCreateParam);
   #endif
   
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
      ("ParameterSelectDialog::~ParameterSelectDialog() Unregister "
       "mSpacecraftComboBox:%d\n", mSpacecraftComboBox);
   #endif
   
   theGuiManager->UnregisterComboBox("Spacecraft", mSpacecraftComboBox);
   theGuiManager->UnregisterComboBox("ImpulsiveBurn", mImpBurnComboBox);
   theGuiManager->UnregisterComboBox("CoordinateSystem", mCoordSysComboBox);
   theGuiManager->UnregisterComboBox("SpacePoint", mCentralBodyComboBox);
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
// virtual void Create()
//------------------------------------------------------------------------------
void ParameterSelectDialog::Create()
{
   #if DEBUG_PARAM_SELECT_DIALOG
   MessageInterface::ShowMessage("ParameterSelectDialog::Create() entered.\n");
   #endif
   
   int bsize = 2;
   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;
   
   //------------------------------------------------------
   // available variables list (1st column)
   //------------------------------------------------------
   
   wxButton *createVarButton;
   
   if (mShowSysVars)
   {
      mParamBoxSizer = theGuiManager->
         CreateParameterSizer(this,
                              &mUserParamListBox, USER_PARAM_LISTBOX,
                              &createVarButton, CREATE_VARIABLE,
                              &mObjectTypeComboBox, ID_COMBOBOX,
                              &mSpacecraftComboBox, ID_COMBOBOX,
                              &mImpBurnComboBox, ID_COMBOBOX,
                              &mPropertyListBox, PROPERTY_LISTBOX,
                              &mCoordSysComboBox, ID_COMBOBOX,
                              &mCentralBodyComboBox, ID_COMBOBOX,
                              &mCoordSysLabel, &mCoordSysSizer,
                              mObjectTypeList, mShowOption, mShowVariable,
                              mShowArray, mObjectType);

      // If showing multiple object types
      if (mObjectTypeList.Count() > 1)
      {
         mSpacecraftList = theGuiManager->GetSpacecraftList();
         mImpBurnList = theGuiManager->GetImpulsiveBurnList();         
         mSpacecraftPropertyList =
            theGuiManager->GetPropertyList("Spacecraft", GuiItemManager::SHOW_PLOTTABLE);
         mImpBurnPropertyList =
            theGuiManager->GetPropertyList("ImpulsiveBurn", GuiItemManager::SHOW_PLOTTABLE);
         mNumSc = theGuiManager->GetNumSpacecraft();
         mNumImpBurn = theGuiManager->GetNumImpulsiveBurn();
         mNumScProperty = theGuiManager->GetNumProperty("Spacecraft");
         mNumImpBurnProperty = theGuiManager->GetNumProperty("ImpulsiveBurn");
         
         #if DEBUG_PARAM_SELECT_DIALOG
         MessageInterface::ShowMessage
            ("===> mNumSc=%d, mNumImpBurn=%d, mNumScProperty=%d, mNumImpBurnProperty=%d\n",
             mNumSc, mNumImpBurn, mNumScProperty, mNumImpBurnProperty);
         #endif
      }
   }
   else
   {
      mParamBoxSizer = theGuiManager->
         CreateUserVarSizer(this, &mUserParamListBox, USER_PARAM_LISTBOX,
                            &createVarButton, CREATE_VARIABLE,
                            mShowOption, mShowArray);
   }
   
   //-------------------------------------------------------
   // add parameter button (2nd column)
   //-------------------------------------------------------

   wxSize buttonSize(20, 20);
   
   #ifdef __WXMAC__
   buttonSize.Set(40, 20);
   #endif
   
   mAddParamButton = new wxButton(this, ID_BUTTON, wxT("->"),
                                  wxDefaultPosition, buttonSize, 0);
   
   mRemoveParamButton = new wxButton(this, ID_BUTTON, wxT("<-"),
                                      wxDefaultPosition, buttonSize, 0);
   
   mRemoveAllParamButton = new wxButton(this, ID_BUTTON, wxT("<="),
                                        wxDefaultPosition, buttonSize, 0);
   
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
   
   mVarSelectedListBox =
      new wxListBox(this, VAR_SEL_LISTBOX, wxDefaultPosition, wxSize(170,284), //0,
                    emptyList, wxLB_SINGLE);
   
   wxStaticBoxSizer *mVarSelectedBoxSizer =
      new wxStaticBoxSizer(selectedStaticBox, wxVERTICAL);
   
   mVarSelectedBoxSizer->Add(selectedParamLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   mVarSelectedBoxSizer->Add(mVarSelectedListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-------------------------------------------------------
   // put in the order
   //-------------------------------------------------------
   wxFlexGridSizer *mFlexGridSizer = new wxFlexGridSizer(3, 0, 0);
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);

   mFlexGridSizer->Add(mParamBoxSizer, 0, wxALIGN_TOP|wxALL, bsize);
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
      mSpacecraftComboBox->SetSelection(0);
      mPropertyListBox->SetSelection(0);

      if (mObjectType == "ImpulsiveBurn")
      {
         mCoordSysLabel->Hide();
         mCoordSysComboBox->SetValue("");
         mCentralBodyComboBox->SetValue("");
         mCoordSysComboBox->Hide();
         mCentralBodyComboBox->Hide();
      }
      else
      {
         mLastCoordSysName = mCoordSysComboBox->GetString(0);
      
         // show coordinate system or central body
         ShowCoordSystem();
      }
   }

   
   if (mShowVariable || mShowArray)
   {
      mUserParamListBox->SetSelection(0);
      mUseUserParam = true;
   }
   
   if (mShowSysVars && (mShowVariable || mShowArray))
   {
      mUserParamListBox->Deselect(mUserParamListBox->GetSelection());
      mUseUserParam = false;
   }
   
   #if DEBUG_PARAM_SELECT_DIALOG
   MessageInterface::ShowMessage("ParameterSelectDialog::LoadData() exiting.\n");
   #endif
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
      
      for(unsigned int i=0; i<mVarSelectedListBox->GetCount(); i++)
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
         
         wxComboBox *objComboBox = GetObjectComboBox();
         if (objComboBox == NULL)
            return;
         
         wxString newParam = objComboBox->GetValue();
         int found = mVarSelectedListBox->FindString(newParam);
         
         // if the string wasn't found in the second list, insert it
         if (found == wxNOT_FOUND)
         {
            mVarSelectedListBox->Append(newParam);
            mVarSelectedListBox->SetStringSelection(newParam);
            EnableUpdate(true);
         }
                  
         return;
      }
      
      // get string in first list
      wxString newParam = FormParamName();
      
      // if newParam is properly created
      if (newParam != "")
      {
         // Create a system paramete if it does not exist
         if (mShowSysVars && mCreateParam)
         {
            Parameter *param = GetParameter(newParam);
            if (param == NULL)
               return;
         }
         
         int found = mVarSelectedListBox->FindString(newParam);
         
         // if the string wasn't found in the second list, insert it
         if (found == wxNOT_FOUND)
         {
            mVarSelectedListBox->Append(newParam);
            mVarSelectedListBox->SetStringSelection(newParam);
            EnableUpdate(true);
         }
         
         //Show next parameter
         if (mUseUserParam)
            mUserParamListBox->SetSelection(mUserParamListBox->GetSelection() + 1);
         else
         {
            mPropertyListBox->SetSelection(mPropertyListBox->GetSelection() + 1);
            OnSelectProperty(event);
         }
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
      
   }
   else if (event.GetEventObject() == mRemoveAllParamButton)
   {
      mVarSelectedListBox->Clear();
   }
   
   EnableUpdate(true);
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
      if (mShowOption == GuiItemManager::SHOW_REPORTABLE && mShowArray)
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
   if (mUserParamListBox->GetSelection() == wxNOT_FOUND)
      return;
   
   if (mUserParamListBox->GetSelection() == mLastUserParamSelection)
   {
      mUserParamListBox->Deselect(mLastUserParamSelection);
      mLastUserParamSelection = wxNOT_FOUND;

      if (mPropertyListBox->GetSelection() == wxNOT_FOUND)
         HighlightObject(event, true);
   }
   else if (mShowSysVars)
   {
      HighlightObject(event, false);
      mLastUserParamSelection = mUserParamListBox->GetSelection();
      
      // deselect property
      mPropertyListBox->Deselect(mPropertyListBox->GetSelection());
      mLastPropertySelection = wxNOT_FOUND;
   
      mUseUserParam = true;
   }
}


//------------------------------------------------------------------------------
// void OnSelectProperty(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnSelectProperty(wxCommandEvent& event)
{
   #if DEBUG_PARAM_SELECT_DIALOG > 1
   MessageInterface::ShowMessage("OnSelectProperty() entered\n");
   #endif
   
   // if click on selected item, deselect
   if (mPropertyListBox->GetSelection() == mLastPropertySelection)
   {
      mLastPropertySelection = wxNOT_FOUND;
      
      if (mShowVariable || mShowArray)
      {
         mPropertyListBox->Deselect(mLastPropertySelection);
      
         if (mUserParamListBox->GetSelection() == wxNOT_FOUND)
            HighlightObject(event, true);
      }
   }
   else
   {
      HighlightObject(event, false);
      mLastPropertySelection = mPropertyListBox->GetSelection();

      if (mShowVariable || mShowArray)
      {
         // deselect user param
         mUserParamListBox->Deselect(mUserParamListBox->GetSelection());
         mLastUserParamSelection = wxNOT_FOUND;
      }
      
      // show coordinate system or central body
      ShowCoordSystem();
      
      mUseUserParam = false;
   }
}


//------------------------------------------------------------------------------
// void OnDoubleClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnDoubleClick(wxCommandEvent& event)
{
   if (!mCanSelectWholeObject)
      return;
   
   if (event.GetEventObject() == mPropertyListBox)
   {
      // deselect property
      mPropertyListBox->Deselect(mPropertyListBox->GetSelection());
      mLastPropertySelection = wxNOT_FOUND;
      
      if (mUserParamListBox->GetSelection() == wxNOT_FOUND)
         HighlightObject(event, true);
   }
   else if (event.GetEventObject() == mUserParamListBox)
   {
      // deselect variable
      mUserParamListBox->Deselect(mPropertyListBox->GetSelection());
      mLastUserParamSelection = wxNOT_FOUND;
      
      if (mPropertyListBox->GetSelection() == wxNOT_FOUND)
         HighlightObject(event, true);
   }
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnComboBoxChange(wxCommandEvent& event)
{
   wxObject *obj = event.GetEventObject();
   
   if (obj == mObjectTypeComboBox)
   {
      if (mObjectTypeComboBox->GetValue() == "Spacecraft")
      {
         // Show Spacecraft objects
         mParamBoxSizer->Show(mSpacecraftComboBox, true, true);
         mParamBoxSizer->Hide(mImpBurnComboBox, true);
         mParamBoxSizer->Layout();
         
         // Set Spacecraft property
         mPropertyListBox->Set(mSpacecraftPropertyList);
         mPropertyListBox->SetSelection(0);         
         ShowCoordSystem();
      }
      else if (mObjectTypeComboBox->GetValue() == "ImpulsiveBurn")
      {
         // Show ImpulsiveBurn objects
         mParamBoxSizer->Show(mImpBurnComboBox, true, true);
         mParamBoxSizer->Hide(mSpacecraftComboBox, true);
         mParamBoxSizer->Layout();
         
         // Set ImpulsiveBurn property
         mPropertyListBox->Set(mImpBurnPropertyList);
      }
   }
   else if (obj == mSpacecraftComboBox || obj == mImpBurnComboBox)
   {
      mPropertyListBox->SetSelection(0);
      mUseUserParam = false;
   }
   else if(obj == mCoordSysComboBox)
   {
      mLastCoordSysName = mCoordSysComboBox->GetValue();
   }
}


//------------------------------------------------------------------------------
// wxString FormParamName()
//------------------------------------------------------------------------------
wxString ParameterSelectDialog::FormParamName()
{   
   wxString paramName;
   
   if (mUseUserParam)
   {
      paramName = mUserParamListBox->GetStringSelection();
   }
   else
   {
      wxString depObj = "";

      // make sure property is selected
      if (mPropertyListBox->GetSelection() == wxNOT_FOUND)
      {
         wxLogMessage("A whole object cannot be selected.\n"
                      "Please select a property or a variable.");
         return "";
      }
      
      if (mCoordSysComboBox->IsShown())
         depObj = mCoordSysComboBox->GetValue();
      else if (mCentralBodyComboBox->IsShown())
         depObj = mCentralBodyComboBox->GetValue();
      
      wxComboBox *objComboBox = GetObjectComboBox();
      if (objComboBox == NULL)
         return "";
      
      if (depObj == "")
      {
         paramName = objComboBox->GetValue() + "." + 
            mPropertyListBox->GetStringSelection();
      }
      else
      {
         paramName = objComboBox->GetValue() + "." + depObj + "." +
            mPropertyListBox->GetStringSelection();
      }
   }

   #if DEBUG_PARAM_SELECT_DIALOG
   MessageInterface::ShowMessage
      ("ParameterSelectDialog::FormParamName() paramName=%s\n",
       paramName.c_str());
   #endif
   
   return paramName;
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
   Parameter *param = 
      theGuiInterpreter->GetParameter(std::string(name.c_str()));

   // create a parameter if it does not exist
   if (param == NULL)
   {
      #if DEBUG_PARAM_SELECT_DIALOG
      MessageInterface::ShowMessage
         ("ParameterSelectDialog::GetParameter() Creating parameter:<%s>\n",
          name.c_str());
      #endif
      
      wxComboBox *objComboBox = GetObjectComboBox();
      if (objComboBox == NULL)
         return NULL;
      
      std::string paramName(name.c_str());
      std::string objName(objComboBox->GetValue().c_str());
      std::string propName(mPropertyListBox->GetStringSelection().c_str());
      std::string depObjName = "";
      
      if (mCoordSysComboBox->IsShown())
         depObjName = std::string(mCoordSysComboBox->GetValue().c_str());
      else if (mCentralBodyComboBox->IsShown())
         depObjName = std::string(mCentralBodyComboBox->GetValue().c_str());
      
      param = theGuiInterpreter->CreateParameter(propName, paramName);
      param->SetRefObjectName(Gmat::SPACECRAFT, objName);
      
      if (depObjName != "")
         param->SetStringParameter("DepObject", depObjName);

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
   
   #if DEBUG_PARAM_SELECT_DIALOG
   MessageInterface::ShowMessage
      ("ShowCoordSystem() property=%s, depObj=%d\n", property.c_str(), depObj);
   #endif
   
   if (depObj == GmatParam::COORD_SYS)
   {
      mCoordSysLabel->Show();
      mCoordSysLabel->SetLabel("Coordinate System");
      
      mCoordSysComboBox->SetStringSelection(mLastCoordSysName);
      
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

//------------------------------------------------------------------------------
// void HighlightObject(wxCommandEvent& event, bool highlight)
//------------------------------------------------------------------------------
void ParameterSelectDialog::HighlightObject(wxCommandEvent& event, bool highlight)
{
   wxComboBox *objComboBox = GetObjectComboBox();
   if (objComboBox == NULL)
      return;
   
   if (highlight)
   {
      // Just set background color for now
      objComboBox->SetBackgroundColour(*wxCYAN);
      objComboBox->Refresh();
   }
   else
   {
      objComboBox->SetBackgroundColour(*wxWHITE);
      objComboBox->Refresh();
   }
}


//------------------------------------------------------------------------------
// wxComboBox* GetObjectComboBox()
//------------------------------------------------------------------------------
wxComboBox* ParameterSelectDialog::GetObjectComboBox()
{
   if (mSpacecraftComboBox->IsShown())
      return mSpacecraftComboBox;
   else if (mImpBurnComboBox->IsShown())
      return mImpBurnComboBox;
   else
   {
      MessageInterface::ShowMessage
         ("**** INTERNAL ERROR **** Internal Invalid Object ComboBox in "
          "ParameterSelectDialog::GetObjectComboBox()\n");
      return NULL;
   }
}
