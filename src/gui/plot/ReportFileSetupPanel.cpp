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
    
    EVT_BUTTON(RF_WRITE_CHECKBOX, ReportFileSetupPanel::OnWriteCheckBoxChange)
    EVT_CHECKBOX(RF_WRITE_CHECKBOX, ReportFileSetupPanel::OnWriteCheckBoxChange)
    
    EVT_BUTTON(ID_BROWSE_BUTTON, ReportFileSetupPanel::OnBrowseButton)
    EVT_TEXT(ID_TEXT_CTRL, ReportFileSetupPanel::OnTextChange)
    EVT_TEXT(ID_TEXT, ReportFileSetupPanel::OnTextChange)
    
   EVT_BUTTON(ADD_VAR_BUTTON, ReportFileSetupPanel::OnAddVariable)
   EVT_BUTTON(REMOVE_VAR_BUTTON, ReportFileSetupPanel::OnRemoveVariable)
   EVT_BUTTON(CLEAR_VAR_BUTTON, ReportFileSetupPanel::OnClearVariable)

   EVT_LISTBOX(USER_PARAM_LISTBOX, ReportFileSetupPanel::OnSelectUserParam)
   EVT_LISTBOX(PROPERTY_LISTBOX, ReportFileSetupPanel::OnSelectProperty)
   EVT_COMBOBOX(ID_COMBOBOX, ReportFileSetupPanel::OnComboBoxChange)
   EVT_BUTTON(CREATE_VARIABLE, ReportFileSetupPanel::OnCreateVariable)

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

    //------------------------------------------------------
    // plot option, (1st column)
    //------------------------------------------------------
    writeCheckBox = new wxCheckBox(this, RF_WRITE_CHECKBOX, wxT("Write Report"),
                                   wxDefaultPosition, wxSize(100, -1), 0);
            
    optionBoxSizer = new wxBoxSizer(wxVERTICAL);
    optionBoxSizer->Add(writeCheckBox, 0, wxALIGN_LEFT|wxALL, 5);
            
    //------------------------------------------------------
    // file option
    //------------------------------------------------------   
    wxBoxSizer *fileSizer = new wxBoxSizer(wxHORIZONTAL);
    // will need to change
    fileStaticText = new wxStaticText( this, ID_TEXT, 
                                       wxT("File: "), 
                                       wxDefaultPosition, wxSize(80,-1), 0 );
    fileTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""), 
                                              wxDefaultPosition, 
                                              wxSize(250, -1),  0);
    browseButton = new wxButton( this, ID_BROWSE_BUTTON, wxT("Browse"), 
                                              wxDefaultPosition, wxDefaultSize, 0 );
 
    fileSizer->Add(fileStaticText, 0, wxALIGN_CENTER|wxALL, 5);
    fileSizer->Add(fileTextCtrl, 0, wxALIGN_CENTER|wxALL, 5);
    fileSizer->Add(browseButton, 0, wxALIGN_CENTER|wxALL, 5);

    Integer bsize = 3; // border size
    wxString emptyList[] = {};

    wxBoxSizer *variablesBoxSizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer *mFlexGridSizer = new wxFlexGridSizer(5, 0, 0);
           
   //------------------------------------------------------
   // available variables list (1st column)
   //------------------------------------------------------
   mVarBoxSizer = new wxBoxSizer(wxVERTICAL);
    
//   wxStaticText *titleAvailable =
//      new wxStaticText(this, -1, wxT("Variables"),
//                       wxDefaultPosition, wxSize(-1,-1), 0);
//   wxArrayString empty;

//   //loj: 10/1/04 call GetPlottableParameterListBox()
//   mVarListBox =
//       theGuiManager->GetPlottableParameterListBox(this, -1, wxSize(150,200));
//      //theGuiManager->GetConfigParameterListBox(this, -1, wxSize(150,200));
   wxButton *createVarButton;

   wxBoxSizer *paramBoxSizer =
      theGuiManager->CreateParameterSizer(this, &createVarButton, CREATE_VARIABLE,
                                          &mObjectComboBox, ID_COMBOBOX,
                                          &mUserParamListBox, USER_PARAM_LISTBOX,
                                          &mPropertyListBox, PROPERTY_LISTBOX,
                                          &mCoordSysComboBox, ID_COMBOBOX,
                                          &mCoordSysLabel);

