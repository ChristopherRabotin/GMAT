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

#include "ColorTypes.hpp"           // for namespace GmatColor::
#include "MessageInterface.hpp"

#include "wx/colordlg.h"            // for wxColourDialog

//#define DEBUG_OPENGL_PANEL 1

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
   EVT_LISTBOX(ID_LISTBOX, OpenGlPlotSetupPanel::OnSelectAvailObject)
   EVT_LISTBOX(SC_SEL_LISTBOX, OpenGlPlotSetupPanel::OnSelectSpacecraft)
   EVT_CHECKBOX(CHECKBOX, OpenGlPlotSetupPanel::OnCheckBoxChange)
   EVT_COMBOBOX(ID_COMBOBOX, OpenGlPlotSetupPanel::OnComboBoxChange)
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
   Subscriber *subscriber =
      theGuiInterpreter->GetSubscriber(std::string(subscriberName.c_str()));
   mOpenGlPlot = (OpenGlPlot*)subscriber;

   mIsScChanged = false;
   mIsColorChanged = false;
   mIsCoordSysChanged = false;
   mScCount = 0;
   
   mOrbitColorMap.clear();
   mTargetColorMap.clear();
   
   Create();
   Show();
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::Create()
{
   #if DEBUG_OPENGL_PANEL
   MessageInterface::ShowMessage("OpenGlPlotSetupPanel::Create() entering...\n");
   #endif

   // empty StaticText
   wxStaticText *emptyStaticText =
      new wxStaticText( this, -1, wxT("  "), wxDefaultPosition, wxDefaultSize, 0 );
  
   wxString emptyList[] = {};
   Integer bsize = 2; // border size
   
   //------------------------------------------------------
   // available spacecraft/celestial object list (1st column)
   //------------------------------------------------------
   wxBoxSizer *availObjBoxSizer = new wxBoxSizer(wxVERTICAL);
   
   wxStaticText *scAvailableLabel =
      new wxStaticText(this, -1, wxT("Spacecraft"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxArrayString empty;
   mSpacecraftListBox =
      theGuiManager->GetSpacecraftListBox(this, ID_LISTBOX, wxSize(150,88), empty);
   
   wxStaticText *coAvailableLabel =
      new wxStaticText(this, -1, wxT("Celestial Object"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mCelesObjectListBox =
      theGuiManager->GetConfigBodyListBox(this, ID_LISTBOX, wxSize(150,88), empty);
   
   availObjBoxSizer->Add(scAvailableLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   availObjBoxSizer->Add(mSpacecraftListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   availObjBoxSizer->Add(coAvailableLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   availObjBoxSizer->Add(mCelesObjectListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   #if DEBUG_OPENGL_PANEL
   MessageInterface::ShowMessage
      ("OpenGlPlotSetupPanel::Create() add, remove, clear...\n");
   #endif
   //------------------------------------------------------
   // add, remove, clear Y buttons (2nd column)
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
    
   #if DEBUG_OPENGL_PANEL
   MessageInterface::ShowMessage
      ("OpenGlPlotSetupPanel::Create() selected spacecraft list...\n");
   #endif
   //------------------------------------------------------
   // selected spacecraft list (3rd column)
   //------------------------------------------------------
   wxStaticText *titleSelected =
      new wxStaticText(this, -1, wxT("Selected Object"),
                       wxDefaultPosition, wxSize(-1,-1), 0);

   mSelectedObjListBox =
      new wxListBox(this, SC_SEL_LISTBOX, wxDefaultPosition,
                    wxSize(150,200), 0, emptyList, wxLB_SINGLE);
        
   wxBoxSizer *mObjSelectedBoxSizer = new wxBoxSizer(wxVERTICAL);
   mObjSelectedBoxSizer->Add(titleSelected, 0, wxALIGN_CENTRE|wxALL, bsize);
   mObjSelectedBoxSizer->Add(mSelectedObjListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // spacecraft color (4th column)
   //------------------------------------------------------
   wxStaticText *orbitColorLabel =
      new wxStaticText(this, -1, wxT("Orbit Color"),
                       wxDefaultPosition, wxSize(-1,-1), wxALIGN_CENTRE);
   mTargetColorLabel =
      new wxStaticText(this, -1, wxT("Target Color"),
                       wxDefaultPosition, wxSize(-1,-1), wxALIGN_CENTRE);
   
   mScOrbitColorButton = new wxButton(this, SC_ORBIT_COLOR_BUTTON, "",
                                      wxDefaultPosition, wxSize(25,20), 0);

   mScTargetColorButton = new wxButton(this, SC_TARGET_COLOR_BUTTON, "",
                                       wxDefaultPosition, wxSize(25,20), 0);
   
   wxFlexGridSizer *scOptionBoxSizer1 = new wxFlexGridSizer(2, 0, 0);
   scOptionBoxSizer1->Add(orbitColorLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   scOptionBoxSizer1->Add(mScOrbitColorButton, 0, wxALIGN_LEFT|wxALL, bsize);
   scOptionBoxSizer1->Add(mTargetColorLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   scOptionBoxSizer1->Add(mScTargetColorButton, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mScOptionBoxSizer = new wxBoxSizer(wxVERTICAL);
   mScOptionBoxSizer->Add(scOptionBoxSizer1, 0, wxALIGN_LEFT|wxALL, bsize);
   
   #if DEBUG_OPENGL_PANEL
   MessageInterface::ShowMessage
      ("OpenGlPlotSetupPanel::Create() plot option...\n");
   #endif
   //------------------------------------------------------
   // plot option
   //------------------------------------------------------
   mPlotCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Show Plot"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   mWireFrameCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw WireFrame"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   mTargetStatusCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw Targeting"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   mEclipticPlaneCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw Ecliptic Plane"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   mEquatorialPlaneCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw Equatorial Plane"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   mOverlapCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Overlap Plot"),
                     wxDefaultPosition, wxSize(-1, -1), 0);

   wxStaticText *coordSysLabel =
      new wxStaticText(this, -1, wxT("Coordinate System"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxStaticText *viewPointRefLabel =
      new wxStaticText(this, -1, wxT("View Point Reference"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxStaticText *viewPointVecLabel =
      new wxStaticText(this, -1, wxT("View Point Vector"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxStaticText *viewDirectionLabel =
      new wxStaticText(this, -1, wxT("View Direction"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxStaticText *viewScaleFactorLabel =
      new wxStaticText(this, -1, wxT("View Scale Factor"),
                       wxDefaultPosition, wxSize(-1,-1), 0);

   mCoordSysComboBox =
      theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX, wxSize(120,-1));
   mViewPointRefComboBox =
      theGuiManager->GetSpacePointComboBox(this, ID_COMBOBOX, wxSize(120,-1), false);
   mViewPointVecComboBox =
      theGuiManager->GetSpacePointComboBox(this, ID_COMBOBOX, wxSize(120,-1), true);
   mViewDirectionComboBox =
      theGuiManager->GetSpacePointComboBox(this, ID_COMBOBOX, wxSize(120,-1), true);

   mViewScaleFactorTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0);

   // vector for ViewPoint
   mViewPoint1TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(50,-1), 0);
   mViewPoint2TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(50,-1), 0);
   mViewPoint3TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("10000"), wxDefaultPosition, wxSize(50,-1), 0);
   
   mViewPointVecSizer = new wxBoxSizer(wxHORIZONTAL);
   mViewPointVecSizer->Add(mViewPoint1TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewPointVecSizer->Add(mViewPoint2TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewPointVecSizer->Add(mViewPoint3TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);

   // vector for ViewDirection
   mViewDir1TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(50,-1), 0);
   mViewDir2TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(50,-1), 0);
   mViewDir3TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("10000"), wxDefaultPosition, wxSize(50,-1), 0);
   
   mViewDirVecSizer = new wxBoxSizer(wxHORIZONTAL);
   mViewDirVecSizer->Add(mViewDir1TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewDirVecSizer->Add(mViewDir2TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewDirVecSizer->Add(mViewDir3TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);

   
   // plot option sizer
   mPlotOptionSizer = new wxFlexGridSizer(4, 0, 0);

   mPlotOptionSizer->Add(mPlotCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(coordSysLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mPlotOptionSizer->Add(mCoordSysComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(emptyStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mPlotOptionSizer->Add(mWireFrameCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(viewPointRefLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mPlotOptionSizer->Add(mViewPointRefComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(emptyStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mPlotOptionSizer->Add(mTargetStatusCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(viewPointVecLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mPlotOptionSizer->Add(mViewPointVecComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(mViewPointVecSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mPlotOptionSizer->Add(mEclipticPlaneCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(viewDirectionLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mPlotOptionSizer->Add(mViewDirectionComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(mViewDirVecSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mPlotOptionSizer->Add(mEquatorialPlaneCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(viewScaleFactorLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mPlotOptionSizer->Add(mViewScaleFactorTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(emptyStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mPlotOptionSizer->Add(mOverlapCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(emptyStaticText, 0, wxALIGN_RIGHT|wxALL, bsize);
   mPlotOptionSizer->Add(emptyStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(emptyStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   
   #if DEBUG_OPENGL_PANEL
   MessageInterface::ShowMessage
      ("OpenGlPlotSetupPanel::Create() put in the order...\n");
   #endif
   //------------------------------------------------------
   // put in the order
   //------------------------------------------------------    
   mFlexGridSizer = new wxFlexGridSizer(5, 0, 0);
   mFlexGridSizer->Add(availObjBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(arrowButtonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(mObjSelectedBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(mScOptionBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Show(mScOptionBoxSizer, false);
   
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   pageBoxSizer->Add(mFlexGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageBoxSizer->Add(mPlotOptionSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
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

   try
   {
      // load data from the core engine
      mPlotCheckBox->SetValue(mOpenGlPlot->IsActive());
      mWireFrameCheckBox->SetValue(mOpenGlPlot->GetStringParameter("WireFrame") == "On");
      mTargetStatusCheckBox->SetValue(mOpenGlPlot->GetStringParameter("TargetStatus") == "On");
      mOverlapCheckBox->SetValue(mOpenGlPlot->GetStringParameter("Overlap") == "On");
      
      mCoordSysComboBox->SetStringSelection
         (mOpenGlPlot->GetStringParameter("CoordinateSystem").c_str());

      // temp code: the following values should be from mOpenGlPlot
      mViewPointRefComboBox->SetStringSelection("Earth");
      mViewPointVecComboBox->SetStringSelection("Vector");
      mViewDirectionComboBox->SetStringSelection("Earth");
      mPlotOptionSizer->Show(mViewDirVecSizer, false);
      mPlotOptionSizer->Layout();

      mViewScaleFactorTextCtrl->SetValue("1.0");

      // get spacecraft list to plot
      StringArray scNameList = mOpenGlPlot->GetStringArrayParameter("Add");
      mScCount = scNameList.size();
   
      if (mScCount > 0)
      {
         wxString *scNames = new wxString[mScCount];
         for (int i=0; i<mScCount; i++)
         {
            scNames[i] = scNameList[i].c_str();
         
            mOrbitColorMap[scNameList[i]]
               = RgbColor(mOpenGlPlot->GetColor("Orbit", scNameList[i]));
            mTargetColorMap[scNameList[i]]
               = RgbColor(mOpenGlPlot->GetColor("Target", scNameList[i]));
         }
      
         mSelectedObjListBox->Set(mScCount, scNames);
         delete scNames;
      
         // show spacecraft option
         mSelectedObjListBox->SetSelection(0);
         ShowSpacecraftOption(mSelectedObjListBox->GetStringSelection(), true);
      }
      else
      {
         ShowSpacecraftOption("", false);
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("OpenGlPlotSetupPanel:LoadData() error occurred!\n%s\n", e.GetMessage().c_str());
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
   mOpenGlPlot->Activate(mPlotCheckBox->IsChecked());
   if (mWireFrameCheckBox->IsChecked())
      mOpenGlPlot->SetStringParameter("WireFrame", "On");
   else
      mOpenGlPlot->SetStringParameter("WireFrame", "Off");
   
   if (mTargetStatusCheckBox->IsChecked())
      mOpenGlPlot->SetStringParameter("TargetStatus", "On");
   else
      mOpenGlPlot->SetStringParameter("TargetStatus", "Off");

   if (mOverlapCheckBox->IsChecked())
      mOpenGlPlot->SetStringParameter("Overlap", "On");
   else
      mOpenGlPlot->SetStringParameter("Overlap", "Off");

   // save spacecraft list
   if (mIsScChanged)
   {
      mIsScChanged = false;
      mIsColorChanged = true;
      
      mScCount = mSelectedObjListBox->GetCount();
      
      if (mScCount == 0 && mPlotCheckBox->IsChecked())
      {
         wxLogMessage(wxT("Spacecraft not selected. The plot will not be activated."));
         mOpenGlPlot->Activate(false);
      }

      if (mScCount >= 0) // >=0 because the list needs to be cleared
      {
         mOpenGlPlot->TakeAction("Clear");
         
         for (int i=0; i<mScCount; i++)
         {
            mSelScName = std::string(mSelectedObjListBox->GetString(i).c_str());
            //MessageInterface::ShowMessage("OpenGlPlotSetupPanel::SaveData() SC = %s\n",
            //                              mSelScName.c_str());
            mOpenGlPlot->
               SetStringParameter("Add", mSelScName, i);
         }
      }
   }
   
   // save color
   if (mIsColorChanged)
   {
      mIsColorChanged = false;
      
      for (int i=0; i<mScCount; i++)
      {
         mSelScName = std::string(mSelectedObjListBox->GetString(i).c_str());
         
         mOpenGlPlot->
            SetColor("Orbit", mSelScName,
                     mOrbitColorMap[mSelScName].GetIntColor());
         mOpenGlPlot->
            SetColor("Target", mSelScName,
                     mTargetColorMap[mSelScName].GetIntColor());
      }
   }
   
   // save coordinate system
   if (mIsCoordSysChanged)
   {
      mIsCoordSysChanged = false;
      mOpenGlPlot->SetStringParameter("CoordinateSystem",
         std::string(mCoordSysComboBox->GetStringSelection().c_str()));
   }
}


//------------------------------------------------------------------------------
// void OnAddSpacecraft(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnAddSpacecraft(wxCommandEvent& event)
{
   if (mSpacecraftListBox->GetSelection() != -1)
   {
      // get string in first list and then search for it
      // in the second list
      wxString s = mSpacecraftListBox->GetStringSelection();
      int found = mSelectedObjListBox->FindString(s);
    
      // if the string wasn't found in the second list, insert it
      if (found == wxNOT_FOUND)
      {
         mSelectedObjListBox->Append(s);
         mSelectedObjListBox->SetStringSelection(s);
      
         // select next available item
         mSpacecraftListBox->
            SetSelection(mSpacecraftListBox->GetSelection()+1);

         ShowSpacecraftOption(s, true);
         mIsScChanged = true;
         theApplyButton->Enable();
      }
   }
   else if (mCelesObjectListBox->GetSelection() != -1)
   {
      wxString s = mCelesObjectListBox->GetStringSelection();
      int found = mSelectedObjListBox->FindString(s);
    
      // if the string wasn't found in the second list, insert it
      if (found == wxNOT_FOUND)
      {
         mSelectedObjListBox->Append(s);
         mSelectedObjListBox->SetStringSelection(s);
      
         // select next available item
         mCelesObjectListBox->
            SetSelection(mCelesObjectListBox->GetSelection()+1);

         ShowSpacecraftOption(s, true);
         mIsScChanged = true;
         theApplyButton->Enable();
      }
   }
}


//------------------------------------------------------------------------------
// void OnRemoveSpacecraft(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnRemoveSpacecraft(wxCommandEvent& event)
{
   int sel = mSelectedObjListBox->GetSelection();
   mSelectedObjListBox->Delete(sel);
   
   if (sel-1 < 0)
   {
      mSelectedObjListBox->SetSelection(0);
      if (mSelectedObjListBox->GetCount() == 0)
         ShowSpacecraftOption("", false); // hide spacecraft color, etc
      else
         ShowSpacecraftOption(mSelectedObjListBox->GetStringSelection(), true);
   }
   else
   {
      mSelectedObjListBox->SetSelection(sel-1);
      ShowSpacecraftOption(mSelectedObjListBox->GetStringSelection(), true);
   }
   
   mIsScChanged = true;
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnClearSpacecraft(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnClearSpacecraft(wxCommandEvent& event)
{
   mSelectedObjListBox->Clear();
   ShowSpacecraftOption("", false);
   mIsScChanged = true;
   theApplyButton->Enable();
}


//------------------------------------------------------------------------------
// void OnSelectAvailObject(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnSelectAvailObject(wxCommandEvent& event)
{
   if (event.GetEventObject() == mSpacecraftListBox)
      mCelesObjectListBox->Deselect(mCelesObjectListBox->GetSelection());
   else if (event.GetEventObject() == mCelesObjectListBox)
      mSpacecraftListBox->Deselect(mSpacecraftListBox->GetSelection());
}


//------------------------------------------------------------------------------
// void OnSelectSpacecraft(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnSelectSpacecraft(wxCommandEvent& event)
{
   ShowSpacecraftOption(mSelectedObjListBox->GetStringSelection(), true);
}


//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnCheckBoxChange(wxCommandEvent& event)
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
      mSelScName = std::string(mSelectedObjListBox->GetStringSelection().c_str());
      
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
      mSelScName = std::string(mSelectedObjListBox->GetStringSelection().c_str());
      
      mScTargetColor = dialog.GetColourData().GetColour();
      mScTargetColorButton->SetBackgroundColour(mScTargetColor);
      mTargetColorMap[mSelScName].Set(mScTargetColor.Red(),
                                      mScTargetColor.Green(),
                                      mScTargetColor.Blue());
      
      theApplyButton->Enable();
      mIsColorChanged = true;
   }
}

//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnComboBoxChange(wxCommandEvent& event)
{    
   if (event.GetEventObject() == mCoordSysComboBox)
   {
      mIsCoordSysChanged = true;
      theApplyButton->Enable();
   }
   else if (event.GetEventObject() == mViewPointVecComboBox)
   {
      if (mViewPointVecComboBox->GetStringSelection() == "Vector")
         mPlotOptionSizer->Show(mViewPointVecSizer, true);
      else
         mPlotOptionSizer->Show(mViewPointVecSizer, false);

      mPlotOptionSizer->Layout();
        
   }
   else if (event.GetEventObject() == mViewDirectionComboBox)
   {
      if (mViewDirectionComboBox->GetStringSelection() == "Vector")
         mPlotOptionSizer->Show(mViewDirVecSizer, true);
      else
         mPlotOptionSizer->Show(mViewDirVecSizer, false);
        
      mPlotOptionSizer->Layout();
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
      int found = mSpacecraftListBox->FindString(name);

      //------------------------------------------
      // if selected object is a spacecraft
      //------------------------------------------
      if (found != wxNOT_FOUND)
      {
         // if spacecraft name not found, insert
         if (mOrbitColorMap.find(mSelScName) == mOrbitColorMap.end())
         {
            mOrbitColorMap[mSelScName] = RgbColor(GmatColor::RED32);
            mTargetColorMap[mSelScName] = RgbColor(GmatColor::ORANGE32);
         }
         
         RgbColor orbColor = mOrbitColorMap[mSelScName];
         RgbColor targColor = mTargetColorMap[mSelScName];
         
         #if DEBUG_OPENGL_PANEL
         MessageInterface::ShowMessage
            ("ShowSpacecraftOption() orbColor=%08x targColor=%08x\n",
             orbColor.GetIntColor(), targColor.GetIntColor());
         #endif
         
         mScOrbitColor.Set(orbColor.Red(), orbColor.Green(), orbColor.Blue());
         mScTargetColor.Set(targColor.Red(), targColor.Green(), targColor.Blue());
         
         mScOrbitColorButton->SetBackgroundColour(mScOrbitColor);
         mScTargetColorButton->SetBackgroundColour(mScTargetColor);
         mTargetColorLabel->Enable();
         mScTargetColorButton->Enable();
         mFlexGridSizer->Show(mScOptionBoxSizer, show);
      }
      //------------------------------------------
      // else selected object is a celestial body
      //------------------------------------------
      else
      {
         // if spacecraft name not found, insert
         if (mOrbitColorMap.find(mSelScName) == mOrbitColorMap.end())
         {
            mOrbitColorMap[mSelScName] = RgbColor(GmatColor::RED32);
         }
         
         RgbColor orbColor = mOrbitColorMap[mSelScName];
         
         #if DEBUG_OPENGL_PANEL
         MessageInterface::ShowMessage("ShowSpacecraftOption() mSelScName=%s\n",
                                       mSelScName.c_str());
         MessageInterface::ShowMessage
            ("ShowSpacecraftOption() orbColor=%08x\n", orbColor.GetIntColor());
         #endif
         
         mScOrbitColor.Set(orbColor.Red(), orbColor.Green(), orbColor.Blue());
         
         mScOrbitColorButton->SetBackgroundColour(mScOrbitColor);
         mScTargetColorButton->SetBackgroundColour(*wxLIGHT_GREY);
         mTargetColorLabel->Disable();
         mScTargetColorButton->Disable();
         mFlexGridSizer->Show(mScOptionBoxSizer, show);
      }
   }
   else
   {
      mFlexGridSizer->Show(mScOptionBoxSizer, false);
   }
   
   mFlexGridSizer->Layout();
}
