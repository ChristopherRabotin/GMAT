//$Id$
//------------------------------------------------------------------------------
//                            GmatColorPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Linda Jun
// Created: 2013/11/08
/**
 * This class sets up SpacePoint orbit and target colors
 */
//------------------------------------------------------------------------------
#include "GmatColorPanel.hpp"
#include "GmatStaticBoxSizer.hpp"
#include <wx/config.h>
#include "RgbColor.hpp"             // for RgbColor
#include "MessageInterface.hpp"

//#define DEBUG_COLOR_CONSTRUCTOR
//#define DEBUG_COLOR_CHANGE
#define DEBUG_PANEL_LOAD
//#define DEBUG_PANEL_SAVE

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(GmatColorPanel, wxPanel)
   EVT_COLOURPICKER_CHANGED(ID_COLOR_CTRL, GmatColorPanel::OnColorPickerChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// GmatColorPanel(wxWindow *parent, GmatPanel *parentGmatPanel, SpacePoint *clonedSpacePoint)
//------------------------------------------------------------------------------
/**
 * Constructs GmatColorPanel object. The caller is responsible for copying user
 * changes in SaveData().
 *
 * @param  parent  The parent window
 * @param  parentGmatPanel  The parent GmatPanel to access EnableUpdate()
 * @param  clonedSpacePoint  The cloned SpacePoint object pointer
 */
//------------------------------------------------------------------------------
GmatColorPanel::GmatColorPanel(wxWindow *parent, GmatPanel *parentGmatPanel,
                               SpacePoint *clonedSpacePoint)
   : wxPanel(parent)
{
   #ifdef DEBUG_PANEL_CONSTRUCTOR
   MessageInterface::ShowMessage
      ("GmatColorPanel::GmatColorPanel() entered, parentGmatPanel=<%p>'%s', "
       "clonedSpacePoint=<%p>'%s'\n", parentGmatPanel, parentGmatPanel->GetName().c_str(),
       clonedSpacePoint, clonedSpacePoint->GetName().c_str());
   #endif
   
   theParentGmatPanel = parentGmatPanel;
   theClonedSpacePoint = clonedSpacePoint;
   colorChanged = false;
   
   SetName("GmatColorPanel");
   
   Create();
   LoadData();
   
   #ifdef DEBUG_PANEL_CONSTRUCTOR
   MessageInterface::ShowMessage
      ("GmatColorPanel::GmatColorPanel() leaving, parentGmatPanel=<%p>'%s', "
       "clonedSpacePoint=<%p>'%s'\n", parentGmatPanel, parentGmatPanel->GetName().c_str(),
       clonedSpacePoint, clonedSpacePoint->GetName().c_str());
   #endif
}

//------------------------------------------------------------------------------
// ~GmatColorPanel()
//------------------------------------------------------------------------------
GmatColorPanel::~GmatColorPanel()
{
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void GmatColorPanel::Create()
{
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/SpacePoint Colors"));
   
   Integer bsize = 2; // border size
   
   //-----------------------------------------------------------------
   // Platform dependent button size
   //-----------------------------------------------------------------
   #ifdef __WXMAC__
   int buttonWidth = 40;
   #else
   int buttonWidth = 25;
   #endif
   
   // Add orbit and target color button
   wxStaticText *orbitColorLabel =
      new wxStaticText(this, -1, wxT("Orbit Color"),
                       wxDefaultPosition, wxSize(-1,-1), wxALIGN_CENTRE);
   mOrbitColorCtrl =
      new wxColourPickerCtrl(this, ID_COLOR_CTRL, *wxRED,
                             wxDefaultPosition, wxSize(buttonWidth, 20), 0);
   wxStaticText *targetColorLabel =
      new wxStaticText(this, -1, wxT("Target Color"),
                       wxDefaultPosition, wxSize(-1,-1), wxALIGN_CENTRE);   
   wxColour targColor = wxTheColourDatabase->Find("STEEL BLUE");
   mTargetColorCtrl =
      new wxColourPickerCtrl(this, ID_COLOR_CTRL, targColor,
                             wxDefaultPosition, wxSize(buttonWidth, 20), 0);
   
   wxBoxSizer *colorBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   colorBoxSizer->Add(orbitColorLabel, 0, wxALIGN_CENTER, bsize);
   colorBoxSizer->Add(mOrbitColorCtrl, 0, wxALIGN_CENTER, bsize);
   colorBoxSizer->Add(20, 20);
   colorBoxSizer->Add(targetColorLabel, 0, wxALIGN_CENTER, bsize);
   colorBoxSizer->Add(mTargetColorCtrl, 0, wxALIGN_CENTER, bsize);
   
   // To center color buttons, use wxFlexGridSizer
   wxFlexGridSizer *colorFlexSizer = new wxFlexGridSizer(3);
   colorFlexSizer->Add(20, 20);
   colorFlexSizer->Add(colorBoxSizer, 0, wxGROW | wxALIGN_CENTER, 0);
   colorFlexSizer->Add(20,20);
   
   GmatStaticBoxSizer *colorSizer = new GmatStaticBoxSizer(wxVERTICAL, this, "Colors");
   colorSizer->Add(colorFlexSizer, 0, wxALIGN_CENTER, 0);
   
   // Sizer for the page
   wxBoxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);
   pageSizer->Add(colorSizer, 0, wxGROW | wxALIGN_CENTER, bsize);
   this->SetAutoLayout( true );  
   this->SetSizer( pageSizer );
   
   #if 0
   this->SetAutoLayout( true );  
   this->SetSizer( boxSizer1 );
   boxSizer1->Fit( this );
   boxSizer1->SetSizeHints( this );
   #endif
}    

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void GmatColorPanel::LoadData()
{
   #ifdef DEBUG_PANEL_LOAD
   MessageInterface::ShowMessage("GmatColorPanel::LoadData() entered.\n");
   #endif
   
   try
   {
      // Load colors from the spacecraft
      RgbColor orbitColor = RgbColor(theClonedSpacePoint->GetCurrentOrbitColor());
      RgbColor targetColor = RgbColor(theClonedSpacePoint->GetCurrentTargetColor());

      #ifdef DEBUG_PANEL_LOAD
      MessageInterface::ShowMessage("   orbitColor  = %06X\n", orbitColor.GetIntColor());
      MessageInterface::ShowMessage("   targetColor = %06X\n", targetColor.GetIntColor());
      #endif
      
      mOrbitColor.Set(orbitColor.Red(), orbitColor.Green(), orbitColor.Blue());      
      mTargetColor.Set(targetColor.Red(), targetColor.Green(), targetColor.Blue());
      mOrbitColorCtrl->SetColour(mOrbitColor);
      mTargetColorCtrl->SetColour(mTargetColor);
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage().c_str());
   }
   
   #ifdef DEBUG_PANEL_LOAD
   MessageInterface::ShowMessage("GmatColorPanel::LoadData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void GmatColorPanel::SaveData()
{
   #ifdef DEBUG_PANEL_LOAD
   MessageInterface::ShowMessage
      ("GmatColorPanel::SaveData() setting colorChanged to true and exiting.\n");
   #endif
   colorChanged = false;
}


//------------------------------------------------------------------------------
// void OnColorPickerChange(wxColourPickerEvent& event)
//------------------------------------------------------------------------------
void GmatColorPanel::OnColorPickerChange(wxColourPickerEvent& event)
{
   #ifdef DEBUG_COLOR_CHANGE
   MessageInterface::ShowMessage("GmatColorPanel::OnColorPickerChange() entered\n");
   #endif
   
   if (event.GetEventObject() == mOrbitColorCtrl)
   {
      wxColour wxcolor = mOrbitColorCtrl->GetColour();
      RgbColor color = RgbColor(wxcolor.Red(), wxcolor.Green(), wxcolor.Blue());
      std::string colorStr = RgbColor::ToRgbString(color.GetIntColor());
      theClonedSpacePoint->SetStringParameter(theClonedSpacePoint->GetParameterID("OrbitColor"), colorStr);
   }
   else if (event.GetEventObject() == mTargetColorCtrl)
   {
      wxColour wxcolor = mTargetColorCtrl->GetColour();
      RgbColor color = RgbColor(wxcolor.Red(), wxcolor.Green(), wxcolor.Blue());
      std::string colorStr = RgbColor::ToRgbString(color.GetIntColor());
      theClonedSpacePoint->SetStringParameter(theClonedSpacePoint->GetParameterID("TargetColor"), colorStr);
   }
   
	colorChanged = true;
	theParentGmatPanel->EnableUpdate(true);
   
   #ifdef DEBUG_COLOR_CHANGE
   MessageInterface::ShowMessage("GmatColorPanel::OnColorPickerChange() leaving\n");
   #endif
}

