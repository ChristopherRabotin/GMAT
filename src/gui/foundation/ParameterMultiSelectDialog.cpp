//$Header$
//------------------------------------------------------------------------------
//                              ParameterMultiSelectDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Implements ParameterMultiSelectDialog class. This class shows dialog window where a
 * user parameter can be selected.
 * 
 */
//------------------------------------------------------------------------------

#include "ParameterMultiSelectDialog.hpp"
#include "ParameterCreateDialog.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

//loj: 10/19/04 removed OnApply
BEGIN_EVENT_TABLE(ParameterMultiSelectDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, ParameterMultiSelectDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, ParameterMultiSelectDialog::OnButton)
   EVT_COMBOBOX(ID_COMBOBOX, ParameterMultiSelectDialog::OnComboBoxChange)
   EVT_LISTBOX(ID_LISTBOX, ParameterMultiSelectDialog::OnListSelect)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ParameterMultiSelectDialog(wxWindow *parent)
//------------------------------------------------------------------------------
ParameterMultiSelectDialog::ParameterMultiSelectDialog(wxWindow *parent,
                                 wxArrayString &paramNames, bool showArray,
                                 bool showSysVars)
   : GmatDialog(parent, -1, wxString(_T("ParameterMultiSelectDialog")))
{
   mParamNames = paramNames;

   mIsParamSelected = false;
   mCanClose = true;
   mUseUserParam = false;
   mShowArray = showArray;
   mShowSysVars = showSysVars;

   Create();
   ShowData();
}

