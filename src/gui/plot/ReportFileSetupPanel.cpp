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
#include "ParameterCreateDialog.hpp"
#include "ParameterInfo.hpp"            // for GetDepObjectType()
#include "StringUtil.hpp"
#include "MessageInterface.hpp"
#include <fstream>


#include "bitmaps/up.xpm"
#include "bitmaps/down.xpm"
#include "bitmaps/back.xpm"
#include "bitmaps/forward.xpm"
#include "bitmaps/backall.xpm"

//#define DEBUG_REPORTFILE_PANEL 1

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
   EVT_BUTTON(UP_VAR_BUTTON, ReportFileSetupPanel::OnMoveUpVariable)
   EVT_BUTTON(DOWN_VAR_BUTTON, ReportFileSetupPanel::OnMoveDownVariable)
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
   mUseUserParam = false;
   mParamChanged = false;
   EnableUpdate(false);
   
   // Listen for Spacecraft and Parameter name change
   theGuiManager->AddToResourceUpdateListeners(this);
}


//------------------------------------------------------------------------------
// ~ReportFileSetupPanel()
//------------------------------------------------------------------------------
ReportFileSetupPanel::~ReportFileSetupPanel()
{
   mObjectTypeList.Clear();
   theGuiManager->UnregisterComboBox("Spacecraft", mSpacecraftComboBox);   
   theGuiManager->UnregisterComboBox("ImpulsiveBurn", mImpBurnComboBox);   
   theGuiManager->UnregisterComboBox("CoordinateSystem", mCoordSysComboBox);
   
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
   
   mUseUserParam = false;
   mParamChanged = false;
   LoadData();
   
   // We don't need to save data if object name changed from the resouce tree
   // while this panel is opened, since base code already has new name
   EnableUpdate(false);
}


