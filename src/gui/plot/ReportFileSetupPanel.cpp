//$Id$
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
#include "ParameterSelectDialog.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "ParameterInfo.hpp"            // for GetDepObjectType()
#include "StringUtil.hpp"
#include "MessageInterface.hpp"
#include <fstream>


//#define DEBUG_REPORTFILE_PANEL 1
//#define DEBUG_REPORTFILE_PANEL_LOAD 1
//#define DEBUG_REPORTFILE_PANEL_SAVE 1

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
   EVT_BUTTON(ID_BUTTON, ReportFileSetupPanel::OnButtonClick)
   EVT_CHECKBOX(ID_CHECKBOX, ReportFileSetupPanel::OnCheckBoxChange)
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
   #if DEBUG_REPORTFILE_PANEL
   MessageInterface::ShowMessage("ReportFileSetupPanel() entering...\n");
   MessageInterface::ShowMessage("ReportFileSetupPanel() subscriberName = " +
                                 std::string(subscriberName.c_str()) + "\n");
   #endif
   
   Subscriber *subscriber = (Subscriber*)
      theGuiInterpreter->GetConfiguredObject(subscriberName.c_str());
   
   reportFile = (ReportFile*)subscriber;
   
   mObjectTypeList.Add("Spacecraft");
   mObjectTypeList.Add("ImpulsiveBurn");
   
   Create();
   Show();
   mHasParameterChanged = false;
   EnableUpdate(false);
   
   // Listen for Spacecraft and Parameter name change
   theGuiManager->AddToResourceUpdateListeners(this);

   #if DEBUG_REPORTFILE_PANEL
   MessageInterface::ShowMessage("ReportFileSetupPanel() exiting...\n");
   #endif
}


//------------------------------------------------------------------------------
// ~ReportFileSetupPanel()
//------------------------------------------------------------------------------
ReportFileSetupPanel::~ReportFileSetupPanel()
{
   mObjectTypeList.Clear();   
   theGuiManager->RemoveFromResourceUpdateListeners(this);
}


//------------------------------------------------------------------------------
// virtual bool PrepareObjectNameChange()
//------------------------------------------------------------------------------
bool ReportFileSetupPanel::PrepareObjectNameChange()
{
   // Save GUI data
   wxCommandEvent event;
   OnApply(event);
   
   return GmatPanel::PrepareObjectNameChange();
}


//------------------------------------------------------------------------------
// virtual void ObjectNameChanged(Gmat::ObjectType type, const wxString &oldName,
//                                const wxString &newName)
//------------------------------------------------------------------------------
/*
 * Reflects resource name change to this panel.
 * By the time this method is called, the base code already changed reference
 * object name, so all we need to do is re-load the data.
 */
//------------------------------------------------------------------------------
void ReportFileSetupPanel::ObjectNameChanged(Gmat::ObjectType type,
                                             const wxString &oldName,
                                             const wxString &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("ReportFilePanel::ObjectNameChanged() type=%d, oldName=<%s>, "
       "newName=<%s>, mDataChanged=%d\n", type, oldName.c_str(), newName.c_str(),
       mDataChanged);
   #endif
   
   if (type != Gmat::SPACECRAFT && type != Gmat::PARAMETER)
      return;
   
   mHasParameterChanged = false;
   LoadData();
   
   // We don't need to save data if object name changed from the resouce tree
   // while this panel is opened, since base code already has new name
   EnableUpdate(false);
}


