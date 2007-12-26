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
#include "GmatStaticBoxSizer.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "PlotTypes.hpp"                // for MAX_XY_CURVE
#include "ParameterSelectDialog.hpp"
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
   
   EVT_BUTTON(ID_BUTTON, XyPlotSetupPanel::OnButtonClick)
   EVT_COMBOBOX(ID_COMBOBOX, XyPlotSetupPanel::OnComboBoxChange)
   EVT_CHECKBOX(ID_CHECKBOX, XyPlotSetupPanel::OnCheckBoxChange)
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
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnComboBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnCheckBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mLineColorButton)
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
   else if (event.GetEventObject() == mViewXButton)
   {
      ParameterSelectDialog paramDlg(this, mObjectTypeList,
                                     GuiItemManager::SHOW_PLOTTABLE, false);
      
      paramDlg.SetParamNameArray(mXvarWxStrings);
      paramDlg.ShowModal();
      
      if (paramDlg.HasSelectionChanged())
      {
         mXParamChanged = true;
         EnableUpdate(true);
         mXvarWxStrings = paramDlg.GetParamNameArray();
         
         if (mXvarWxStrings.Count() > 0)
         {
            mXSelectedListBox->Clear();
            for (unsigned int i=0; i<mXvarWxStrings.Count(); i++)
               mXSelectedListBox->Append(mXvarWxStrings[i]);
         }
         else // no selections
         {
            mXSelectedListBox->Clear();
         }
      }
   }
   else if (event.GetEventObject() == mViewYButton)
   {
      ParameterSelectDialog paramDlg(this, mObjectTypeList,
                                     GuiItemManager::SHOW_PLOTTABLE, true);
      
      paramDlg.SetParamNameArray(mYvarWxStrings);
      paramDlg.ShowModal();
      
      if (paramDlg.HasSelectionChanged())
      {
         mYParamChanged = true;
         EnableUpdate(true);
         mYvarWxStrings = paramDlg.GetParamNameArray();
         
         if (mYvarWxStrings.Count() > 0)
         {
            mYSelectedListBox->Clear();
            for (unsigned int i=0; i<mYvarWxStrings.Count(); i++)
               mYSelectedListBox->Append(mYvarWxStrings[i]);
         }
         else // no selections
         {
            mYSelectedListBox->Clear();
         }
      }
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
   Integer bsize = 2; // border size
   
   //------------------------------------------------------
   // plot option
   //------------------------------------------------------
   showPlotCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Show Plot"),
                     wxDefaultPosition, wxSize(100, -1), 0);
   
   showGridCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Show Grid"),
                     wxDefaultPosition, wxSize(100, -1), 0);
   
   //----- Solver Iteration ComboBox
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
   
   //----- line color
   wxStaticText *titleColor =
      new wxStaticText(this, -1, wxT("Color"),
                       wxDefaultPosition, wxSize(40,20), wxALIGN_CENTRE);
   
   mLineColorButton = new wxButton(this, ID_BUTTON, "",
                                   wxDefaultPosition, wxSize(25,20), 0);
   
   mLineColorButton->SetBackgroundColour(mLineColor);
   
   wxFlexGridSizer *option2Sizer = new wxFlexGridSizer(2);
   option2Sizer->Add(showPlotCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   option2Sizer->Add(20, 20);
   option2Sizer->Add(showGridCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   option2Sizer->Add(20, 20);
   option2Sizer->Add(solverIterLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   option2Sizer->Add(mSolverIterComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   option2Sizer->Add(titleColor, 0, wxALIGN_LEFT|wxALL, bsize);
   option2Sizer->Add(mLineColorButton, 0, wxALIGN_LEFT|wxALL, bsize);
   
   GmatStaticBoxSizer *optionSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Options");
   optionSizer->Add(option2Sizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   
   //------------------------------------------------------
   // X selected
   //------------------------------------------------------
   
   mXSelectedListBox =
      new wxListBox(this, ID_LISTBOX, wxDefaultPosition, wxSize(170, 200),
                    emptyList, wxLB_SINGLE);
   
   mViewXButton = new wxButton(this, ID_BUTTON, "View X",
                               wxDefaultPosition, wxDefaultSize, 0);
   
   GmatStaticBoxSizer *xSelectedBoxSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Selected X");
   
   xSelectedBoxSizer->Add(mXSelectedListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   xSelectedBoxSizer->Add(mViewXButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // Y selected
   //------------------------------------------------------
   mYSelectedListBox =
      new wxListBox(this, ID_LISTBOX, wxDefaultPosition, wxSize(170,200),
                    emptyList, wxLB_SINGLE);
   
   mViewYButton = new wxButton(this, ID_BUTTON, "View Y",
                               wxDefaultPosition, wxDefaultSize, 0);
   
   GmatStaticBoxSizer *ySelectedBoxSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Selected Y");
   
   ySelectedBoxSizer->Add(mYSelectedListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   ySelectedBoxSizer->Add(mViewYButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // put in the order
   //------------------------------------------------------
   wxFlexGridSizer *plotSizer = new wxFlexGridSizer(3, 0, 0);
   plotSizer->Add(optionSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   plotSizer->Add(xSelectedBoxSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   plotSizer->Add(ySelectedBoxSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   pageBoxSizer->Add(plotSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
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
      
      showPlotCheckBox->SetValue(mXyPlot->IsActive());
      showGridCheckBox->SetValue(mXyPlot->GetStringParameter("Grid") == "On");
      mSolverIterComboBox->
         SetValue(mXyPlot->GetStringParameter("SolverIterations").c_str());
      
      // get X parameter
      wxString *xParamNames = new wxString[1];
      xParamNames[0] = mXyPlot->GetStringParameter("IndVar").c_str();
      mXvarWxStrings.Add(xParamNames[0]);
      
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
            mYvarWxStrings.Add(yParamNames[i]);
            
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
   }
   
}



