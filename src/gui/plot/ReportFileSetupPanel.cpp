//$Header$
//------------------------------------------------------------------------------
//                              ReportFileSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/02/12
/**
 * Implements ReportFileSetupPanel class.
 */
//------------------------------------------------------------------------------
#include "ReportFileSetupPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "ParameterCreateDialog.hpp"
#include "ParameterInfo.hpp"            // for GetDepObjectType()
#include "MessageInterface.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(ReportFileSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
   
   EVT_TEXT(ID_TEXT_CTRL, ReportFileSetupPanel::OnTextChange)
   EVT_TEXT(ID_TEXT, ReportFileSetupPanel::OnTextChange)
   EVT_BUTTON(ID_BROWSE_BUTTON, ReportFileSetupPanel::OnBrowseButton)
   EVT_BUTTON(ADD_VAR_BUTTON, ReportFileSetupPanel::OnAddVariable)
   EVT_BUTTON(REMOVE_VAR_BUTTON, ReportFileSetupPanel::OnRemoveVariable)
   EVT_BUTTON(CLEAR_VAR_BUTTON, ReportFileSetupPanel::OnClearVariable)
   EVT_BUTTON(CREATE_VARIABLE, ReportFileSetupPanel::OnCreateVariable)
   EVT_CHECKBOX(RF_WRITE_CHECKBOX, ReportFileSetupPanel::OnWriteCheckBoxChange)
   EVT_LISTBOX(USER_PARAM_LISTBOX, ReportFileSetupPanel::OnSelectUserParam)
   EVT_LISTBOX(PROPERTY_LISTBOX, ReportFileSetupPanel::OnSelectProperty)
   EVT_COMBOBOX(ID_COMBOBOX, ReportFileSetupPanel::OnComboBoxChange)

END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
    
//------------------------------------------------------------------------------
// ReportFileSetupPanel(wxWindow *parent, const wxString &subscriberName)
//------------------------------------------------------------------------------
/**
 * Constructs ReportFileSetupPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the ReportFileSetupPanel GUI
 */