//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void ParameterMultiSelectDialog::Create()
{
   int borderSize = 2;
   wxString *tempList;

   unsigned int numParams = mParamNames.Count();
   if (numParams > 0)
   {
      tempList = new wxString[numParams];
      for (unsigned int i=0; i<numParams; i++)
      {
         tempList[i] = mParamNames[i];
      }
   }
   else
      tempList = new wxString[0];

   //wxStaticBox
   wxStaticBox *userParamStaticBox = new wxStaticBox( this, -1, wxT("") );
   
   if (mShowSysVars)
     systemParamStaticBox = new wxStaticBox( this, -1, wxT("") );
   wxStaticBox *selParamStaticBox = new wxStaticBox( this, -1, wxT("") );
  
   //wxStaticText
   wxStaticText *userVarStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Variables"),
                        wxDefaultPosition, wxDefaultSize, 0 );

   if (mShowSysVars)
   {
   objectStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Object"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   
   propertyStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Property"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   }
                     
   wxStaticText *paramSelectedStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Variables Selected"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   
   //wxStaticText *emptyStaticText =
   //   new wxStaticText( this, ID_TEXT, wxT("  "),
   //                     wxDefaultPosition, wxDefaultSize, 0 );
   
   // wxButton
   mAddParamButton = new wxButton( this, ID_BUTTON, wxT("->"),
                                   wxDefaultPosition, wxSize(20,20), 0 );
   mRemoveParamButton = new wxButton( this, ID_BUTTON, wxT("<-"),
                                   wxDefaultPosition, wxSize(20,20), 0 );
   mRemoveAllParamButton = new wxButton( this, ID_BUTTON, wxT("<="),
                                   wxDefaultPosition, wxSize(20,20), 0 );

   mCreateParamButton = new wxButton( this, ID_BUTTON, wxT("Create"),
                                      wxDefaultPosition, wxSize(-1,-1), 0 );

   // wxComboBox
   if (mShowSysVars)
   {
   mObjectComboBox =
      theGuiManager->GetSpacecraftComboBox(this, ID_COMBOBOX, wxSize(150, 20));
   }

   // wxListBox
   if (mShowArray)
   {
      mUserParamListBox =
         theGuiManager->GetUserParameterListBox(this, ID_LISTBOX, wxSize(150, 50));
   }
   else
   {
      mUserParamListBox =
         theGuiManager->GetUserVariableListBox(this, ID_LISTBOX, wxSize(150, 50));
   }

   //loj: 10/1/04 changed GetParameterListBox() to GetPropertyListBox()
   if (mShowSysVars)
   {
   mPropertyListBox = theGuiManager->
      GetPropertyListBox(this, ID_LISTBOX, wxSize(150, 100), "Spacecraft");
   }

   mParamSelectedListBox = new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                                         wxSize(150, 250), numParams, tempList, wxLB_SINGLE);
   
   
   // wxSizers
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   wxStaticBoxSizer *userParamBoxSizer =
      new wxStaticBoxSizer(userParamStaticBox, wxVERTICAL);
   if (mShowSysVars)
   {
   systemParamBoxSizer =
      new wxStaticBoxSizer(systemParamStaticBox, wxVERTICAL);
   }
   wxStaticBoxSizer *selParamBoxSizer =
      new wxStaticBoxSizer(selParamStaticBox, wxVERTICAL);
   wxBoxSizer *availParamBoxSizer = new wxBoxSizer(wxVERTICAL);
   wxFlexGridSizer *paramGridSizer = new wxFlexGridSizer( 3, 0, 0 );
   wxBoxSizer *addRemoveBoxSizer = new wxBoxSizer(wxVERTICAL);

   userParamBoxSizer->Add
      (userVarStaticText, 0, wxALIGN_CENTRE|wxLEFT|wxRight|wxBOTTOM, borderSize);
   userParamBoxSizer->Add
      (mUserParamListBox, 0, wxALIGN_CENTRE|wxLEFT|wxRight|wxBOTTOM, borderSize);
   userParamBoxSizer->Add
      (mCreateParamButton, 0, wxALIGN_CENTRE|wxLEFT|wxRight|wxBOTTOM, borderSize);

   if (mShowSysVars)
   {   
   systemParamBoxSizer->Add
      (objectStaticText, 0, wxALIGN_CENTRE|wxLEFT|wxRight|wxBOTTOM, borderSize);
   systemParamBoxSizer->Add
      (mObjectComboBox, 0, wxALIGN_CENTRE|wxLEFT|wxRight|wxBOTTOM, borderSize);
   systemParamBoxSizer->Add
      (propertyStaticText, 0, wxALIGN_CENTRE|wxLEFT|wxRight|wxBOTTOM, borderSize);
   systemParamBoxSizer->Add
      (mPropertyListBox, 0, wxALIGN_CENTRE|wxLEFT|wxRight|wxBOTTOM, borderSize);
   }
   
   availParamBoxSizer->Add(userParamBoxSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   if (mShowSysVars)
   {
   availParamBoxSizer->Add(systemParamBoxSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   }

   selParamBoxSizer->Add(paramSelectedStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   selParamBoxSizer->Add(mParamSelectedListBox, 0, wxALIGN_CENTRE|wxALL, borderSize);

   addRemoveBoxSizer->Add(mAddParamButton, 0, wxALIGN_CENTRE|wxALL, borderSize);
   addRemoveBoxSizer->Add(mRemoveParamButton, 0, wxALIGN_CENTRE|wxALL, borderSize);
   addRemoveBoxSizer->Add(mRemoveAllParamButton, 0, wxALIGN_CENTRE|wxALL, borderSize);

   paramGridSizer->Add(availParamBoxSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   paramGridSizer->Add(addRemoveBoxSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   paramGridSizer->Add(selParamBoxSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);

   
   pageBoxSizer->Add( paramGridSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);

   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);

}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ParameterMultiSelectDialog::LoadData()
{
  if (mShowSysVars)
  {
   mObjectComboBox->SetSelection(0);
   mPropertyListBox->SetSelection(0);
  }
  else
     mUserParamListBox->SetSelection(0);
}

//------------------------------------------------------------------------------
// virtual void OnOK()
//------------------------------------------------------------------------------
void ParameterMultiSelectDialog::OnOK()
{
   SaveData();

//   if (mCanClose)
   Close();
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ParameterMultiSelectDialog::SaveData()
{
   mCanClose = true;
   mParamNames.Clear();

   if (mParamSelectedListBox->GetCount() > 0)
   {
      for(int i=0; i<mParamSelectedListBox->GetCount(); i++)
      {
        mParamNames.Add(mParamSelectedListBox->GetString(i));
      }

      mIsParamSelected = false;
      mCanClose = false;
   }
   else
   {
      mIsParamSelected = false;
   }
}

//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void ParameterMultiSelectDialog::ResetData()
{
   mIsParamSelected = false;
}

//------------------------------------------------------------------------------
// void OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterMultiSelectDialog::OnButton(wxCommandEvent& event)
{    
   if ( event.GetEventObject() == mAddParamButton )  
   {
      // get string in first list and then search for it in the second list
      wxString newParam = GetNewParam();
      int found = mParamSelectedListBox->FindString(newParam);

      // if the string wasn't found in the second list, insert it
      if (found == wxNOT_FOUND)
      {
         // Create a paramete if it does not exist
          CreateParameter(newParam);

//        if (param->IsPlottable())
//        {
           mParamSelectedListBox->Append(newParam);
           mIsParamSelected = true;
           theOkButton->Enable();
//        }
//        else
//        {
//           wxLogMessage("Selected parameter:%s is not a single value. Please select "
//                      "another parameter\n", newParam.c_str());
//        }
      }
   }
   else if ( event.GetEventObject() == mRemoveParamButton )
   {
      int sel = mParamSelectedListBox->GetSelection();
      mParamSelectedListBox->Delete(sel);

      if (sel-1 < 0)
      {
         mParamSelectedListBox->SetSelection(0);
      }
      else
      {
         mParamSelectedListBox->SetSelection(sel-1);
      }

      mIsParamSelected = true;
      theOkButton->Enable();
   }
   else if ( event.GetEventObject() == mRemoveAllParamButton )
   {
      mParamSelectedListBox->Clear();
      mIsParamSelected = true;
      theOkButton->Enable();
   }

   else if ( event.GetEventObject() == mCreateParamButton )  
   {
      //loj: 7/28/04 added
      // show dialog to create parameter
      ParameterCreateDialog paramDlg(this);
      paramDlg.ShowModal();
      
      if (mShowArray)
      {
         mUserParamListBox->Set(theGuiManager->GetNumUserParameter(),
                                theGuiManager->GetUserParameterList());
      }
      else
      {
         mUserParamListBox->Set(theGuiManager->GetNumUserVariable(),
                                theGuiManager->GetUserVariableList());
      }
      mAddParamButton->Disable();
   }
}

//------------------------------------------------------------------------------
// void OnListSelect(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterMultiSelectDialog::OnListSelect(wxCommandEvent& event)
{    
   if (event.GetEventObject() == mPropertyListBox)  
   {
      mAddParamButton->Enable();
      mUserParamListBox->Deselect(mUserParamListBox->GetSelection());
      mUseUserParam = false;
   }
   else if (event.GetEventObject() == mUserParamListBox)  
   {
      if (mShowSysVars)  
         mPropertyListBox->Deselect(mPropertyListBox->GetSelection());
      mAddParamButton->Enable();
      mUseUserParam = true;
   }
}

//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterMultiSelectDialog::OnComboBoxChange(wxCommandEvent& event)
{    
   if (event.GetEventObject() == mObjectComboBox)
   {
      mPropertyListBox->Deselect(mPropertyListBox->GetSelection());
      mAddParamButton->Disable();
      mUseUserParam = false;
   }
}

//---------------------------------
// private methods
//---------------------------------
//------------------------------------------------------------------------------
// wxString GetNewParam()
//------------------------------------------------------------------------------
wxString ParameterMultiSelectDialog::GetNewParam()
{
   if (mUseUserParam)
   {
      return mUserParamListBox->GetStringSelection();
   }
   else
   {
      return mObjectComboBox->GetStringSelection() + "." +
         mPropertyListBox->GetStringSelection();
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
Parameter* ParameterMultiSelectDialog::CreateParameter(const wxString &name)
{
   Parameter *param;
   
   if (mShowSysVars)
   {               
       std::string paramName(name.c_str());
       std::string objName(mObjectComboBox->GetStringSelection().c_str());
       std::string propName(mPropertyListBox->GetStringSelection().c_str());
    
       param = theGuiInterpreter->GetParameter(paramName);
    
       // create a parameter if it does not exist
       if (param == NULL)
       {
          param = theGuiInterpreter->CreateParameter(propName, paramName);
          param->SetRefObjectName(Gmat::SPACECRAFT, objName);
       }
   }
   else
   {
       std::string paramName(name.c_str());
       param = theGuiInterpreter->GetParameter(paramName);
   }
   
   return param;

}
