//$Header$
//------------------------------------------------------------------------------
//                              OpenGlOptionDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/12/20
//
/**
 * Implements OpenGlOptionDialog class. This dialog allows user to view and
 * select various OpenGL plot options.
 */
//------------------------------------------------------------------------------

#include "OpenGlOptionDialog.hpp"
#include "MdiChildTrajFrame.hpp"
#include "CelesBodySelectDialog.hpp"

#include "ColorTypes.hpp"           // for GmatColor::
#include "RgbColor.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"

#include "wx/colordlg.h"            // for wxColourDialog

//#define DEBUG_GLOPTION_CREATE 1
//#define DEBUG_GLOPTION_LOAD 1
//#define DEBUG_GLOPTION 2
//#define DEBUG_GLOPTION_OBJECT 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(OpenGlOptionDialog, wxDialog)
   EVT_TEXT(ID_TEXTCTRL, OpenGlOptionDialog::OnTextChange)
   EVT_CHECKBOX(ID_CHECKBOX, OpenGlOptionDialog::OnCheckBoxChange)
   EVT_COMBOBOX(ID_COMBOBOX, OpenGlOptionDialog::OnComboBoxChange)
   EVT_LISTBOX(ID_LISTBOX, OpenGlOptionDialog::OnSelectObject)
   EVT_BUTTON(ID_BUTTON, OpenGlOptionDialog::OnButtonClick)
   EVT_BUTTON(ID_BUTTON_APPLY, OpenGlOptionDialog::OnApplyButtonClick)
   EVT_BUTTON(ID_EQPLANE_COLOR_BUTTON, OpenGlOptionDialog::OnColorButtonClick)
   EVT_BUTTON(ID_ECPLANE_COLOR_BUTTON, OpenGlOptionDialog::OnColorButtonClick)
   EVT_BUTTON(ID_SUNLINE_COLOR_BUTTON, OpenGlOptionDialog::OnColorButtonClick)
   EVT_BUTTON(ID_OBJECT_COLOR_BUTTON, OpenGlOptionDialog::OnObjectColorButtonClick)
//    EVT_BUTTON(ID_ADD_BODY_BUTTON, OpenGlOptionDialog::OnAddBodyButtonClick)

   EVT_CLOSE(OpenGlOptionDialog::OnClose)
END_EVENT_TABLE()

using namespace GmatPlot;

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// OpenGlOptionDialog(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs OpenGlOptionDialog object.
 *
 * @param <parent> parent window
 *
 */
//------------------------------------------------------------------------------
OpenGlOptionDialog::OpenGlOptionDialog(wxWindow *parent, const wxString &title,
                                       const wxArrayString &objectNames,
                                       const UnsignedIntArray &objectColors)
   : wxDialog(parent, -1, title)
{
   theGuiManager = GuiItemManager::GetInstance();
   mTrajFrame = (MdiChildTrajFrame*)parent;
   
   mHasRotateAboutXYChanged = false;
   mHasUseViewPointSpecChanged = false;
   mHasUsePerspModeChanged = false;
   mHasDistanceChanged = false;
   mHasGotoObjectChanged = false;
   mHasCoordSysChanged = false;
   
   mHasDrawWireFrameChanged = false;
   mHasDrawXyPlaneChanged = false;
   mHasDrawAxesChanged = false;
   mHasDrawEcPlaneChanged = false;
   mHasDrawESLineChanged = false;
   
   mHasXyPlaneColorChanged = false;
   mHasEcPlaneColorChanged = false;
   mHasESLineColorChanged = false;
   mHasObjectColorChanged = false;
   
   mHasShowObjectChanged = false;
   mHasShowOrbitNormalChanged = false;

   mDistance = 30000;
   mGotoObjectName = "";
   
   mObjectCount = objectNames.GetCount();
   
   mObjectColorMap.clear();
   mShowObjectMap.clear();
   mShowOrbitNormalMap.clear();
   
   for (int i=0; i<mObjectCount; i++)
   {
      mObjectNames.Add(objectNames[i]);
      mObjectIntColors.push_back(objectColors[i]);

      mObjectColorMap[objectNames[i]] = RgbColor(objectColors[i]);
      mShowObjectMap[objectNames[i]] = true;
      mShowOrbitNormalMap[objectNames[i]] = false;
      
      #ifdef DEBUG_GLOPTION
      MessageInterface::ShowMessage
         ("OpenGlOptionDialog() object=%s, color=%d\n",
          mObjectNames[i].c_str(), mObjectIntColors[i]);
      #endif
   }
   
   mXyPlaneColor = wxColor("GREY");
   mEcPlaneColor = wxColor("DARK SLATE BLUE");
   mESLinesColor = wxColor("BROWN");
   
   Create();
   ShowData();
}