//   mVarBoxSizer->Add(titleAvailable, 0, wxALIGN_CENTRE|wxALL, bsize);
//   mVarBoxSizer->Add(mVarListBox, 0, wxALIGN_CENTRE|wxALL, bsize);

   //------------------------------------------------------
   // add, remove, clear parameter buttons (2nd column)
   //------------------------------------------------------
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
    
   //------------------------------------------------------
   // selected spacecraft list (4th column)
   //------------------------------------------------------
   //wxStaticBox
   wxStaticBox *selectedStaticBox = new wxStaticBox( this, -1, wxT("") );

   //wxStaticText
   wxStaticText *titleSelectedText =
      new wxStaticText(this, -1, wxT("Selected"),
                       wxDefaultPosition, wxSize(80,-1), 0);

   mVarSelectedListBox =
      new wxListBox(this, VAR_SEL_LISTBOX, wxDefaultPosition,
                    wxSize(150,250), 0, emptyList, wxLB_SINGLE);
        
   wxStaticBoxSizer *mVarSelectedBoxSizer =
      new wxStaticBoxSizer(selectedStaticBox, wxVERTICAL);

   mVarSelectedBoxSizer->Add(titleSelectedText, 0, wxALIGN_CENTRE|wxALL, bsize);
   mVarSelectedBoxSizer->Add(mVarSelectedListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // options
   //------------------------------------------------------
   showHeaderCheckBox =
      new wxCheckBox(this, RF_WRITE_CHECKBOX, wxT("Show Headers"),
                     wxDefaultPosition, wxSize(100, -1), 0);
   
   wxBoxSizer *colWidthBoxSizer = new wxBoxSizer(wxHORIZONTAL);         
   wxStaticText *colWidthText =
      new wxStaticText(this, -1, wxT("Column Width  "),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   colWidthTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""), 
                                              wxDefaultPosition, 
                                              wxSize(35, -1),  0);

   colWidthBoxSizer->Add(colWidthText, 0, wxALIGN_CENTER|wxALL, 0);
   colWidthBoxSizer->Add(colWidthTextCtrl, 0, wxALIGN_CENTER|wxALL, 0);           

   optionBoxSizer->Add(showHeaderCheckBox, 0, wxALIGN_LEFT|wxALL, 5);                       
   optionBoxSizer->Add(colWidthBoxSizer, 0, wxALIGN_LEFT|wxALL, 5);
      
   //------------------------------------------------------
   // put in the order
   //------------------------------------------------------    
   mFlexGridSizer->Add(paramBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(arrowButtonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(mVarSelectedBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   variablesBoxSizer->Add(mFlexGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

    //------------------------------------------------------
    // add to parent sizer
    //------------------------------------------------------
    theMiddleSizer->Add(variablesBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
    theMiddleSizer->Add(fileSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    theMiddleSizer->Add(optionBoxSizer, 0, wxGROW|wxALIGN_LEFT|wxALL, 5);

//    theMiddleSizer->Add(plotOptionBoxSizer, 0, wxALIGN_LEFT|wxALL, bsize);
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ReportFileSetupPanel::LoadData()
{
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
      Parameter *param = CreateParameter(newParam);

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
      
      return mObjectComboBox->GetStringSelection() + "." + depObj + "." +
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
Parameter* ReportFileSetupPanel::CreateParameter(const wxString &name)
{
   std::string paramName(name.c_str());
   std::string objName(mObjectComboBox->GetStringSelection().c_str());
   std::string propName(mPropertyListBox->GetStringSelection().c_str());

   Parameter *param = theGuiInterpreter->GetParameter(paramName);

   // create a parameter if it does not exist
   if (param == NULL)
   {
      param = theGuiInterpreter->CreateParameter(propName, paramName);
      param->SetRefObjectName(Gmat::SPACECRAFT, objName);
   }

   return param;
}

//------------------------------------------------------------------------------
// void ShowCoordSystem()
//------------------------------------------------------------------------------
void ReportFileSetupPanel::ShowCoordSystem()
{
   // get parameter pointer
   wxString paramName = GetParamName();
   Parameter *mCurrParam = CreateParameter(paramName);
   
   if (mCurrParam->IsCoordSysDependent())
   {
      mCoordSysLabel->Show();
      mCoordSysLabel->SetLabel("Coordinate System");
      mCoordSysComboBox->Clear();
      mCoordSysComboBox->Show();
   }
   else if (mCurrParam->IsOriginDependent())
   {
      mCoordSysLabel->Show();
      mCoordSysLabel->SetLabel("Central Body");
      
      mCoordSysComboBox->Clear();
      int bodyCount = theGuiManager->GetNumConfigBody();
      wxString *bodyList = theGuiManager->GetConfigBodyList();
      
      for (int i=0; i<bodyCount; i++)
         mCoordSysComboBox->Append(bodyList[i]);
      
      mCoordSysComboBox->SetStringSelection
         (mCurrParam->GetStringParameter("DepObject").c_str());
      mCoordSysComboBox->Show();
   }
   else
   {
      mCoordSysLabel->Hide();
      mCoordSysComboBox->Hide();
   }
   
}
