//$Header$
//------------------------------------------------------------------------------
//                              OpenGlPlotSetupPanel
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
 * Implements OpenGlPlotSetupPanel class.
 */
//------------------------------------------------------------------------------

#include "OpenGlPlotSetupPanel.hpp"
#include "MessageInterface.hpp"

#include "wx/colordlg.h"   // for wxColourDialog

#define DEBUG_OPENGL_PANEL 0

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(OpenGlPlotSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)

   EVT_BUTTON(ADD_SC_BUTTON, OpenGlPlotSetupPanel::OnAddSpacecraft)
   EVT_BUTTON(REMOVE_SC_BUTTON, OpenGlPlotSetupPanel::OnRemoveSpacecraft)
   EVT_BUTTON(CLEAR_SC_BUTTON, OpenGlPlotSetupPanel::OnClearSpacecraft)
   EVT_BUTTON(SC_ORBIT_COLOR_BUTTON, OpenGlPlotSetupPanel::OnOrbitColorClick)
   EVT_BUTTON(SC_TARGET_COLOR_BUTTON, OpenGlPlotSetupPanel::OnTargetColorClick)
   EVT_LISTBOX(SC_SEL_LISTBOX, OpenGlPlotSetupPanel::OnSelectSpacecraft)
   EVT_CHECKBOX(PLOT_CHECKBOX, OpenGlPlotSetupPanel::OnPlotCheckBoxChange)
   EVT_CHECKBOX(WIREFRAME_CHECKBOX, OpenGlPlotSetupPanel::OnWireFrameCheckBoxChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
//------------------------------------------------------------------------------
// OpenGlPlotSetupPanel(wxWindow *parent, const wxString &subscriberName)
//------------------------------------------------------------------------------
/**
 * Constructs OpenGlPlotSetupPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the OpenGlPlotSetupPanel GUI
 */
//------------------------------------------------------------------------------
OpenGlPlotSetupPanel::OpenGlPlotSetupPanel(wxWindow *parent,
                                           const wxString &subscriberName)
   : GmatPanel(parent)
{
#if DEBUG_OPENGL_PANEL
   MessageInterface::ShowMessage("OpenGlPlotSetupPanel() entering...\n");
   MessageInterface::ShowMessage("OpenGlPlotSetupPanel() subscriberName = " +
                                 std::string(subscriberName.c_str()) + "\n");
#endif
   mSubscriber =
      theGuiInterpreter->GetSubscriber(std::string(subscriberName.c_str()));
   mOpenGlPlot = (OpenGlPlot*)mSubscriber;

   mOrbitColorMap.clear();
   mTargetColorMap.clear();
   
   Create();
   Show();
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnAddSpacecraft(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnAddSpacecraft(wxCommandEvent& event)
{
   // get string in first list and then search for it
   // in the second list
   wxString s = mScAvailableListBox->GetStringSelection();
   int found = mScSelectedListBox->FindString(s);
    
   // if the string wasn't found in the second list, insert it
   if (found == wxNOT_FOUND)
   {
      mScSelectedListBox->Append(s);
      mScSelectedListBox->SetStringSelection(s);
      ShowSpacecraftOption(s, true);
      mIsScChanged = true;
      theApplyButton->Enable();
   }
}

//------------------------------------------------------------------------------
// void OnRemoveSpacecraft(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnRemoveSpacecraft(wxCommandEvent& event)
{
   int sel = mScSelectedListBox->GetSelection();
   mScSelectedListBox->Delete(sel);

   if (sel-1 < 0)
   {
      mScSelectedListBox->SetSelection(0);
      if (mScSelectedListBox->GetCount() == 0)
         ShowSpacecraftOption("", false); // hide spacecraft color, etc
      else
         ShowSpacecraftOption(mScSelectedListBox->GetStringSelection(), true);
   }
   else
   {
      mScSelectedListBox->SetSelection(sel-1);
      ShowSpacecraftOption(mScSelectedListBox->GetStringSelection(), true);
   }
   
   mIsScChanged = true;
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnClearSpacecraft(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnClearSpacecraft(wxCommandEvent& event)
{
   mScSelectedListBox->Clear();
   ShowSpacecraftOption("", false);
   mIsScChanged = true;
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnSelectSpacecraft(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnSelectSpacecraft(wxCommandEvent& event)
{
   ShowSpacecraftOption(mScSelectedListBox->GetStringSelection(), true);
}

//------------------------------------------------------------------------------
// void OnPlotCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnPlotCheckBoxChange(wxCommandEvent& event)
{
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnWireFrameCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnWireFrameCheckBoxChange(wxCommandEvent& event)
{
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnOrbitColorClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnOrbitColorClick(wxCommandEvent& event)
{
   wxColourData data;
   data.SetColour(mScOrbitColor);

   wxColourDialog dialog(this, &data);
   //dialog.CenterOnParent();
   dialog.Center();
   
   if (dialog.ShowModal() == wxID_OK)
   {
      mSelScName = std::string(mScSelectedListBox->GetStringSelection().c_str());
      
      mScOrbitColor = dialog.GetColourData().GetColour();
      mScOrbitColorButton->SetBackgroundColour(mScOrbitColor);
      
      mOrbitColorMap[mSelScName].Set(mScOrbitColor.Red(),
                                     mScOrbitColor.Green(),
                                     mScOrbitColor.Blue());

#if DEBUG_OPENGL_PANEL
      MessageInterface::ShowMessage("OnOrbitColorClick() r=%d g=%d b=%d\n",
                                    mScOrbitColor.Red(), mScOrbitColor.Green(),
                                    mScOrbitColor.Blue());

      MessageInterface::ShowMessage("OnOrbitColorClick() UnsignedInt=%d\n",
                                    mOrbitColorMap[mSelScName].GetIntColor());
#endif
      theApplyButton->Enable();
      mIsColorChanged = true;
   }
}

//------------------------------------------------------------------------------
// void OnTargetColorClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnTargetColorClick(wxCommandEvent& event)
{
   wxColourData data;
   data.SetColour(mScTargetColor);

   wxColourDialog dialog(this, &data);
   //dialog.CenterOnParent();
   dialog.Center();
   
   if (dialog.ShowModal() == wxID_OK)
   {
      mSelScName = std::string(mScSelectedListBox->GetStringSelection().c_str());
      
      MessageInterface::ShowMessage("OnTargetColorClick()\n");
      mScTargetColor = dialog.GetColourData().GetColour();
      mScTargetColorButton->SetBackgroundColour(mScTargetColor);
      mTargetColorMap[mSelScName].Set(mScTargetColor.Red(),
                                      mScTargetColor.Green(),
                                      mScTargetColor.Blue());
      
      theApplyButton->Enable();
      mIsColorChanged = true;
   }
}

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::Create()
{
#if DEBUG_OPENGL_PANEL
   MessageInterface::ShowMessage("OpenGlPlotSetupPanel::Create() entering...\n");
#endif

   wxString emptyList[] = {};
   Integer bsize = 3; // border size
   
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   mFlexGridSizer = new wxFlexGridSizer(5, 0, 0);

   //------------------------------------------------------
   // plot option, (1st column)
   //------------------------------------------------------
   plotCheckBox =
      new wxCheckBox(this, PLOT_CHECKBOX, wxT("Show Plot"),
                     wxDefaultPosition, wxSize(100, -1), 0);
    
   wireFrameCheckBox =
      new wxCheckBox(this, WIREFRAME_CHECKBOX, wxT("Draw WireFrame"),
                     wxDefaultPosition, wxSize(100, -1), 0);
    
   wxBoxSizer *plotOptionBoxSizer = new wxBoxSizer(wxVERTICAL);
   plotOptionBoxSizer->Add(plotCheckBox, 0, wxALIGN_CENTER|wxALL, bsize);
   plotOptionBoxSizer->Add(wireFrameCheckBox, 0, wxALIGN_CENTER|wxALL, bsize);
            
   //------------------------------------------------------
   // available spacecraft list (2th column)
   //------------------------------------------------------
   wxBoxSizer *mScAvailableBoxSizer = new wxBoxSizer(wxVERTICAL);
    
   wxStaticText *titleAvailable =
      new wxStaticText(this, -1, wxT("Available Spacecraft"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
    
   mScAvailableListBox =
      theGuiManager->GetSpacecraftListBox(this, wxSize(150,200));
    
   mScAvailableBoxSizer->Add(titleAvailable, 0, wxALIGN_CENTRE|wxALL, bsize);
   mScAvailableBoxSizer->Add(mScAvailableListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   //------------------------------------------------------
   // add, remove, clear Y buttons (3rd column)
   //------------------------------------------------------
   addScButton = new wxButton(this, ADD_SC_BUTTON, wxT("-->"),
                              wxDefaultPosition, wxSize(20,20), 0);

   removeScButton = new wxButton(this, REMOVE_SC_BUTTON, wxT("<--"),
                                 wxDefaultPosition, wxSize(20,20), 0);
    
   clearScButton = new wxButton(this, CLEAR_SC_BUTTON, wxT("<="),
                                wxDefaultPosition, wxSize(20,20), 0);
    
   wxBoxSizer *arrowButtonsBoxSizer = new wxBoxSizer(wxVERTICAL);
   arrowButtonsBoxSizer->Add(addScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsBoxSizer->Add(removeScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsBoxSizer->Add(clearScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   //------------------------------------------------------
   // selected spacecraft list (4th column)
   //------------------------------------------------------
   wxStaticText *titleSelected =
      new wxStaticText(this, -1, wxT("Selected Spacecraft"),
                       wxDefaultPosition, wxSize(-1,-1), 0);

   mScSelectedListBox =
      new wxListBox(this, SC_SEL_LISTBOX, wxDefaultPosition,
                    wxSize(150,200), 0, emptyList, wxLB_SINGLE);
        
   wxBoxSizer *mScSelectedBoxSizer = new wxBoxSizer(wxVERTICAL);
   mScSelectedBoxSizer->Add(titleSelected, 0, wxALIGN_CENTRE|wxALL, bsize);
   mScSelectedBoxSizer->Add(mScSelectedListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   //------------------------------------------------------
   // spacecraft color (5th column)
   //------------------------------------------------------
   wxStaticText *orbitColorLabel =
      new wxStaticText(this, -1, wxT("Orbit Color"),
                       wxDefaultPosition, wxSize(80,20), wxALIGN_CENTRE);
   wxStaticText *targetColorLabel =
      new wxStaticText(this, -1, wxT("Target Color"),
                       wxDefaultPosition, wxSize(80,20), wxALIGN_CENTRE);
   
   mScOrbitColorButton = new wxButton(this, SC_ORBIT_COLOR_BUTTON, "",
                                      wxDefaultPosition, wxSize(25,20), 0);

   mScTargetColorButton = new wxButton(this, SC_TARGET_COLOR_BUTTON, "",
                                       wxDefaultPosition, wxSize(25,20), 0);
   
   wxFlexGridSizer *scOptionBoxSizer1 = new wxFlexGridSizer(2, 0, 0);
   scOptionBoxSizer1->Add(orbitColorLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   scOptionBoxSizer1->Add(mScOrbitColorButton, 0, wxALIGN_LEFT|wxALL, bsize);
   scOptionBoxSizer1->Add(targetColorLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   scOptionBoxSizer1->Add(mScTargetColorButton, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mScOptionBoxSizer = new wxBoxSizer(wxVERTICAL);
   mScOptionBoxSizer->Add(scOptionBoxSizer1, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //------------------------------------------------------
   // put in the order
   //------------------------------------------------------    
   mFlexGridSizer->Add(mScAvailableBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(arrowButtonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(mScSelectedBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(mScOptionBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Show(mScOptionBoxSizer, false);
   
   pageBoxSizer->Add(mFlexGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageBoxSizer->Add(plotOptionBoxSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
#if DEBUG_OPENGL_PANEL
   MessageInterface::ShowMessage("OpenGlPlotSetupPanel::Create() exiting...\n");
#endif

}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::LoadData()
{
#if DEBUG_OPENGL_PANEL
   MessageInterface::ShowMessage("OpenGlPlotSetupPanel::LoadData() entering...\n");
#endif

   // load data from the core engine
   plotCheckBox->SetValue(mSubscriber->IsActive());
   wireFrameCheckBox->SetValue(mSubscriber->GetStringParameter("WireFrame") == "On");

   // get spacecraft list to plot
   StringArray scNameList = mSubscriber->GetStringArrayParameter("SpacecraftList");
   mScCount = scNameList.size();
   
   if (mScCount > 0)
   {
      wxString *scNames = new wxString[mScCount];
      for (int i=0; i<mScCount; i++)
      {
         scNames[i] = scNameList[i].c_str();
         mOrbitColorMap[scNameList[i]]
            = RgbColor(mOpenGlPlot->GetUnsignedIntParameter("OrbitColor", scNameList[i]));
         mTargetColorMap[scNameList[i]]
            = RgbColor(mOpenGlPlot->GetUnsignedIntParameter("TargetColor", scNameList[i]));
      }
    
      mScSelectedListBox->Set(mScCount, scNames);
      delete scNames;

      // show spacecraft option
      mScSelectedListBox->SetSelection(0);
      ShowSpacecraftOption(mScSelectedListBox->GetStringSelection(), true);
   }
   else
   {
      ShowSpacecraftOption("", false);
   }
   
#if DEBUG_OPENGL_PANEL
   MessageInterface::ShowMessage("OpenGlPlotSetupPanel::LoadData() exiting...\n");
#endif
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::SaveData()
{
   
   // save data to core engine
   mSubscriber->Activate(plotCheckBox->IsChecked());
   if (wireFrameCheckBox->IsChecked())
      mSubscriber->SetStringParameter("WireFrame", "On");
   else
      mSubscriber->SetStringParameter("WireFrame", "Off");

   // save spacecraft list
   if (mIsScChanged)
   {
      mIsScChanged = false;
      mIsColorChanged = true;
      
      mScCount = mScSelectedListBox->GetCount();
      
      if (mScCount == 0 && plotCheckBox->IsChecked())
      {
         wxLogMessage(wxT("Spacecraft not selected. The plot will not be activated."));
         mSubscriber->Activate(false);
      }

      if (mScCount >= 0) // >=0 because the list needs to be cleared
      {
         mSubscriber->SetBooleanParameter("ClearSpacecraftList", true);
         for (int i=0; i<mScCount; i++)
         {
            mSelScName = std::string(mScSelectedListBox->GetString(i).c_str());
            //MessageInterface::ShowMessage("OpenGlPlotSetupPanel::SaveData() SC = %s\n",
            //                              mSelScName.c_str());
            mSubscriber->
               SetStringParameter("Add", mSelScName);
         }
      }
   }
   
   // save color
   if (mIsColorChanged)
   {
      mIsColorChanged = false;
      
      for (int i=0; i<mScCount; i++)
      {
         mSelScName = std::string(mScSelectedListBox->GetString(i).c_str());
         mOpenGlPlot->
            SetUnsignedIntParameter("OrbitColor", mSelScName,
                                    mOrbitColorMap[mSelScName].GetIntColor());
         mOpenGlPlot->
            SetUnsignedIntParameter("TargetColor", mSelScName,
                                    mTargetColorMap[mSelScName].GetIntColor());
      }
   }
}

//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void ShowSpacecraftOption(const wxString &name, bool show = true)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::ShowSpacecraftOption(const wxString &name, bool show)
{
#if DEBUG_OPENGL_PANEL
   MessageInterface::ShowMessage
      ("OpenGlPlotSetupPanel::ShowSpacecraftOption() name=%s\n",
       name.c_str());
#endif
   
   if (!name.IsSameAs(""))
   {
      mSelScName = std::string(name.c_str());
      RgbColor orbColor = mOrbitColorMap[mSelScName];
      RgbColor targColor = mTargetColorMap[mSelScName];

#if DEBUG_OPENGL_PANEL
      MessageInterface::ShowMessage("ShowSpacecraftOption() mSelScName=%s\n",
                                    mSelScName.c_str());
      MessageInterface::ShowMessage
         ("ShowSpacecraftOption() orbColor=%d targColor=%d\n",
          orbColor.GetIntColor(),targColor.GetIntColor());
#endif
      
      mScOrbitColor.Set(orbColor.Red(), orbColor.Green(), orbColor.Blue());
      mScTargetColor.Set(targColor.Red(), targColor.Green(), targColor.Blue());
      
      mScOrbitColorButton->SetBackgroundColour(mScOrbitColor);
      mScTargetColorButton->SetBackgroundColour(mScTargetColor);
      mFlexGridSizer->Show(mScOptionBoxSizer, show);
   }
   else
   {
      mFlexGridSizer->Show(mScOptionBoxSizer, false);
   }
   
   mFlexGridSizer->Layout();
}
