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
//#define DEBUG_OPENGL_PANEL_CREATE 1
//#define DEBUG_OPENGL_PANEL_LOAD 1
//#define DEBUG_OPENGL_PANEL_SAVE 1

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(OpenGlPlotSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)

   EVT_BUTTON(ADD_SP_BUTTON, OpenGlPlotSetupPanel::OnAddSpacePoint)
   EVT_BUTTON(REMOVE_SP_BUTTON, OpenGlPlotSetupPanel::OnRemoveSpacePoint)
   EVT_BUTTON(CLEAR_SP_BUTTON, OpenGlPlotSetupPanel::OnClearSpacePoint)
   EVT_BUTTON(ORBIT_COLOR_BUTTON, OpenGlPlotSetupPanel::OnOrbitColorClick)
   EVT_BUTTON(TARGET_COLOR_BUTTON, OpenGlPlotSetupPanel::OnTargetColorClick)
   EVT_LISTBOX(ID_LISTBOX, OpenGlPlotSetupPanel::OnSelectAvailObject)
   EVT_LISTBOX(SC_SEL_LISTBOX, OpenGlPlotSetupPanel::OnSelectSpacecraft)
   EVT_LISTBOX(OBJ_SEL_LISTBOX, OpenGlPlotSetupPanel::OnSelectOtherObject)
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
   
   mHasSpChanged = false;
   mHasColorChanged = false;
   mHasCoordSysChanged = false;
   mHasViewInfoChanged = false;
   mScCount = 0;
   mNonScCount = 0;
   
   mOrbitColorMap.clear();
   mTargetColorMap.clear();
   
   Create();
   Show();
}


//------------------------------------------------------------------------------
// ~OpenGlPlotSetupPanel()
//------------------------------------------------------------------------------
OpenGlPlotSetupPanel::~OpenGlPlotSetupPanel()
{
   #if DEBUG_OPENGL_PANEL
   MessageInterface::ShowMessage
      ("OpenGlPlotSetupPanel::~OpenGlPlotSetupPanel() unregistering mSpacecraftListBox:%d\n",
       mSpacecraftListBox);
   #endif
   
   theGuiManager->UnregisterListBox("Spacecraft", mSpacecraftListBox, &mExcludedScList);
   
   theGuiManager->UnregisterComboBox("CoordinateSystem", mCoordSysComboBox);
   theGuiManager->UnregisterComboBox("CoordinateSystem", mViewUpCsComboBox);
   theGuiManager->UnregisterComboBox("SpacePoint", mViewPointRefComboBox);
   theGuiManager->UnregisterComboBox("SpacePoint", mViewPointVectorComboBox);
   theGuiManager->UnregisterComboBox("SpacePoint", mViewDirectionComboBox);
}


