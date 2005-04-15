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

#include "ColorTypes.hpp"           // for GmatColor::
#include "RgbColor.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"

#include "wx/colordlg.h"            // for wxColourDialog

//#define DEBUG_GL_OPTION_DIALOG 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(OpenGlOptionDialog, wxDialog)
   EVT_TEXT(ID_TEXTCTRL, OpenGlOptionDialog::OnTextChange)
   EVT_CHECKBOX(ID_CHECKBOX, OpenGlOptionDialog::OnCheckBoxChange)
   EVT_COMBOBOX(ID_COMBOBOX, OpenGlOptionDialog::OnComboBoxChange)
   EVT_BUTTON(ID_BUTTON, OpenGlOptionDialog::OnButtonClick)
   EVT_BUTTON(ID_BUTTON_APPLY, OpenGlOptionDialog::OnApplyButtonClick)
   EVT_BUTTON(ID_EQPLANE_COLOR_BUTTON, OpenGlOptionDialog::OnColorButtonClick)
   EVT_BUTTON(ID_ECPLANE_COLOR_BUTTON, OpenGlOptionDialog::OnColorButtonClick)
   EVT_BUTTON(ID_SUNLINE_COLOR_BUTTON, OpenGlOptionDialog::OnColorButtonClick)
   EVT_BUTTON(ID_BODY_COLOR_BUTTON, OpenGlOptionDialog::OnBodyColorButtonClick)
   EVT_BUTTON(ID_BODY_COLOR_BUTTON1, OpenGlOptionDialog::OnBodyColorButtonClick)
   EVT_BUTTON(ID_BODY_COLOR_BUTTON2, OpenGlOptionDialog::OnBodyColorButtonClick)
   EVT_BUTTON(ID_ADD_BODY_BUTTON, OpenGlOptionDialog::OnAddBodyButtonClick)
   
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
                                       const wxArrayString &bodyNames,
                                       const UnsignedIntArray &bodyColors)
   : wxDialog(parent, -1, title)
{
   theGuiManager = GuiItemManager::GetInstance();
   mTrajFrame = (MdiChildTrajFrame*)parent;
   
   mHasChangeMade = false;
   mHasDistanceChanged = false;
   mHasGotoBodyChanged = false;
   mHasCoordSysChanged = false;
   mHasDrawEqPlaneChanged = false;
   mHasDrawEcPlaneChanged = false;
   mHasDrawEcLineChanged = false;
   mHasDrawAxesChanged = false;
   mHasDrawWireFrameChanged = false;
   mHasEqPlaneColorChanged = false;
   mHasEcPlaneColorChanged = false;
   mHasEcLineColorChanged = false;
   mHasRotateAboutXYChanged = false;
   
   mDistance = 30000;
   mGotoBodyName = "";
   
   mBodyCount = bodyNames.GetCount();
   
   for (int i=0; i<mBodyCount; i++)
   {
      mBodyNames.Add(bodyNames[i]);
      mBodyIntColors.push_back(bodyColors[i]);
      
      #ifdef DEBUG_GL_OPTION_DIALOG
      MessageInterface::ShowMessage
         ("OpenGlOptionDialog() body=%s, color=%d\n",
          mBodyNames[i].c_str(), mBodyIntColors[i]);
      #endif
   }
   
   mEqPlaneColor = wxColor("GREY");
   mEcPlaneColor = wxColor("DARK SLATE BLUE");
   mEcLineColor = wxColor("BROWN");
   
   Create();
   ShowData();
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
}

//------------------------------------------------------------------------------
// void SetDrawEqPlane(bool flag)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::SetDrawEqPlane(bool flag)
{
   mEqPlaneCheckBox->SetValue(flag);
}

//------------------------------------------------------------------------------
// void SetDrawWireFrame(bool flag)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::SetDrawWireFrame(bool flag)
{
   mWireFrameCheckBox->SetValue(flag);
}

