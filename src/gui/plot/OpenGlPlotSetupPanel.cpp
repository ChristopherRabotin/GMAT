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
   EVT_TEXT(ID_TEXTCTRL, OpenGlPlotSetupPanel::OnTextChange)
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

   // Set the pointer for the "Show Script" button
   mObject = mOpenGlPlot;

   mHasScChanged = false;
   mHasColorChanged = false;
   mHasCoordSysChanged = false;
   mHasViewInfoChanged = false;
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

   mUseViewPointInfoCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Use ViewPoint Info"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   mPerspectiveModeCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Use Perspective Mode"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   wxStaticText *coordSysLabel =
      new wxStaticText(this, -1, wxT("Coordinate System"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxStaticText *viewPointRefLabel =
      new wxStaticText(this, -1, wxT("View Point Reference"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxStaticText *viewPointVectorLabel =
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
      theGuiManager->GetSpacePointComboBox(this, ID_COMBOBOX, wxSize(120,-1), true);
   mViewPointVectorComboBox =
      theGuiManager->GetSpacePointComboBox(this, ID_COMBOBOX, wxSize(120,-1), true);
   mViewDirectionComboBox =
      theGuiManager->GetSpacePointComboBox(this, ID_COMBOBOX, wxSize(120,-1), true);

   mViewScaleFactorTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0);

   // vector for ViewPointRef
   mViewPointRef1TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(50,-1), 0);
   mViewPointRef2TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(50,-1), 0);
   mViewPointRef3TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(50,-1), 0);
   
   mViewPointRefSizer = new wxBoxSizer(wxHORIZONTAL);
   mViewPointRefSizer->Add(mViewPointRef1TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewPointRefSizer->Add(mViewPointRef2TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewPointRefSizer->Add(mViewPointRef3TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);

   // vector for ViewPointVector
   mViewPointVec1TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(50,-1), 0);
   mViewPointVec2TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(50,-1), 0);
   mViewPointVec3TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("30000"), wxDefaultPosition, wxSize(50,-1), 0);
   
   mViewPointVectorSizer = new wxBoxSizer(wxHORIZONTAL);
   mViewPointVectorSizer->Add(mViewPointVec1TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewPointVectorSizer->Add(mViewPointVec2TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewPointVectorSizer->Add(mViewPointVec3TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);

   // vector for ViewDirection
   mViewDir1TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(50,-1), 0);
   mViewDir2TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(50,-1), 0);
   mViewDir3TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("-1"), wxDefaultPosition, wxSize(50,-1), 0);
   
   mViewDirVectorSizer = new wxBoxSizer(wxHORIZONTAL);
   mViewDirVectorSizer->Add(mViewDir1TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewDirVectorSizer->Add(mViewDir2TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewDirVectorSizer->Add(mViewDir3TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);

   
   // plot option sizer
   mPlotOptionSizer = new wxFlexGridSizer(4, 0, 0);

   mPlotOptionSizer->Add(mPlotCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(coordSysLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mPlotOptionSizer->Add(mCoordSysComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(emptyStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mPlotOptionSizer->Add(mWireFrameCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(viewPointRefLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mPlotOptionSizer->Add(mViewPointRefComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(mViewPointRefSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mPlotOptionSizer->Add(mTargetStatusCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(viewPointVectorLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mPlotOptionSizer->Add(mViewPointVectorComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(mViewPointVectorSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mPlotOptionSizer->Add(mEclipticPlaneCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(viewDirectionLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mPlotOptionSizer->Add(mViewDirectionComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(mViewDirVectorSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mPlotOptionSizer->Add(mEquatorialPlaneCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(viewScaleFactorLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mPlotOptionSizer->Add(mViewScaleFactorTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(emptyStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mPlotOptionSizer->Add(mOverlapCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mPlotOptionSizer->Add(mUseViewPointInfoCheckBox, 0, wxALIGN_RIGHT|wxALL, bsize);
   mPlotOptionSizer->Add(mPerspectiveModeCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
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
      mEquatorialPlaneCheckBox->SetValue(mOpenGlPlot->GetStringParameter("EquatorialPlane") == "On");
      mEclipticPlaneCheckBox->SetValue(mOpenGlPlot->GetStringParameter("CelestialPlane") == "On");
      mWireFrameCheckBox->SetValue(mOpenGlPlot->GetStringParameter("WireFrame") == "On");
      mTargetStatusCheckBox->SetValue(mOpenGlPlot->GetStringParameter("TargetStatus") == "On");
      mOverlapCheckBox->SetValue(mOpenGlPlot->GetStringParameter("Overlap") == "On");
      mUseViewPointInfoCheckBox->SetValue(mOpenGlPlot->GetStringParameter("UseViewPointInfo") == "On");
      mPerspectiveModeCheckBox->SetValue(mOpenGlPlot->GetStringParameter("PerspectiveMode") == "On");
      
      mCoordSysComboBox->SetStringSelection
         (mOpenGlPlot->GetStringParameter("CoordinateSystem").c_str());

      // load ViewPoint info
      mViewPointRefComboBox->
         SetStringSelection(mOpenGlPlot->GetStringParameter("ViewPointRef").c_str());
      mViewPointVectorComboBox->
         SetStringSelection(mOpenGlPlot->GetStringParameter("ViewPointVector").c_str());
      mViewDirectionComboBox->
         SetStringSelection(mOpenGlPlot->GetStringParameter("ViewDirection").c_str());

      wxString str;
      str.Printf("%g", mOpenGlPlot->GetRealParameter("ViewScaleFactor"));
      mViewScaleFactorTextCtrl->SetValue(str);
      
      // show vector if viewpoint vector name is Vector
      if (mViewPointRefComboBox->GetStringSelection() == "Vector")
      {
         Rvector vec = mOpenGlPlot->GetRvectorParameter("ViewPointRefVector");
         MessageInterface::ShowMessage
            ("OpenGlPlotSetupPanel::LoadData() ViewPointRefVector = %s\n",
             vec.ToString().c_str());
         wxString str;
         str.Printf("%g", vec[0]);
         mViewPointRef1TextCtrl->SetValue(str);
         str.Printf("%g", vec[1]);
         mViewPointRef2TextCtrl->SetValue(str);
         str.Printf("%g", vec[2]);
         mViewPointRef3TextCtrl->SetValue(str);

         mPlotOptionSizer->Show(mViewPointRefSizer, true);
      }
      else
      {
         mPlotOptionSizer->Show(mViewPointRefSizer, false);
      }
      
      // show vector if viewpoint vector name is Vector
      if (mViewPointVectorComboBox->GetStringSelection() == "Vector")
      {
         Rvector vec = mOpenGlPlot->GetRvectorParameter("ViewPointVectorVector");
         
         #if DEBUG_OPENGL_PANEL
         MessageInterface::ShowMessage
            ("OpenGlPlotSetupPanel::LoadData() ViewPointVectorVector = %s\n",
             vec.ToString().c_str());
         #endif
         
         wxString str;
         str.Printf("%g", vec[0]);
         mViewPointVec1TextCtrl->SetValue(str);
         str.Printf("%g", vec[1]);
         mViewPointVec2TextCtrl->SetValue(str);
         str.Printf("%g", vec[2]);
         mViewPointVec3TextCtrl->SetValue(str);

         mPlotOptionSizer->Show(mViewPointVectorSizer, true);
      }
      else
      {
         mPlotOptionSizer->Show(mViewPointVectorSizer, false);
      }
            
      // show vector if view direction name is Vector
      if (mViewDirectionComboBox->GetStringSelection() == "Vector")
      {
         Rvector vec = mOpenGlPlot->GetRvectorParameter("ViewDirectionVector");
         wxString str;
         str.Printf("%g", vec[0]);
         mViewDir1TextCtrl->SetValue(str);
         str.Printf("%g", vec[1]);
         mViewDir2TextCtrl->SetValue(str);
         str.Printf("%g", vec[2]);
         mViewDir3TextCtrl->SetValue(str);
         
         mPlotOptionSizer->Show(mViewDirVectorSizer, true);
      }
      else
      {
         mPlotOptionSizer->Show(mViewDirVectorSizer, false);
      }

      // set layout
      mPlotOptionSizer->Layout();
      
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
   
   mPerspectiveModeCheckBox->Disable();
   theApplyButton->Disable();
   
   #if DEBUG_OPENGL_PANEL
   MessageInterface::ShowMessage("OpenGlPlotSetupPanel::LoadData() exiting...\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::SaveData()
{

   try
   {
      // save data to core engine
      mOpenGlPlot->Activate(mPlotCheckBox->IsChecked());
      
      if (mEquatorialPlaneCheckBox->IsChecked())
         mOpenGlPlot->SetStringParameter("EquatorialPlane", "On");
      else
         mOpenGlPlot->SetStringParameter("EquatorialPlane", "Off");
   
      if (mEclipticPlaneCheckBox->IsChecked())
         mOpenGlPlot->SetStringParameter("CelestialPlane", "On");
      else
         mOpenGlPlot->SetStringParameter("CelestialPlane", "Off");
   
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

      if (mUseViewPointInfoCheckBox->IsChecked())
         mOpenGlPlot->SetStringParameter("UseViewPointInfo", "On");
      else
         mOpenGlPlot->SetStringParameter("UseViewPointInfo", "Off");

      if (mPerspectiveModeCheckBox->IsChecked())
         mOpenGlPlot->SetStringParameter("PerspectiveMode", "On");
      else
         mOpenGlPlot->SetStringParameter("PerspectiveMode", "Off");

      // save spacecraft list
      if (mHasScChanged)
      {
         mHasScChanged = false;
         mHasColorChanged = true;
      
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
      if (mHasColorChanged)
      {
         mHasColorChanged = false;
      
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
      if (mHasCoordSysChanged)
      {
         mHasCoordSysChanged = false;
         mOpenGlPlot->SetStringParameter
            ("CoordinateSystem",
             std::string(mCoordSysComboBox->GetStringSelection().c_str()));
      }

      // save ViewPoint info
      if (mHasViewInfoChanged)
      {
         mHasViewInfoChanged = false;
         mOpenGlPlot->SetStringParameter
            ("ViewPointRef",
             std::string(mViewPointRefComboBox->GetStringSelection().c_str()));
         mOpenGlPlot->SetStringParameter
            ("ViewPointVector",
             std::string(mViewPointVectorComboBox->GetStringSelection().c_str()));
         mOpenGlPlot->SetStringParameter
            ("ViewDirection",
             std::string(mViewDirectionComboBox->GetStringSelection().c_str()));
         
         Real realVal;
         mViewScaleFactorTextCtrl->GetValue().ToDouble(&realVal);
         mOpenGlPlot->SetRealParameter("ViewScaleFactor", realVal);

         // save ViewPoint ref. vector
         if (mViewPointRefComboBox->GetStringSelection() == "Vector")
         {
            Rvector vec(3);
            Real val1, val2, val3;
            mViewPointRef1TextCtrl->GetValue().ToDouble(&val1);
            mViewPointRef2TextCtrl->GetValue().ToDouble(&val2);
            mViewPointRef3TextCtrl->GetValue().ToDouble(&val3);
            vec(0) = val1;
            vec(1) = val2;
            vec(2) = val3;
            mOpenGlPlot->SetRvectorParameter("ViewPointRefVector", vec);
         }
         
         // save ViewPoint vector
         if (mViewPointVectorComboBox->GetStringSelection() == "Vector")
         {
            Rvector vec(3);
            Real val1, val2, val3;
            mViewPointVec1TextCtrl->GetValue().ToDouble(&val1);
            mViewPointVec2TextCtrl->GetValue().ToDouble(&val2);
            mViewPointVec3TextCtrl->GetValue().ToDouble(&val3);
            vec(0) = val1;
            vec(1) = val2;
            vec(2) = val3;
            mOpenGlPlot->SetRvectorParameter("ViewPointVectorVector", vec);
         }

         // save View direction
         if (mViewDirectionComboBox->GetStringSelection() == "Vector")
         {
            Rvector vec(3);
            Real val1, val2, val3;
            mViewDir1TextCtrl->GetValue().ToDouble(&val1);
            mViewDir2TextCtrl->GetValue().ToDouble(&val2);
            mViewDir3TextCtrl->GetValue().ToDouble(&val3);
            vec(0) = val1;
            vec(1) = val2;
            vec(2) = val3;
            mOpenGlPlot->SetRvectorParameter("ViewDirectionVector", vec);
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("OpenGlPlotSetupPanel:SaveData() error occurred!\n%s\n", e.GetMessage().c_str());
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
         mHasScChanged = true;
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
         mHasScChanged = true;
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
   
   mHasScChanged = true;
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnClearSpacecraft(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnClearSpacecraft(wxCommandEvent& event)
{
   mSelectedObjListBox->Clear();
   ShowSpacecraftOption("", false);
   mHasScChanged = true;
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
      mHasColorChanged = true;
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
      mHasColorChanged = true;
   }
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnComboBoxChange(wxCommandEvent& event)
{    
   if (event.GetEventObject() == mCoordSysComboBox)
   {
      mHasCoordSysChanged = true;
   }
   else if (event.GetEventObject() == mViewPointRefComboBox)
   {
      mHasViewInfoChanged = true;
      
      if (mViewPointRefComboBox->GetStringSelection() == "Vector")
         mPlotOptionSizer->Show(mViewPointRefSizer, true);
      else
         mPlotOptionSizer->Show(mViewPointRefSizer, false);

      mPlotOptionSizer->Layout();
        
   }
   else if (event.GetEventObject() == mViewPointVectorComboBox)
   {
      mHasViewInfoChanged = true;
      
      if (mViewPointVectorComboBox->GetStringSelection() == "Vector")
         mPlotOptionSizer->Show(mViewPointVectorSizer, true);
      else
         mPlotOptionSizer->Show(mViewPointVectorSizer, false);

      mPlotOptionSizer->Layout();
        
   }
   else if (event.GetEventObject() == mViewDirectionComboBox)
   {
      mHasViewInfoChanged = true;
      
      if (mViewDirectionComboBox->GetStringSelection() == "Vector")
         mPlotOptionSizer->Show(mViewDirVectorSizer, true);
      else
         mPlotOptionSizer->Show(mViewDirVectorSizer, false);
        
      mPlotOptionSizer->Layout();
   }
   
   theApplyButton->Enable();
}


//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnTextChange(wxCommandEvent& event)
{
   mHasViewInfoChanged = true;
   theApplyButton->Enable();
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
