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
 * 
 */
//------------------------------------------------------------------------------

#include "ParameterSelectDialog.hpp"
#include "ParameterCreateDialog.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

//loj: 10/19/04 removed OnApply
BEGIN_EVENT_TABLE(ParameterSelectDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, ParameterSelectDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, ParameterSelectDialog::OnButton)
   EVT_COMBOBOX(ID_COMBOBOX, ParameterSelectDialog::OnComboBoxChange)
   EVT_LISTBOX(ID_LISTBOX, ParameterSelectDialog::OnListSelect)
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
   int borderSize = 2;
   wxString emptyList[] = {};

   //wxStaticBox
   wxStaticBox *userParamStaticBox = new wxStaticBox( this, -1, wxT("") );
   wxStaticBox *systemParamStaticBox = new wxStaticBox( this, -1, wxT("") );
   wxStaticBox *selParamStaticBox = new wxStaticBox( this, -1, wxT("") );
   
   //wxStaticText
   wxStaticText *userVarStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Variables"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   
   wxStaticText *objectStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Object"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   
   wxStaticText *propertyStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Property"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   
   wxStaticText *paramSelectedStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Variables Selected"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   
   //wxStaticText *emptyStaticText =
   //   new wxStaticText( this, ID_TEXT, wxT("  "),
   //                     wxDefaultPosition, wxDefaultSize, 0 );
   
   // wxButton
   mAddParamButton = new wxButton( this, ID_BUTTON, wxT("->"),
                                   wxDefaultPosition, wxSize(20,20), 0 );
   mCreateParamButton = new wxButton( this, ID_BUTTON, wxT("Create"),
                                      wxDefaultPosition, wxSize(-1,-1), 0 );

   // wxComboBox
   mObjectComboBox =
      theGuiManager->GetSpacecraftComboBox(this, ID_COMBOBOX, wxSize(150, 20));
   

   // wxListBox
   //loj: 11/8/04 added
   // if add user array to the list
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
   
   mPropertyListBox = theGuiManager->
      GetPropertyListBox(this, ID_LISTBOX, wxSize(150, 100), "Spacecraft");
   
   mParamSelectedListBox = new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                                         wxSize(150, 250), 0, emptyList, wxLB_SINGLE);
   
   
   // wxSizers
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   wxStaticBoxSizer *userParamBoxSizer =
      new wxStaticBoxSizer(userParamStaticBox, wxVERTICAL);
   wxStaticBoxSizer *systemParamBoxSizer =
      new wxStaticBoxSizer(systemParamStaticBox, wxVERTICAL);
   wxStaticBoxSizer *selParamBoxSizer =
      new wxStaticBoxSizer(selParamStaticBox, wxVERTICAL);
   wxBoxSizer *availParamBoxSizer = new wxBoxSizer(wxVERTICAL);
   wxFlexGridSizer *paramGridSizer = new wxFlexGridSizer( 3, 0, 0 );

   userParamBoxSizer->Add
      (userVarStaticText, 0, wxALIGN_CENTRE|wxLEFT|wxRight|wxBOTTOM, borderSize);
   userParamBoxSizer->Add
      (mUserParamListBox, 0, wxALIGN_CENTRE|wxLEFT|wxRight|wxBOTTOM, borderSize);
   userParamBoxSizer->Add
      (mCreateParamButton, 0, wxALIGN_CENTRE|wxLEFT|wxRight|wxBOTTOM, borderSize);
   
   systemParamBoxSizer->Add
      (objectStaticText, 0, wxALIGN_CENTRE|wxLEFT|wxRight|wxBOTTOM, borderSize);
   systemParamBoxSizer->Add
      (mObjectComboBox, 0, wxALIGN_CENTRE|wxLEFT|wxRight|wxBOTTOM, borderSize);
   systemParamBoxSizer->Add
      (propertyStaticText, 0, wxALIGN_CENTRE|wxLEFT|wxRight|wxBOTTOM, borderSize);
   systemParamBoxSizer->Add
      (mPropertyListBox, 0, wxALIGN_CENTRE|wxLEFT|wxRight|wxBOTTOM, borderSize);
   
   availParamBoxSizer->Add(userParamBoxSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   availParamBoxSizer->Add(systemParamBoxSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);

   selParamBoxSizer->Add(paramSelectedStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   selParamBoxSizer->Add(mParamSelectedListBox, 0, wxALIGN_CENTRE|wxALL, borderSize);

   paramGridSizer->Add(availParamBoxSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   paramGridSizer->Add(mAddParamButton, 0, wxALIGN_CENTRE|wxALL, borderSize);
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
void ParameterSelectDialog::LoadData()
{
   mObjectComboBox->SetSelection(0);
   mPropertyListBox->SetSelection(0);
   mUserParamListBox->Deselect(mUserParamListBox->GetSelection());
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
   if (mParamSelectedListBox->GetCount() > 0)
   {
      mParamName = mParamSelectedListBox->GetString(0); // first selection
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
// void OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnButton(wxCommandEvent& event)
{    
   if ( event.GetEventObject() == mAddParamButton )  
   {
      // empty listbox first, only one parameter is allowed
      // loj: Do we need multiple selection?
      mParamSelectedListBox->Clear();

      if (mUseUserParam)
      {
         mParamSelectedListBox->Append(mUserParamListBox->GetStringSelection());
      }
      else
      {
         wxString object = mObjectComboBox->GetStringSelection();
         wxString property = mPropertyListBox->GetStringSelection();
         mParamSelectedListBox->Append(object + "." + property);
      }
      theOkButton->Enable();
   }
   else if ( event.GetEventObject() == mCreateParamButton )  
   {
      ParameterCreateDialog paramDlg(this);
      paramDlg.ShowModal();
      
      if (paramDlg.IsParamCreated())
      {
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
}

//------------------------------------------------------------------------------
// void OnListSelect(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnListSelect(wxCommandEvent& event)
{    
   if (event.GetEventObject() == mPropertyListBox)  
   {
      mAddParamButton->Enable();
      mUserParamListBox->Deselect(mUserParamListBox->GetSelection());
      mUseUserParam = false;
   }
   else if (event.GetEventObject() == mUserParamListBox)  
   {
      mPropertyListBox->Deselect(mPropertyListBox->GetSelection());
      mAddParamButton->Enable();
      mUseUserParam = true;
   }
}

//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnComboBoxChange(wxCommandEvent& event)
{    
   if (event.GetEventObject() == mObjectComboBox)
   {
      mPropertyListBox->Deselect(mPropertyListBox->GetSelection());
      mAddParamButton->Disable();
      mUseUserParam = false;
   }
}
