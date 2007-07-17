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
#include "GmatStaticBoxSizer.hpp"
#include "StringUtil.hpp"

#include "ColorTypes.hpp"           // for namespace GmatColor::
#include "MessageInterface.hpp"

#include "wx/colordlg.h"            // for wxColourDialog

// To enable perspective mode
//#define __ENABLE_GL_PERSPECTIVE__

//#define DEBUG_OPENGL_PANEL 1
//#define DEBUG_OPENGL_PANEL_CREATE 1
//#define DEBUG_OPENGL_PANEL_LOAD 1
//#define DEBUG_OPENGL_PANEL_SAVE 1
//#define DEBUG_OPENGL_PANEL_CHECKBOX 1
//#define DEBUG_OPENGL_PANEL_SHOW 1

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
   Subscriber *subscriber = (Subscriber*)
      theGuiInterpreter->GetConfiguredObject(subscriberName.c_str());
   
   mOpenGlPlot = (OpenGlPlot*)subscriber;

   // Set the pointer for the "Show Script" button
   mObject = mOpenGlPlot;
   
   mHasSpChanged = false;
   mHasOrbitColorChanged = false;
   mHasTargetColorChanged = false;
   mHasShowObjectChanged = false;
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
      MessageInterface::ShowMessage("OpenGlPlotSetupPanel::Create() entered\n");
   #endif
   
   Integer bsize = 2; // border size
   
   // create axis array
   wxArrayString emptyList;
   wxString axisArray[] = {"X", "-X", "Y", "-Y", "Z", "-Z"};   
   wxArrayString empty;
   wxStaticText *emptyStaticText =
      new wxStaticText( this, -1, wxT("  "), wxDefaultPosition, wxDefaultSize, 0 );   
   
   
   //-----------------------------------------------------------------
   // platform dependent button size
   //-----------------------------------------------------------------
   int arrowW = 20;
   int colorW = 25;
   #ifdef __WXMAC__
   arrowW = 40;
   colorW = 10;
   #else
   arrowW = 20;
   colorW = 25;
   #endif
   
   //-----------------------------------------------------------------
   // Data collect and update frequency
   //-----------------------------------------------------------------
   wxBoxSizer *plotOptionSizer = new wxBoxSizer(wxVERTICAL);
   
   wxStaticText *dataCollectFreqLabel1 =
      new wxStaticText(this, -1, wxT("Collect data every "),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxStaticText *dataCollectFreqLabel2 =
      new wxStaticText(this, -1, wxT("step"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxStaticText *updatePlotFreqLabel1 =
      new wxStaticText(this, -1, wxT("Update plot every "),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxStaticText *updatePlotFreqLabel2 =
      new wxStaticText(this, -1, wxT("cycle"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   mDataCollectFreqTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(35, 20), 0);
   
   mUpdatePlotFreqTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(35, 20), 0);
   
   wxBoxSizer *colFreqSizer = new wxBoxSizer(wxHORIZONTAL);
   colFreqSizer->Add(dataCollectFreqLabel1, 0, wxALIGN_LEFT|wxALL, bsize);
   colFreqSizer->Add(mDataCollectFreqTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   colFreqSizer->Add(dataCollectFreqLabel2, 0, wxALIGN_LEFT|wxALL, bsize);
   
   wxBoxSizer *updFreqSizer = new wxBoxSizer(wxHORIZONTAL);
   updFreqSizer->Add(updatePlotFreqLabel1, 0, wxALIGN_LEFT|wxALL, bsize);
   updFreqSizer->Add(mUpdatePlotFreqTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   updFreqSizer->Add(updatePlotFreqLabel2, 0, wxALIGN_LEFT|wxALL, bsize);
   
   plotOptionSizer->Add(colFreqSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   plotOptionSizer->Add(updFreqSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   wxStaticText *numPointsToRedrawLabel1 =
      new wxStaticText(this, -1, wxT("Number of points to redraw\n"
                                     "(Enter 0 to redraw whole plot)"),
                       wxDefaultPosition, wxSize(-1, 30), 0);
   mNumPointsToRedrawTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(30, 20), 0);
   
   wxBoxSizer *numPointsSizer = new wxBoxSizer(wxHORIZONTAL);
   numPointsSizer->Add(numPointsToRedrawLabel1, 0, wxALIGN_LEFT|wxALL, 0);
   numPointsSizer->Add(mNumPointsToRedrawTextCtrl, 0, wxALIGN_LEFT|wxALL, 0);
   
   plotOptionSizer->Add(numPointsSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Show plot
   //-----------------------------------------------------------------
   mShowPlotCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Show Plot"),
                     wxDefaultPosition, wxSize(-1, -1), bsize);
   
   plotOptionSizer->AddSpacer(10);
   plotOptionSizer->Add(mShowPlotCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   GmatStaticBoxSizer *plotOptionStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Plot Option");
   
   plotOptionStaticSizer->Add(plotOptionSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // View option
   //-----------------------------------------------------------------
   wxBoxSizer *viewOptionSizer = new wxBoxSizer(wxVERTICAL);
   
   mUseInitialViewCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Use Initial View Def."),
                     wxDefaultPosition, wxSize(-1, -1), bsize);
   
   viewOptionSizer->Add(mUseInitialViewCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   #ifdef __ENABLE_GL_PERSPECTIVE__
   
   mPerspectiveModeCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Use Perspective Mode"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   mUseFixedFovCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Use Fixed FOV Angle"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   viewOptionSizer->Add(mPerspectiveModeCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   viewOptionSizer->Add(mUseFixedFovCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mFovLabel =
      new wxStaticText(this, -1, wxT("Field Of View (deg): "),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mFixedFovTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(35, -1), 0);
   
   wxBoxSizer *fovSizer = new wxBoxSizer(wxHORIZONTAL);
   fovSizer->Add(mFovLabel, 0, wxALIGN_LEFT|wxALL, 0);
   fovSizer->Add(mFixedFovTextCtrl, 0, wxALIGN_LEFT|wxALL, 0);
   viewOptionSizer->Add(fovSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   #endif
   
   GmatStaticBoxSizer *viewOptionStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "View Option");
   viewOptionStaticSizer->Add(viewOptionSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Drawing option
   //-----------------------------------------------------------------
   wxBoxSizer *drawOptionSizer = new wxBoxSizer(wxVERTICAL);
      
   mWireFrameCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw WireFrame"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   mTargetStatusCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw Targeting"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   mEclipticPlaneCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw Ecliptic Plane"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   mXYPlaneCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw XY Plane"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   mAxesCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw Axes"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   mGridCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw Grid"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   mOriginSunLineCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw Sun Line"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   drawOptionSizer->Add(20, 2, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionSizer->Add(mWireFrameCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionSizer->Add(mTargetStatusCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionSizer->Add(mEclipticPlaneCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionSizer->Add(mXYPlaneCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionSizer->Add(mAxesCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionSizer->Add(mGridCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionSizer->Add(mOriginSunLineCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionSizer->Add(20, 2, 0, wxALIGN_LEFT|wxALL, bsize);
   
   GmatStaticBoxSizer *drawOptionStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Drawing Option");
   drawOptionStaticSizer->Add(drawOptionSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Available spcecrafts and objects
   //-----------------------------------------------------------------
   wxStaticText *scAvailableLabel =
      new wxStaticText(this, -1, wxT("Spacecraft"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mSpacecraftListBox = theGuiManager->
      GetSpacecraftListBox(this, ID_LISTBOX, wxSize(150,65), &mExcludedScList);
   wxStaticText *coAvailableLabel =
      new wxStaticText(this, -1, wxT("Celestial Object"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mCelesObjectListBox = theGuiManager->
      GetCelestialPointListBox(this, ID_LISTBOX, wxSize(150,65), empty);
   
   wxBoxSizer *availObjSizer = new wxBoxSizer(wxVERTICAL);
   availObjSizer->Add(scAvailableLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   availObjSizer->Add(mSpacecraftListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   availObjSizer->Add(coAvailableLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   availObjSizer->Add(mCelesObjectListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // add, remove, clear buttons
   //-----------------------------------------------------------------
   addScButton = new wxButton(this, ADD_SP_BUTTON, wxT("-->"),
                              wxDefaultPosition, wxSize(arrowW, 20), 0);
   removeScButton = new wxButton(this, REMOVE_SP_BUTTON, wxT("<--"),
                                 wxDefaultPosition, wxSize(arrowW ,20), 0);
   clearScButton = new wxButton(this, CLEAR_SP_BUTTON, wxT("<="),
                                wxDefaultPosition, wxSize(arrowW, 20), 0);
   
   wxBoxSizer *arrowButtonsSizer = new wxBoxSizer(wxVERTICAL);
   arrowButtonsSizer->Add(addScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsSizer->Add(removeScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsSizer->Add(clearScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Selected spacecraft and objects
   //-----------------------------------------------------------------
   wxStaticText *titleSelectedSc =
      new wxStaticText(this, -1, wxT("Selected Spacecraft"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxStaticText *titleSelectedObj =
      new wxStaticText(this, -1, wxT("Selected Celestial Object"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mSelectedScListBox =
      new wxListBox(this, SC_SEL_LISTBOX, wxDefaultPosition, wxSize(150,65), //0,
                    emptyList, wxLB_SINGLE);
   mSelectedObjListBox =
      new wxListBox(this, OBJ_SEL_LISTBOX, wxDefaultPosition, wxSize(150,65), //0,
                    emptyList, wxLB_SINGLE);
   
   wxBoxSizer *mObjSelectedSizer = new wxBoxSizer(wxVERTICAL);
   mObjSelectedSizer->Add(titleSelectedSc, 0, wxALIGN_CENTRE|wxALL, bsize);
   mObjSelectedSizer->Add(mSelectedScListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   mObjSelectedSizer->Add(titleSelectedObj, 0, wxALIGN_CENTRE|wxALL, bsize);
   mObjSelectedSizer->Add(mSelectedObjListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Draw object, orbit and target color
   //-----------------------------------------------------------------
   mDrawObjectCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw Object"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   wxStaticText *orbitColorLabel =
      new wxStaticText(this, -1, wxT("Orbit Color"),
                       wxDefaultPosition, wxSize(-1,-1), wxALIGN_CENTRE);
   mTargetColorButton = new wxButton(this, TARGET_COLOR_BUTTON, "",
                                     wxDefaultPosition, wxSize(colorW, 20), 0);
   mTargetColorLabel =
      new wxStaticText(this, -1, wxT("Target Color"),
                       wxDefaultPosition, wxSize(-1,-1), wxALIGN_CENTRE);
   mOrbitColorButton = new wxButton(this, ORBIT_COLOR_BUTTON, "",
                                    wxDefaultPosition, wxSize(colorW, 20), 0);
   
   wxFlexGridSizer *scOptionSizer1 = new wxFlexGridSizer(1, 0, 0);
   scOptionSizer1->Add(mDrawObjectCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   scOptionSizer1->Add(20, 10, 0, wxALIGN_LEFT|wxALL, bsize);
   scOptionSizer1->Add(orbitColorLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   scOptionSizer1->Add(mOrbitColorButton, 0, wxALIGN_LEFT|wxALL, bsize);
   scOptionSizer1->Add(mTargetColorLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   scOptionSizer1->Add(mTargetColorButton, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mScOptionSizer = new wxBoxSizer(wxVERTICAL);
   mScOptionSizer->Add(scOptionSizer1, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mObjectSizer = new wxFlexGridSizer(5, 0, 0);
   mObjectSizer->Add(availObjSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mObjectSizer->Add(arrowButtonsSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mObjectSizer->Add(mObjSelectedSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mObjectSizer->Add(mScOptionSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   GmatStaticBoxSizer *viewObjectStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "View Object");
   viewObjectStaticSizer->Add(mObjectSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // View point reference
   //-----------------------------------------------------------------
   mViewPointRef1TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(60,-1), 0);
   mViewPointRef2TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(60,-1), 0);
   mViewPointRef3TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(60,-1), 0);
   mViewPointRefStaticText = new wxStaticText(this, -1, wxT("km"),
     wxDefaultPosition, wxSize(-1,-1), 0);
   
   mViewPointRefSizer = new wxBoxSizer(wxHORIZONTAL);
   mViewPointRefSizer->Add(mViewPointRef1TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewPointRefSizer->Add(mViewPointRef2TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewPointRefSizer->Add(mViewPointRef3TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewPointRefSizer->Add(mViewPointRefStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // View point vector
   //-----------------------------------------------------------------
   mViewPointVec1TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(60,-1), 0);
   mViewPointVec2TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(60,-1), 0);
   mViewPointVec3TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("30000"), wxDefaultPosition, wxSize(60,-1), 0);
   
   mViewPointVecStaticText = new wxStaticText(this, -1, wxT("km"),
     wxDefaultPosition, wxSize(-1,-1), 0);
   
   mViewPointVectorSizer = new wxBoxSizer(wxHORIZONTAL);
   mViewPointVectorSizer->Add(mViewPointVec1TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewPointVectorSizer->Add(mViewPointVec2TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewPointVectorSizer->Add(mViewPointVec3TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewPointVectorSizer->Add(mViewPointVecStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // View direction
   //-----------------------------------------------------------------
   mViewDirVectorSizer = new wxBoxSizer(wxHORIZONTAL);
   
   mViewDir1TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(60,-1), 0);
   mViewDir2TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(60,-1), 0);
   mViewDir3TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("-1"), wxDefaultPosition, wxSize(60,-1), 0);
   mViewDirStaticText = new wxStaticText(this, -1, wxT("km"),
     wxDefaultPosition, wxSize(-1,-1), 0);

   mViewDirVectorSizer->Add(mViewDir1TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewDirVectorSizer->Add(mViewDir2TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewDirVectorSizer->Add(mViewDir3TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewDirVectorSizer->Add(mViewDirStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   
   wxStaticText *coordSysLabel =
      new wxStaticText(this, -1, wxT("Coordinate System"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mCoordSysComboBox =
      theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX, wxSize(120,-1));
   
   wxStaticText *viewPointRefLabel =
      new wxStaticText(this, -1, wxT("View Point Reference"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mViewPointRefComboBox =
      theGuiManager->GetSpacePointComboBox(this, ID_COMBOBOX, wxSize(120,-1), true);
   
   wxStaticText *viewPointVectorLabel =
      new wxStaticText(this, -1, wxT("View Point Vector"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mViewPointVectorComboBox =
      theGuiManager->GetSpacePointComboBox(this, ID_COMBOBOX, wxSize(120,-1), true);
   
   //-----------------------------------------------------------------
   // Add to view definitin sizer
   //-----------------------------------------------------------------
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
   
   wxStaticText *viewScaleFactorLabel =
      new wxStaticText(this, -1, wxT("View Scale Factor"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mViewScaleFactorTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0);

   mViewDefSizer->Add(viewScaleFactorLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mViewDefSizer->Add(mViewScaleFactorTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewDefSizer->Add(emptyStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   
   wxStaticText *viewDirectionLabel =
      new wxStaticText(this, -1, wxT("View Direction"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mViewDirectionComboBox =
      theGuiManager->GetSpacePointComboBox(this, ID_COMBOBOX, wxSize(120,-1), true);
   
   mViewDefSizer->Add(viewDirectionLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mViewDefSizer->Add(mViewDirectionComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewDefSizer->Add(mViewDirVectorSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   GmatStaticBoxSizer *viewDefStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "View Definition");
   viewDefStaticSizer->Add(mViewDefSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // View up definition
   //-----------------------------------------------------------------
   wxStaticText *upCsLabel =
      new wxStaticText(this, -1, wxT("Coordinate System"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mViewUpCsComboBox =
      theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX, wxSize(120,-1));
   wxStaticText *upAxisLabel =
      new wxStaticText(this, -1, wxT("Axis"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mViewUpAxisComboBox =
      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition, 
                     wxSize(50,-1),6, axisArray, wxCB_READONLY);
   
   wxBoxSizer *viewUpSizer = new wxBoxSizer(wxHORIZONTAL);
   viewUpSizer->Add(upCsLabel, 0, wxALIGN_CENTER|wxALL, bsize);
   viewUpSizer->Add(mViewUpCsComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   viewUpSizer->AddSpacer(20);
   viewUpSizer->Add(upAxisLabel, 0, wxALIGN_CENTER|wxALL, bsize);
   viewUpSizer->Add(mViewUpAxisComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   
   GmatStaticBoxSizer *upDefStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "View Up Definition");
   
   upDefStaticSizer->Add(viewUpSizer, 0, wxALIGN_LEFT|wxALL, bsize);
      
   //-----------------------------------------------------------------
   // Add to page sizer
   //-----------------------------------------------------------------
   
   wxFlexGridSizer *pageSizer1 = new wxFlexGridSizer(3, 2, 0, 0);
   pageSizer1->Add(plotOptionStaticSizer, 0, wxALIGN_CENTRE|wxGROW|wxALL, bsize);
   pageSizer1->Add(viewObjectStaticSizer, 0, wxALIGN_CENTRE|wxGROW|wxALL, bsize);
   
   pageSizer1->Add(drawOptionStaticSizer, 0, wxALIGN_CENTRE|wxGROW|wxALL, bsize);   
   pageSizer1->Add(viewDefStaticSizer, 0, wxALIGN_CENTRE|wxGROW|wxALL, bsize);
   
   pageSizer1->Add(viewOptionStaticSizer, 0, wxALIGN_CENTRE|wxGROW|wxALL, bsize);
   pageSizer1->Add(upDefStaticSizer, 0, wxALIGN_CENTRE|wxGROW|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Add to middle sizer
   //-----------------------------------------------------------------
   wxBoxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);
   pageSizer->Add(pageSizer1, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   theMiddleSizer->Add(pageSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   #if DEBUG_OPENGL_PANEL_CREATE
   MessageInterface::ShowMessage
      ("OpenGlPlotSetupPanel::Create() Exiting sizers for Windows\n");
   #endif
   
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
      Real rval;
      
      str.Printf("%d", mOpenGlPlot->GetIntegerParameter("DataCollectFrequency"));
      mDataCollectFreqTextCtrl->SetValue(str);
      str.Printf("%d", mOpenGlPlot->GetIntegerParameter("UpdatePlotFrequency"));
      mUpdatePlotFreqTextCtrl->SetValue(str);
      str.Printf("%d", mOpenGlPlot->GetIntegerParameter("NumPointsToRedraw"));
      mNumPointsToRedrawTextCtrl->SetValue(str);
      
      mShowPlotCheckBox->SetValue(mOpenGlPlot->IsActive());
      mXYPlaneCheckBox->
         SetValue(mOpenGlPlot->GetOnOffParameter("XYPlane") == "On");
      mEclipticPlaneCheckBox->
         SetValue(mOpenGlPlot->GetOnOffParameter("CelestialPlane") == "On");
      mWireFrameCheckBox->
         SetValue(mOpenGlPlot->GetOnOffParameter("WireFrame") == "On");
      mTargetStatusCheckBox->
         SetValue(mOpenGlPlot->GetOnOffParameter("TargetStatus") == "On");
      mAxesCheckBox->
         SetValue(mOpenGlPlot->GetOnOffParameter("Axes") == "On");
      mGridCheckBox->
         SetValue(mOpenGlPlot->GetOnOffParameter("Grid") == "On");
      mOriginSunLineCheckBox->
         SetValue(mOpenGlPlot->GetOnOffParameter("EarthSunLines") == "On");
      mUseInitialViewCheckBox->
         SetValue(mOpenGlPlot->GetOnOffParameter("UseInitialView") == "On");

      #ifdef __ENABLE_GL_PERSPECTIVE__
      mPerspectiveModeCheckBox->
         SetValue(mOpenGlPlot->GetOnOffParameter("PerspectiveMode") == "On");
      mUseFixedFovCheckBox->
         SetValue(mOpenGlPlot->GetOnOffParameter("UseFixedFov") == "On");
      rval = mOpenGlPlot->GetRealParameter("FixedFovAngle");
      mFixedFovTextCtrl->SetValue(theGuiManager->ToWxString(rval));
      #endif
      
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

      rval = mOpenGlPlot->GetRealParameter("ViewScaleFactor");
      mViewScaleFactorTextCtrl->SetValue(theGuiManager->ToWxString(rval));
      
      // show vector if viewpoint vector name is Vector
      if (mViewPointRefComboBox->GetStringSelection() == "Vector")
      {
         Rvector vec = mOpenGlPlot->GetRvectorParameter("ViewPointRefVector");
         
         #if DEBUG_OPENGL_PANEL_LOAD
         MessageInterface::ShowMessage
            ("OpenGlPlotSetupPanel::LoadData() ViewPointRefVector = %s\n",
             vec.ToString().c_str());
         #endif
         
         mViewPointRef1TextCtrl->SetValue(theGuiManager->ToWxString(vec[0]));
         mViewPointRef2TextCtrl->SetValue(theGuiManager->ToWxString(vec[1]));
         mViewPointRef3TextCtrl->SetValue(theGuiManager->ToWxString(vec[2]));

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
         
         mViewPointVec1TextCtrl->SetValue(theGuiManager->ToWxString(vec[0]));
         mViewPointVec2TextCtrl->SetValue(theGuiManager->ToWxString(vec[1]));
         mViewPointVec3TextCtrl->SetValue(theGuiManager->ToWxString(vec[2]));
         
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
         mViewDir1TextCtrl->SetValue(theGuiManager->ToWxString(vec[0]));
         mViewDir2TextCtrl->SetValue(theGuiManager->ToWxString(vec[1]));
         mViewDir3TextCtrl->SetValue(theGuiManager->ToWxString(vec[2]));
         
         mViewDefSizer->Show(mViewDirVectorSizer, true);
      }
      else
      {
         mViewDefSizer->Show(mViewDirVectorSizer, false);
      }
      
      // set layout
      //mBottomViewSizer->Layout();

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
      
      //--------------------------------------------------------------
      // get object show, color
      //--------------------------------------------------------------
      if (mScCount > 0)
      {
         wxString *scNames = new wxString[mScCount];
         for (int i=0; i<mScCount; i++)
         {
            scNames[i] = scNameArray[i].c_str();
            
            mDrawObjectMap[scNameArray[i]] =
               mOpenGlPlot->GetShowObject(scNameArray[i]);
            mOrbitColorMap[scNameArray[i]]
               = RgbColor(mOpenGlPlot->GetColor("Orbit", scNameArray[i]));
            mTargetColorMap[scNameArray[i]]
               = RgbColor(mOpenGlPlot->GetColor("Target", scNameArray[i]));
            
            #if DEBUG_OPENGL_PANEL_LOAD > 1
            MessageInterface::ShowMessage
               ("OpenGlPlotSetupPanel::LoadData() scName=%s, orbColor=%u, "
                "targetColor=%u\n", scNameArray[i].c_str(),
                mOrbitColorMap[scNameArray[i]].GetIntColor(),
                mTargetColorMap[scNameArray[i]].GetIntColor());
            #endif
         }
         
         mSelectedScListBox->Set(mScCount, scNames);
         delete [] scNames;
      }
      
      if (mNonScCount > 0)
      {
         wxString *nonScNames = new wxString[mNonScCount];
         for (int i=0; i<mNonScCount; i++)
         {
            nonScNames[i] = nonScNameArray[i].c_str();
         
            mDrawObjectMap[nonScNameArray[i]] =
               mOpenGlPlot->GetShowObject(nonScNameArray[i]);
            mOrbitColorMap[nonScNameArray[i]]
               = RgbColor(mOpenGlPlot->GetColor("Orbit", nonScNameArray[i]));
            mTargetColorMap[nonScNameArray[i]]
               = RgbColor(mOpenGlPlot->GetColor("Target", nonScNameArray[i]));
         }
         
         mSelectedObjListBox->Set(mNonScCount, nonScNames);
         delete [] nonScNames;
      }
      
      // show spacecraft option
      mSelectedScListBox->SetSelection(0);
      ShowSpacePointOption(mSelectedScListBox->GetStringSelection(), true);
      
      
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("OpenGlPlotSetupPanel:LoadData() error occurred!\n%s\n",
          e.GetFullMessage().c_str());
   }

   // deselect available object list
   mSpacecraftListBox->Deselect(mSpacecraftListBox->GetSelection());
   mCelesObjectListBox->Deselect(mCelesObjectListBox->GetSelection());

   #ifdef __ENABLE_GL_PERSPECTIVE__
   mPerspectiveModeCheckBox->Enable();
   #endif
   
   EnableUpdate(false);
   
   #ifdef __ENABLE_GL_PERSPECTIVE__
   if (!mUseFixedFovCheckBox->IsChecked())
   {
      mFovLabel->Disable();
      mFixedFovTextCtrl->Disable();
   }

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
   #endif
   
   
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
   
   canClose = false;
   try
   {
      // save data to core engine
      Integer intVal[2];
      std::string inputString[3];
      std::string msg = "The value of \"%s\" for field \"%s\" on object \"" +
                 mOpenGlPlot->GetName() + "\" is not an allowed value.  "
                 "\nThe allowed values are: [ %s ].";
      
      inputString[0] = mDataCollectFreqTextCtrl->GetValue();
      inputString[1] = mUpdatePlotFreqTextCtrl->GetValue();
      for (Integer i=0; i < 2; ++i)
      {
          if (!GmatStringUtil::ToInteger(inputString[i], &intVal[i]) || 
              intVal[i] < 1)
          {
             MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
                 inputString[i].c_str(), "DataCollectFrequency","Integer >= 1");
             return;
          }
      }

      mOpenGlPlot->SetIntegerParameter("DataCollectFrequency", intVal[0]);
      mOpenGlPlot->SetIntegerParameter("UpdatePlotFrequency", intVal[1]);

      inputString[0] = mNumPointsToRedrawTextCtrl->GetValue();
      if (!GmatStringUtil::ToInteger(inputString[0], &intVal[0]) || 
          intVal[0] < 0)
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
                 inputString[0].c_str(), "NumPointsToRedraw","Integer >= 0");
         return;
      }
      
      mOpenGlPlot->SetIntegerParameter("NumPointsToRedraw", intVal[0]);
      
      mOpenGlPlot->Activate(mShowPlotCheckBox->IsChecked());
      
      if (mXYPlaneCheckBox->IsChecked())
         mOpenGlPlot->SetOnOffParameter("XYPlane", "On");
      else
         mOpenGlPlot->SetOnOffParameter("XYPlane", "Off");
      
      if (mEclipticPlaneCheckBox->IsChecked())
         mOpenGlPlot->SetOnOffParameter("CelestialPlane", "On");
      else
         mOpenGlPlot->SetOnOffParameter("CelestialPlane", "Off");
      
      if (mWireFrameCheckBox->IsChecked())
         mOpenGlPlot->SetOnOffParameter("WireFrame", "On");
      else
         mOpenGlPlot->SetOnOffParameter("WireFrame", "Off");
      
      if (mTargetStatusCheckBox->IsChecked())
         mOpenGlPlot->SetOnOffParameter("TargetStatus", "On");
      else
         mOpenGlPlot->SetOnOffParameter("TargetStatus", "Off");
      
      if (mAxesCheckBox->IsChecked())
         mOpenGlPlot->SetOnOffParameter("Axes", "On");
      else
         mOpenGlPlot->SetOnOffParameter("Axes", "Off");
      
      if (mGridCheckBox->IsChecked())
         mOpenGlPlot->SetOnOffParameter("Grid", "On");
      else
         mOpenGlPlot->SetOnOffParameter("Grid", "Off");
      
      if (mOriginSunLineCheckBox->IsChecked())
         mOpenGlPlot->SetOnOffParameter("EarthSunLines", "On");
      else
         mOpenGlPlot->SetOnOffParameter("EarthSunLines", "Off");
      
      //if (mOverlapCheckBox->IsChecked())
      //   mOpenGlPlot->SetOnOffParameter("Overlap", "On");
      //else
      //   mOpenGlPlot->SetOnOffParameter("Overlap", "Off");
      
      if (mUseInitialViewCheckBox->IsChecked())
         mOpenGlPlot->SetOnOffParameter("UseInitialView", "On");
      else
         mOpenGlPlot->SetOnOffParameter("UseInitialView", "Off");
      
      
      #ifdef __ENABLE_GL_PERSPECTIVE__
      if (mPerspectiveModeCheckBox->IsChecked())
         mOpenGlPlot->SetOnOffParameter("PerspectiveMode", "On");
      else
         mOpenGlPlot->SetOnOffParameter("PerspectiveMode", "Off");
      
      if (mUseFixedFovCheckBox->IsChecked())
         mOpenGlPlot->SetOnOffParameter("UseFixedFov", "On");
      else
         mOpenGlPlot->SetOnOffParameter("UseFixedFov", "Off");
      
      Real fov;
      inputString[0] = mFixedFovTextCtrl->GetValue();
      if (!GmatStringUtil::ToReal(inputString[0], &fov) || fov < 1)
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
                 inputString[0].c_str(), "FixedFovAngle","Real Number >= 1");
         return;
      }
      mOpenGlPlot->SetRealParameter("FixedFovAngle", fov);
      #endif

      
      //--------------------------------------------------------------
      // save spacecraft list
      //--------------------------------------------------------------
      if (mHasSpChanged)
      {
         mHasSpChanged = false;
         mHasOrbitColorChanged = true;
         mHasTargetColorChanged = true;
      
         mScCount = mSelectedScListBox->GetCount();
         mNonScCount = mSelectedObjListBox->GetCount();
         
         #if DEBUG_OPENGL_PANEL_SAVE
         MessageInterface::ShowMessage
            ("OpenGlPlotSetupPanel::SaveData() mScCount=%d, mNonScCount=%d\n",
             mScCount, mNonScCount);
         #endif
         
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
      
      //--------------------------------------------------------------
      // save draw object
      //--------------------------------------------------------------
      if (mHasShowObjectChanged)
      {
         mHasShowObjectChanged = false;
         
         // change draw spacecraft
         for (int i=0; i<mScCount; i++)
         {
            mSelSpName = std::string(mSelectedScListBox->GetString(i).c_str());
         
            mOpenGlPlot->
               SetShowObject( mSelSpName, mDrawObjectMap[mSelSpName]);
         }
         
         // change draw non-spacecraft
         for (int i=0; i<mNonScCount; i++)
         {
            mSelSpName = std::string(mSelectedObjListBox->GetString(i).c_str());
         
            mOpenGlPlot->
               SetShowObject(mSelSpName, mDrawObjectMap[mSelSpName]);
         }
      }
      
      //--------------------------------------------------------------
      // save orbit color
      //--------------------------------------------------------------
      if (mHasOrbitColorChanged)
      {
         mHasOrbitColorChanged = false;
         
         // change spacecraft orbit color
         for (int i=0; i<mScCount; i++)
         {
            mSelSpName = std::string(mSelectedScListBox->GetString(i).c_str());
            
            #if DEBUG_OPENGL_PANEL_SAVE
            MessageInterface::ShowMessage
               ("OpenGlPlotSetupPanel::SaveData() objName=%s, orbColor=%u\n",
                mSelSpName.c_str(), mOrbitColorMap[mSelSpName].GetIntColor());
            #endif
            
            mOpenGlPlot->
               SetColor("Orbit", mSelSpName,
                        mOrbitColorMap[mSelSpName].GetIntColor());
         }
         
         // change non-spacecraft orbit color
         for (int i=0; i<mNonScCount; i++)
         {
            mSelSpName = std::string(mSelectedObjListBox->GetString(i).c_str());
         
            mOpenGlPlot->
               SetColor("Orbit", mSelSpName,
                        mOrbitColorMap[mSelSpName].GetIntColor());
         }
      }
      
      //--------------------------------------------------------------
      // save target color
      //--------------------------------------------------------------
      if (mHasTargetColorChanged)
      {
         mHasTargetColorChanged = false;
         
         // change spacecraft target color
         for (int i=0; i<mScCount; i++)
         {
            mSelSpName = std::string(mSelectedScListBox->GetString(i).c_str());
            
            #if DEBUG_OPENGL_PANEL_SAVE
            MessageInterface::ShowMessage
               ("OpenGlPlotSetupPanel::SaveData() objName=%s targetColor=%u\n",
                mSelSpName.c_str(), mTargetColorMap[mSelSpName].GetIntColor());
            #endif
            
            mOpenGlPlot->
               SetColor("Target", mSelSpName,
                        mTargetColorMap[mSelSpName].GetIntColor());
         }
      }
      
      //--------------------------------------------------------------
      // save coordinate system
      //--------------------------------------------------------------
      if (mHasCoordSysChanged)
      {
         mHasCoordSysChanged = false;
         mOpenGlPlot->SetStringParameter
            ("CoordinateSystem",
             std::string(mCoordSysComboBox->GetStringSelection().c_str()));
      }
      
      //--------------------------------------------------------------
      // save ViewPoint info
      //--------------------------------------------------------------
      if (mHasViewInfoChanged)
      {
         mOpenGlPlot->SetStringParameter
            ("ViewPointRef",
             std::string(mViewPointRefComboBox->GetStringSelection().c_str()));
         mOpenGlPlot->SetStringParameter
            ("ViewPointVector",
             std::string(mViewPointVectorComboBox->GetStringSelection().c_str()));
         mOpenGlPlot->SetStringParameter
            ("ViewDirection",
             std::string(mViewDirectionComboBox->GetStringSelection().c_str()));
         
         Real realVal[3];
         inputString[0] = mViewScaleFactorTextCtrl->GetValue();
         if (!GmatStringUtil::ToReal(inputString[0], &realVal[0]) || 
             realVal[0] < 0)
         {
            MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
              inputString[0].c_str(), "ViewScaleFactor","Real Number >= 0");
            return;
         }
         mOpenGlPlot->SetRealParameter("ViewScaleFactor", realVal[0]);

         // save ViewPoint ref. vector
         if (mViewPointRefComboBox->GetStringSelection() == "Vector")
         {
            Rvector vec(3);
            inputString[0] = mViewPointRef1TextCtrl->GetValue();
            inputString[1] = mViewPointRef2TextCtrl->GetValue();
            inputString[2] = mViewPointRef3TextCtrl->GetValue();
            for (Integer i=0; i < 3; ++i)
            {
               if (!GmatStringUtil::ToReal(inputString[i], &realVal[i]))
               {
                  MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
                          inputString[i].c_str(), "ViewPointRefVector",
                          "Real Number");
                  return;
               }
               vec(i) = realVal[i];
            }
            mOpenGlPlot->SetRvectorParameter("ViewPointRefVector", vec);
         }
         
         // save ViewPoint vector
         if (mViewPointVectorComboBox->GetStringSelection() == "Vector")
         {
            Rvector vec(3);
            inputString[0] = mViewPointVec1TextCtrl->GetValue();
            inputString[1] = mViewPointVec2TextCtrl->GetValue();
            inputString[2] = mViewPointVec3TextCtrl->GetValue();
            for (Integer i=0; i < 3; ++i)
            {
               if (!GmatStringUtil::ToReal(inputString[i], &realVal[i]))
               {
                  MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
                          inputString[i].c_str(), "ViewPointVectorVector",
                          "Real Number");
                  return;
               }
               vec(i) = realVal[i];
            }
            mOpenGlPlot->SetRvectorParameter("ViewPointVectorVector", vec);
         }

         // save View direction
         if (mViewDirectionComboBox->GetStringSelection() == "Vector")
         {
            Rvector vec(3);
            inputString[0] = mViewDir1TextCtrl->GetValue();
            inputString[1] = mViewDir2TextCtrl->GetValue();
            inputString[2] = mViewDir3TextCtrl->GetValue();

            for (Integer i=0; i < 3; ++i)
            {
               if (!GmatStringUtil::ToReal(inputString[i], &realVal[i]))
               {
                  MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
                          inputString[i].c_str(), "ViewDirectionVector",
                          "Real Number");
                  return;
               }
               vec(i) = realVal[i];
            }
            mOpenGlPlot->SetRvectorParameter("ViewDirectionVector", vec);
         }
         
         mHasViewInfoChanged = false;
      } // end if ( mHasViewInfoChanged)
      
      //--------------------------------------------------------------
      // save view up direction info
      //--------------------------------------------------------------
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
      
      //--------------------------------------------------------------
      // Just warning message
      //--------------------------------------------------------------
      // Check if Sun was added to draw Sun line
      if (mOriginSunLineCheckBox->IsChecked() &&
          mSelectedObjListBox->FindString("Sun") == wxNOT_FOUND)
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "\"Sun\" needs to be added to the view object list "
             "to draw Sun line");
      
      EnableUpdate(false);
      canClose = true;
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("OpenGlPlotSetupPanel:SaveData() error occurred!\n%s\n",
          e.GetFullMessage().c_str());
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
         
         mDrawObjectMap[s.c_str()] = true;
         ShowSpacePointOption(s, true, true, GmatColor::RED32);
         mHasSpChanged = true;
         EnableUpdate(true);
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
         
         mDrawObjectMap[s.c_str()] = true;
         ShowSpacePointOption(s, true, false, GmatColor::L_BROWN32);
         mHasSpChanged = true;
         EnableUpdate(true);
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
   EnableUpdate(true);
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
   EnableUpdate(true);
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
   #ifdef __ENABLE_GL_PERSPECTIVE__
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
   #endif
   
   
   if (event.GetEventObject() == mDrawObjectCheckBox)
   {
      if (mSelectedScListBox->GetSelection() != -1)
      {
         mSelSpName = std::string(mSelectedScListBox->GetStringSelection().c_str());
         mDrawObjectMap[mSelSpName] = mDrawObjectCheckBox->GetValue();
         mHasShowObjectChanged = true;
      }
      else if (mSelectedObjListBox->GetSelection() != -1)
      {
         mSelSpName = std::string(mSelectedObjListBox->GetStringSelection().c_str());
         mDrawObjectMap[mSelSpName] = mDrawObjectCheckBox->GetValue();
         mHasShowObjectChanged = true;
      }
      
      #if DEBUG_OPENGL_PANEL_CHECKBOX
      MessageInterface::ShowMessage
         ("OpenGlPlotSetupPanel::OnCheckBoxChange() mSelSpName=%s, show=%d\n",
          mSelSpName.c_str(), mDrawObjectMap[mSelSpName]);
      #endif
   }
   
   EnableUpdate(true);
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
         mOrbitColorButton->Refresh();
         mOrbitColorMap[mSelSpName].Set(mOrbitColor.Red(),
                                        mOrbitColor.Green(),
                                        mOrbitColor.Blue());

         #if DEBUG_OPENGL_PANEL
         MessageInterface::ShowMessage("OnOrbitColorClick() r=%d g=%d b=%d\n",
                                       mOrbitColor.Red(), mOrbitColor.Green(),
                                       mOrbitColor.Blue());
         
         MessageInterface::ShowMessage
            ("OnOrbitColorClick() mOrbitColorMap[%s]=%u\n",
             mSelSpName.c_str(), mOrbitColorMap[mSelSpName].GetIntColor());
         #endif
      }
      else if (mSelectedObjListBox->GetSelection() != -1)
      {
         mSelSpName = std::string(mSelectedObjListBox->GetStringSelection().c_str());
      
         mOrbitColor = dialog.GetColourData().GetColour();
         mOrbitColorButton->SetBackgroundColour(mOrbitColor);
         mOrbitColorButton->Refresh();

         mOrbitColorMap[mSelSpName].Set(mOrbitColor.Red(),
                                        mOrbitColor.Green(),
                                        mOrbitColor.Blue());
         
         #if DEBUG_OPENGL_PANEL
         MessageInterface::ShowMessage
            ("OnOrbitColorClick() r=%d g=%d b=%d\n", mOrbitColor.Red(),
             mOrbitColor.Green(), mOrbitColor.Blue());

         MessageInterface::ShowMessage
            ("OnOrbitColorClick() mOrbitColorMap[%s]=%u\n",
             mSelSpName.c_str(), mOrbitColorMap[mSelSpName].GetIntColor());
         #endif
      }
      
      EnableUpdate(true);
      mHasOrbitColorChanged = true;
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
      mSelSpName = std::string(mSelectedScListBox->GetStringSelection().c_str());
      
      mTargetColor = dialog.GetColourData().GetColour();
      mTargetColorButton->SetBackgroundColour(mTargetColor);
      mTargetColorButton->Refresh();
      mTargetColorMap[mSelSpName].Set(mTargetColor.Red(),
                                      mTargetColor.Green(),
                                      mTargetColor.Blue());
      
      #if DEBUG_OPENGL_PANEL
      MessageInterface::ShowMessage
         ("OnTargetColorClick() r=%d g=%d b=%d\n", mTargetColor.Red(),
          mTargetColor.Green(), mTargetColor.Blue());
      
      MessageInterface::ShowMessage
         ("OnTargetColorClick() mTargetColorMap[%s]=%u\n",
          mSelSpName.c_str(), mTargetColorMap[mSelSpName].GetIntColor());
      #endif
      
      EnableUpdate(true);
      mHasTargetColorChanged = true;
   }
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnComboBoxChange(wxCommandEvent& event)
{    
   if (event.GetEventObject() == mCoordSysComboBox)
   {
      // if coordinate system is other than EarthMJ2000Eq,
      // uncheck and disable draw ecliptic plane CheckBox
      if (mCoordSysComboBox->GetValue() == "EarthMJ2000Eq")
      {
         mEclipticPlaneCheckBox->Enable();
      }
      else
      {
         mEclipticPlaneCheckBox->SetValue(false);
         mEclipticPlaneCheckBox->Disable();
      }
      
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
      
      //mBottomViewSizer->Layout();
   }
   else if (event.GetEventObject() == mViewPointVectorComboBox)
   {
      mHasViewInfoChanged = true;
      
      if (mViewPointVectorComboBox->GetStringSelection() == "Vector")
         mViewDefSizer->Show(mViewPointVectorSizer, true);
      else
         mViewDefSizer->Show(mViewPointVectorSizer, false);
      
      //mBottomViewSizer->Layout();      
   }
   else if (event.GetEventObject() == mViewDirectionComboBox)
   {
      mHasViewInfoChanged = true;
      
      if (mViewDirectionComboBox->GetStringSelection() == "Vector")
         mViewDefSizer->Show(mViewDirVectorSizer, true);
      else
         mViewDefSizer->Show(mViewDirVectorSizer, false);
      
      //mBottomViewSizer->Layout();      
   }
   
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnTextChange(wxCommandEvent& event)
{
   mHasViewInfoChanged = true;
   EnableUpdate(true);
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
   #if DEBUG_OPENGL_PANEL_SHOW
   MessageInterface::ShowMessage
      ("OpenGlPlotSetupPanel::ShowSpacePointOption() name=%s, show=%d, isSc=%d, "
       "color=%u\n", name.c_str(), show, isSc, color);
   #endif
   
   if (!name.IsSameAs(""))
   {
      mSelSpName = std::string(name.c_str());
      
      // if object name not found, insert
      if (mOrbitColorMap.find(mSelSpName) == mOrbitColorMap.end())
      {
         #if DEBUG_OPENGL_PANEL
         MessageInterface::ShowMessage
            ("ShowSpacePointOption() name not found, so adding it to color map\n");
         #endif
         
         mOrbitColorMap[mSelSpName] = RgbColor(color);
         mTargetColorMap[mSelSpName] = RgbColor(GmatColor::ORANGE32);
      }
      
      RgbColor orbColor = mOrbitColorMap[mSelSpName];
      RgbColor targColor = mTargetColorMap[mSelSpName];
      
      #if DEBUG_OPENGL_PANEL_SHOW
      MessageInterface::ShowMessage
         ("OpenGlPlotSetupPanel::ShowSpacePointOption() orbColor=%u, targColor=%u\n",
          orbColor.GetIntColor(), targColor.GetIntColor());
      #endif
      
      mDrawObjectCheckBox->SetValue(mDrawObjectMap[mSelSpName]);
      
      mOrbitColor.Set(orbColor.Red(), orbColor.Green(), orbColor.Blue());
      mTargetColor.Set(targColor.Red(), targColor.Green(), targColor.Blue());
      
      mOrbitColorButton->SetBackgroundColour(mOrbitColor);
      mTargetColorButton->SetBackgroundColour(mTargetColor);
      mOrbitColorButton->Refresh();
      mTargetColorButton->Refresh();
      
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
      
      mObjectSizer->Show(mScOptionSizer, show);
   }
   else
   {
      mObjectSizer->Show(mScOptionSizer, false);
   }
}