//------------------------------------------------------------------------------
// void SetGotoStdBody(int bodyId)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::SetGotoStdBody(int bodyId)
{
   mGotoBodyComboBox->SetStringSelection(wxString(BodyInfo::BODY_NAME[bodyId].c_str()));
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
   int borderSize = 2;
   
   // wxString
   wxString strBodyArray[] = { wxT("Sun"), wxT("Earth"), wxT("Luna") };

   // emptyStaticText
   wxStaticText *emptyStaticText =
      new wxStaticText( this, -1, wxT("  "), wxDefaultPosition, wxDefaultSize, 0 );
   
   //-----------------------------------------------------------------
   // view option
   //-----------------------------------------------------------------
   wxStaticText *distanceStaticText =
      new wxStaticText(this, -1, wxT("Distance (Km)"),
                       wxDefaultPosition, wxDefaultSize, 0);
   mDistanceTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(105, 20), 0);
   wxStaticText *centerOfViewStaticText =
      new wxStaticText(this, -1, wxT("Go To"),
                       wxDefaultPosition, wxDefaultSize, 0);
   mGotoBodyComboBox =
      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition,
                     wxSize(105,-1), 3, strBodyArray, wxCB_DROPDOWN);
   mGotoBodyComboBox->SetStringSelection("Earth");
   
   wxStaticText *coordSysStaticText =
      new wxStaticText(this, -1, wxT("Coord System"),
                       wxDefaultPosition, wxDefaultSize, 0);
   mCoordSysComboBox =
      theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX, wxSize(105, 20));
   
   mCreateCoordSysButton =
      new wxButton(this, ID_BUTTON, "Create", wxDefaultPosition, wxSize(-1,-1), 0);
   
   wxFlexGridSizer *viewGridSizer = new wxFlexGridSizer(2, 0, 0);
   viewGridSizer->Add(distanceStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   viewGridSizer->Add(mDistanceTextCtrl, 0, wxALIGN_CENTRE|wxALL, borderSize);
   viewGridSizer->Add(centerOfViewStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   viewGridSizer->Add(mGotoBodyComboBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   viewGridSizer->Add(coordSysStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   viewGridSizer->Add(mCoordSysComboBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   wxStaticBox *viewOptionStaticBox =
      new wxStaticBox(this, -1, wxT("View Options"));
   
   wxStaticBoxSizer *viewOptionSizer
      = new wxStaticBoxSizer(viewOptionStaticBox, wxVERTICAL);
   
   viewOptionSizer->Add(viewGridSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   viewOptionSizer->Add(mCreateCoordSysButton, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   //-----------------------------------------------------------------
   // drawing option
   //-----------------------------------------------------------------
   mWireFrameCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Draw Wire Frame"),
                     wxDefaultPosition, wxSize(150, 20), 0);
   mEqPlaneCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Draw Equatorial Plane"),
                     wxDefaultPosition, wxSize(150, 20), 0);
   
   mEcPlaneCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Draw Ecliptic Plane"),
                     wxDefaultPosition, wxSize(150, 20), 0);
   
   mEcLineCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Draw Earth Sun Lines"),
                     wxDefaultPosition, wxSize(150, 20), 0);

   mDrawAxesCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Draw Axes"),
                     wxDefaultPosition, wxSize(150, 20), 0);

   mRotateAboutXYCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Rotate XY"),
                     wxDefaultPosition, wxSize(150, 20), 0);

   // equatorial plane color
   mEqPlaneColorButton =
      new wxButton(this, ID_EQPLANE_COLOR_BUTTON, "", wxDefaultPosition,
                   wxSize(20,20), 0);
   mEqPlaneColorButton->SetBackgroundColour(mEqPlaneColor);
   
   // ecliptic plane color
   mEcPlaneColorButton =
      new wxButton(this, ID_ECPLANE_COLOR_BUTTON, "", wxDefaultPosition,
                   wxSize(20,20), 0);
   mEcPlaneColorButton->SetBackgroundColour(mEcPlaneColor);

   // Sun line color
   mEcLineColorButton =
      new wxButton(this, ID_SUNLINE_COLOR_BUTTON, "", wxDefaultPosition,
                   wxSize(20,20), 0);
   mEcLineColorButton->SetBackgroundColour(mEcLineColor);

   wxStaticBox *drawingOptionStaticBox =
      new wxStaticBox(this, -1, wxT("Drawing Options"));
   
   wxStaticBoxSizer *drawingOptionSizer
      = new wxStaticBoxSizer(drawingOptionStaticBox, wxVERTICAL);

   borderSize = 1;
   wxFlexGridSizer *drawGridSizer = new wxFlexGridSizer(2, 0, 0);
   drawGridSizer->Add(mRotateAboutXYCheckBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mWireFrameCheckBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mDrawAxesCheckBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mEqPlaneCheckBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mEqPlaneColorButton, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mEcPlaneCheckBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mEcPlaneColorButton, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mEcLineCheckBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mEcLineColorButton, 0, wxALIGN_CENTRE|wxALL, borderSize);

   drawingOptionSizer->Add(drawGridSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   //-----------------------------------------------------------------
   // view bodies options
   //-----------------------------------------------------------------

   RgbColor rgbcolor;
   
   for (int i=0; i<mBodyCount; i++)
   {
      rgbcolor.Set(mBodyIntColors[i]);
      wxColour wxcolor(rgbcolor.Red(), rgbcolor.Green(), rgbcolor.Blue());
      
      mViewBodies[i].name = mBodyNames[i];
      mViewBodies[i].color = wxcolor;
      
      mViewBodies[i].checkBox =
         new wxCheckBox(this, ID_CHECKBOX, mBodyNames[i], wxDefaultPosition,
                        wxSize(150, -1), 0);
      
      mViewBodies[i].checkBox->SetValue(true);
      mViewBodies[i].checkBox->Disable(); // for now
      
      mViewBodies[i].colorButton =
         new wxButton(this, ID_BODY_COLOR_BUTTON + i, "", wxDefaultPosition,
                      wxSize(20,20), 0);
      mViewBodies[i].colorButton->SetBackgroundColour(wxcolor);
      mViewBodies[i].colorButton->Disable(); // for now
      
      #ifdef DEBUG_GL_OPTION_DIALOG
      MessageInterface::ShowMessage
         ("OpenGlOptionDialog()::Create() body=%s, color=%d,%d,%d\n",
          mViewBodies[i].name.c_str(), wxcolor.Red(), wxcolor.Green(), wxcolor.Blue());
      #endif
   }

   
   mAddBodyButton = 
      new wxButton(this, ID_ADD_BODY_BUTTON, "Add/Remove Body", wxDefaultPosition,
                   wxSize(130, 20), 0);
   
   wxStaticBox *viewBodyStaticBox =
      new wxStaticBox(this, -1, wxT("View Body"));

   //-----------------------------------------------------------------
   // create body sizers
   //-----------------------------------------------------------------
   wxStaticBoxSizer *viewBodySizer
      = new wxStaticBoxSizer(viewBodyStaticBox, wxVERTICAL);

   wxFlexGridSizer *bodyGridSizer = new wxFlexGridSizer(2, 0, 0);

   for (int i=0; i<mBodyCount; i++)
   {
      bodyGridSizer->Add(mViewBodies[i].checkBox, 0, wxALIGN_CENTRE|wxALL,
                         borderSize);
      bodyGridSizer->Add(mViewBodies[i].colorButton, 0, wxALIGN_CENTRE|wxALL,
                         borderSize);
   }
   viewBodySizer->Add(bodyGridSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   viewBodySizer->Add(mAddBodyButton, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   
   //-----------------------------------------------------------------
   // create page sizers
   //-----------------------------------------------------------------
   wxBoxSizer *leftColSizer = new wxBoxSizer(wxVERTICAL);
   leftColSizer->Add(viewOptionSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   leftColSizer->Add(drawingOptionSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);

   wxFlexGridSizer *pageSizer = new wxFlexGridSizer(1, 0, 0);

   pageSizer->Add(leftColSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   pageSizer->Add(viewBodySizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   //------------------------------------------------------
   // create dialog sizer
   //------------------------------------------------------   
   theDialogSizer = new wxBoxSizer(wxVERTICAL);
   
   theApplyButton =
      new wxButton(this, ID_BUTTON_APPLY, "Apply", wxDefaultPosition, wxDefaultSize, 0);
   
   theDialogSizer->Add(pageSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   theDialogSizer->Add(theApplyButton, 0, wxALIGN_CENTRE|wxALL, 5);
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
   #if DEBUG_GL_OPTION_DIALOG
   MessageInterface::ShowMessage("OpenGlOptionDialog::LoadData() entered.\n");
   #endif
   
   // distance
   wxString strVal;
   mDistance = mTrajFrame->GetDistance();
   strVal.Printf("%g", mDistance);
   mDistanceTextCtrl->SetValue(strVal);
   
   // goto body
   mGotoBodyComboBox->
      SetStringSelection(BodyInfo::BODY_NAME[mTrajFrame->GetGotoBodyId()].c_str());

   // coordinate system
   mCoordSysComboBox->SetStringSelection(mTrajFrame->GetDesiredCoordSysName());
   
   // equatorial plane, eclitic plane, Earth-Sun line
   mEqPlaneCheckBox->SetValue(mTrajFrame->GetDrawEqPlane());
   RgbColor rgb(mTrajFrame->GetEqPlaneColor());
   mEqPlaneColor.Set(rgb.Red(), rgb.Green(), rgb.Blue());
   mEqPlaneColorButton->SetBackgroundColour(mEqPlaneColor);
   
   mEcPlaneCheckBox->SetValue(mTrajFrame->GetDrawEcPlane());
   rgb.Set(mTrajFrame->GetEcPlaneColor());
   mEcPlaneColor.Set(rgb.Red(), rgb.Green(), rgb.Blue());
   mEcPlaneColorButton->SetBackgroundColour(mEcPlaneColor);
   
   mEcLineCheckBox->SetValue(mTrajFrame->GetDrawEcLine());
   rgb.Set(mTrajFrame->GetEcLineColor());
   mEcLineColor.Set(rgb.Red(), rgb.Green(), rgb.Blue());
   mEcLineColorButton->SetBackgroundColour(mEcLineColor);
   
   // wire frame
   mWireFrameCheckBox->SetValue(mTrajFrame->GetDrawWireFrame());
   mRotateAboutXYCheckBox->SetValue(mTrajFrame->GetRotateAboutXY());
   
   mCreateCoordSysButton->Disable();
   mEcPlaneCheckBox->Enable();
   mEcPlaneColorButton->Enable();
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
   mHasChangeMade = true;

   if (mHasDistanceChanged)
   {
      mHasDistanceChanged = false;
      mTrajFrame->SetDistance(mDistance);
   }
   
   if (mHasGotoBodyChanged)
   {
      mHasGotoBodyChanged = false;
      mTrajFrame->SetGotoBodyName(mGotoBodyName);
   }
   
   if (mHasCoordSysChanged)
   {
      mHasCoordSysChanged = false;
      mTrajFrame->DrawInOtherCoordSystem(mCoordSysName);
   }
   
   if (mHasDrawEqPlaneChanged)
   {
      mHasDrawEqPlaneChanged = false;
      mTrajFrame->SetDrawEqPlane(mDrawEqPlane);
   }
   
   if (mHasDrawEcPlaneChanged)
   {
      mHasDrawEcPlaneChanged = false;
      mTrajFrame->SetDrawEcPlane(mDrawEcPlane);
   }
   
   if (mHasDrawEcLineChanged)
   {
      mHasDrawEcLineChanged = false;
      mTrajFrame->SetDrawEcLine(mDrawEcLine);
   }
   
   if (mHasDrawWireFrameChanged)
   {
      mHasDrawWireFrameChanged = false;
      mTrajFrame->SetDrawWireFrame(mDrawWireFrame);
   }
   
   if (mHasDrawAxesChanged)
   {
      mHasDrawAxesChanged = false;
      mTrajFrame->SetDrawAxes(mDrawAxes);
   }
   
   if (mHasRotateAboutXYChanged)
   {
      mHasRotateAboutXYChanged = false;
      if (mRotateAboutXYCheckBox->GetValue())
         mTrajFrame->SetRotateAboutXY(true);
      else
         mTrajFrame->SetRotateAboutXY(false);
   }
   
   if (mHasEqPlaneColorChanged)
   {
      mHasEqPlaneColorChanged = false;
      RgbColor rgb(mEqPlaneColor.Red(), mEqPlaneColor.Green(), mEqPlaneColor.Blue());
      mTrajFrame->SetEqPlaneColor(rgb.GetIntColor());      
   }
   
   if (mHasEcPlaneColorChanged)
   {
      mHasEcPlaneColorChanged = false;
      RgbColor rgb(mEcPlaneColor.Red(), mEcPlaneColor.Green(), mEcPlaneColor.Blue());
      mTrajFrame->SetEcPlaneColor(rgb.GetIntColor());      
   }
   
   if (mHasEcLineColorChanged)
   {
      mHasEcLineColorChanged = false;
      RgbColor rgb(mEcLineColor.Red(), mEcLineColor.Green(), mEcLineColor.Blue());
      mTrajFrame->SetEcLineColor(rgb.GetIntColor());      
   }
   
   mTrajFrame->UpdatePlot();
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
// bool ShowColorDialog(wxColor &oldColor, wxButton *button)
//------------------------------------------------------------------------------
/**
 * Shows color dialog and set color to button background.
 */
//------------------------------------------------------------------------------
bool OpenGlOptionDialog::ShowColorDialog(wxColor &oldColor, wxButton *button)
{
   #ifdef DEBUG_GL_OPTION_DIALOG
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
      
      #ifdef DEBUG_GL_OPTION_DIALOG
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
         theApplyButton->Enable();
      }
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
   if (event.GetEventObject() == mEqPlaneCheckBox)
   {
      mHasDrawEqPlaneChanged = true;
      mDrawEqPlane = mEqPlaneCheckBox->GetValue();
   }
   else if (event.GetEventObject() == mEcPlaneCheckBox)
   {
      mHasDrawEcPlaneChanged = true;
      mDrawEcPlane = mEcPlaneCheckBox->GetValue();
   }
   else if (event.GetEventObject() == mEcLineCheckBox)
   {
      mHasDrawEcLineChanged = true;
      mDrawEcLine = mEcLineCheckBox->GetValue();
   }
   else if (event.GetEventObject() == mWireFrameCheckBox)
   {
      mHasDrawWireFrameChanged = true;
      mDrawWireFrame = mWireFrameCheckBox->GetValue();
   }
   else if (event.GetEventObject() == mDrawAxesCheckBox)
   {
      mHasDrawAxesChanged = true;
      mDrawAxes = mDrawAxesCheckBox->GetValue();
   }
   else if (event.GetEventObject() == mRotateAboutXYCheckBox)
   {
      mHasRotateAboutXYChanged = true;
   }
   
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
   if (event.GetEventObject() == mGotoBodyComboBox)
   {
      mHasGotoBodyChanged = true;
      mGotoBodyName = mGotoBodyComboBox->GetStringSelection();
      theApplyButton->Enable();
   }
   else if (event.GetEventObject() == mCoordSysComboBox)
   {
      if (!mCoordSysName.IsSameAs(mCoordSysComboBox->GetStringSelection()))
      {
         mHasCoordSysChanged = true;
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
   if (event.GetEventObject() == mEqPlaneColorButton)
   {
      mHasEqPlaneColorChanged = ShowColorDialog(mEqPlaneColor, mEqPlaneColorButton);
      
//        #ifdef DEBUG_GL_OPTION_DIALOG
//        MessageInterface::ShowMessage
//           ("mEqPlaneColorButton old color: R=%d G=%d B=%d\n",
//            mEqPlaneColor.Red(), mEqPlaneColor.Green(), mEqPlaneColor.Blue());
//        #endif
      
//        wxColourData data;
//        data.SetColour(mEqPlaneColor);

//        wxColourDialog dlg(this, &data);
//        dlg.Center();
   
//        if (dlg.ShowModal() == wxID_OK)
//        {
//           mEqPlaneColor = dlg.GetColourData().GetColour();
//           mEqPlaneColorButton->SetBackgroundColour(mEqPlaneColor);
//           theApplyButton->Enable();
//           mHasEqPlaneColorChanged = true;
         
//           #ifdef DEBUG_GL_OPTION_DIALOG
//           MessageInterface::ShowMessage
//              ("mEqPlaneColorButton new color: R=%d G=%d B=%d\n",
//               mEqPlaneColor.Red(), mEqPlaneColor.Green(), mEqPlaneColor.Blue());
//           #endif
//        }
   }
   else if (event.GetEventObject() == mEcPlaneColorButton)
   {
      mHasEcPlaneColorChanged = ShowColorDialog(mEcPlaneColor, mEcPlaneColorButton);
   }
   else if (event.GetEventObject() == mEcLineColorButton)
   {
      #ifdef DEBUG_GL_OPTION_DIALOG
      MessageInterface::ShowMessage
         ("mEcLineColorButton old color: R=%d G=%d B=%d\n",
          mEcLineColor.Red(), mEcLineColor.Green(), mEcLineColor.Blue());
      #endif
      
      wxColourData data;
      data.SetColour(mEcLineColor);

      wxColourDialog dlg(this, &data);
      dlg.Center();
   
      if (dlg.ShowModal() == wxID_OK)
      {
         mEcLineColor = dlg.GetColourData().GetColour();
         mEcLineColorButton->SetBackgroundColour(mEcLineColor);
         mEcLineColorButton->Clear();
         theApplyButton->Enable();
         
         #ifdef DEBUG_GL_OPTION_DIALOG
         MessageInterface::ShowMessage
            ("mEcLineColorButton new color: R=%d G=%d B=%d\n",
             mEcLineColor.Red(), mEcLineColor.Green(), mEcLineColor.Blue());
         #endif
      }
   }
}

//------------------------------------------------------------------------------
// void OnBodyColorButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles button click event.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::OnBodyColorButtonClick(wxCommandEvent& event)
{
   for (int i=0; i<mBodyCount; i++)
   {
      if (event.GetEventObject() == mViewBodies[i].colorButton)
      {
         wxColourData data;
         data.SetColour(mViewBodies[i].color);
         
         wxColourDialog dlg(this, &data);
         dlg.Center();
         
         if (dlg.ShowModal() == wxID_OK)
         {
            wxColour wxcolor = dlg.GetColourData().GetColour();
            mViewBodies[i].color = wxcolor;
            mViewBodies[i].colorButton->SetBackgroundColour(wxcolor);
         }

         break;
      }
   }
}

//------------------------------------------------------------------------------
// void OnAddBodyButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles button click event.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::OnAddBodyButtonClick(wxCommandEvent& event)
{
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
   if (event.GetEventObject() == mCreateCoordSysButton)
   {
      // show dialog to create CoordinateSystem
      // assuming the dialog will create and configure the CoordinateSystem

      // add CoordinateSystem to ComboBox
   }
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
   if ( event.CanVeto() )
   {
      wxMessageBox(_T("Use the menu item to close this dialog"),
                   _T("Modeless dialog"),
                   wxOK | wxICON_INFORMATION, this);
      
      event.Veto();
   }
}

