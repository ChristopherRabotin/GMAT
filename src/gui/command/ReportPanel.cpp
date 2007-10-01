//$Header$
//------------------------------------------------------------------------------
//                              ReportPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2006/08/10
/**
 * Implements ReportPanel class. This class allows user to setup Report command.
 */
//------------------------------------------------------------------------------
#include "ReportPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "ParameterCreateDialog.hpp"
#include "ParameterInfo.hpp"            // for GetDepObjectType()
#include "ReportFile.hpp"
#include "MessageInterface.hpp"

#include "bitmaps/up.xpm"
#include "bitmaps/down.xpm"
#include "bitmaps/back.xpm"
#include "bitmaps/forward.xpm"
#include "bitmaps/backall.xpm"

//#define DEBUG_REPORT_PANEL 1
//#define DEBUG_REPORT_PANEL_SAVE 1

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(ReportPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
   
   EVT_BUTTON(UP_VAR_BUTTON, ReportPanel::OnMoveUpVariable)
   EVT_BUTTON(DOWN_VAR_BUTTON, ReportPanel::OnMoveDownVariable)
   EVT_BUTTON(ADD_VAR_BUTTON, ReportPanel::OnAddVariable)
   EVT_BUTTON(REMOVE_VAR_BUTTON, ReportPanel::OnRemoveVariable)
   EVT_BUTTON(CLEAR_VAR_BUTTON, ReportPanel::OnClearVariable)
   EVT_BUTTON(CREATE_VARIABLE, ReportPanel::OnCreateVariable)
   EVT_LISTBOX(USER_PARAM_LISTBOX, ReportPanel::OnSelectUserParam)
   EVT_LISTBOX(PROPERTY_LISTBOX, ReportPanel::OnSelectProperty)
   EVT_COMBOBOX(ID_COMBOBOX, ReportPanel::OnComboBoxChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
    
//------------------------------------------------------------------------------
// ReportPanel(wxWindow *parent, const GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Constructs ReportPanel object.
 *
 * @param <parent> input parent.
 * @param <cmd> input command.
 */
//------------------------------------------------------------------------------
ReportPanel::ReportPanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   #if DEBUG_REPORT_PANEL
   MessageInterface::ShowMessage("ReportPanel() entering...\n");
   MessageInterface::ShowMessage("ReportPanel() cmd = " +
                                 cmd->GetTypeName() + "\n");
   #endif
   
   theCommand = cmd;
   mUseUserParam = false;
   mIsReportFileChanged = false;
   mIsParameterChanged = false;
   
   if (theCommand != NULL)
   {
      mObjectTypeList.Add("Spacecraft");
      mObjectTypeList.Add("ImpulsiveBurn");
      Create();
      Show();
      EnableUpdate(false);
   }
}


//------------------------------------------------------------------------------
// ~ReportPanel()
//------------------------------------------------------------------------------
ReportPanel::~ReportPanel()
{
   mObjectTypeList.Clear();
   theGuiManager->UnregisterComboBox("ReportFile", mReportFileComboBox);   
   theGuiManager->UnregisterComboBox("Spacecraft", mSpacecraftComboBox);   
   theGuiManager->UnregisterComboBox("ImpulsiveBurn", mImpBurnComboBox);   
   theGuiManager->UnregisterComboBox("CoordinateSystem", mCoordSysComboBox);
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
void ReportPanel::Create()
{
   #if DEBUG_REPORT_PANEL
   MessageInterface::ShowMessage("ReportPanel::Create() entering...\n");
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
   // report file
   //------------------------------------------------------
   wxStaticText *reportFileLabel =
      new wxStaticText(this, -1, wxT("ReportFile: "), wxDefaultPosition,
                       wxDefaultSize, 0);
   
   mReportFileComboBox = theGuiManager->
      GetReportFileComboBox(this, ID_COMBOBOX, wxSize(170, 20));
   
   wxBoxSizer *reportFileSizer = new wxBoxSizer(wxHORIZONTAL);
   reportFileSizer->Add(reportFileLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   reportFileSizer->Add(mReportFileComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
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
      
      #if DEBUG_REPORT_PANEL
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
   theMiddleSizer->Add(reportFileSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   theMiddleSizer->Add(20, 10, 0, wxALIGN_CENTRE|wxALL, bsize);
   theMiddleSizer->Add(variablesBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

   #if DEBUG_REPORT_PANEL
   MessageInterface::ShowMessage("ReportPanel::Create() exiting...\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ReportPanel::LoadData()
{
   #if DEBUG_REPORT_PANEL
   MessageInterface::ShowMessage("ReportPanel::LoadData() entering...\n");
   #endif

   try
   {
      // Set the pointer for the "Show Script" button
      mObject = theCommand;
      
      // Get ReportFile name
      std::string reportFileName = theCommand->GetRefObjectName(Gmat::SUBSCRIBER);
      
      // Get parameters to report
      StringArray varParamList = theCommand->GetRefObjectNameArray(Gmat::PARAMETER);
      mNumVarParams = varParamList.size();
      
      if (mNumVarParams > 0)
      {      
         Parameter *param;
         
         for (int i=0; i<mNumVarParams; i++)
         {
            param = theGuiInterpreter->GetParameter(varParamList[i]);
            mVarSelectedListBox->Append(varParamList[i].c_str());
         }
         
         mVarSelectedListBox->SetSelection(0);
      }
      
      mUserParamListBox->Deselect(mUserParamListBox->GetSelection());
      mSpacecraftComboBox->SetSelection(0);
      mPropertyListBox->SetSelection(0);
      
      mLastCoordSysName = mCoordSysComboBox->GetString(0);
      
      // show coordinate system or central body
      ShowCoordSystem();
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   #if DEBUG_REPORT_PANEL
   MessageInterface::ShowMessage("ReportPanel::LoadData() exiting...\n");
   #endif
   
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ReportPanel::SaveData()
{
   #if DEBUG_REPORT_PANEL_SAVE
   MessageInterface::ShowMessage("ReportPanel::SaveData() entering...\n");
   #endif
   
   canClose = true;
   
   std::string rfName = mReportFileComboBox->GetValue().c_str();
   
   ReportFile *reportFile =
      (ReportFile*)theGuiInterpreter->GetConfiguredObject(rfName);
   
   try
   {
      // save ReportFile
      if (mIsReportFileChanged)
      {
         #if DEBUG_REPORT_PANEL_SAVE
         MessageInterface::ShowMessage
            ("    rfName=%s, reportFile=%p\n", rfName.c_str(), reportFile);
         #endif
         
         theCommand->SetRefObject(reportFile, Gmat::SUBSCRIBER, rfName, 0);
         mIsReportFileChanged = false;
      }
      
      // save parameters
      if (mIsParameterChanged)
      {
         mIsParameterChanged = false;
         theCommand->TakeAction("Clear");
         Parameter *param = NULL;
         mNumVarParams = mVarSelectedListBox->GetCount();
         
         for (int i=0; i<mNumVarParams; i++)
         {
            std::string selYName =
               std::string(mVarSelectedListBox->GetString(i).c_str());
            param = theGuiInterpreter->GetParameter(selYName);
            theCommand->SetRefObject(param, Gmat::PARAMETER, selYName, 1);
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
   }
   
   #if DEBUG_REPORT_PANEL_SAVE
   MessageInterface::ShowMessage("ReportPanel::SaveData() exiting...\n");
   #endif
   
}


//------------------------------------------------------------------------------
// void OnMoveUpVariable(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportPanel::OnMoveUpVariable(wxCommandEvent& event)
{
   int sel = mVarSelectedListBox->GetSelection();

   if (sel-1 >= 0)
   {
      wxString frontString = mVarSelectedListBox->GetString(sel-1);
      mVarSelectedListBox->SetString(sel-1, mVarSelectedListBox->GetStringSelection());
      mVarSelectedListBox->SetString(sel, frontString);
      mVarSelectedListBox->SetSelection(sel-1);
   }

   mIsParameterChanged = true;
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnMoveDownVariable(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportPanel::OnMoveDownVariable(wxCommandEvent& event)
{
   unsigned int sel = mVarSelectedListBox->GetSelection();
   
   if (sel+1 >= 1 && (sel+1) < mVarSelectedListBox->GetCount())
   {
      wxString rearString = mVarSelectedListBox->GetString(sel+1);
      mVarSelectedListBox->SetString(sel+1, mVarSelectedListBox->GetStringSelection());
      mVarSelectedListBox->SetString(sel, rearString);      
      mVarSelectedListBox->SetSelection(sel+1);
   }
   
   mIsParameterChanged = true;
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnAddVariable(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportPanel::OnAddVariable(wxCommandEvent& event)
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
   
   mIsParameterChanged = true;
}


//------------------------------------------------------------------------------
// void OnRemoveVariable(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportPanel::OnRemoveVariable(wxCommandEvent& event)
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

   mIsParameterChanged = true;
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnClearVariable(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportPanel::OnClearVariable(wxCommandEvent& event)
{
   mVarSelectedListBox->Clear();
   mIsParameterChanged = true;
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnCreateVariable(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportPanel::OnCreateVariable(wxCommandEvent& event)
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
void ReportPanel::OnSelectUserParam(wxCommandEvent& event)
{
   // deselect property
   mPropertyListBox->Deselect(mPropertyListBox->GetSelection());

   mUseUserParam = true;
}


//------------------------------------------------------------------------------
// void OnSelectProperty(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportPanel::OnSelectProperty(wxCommandEvent& event)
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
void ReportPanel::OnComboBoxChange(wxCommandEvent& event)
{
   wxObject *obj = event.GetEventObject();
   
   if (obj == mReportFileComboBox)
   {
      mIsReportFileChanged = true;
      EnableUpdate(true);
   }
   else if (obj == mObjectTypeComboBox)
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
}


//------------------------------------------------------------------------------
// void ShowCoordSystem()
//------------------------------------------------------------------------------
void ReportPanel::ShowCoordSystem()
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
wxString ReportPanel::GetParamName()
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
Parameter* ReportPanel::GetParameter(const wxString &name)
{
   Parameter *param = theGuiInterpreter->GetParameter(std::string(name.c_str()));
   
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
            ("ReportPanel:GetParameter() error occurred!\n%s\n",
             e.GetFullMessage().c_str());
      }
   }
   
   return param;
}


//------------------------------------------------------------------------------
// wxComboBox* GetObjectComboBox()
//------------------------------------------------------------------------------
wxComboBox* ReportPanel::GetObjectComboBox()
{
   if (mSpacecraftComboBox->IsShown())
      return mSpacecraftComboBox;
   else if (mImpBurnComboBox->IsShown())
      return mImpBurnComboBox;
   else
   {
      MessageInterface::ShowMessage
         ("**** INTERNAL ERROR **** Internal Invalid Object ComboBox in "
          "ReportPanel::GetObjectComboBox()\n");
      return NULL;
   }
}