//------------------------------------------------------------------------------
ReportFileSetupPanel::ReportFileSetupPanel(wxWindow *parent,
                                   const wxString &subscriberName)
    : GmatPanel(parent)
{
    //MessageInterface::ShowMessage("ReportFileSetupPanel() entering...\n");
    //MessageInterface::ShowMessage("ReportFileSetupPanel() subscriberName = " +
    //                              std::string(subscriberName.c_str()) + "\n");
    
    Subscriber *subscriber =
        theGuiInterpreter->GetSubscriber(std::string(subscriberName.c_str()));

    reportFile = (ReportFile*)subscriber;

    Create();
    Show();
    mUseUserParam = false;
    theApplyButton->Disable();
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnWriteCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnWriteCheckBoxChange(wxCommandEvent& event)
{
    theApplyButton->Enable();
}

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * @param <scName> input spacecraft name.
 *
 * @note Creates the notebook for spacecraft information
 */
//------------------------------------------------------------------------------
void ReportFileSetupPanel::Create()
{
    //MessageInterface::ShowMessage("ReportFileSetupPanel::Create() entering...\n");
   
    Integer bsize = 2; // border size
    wxString emptyList[] = {};

   //------------------------------------------------------
   // available variables list (1st column)
   //------------------------------------------------------
   mVarBoxSizer = new wxBoxSizer(wxVERTICAL);
    
   wxButton *createVarButton;

   //loj: 2/8/05 CreateParameterSizer() calling sequence changed
   mParamBoxSizer = theGuiManager->
      CreateParameterSizer(this, &mUserParamListBox, USER_PARAM_LISTBOX,
                           &createVarButton, CREATE_VARIABLE,
                           &mObjectComboBox, ID_COMBOBOX,
                           &mPropertyListBox, PROPERTY_LISTBOX,
                           &mCoordSysComboBox, ID_COMBOBOX,
                           &mCentralBodyComboBox, ID_COMBOBOX,
                           &mCoordSysLabel, &mCoordSysSizer);
   
   //-------------------------------------------------------
   // add, remove, clear parameter buttons (2nd column)
   //-------------------------------------------------------
   wxButton *addScButton = new wxButton(this, ADD_VAR_BUTTON, wxT("-->"),
                              wxDefaultPosition, wxSize(20,20), 0);

   wxButton *removeScButton = new wxButton(this, REMOVE_VAR_BUTTON, wxT("<--"),
                                 wxDefaultPosition, wxSize(20,20), 0);
    
   wxButton *clearScButton = new wxButton(this, CLEAR_VAR_BUTTON, wxT("<="),
                                wxDefaultPosition, wxSize(20,20), 0);
    
   wxBoxSizer *arrowButtonsBoxSizer = new wxBoxSizer(wxVERTICAL);
   arrowButtonsBoxSizer->Add(addScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsBoxSizer->Add(removeScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsBoxSizer->Add(clearScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   //-------------------------------------------------------
   // selected parameter list (3rd column)
   //-------------------------------------------------------
   wxStaticBox *selectedStaticBox = new wxStaticBox( this, -1, wxT("") );

   wxStaticText *titleSelectedText =
      new wxStaticText(this, -1, wxT("Selected"),
                       wxDefaultPosition, wxSize(80,-1), 0);
   
   //loj: 1/19/05 Changed 150,250 to 170,260
   mVarSelectedListBox =
      new wxListBox(this, VAR_SEL_LISTBOX, wxDefaultPosition,
                    wxSize(170,260), 0, emptyList, wxLB_SINGLE);
   
   wxStaticBoxSizer *mVarSelectedBoxSizer =
      new wxStaticBoxSizer(selectedStaticBox, wxVERTICAL);

   mVarSelectedBoxSizer->Add(titleSelectedText, 0, wxALIGN_CENTRE|wxALL, bsize);
   mVarSelectedBoxSizer->Add(mVarSelectedListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-------------------------------------------------------
   // report file name
   //-------------------------------------------------------
   wxBoxSizer *fileSizer = new wxBoxSizer(wxHORIZONTAL);
   fileStaticText = new wxStaticText(this, ID_TEXT, wxT("File: "), 
                                     wxDefaultPosition, wxDefaultSize, 0 );
   fileTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""), 
                                 wxDefaultPosition, wxSize(250, -1),  0);
   browseButton = new wxButton( this, ID_BROWSE_BUTTON, wxT("Browse"), 
                                wxDefaultPosition, wxDefaultSize, 0 );
   
   fileSizer->Add(fileStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   fileSizer->Add(fileTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   fileSizer->Add(browseButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   //-------------------------------------------------------
   // options
   //-------------------------------------------------------

   //loj: 1/19/05 put report option in Horizontal order to save space
   writeCheckBox = new wxCheckBox(this, RF_WRITE_CHECKBOX, wxT("Write Report"),
                                  wxDefaultPosition, wxSize(100, -1), 0);
   
   showHeaderCheckBox =
      new wxCheckBox(this, RF_WRITE_CHECKBOX, wxT("Show Headers"),
                     wxDefaultPosition, wxSize(100, -1), 0);
   
   wxStaticText *colWidthText =
      new wxStaticText(this, -1, wxT("Column Width  "),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   colWidthTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(35, -1),  0);
   
   wxBoxSizer *reportOptionSizer = new wxBoxSizer(wxHORIZONTAL);
   reportOptionSizer->Add(writeCheckBox, 0, wxALIGN_CENTER|wxALL, bsize);
   reportOptionSizer->Add(showHeaderCheckBox, 0, wxALIGN_CENTER|wxALL, bsize);                       
   reportOptionSizer->Add(colWidthText, 0, wxALIGN_CENTER|wxALL, bsize);
   reportOptionSizer->Add(colWidthTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   
   optionBoxSizer = new wxBoxSizer(wxVERTICAL);
   optionBoxSizer->Add(fileSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   optionBoxSizer->Add(reportOptionSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-------------------------------------------------------
   // put in the order
   //-------------------------------------------------------
   wxFlexGridSizer *mFlexGridSizer = new wxFlexGridSizer(5, 0, 0);
   wxBoxSizer *variablesBoxSizer = new wxBoxSizer(wxVERTICAL);
   
   mFlexGridSizer->Add(mParamBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(arrowButtonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(mVarSelectedBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   variablesBoxSizer->Add(mFlexGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

   //-------------------------------------------------------
   // add to parent sizer
   //-------------------------------------------------------
   theMiddleSizer->Add(variablesBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   theMiddleSizer->Add(optionBoxSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ReportFileSetupPanel::LoadData()
{
    // Set the pointer for the "Show Script" button
    mObject = reportFile;
    
    // load data from the core engine
    writeCheckBox->SetValue(reportFile->IsActive());

    // load file name data from core engine
    int filenameId = reportFile->GetParameterID("Filename");
    std::string filename = reportFile->GetStringParameter(filenameId);
    fileTextCtrl->SetValue(wxT(filename.c_str()));
    
    int writeHeadersId = reportFile->GetParameterID("WriteHeaders");
    if (strcmp(reportFile->GetStringParameter(writeHeadersId).c_str(), "On") == 0)
      showHeaderCheckBox->SetValue(true);
    else
      showHeaderCheckBox->SetValue(false);                     
    
    int spacesId = reportFile->GetParameterID("ColumnWidth");
    wxString numSpacesValue;
    numSpacesValue.Printf("%d", reportFile->GetIntegerParameter(spacesId));
    colWidthTextCtrl->SetValue(numSpacesValue);
    
    StringArray varParamList = reportFile->GetStringArrayParameter("Add");
    mNumVarParams = varParamList.size();

    if (mNumVarParams > 0)
    {

       wxString *varParamNames = new wxString[mNumVarParams];
       Parameter *param;
         
       for (int i=0; i<mNumVarParams; i++)
       {
          varParamNames[i] = varParamList[i].c_str();
          param = theGuiInterpreter->GetParameter(varParamList[i]);
       }
    
       mVarSelectedListBox->Set(mNumVarParams, varParamNames);
       mVarSelectedListBox->SetSelection(0);
       delete varParamNames;
    }
    
    mUserParamListBox->Deselect(mUserParamListBox->GetSelection());
    mObjectComboBox->SetSelection(0);
    mPropertyListBox->SetSelection(0);
    
    // show coordinate system or central body
    ShowCoordSystem();

}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ReportFileSetupPanel::SaveData()
{
    // save data to core engine
    reportFile->Activate(writeCheckBox->IsChecked());
//    if (theSubscriber->IsActive())
//      MessageInterface::ShowMessage("\nReportFileSetupPanel:: The subscriber was activiated\n");
//    else
//      MessageInterface::ShowMessage("\nReportFileSetupPanel:: The subscriber was NOT activiated\n");
    
    int writeHeadersId = reportFile->GetParameterID("WriteHeaders");
    if (showHeaderCheckBox->IsChecked())
      reportFile->SetStringParameter(writeHeadersId, "On");
    else
      reportFile->SetStringParameter(writeHeadersId, "Off");
                  
    int spacesId = reportFile->GetParameterID("ColumnWidth");
    reportFile->SetIntegerParameter(spacesId,
                  atoi(colWidthTextCtrl->GetValue()));
                  
    // save file name data to core engine
    wxString filename = fileTextCtrl->GetValue();
    int filenameId = reportFile->GetParameterID("Filename");
    reportFile->SetStringParameter(filenameId,
                  std::string (filename.c_str()));
                 
            
    mNumVarParams = mVarSelectedListBox->GetCount();
    
    if (mNumVarParams >= 0) // >=0 because the list needs to be cleared
    {
         reportFile->TakeAction("Clear");
         for (int i=0; i<mNumVarParams; i++)
         {
            std::string selYName = std::string(mVarSelectedListBox->GetString(i).c_str());
            reportFile->SetStringParameter("Add", selYName, i); //arg: 11/17/04 added index
         }
      }

}

//------------------------------------------------------------------------------
// void OnBrowseButton()
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnBrowseButton(wxCommandEvent& event)
{
    wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
    
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString filename;
        
        filename = dialog.GetPath().c_str();
        fileTextCtrl->SetValue(filename); 
    }
}

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnTextChange()
{
    theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnAddSpacecraft(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnAddVariable(wxCommandEvent& event)
{
   // get string in first list and then search for it in the second list
   wxString newParam = GetParamName();
   int found = mVarSelectedListBox->FindString(newParam);
    
   // if the string wasn't found in the second list, insert it
   if (found == wxNOT_FOUND)
   {
      // Create a paramete if it does not exist
      Parameter *param = GetParameter(newParam);

      if (param->IsPlottable())
      {
         mVarSelectedListBox->Append(newParam);
         mVarSelectedListBox->SetStringSelection(newParam);
         theApplyButton->Enable();
      }
      else
      {
         wxLogMessage("Selected  parameter:%s is not reportable. Please select "
                      "another parameter\n", newParam.c_str());
      }
   }
}

//------------------------------------------------------------------------------
// void OnRemoveSpacecraft(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnRemoveVariable(wxCommandEvent& event)
{
   int sel = mVarSelectedListBox->GetSelection();
   mVarSelectedListBox->Delete(sel);

   if (sel-1 < 0)
   {
      mVarSelectedListBox->SetSelection(0);
   }
   else
   {
      mVarSelectedListBox->SetSelection(sel-1);
   }
   
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnClearSpacecraft(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnClearVariable(wxCommandEvent& event)
{
   mVarSelectedListBox->Clear();
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnCreateVariable(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnCreateVariable(wxCommandEvent& event)
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
void ReportFileSetupPanel::OnSelectUserParam(wxCommandEvent& event)
{
   // deselect property
   mPropertyListBox->Deselect(mPropertyListBox->GetSelection());

   mUseUserParam = true;
}

//------------------------------------------------------------------------------
// void OnSelectProperty(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnSelectProperty(wxCommandEvent& event)
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
void ReportFileSetupPanel::OnComboBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mObjectComboBox)
   {
      mPropertyListBox->Deselect(mPropertyListBox->GetSelection());
      mUseUserParam = false;
   }
}


//------------------------------------------------------------------------------
// wxString GetParamName()
//------------------------------------------------------------------------------
wxString ReportFileSetupPanel::GetParamName()
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
Parameter* ReportFileSetupPanel::GetParameter(const wxString &name)
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
void ReportFileSetupPanel::ShowCoordSystem()
{
   //loj: 1/24/05 use ParameterInfo for dependent object type
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