//------------------------------------------------------------------------------
// ~OpenGlOptionDialog()
//------------------------------------------------------------------------------
OpenGlOptionDialog::~OpenGlOptionDialog()
{
   theGuiManager->UnregisterComboBox("CoordinateSystem", mCoordSysComboBox);
}


//------------------------------------------------------------------------------
// void SetDistance(float dist)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::SetDistance(float dist)
{
   wxString strVal;
   mDistance = dist;
   strVal.Printf("%g", mDistance);
   mDistanceTextCtrl->SetValue(strVal);
   theApplyButton->Disable(); //loj: 5/23/05 We don't want to enable it here.
}


//------------------------------------------------------------------------------
// void SetDrawXyPlane(bool flag)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::SetDrawXyPlane(bool flag)
{
   mXyPlaneCheckBox->SetValue(flag);
}


//------------------------------------------------------------------------------
// void SetDrawWireFrame(bool flag)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::SetDrawWireFrame(bool flag)
{
   mWireFrameCheckBox->SetValue(flag);
}


//------------------------------------------------------------------------------
// void SetDrawAxes(bool flag)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::SetDrawAxes(bool flag)
{
   mAxesCheckBox->SetValue(flag);
}


//------------------------------------------------------------------------------
// void SetDrawEarthSunLines(bool flag)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::SetDrawEarthSunLines(bool flag)
{
   mESLinesCheckBox->SetValue(flag);
}


//------------------------------------------------------------------------------
// void SetGotoObjectName(const wxString &objName)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::SetGotoObjectName(const wxString &objName)
{
   mGotoObjectComboBox->SetStringSelection(objName);
   mGotoObjectName = objName;
   
   // We don't want to enable this
   theApplyButton->Disable();
}


//------------------------------------------------------------------------------
// void SetCoordSysName(const wxString &csName)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::SetCoordSysName(const wxString &csName)
{
   mCoordSysComboBox->SetStringSelection(csName);
   mCoordSysName = csName;
   
   // We don't want to enable this
   theApplyButton->Disable();
}


