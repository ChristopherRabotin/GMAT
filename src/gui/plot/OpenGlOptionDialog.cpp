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
   EVT_BUTTON(ID_BUTTON_APPLY, OpenGlOptionDialog::OnApplyButtonClick)
   EVT_BUTTON(ID_EQUPLANE_COLOR_BUTTON, OpenGlOptionDialog::OnColorButtonClick)
   EVT_BUTTON(ID_SUNLINE_COLOR_BUTTON, OpenGlOptionDialog::OnColorButtonClick)
   EVT_BUTTON(ID_BODY_COLOR_BUTTON, OpenGlOptionDialog::OnBodyColorButtonClick)
   EVT_BUTTON(ID_BODY_COLOR_BUTTON1, OpenGlOptionDialog::OnBodyColorButtonClick)
   EVT_BUTTON(ID_BODY_COLOR_BUTTON2, OpenGlOptionDialog::OnBodyColorButtonClick)
   EVT_BUTTON(ID_ADD_BODY_BUTTON, OpenGlOptionDialog::OnAddBodyButtonClick)
   
   EVT_CLOSE(OpenGlOptionDialog::OnClose)
END_EVENT_TABLE()


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
   mTrajFrame = (MdiChildTrajFrame*)parent;
   
   mHasChangeMade = false;
   mIsDistanceChanged = false;
   mIsGotoBodyChanged = false;

   mDistance = 30000;
   mGotoBodyName = "";
   
   mBodyCount = bodyNames.GetCount();
   
   for (int i=0; i<mBodyCount; i++)
   {
      //mBodyColorMap[bodyNames[i]] = bodyColors[i];
      mBodyNames.Add(bodyNames[i]);
      mBodyIntColors.push_back(bodyColors[i]);
      
#ifdef DEBUG_GL_OPTION_DIALOG
      MessageInterface::ShowMessage
         ("OpenGlOptionDialog() body=%s, color=%d\n",
          mBodyNames[i].c_str(), mBodyIntColors[i]);
#endif
   }
   
   mEquPlaneColor = wxColor("GREY");
   mSunLineColor = wxColor("BROWN");
   
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
// void SetDrawEquPlane(bool flag)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::SetDrawEquPlane(bool flag)
{
   mEquPlaneCheckBox->SetValue(flag);
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
   mGotoBodyComboBox->SetStringSelection(wxString(GmatPlot::BODY_NAME[bodyId].c_str()));
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
   wxString strCoordArray[] = { wxT("") };

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
      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition,
                     wxSize(105,-1), 1, strCoordArray, wxCB_DROPDOWN);

   wxStaticBox *viewOptionStaticBox =
      new wxStaticBox(this, -1, wxT("View Options"));
   
   wxStaticBoxSizer *viewOptionSizer
      = new wxStaticBoxSizer(viewOptionStaticBox, wxVERTICAL);

   wxFlexGridSizer *viewGridSizer = new wxFlexGridSizer(2, 0, 0);
   viewGridSizer->Add(distanceStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   viewGridSizer->Add(mDistanceTextCtrl, 0, wxALIGN_CENTRE|wxALL, borderSize);
   viewGridSizer->Add(centerOfViewStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   viewGridSizer->Add(mGotoBodyComboBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   viewGridSizer->Add(coordSysStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   viewGridSizer->Add(mCoordSysComboBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   viewOptionSizer->Add(viewGridSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   //-----------------------------------------------------------------
   // drawing option
   //-----------------------------------------------------------------
   mWireFrameCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Draw Wire Frame"),
                     wxDefaultPosition, wxSize(150, 20), 0);
   mEquPlaneCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Draw Equatorial Plane"),
                     wxDefaultPosition, wxSize(150, 20), 0);
   
   mEarthSunLineCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Draw Earth Sun Lines"),
                     wxDefaultPosition, wxSize(150, 20), 0);
   mEarthSunLineCheckBox->Disable(); // for now
   
   // equatorial plane color
   mEquPlaneColorButton =
      new wxButton(this, ID_EQUPLANE_COLOR_BUTTON, "", wxDefaultPosition,
                   wxSize(20,20), 0);
   mEquPlaneColorButton->SetBackgroundColour(mEquPlaneColor);
   
   // Sun line color
   mEarthSunLineColorButton =
      new wxButton(this, ID_SUNLINE_COLOR_BUTTON, "", wxDefaultPosition,
                   wxSize(20,20), 0);
   mEarthSunLineColorButton->SetBackgroundColour(mSunLineColor);
   mEarthSunLineColorButton->Disable(); // for now

   wxStaticBox *drawingOptionStaticBox =
      new wxStaticBox(this, -1, wxT("Drawing Options"));
   
   wxStaticBoxSizer *drawingOptionSizer
      = new wxStaticBoxSizer(drawingOptionStaticBox, wxVERTICAL);

   wxFlexGridSizer *drawGridSizer = new wxFlexGridSizer(2, 0, 0);
   drawGridSizer->Add(mWireFrameCheckBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mEquPlaneCheckBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mEquPlaneColorButton, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mEarthSunLineCheckBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mEarthSunLineColorButton, 0, wxALIGN_CENTRE|wxALL, borderSize);

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
   // distance
   wxString strVal;
   mDistance = mTrajFrame->GetDistance();
   strVal.Printf("%g", mDistance);
   mDistanceTextCtrl->SetValue(strVal);

   // goto body
   mGotoBodyComboBox->
      SetStringSelection(GmatPlot::BODY_NAME[mTrajFrame->GetGotoBodyId()].c_str());

   // equatorial plane, wire frame
   mEquPlaneCheckBox->SetValue(mTrajFrame->GetDrawEquPlane());
   RgbColor rgb(mTrajFrame->GetEquPlaneColor());
   mEquPlaneColor.Set(rgb.Red(), rgb.Green(), rgb.Blue());
   mEquPlaneColorButton->SetBackgroundColour(mEquPlaneColor);
   mWireFrameCheckBox->SetValue(mTrajFrame->GetDrawWireFrame());

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

   if (mIsDistanceChanged)
   {
      mIsDistanceChanged = false;
      mTrajFrame->SetDistance(mDistance);
   }
   
   if (mIsGotoBodyChanged)
   {
      mIsGotoBodyChanged = false;
      mTrajFrame->SetGotoBodyName(mGotoBodyName);
   }

   if (mDrawEquPlaneChanged)
   {
      mDrawEquPlaneChanged = false;
      mTrajFrame->SetDrawEquPlane(mDrawEquPlane);
   }
   
   if (mDrawWireFrameChanged)
   {
      mDrawWireFrameChanged = false;
      mTrajFrame->SetDrawWireFrame(mDrawWireFrame);
   }
   
   if (mIsEquPlaneColorChanged)
   {
      mIsEquPlaneColorChanged = false;
      RgbColor rgb(mEquPlaneColor.Red(), mEquPlaneColor.Green(), mEquPlaneColor.Blue());
      mTrajFrame->SetEquPlaneColor(rgb.GetIntColor());      
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
         mIsDistanceChanged = true;
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
   if (event.GetEventObject() == mEquPlaneCheckBox)
   {
      mDrawEquPlaneChanged = true;
      mDrawEquPlane = mEquPlaneCheckBox->GetValue();
   }
   else if (event.GetEventObject() == mWireFrameCheckBox)
   {
      mDrawWireFrameChanged = true;
      mDrawWireFrame = mWireFrameCheckBox->GetValue();
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
   mIsGotoBodyChanged = true;
   mGotoBodyName = mGotoBodyComboBox->GetStringSelection();
   theApplyButton->Enable();
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
   if (event.GetEventObject() == mEquPlaneColorButton)
   {
#ifdef DEBUG_GL_OPTION_DIALOG
      MessageInterface::ShowMessage
         ("mEquPlaneColorButton old color: R=%d G=%d B=%d\n",
          mEquPlaneColor.Red(), mEquPlaneColor.Green(), mEquPlaneColor.Blue());
#endif
      wxColourData data;
      data.SetColour(mEquPlaneColor);

      wxColourDialog dlg(this, &data);
      dlg.Center();
   
      if (dlg.ShowModal() == wxID_OK)
      {
         mEquPlaneColor = dlg.GetColourData().GetColour();
         mEquPlaneColorButton->SetBackgroundColour(mEquPlaneColor);
         //mEquPlaneColorButton->Clear();
         theApplyButton->Enable();
         mIsEquPlaneColorChanged = true;
         
#ifdef DEBUG_GL_OPTION_DIALOG
         MessageInterface::ShowMessage
            ("mEquPlaneColorButton new color: R=%d G=%d B=%d\n",
             mEquPlaneColor.Red(), mEquPlaneColor.Green(), mEquPlaneColor.Blue());
#endif
      }
   }
   else if (event.GetEventObject() == mEarthSunLineColorButton)
   {
#ifdef DEBUG_GL_OPTION_DIALOG
      MessageInterface::ShowMessage
         ("mEarthSunLineColorButton old color: R=%d G=%d B=%d\n",
          mSunLineColor.Red(), mSunLineColor.Green(), mSunLineColor.Blue());
#endif
      wxColourData data;
      data.SetColour(mSunLineColor);

      wxColourDialog dlg(this, &data);
      dlg.Center();
   
      if (dlg.ShowModal() == wxID_OK)
      {
         mSunLineColor = dlg.GetColourData().GetColour();
         mEarthSunLineColorButton->SetBackgroundColour(mSunLineColor);
         mEarthSunLineColorButton->Clear();
         theApplyButton->Enable();
         
#ifdef DEBUG_GL_OPTION_DIALOG
         MessageInterface::ShowMessage
            ("mEarthSunLineColorButton new color: R=%d G=%d B=%d\n",
             mSunLineColor.Red(), mSunLineColor.Green(), mSunLineColor.Blue());
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