//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnCheckBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
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
   #if DEBUG_REPORTFILE_PANEL
   MessageInterface::ShowMessage("ReportFileSetupPanel::Create() entering...\n");
   #endif
   
   Integer bsize = 2; // border size
   wxArrayString emptyList;   
   
   //-------------------------------------------------------
   // options
   //-------------------------------------------------------
   
   writeCheckBox = new wxCheckBox(this, ID_CHECKBOX, wxT("Write Report"),
                                  wxDefaultPosition, wxDefaultSize, 0);
   
   showHeaderCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Write Headers"),
                     wxDefaultPosition, wxDefaultSize, 0);
   
   leftJustifyCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Left Justify"),
                     wxDefaultPosition, wxDefaultSize, 0);
   
   zeroFillCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Zero Fill"),
                     wxDefaultPosition, wxDefaultSize, 0);
   
   // Solver Iteration ComboBox
   wxStaticText *solverIterLabel =
      new wxStaticText(this, -1, wxT("Solver Iterations"),
                       wxDefaultPosition, wxSize(-1, -1), 0);
   
   mSolverIterComboBox =
      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxSize(65, -1));
   
   // Get Solver Iteration option list from the Subscriber
   const std::string *solverIterList = Subscriber::GetSolverIterOptionList();
   int count = Subscriber::GetSolverIterOptionCount();
   for (int i=0; i<count; i++)
      mSolverIterComboBox->Append(solverIterList[i].c_str());
   
   wxStaticText *colWidthText =
      new wxStaticText(this, -1, wxT("Column Width"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   colWidthTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(35, -1),  0);
   
   wxStaticText *precisionText =
      new wxStaticText(this, -1, wxT("Precision"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   precisionTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""),
                                     wxDefaultPosition, wxSize(35, -1),  0);
   
   wxFlexGridSizer *option2Sizer = new wxFlexGridSizer(2);
   option2Sizer->Add(writeCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   option2Sizer->Add(20, 20);
   option2Sizer->Add(showHeaderCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   option2Sizer->Add(20, 20);
   option2Sizer->Add(leftJustifyCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   option2Sizer->Add(20, 20);
   option2Sizer->Add(zeroFillCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   option2Sizer->Add(20, 20);
   option2Sizer->Add(20, 20);
   option2Sizer->Add(20, 20);
   option2Sizer->Add(solverIterLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   option2Sizer->Add(mSolverIterComboBox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   option2Sizer->Add(colWidthText, 0, wxALIGN_LEFT|wxALL, bsize);
   option2Sizer->Add(colWidthTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   option2Sizer->Add(precisionText, 0, wxALIGN_LEFT|wxALL, bsize);
   option2Sizer->Add(precisionTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   GmatStaticBoxSizer *optionSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Options");
   optionSizer->Add(option2Sizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-------------------------------------------------------
   // selected parameter list
   //-------------------------------------------------------
   mSelectedListBox =
      new wxListBox(this, ID_LISTBOX, wxDefaultPosition, wxSize(200, 150),
                    emptyList, wxLB_SINGLE);
   
   mViewButton = new wxButton( this, ID_BUTTON, wxT("View"), 
                                wxDefaultPosition, wxDefaultSize, 0 );
   
   GmatStaticBoxSizer *selectedSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Parameter List");
   
   selectedSizer->Add(mSelectedListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   selectedSizer->Add(mViewButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-------------------------------------------------------
   // report file name
   //-------------------------------------------------------
   wxBoxSizer *fileSizer = new wxBoxSizer(wxHORIZONTAL);
   wxStaticText *fileStaticText =
      new wxStaticText(this, ID_TEXT, wxT("File: "), 
                       wxDefaultPosition, wxDefaultSize, 0 );
   mFileTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""), 
                                 wxDefaultPosition, wxSize(300, -1),  0);
   mBrowseButton = new wxButton( this, ID_BUTTON, wxT("Browse"), 
                                wxDefaultPosition, wxDefaultSize, 0 );
   
   fileSizer->Add(fileStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   fileSizer->Add(mFileTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   fileSizer->Add(mBrowseButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   //-------------------------------------------------------
   // put in the order
   //-------------------------------------------------------
   wxFlexGridSizer *variablesBoxSizer = new wxFlexGridSizer(2);
   variablesBoxSizer->Add(optionSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   variablesBoxSizer->Add(selectedSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   //-------------------------------------------------------
   // add to parent sizer
   //-------------------------------------------------------
   theMiddleSizer->Add(variablesBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   theMiddleSizer->Add(10, 10);
   theMiddleSizer->Add(fileSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   #if DEBUG_REPORTFILE_PANEL
   MessageInterface::ShowMessage("ReportFileSetupPanel::Create() exiting...\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ReportFileSetupPanel::LoadData()
{
   #if DEBUG_REPORTFILE_PANEL_LOAD
   MessageInterface::ShowMessage("ReportFileSetupPanel::LoadData() entering...\n");
   #endif
   
   // Set the pointer for the "Show Script" button
   mObject = reportFile;
   
   // load data from the core engine
   writeCheckBox->SetValue(reportFile->IsActive());
   
   try
   {
      // load file name data from core engine
      //Note: Do not use GetStringParameter() since filename may be empty.
      //      GetFileName() constructs filename if name is empty
      std::string filename = reportFile->GetFileName();
      
      Integer id;
      
      mFileTextCtrl->SetValue(wxT(filename.c_str()));
      
      #if DEBUG_REPORTFILE_PANEL_LOAD
      MessageInterface::ShowMessage
         ("ReportFileSetupPanel::LoadData() filename=%s\n", filename.c_str());
      #endif
      
      id = reportFile->GetParameterID("WriteHeaders");
      if (strcmp(reportFile->GetOnOffParameter(id).c_str(), "On") == 0)
         showHeaderCheckBox->SetValue(true);
      else
         showHeaderCheckBox->SetValue(false);
      
      id = reportFile->GetParameterID("LeftJustify");
      if (strcmp(reportFile->GetOnOffParameter(id).c_str(), "On") == 0)
         leftJustifyCheckBox->SetValue(true);
      else
         leftJustifyCheckBox->SetValue(false);
      
      id = reportFile->GetParameterID("ZeroFill");
      if (strcmp(reportFile->GetOnOffParameter(id).c_str(), "On") == 0)
         zeroFillCheckBox->SetValue(true);
      else
         zeroFillCheckBox->SetValue(false);
      
      id = reportFile->GetParameterID("SolverIterations");
      
      mSolverIterComboBox->
         SetValue(reportFile->GetStringParameter("SolverIterations").c_str());
      
      id = reportFile->GetParameterID("ColumnWidth");
      wxString numSpacesValue;
      numSpacesValue.Printf("%d", reportFile->GetIntegerParameter(id));
      colWidthTextCtrl->SetValue(numSpacesValue);
      
      id = reportFile->GetParameterID("Precision");
      wxString precisionValue;
      precisionValue.Printf("%d", reportFile->GetIntegerParameter(id));
      precisionTextCtrl->SetValue(precisionValue);
      
      StringArray parameterList = reportFile->GetStringArrayParameter("Add");
      mNumParameters = parameterList.size();
      
      #if DEBUG_REPORTFILE_PANEL_LOAD
      MessageInterface::ShowMessage("   mNumParameters=%d\n", mNumParameters);
      #endif
      
      if (mNumParameters > 0)
      {
         Parameter *param;
         wxString paramName;
         
         for (int i=0; i<mNumParameters; i++)
         {
            param = theGuiInterpreter->GetParameter(parameterList[i]);
            paramName = parameterList[i].c_str();
            mSelectedListBox->Append(paramName);
            mReportWxStrings.Add(paramName);
            
            #if DEBUG_REPORTFILE_PANEL_LOAD
            MessageInterface::ShowMessage("   added %s\n", paramName.c_str());
            #endif
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   #if DEBUG_REPORTFILE_PANEL_LOAD
   MessageInterface::ShowMessage("ReportFileSetupPanel::LoadData() exiting...\n");
   #endif
   
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ReportFileSetupPanel::SaveData()
{
   #if DEBUG_REPORTFILE_PANEL_SAVE
   MessageInterface::ShowMessage
      ("ReportFileSetupPanel::SaveData() mHasParameterChanged=%d\n", mHasParameterChanged);
   #endif
   
   canClose = true;
   std::string str;
   Integer width, prec;
   
   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   
   str = colWidthTextCtrl->GetValue();
   CheckInteger(width, str, "Column Width", "Integer Number > 0");
   
   str = precisionTextCtrl->GetValue();
   CheckInteger(prec, str, "Column Precision", "Integer Number > 0");
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      Integer id;
      
      reportFile->Activate(writeCheckBox->IsChecked());
      
      #if DEBUG_RF_PANEL_SAVE
      if (theSubscriber->IsActive())
         MessageInterface::ShowMessage
            ("\nReportFileSetupPanel:: The subscriber was activiated\n");
      else
         MessageInterface::ShowMessage
            ("\nReportFileSetupPanel:: The subscriber was NOT activiated\n");
      #endif
      
      // To show all the error messages, multiple try/catch is used
      try
      {
         id = reportFile->GetParameterID("ColumnWidth");
         reportFile->SetIntegerParameter(id, width);
      }
      catch (BaseException &e)
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
         canClose = false;
      }
      
      try
      {
         id = reportFile->GetParameterID("Precision");
         reportFile->SetIntegerParameter(id, prec);
      }
      catch (BaseException &e)
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
         canClose = false;
      }
      
      id = reportFile->GetParameterID("WriteHeaders");
      if (showHeaderCheckBox->IsChecked())
         reportFile->SetOnOffParameter(id, "On");
      else
         reportFile->SetOnOffParameter(id, "Off");
      
      id = reportFile->GetParameterID("LeftJustify");
      if (leftJustifyCheckBox->IsChecked())
         reportFile->SetOnOffParameter(id, "On");
      else
         reportFile->SetOnOffParameter(id, "Off");
      
      id = reportFile->GetParameterID("ZeroFill");
      if (zeroFillCheckBox->IsChecked())
         reportFile->SetOnOffParameter(id, "On");
      else
         reportFile->SetOnOffParameter(id, "Off");
      
      id = reportFile->GetParameterID("SolverIterations");
      reportFile->
         SetStringParameter(id, mSolverIterComboBox->GetValue().c_str());
      
      // save file name data
      str = mFileTextCtrl->GetValue();
      std::ofstream filename(str.c_str());
      
      // Check for non-existing pathname/filename
      if (!filename)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, mMsgFormat.c_str(), str.c_str(),
             "File", "Valid File Path and Name");
         canClose = false;
         return;
      }
      
      id = reportFile->GetParameterID("Filename");
      reportFile->SetStringParameter(id, str.c_str());
      
      // if parameter changed, clear the list and re-add parameters
      if (mHasParameterChanged)
      {
         mHasParameterChanged = false;
         mNumParameters = mSelectedListBox->GetCount();
         
         #if DEBUG_REPORTFILE_PANEL_SAVE
         MessageInterface::ShowMessage("   mNumParameters=%d\n", mNumParameters);
         #endif
         
         if (mNumParameters >= 0) // >=0 because the list needs to be cleared
         {
            if (mNumParameters == 0)
            {
               MessageInterface::ShowMessage
                  ("\nWarning:  No variable in %s's \"Selected\"  selection box",
                   reportFile->GetName().c_str());
            }
            reportFile->TakeAction("Clear");
            for (int i=0; i<mNumParameters; i++)
            {
               std::string selYName = mSelectedListBox->GetString(i).c_str();
               reportFile->SetStringParameter("Add", selYName, i);
            }
         }
         
         theGuiInterpreter->ValidateSubscriber(mObject);
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
}


//------------------------------------------------------------------------------
// void OnButtonClick()
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mViewButton)
   {
      ParameterSelectDialog paramDlg(this, mObjectTypeList,
                                     GuiItemManager::SHOW_REPORTABLE, true, true);
      
      paramDlg.SetParamNameArray(mReportWxStrings);
      paramDlg.ShowModal();
      
      if (paramDlg.HasSelectionChanged())
      {
         mHasParameterChanged = true;
         EnableUpdate(true);
         mReportWxStrings = paramDlg.GetParamNameArray();
         
         if (mReportWxStrings.Count() > 0)
         {
            mSelectedListBox->Clear();
            for (unsigned int i=0; i<mReportWxStrings.Count(); i++)
               mSelectedListBox->Append(mReportWxStrings[i]);
         }
         else // no selections
         {
            mSelectedListBox->Clear();
         }
      }
   }
   else if (event.GetEventObject() == mBrowseButton)
   {
      wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
    
      if (dialog.ShowModal() == wxID_OK)
      {
         wxString filename;
         
         filename = dialog.GetPath().c_str();
         mFileTextCtrl->SetValue(filename); 
      }
   }
}


//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnTextChange(wxCommandEvent &event)
{
    EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnComboBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}