//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::Create()
{
   #if DEBUG_OPENGL_PANEL_CREATE
      MessageInterface::ShowMessage("OpenGlPlotSetupPanel::Create() entering...\n");
   #endif

   // empty StaticText
   wxStaticText *emptyStaticText =
      new wxStaticText( this, -1, wxT("  "), wxDefaultPosition, wxDefaultSize, 0 );
  
   wxString emptyList[] = {};
   Integer bsize = 2; // border size
   
   //------------------------------------------------------
   // plot/update option
   //------------------------------------------------------
   mPlotCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Show Plot"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   //loj: 7/22/05 OverlapPlot will no longer works, because it clears
   // plot every time.
   //mOverlapCheckBox =
   //   new wxCheckBox(this, CHECKBOX, wxT("Overlap Plot"),
   //                  wxDefaultPosition, wxSize(-1, -1), 0);
   
   wxBoxSizer *plotOptionBoxSizer = new wxBoxSizer(wxVERTICAL);
   plotOptionBoxSizer->Add(mPlotCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   //plotOptionBoxSizer->Add(mOverlapCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   plotOptionBoxSizer->Add(20, 14, 0, wxALIGN_LEFT|wxALL, bsize);
   
   wxStaticBox *plotOptionStaticBox = new wxStaticBox(this, -1, wxT("Plot Option"));
   wxStaticBoxSizer *plotOptionStaticSizer =
      new wxStaticBoxSizer(plotOptionStaticBox, wxVERTICAL);
   plotOptionStaticSizer->Add(plotOptionBoxSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //------------------------------------------------------
   // update option
   //------------------------------------------------------
   wxStaticText *dataCollectFreqLabel1 =
      new wxStaticText(this, -1, wxT("Collect data every "),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mDataCollectFreqTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(40, -1), 0);
   wxStaticText *dataCollectFreqLabel2 =
      new wxStaticText(this, -1, wxT("step"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxStaticText *updatePlotFreqLabel1 =
      new wxStaticText(this, -1, wxT("Update plot every "),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mUpdatePlotFreqTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(40, -1), 0);
   wxStaticText *updatePlotFreqLabel2 =
      new wxStaticText(this, -1, wxT("cycle"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   wxBoxSizer *colFreqBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   colFreqBoxSizer->Add(mDataCollectFreqTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   colFreqBoxSizer->Add(dataCollectFreqLabel2, 0, wxALIGN_LEFT|wxALL, bsize);
   
   wxBoxSizer *updFreqBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   updFreqBoxSizer->Add(mUpdatePlotFreqTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   updFreqBoxSizer->Add(updatePlotFreqLabel2, 0, wxALIGN_LEFT|wxALL, bsize);
   
   plotOptionBoxSizer->Add(dataCollectFreqLabel1, 0, wxALIGN_LEFT|wxALL, bsize);
   plotOptionBoxSizer->Add(colFreqBoxSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   plotOptionBoxSizer->Add(updatePlotFreqLabel1, 0, wxALIGN_LEFT|wxALL, bsize);
   plotOptionBoxSizer->Add(updFreqBoxSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //------------------------------------------------------
   // view option
   //------------------------------------------------------
   mLockViewCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Lock View"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   mPerspectiveModeCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Use Perspective Mode"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   mUseFixedFovCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Use Fixed FOV Angle"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   mFovLabel =
      new wxStaticText(this, -1, wxT("Field Of View(Deg): "),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mFixedFovTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(40, -1), 0);
   
   wxBoxSizer *viewOptionBoxSizer = new wxBoxSizer(wxVERTICAL);
   viewOptionBoxSizer->Add(mLockViewCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   viewOptionBoxSizer->Add(mPerspectiveModeCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   viewOptionBoxSizer->Add(mUseFixedFovCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   wxBoxSizer *fovBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   fovBoxSizer->Add(mFovLabel, 0, wxALIGN_LEFT|wxALL, 0);
   fovBoxSizer->Add(mFixedFovTextCtrl, 0, wxALIGN_LEFT|wxALL, 0);
   viewOptionBoxSizer->Add(fovBoxSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   wxStaticBox *viewOptionStaticBox =
      new wxStaticBox(this, -1, wxT("View Option"));
   wxStaticBoxSizer *viewOptionStaticSizer =
      new wxStaticBoxSizer(viewOptionStaticBox, wxVERTICAL);
   viewOptionStaticSizer->Add(viewOptionBoxSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //------------------------------------------------------
   // draw option
   //------------------------------------------------------
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
   
   mAxesCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw Axes"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   mEarthSunLinesCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw Earth Sun Lines"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   wxBoxSizer *drawOptionBoxSizer = new wxBoxSizer(wxVERTICAL);
   drawOptionBoxSizer->Add(mWireFrameCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionBoxSizer->Add(20, 1, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionBoxSizer->Add(mTargetStatusCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionBoxSizer->Add(20, 1, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionBoxSizer->Add(mEclipticPlaneCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionBoxSizer->Add(20, 1, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionBoxSizer->Add(mEquatorialPlaneCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionBoxSizer->Add(20, 1, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionBoxSizer->Add(mAxesCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionBoxSizer->Add(20, 1, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionBoxSizer->Add(mEarthSunLinesCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   wxStaticBox *drawOptionStaticBox = new wxStaticBox(this, -1, wxT("Drawing Option"));
   wxStaticBoxSizer *drawOptionStaticSizer =
      new wxStaticBoxSizer(drawOptionStaticBox, wxVERTICAL);
   drawOptionStaticSizer->Add(drawOptionBoxSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //------------------------------------------------------
   // available spacecraft/celestial object list
   //------------------------------------------------------
   wxStaticText *scAvailableLabel =
      new wxStaticText(this, -1, wxT("Spacecraft"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxArrayString empty;
   mSpacecraftListBox = theGuiManager->
      GetSpacecraftListBox(this, ID_LISTBOX, wxSize(150,65), &mExcludedScList);
   
   wxStaticText *coAvailableLabel =
      new wxStaticText(this, -1, wxT("Celestial Object"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mCelesObjectListBox = theGuiManager->
      GetCelestialPointListBox(this, ID_LISTBOX, wxSize(150,65), empty);
   
   wxBoxSizer *availObjBoxSizer = new wxBoxSizer(wxVERTICAL);
   
   availObjBoxSizer->Add(scAvailableLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   availObjBoxSizer->Add(mSpacecraftListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   availObjBoxSizer->Add(coAvailableLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   availObjBoxSizer->Add(mCelesObjectListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   #if DEBUG_OPENGL_PANEL_CREATE
      MessageInterface::ShowMessage
         ("OpenGlPlotSetupPanel::Create() add, remove, clear...\n");
   #endif
      
   //------------------------------------------------------
   // add, remove, clear Y buttons
   //------------------------------------------------------
   addScButton = new wxButton(this, ADD_SP_BUTTON, wxT("-->"),
                              wxDefaultPosition, wxSize(20,20), 0);

   removeScButton = new wxButton(this, REMOVE_SP_BUTTON, wxT("<--"),
                                 wxDefaultPosition, wxSize(20,20), 0);
    
   clearScButton = new wxButton(this, CLEAR_SP_BUTTON, wxT("<="),
                                wxDefaultPosition, wxSize(20,20), 0);
    
   wxBoxSizer *arrowButtonsBoxSizer = new wxBoxSizer(wxVERTICAL);
   arrowButtonsBoxSizer->Add(addScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsBoxSizer->Add(removeScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsBoxSizer->Add(clearScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   #if DEBUG_OPENGL_PANEL_CREATE
      MessageInterface::ShowMessage
         ("OpenGlPlotSetupPanel::Create() selected spacecraft list...\n");
   #endif
   
   //------------------------------------------------------
   // selected spacecraft list
   //------------------------------------------------------
   wxStaticText *titleSelectedSc =
      new wxStaticText(this, -1, wxT("Selected Spacecraft"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   wxStaticText *titleSelectedObj =
      new wxStaticText(this, -1, wxT("Selected Celestial Object"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   mSelectedScListBox =
      new wxListBox(this, SC_SEL_LISTBOX, wxDefaultPosition,
                    wxSize(150,65), 0, emptyList, wxLB_SINGLE);
   
   mSelectedObjListBox =
      new wxListBox(this, OBJ_SEL_LISTBOX, wxDefaultPosition,
                    wxSize(150,65), 0, emptyList, wxLB_SINGLE);
   
   wxBoxSizer *mObjSelectedBoxSizer = new wxBoxSizer(wxVERTICAL);
   mObjSelectedBoxSizer->Add(titleSelectedSc, 0, wxALIGN_CENTRE|wxALL, bsize);
   mObjSelectedBoxSizer->Add(mSelectedScListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   mObjSelectedBoxSizer->Add(titleSelectedObj, 0, wxALIGN_CENTRE|wxALL, bsize);
   mObjSelectedBoxSizer->Add(mSelectedObjListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // spacecraft color 
   //------------------------------------------------------
   wxStaticText *orbitColorLabel =
      new wxStaticText(this, -1, wxT("Orbit Color"),
                       wxDefaultPosition, wxSize(-1,-1), wxALIGN_CENTRE);
   mTargetColorLabel =
      new wxStaticText(this, -1, wxT("Target Color"),
                       wxDefaultPosition, wxSize(-1,-1), wxALIGN_CENTRE);
   
   mOrbitColorButton = new wxButton(this, ORBIT_COLOR_BUTTON, "",
                                    wxDefaultPosition, wxSize(25,20), 0);

   mTargetColorButton = new wxButton(this, TARGET_COLOR_BUTTON, "",
                                     wxDefaultPosition, wxSize(25,20), 0);
   
   wxFlexGridSizer *scOptionBoxSizer1 = new wxFlexGridSizer(1, 0, 0);
   scOptionBoxSizer1->Add(orbitColorLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   scOptionBoxSizer1->Add(mOrbitColorButton, 0, wxALIGN_LEFT|wxALL, bsize);
   scOptionBoxSizer1->Add(mTargetColorLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   scOptionBoxSizer1->Add(mTargetColorButton, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mScOptionBoxSizer = new wxBoxSizer(wxVERTICAL);
   mScOptionBoxSizer->Add(scOptionBoxSizer1, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mObjectGridSizer = new wxFlexGridSizer(5, 0, 0);
   mObjectGridSizer->Add(availObjBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mObjectGridSizer->Add(arrowButtonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mObjectGridSizer->Add(mObjSelectedBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mObjectGridSizer->Add(mScOptionBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mObjectGridSizer->Show(mScOptionBoxSizer, false);
   
   wxStaticBox *viewObjectStaticBox = new wxStaticBox(this, -1, wxT("View Object"));
   wxStaticBoxSizer *viewObjectStaticSizer
      = new wxStaticBoxSizer(viewObjectStaticBox, wxVERTICAL);
   viewObjectStaticSizer->Add(mObjectGridSizer, 0, wxALIGN_LEFT|wxALL, bsize);

   #if DEBUG_OPENGL_PANEL_CREATE
   MessageInterface::ShowMessage
      ("OpenGlPlotSetupPanel::Create() view geometry...\n");
   #endif
   
   //------------------------------------------------------
   // view geometry
   //------------------------------------------------------
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
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(60,-1), 0);
   mViewPointRef2TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(60,-1), 0);
   mViewPointRef3TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(60,-1), 0);
   
   mViewPointRefSizer = new wxBoxSizer(wxHORIZONTAL);
   mViewPointRefSizer->Add(mViewPointRef1TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewPointRefSizer->Add(mViewPointRef2TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewPointRefSizer->Add(mViewPointRef3TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);

   // vector for ViewPointVector
   mViewPointVec1TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(60,-1), 0);
   mViewPointVec2TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(60,-1), 0);
   mViewPointVec3TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("30000"), wxDefaultPosition, wxSize(60,-1), 0);
   
   mViewPointVectorSizer = new wxBoxSizer(wxHORIZONTAL);
   mViewPointVectorSizer->Add(mViewPointVec1TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewPointVectorSizer->Add(mViewPointVec2TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewPointVectorSizer->Add(mViewPointVec3TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);

   // vector for ViewDirection
   mViewDir1TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(60,-1), 0);
   mViewDir2TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(60,-1), 0);
   mViewDir3TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("-1"), wxDefaultPosition, wxSize(60,-1), 0);
   
   mViewDirVectorSizer = new wxBoxSizer(wxHORIZONTAL);
   mViewDirVectorSizer->Add(mViewDir1TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewDirVectorSizer->Add(mViewDir2TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewDirVectorSizer->Add(mViewDir3TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   
   // view definition sizer
   mViewDefSizer = new wxFlexGridSizer(3, 0, 0);
   
   mViewDefSizer->Add(coordSysLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mViewDefSizer->Add(mCoordSysComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewDefSizer->Add(emptyStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mViewDefSizer->Add(viewPointRefLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mViewDefSizer->Add(mViewPointRefComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewDefSizer->Add(mViewPointRefSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mViewDefSizer->Add(viewPointVectorLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mViewDefSizer->Add(mViewPointVectorComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewDefSizer->Add(mViewPointVectorSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mViewDefSizer->Add(viewScaleFactorLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mViewDefSizer->Add(mViewScaleFactorTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewDefSizer->Add(emptyStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mViewDefSizer->Add(viewDirectionLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mViewDefSizer->Add(mViewDirectionComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewDefSizer->Add(mViewDirVectorSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   wxStaticBox *viewDefStaticBox = new wxStaticBox(this, -1, wxT("View Definition"));
   wxStaticBoxSizer *viewDefStaticSizer
      = new wxStaticBoxSizer(viewDefStaticBox, wxVERTICAL);
   viewDefStaticSizer->Add(mViewDefSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //------------------------------------------------------
   // view up definition
   //------------------------------------------------------
   wxString axisArray[] = {"X", "-X", "Y", "-Y", "Z", "-Z"};
   
   wxStaticText *upCsLabel =
      new wxStaticText(this, -1, wxT("Coordinate System"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxStaticText *upAxisLabel =
      new wxStaticText(this, -1, wxT("Axis"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   mViewUpCsComboBox =
      theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX, wxSize(120,-1));
   
   mViewUpAxisComboBox =
      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition,
                     wxSize(120,-1), 6, axisArray, wxCB_DROPDOWN);
   
   wxStaticBox *upDefStaticBox = new wxStaticBox(this, -1, wxT("View Up Definition"));
   wxStaticBoxSizer *upDefStaticSizer
      = new wxStaticBoxSizer(upDefStaticBox, wxVERTICAL);
   upDefStaticSizer->Add(upCsLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   upDefStaticSizer->Add(mViewUpCsComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   upDefStaticSizer->Add(upAxisLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   upDefStaticSizer->Add(mViewUpAxisComboBox, 0, wxALIGN_LEFT|wxALL, bsize);   
   
   #if DEBUG_OPENGL_PANEL_CREATE
   MessageInterface::ShowMessage
      ("OpenGlPlotSetupPanel::Create() put in the order...\n");
   #endif
   
   //------------------------------------------------------
   // put in the order
   //------------------------------------------------------    
   wxBoxSizer *topViewBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   //topViewBoxSizer->Add(updateStaticSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   //topViewBoxSizer->Add(plotUpdateBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   topViewBoxSizer->Add(plotOptionStaticSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   topViewBoxSizer->Add(drawOptionStaticSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   topViewBoxSizer->Add(viewObjectStaticSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   mBottomViewSizer = new wxBoxSizer(wxHORIZONTAL);
   mBottomViewSizer->Add(viewOptionStaticSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mBottomViewSizer->Add(viewDefStaticSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mBottomViewSizer->Add(upDefStaticSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   pageBoxSizer->Add(topViewBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageBoxSizer->Add(mBottomViewSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   #if DEBUG_OPENGL_PANEL_CREATE
   MessageInterface::ShowMessage("OpenGlPlotSetupPanel::Create() exiting...\n");
   #endif
   
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::LoadData()
{
   #if DEBUG_OPENGL_PANEL_LOAD
      MessageInterface::ShowMessage("OpenGlPlotSetupPanel::LoadData() entering...\n");
   #endif

   try
   {
      // load data from the core engine
      wxString str;
      str.Printf("%d", mOpenGlPlot->GetIntegerParameter("DataCollectFrequency"));
      mDataCollectFreqTextCtrl->SetValue(str);
      str.Printf("%d", mOpenGlPlot->GetIntegerParameter("UpdatePlotFrequency"));
      mUpdatePlotFreqTextCtrl->SetValue(str);
      
      mPlotCheckBox->SetValue(mOpenGlPlot->IsActive());
      mEquatorialPlaneCheckBox->
         SetValue(mOpenGlPlot->GetStringParameter("EquatorialPlane") == "On");
      mEclipticPlaneCheckBox->
         SetValue(mOpenGlPlot->GetStringParameter("CelestialPlane") == "On");
      mWireFrameCheckBox->
         SetValue(mOpenGlPlot->GetStringParameter("WireFrame") == "On");
      mTargetStatusCheckBox->
         SetValue(mOpenGlPlot->GetStringParameter("TargetStatus") == "On");
      mAxesCheckBox->
         SetValue(mOpenGlPlot->GetStringParameter("Axes") == "On");
      mEarthSunLinesCheckBox->
         SetValue(mOpenGlPlot->GetStringParameter("EarthSunLines") == "On");
      //mOverlapCheckBox->
      //   SetValue(mOpenGlPlot->GetStringParameter("Overlap") == "On");
      mLockViewCheckBox->
         SetValue(mOpenGlPlot->GetStringParameter("LockView") == "On");
      mPerspectiveModeCheckBox->
         SetValue(mOpenGlPlot->GetStringParameter("PerspectiveMode") == "On");
      mUseFixedFovCheckBox->
         SetValue(mOpenGlPlot->GetStringParameter("UseFixedFov") == "On");
      
      str.Printf("%g", mOpenGlPlot->GetRealParameter("FixedFovAngle"));
      mFixedFovTextCtrl->SetValue(str);
      
      mCoordSysComboBox->SetStringSelection
         (mOpenGlPlot->GetStringParameter("CoordinateSystem").c_str());
      
      //--------------------------------------------------------------
      // load view up direction info
      //--------------------------------------------------------------
      mViewUpAxisComboBox->
         SetStringSelection(mOpenGlPlot->GetStringParameter("ViewUpAxis").c_str());
      mViewUpCsComboBox->
         SetStringSelection(mOpenGlPlot->GetStringParameter("ViewUpCoordinateSystem").c_str());
      
      //--------------------------------------------------------------
      // load ViewPoint info
      //--------------------------------------------------------------
      mViewPointRefComboBox->
         SetStringSelection(mOpenGlPlot->GetStringParameter("ViewPointRef").c_str());
      mViewPointVectorComboBox->
         SetStringSelection(mOpenGlPlot->GetStringParameter("ViewPointVector").c_str());
      mViewDirectionComboBox->
         SetStringSelection(mOpenGlPlot->GetStringParameter("ViewDirection").c_str());
      
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

         mViewDefSizer->Show(mViewPointRefSizer, true);
      }
      else
      {
         mViewDefSizer->Show(mViewPointRefSizer, false);
      }
      
      // show vector if viewpoint vector name is Vector
      if (mViewPointVectorComboBox->GetStringSelection() == "Vector")
      {
         Rvector vec = mOpenGlPlot->GetRvectorParameter("ViewPointVectorVector");
         
         #if DEBUG_OPENGL_PANEL_LOAD
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

         mViewDefSizer->Show(mViewPointVectorSizer, true);
      }
      else
      {
         mViewDefSizer->Show(mViewPointVectorSizer, false);
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
         
         mViewDefSizer->Show(mViewDirVectorSizer, true);
      }
      else
      {
         mViewDefSizer->Show(mViewDirVectorSizer, false);
      }
      
      // set layout
      mViewDefSizer->Layout();
      
      //--------------------------------------------------------------
      // get SpacePoint list to plot
      //--------------------------------------------------------------
      StringArray spNameList = mOpenGlPlot->GetSpacePointList();
      int spCount = spNameList.size();
      
      #if DEBUG_OPENGL_PANEL_LOAD
      MessageInterface::ShowMessage
         ("OpenGlPlotSetupPanel::LoadData() spCount=%d\n", spCount);
      
      for (int i=0; i<spCount; i++)
      {
         MessageInterface::ShowMessage
            ("OpenGlPlotSetupPanel::LoadData() spNameList[%d]=%s\n",
             i, spNameList[i].c_str());
      }
      #endif

      StringArray scNameArray;
      StringArray nonScNameArray;
      
      // get spacecraft and non-spacecraft list
      for (int i=0; i<spCount; i++)
      {
         if (mSpacecraftListBox->FindString(spNameList[i].c_str()) == wxNOT_FOUND)
            nonScNameArray.push_back(spNameList[i]);
         else
            scNameArray.push_back(spNameList[i]);
      }

      mScCount = scNameArray.size();
      mNonScCount = nonScNameArray.size();

      #if DEBUG_OPENGL_PANEL_LOAD
      MessageInterface::ShowMessage
         ("OpenGlPlotSetupPanel::LoadData() mScCount=%d, mNonScCount=%d\n",
          mScCount, mNonScCount);
      #endif
      
      if (mScCount > 0)
      {
         wxString *scNames = new wxString[mScCount];
         for (int i=0; i<mScCount; i++)
         {
            scNames[i] = scNameArray[i].c_str();
         
            mOrbitColorMap[scNameArray[i]]
               = RgbColor(mOpenGlPlot->GetColor("Orbit", scNameArray[i]));
            mTargetColorMap[scNameArray[i]]
               = RgbColor(mOpenGlPlot->GetColor("Target", scNameArray[i]));
         }
         
         mSelectedScListBox->Set(mScCount, scNames);
         delete scNames;

      }
      
      if (mNonScCount > 0)
      {
         wxString *nonScNames = new wxString[mNonScCount];
         for (int i=0; i<mNonScCount; i++)
         {
            nonScNames[i] = nonScNameArray[i].c_str();
         
            mOrbitColorMap[nonScNameArray[i]]
               = RgbColor(mOpenGlPlot->GetColor("Orbit", nonScNameArray[i]));
            mTargetColorMap[nonScNameArray[i]]
               = RgbColor(mOpenGlPlot->GetColor("Target", nonScNameArray[i]));
         }
         
         mSelectedObjListBox->Set(mNonScCount, nonScNames);
         delete nonScNames;

      }
      
      // show spacecraft option
      mSelectedScListBox->SetSelection(0);
      ShowSpacePointOption(mSelectedScListBox->GetStringSelection(), true);

         
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("OpenGlPlotSetupPanel:LoadData() error occurred!\n%s\n", e.GetMessage().c_str());
   }

   // deselect available object list
   mSpacecraftListBox->Deselect(mSpacecraftListBox->GetSelection());
   mCelesObjectListBox->Deselect(mCelesObjectListBox->GetSelection());
   
   mPerspectiveModeCheckBox->Enable();
   theApplyButton->Disable();
   
   if (!mUseFixedFovCheckBox->IsChecked())
   {
      mFovLabel->Disable();
      mFixedFovTextCtrl->Disable();
   }
   
   //5.13.mViewUpAxisComboBox->SetSelection(0);
   
   // if perspective mode, enalbe fov
   if (mPerspectiveModeCheckBox->IsChecked())
   {
      mUseFixedFovCheckBox->Enable();
      if (mUseFixedFovCheckBox->IsChecked())
      {
         mFovLabel->Enable();
         mFixedFovTextCtrl->Enable();
      }
      else
      {
         mFovLabel->Disable();
         mFixedFovTextCtrl->Disable();
      }
   }
   else
   {
      mUseFixedFovCheckBox->Disable();
      mFovLabel->Disable();
      mFixedFovTextCtrl->Disable();
   }

   
   #if DEBUG_OPENGL_PANEL_LOAD
   MessageInterface::ShowMessage("OpenGlPlotSetupPanel::LoadData() exiting...\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::SaveData()
{

   #if DEBUG_OPENGL_PANEL_SAVE
   MessageInterface::ShowMessage
      ("OpenGlPlotSetupPanel::SaveData() entered.\n");
   #endif
   
   try
   {
      // save data to core engine
      long longVal;
      mDataCollectFreqTextCtrl->GetValue().ToLong(&longVal);
      mOpenGlPlot->SetIntegerParameter("DataCollectFrequency", longVal);
      mUpdatePlotFreqTextCtrl->GetValue().ToLong(&longVal);
      mOpenGlPlot->SetIntegerParameter("UpdatePlotFrequency", longVal);
      
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
      
      if (mAxesCheckBox->IsChecked())
         mOpenGlPlot->SetStringParameter("Axes", "On");
      else
         mOpenGlPlot->SetStringParameter("Axes", "Off");
      
      if (mEarthSunLinesCheckBox->IsChecked())
         mOpenGlPlot->SetStringParameter("EarthSunLines", "On");
      else
         mOpenGlPlot->SetStringParameter("EarthSunLines", "Off");
      
      //if (mOverlapCheckBox->IsChecked())
      //   mOpenGlPlot->SetStringParameter("Overlap", "On");
      //else
      //   mOpenGlPlot->SetStringParameter("Overlap", "Off");
      
      if (mLockViewCheckBox->IsChecked())
         mOpenGlPlot->SetStringParameter("LockView", "On");
      else
         mOpenGlPlot->SetStringParameter("LockView", "Off");
      
      if (mPerspectiveModeCheckBox->IsChecked())
         mOpenGlPlot->SetStringParameter("PerspectiveMode", "On");
      else
         mOpenGlPlot->SetStringParameter("PerspectiveMode", "Off");
      
      if (mUseFixedFovCheckBox->IsChecked())
         mOpenGlPlot->SetStringParameter("UseFixedFov", "On");
      else
         mOpenGlPlot->SetStringParameter("UseFixedFov", "Off");
      
      Real fov;
      mFixedFovTextCtrl->GetValue().ToDouble(&fov);
      mOpenGlPlot->SetRealParameter("FixedFovAngle", fov);
      
      // save spacecraft list
      if (mHasSpChanged)
      {
         mHasSpChanged = false;
         mHasColorChanged = true;
      
         mScCount = mSelectedScListBox->GetCount();
         mNonScCount = mSelectedObjListBox->GetCount();
         
         #if DEBUG_OPENGL_PANEL_SAVE
         MessageInterface::ShowMessage
            ("OpenGlPlotSetupPanel::SaveData() mScCount=%d, mNonScCount=%d\n",
             mScCount, mNonScCount);
         #endif
         
         if (mScCount == 0 && mPlotCheckBox->IsChecked())
         {
            wxLogMessage(wxT("Spacecraft not selected. "
                             "The plot will not be activated."));
            mOpenGlPlot->Activate(false);
         }

         // clear the list first
         mOpenGlPlot->TakeAction("Clear");

         // add spacecraft
         for (int i=0; i<mScCount; i++)
         {
            mSelSpName = std::string(mSelectedScListBox->GetString(i).c_str());
            
            #if DEBUG_OPENGL_PANEL_SAVE
            MessageInterface::ShowMessage
               ("OpenGlPlotSetupPanel::SaveData() Sc[%d] = %s\n", i,
                mSelSpName.c_str());
            #endif
            
            mOpenGlPlot->
               SetStringParameter("Add", mSelSpName, i);
         }
         
         
         // add non-spacecraft
         for (int i=0; i<mNonScCount; i++)
         {
            mSelSpName = std::string(mSelectedObjListBox->GetString(i).c_str());
            
            #if DEBUG_OPENGL_PANEL_SAVE
            MessageInterface::ShowMessage
               ("OpenGlPlotSetupPanel::SaveData() NonSc[%d] = %s\n", i,
                mSelSpName.c_str());
            #endif
            
            mOpenGlPlot->
               SetStringParameter("Add", mSelSpName, mScCount+i);
         }
      }
            
      // save color
      if (mHasColorChanged)
      {
         mHasColorChanged = false;

         // change spacecraft color
         for (int i=0; i<mScCount; i++)
         {
            mSelSpName = std::string(mSelectedScListBox->GetString(i).c_str());
         
            mOpenGlPlot->
               SetColor("Orbit", mSelSpName,
                        mOrbitColorMap[mSelSpName].GetIntColor());
            mOpenGlPlot->
               SetColor("Target", mSelSpName,
                        mTargetColorMap[mSelSpName].GetIntColor());
         }
         
         // change non-spacecraft color
         for (int i=0; i<mNonScCount; i++)
         {
            mSelSpName = std::string(mSelectedObjListBox->GetString(i).c_str());
         
            mOpenGlPlot->
               SetColor("Orbit", mSelSpName,
                        mOrbitColorMap[mSelSpName].GetIntColor());
            mOpenGlPlot->
               SetColor("Target", mSelSpName,
                        mTargetColorMap[mSelSpName].GetIntColor());
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
      
      // save view up direction info
      if (mHasViewUpInfoChanged)
      {
         mHasViewUpInfoChanged = false;
         mOpenGlPlot->SetStringParameter
            ("ViewUpCoordinateSystem",
             std::string(mViewUpCsComboBox->GetStringSelection().c_str()));
         mOpenGlPlot->SetStringParameter
            ("ViewUpAxis",
             std::string(mViewUpAxisComboBox->GetStringSelection().c_str()));
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("OpenGlPlotSetupPanel:SaveData() error occurred!\n%s\n", e.GetMessage().c_str());
   }
}


//------------------------------------------------------------------------------
// void OnAddSpacePoint(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnAddSpacePoint(wxCommandEvent& event)
{
   if (mSpacecraftListBox->GetSelection() != -1)
   {
      // get string in first list and then search for it
      // in the second list
      wxString s = mSpacecraftListBox->GetStringSelection();
      int found = mSelectedScListBox->FindString(s);
    
      // if the string wasn't found in the second list, insert it
      if (found == wxNOT_FOUND)
      {
         mSelectedScListBox->Append(s);
         mSelectedScListBox->SetStringSelection(s);
      
         // select next available item
         mSpacecraftListBox->
            SetSelection(mSpacecraftListBox->GetSelection()+1);

         // deselect selected other object
         mSelectedObjListBox->Deselect(mSelectedObjListBox->GetSelection());

         ShowSpacePointOption(s, true, GmatColor::RED32);
         mHasSpChanged = true;
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
         
         // deselect selected spacecraft
         mSelectedScListBox->Deselect(mSelectedScListBox->GetSelection());
         
         ShowSpacePointOption(s, true, false, GmatColor::L_BROWN32);
         mHasSpChanged = true;
         theApplyButton->Enable();
      }
   }
}


//------------------------------------------------------------------------------
// void OnRemoveSpacePoint(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnRemoveSpacePoint(wxCommandEvent& event)
{
   if (mSelectedScListBox->GetSelection() != -1)
   {
      int sel = mSelectedScListBox->GetSelection();
      mSelectedScListBox->Delete(sel);
   
      if (sel-1 < 0)
      {
         mSelectedScListBox->SetSelection(0);
         if (mSelectedScListBox->GetCount() == 0)
            ShowSpacePointOption("", false); // hide spacecraft color, etc
         else
            ShowSpacePointOption(mSelectedScListBox->GetStringSelection(), true);
      }
      else
      {
         mSelectedScListBox->SetSelection(sel-1);
         ShowSpacePointOption(mSelectedScListBox->GetStringSelection(), true);
      }
   }
   else if (mSelectedObjListBox->GetSelection() != -1)
   {
      int sel = mSelectedObjListBox->GetSelection();
      mSelectedObjListBox->Delete(sel);
   
      if (sel-1 < 0)
      {
         mSelectedObjListBox->SetSelection(0);
         if (mSelectedObjListBox->GetCount() == 0)
            ShowSpacePointOption("", false); // hide spacecraft color, etc
         else
            ShowSpacePointOption(mSelectedObjListBox->GetStringSelection(), true);
      }
      else
      {
         mSelectedObjListBox->SetSelection(sel-1);
         ShowSpacePointOption(mSelectedObjListBox->GetStringSelection(), true);
      }
   }
   
   mHasSpChanged = true;
   theApplyButton->Enable();
}


//------------------------------------------------------------------------------
// void OnClearSpacePoint(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnClearSpacePoint(wxCommandEvent& event)
{
   if (mSelectedScListBox->GetSelection() != -1)
   {
      mSelectedScListBox->Clear();
   }
   else if (mSelectedObjListBox->GetSelection() != -1)
   {
      mSelectedObjListBox->Clear();
   }
   
   ShowSpacePointOption("", false);
   mHasSpChanged = true;
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
   ShowSpacePointOption(mSelectedScListBox->GetStringSelection(), true);
   mSelectedObjListBox->Deselect(mSelectedObjListBox->GetSelection());
}


//------------------------------------------------------------------------------
// void OnSelectOtherObject(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnSelectOtherObject(wxCommandEvent& event)
{
   ShowSpacePointOption(mSelectedObjListBox->GetStringSelection(), true, false);
   mSelectedScListBox->Deselect(mSelectedScListBox->GetSelection());
}


//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnCheckBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mPerspectiveModeCheckBox)
   {
      if (mPerspectiveModeCheckBox->IsChecked())
      {
         mUseFixedFovCheckBox->Enable();
         if (mUseFixedFovCheckBox->IsChecked())
         {
            mFovLabel->Enable();
            mFixedFovTextCtrl->Enable();
         }
         else
         {
            mFovLabel->Disable();
            mFixedFovTextCtrl->Disable();
         }
      }
      else
      {
          mUseFixedFovCheckBox->Disable();
          mFovLabel->Disable();
          mFixedFovTextCtrl->Disable();
      }
   }
   else if (event.GetEventObject() == mUseFixedFovCheckBox)
   {
      if (mUseFixedFovCheckBox->IsChecked())
      {
         mFovLabel->Enable();
         mFixedFovTextCtrl->Enable();
      }
      else
      {
         mFovLabel->Disable();
         mFixedFovTextCtrl->Disable();
      }
   }
   
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnOrbitColorClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnOrbitColorClick(wxCommandEvent& event)
{
   wxColourData data;
   data.SetColour(mOrbitColor);

   wxColourDialog dialog(this, &data);
   //dialog.CenterOnParent();
   dialog.Center();
   
   if (dialog.ShowModal() == wxID_OK)
   {
      // if spacecraft is selected
      if (mSelectedScListBox->GetSelection() != -1)
      {
         mSelSpName = std::string(mSelectedScListBox->GetStringSelection().c_str());
      
         mOrbitColor = dialog.GetColourData().GetColour();
         mOrbitColorButton->SetBackgroundColour(mOrbitColor);
      
         mOrbitColorMap[mSelSpName].Set(mOrbitColor.Red(),
                                        mOrbitColor.Green(),
                                        mOrbitColor.Blue());

         #if DEBUG_OPENGL_PANEL
         MessageInterface::ShowMessage("OnOrbitColorClick() r=%d g=%d b=%d\n",
                                       mOrbitColor.Red(), mOrbitColor.Green(),
                                       mOrbitColor.Blue());

         MessageInterface::ShowMessage("OnOrbitColorClick() UnsignedInt=%d\n",
                                       mOrbitColorMap[mSelSpName].GetIntColor());
         #endif
      }
      else if (mSelectedObjListBox->GetSelection() != -1)
      {
         mSelSpName = std::string(mSelectedObjListBox->GetStringSelection().c_str());
      
         mOrbitColor = dialog.GetColourData().GetColour();
         mOrbitColorButton->SetBackgroundColour(mOrbitColor);
      
         mOrbitColorMap[mSelSpName].Set(mOrbitColor.Red(),
                                        mOrbitColor.Green(),
                                        mOrbitColor.Blue());

         #if DEBUG_OPENGL_PANEL
         MessageInterface::ShowMessage("OnOrbitColorClick() r=%d g=%d b=%d\n",
                                       mOrbitColor.Red(), mOrbitColor.Green(),
                                       mOrbitColor.Blue());

         MessageInterface::ShowMessage("OnOrbitColorClick() UnsignedInt=%d\n",
                                       mOrbitColorMap[mSelSpName].GetIntColor());
         #endif
      }
      
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
   data.SetColour(mTargetColor);

   wxColourDialog dialog(this, &data);
   //dialog.CenterOnParent();
   dialog.Center();
   
   if (dialog.ShowModal() == wxID_OK)
   {
      mSelSpName = std::string(mSelectedObjListBox->GetStringSelection().c_str());
      
      mTargetColor = dialog.GetColourData().GetColour();
      mTargetColorButton->SetBackgroundColour(mTargetColor);
      mTargetColorMap[mSelSpName].Set(mTargetColor.Red(),
                                      mTargetColor.Green(),
                                      mTargetColor.Blue());
      
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
   else if (event.GetEventObject() == mViewUpCsComboBox ||
            event.GetEventObject() == mViewUpAxisComboBox)
   {
      mHasViewUpInfoChanged = true;
   }
   else if (event.GetEventObject() == mViewPointRefComboBox)
   {
      mHasViewInfoChanged = true;
      
      if (mViewPointRefComboBox->GetStringSelection() == "Vector")
         mViewDefSizer->Show(mViewPointRefSizer, true);
      else
         mViewDefSizer->Show(mViewPointRefSizer, false);
      
      //mViewDefSizer->Layout();
      mBottomViewSizer->Layout();
   }
   else if (event.GetEventObject() == mViewPointVectorComboBox)
   {
      mHasViewInfoChanged = true;
      
      if (mViewPointVectorComboBox->GetStringSelection() == "Vector")
         mViewDefSizer->Show(mViewPointVectorSizer, true);
      else
         mViewDefSizer->Show(mViewPointVectorSizer, false);
      
      //mViewDefSizer >Layout();
      mBottomViewSizer->Layout();      
   }
   else if (event.GetEventObject() == mViewDirectionComboBox)
   {
      mHasViewInfoChanged = true;
      
      if (mViewDirectionComboBox->GetStringSelection() == "Vector")
         mViewDefSizer->Show(mViewDirVectorSizer, true);
      else
         mViewDefSizer->Show(mViewDirVectorSizer, false);
      
      //mViewDefSizer->Layout();
      mBottomViewSizer->Layout();      
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
// void ShowSpacePointOption(const wxString &name, bool show = true,
//                           bool isSc = true,
//                           UnsignedInt color = GmatColor::RED32)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::ShowSpacePointOption(const wxString &name, bool show,
                                                bool isSc, UnsignedInt color)
{
   #if DEBUG_OPENGL_PANEL
   MessageInterface::ShowMessage
      ("OpenGlPlotSetupPanel::ShowSpacePointOption() name=%s, show=%d, isSc=%d\n",
       name.c_str(), show, isSc);
   #endif
   
   if (!name.IsSameAs(""))
   {
      mSelSpName = std::string(name.c_str());

      // if object name not found, insert
      if (mOrbitColorMap.find(mSelSpName) == mOrbitColorMap.end())
      {
         mOrbitColorMap[mSelSpName] = RgbColor(color);
         mTargetColorMap[mSelSpName] = RgbColor(GmatColor::ORANGE32);
      }
      
      RgbColor orbColor = mOrbitColorMap[mSelSpName];
      RgbColor targColor = mTargetColorMap[mSelSpName];
      
      #if DEBUG_OPENGL_PANEL
      MessageInterface::ShowMessage
         ("ShowSpacePointOption() orbColor=%08x targColor=%08x\n",
          orbColor.GetIntColor(), targColor.GetIntColor());
      #endif
      
      mOrbitColor.Set(orbColor.Red(), orbColor.Green(), orbColor.Blue());
      mTargetColor.Set(targColor.Red(), targColor.Green(), targColor.Blue());
      
      mOrbitColorButton->SetBackgroundColour(mOrbitColor);
      mTargetColorButton->SetBackgroundColour(mTargetColor);

      if (isSc)
      {
         mTargetColorLabel->Enable();
         mTargetColorButton->Enable();
      }
      else
      {
         mTargetColorLabel->Disable();
         mTargetColorButton->Disable();
      }
      
      mObjectGridSizer->Show(mScOptionBoxSizer, show);
   }
   else
   {
      mObjectGridSizer->Show(mScOptionBoxSizer, false);
   }
   
   mObjectGridSizer->Layout();
}