//------------------------------------------------------------------------------
// void UpdateObjectList(const wxArrayString &objNames, ...
//------------------------------------------------------------------------------
void OpenGlOptionDialog::UpdateObjectList(const wxArrayString &objNames,
                                          const wxArrayString &validCSNames,
                                          const wxStringBoolMap &showObjects,
                                          const wxStringColorMap &objColors)
{
   #ifdef DEBUG_GLOPTION_OBJECT
   MessageInterface::ShowMessage
      ("OpenGlOptionDialog::UpdateObjectList() mObjectNames.size=%d, "
       "mObjectIntColors.size=%d\n", mObjectNames.GetCount(), mObjectIntColors.size());
   #endif
   
   mObjectNames = objNames;
   mObjectCount = objNames.GetCount();
   mValidCSNames = validCSNames;
   mValidCSCount = validCSNames.GetCount();
   mInitialShowObjectMap = showObjects;
   mShowObjectMap = showObjects;
   mObjectColorMap = objColors;
   mObjectIntColors.clear();
   int intColor;
   
   for (int i=0; i<mObjectCount; i++)
   {
      intColor = mObjectColorMap[mObjectNames[i]].GetIntColor();
      mObjectIntColors.push_back(intColor);
      
      #ifdef DEBUG_GLOPTION_OBJECT
      MessageInterface::ShowMessage
         ("OpenGlOptionDialog::UpdateObjectList() object=%s, color=%d\n",
          mObjectNames[i].c_str(), mObjectIntColors[i]);
      #endif
   }

   UpdateCoordSysComboBox();
   UpdateObjectComboBox();
   UpdateObjectListBox();
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates GUI compoments.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::Create()
{
   #if DEBUG_GLOPTION_CREATE
   MessageInterface::ShowMessage("OpenGlOptionDialog::Create() entered.\n");
   #endif
   int borderSize = 2;
   
   // emptyStaticText
   wxStaticText *emptyStaticText =
      new wxStaticText( this, -1, wxT("  "), wxDefaultPosition, wxDefaultSize, 0 );
   
   //-----------------------------------------------------------------
   // animation
   //-----------------------------------------------------------------
   
   mUseInitialViewDefCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Use Initial View Definition"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   wxStaticText *animationStaticText =
      new wxStaticText(this, -1, wxT("Update Interval (msec)\n<Esc> to interrupt"),
                       wxDefaultPosition, wxSize(-1, -1), 0);
   
   mAnimationUpdIntTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(60, -1), 0);
   
   mViewAnimationButton =
      new wxButton(this, ID_BUTTON, "View Animation", wxDefaultPosition,
                   wxSize(-1, -1), 0);
   
   wxBoxSizer *updateSizer = new wxBoxSizer(wxHORIZONTAL);
   updateSizer->Add(animationStaticText, 0, wxALIGN_LEFT|wxALL, borderSize);
   updateSizer->Add(mAnimationUpdIntTextCtrl, 0, wxALIGN_LEFT|wxALL, borderSize);
   
   wxBoxSizer *animationBoxSizer = new wxBoxSizer(wxVERTICAL);
   animationBoxSizer->Add(mUseInitialViewDefCheckBox, 0, wxALIGN_LEFT|wxALL, borderSize);
   animationBoxSizer->Add(updateSizer, 0, wxALIGN_LEFT|wxALL, borderSize);
   animationBoxSizer->Add(mViewAnimationButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   
   wxStaticBox *animationStaticBox =
      new wxStaticBox(this, -1, wxT("View Animation"));
   
   wxStaticBoxSizer *animationSizer
      = new wxStaticBoxSizer(animationStaticBox, wxVERTICAL);
   
   animationSizer->Add(animationBoxSizer, 0, wxALIGN_LEFT|wxALL, borderSize);
   
   #if DEBUG_GLOPTION_CREATE
   MessageInterface::ShowMessage
      ("OpenGlOptionDialog::Create() animationSizer created.\n");
   #endif
   
   //-----------------------------------------------------------------
   // view mode
   //-----------------------------------------------------------------
   mUsePerspModeCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Perspective Mode"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   //-----------------------------------------------------------------
   // view option
   //-----------------------------------------------------------------
   wxStaticText *distanceStaticText =
      new wxStaticText(this, -1, wxT("Distance (Km)"),
                       wxDefaultPosition, wxSize(-1, -1), 0);
   mDistanceTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(105, -1), 0);
   wxStaticText *centerOfViewStaticText =
      new wxStaticText(this, -1, wxT("Go To"),
                       wxDefaultPosition, wxSize(-1, -1), 0);
   //VC++ error C2057: expected constant expression 
   //wxString strObjectArray[mObjectCount];
   wxArrayString strObjectArray;
   for (int i=0; i<mObjectCount; i++)
      //strObjectArray[i] = mObjectNames[i];
      strObjectArray.Add(mObjectNames[i]);
   
   mGotoObjectComboBox = new wxComboBox
      (this, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxSize(105,-1), //mObjectCount,
       strObjectArray, wxCB_DROPDOWN);
   
   mGotoObjectComboBox->SetStringSelection("Earth");
   
   wxStaticText *coordSysStaticText =
      new wxStaticText(this, -1, wxT("Coord System"),
                       wxDefaultPosition, wxSize(-1, -1), 0);
   mCoordSysComboBox =
      theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX, wxSize(105, -1));
   
   wxFlexGridSizer *viewGridSizer = new wxFlexGridSizer(2, 0, 0);
   viewGridSizer->Add(distanceStaticText, 0, wxALIGN_LEFT|wxALL, borderSize);
   viewGridSizer->Add(mDistanceTextCtrl, 0, wxALIGN_LEFT|wxALL, borderSize);
   viewGridSizer->Add(centerOfViewStaticText, 0, wxALIGN_LEFT|wxALL, borderSize);
   viewGridSizer->Add(mGotoObjectComboBox, 0, wxALIGN_LEFT|wxALL, borderSize);
   viewGridSizer->Add(coordSysStaticText, 0, wxALIGN_LEFT|wxALL, borderSize);
   viewGridSizer->Add(mCoordSysComboBox, 0, wxALIGN_LEFT|wxALL, borderSize);
   
   #if DEBUG_GLOPTION_CREATE
   MessageInterface::ShowMessage("OpenGlOptionDialog::Create() viewGridSizer created.\n");
   #endif
   
   wxStaticBox *viewOptionStaticBox =
      new wxStaticBox(this, -1, wxT("View Options"));
   
   wxStaticBoxSizer *viewOptionSizer
      = new wxStaticBoxSizer(viewOptionStaticBox, wxVERTICAL);
   
   viewOptionSizer->Add(mUsePerspModeCheckBox, 0, wxALIGN_LEFT|wxALL, borderSize);
   viewOptionSizer->Add(viewGridSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   #if DEBUG_GLOPTION_CREATE
   MessageInterface::ShowMessage
      ("OpenGlOptionDialog::Create() viewOptionSizer created.\n");
   #endif
   
   //-----------------------------------------------------------------
   // drawing option
   //-----------------------------------------------------------------
   mRotateAboutXYCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Rotate XY"),
                     wxDefaultPosition, wxSize(150, -1), 0);

   mWireFrameCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Draw Wire Frame"),
                     wxDefaultPosition, wxSize(150, -1), 0);
   
   mXyPlaneCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Draw XY Plane"),
                     wxDefaultPosition, wxSize(150, -1), 0);
   
   mEcPlaneCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Draw Ecliptic Plane"),
                     wxDefaultPosition, wxSize(150, -1), 0);
   
   mAxesCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Draw Axes"),
                     wxDefaultPosition, wxSize(150, -1), 0);

   mESLinesCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Draw Earth Sun Lines"),
                     wxDefaultPosition, wxSize(150, -1), 0);

   // equatorial plane color
   mXyPlaneColorButton =
      new wxButton(this, ID_EQPLANE_COLOR_BUTTON, "", wxDefaultPosition,
                   wxSize(20, 15), 0);
   mXyPlaneColorButton->SetBackgroundColour(mXyPlaneColor);
   
   // ecliptic plane color
   mEcPlaneColorButton =
      new wxButton(this, ID_ECPLANE_COLOR_BUTTON, "", wxDefaultPosition,
                   wxSize(20, 15), 0);
   mEcPlaneColorButton->SetBackgroundColour(mEcPlaneColor);

   // Sun line color
   mESLinesColorButton =
      new wxButton(this, ID_SUNLINE_COLOR_BUTTON, "", wxDefaultPosition,
                   wxSize(20, 15), 0);
   mESLinesColorButton->SetBackgroundColour(mESLinesColor);

   wxStaticBox *drawingOptionStaticBox =
      new wxStaticBox(this, -1, wxT("Drawing Options"));
   
   wxStaticBoxSizer *drawingOptionSizer
      = new wxStaticBoxSizer(drawingOptionStaticBox, wxVERTICAL);
   
   borderSize = 2;
   wxFlexGridSizer *drawGridSizer = new wxFlexGridSizer(2, 0, 0);
   drawGridSizer->Add(mRotateAboutXYCheckBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mWireFrameCheckBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mAxesCheckBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mXyPlaneCheckBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mXyPlaneColorButton, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mEcPlaneCheckBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mEcPlaneColorButton, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mESLinesCheckBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mESLinesColorButton, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   #if DEBUG_GLOPTION_CREATE
   MessageInterface::ShowMessage
      ("OpenGlOptionDialog::Create() drawGridSizer created.\n");
   #endif
   
   drawingOptionSizer->Add(drawGridSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   //-----------------------------------------------------------------
   // view bodies options
   //-----------------------------------------------------------------

   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;

   mObjectListBox =
      new wxListBox(this, ID_LISTBOX, wxDefaultPosition, wxSize(75,60), //0,
                    emptyList, wxLB_SINGLE);
   
   mObjectColorButton = new wxButton(this, ID_OBJECT_COLOR_BUTTON, "",
                                    wxDefaultPosition, wxSize(20,15), 0);
   mShowObjectCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Show"),
                     wxDefaultPosition, wxSize(90, -1), 0);
   
   mShowOrbitNormalCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Draw Orb Norm"),
                     wxDefaultPosition, wxSize(90, -1), 0);
   
   wxBoxSizer *colorSizer = new wxBoxSizer(wxVERTICAL);
   colorSizer->Add(mObjectColorButton, 0, wxALIGN_LEFT|wxALL, borderSize);
   colorSizer->Add(mShowObjectCheckBox, 0, wxALIGN_LEFT|wxALL, borderSize);
   colorSizer->Add(mShowOrbitNormalCheckBox, 0, wxALIGN_LEFT|wxALL, borderSize);
   
   RgbColor rgbcolor;
   
   for (int i=0; i<mObjectCount; i++)
   {
      mObjectListBox->Append(mObjectNames[i]);
   }
   
   wxBoxSizer *objectSizer = new wxBoxSizer(wxHORIZONTAL);
   objectSizer->Add(mObjectListBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   objectSizer->Add(colorSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   wxStaticBox *viewObjectStaticBox =
      new wxStaticBox(this, -1, wxT("View Object"));

   //-----------------------------------------------------------------
   // create object sizers
   //-----------------------------------------------------------------
   mViewObjectSizer = new wxStaticBoxSizer(viewObjectStaticBox, wxVERTICAL);

   mViewObjectSizer->Add(objectSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);   
   
   //-----------------------------------------------------------------
   // create page sizers
   //-----------------------------------------------------------------
   wxBoxSizer *topViewSizer = new wxBoxSizer(wxVERTICAL);
   topViewSizer->Add(animationSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   topViewSizer->Add(viewOptionSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   topViewSizer->Add(drawingOptionSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);

   wxFlexGridSizer *pageSizer = new wxFlexGridSizer(1, 0, 0);

   pageSizer->Add(topViewSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   pageSizer->Add(mViewObjectSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   //------------------------------------------------------
   // create dialog sizer
   //------------------------------------------------------   
   theDialogSizer = new wxBoxSizer(wxVERTICAL);
   
   theApplyButton =
      new wxButton(this, ID_BUTTON_APPLY, "Apply", wxDefaultPosition, wxDefaultSize, 0);
   
   theDialogSizer->Add(pageSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   theDialogSizer->Add(theApplyButton, 0, wxALIGN_CENTRE|wxALL, 5);
   
   #if DEBUG_GLOPTION_CREATE
   MessageInterface::ShowMessage("OpenGlOptionDialog::Create() exiting.\n");
   #endif
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Loades data into GUI compoments.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::LoadData()
{
   #if DEBUG_GLOPTION_LOAD
   MessageInterface::ShowMessage("OpenGlOptionDialog::LoadData() entered.\n");
   #endif

   wxString strVal;
   
   // view mode
   mUseInitialViewDefCheckBox->SetValue(mTrajFrame->GetUseViewPointInfo());
   mUsePerspModeCheckBox->SetValue(mTrajFrame->GetUsePerspectiveMode());

   #if DEBUG_GLOPTION_LOAD
   MessageInterface::ShowMessage
      ("OpenGlOptionDialog::LoadData() setting mAnimationUpdIntTextCtrl.\n");
   #endif
   
   // animiation
   strVal.Printf("%d", mTrajFrame->GetAnimationUpdateInterval());
   mAnimationUpdIntTextCtrl->SetValue(strVal);
   
   #if DEBUG_GLOPTION_LOAD
   MessageInterface::ShowMessage
      ("OpenGlOptionDialog::LoadData() setting mDistanceTextCtrl.\n");
   #endif
   
   // distance
   mDistance = mTrajFrame->GetDistance();
   strVal.Printf("%g", mDistance);
   mDistanceTextCtrl->SetValue(strVal);
   
   #if DEBUG_GLOPTION_LOAD
   MessageInterface::ShowMessage
      ("OpenGlOptionDialog::LoadData() setting mGotoObjectComboBox.\n");
   #endif
   
   // goto object
   mGotoObjectComboBox->
      SetStringSelection(mTrajFrame->GetGotoObjectName());
      
   #if DEBUG_GLOPTION_LOAD
   MessageInterface::ShowMessage
      ("OpenGlOptionDialog::LoadData() setting mCoordSysComboBox.\n");
   #endif
   
   // coordinate system
   mCoordSysComboBox->SetStringSelection(mTrajFrame->GetViewCoordSysName());
   
   // equatorial plane, eclitic plane, Earth-Sun line
   mXyPlaneCheckBox->SetValue(mTrajFrame->GetDrawXyPlane());
   RgbColor rgb(mTrajFrame->GetXyPlaneColor());
   mXyPlaneColor.Set(rgb.Red(), rgb.Green(), rgb.Blue());
   mXyPlaneColorButton->SetBackgroundColour(mXyPlaneColor);
   
   mEcPlaneCheckBox->SetValue(mTrajFrame->GetDrawEcPlane());
   rgb.Set(mTrajFrame->GetEcPlaneColor());
   mEcPlaneColor.Set(rgb.Red(), rgb.Green(), rgb.Blue());
   mEcPlaneColorButton->SetBackgroundColour(mEcPlaneColor);
   
   mESLinesCheckBox->SetValue(mTrajFrame->GetDrawESLines());
   rgb.Set(mTrajFrame->GetESLineColor());
   mESLinesColor.Set(rgb.Red(), rgb.Green(), rgb.Blue());
   mESLinesColorButton->SetBackgroundColour(mESLinesColor);
   
   // wire frame, axes, rotation axis
   mWireFrameCheckBox->SetValue(mTrajFrame->GetDrawWireFrame());
   mRotateAboutXYCheckBox->SetValue(mTrajFrame->GetRotateAboutXY());
   mAxesCheckBox->SetValue(mTrajFrame->GetDrawAxes()); //loj: 7/27/05 Added
   
   // view object
   mObjectListBox->SetSelection(0);
   ShowSpacePointOption(mObjectListBox->GetStringSelection());

   mEcPlaneCheckBox->Enable();
   mEcPlaneColorButton->Enable();
   mUsePerspModeCheckBox->Enable();
   
   #if DEBUG_GLOPTION_LOAD
   MessageInterface::ShowMessage("OpenGlOptionDialog::LoadData() exiting.\n");
   #endif
}


//------------------------------------------------------------------------------
// void ShowData()
//------------------------------------------------------------------------------
/**
 * Loades data into GUI compoments.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::ShowData()
{
   // tells the enclosing window to adjust to the size of the sizer
   SetAutoLayout(TRUE);
   SetSizer(theDialogSizer);
   theDialogSizer->Fit(this);
   theDialogSizer->SetSizeHints(this);

   CenterOnScreen(wxBOTH);
   
   LoadData();

   theApplyButton->Disable();
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves data from GUI compoments.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::SaveData()
{

   if (mHasRotateAboutXYChanged)
   {
      mHasRotateAboutXYChanged = false;
      if (mRotateAboutXYCheckBox->GetValue())
         mTrajFrame->SetRotateAboutXY(true);
      else
         mTrajFrame->SetRotateAboutXY(false);
   }
   
   if (mHasUseViewPointSpecChanged)
   {
      mHasUseViewPointSpecChanged = false;
      mTrajFrame->SetUseInitialViewDef(mUseInitialViewDefCheckBox->GetValue());
   }
   
   if (mHasUsePerspModeChanged)
   {
      mHasUsePerspModeChanged = false;
      mTrajFrame->SetUsePerspectiveMode(mUsePerspModeCheckBox->GetValue());
   }
   
   if (mHasDistanceChanged)
   {
      mHasDistanceChanged = false;
      mTrajFrame->SetDistance(mDistance);
   }
   
   if (mHasCoordSysChanged)
   {
      mHasCoordSysChanged = false;
      mTrajFrame->DrawInOtherCoordSystem(mCoordSysName);
   }
   
   if (mHasGotoObjectChanged)
   {
      mHasGotoObjectChanged = false;
      mTrajFrame->SetGotoObjectName(mGotoObjectName);
   }
   
   if (mHasDrawXyPlaneChanged)
   {
      mHasDrawXyPlaneChanged = false;
      mTrajFrame->SetDrawXyPlane(mXyPlaneCheckBox->GetValue());
   }
   
   if (mHasDrawEcPlaneChanged)
   {
      mHasDrawEcPlaneChanged = false;
      mTrajFrame->SetDrawEcPlane(mEcPlaneCheckBox->GetValue());
   }
   
   if (mHasDrawESLineChanged)
   {
      mHasDrawESLineChanged = false;
      mTrajFrame->SetDrawESLines(mESLinesCheckBox->GetValue());
   }
   
   if (mHasDrawWireFrameChanged)
   {
      mHasDrawWireFrameChanged = false;
      mTrajFrame->SetDrawWireFrame(mWireFrameCheckBox->GetValue());
   }
   
   if (mHasDrawAxesChanged)
   {
      mHasDrawAxesChanged = false;
      mTrajFrame->SetDrawAxes(mAxesCheckBox->GetValue());
   }
   
   if (mHasXyPlaneColorChanged)
   {
      mHasXyPlaneColorChanged = false;
      RgbColor rgb(mXyPlaneColor.Red(), mXyPlaneColor.Green(), mXyPlaneColor.Blue());
      mTrajFrame->SetXyPlaneColor(rgb.GetIntColor());      
   }
   
   if (mHasEcPlaneColorChanged)
   {
      mHasEcPlaneColorChanged = false;
      RgbColor rgb(mEcPlaneColor.Red(), mEcPlaneColor.Green(), mEcPlaneColor.Blue());
      mTrajFrame->SetEcPlaneColor(rgb.GetIntColor());      
   }
   
   if (mHasESLineColorChanged)
   {
      mHasESLineColorChanged = false;
      RgbColor rgb(mESLinesColor.Red(), mESLinesColor.Green(), mESLinesColor.Blue());
      mTrajFrame->SetESLineColor(rgb.GetIntColor());      
   }
   
   if (mHasShowObjectChanged)
   {
      mHasShowObjectChanged = false;
      mTrajFrame->SetShowObjects(mShowObjectMap);    
   }
   
   if (mHasShowOrbitNormalChanged)
   {
      mHasShowOrbitNormalChanged = false;
      mTrajFrame->SetShowOrbitNormals(mShowOrbitNormalMap);    
   }
   
   if (mHasObjectColorChanged)
   {
      mHasObjectColorChanged = false;
      RgbColor rgb(mESLinesColor.Red(), mESLinesColor.Green(), mESLinesColor.Blue());
      mTrajFrame->SetObjectColors(mObjectColorMap);      
   }
   
   mTrajFrame->RedrawPlot(false);

   theApplyButton->Disable();
}


//------------------------------------------------------------------------------
// void ResetData()
//------------------------------------------------------------------------------
/**
 * Saves data from GUI compoments.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::ResetData()
{
}


//------------------------------------------------------------------------------
// void UpdateCoordSysComboBox()
//------------------------------------------------------------------------------
void OpenGlOptionDialog::UpdateCoordSysComboBox()
{
   #ifdef DEBUG_GLOPTION_OBJECT
   MessageInterface::ShowMessage
      ("OpenGlOptionDialog::UpdateCoordSysComboBox() mObjectCount=%d\n", mObjectCount);
   #endif
   
   mCoordSysComboBox->Clear();
   
   for (int i=0; i<mValidCSCount; i++)
      mCoordSysComboBox->Append(mValidCSNames[i]);

   mCoordSysComboBox->SetStringSelection(mTrajFrame->GetViewCoordSysName());

}


//------------------------------------------------------------------------------
// void UpdateObjectComboBox()
//------------------------------------------------------------------------------
void OpenGlOptionDialog::UpdateObjectComboBox()
{
   #ifdef DEBUG_GLOPTION_OBJECT
   MessageInterface::ShowMessage
      ("OpenGlOptionDialog::UpdateObjectComboBox() mObjectCount=%d\n", mObjectCount);
   #endif
   
   mGotoObjectComboBox->Clear();
   
   for (int i=0; i<mObjectCount; i++)
      mGotoObjectComboBox->Append(mObjectNames[i]);

   mGotoObjectComboBox->SetStringSelection(mTrajFrame->GetGotoObjectName());
}


//------------------------------------------------------------------------------
// void UpdateObjectListBox()
//------------------------------------------------------------------------------
void OpenGlOptionDialog::UpdateObjectListBox()
{
   #ifdef DEBUG_GLOPTION_OBJECT
   MessageInterface::ShowMessage
      ("OpenGlOptionDialog::UpdateObjectListBox() mObjectCount=%d\n", mObjectCount);
   #endif
   
   mObjectListBox->Clear();
   
   for (int i=0; i<mObjectCount; i++)
      mObjectListBox->Append(mObjectNames[i]);

   mObjectListBox->SetStringSelection(mGotoObjectComboBox->GetStringSelection());
   ShowSpacePointOption(mObjectListBox->GetStringSelection());

}


//------------------------------------------------------------------------------
// bool ShowColorDialog(wxColor &oldColor, wxButton *button)
//------------------------------------------------------------------------------
/**
 * Shows color dialog and set color to button background.
 */
//------------------------------------------------------------------------------
bool OpenGlOptionDialog::ShowColorDialog(wxColor &oldColor, wxButton *button)
{
   #ifdef DEBUG_GLOPTION
   MessageInterface::ShowMessage
      ("ShowColorDialog() old color: R=%d G=%d B=%d\n",
       oldColor.Red(), oldColor.Green(), oldColor.Blue());
   #endif
   
   wxColourData data;
   data.SetColour(oldColor);
   
   wxColourDialog dlg(this, &data);
   dlg.Center();
   
   if (dlg.ShowModal() == wxID_OK)
   {
      oldColor = dlg.GetColourData().GetColour();
      button->SetBackgroundColour(oldColor);
      theApplyButton->Enable();
      
      #ifdef DEBUG_GLOPTION
      MessageInterface::ShowMessage
         ("ShowColorDialog() new color: R=%d G=%d B=%d\n",
          oldColor.Red(), oldColor.Green(), oldColor.Blue());
      #endif
      
      return true;
   }

   return false;
}

//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles TextCtrl event.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::OnTextChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mDistanceTextCtrl)
   {
      if (mDistanceTextCtrl->IsModified())
      {
         mHasDistanceChanged = true;
         mDistance = atof(mDistanceTextCtrl->GetValue());
      }
   
      theApplyButton->Enable();
   }
}


//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles CheckBox event.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::OnCheckBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mUseInitialViewDefCheckBox)
      mHasUseViewPointSpecChanged = true;
   else if (event.GetEventObject() == mUsePerspModeCheckBox)
      mHasUsePerspModeChanged = true;
   else if (event.GetEventObject() == mXyPlaneCheckBox)
      mHasDrawXyPlaneChanged = true;
   else if (event.GetEventObject() == mEcPlaneCheckBox)
      mHasDrawEcPlaneChanged = true;
   else if (event.GetEventObject() == mESLinesCheckBox)
      mHasDrawESLineChanged = true;
   else if (event.GetEventObject() == mWireFrameCheckBox)
      mHasDrawWireFrameChanged = true;
   else if (event.GetEventObject() == mAxesCheckBox)
      mHasDrawAxesChanged = true;
   else if (event.GetEventObject() == mRotateAboutXYCheckBox)
      mHasRotateAboutXYChanged = true;
   else if (event.GetEventObject() == mShowObjectCheckBox)
   {
      mShowObjectMap[mObjectListBox->GetStringSelection()] =
         mShowObjectCheckBox->GetValue();
      mHasShowObjectChanged = true;
   }
   else if (event.GetEventObject() == mShowOrbitNormalCheckBox)
   {
      mShowOrbitNormalMap[mObjectListBox->GetStringSelection()] =
         mShowOrbitNormalCheckBox->GetValue();
      mHasShowOrbitNormalChanged = true;
   }
   
   if (event.GetEventObject() != mUseInitialViewDefCheckBox)
      theApplyButton->Enable();
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles ComboBox event.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::OnComboBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mGotoObjectComboBox)
   {
      if (!mCoordSysName.IsSameAs(mGotoObjectComboBox->GetStringSelection()))
      {
         mHasGotoObjectChanged = true;
         mGotoObjectName = mGotoObjectComboBox->GetStringSelection();
         theApplyButton->Enable();
      }
   }
   else if (event.GetEventObject() == mCoordSysComboBox)
   {
      if (!mCoordSysName.IsSameAs(mCoordSysComboBox->GetStringSelection()))
      {
         mHasCoordSysChanged = true;
         mHasDrawEcPlaneChanged = true;
         
         // if coordinate system is other than EarthMJ2000Eq,
         // uncheck and disable draw ecliptic plane CheckBox
         if (mCoordSysComboBox->GetValue() == "EarthMJ2000Eq")
         {
            mEcPlaneCheckBox->Enable();
         }
         else
         {
            mEcPlaneCheckBox->SetValue(false);
            mEcPlaneCheckBox->Disable();
         }
         
         mCoordSysName = mCoordSysComboBox->GetStringSelection();
         theApplyButton->Enable();
      }
   }
}


//------------------------------------------------------------------------------
// void OnColorButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles button click event.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::OnColorButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mXyPlaneColorButton)
   {
      mHasXyPlaneColorChanged = ShowColorDialog(mXyPlaneColor, mXyPlaneColorButton);
   }
   else if (event.GetEventObject() == mEcPlaneColorButton)
   {
      mHasEcPlaneColorChanged = ShowColorDialog(mEcPlaneColor, mEcPlaneColorButton);
   }
   else if (event.GetEventObject() == mESLinesColorButton)
   {
      mHasESLineColorChanged = ShowColorDialog(mESLinesColor, mESLinesColorButton);
   }
}


//------------------------------------------------------------------------------
// void OnObjectColorButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles button click event.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::OnObjectColorButtonClick(wxCommandEvent& event)
{
   wxColourData data;
   data.SetColour(mObjectColor);
         
   wxColourDialog dlg(this, &data);
   dlg.Center();
   
   if (dlg.ShowModal() == wxID_OK)
   {
      mHasObjectColorChanged = true;
      wxString name = mObjectListBox->GetStringSelection();
      mObjectColor = dlg.GetColourData().GetColour();
      mObjectColorButton->SetBackgroundColour(mObjectColor);
      
      mObjectColorMap[name].Set(mObjectColor.Red(),
                                mObjectColor.Green(),
                                mObjectColor.Blue());
      
      theApplyButton->Enable();
   }
}


//------------------------------------------------------------------------------
// void OnApplyButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::OnApplyButtonClick(wxCommandEvent& event)
{
   theApplyButton->Disable();
   SaveData();
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mViewAnimationButton)
   {
      mTrajFrame->SetUseInitialViewDef(mUseInitialViewDefCheckBox->GetValue());
      mAnimationUpdInt = atoi(mAnimationUpdIntTextCtrl->GetValue());
      mTrajFrame->SetAnimationUpdateInterval(mAnimationUpdInt);
      mTrajFrame->RedrawPlot(true);
   }
}


//------------------------------------------------------------------------------
// void OnSelectObject(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::OnSelectObject(wxCommandEvent& event)
{
   ShowSpacePointOption(mObjectListBox->GetStringSelection());
}


//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles close event. The dialog is not closed here but the parent should
 * hide this dialog.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::OnClose(wxCloseEvent& event)
{
   Hide();
   mTrajFrame->ResetShowViewOption();

   //loj: 4/21/05 allow to hdie this dialog
//    if ( event.CanVeto() )
//    {
//       wxMessageBox(_T("Use the menu item to close this dialog"),
//                    _T("Modeless dialog"),
//                    wxOK | wxICON_INFORMATION, this);
      
//       event.Veto();
//    }
}


//------------------------------------------------------------------------------
// void ShowSpacePointOption(const wxString &name)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::ShowSpacePointOption(const wxString &name)
{
   
   // if object name not found, insert
   if (mObjectColorMap.find(name) == mObjectColorMap.end())
   {
      mObjectColorMap[name] = RgbColor(GmatColor::L_BROWN32);
   }
   
   RgbColor orbColor = mObjectColorMap[name];
   
   #if DEBUG_GLOPTION
   MessageInterface::ShowMessage
      ("OpenGlOptionDialog::ShowSpacePointOption() name=%s, initialshow=%d, "
       "show=%d, orbColor=%08x\n", name.c_str(), mInitialShowObjectMap[name],
       mShowObjectMap[name], orbColor.GetIntColor());
   #endif
   
   mObjectColor.Set(orbColor.Red(), orbColor.Green(), orbColor.Blue());
   mObjectColorButton->SetBackgroundColour(mObjectColor);
   mShowObjectCheckBox->SetValue(mShowObjectMap[name]);
   mShowOrbitNormalCheckBox->SetValue(mShowOrbitNormalMap[name]);

   if (mInitialShowObjectMap[name])
   {
      mObjectColorButton->Enable();
      mShowObjectCheckBox->Enable();
      mShowOrbitNormalCheckBox->Enable();
   }
   else
   {
      mObjectColorButton->Disable();
      mShowObjectCheckBox->Disable();
      mShowOrbitNormalCheckBox->Disable();
   }
}