//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnWriteCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnWriteCheckBoxChange(wxCommandEvent& event)
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
   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;
   wxBitmap upBitmap = wxBitmap(up_xpm);
   wxBitmap downBitmap = wxBitmap(down_xpm);
   wxBitmap backBitmap = wxBitmap(back_xpm);
   wxBitmap forwardBitmap = wxBitmap(forward_xpm);
   wxBitmap backAllBitmap = wxBitmap(backall_xpm);
   
   //------------------------------------------------------
   // available variables list (1st column)
   //------------------------------------------------------
   
   wxButton *createVarButton;
   
   mParamBoxSizer = theGuiManager->
      CreateParameterSizer(this, &mUserParamListBox, USER_PARAM_LISTBOX,
                           &createVarButton, CREATE_VARIABLE,
                           &mObjectTypeComboBox, ID_COMBOBOX,
                           &mSpacecraftComboBox, ID_COMBOBOX,
                           &mImpBurnComboBox, ID_COMBOBOX,
                           &mPropertyListBox, PROPERTY_LISTBOX,
                           &mCoordSysComboBox, ID_COMBOBOX,
                           &mCentralBodyComboBox, ID_COMBOBOX,
                           &mCoordSysLabel, &mCoordSysSizer,
                           mObjectTypeList, GuiItemManager::SHOW_REPORTABLE);
   
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
      
      #if DEBUG_REPORTFILE_PANEL
      MessageInterface::ShowMessage
         ("===> mNumSc=%d, mNumImpBurn=%d, mNumScProperty=%d, mNumImpBurnProperty=%d\n",
          mNumSc, mNumImpBurn, mNumScProperty, mNumImpBurnProperty);
      #endif
   }
   
   //-------------------------------------------------------
   // add, remove, clear parameter buttons (2nd column)
   //-------------------------------------------------------

   wxBitmapButton *upVarButton =
      new wxBitmapButton(this, UP_VAR_BUTTON, upBitmap, wxDefaultPosition,
                         wxSize(20,20));
   wxBitmapButton *downVarButton =
      new wxBitmapButton(this, DOWN_VAR_BUTTON, downBitmap, wxDefaultPosition,
                         wxSize(20,20));
   wxBitmapButton *addVarButton =
      new wxBitmapButton(this, ADD_VAR_BUTTON, forwardBitmap, wxDefaultPosition,
                         wxSize(20,20));
   wxBitmapButton *removeVarButton =
      new wxBitmapButton(this, REMOVE_VAR_BUTTON, backBitmap, wxDefaultPosition,
                         wxSize(20,20));
   wxBitmapButton *clearVarButton =
      new wxBitmapButton(this, CLEAR_VAR_BUTTON, backAllBitmap, wxDefaultPosition,
                         wxSize(20,20));
   
   clearVarButton->SetToolTip("Remove All");
      
   wxBoxSizer *arrowButtonsBoxSizer = new wxBoxSizer(wxVERTICAL);
   arrowButtonsBoxSizer->Add(upVarButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsBoxSizer->Add(downVarButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsBoxSizer->Add(20,20, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsBoxSizer->Add(addVarButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsBoxSizer->Add(removeVarButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsBoxSizer->Add(clearVarButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-------------------------------------------------------
   // selected parameter list (3rd column)
   //-------------------------------------------------------
   wxStaticBox *selectedStaticBox = new wxStaticBox( this, -1, wxT("") );

   wxStaticText *titleSelectedText =
      new wxStaticText(this, -1, wxT("Selected"),
                       wxDefaultPosition, wxSize(80,-1), 0);
   
   mVarSelectedListBox =
      new wxListBox(this, VAR_SEL_LISTBOX, wxDefaultPosition, wxSize(170,260), //0,
                    emptyList, wxLB_SINGLE);
   
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
                                 wxDefaultPosition, wxSize(300, -1),  0);
   browseButton = new wxButton( this, ID_BROWSE_BUTTON, wxT("Browse"), 
                                wxDefaultPosition, wxDefaultSize, 0 );
   
   fileSizer->Add(fileStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   fileSizer->Add(fileTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   fileSizer->Add(browseButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   //-------------------------------------------------------
   // options
   //-------------------------------------------------------
   
   writeCheckBox = new wxCheckBox(this, RF_WRITE_CHECKBOX, wxT("Write Report"),
                                  wxDefaultPosition, wxDefaultSize, 0);
   
   showHeaderCheckBox =
      new wxCheckBox(this, RF_WRITE_CHECKBOX, wxT("Write Headers"),
                     wxDefaultPosition, wxDefaultSize, 0);
   
   leftJustifyCheckBox =
      new wxCheckBox(this, RF_WRITE_CHECKBOX, wxT("Left Justify"),
                     wxDefaultPosition, wxDefaultSize, 0);
   
   zeroFillCheckBox =
      new wxCheckBox(this, RF_WRITE_CHECKBOX, wxT("Zero Fill"),
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
   wxBoxSizer *solverIterOptionSizer = new wxBoxSizer(wxHORIZONTAL);
   solverIterOptionSizer->Add(solverIterLabel, 0, wxALIGN_CENTER|wxALL, bsize);
   solverIterOptionSizer->Add(mSolverIterComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   wxStaticText *colWidthText =
      new wxStaticText(this, -1, wxT("Column Width"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   colWidthTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(35, -1),  0);

   wxStaticText *precisionText =
      new wxStaticText(this, -1, wxT("  Precision"),
                       wxDefaultPosition, wxDefaultSize, 0);

   precisionTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""),
                                     wxDefaultPosition, wxSize(35, -1),  0);

   bsize = 1;
   wxBoxSizer *reportOptionSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *reportOptionColPrecSizer = new wxBoxSizer(wxHORIZONTAL);
   reportOptionSizer->Add(writeCheckBox, 0, wxALIGN_CENTER|wxALL, bsize);
   reportOptionSizer->Add(showHeaderCheckBox, 0, wxALIGN_CENTER|wxALL, bsize);
   reportOptionSizer->Add(leftJustifyCheckBox, 0, wxALIGN_CENTER|wxALL, bsize);
   reportOptionSizer->Add(zeroFillCheckBox, 0, wxALIGN_CENTER|wxALL, bsize);
   reportOptionColPrecSizer->Add(solverIterOptionSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   reportOptionColPrecSizer->Add(colWidthText, 0, wxALIGN_CENTER|wxALL, bsize);
   reportOptionColPrecSizer->Add(colWidthTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   reportOptionColPrecSizer->Add(precisionText, 0, wxALIGN_CENTER|wxALL, bsize);
   reportOptionColPrecSizer->Add(precisionTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);

   wxBoxSizer *optionBoxSizer = new wxBoxSizer(wxVERTICAL);
   optionBoxSizer->Add(20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   optionBoxSizer->Add(fileSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   optionBoxSizer->Add(reportOptionSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   optionBoxSizer->Add(reportOptionColPrecSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-------------------------------------------------------
   // put in the order
   //-------------------------------------------------------
   wxFlexGridSizer *paramGridSizer = new wxFlexGridSizer(3, 0, 0);
   paramGridSizer->Add(mParamBoxSizer, 0, wxALIGN_TOP|wxALL, bsize);
   paramGridSizer->Add(arrowButtonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   paramGridSizer->Add(mVarSelectedBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   wxBoxSizer *variablesBoxSizer = new wxBoxSizer(wxVERTICAL);
   variablesBoxSizer->Add(paramGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-------------------------------------------------------
   // add to parent sizer
   //-------------------------------------------------------
   theMiddleSizer->Add(variablesBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   theMiddleSizer->Add(optionBoxSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   #if DEBUG_REPORTFILE_PANEL
   MessageInterface::ShowMessage("ReportFileSetupPanel::Create() exiting...\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ReportFileSetupPanel::LoadData()
{
   #if DEBUG_REPORTFILE_PANEL
   MessageInterface::ShowMessage("ReportFileSetupPanel::LoadData() entering...\n");
   #endif
   
   // Set the pointer for the "Show Script" button
   mObject = reportFile;
   mLastCoordSysName = mCoordSysComboBox->GetString(0);
   
   // load data from the core engine
   writeCheckBox->SetValue(reportFile->IsActive());
   
   try
   {
      // load file name data from core engine
      //Note: Do not use GetStringParameter() since filename may be empty.
      //      GetFileName() constructs filename if name is empty
      std::string filename = reportFile->GetFileName();
      
      Integer id;
      
      fileTextCtrl->SetValue(wxT(filename.c_str()));
      
      #if DEBUG_REPORTFILE_PANEL
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
      
      StringArray varParamList = reportFile->GetStringArrayParameter("Add");
      mNumVarParams = varParamList.size();
      
      if (mNumVarParams > 0)
      {      
         wxString *varParamNames = new wxString[mNumVarParams];
         Parameter *param;
      
         for (int i=0; i<mNumVarParams; i++)
         {
            varParamNames[i] = varParamList[i].c_str();
            param = (Parameter*)
               theGuiInterpreter->GetConfiguredObject(varParamList[i]);
         }
      
         mVarSelectedListBox->Set(mNumVarParams, varParamNames);
         mVarSelectedListBox->SetSelection(0);
         delete [] varParamNames;
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   mUserParamListBox->Deselect(mUserParamListBox->GetSelection());   
   mSpacecraftComboBox->SetSelection(0);
   mPropertyListBox->SetSelection(0);
   
   // show coordinate system or central body
   ShowCoordSystem();
   
   #if DEBUG_REPORTFILE_PANEL
   MessageInterface::ShowMessage("ReportFileSetupPanel::LoadData() exiting...\n");
   #endif
   
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ReportFileSetupPanel::SaveData()
{
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
      str = fileTextCtrl->GetValue();
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
      if (mParamChanged)
      {
         mParamChanged = false;
         mNumVarParams = mVarSelectedListBox->GetCount();
         
         if (mNumVarParams >= 0) // >=0 because the list needs to be cleared
         {
            if (mNumVarParams == 0)
            {
               MessageInterface::ShowMessage
                  ("\nWarning:  No variable in %s's \"Selected\"  selection box",
                   reportFile->GetName().c_str());
            }
            reportFile->TakeAction("Clear");
            for (int i=0; i<mNumVarParams; i++)
            {
               std::string selYName = std::string(mVarSelectedListBox->GetString(i).c_str());
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
void ReportFileSetupPanel::OnTextChange(wxCommandEvent &event)
{
    EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnMoveUpVariable(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnMoveUpVariable(wxCommandEvent& event)
{
   int sel = mVarSelectedListBox->GetSelection();

   if (sel-1 >= 0)
   {
      wxString frontString = mVarSelectedListBox->GetString(sel-1);
      mVarSelectedListBox->SetString(sel-1, mVarSelectedListBox->GetStringSelection());
      mVarSelectedListBox->SetString(sel, frontString);
      mVarSelectedListBox->SetSelection(sel-1);
   }

   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnMoveDownVariable(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnMoveDownVariable(wxCommandEvent& event)
{
   unsigned int sel = mVarSelectedListBox->GetSelection();

   if (sel+1 >= 1 && (sel+1) < mVarSelectedListBox->GetCount())
   {
      wxString rearString = mVarSelectedListBox->GetString(sel+1);
      mVarSelectedListBox->SetString(sel+1, mVarSelectedListBox->GetStringSelection());
      mVarSelectedListBox->SetString(sel, rearString);      
      mVarSelectedListBox->SetSelection(sel+1);
   }
   
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnAddVariable(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnAddVariable(wxCommandEvent& event)
{
   wxString newParam = GetParamName();

   // if none selected just return (loj: 4/25/06)
   if (newParam == "")
      return;
   
   // get string in first list and then search for it in the second list
   int found = mVarSelectedListBox->FindString(newParam);
   
   // if the string wasn't found in the second list, insert it
   if (found == wxNOT_FOUND)
   {
      // Create a paramete if it does not exist
      Parameter *param = GetParameter(newParam);
      
      //if (param->IsPlottable())
      if (param->IsReportable())
      {
         mVarSelectedListBox->Append(newParam);
         mVarSelectedListBox->SetStringSelection(newParam);
         //Show next parameter
         mPropertyListBox->SetSelection(mPropertyListBox->GetSelection() + 1);
         OnSelectProperty(event);
         EnableUpdate(true);
         mParamChanged = true;
      }
      else
      {
         wxLogMessage("Selected  parameter:%s is not reportable.\nPlease select "
                      "another parameter.\n", newParam.c_str());
         
         //Show next parameter
         mPropertyListBox->SetSelection(mPropertyListBox->GetSelection() + 1);
         OnSelectProperty(event);
      }
   }
   else
   {
      //Show next parameter
      mPropertyListBox->SetSelection(mPropertyListBox->GetSelection() + 1);
      OnSelectProperty(event);
   }
}


//------------------------------------------------------------------------------
// void OnRemoveVariable(wxCommandEvent& event)
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
   
   EnableUpdate(true);
   mParamChanged = true;
}


//------------------------------------------------------------------------------
// void OnClearVariable(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnClearVariable(wxCommandEvent& event)
{
   mVarSelectedListBox->Clear();
   EnableUpdate(true);
   mParamChanged = true;
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

      mUserParamListBox->SetSelection(0);
      mPropertyListBox->Deselect(mPropertyListBox->GetSelection());
      mUseUserParam = true;
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
         mPropertyListBox->SetSelection(0);
         ShowCoordSystem();
      }
   }
   else if (obj == mSpacecraftComboBox || obj == mImpBurnComboBox)
   {
      mUseUserParam = false;
   }
   else if(obj == mCoordSysComboBox)
   {
      mLastCoordSysName = mCoordSysComboBox->GetStringSelection();
   }
   else
   {
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void ShowCoordSystem()
//------------------------------------------------------------------------------
void ReportFileSetupPanel::ShowCoordSystem()
{
   // use ParameterInfo for dependent object type
   std::string property = std::string(mPropertyListBox->GetStringSelection().c_str());
   GmatParam::DepObject depObj = ParameterInfo::Instance()->GetDepObjectType(property);
   
   if (depObj == GmatParam::COORD_SYS)
   {
      mCoordSysLabel->Show();
      mCoordSysLabel->SetLabel("Coordinate System");
      
      //Set CoordSystem to last one selected
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
      
      //Set Origin to last one selected
      //mCentralBodyComboBox->SetStringSelection("Earth");
      //mCentralBodyComboBox->SetStringSelection("mLastOriginName");
      
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
// wxString GetParamName()
//------------------------------------------------------------------------------
wxString ReportFileSetupPanel::GetParamName()
{
   if ((mUserParamListBox->GetSelection() == -1) &&
       (mPropertyListBox->GetSelection() == -1))
      return "";
   
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
      
      wxComboBox *objComboBox = GetObjectComboBox();
      if (objComboBox == NULL)
         return "";
      
      if (depObj == "")
         return objComboBox->GetStringSelection() + "." + 
            mPropertyListBox->GetStringSelection();
      else
         return objComboBox->GetStringSelection() + "." + depObj + "." +
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
   Parameter *param = (Parameter*)
      theGuiInterpreter->GetConfiguredObject(name.c_str());
   
   // create a parameter if it does not exist
   if (param == NULL)
   {
      wxComboBox *objComboBox = GetObjectComboBox();
      if (objComboBox == NULL)
         return NULL;
      
      std::string paramName(name.c_str());
      std::string objName(objComboBox->GetStringSelection().c_str());
      std::string propName(mPropertyListBox->GetStringSelection().c_str());
      std::string depObjName = "";
      bool isScProperty = true;
      
      if (mObjectTypeComboBox->GetValue() == "ImpulsiveBurn")
         isScProperty = false;
      
      if (mCoordSysComboBox->IsShown())
         depObjName = std::string(mCoordSysComboBox->GetStringSelection().c_str());
      else if (mCentralBodyComboBox->IsShown())
         depObjName = std::string(mCentralBodyComboBox->GetStringSelection().c_str());
      
      try
      {
         param = theGuiInterpreter->CreateParameter(propName, paramName);

         if (isScProperty)
         {
            param->SetRefObjectName(Gmat::SPACECRAFT, objName);
         
            if (depObjName != "")
               param->SetStringParameter("DepObject", depObjName);
         
            if (param->IsCoordSysDependent())
               param->SetRefObjectName(Gmat::COORDINATE_SYSTEM, depObjName);
            else if (param->IsOriginDependent())
               param->SetRefObjectName(Gmat::SPACE_POINT, depObjName);
         }
         else
         {
            param->SetRefObjectName(Gmat::IMPULSIVE_BURN, objName);
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage
            ("ReportFileSetupPanel:GetParameter() error occurred!\n%s\n",
             e.GetFullMessage().c_str());
      }
   }
   
   return param;
}


//------------------------------------------------------------------------------
// wxComboBox* GetObjectComboBox()
//------------------------------------------------------------------------------
wxComboBox* ReportFileSetupPanel::GetObjectComboBox()
{
   if (mSpacecraftComboBox->IsShown())
      return mSpacecraftComboBox;
   else if (mImpBurnComboBox->IsShown())
      return mImpBurnComboBox;
   else
   {
      MessageInterface::ShowMessage
         ("**** INTERNAL ERROR **** Internal Invalid Object ComboBox in "
          "ReportFileSetupPanel::GetObjectComboBox()\n");
      return NULL;
   }
}
