//$Id$
//------------------------------------------------------------------------------
//                              XyPlotSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/02/02
/**
 * Implements XyPlotSetupPanel class.
 */
//------------------------------------------------------------------------------

#include "XyPlotSetupPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "PlotTypes.hpp"                // for MAX_XY_CURVE
#include "ParameterCreateDialog.hpp"
#include "ParameterInfo.hpp"            // for GetDepObjectType()
#include "MessageInterface.hpp"

#include "wx/colordlg.h"                // for wxColourDialog


//#define DEBUG_XYPLOT_PANEL 1
//#define DEBUG_XYPLOT_PANEL_LOAD 1
//#define DEBUG_XYPLOT_PANEL_SAVE 1

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(XyPlotSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
   
   EVT_BUTTON(ADD_X, XyPlotSetupPanel::OnAddX)
   EVT_BUTTON(ADD_Y, XyPlotSetupPanel::OnAddY)
   EVT_BUTTON(REMOVE_X, XyPlotSetupPanel::OnRemoveX)
   EVT_BUTTON(REMOVE_Y, XyPlotSetupPanel::OnRemoveY)
   EVT_BUTTON(CLEAR_Y, XyPlotSetupPanel::OnClearY)
   EVT_BUTTON(CREATE_VARIABLE, XyPlotSetupPanel::OnCreateVariable)
   EVT_BUTTON(LINE_COLOR_BUTTON, XyPlotSetupPanel::OnLineColorClick)
   EVT_LISTBOX(USER_PARAM_LISTBOX, XyPlotSetupPanel::OnSelectUserParam)
   EVT_LISTBOX(PROPERTY_LISTBOX, XyPlotSetupPanel::OnSelectProperty)
   EVT_LISTBOX(Y_SEL_LISTBOX, XyPlotSetupPanel::OnSelectY)
   EVT_COMBOBOX(ID_COMBOBOX, XyPlotSetupPanel::OnComboBoxChange)
   EVT_CHECKBOX(CHECKBOX, XyPlotSetupPanel::OnCheckBoxChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
//------------------------------------------------------------------------------
// XyPlotSetupPanel(wxWindow *parent, const wxString &subscriberName)
//------------------------------------------------------------------------------
/**
 * Constructs XyPlotSetupPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the XyPlotSetupPanel GUI
 */
//------------------------------------------------------------------------------
XyPlotSetupPanel::XyPlotSetupPanel(wxWindow *parent,
                                   const wxString &subscriberName)
   : GmatPanel(parent)
{
   #if DEBUG_XYPLOT_PANEL
   MessageInterface::ShowMessage("XyPlotSetupPanel() entering...\n");
   MessageInterface::ShowMessage("XyPlotSetupPanel() subscriberName = " +
                                 std::string(subscriberName.c_str()) + "\n");
   #endif
   
   Subscriber *subscriber = (Subscriber*)
      theGuiInterpreter->GetConfiguredObject(std::string(subscriberName.c_str()));

   mXyPlot = (TsPlot*)subscriber;

   mXParamChanged = false;
   mYParamChanged = false;
   mIsColorChanged = false;
   mUseUserParam = false;
   mNumXParams = 0;
   mNumYParams = 0;
   
   mLineColor.Set(0,0,0); //black for now
   mColorMap.clear();
   
   mObjectTypeList.Add("Spacecraft");
   mObjectTypeList.Add("ImpulsiveBurn");
   
   Create();
   Show();
   
   // force saving data
   if (mNumXParams == 0 || mNumYParams <= 0)
   {
      EnableUpdate(true);
      mXParamChanged = true;
      mYParamChanged = true;
      showPlotCheckBox->SetValue(true);
   }
   
   // Listen for Spacecraft and Parameter name change
   theGuiManager->AddToResourceUpdateListeners(this);
   
   #if DEBUG_XYPLOT_PANEL
   MessageInterface::ShowMessage
      ("   mNumXParams=%d, mNumYParams=%d, mXParamChanged=%d, mYParamChanged=%d\n",
       mNumXParams, mNumYParams, mXParamChanged, mYParamChanged);
   #endif
}


//------------------------------------------------------------------------------
// ~XyPlotSetupPanel()
//------------------------------------------------------------------------------
XyPlotSetupPanel::~XyPlotSetupPanel()
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
bool XyPlotSetupPanel::PrepareObjectNameChange()
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
void XyPlotSetupPanel::ObjectNameChanged(Gmat::ObjectType type,
                                         const wxString &oldName,
                                         const wxString &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("XyPlotPanel::ObjectNameChanged() type=%d, oldName=<%s>, "
       "newName=<%s>, mDataChanged=%d\n", type, oldName.c_str(), newName.c_str(),
       mDataChanged);
   #endif
   
   if (type != Gmat::SPACECRAFT && type != Gmat::PARAMETER)
      return;
   
   mXParamChanged = false;
   mYParamChanged = false;
   mIsColorChanged = false;
   mUseUserParam = false;
   mNumXParams = 0;
   mNumYParams = 0;
   
   mColorMap.clear();
   
   LoadData();
   
   // force saving data
   if (mNumXParams == 0 || mNumYParams <= 0)
   {
      EnableUpdate(true);
      mXParamChanged = true;
      mYParamChanged = true;
      showPlotCheckBox->SetValue(true);
   }
   
   // We don't need to save data if object name changed from the resouce tree
   // while this panel is opened, since base code already has new name
   EnableUpdate(false);
}


//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnAddX(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnAddX(wxCommandEvent& event)
{
   wxString oldParam = mXSelectedListBox->GetStringSelection();
   wxString newParam = GetParamName();
   if (newParam == "")
      return;
   
   if (!oldParam.IsSameAs(newParam))
   {
      // Get the parameter
      Parameter *param = GetParameter(newParam);
      if (param == NULL)
         return;
      
      if (param->IsPlottable())
      {
         // empty listbox first, only one parameter is allowed
         mXSelectedListBox->Clear();
         mXSelectedListBox->Append(newParam);
         mXSelectedListBox->SetStringSelection(newParam);
         
         mXParamChanged = true;
         EnableUpdate(true);
      }
      else
      {
         wxLogMessage("Selected X parameter:%s is not plottable. Please select "
                      "another parameter\n", newParam.c_str());
      }
   }
}


//------------------------------------------------------------------------------
// void OnAddY(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnAddY(wxCommandEvent& event)
{
   // get string in first list and then search for it in the second list
   wxString newParam = GetParamName();
   if (newParam == "")
      return;
   
   int found = mYSelectedListBox->FindString(newParam);
   
   // if the string wasn't found in the second list, insert it
   if (found == wxNOT_FOUND)
   {
      // Get a parameter
      Parameter *param = GetParameter(newParam);
      
      if (param->IsPlottable())
      {
         mYSelectedListBox->Append(newParam);
         mYSelectedListBox->SetStringSelection(newParam);
         ShowParameterOption(newParam, true);
         //Show next parameter
         mPropertyListBox->SetSelection(mPropertyListBox->GetSelection() + 1);
         OnSelectProperty(event);
         mYParamChanged = true;
         EnableUpdate(true);
      }
      else
      {
         wxLogMessage("Selected Y parameter:%s is not plottable.\nPlease select "
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
// void OnRemoveX(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnRemoveX(wxCommandEvent& event)
{
   int sel = mXSelectedListBox->GetSelection();
   mXSelectedListBox->Delete(sel);
   mXParamChanged = true;
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnRemoveY(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnRemoveY(wxCommandEvent& event)
{
   int sel = mYSelectedListBox->GetSelection();
   mYSelectedListBox->Delete(sel);
   
   if (sel-1 < 0)
   {
      mYSelectedListBox->SetSelection(0);
      if (mYSelectedListBox->GetCount() == 0)
         ShowParameterOption("", false); // hide parameter color, etc
      else
         ShowParameterOption(mYSelectedListBox->GetStringSelection(), true);
   }
   else
   {
      mYSelectedListBox->SetSelection(sel-1);
      ShowParameterOption(mYSelectedListBox->GetStringSelection(), true);
   }

   mYParamChanged = true;
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnClearY(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnClearY(wxCommandEvent& event)
{
   mYSelectedListBox->Clear();
   ShowParameterOption("", false);
   mYParamChanged = true;
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnSelectUserParam(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnSelectUserParam(wxCommandEvent& event)
{
   mAddXButton->Enable();
   mAddYButton->Enable();
   
   // deselect property
   mPropertyListBox->Deselect(mPropertyListBox->GetSelection());
   
   mUseUserParam = true;
}


//------------------------------------------------------------------------------
// void OnSelectProperty(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnSelectProperty(wxCommandEvent& event)
{
   mAddXButton->Enable();
   mAddYButton->Enable();
   
   // deselect user param
   mUserParamListBox->Deselect(mUserParamListBox->GetSelection());

   // show coordinate system or central body
   ShowCoordSystem();
   mUseUserParam = false;
}


//------------------------------------------------------------------------------
// void OnSelectY(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnSelectY(wxCommandEvent& event)
{
   ShowParameterOption(mYSelectedListBox->GetStringSelection(), true);
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnComboBoxChange(wxCommandEvent& event)
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
// void OnCreateVariable(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnCreateVariable(wxCommandEvent& event)
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
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnCheckBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnLineColorClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnLineColorClick(wxCommandEvent& event)
{
   wxColourData data;
   data.SetColour(mLineColor);
   wxColourDialog dialog(this, &data);
   dialog.Center();
   
   if (dialog.ShowModal() == wxID_OK)
   {
      mSelYName = std::string(mYSelectedListBox->GetStringSelection().c_str());
      
      mLineColor = dialog.GetColourData().GetColour();
      mLineColorButton->SetBackgroundColour(mLineColor);
      
      mColorMap[mSelYName].Set(mLineColor.Red(),
                               mLineColor.Green(),
                               mLineColor.Blue());
      
      EnableUpdate(true);
      mIsColorChanged = true;
   }
}

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void XyPlotSetupPanel::Create()
{
   wxArrayString emptyList;
   Integer bsize = 1; // border size
   
   // empty text for spacing line
   wxStaticText *emptyText =
      new wxStaticText(this, -1, wxT(""),
                       wxDefaultPosition, wxSize(-1, -1), 0);
   
   //------------------------------------------------------
   // X box label (1st column)
   //------------------------------------------------------
   
   wxStaticBox *xSelectedStaticBox = new wxStaticBox( this, -1, wxT("") );

   wxStaticText *titleXText =
      new wxStaticText(this, -1, wxT("Selected X"),
                       wxDefaultPosition, wxSize(-1,-1), 0);

   mXSelectedListBox =
      new wxListBox(this, X_SEL_LISTBOX, wxDefaultPosition, wxSize(170,260), //0,
                    emptyList, wxLB_SINGLE);
   
   wxStaticBoxSizer *xSelectedBoxSizer =
      new wxStaticBoxSizer(xSelectedStaticBox, wxVERTICAL);
   
   xSelectedBoxSizer->Add(titleXText, 0, wxALIGN_CENTRE|wxALL, bsize);
   xSelectedBoxSizer->Add(mXSelectedListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // add, remove X buttons (2nd column)
   //------------------------------------------------------
   wxSize buttonSize;
   #if __WXMAC__
   buttonSize = wxSize(40, 20);
   #else
   buttonSize = wxSize(20, 20);
   #endif
   
   mAddXButton =
      new wxButton(this, ADD_X, wxT("<--"), wxDefaultPosition, buttonSize, 0);
   wxButton *removeXButton =
      new wxButton(this, REMOVE_X, wxT("-->"), wxDefaultPosition, buttonSize, 0);
   
   wxBoxSizer *xButtonsBoxSizer = new wxBoxSizer(wxVERTICAL);
   xButtonsBoxSizer->Add(30, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   xButtonsBoxSizer->Add(mAddXButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   xButtonsBoxSizer->Add(removeXButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // parameters box (3rd column)
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
                           mObjectTypeList);
   
   #if DEBUG_XYPLOT_PANEL
   MessageInterface::ShowMessage
      ("XyPlotSetupPanel::Create() got mParamBoxSizer from theGuiManager\n"
       "   createVarButton=%p,  mUserParamListBox=%p, mPropertyListBox=%p\n",
       createVarButton, mUserParamListBox, mPropertyListBox);
   #endif
   
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
      
      #if DEBUG_XYPLOT_PANEL
      MessageInterface::ShowMessage
         ("   mNumSc=%d, mNumImpBurn=%d, mNumScProperty=%d, mNumImpBurnProperty=%d\n",
          mNumSc, mNumImpBurn, mNumScProperty, mNumImpBurnProperty);
      MessageInterface::ShowMessage
         ("   mSpacecraftPropertyList.Count()=%d\n", mSpacecraftPropertyList.Count());
      #endif
   }
   
   //------------------------------------------------------
   // add, remove, clear Y buttons (4th column)
   //------------------------------------------------------
   
   mAddYButton =
      new wxButton(this, ADD_Y, wxT("-->"), wxDefaultPosition, buttonSize, 0);
   wxButton *removeYButton =
      new wxButton(this, REMOVE_Y, wxT("<--"), wxDefaultPosition, buttonSize, 0);
   wxButton *clearYButton =
      new wxButton(this, CLEAR_Y, wxT("<="), wxDefaultPosition, buttonSize, 0);
   
   clearYButton->SetToolTip("Remove All");
   
   wxBoxSizer *yButtonsBoxSizer = new wxBoxSizer(wxVERTICAL);
   yButtonsBoxSizer->Add(mAddYButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   yButtonsBoxSizer->Add(removeYButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   yButtonsBoxSizer->Add(clearYButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // Y box label (5th column)
   //------------------------------------------------------
   wxStaticBox *ySelectedStaticBox = new wxStaticBox( this, -1, wxT("") );

   wxStaticText *titleYText =
      new wxStaticText(this, -1, wxT("Selected Y"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   mYSelectedListBox =
      new wxListBox(this, Y_SEL_LISTBOX, wxDefaultPosition, wxSize(170,260), //0,
                    emptyList, wxLB_SINGLE);
   
   wxStaticBoxSizer *ySelectedBoxSizer =
      new wxStaticBoxSizer(ySelectedStaticBox, wxVERTICAL);
   
   ySelectedBoxSizer->Add(titleYText, 0, wxALIGN_CENTRE|wxALL, bsize);
   ySelectedBoxSizer->Add(mYSelectedListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // line color (6th column)
   //------------------------------------------------------
   wxStaticText *titleColor =
      new wxStaticText(this, -1, wxT("Color"),
                       wxDefaultPosition, wxSize(40,20), wxALIGN_CENTRE);
   
   mLineColorButton = new wxButton(this, LINE_COLOR_BUTTON, "",
                                   wxDefaultPosition, wxSize(25,20), 0);

   mLineColorButton->SetBackgroundColour(mLineColor);
   wxFlexGridSizer *paramOptionBoxSizer1 = new wxFlexGridSizer(2, 0, 0);
   paramOptionBoxSizer1->Add(titleColor, 0, wxALIGN_LEFT|wxALL, bsize);
   paramOptionBoxSizer1->Add(mLineColorButton, 0, wxALIGN_LEFT|wxALL, bsize);

   mParamOptionBoxSizer = new wxBoxSizer(wxVERTICAL);
   mParamOptionBoxSizer->Add(paramOptionBoxSizer1, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //------------------------------------------------------
   // plot option
   //------------------------------------------------------
   showPlotCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Show Plot"),
                     wxDefaultPosition, wxSize(100, -1), 0);
   
   showGridCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Show Grid"),
                     wxDefaultPosition, wxSize(100, -1), 0);
   
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
   
   wxBoxSizer *plotOptionBoxSizer = new wxBoxSizer(wxVERTICAL);
   plotOptionBoxSizer->Add(showPlotCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   plotOptionBoxSizer->Add(showGridCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   plotOptionBoxSizer->Add(solverIterOptionSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //------------------------------------------------------
   // put in the order
   //------------------------------------------------------
   mFlexGridSizer = new wxFlexGridSizer(5, 0, 0);
   mFlexGridSizer->Add(xSelectedBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(xButtonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(mParamBoxSizer, 0, wxALIGN_TOP|wxALL, bsize);
   mFlexGridSizer->Add(yButtonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(ySelectedBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   mFlexGridSizer->Add(plotOptionBoxSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   mFlexGridSizer->Add(emptyText, 0, wxALIGN_LEFT|wxALL, bsize);
   mFlexGridSizer->Add(emptyText, 0, wxALIGN_LEFT|wxALL, bsize);
   mFlexGridSizer->Add(emptyText, 0, wxALIGN_LEFT|wxALL, bsize);
   mFlexGridSizer->Add(mParamOptionBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   pageBoxSizer->Add(mFlexGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void XyPlotSetupPanel::LoadData()
{
   // load data from the core engine
   
   try
   {   
      // Set the pointer for the "Show Script" button
      mObject = mXyPlot;
      mLastCoordSysName = mCoordSysComboBox->GetString(0);
      
      showPlotCheckBox->SetValue(mXyPlot->IsActive());
      showGridCheckBox->SetValue(mXyPlot->GetStringParameter("Grid") == "On");
      mSolverIterComboBox->
         SetValue(mXyPlot->GetStringParameter("SolverIterations").c_str());
      
      // get X parameter
      wxString *xParamNames = new wxString[1];
      xParamNames[0] = mXyPlot->GetStringParameter("IndVar").c_str();
      
      #if DEBUG_XYPLOT_PANEL_LOAD
      MessageInterface::ShowMessage
         ("XyPlotSetupPanel::LoadData() xParamNames[0] = %s\n",
          xParamNames[0].c_str());
      #endif
      
      if (!xParamNames[0].IsSameAs(""))
      {
         mNumXParams = 1;
         mXSelectedListBox->Set(1, xParamNames);
      }
      
      // get Y parameters
      StringArray yParamList = mXyPlot->GetStringArrayParameter("Add");
      mNumYParams = yParamList.size();
      //MessageInterface::ShowMessage("XyPlotSetupPanel::LoadData() mNumYParams = %d\n",
      //                              mNumYParams);
      
      delete [] xParamNames;
      
      if (mNumYParams > 0)
      {
         wxString *yParamNames = new wxString[mNumYParams];
         Parameter *param;
         
         for (int i=0; i<mNumYParams; i++)
         {
            #if DEBUG_XYPLOT_PANEL_LOAD
               MessageInterface::ShowMessage
                  ("XyPlotSetupPanel::LoadData() y param = %s\n",
                   yParamList[i].c_str());
            #endif
            yParamNames[i] = yParamList[i].c_str();
            
            // get parameter color            
            param = (Parameter*)theGuiInterpreter->GetConfiguredObject(yParamList[i]);
            if (param != NULL)
            {
               mColorMap[yParamList[i]]
                  = RgbColor(param->GetUnsignedIntParameter("Color"));               
            }
         }
         
         mYSelectedListBox->Set(mNumYParams, yParamNames);
         mYSelectedListBox->SetSelection(0);
         delete [] yParamNames;
         
         // show parameter option
         ShowParameterOption(mYSelectedListBox->GetStringSelection(), true);
      }
      else
      {
         ShowParameterOption("", false);
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      //MessageInterface::ShowMessage
      //   ("XyPlotSetupPanel:LoadData() error occurred!\n%s\n", e.GetFullMessage().c_str());
   }
   
   mUserParamListBox->Deselect(mUserParamListBox->GetSelection());
   mSpacecraftComboBox->SetSelection(0);
   mPropertyListBox->SetSelection(0);
   
   // show coordinate system or central body
   ShowCoordSystem();
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void XyPlotSetupPanel::SaveData()
{
   #if DEBUG_XYPLOT_PANEL_SAVE
   MessageInterface::ShowMessage
      ("XyPlotSetupPanel::SaveData() mNumXParams=%d, mNumYParams=%d, "
       "mXParamChanged=%d, mYParamChanged=%d\n",
       mNumXParams, mNumYParams, mXParamChanged, mYParamChanged);
   #endif
   
   canClose = true;
   
   mXyPlot->Activate(showPlotCheckBox->IsChecked());
   
   if (showGridCheckBox->IsChecked())
      mXyPlot->SetStringParameter("Grid", "On");
   else
      mXyPlot->SetStringParameter("Grid", "Off");
   
   mXyPlot->SetStringParameter("SolverIterations",
                               mSolverIterComboBox->GetValue().c_str());
   
   // set X parameter
   if (mXParamChanged)
   {
      if (mXSelectedListBox->GetCount() == 0 && showPlotCheckBox->IsChecked())
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "X parameter not selected. "
             "The plot will not be activated.");
         mXyPlot->Activate(false);
      }
      else
      {
         std::string selXName = std::string(mXSelectedListBox->GetString(0).c_str());
         mXyPlot->SetStringParameter("IndVar", selXName);
         mXParamChanged = false;
      }
   }
   
   // set Y parameters
   if (mYParamChanged)
   {
      mIsColorChanged = true;
      
      int numYParams = mYSelectedListBox->GetCount();

      mNumYParams = numYParams;
      if (mNumYParams == 0 && showPlotCheckBox->IsChecked())
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_,"Y parameters not selected. "
             "The plot will not be activated.");
         mXyPlot->Activate(false);
      }
      else if (numYParams > GmatPlot::MAX_XY_CURVE)
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "Selected Y parameter count is greater than 6.\n"
             "First 6 parameters will be plotted.");
         
         mNumYParams = GmatPlot::MAX_XY_CURVE;
      }
      else
      {
         mNumYParams = numYParams;
         mYParamChanged = false;
      }
      
      if (mNumYParams >= 0) // >=0 because the list needs to be cleared
      {
         mXyPlot->TakeAction("Clear");
         for (int i=0; i<mNumYParams; i++)
         {
            #if DEBUG_XYPLOT_PANEL_SAVE
               MessageInterface::ShowMessage
                  ("XyPlotSetupPanel::SaveData() DepVar = %s\n",
                   mYSelectedListBox->GetString(i).c_str());
            #endif
            std::string selYName = std::string(mYSelectedListBox->GetString(i).c_str());
            mXyPlot->SetStringParameter("Add", selYName, i);
         }
      }
   }
   
   // save color
   if (mIsColorChanged)
   {
      mIsColorChanged = false;
      Parameter *param;
      UnsignedInt intColor;
      
      for (int i=0; i<mNumYParams; i++)
      {
         mSelYName = std::string(mYSelectedListBox->GetString(i).c_str());
         param = (Parameter*)theGuiInterpreter->GetConfiguredObject(mSelYName);

         if (param != NULL)
         {
            intColor = mColorMap[mSelYName].GetIntColor();
            param->SetUnsignedIntParameter("Color", intColor);
         }
      }
   }
}

//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void ShowParameterOption(const wxString &name, bool show = true)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::ShowParameterOption(const wxString &name, bool show)
{
   #if DEBUG_XYPLOT_PANEL
      MessageInterface::ShowMessage
         ("XyPlotSetupPanel::ShowParameterOption() name=%s\n", name.c_str());
   #endif

   if (!name.IsSameAs(""))
   {
      mSelYName = std::string(name.c_str());
      RgbColor color;
      
      // if parameter not in the map
      if (mColorMap.find(mSelYName) == mColorMap.end())
      {
         Parameter *param = (Parameter*)theGuiInterpreter->GetConfiguredObject(mSelYName);

         if (param != NULL)
         {
            UnsignedInt intColor = param->GetUnsignedIntParameter("Color");
            color.Set(intColor);

            mColorMap[mSelYName] = color;
         }
      }
      else
      {
         color = mColorMap[mSelYName];
      }
      
      #if DEBUG_XYPLOT_PANEL
      MessageInterface::ShowMessage
         ("   intColor=%u\n", mColorMap[mSelYName].GetIntColor());
      #endif
      
      mLineColor.Set(color.Red(), color.Green(), color.Blue());
      mLineColorButton->SetBackgroundColour(mLineColor);
      
      mFlexGridSizer->Show(mParamOptionBoxSizer, show);
   }
   else
   {
      mFlexGridSizer->Show(mParamOptionBoxSizer, false);
   }
   
}


//------------------------------------------------------------------------------
// void ShowCoordSystem()
//------------------------------------------------------------------------------
void XyPlotSetupPanel::ShowCoordSystem()
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
wxString XyPlotSetupPanel::GetParamName()
{
   wxString paramName;
   
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
Parameter* XyPlotSetupPanel::GetParameter(const wxString &name)
{
   
   Parameter *param = (Parameter*)
      theGuiInterpreter->GetConfiguredObject(std::string(name.c_str()));
   
   // create a parameter if it does not exist
   if (param == NULL)
   {
      wxComboBox *objComboBox = GetObjectComboBox();
      if (objComboBox == NULL)
         return NULL;
      
      std::string paramName(name.c_str());
      std::string ownerName(objComboBox->GetStringSelection().c_str());
      std::string propName(mPropertyListBox->GetStringSelection().c_str());
      std::string depObjName = "";
      
      if (mCoordSysComboBox->IsShown())
         depObjName = std::string(mCoordSysComboBox->GetStringSelection().c_str());
      else if (mCentralBodyComboBox->IsShown())
         depObjName = std::string(mCentralBodyComboBox->GetStringSelection().c_str());
      
      #ifdef DEBUG_XYPLOT_PANEL
      MessageInterface::ShowMessage
         ("XyPlotSetupPanel::GetParameter() paramName : %s does not exist. "
          "So creating.\n", paramName.c_str());
      #endif
      
      try
      {
         param = theGuiInterpreter->CreateParameter(propName, paramName);
         param->SetRefObjectName(Gmat::SPACECRAFT, ownerName);
         
         if (depObjName != "")
            param->SetStringParameter("DepObject", depObjName);
         
         if (param->IsCoordSysDependent())
            param->SetRefObjectName(Gmat::COORDINATE_SYSTEM, depObjName);
         else if (param->IsOriginDependent())
            param->SetRefObjectName(Gmat::SPACE_POINT, depObjName);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage
            ("XyPlotSetupPanel:GetParameter() error occurred!\n%s\n",
             e.GetFullMessage().c_str());
      }
   }
   
   return param;
}


//------------------------------------------------------------------------------
// wxComboBox* GetObjectComboBox()
//------------------------------------------------------------------------------
wxComboBox* XyPlotSetupPanel::GetObjectComboBox()
{
   if (mSpacecraftComboBox->IsShown())
      return mSpacecraftComboBox;
   else if (mImpBurnComboBox->IsShown())
      return mImpBurnComboBox;
   else
   {
      MessageInterface::ShowMessage
         ("**** INTERNAL ERROR **** Internal Invalid Object ComboBox in "
          "XyPlotSetupPanel::GetObjectComboBox()\n");
      return NULL;
   }
}
