//$Id$
//------------------------------------------------------------------------------
//                            BallisticsMassPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Allison Greene
// Created: 2004/04/01
/**
 * This class contains information needed to setup users spacecraft ballistics
 * and mass through GUI
 * 
 */
//------------------------------------------------------------------------------
#include <fstream>
#include <wx/config.h>
#include "BallisticsMassPanel.hpp"
#include "GuiItemManager.hpp"
#include "MessageInterface.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "StringUtil.hpp"  // for ToReal()
#include "bitmaps/OpenFolder.xpm"

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(BallisticsMassPanel, wxPanel)
   EVT_TEXT(ID_TEXTCTRL,             BallisticsMassPanel::OnTextChange)
   EVT_TEXT(ID_SPAD_TEXTCTRL,        BallisticsMassPanel::OnSPADTextChange)
   EVT_BUTTON(ID_SPAD_BUTTON_BROWSE, BallisticsMassPanel::OnBrowseButton)
END_EVENT_TABLE()
   
//------------------------------
// public methods
//------------------------------
//------------------------------------------------------------------------------
// BallisticsMassPanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs BallisticsMassPanel object.
 *
 * @param <parent> Window parent.
 */
//------------------------------------------------------------------------------
BallisticsMassPanel::BallisticsMassPanel(GmatPanel *scPanel, wxWindow *parent,
                                         Spacecraft *spacecraft)
   : wxPanel(parent)
{
   theScPanel = scPanel;
   theSpacecraft = spacecraft;
   
   canClose        = true;
   dataChanged     = false;
   spadFileChanged = false;
   
   Create();
}

//------------------------------------------------------------------------------
// ~BallisticsMassPanel()
//------------------------------------------------------------------------------
/**
 * Destructs a  BallisticsMassPanel object.
 *
 */
//------------------------------------------------------------------------------
BallisticsMassPanel::~BallisticsMassPanel()
{
}

//-------------------------------
// private methods
//-------------------------------
//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates the page for ballistics and mass information.
 */
//------------------------------------------------------------------------------
void BallisticsMassPanel::Create()
{

   #if __WXMAC__
   int buttonWidth = 40;
   #else
   int buttonWidth = 25;
   #endif

   wxBitmap openBitmap = wxBitmap(OpenFolder_xpm);

   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Spacecraft Ballistic Mass"));

    wxStaticText *emptyText = new wxStaticText( this, ID_TEXT,
                            wxT(""),
                            wxDefaultPosition, wxDefaultSize, 0 );

    wxStaticBox *item1 = new wxStaticBox( this, -1, wxT("") );
    wxStaticBoxSizer *item0 = new wxStaticBoxSizer( item1, wxVERTICAL );
    GmatStaticBoxSizer *optionsSizer = new GmatStaticBoxSizer( wxVERTICAL, this, "Spherical" );
    item0->Add(optionsSizer, 1, wxALIGN_LEFT|wxGROW);

    GmatStaticBoxSizer *spadSizer = new GmatStaticBoxSizer( wxVERTICAL, this, "SPAD File" );
    item0->Add(spadSizer, 1, wxALIGN_LEFT|wxGROW);

    wxFlexGridSizer *item2 = new wxFlexGridSizer( 3, 0, 0 );
    item2->AddGrowableCol(1);

    wxStaticText *dryMassStaticText = new wxStaticText( this, ID_TEXT, 
                            "Dry " GUI_ACCEL_KEY "Mass",
                            wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( dryMassStaticText, 0, wxALIGN_LEFT|wxALL, 5 );

    dryMassTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
    dryMassTextCtrl->SetToolTip(pConfig->Read(_T("DryMassHint")));
    item2->Add( dryMassTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *dryMassUnitsText = new wxStaticText( this, ID_TEXT,
                            wxT("kg"),
                            wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( dryMassUnitsText, 0, wxALIGN_LEFT|wxALL, 5 );

    wxStaticText *dragCoeffStaticText = new wxStaticText( this, ID_TEXT, 
                            "Coefficient of " GUI_ACCEL_KEY "Drag", wxDefaultPosition,
                            wxDefaultSize, 0 );
    item2->Add( dragCoeffStaticText, 0, wxALIGN_LEFT|wxALL, 5 );

    dragCoeffTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
    dragCoeffTextCtrl->SetToolTip(pConfig->Read(_T("DragCoefficientHint")));
    item2->Add( dragCoeffTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    item2->Add( emptyText, 0, wxALIGN_LEFT|wxALL, 5 );

    wxStaticText *reflectCoeffStaticText = new wxStaticText( this, ID_TEXT, 
                            "Coefficient of " GUI_ACCEL_KEY "Reflectivity",
                            wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( reflectCoeffStaticText, 0, wxALIGN_LEFT|wxALL, 5 );

    reflectCoeffTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
    reflectCoeffTextCtrl->SetToolTip(pConfig->Read(_T("ReflectivityCoefficientHint")));
    item2->Add( reflectCoeffTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    item2->Add( emptyText, 0, wxALIGN_LEFT|wxALL, 5 );

    wxStaticText *dragAreaStaticText = new wxStaticText( this, ID_TEXT, 
                            "Drag " GUI_ACCEL_KEY "Area",
                            wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( dragAreaStaticText, 0, wxALIGN_LEFT|wxALL, 5 );

    dragAreaTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
    dragAreaTextCtrl->SetToolTip(pConfig->Read(_T("DragAreaHint")));
    item2->Add( dragAreaTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    wxStaticText *dragAreaUnitsText = new wxStaticText( this, ID_TEXT,
                            wxT("m^2"),
                            wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( dragAreaUnitsText, 0, wxALIGN_LEFT|wxALL, 5 );

    wxStaticText *srpAreaStaticText = new wxStaticText( this, ID_TEXT, 
                            GUI_ACCEL_KEY"SRP Area", 
                            wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( srpAreaStaticText, 0, wxALIGN_LEFT|wxALL, 5 );

    srpAreaTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
    srpAreaTextCtrl->SetToolTip(pConfig->Read(_T("SRPAreaHint")));
    item2->Add( srpAreaTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    wxStaticText *srpAreaUnitsText = new wxStaticText( this, ID_TEXT,
                            wxT("m^2"),
                            wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( srpAreaUnitsText, 0, wxALIGN_LEFT|wxALL, 5 );

    wxFlexGridSizer *spadFlexSizer = new wxFlexGridSizer( 3, 0, 0 );
    spadFlexSizer->AddGrowableCol(1);

    wxStaticText *srpFileStaticText = new wxStaticText( this, ID_TEXT,
                            GUI_ACCEL_KEY"SPAD SRP File",
                            wxDefaultPosition, wxDefaultSize, 0 );
    spadFlexSizer->Add( srpFileStaticText, 0, wxALIGN_LEFT|wxALL, 5 );

    spadSrpFileTextCtrl = new wxTextCtrl( this, ID_SPAD_TEXTCTRL, wxT(""),
                            wxDefaultPosition, wxSize(300,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
    spadSrpFileTextCtrl->SetToolTip(pConfig->Read(_T("SPADSRPFileHint")));
    spadFlexSizer->Add( spadSrpFileTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    spadBrowseButton =
       new wxBitmapButton(this, ID_SPAD_BUTTON_BROWSE, openBitmap, wxDefaultPosition,
                          wxSize(buttonWidth, -1));
    spadBrowseButton->SetToolTip(pConfig->Read(_T("BrowseSPADSRPFileNameHint")));
    spadFlexSizer->Add( spadBrowseButton, 0, wxALIGN_LEFT|wxALL, 5 );

    wxStaticText *srpScaleFactorStaticText = new wxStaticText( this, ID_TEXT,
                            GUI_ACCEL_KEY"SPAD SRP Scale Factor",
                            wxDefaultPosition, wxDefaultSize, 0 );
    spadFlexSizer->Add( srpScaleFactorStaticText, 0, wxALIGN_LEFT|wxALL, 5 );

    spadSrpScaleFactorTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
                            wxDefaultPosition, wxSize(80,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
    spadSrpScaleFactorTextCtrl->SetToolTip(pConfig->Read(_T("SPADSRPScaleFactorHint")));
    spadFlexSizer->Add( spadSrpScaleFactorTextCtrl, 0, wxALIGN_LEFT|wxALL, 5 );
    spadFlexSizer->Add( emptyText, 0, wxALIGN_LEFT|wxALL, 5 );


    optionsSizer->Add( item2, 0, wxALIGN_LEFT|wxALL, 5 );
    spadSizer->Add( spadFlexSizer, 0, wxALIGN_LEFT|wxALL, 5 );

    this->SetAutoLayout( TRUE );  
    this->SetSizer( item0 );
    item0->Fit( this );
    item0->SetSizeHints( this );
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the data for the ballistics and mass panel.
 */
//------------------------------------------------------------------------------
void BallisticsMassPanel::LoadData()
{
   try
   {
      Integer dryMassID      = theSpacecraft->GetParameterID("DryMass");
      Integer coeffDragID    = theSpacecraft->GetParameterID("Cd");
      Integer dragAreaID     = theSpacecraft->GetParameterID("DragArea");
      Integer reflectCoeffID = theSpacecraft->GetParameterID("Cr");
      Integer srpAreaID      = theSpacecraft->GetParameterID("SRPArea");

      Integer srpFileID        = theSpacecraft->GetParameterID("SPADSRPFile");
      Integer srpScaleFactorID = theSpacecraft->GetParameterID("SPADSRPScaleFactor");
      
      Real mass         = theSpacecraft->GetRealParameter(dryMassID);
      Real dragCoeff    = theSpacecraft->GetRealParameter(coeffDragID);
      Real dragArea     = theSpacecraft->GetRealParameter(dragAreaID);
      Real reflectCoeff = theSpacecraft->GetRealParameter(reflectCoeffID);
      Real srpArea      = theSpacecraft->GetRealParameter(srpAreaID);
      std::string srpFile = theSpacecraft->GetStringParameter(srpFileID);
      Real srpScaleFactor = theSpacecraft->GetRealParameter(srpScaleFactorID);
      
      GuiItemManager *theGuiManager = GuiItemManager::GetInstance();
      
      dryMassTextCtrl->SetValue(theGuiManager->ToWxString(mass));
      dragCoeffTextCtrl->SetValue(theGuiManager->ToWxString(dragCoeff));
      dragAreaTextCtrl->SetValue(theGuiManager->ToWxString(dragArea));
      reflectCoeffTextCtrl->SetValue(theGuiManager->ToWxString(reflectCoeff));
      srpAreaTextCtrl->SetValue(theGuiManager->ToWxString(srpArea));
      spadSrpFileTextCtrl->SetValue(srpFile.c_str());
      spadSrpScaleFactorTextCtrl->SetValue(theGuiManager->ToWxString(srpScaleFactor));
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage(e.GetFullMessage());
   }
   
   dataChanged = false;
   
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves the data from the ballistics and mass panel to the Spacecraft.
 */
//------------------------------------------------------------------------------
void BallisticsMassPanel::SaveData()
{
   try
   {
      canClose    = true;
    
      Integer dryMassID      = theSpacecraft->GetParameterID("DryMass");
      Integer coeffDragID    = theSpacecraft->GetParameterID("Cd");
      Integer reflectCoeffID = theSpacecraft->GetParameterID("Cr");
      Integer dragAreaID     = theSpacecraft->GetParameterID("DragArea");
      Integer srpAreaID      = theSpacecraft->GetParameterID("SRPArea");
      Integer srpFileID        = theSpacecraft->GetParameterID("SPADSRPFile");
      Integer srpScaleFactorID = theSpacecraft->GetParameterID("SPADSRPScaleFactor");

      std::string inputString;
    
      Real rvalue;
      std::string msg = "The value of \"%s\" for field \"%s\" on object \"" + 
                         theSpacecraft->GetName() + "\" is not an allowed value. \n"
                        "The allowed values are: [%s].";                        

      // check to see if dry mass is a real and 
      // greater than or equal to 0.0
      inputString = dryMassTextCtrl->GetValue();      
      if ((GmatStringUtil::ToReal(inputString,&rvalue)) && 
          (rvalue >= 0.0))
         theSpacecraft->SetRealParameter(dryMassID, rvalue);
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
            inputString.c_str(),"Dry Mass","Real Number >= 0.0");
         canClose = false;
      }

      // check to see if coeff of drag is a real and 
      // greater than or equal to 0.0
      inputString = dragCoeffTextCtrl->GetValue();      
      if ((GmatStringUtil::ToReal(inputString,&rvalue)) && 
          (rvalue >= 0.0))
         theSpacecraft->SetRealParameter(coeffDragID, rvalue);
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
            inputString.c_str(),"Coefficient of Drag","Real Number >= 0.0");
         canClose = false;
      }

      // check to see if coeff of reflectivity is a real and 
      // greater than or equal to 0.0
      inputString = reflectCoeffTextCtrl->GetValue();      
      if ((GmatStringUtil::ToReal(inputString,&rvalue)) && 
          (rvalue >= 0.0) && (rvalue <= 2.0))
         theSpacecraft->SetRealParameter(reflectCoeffID, rvalue);
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
            inputString.c_str(),"Coefficient of Reflectivity",
            "0.0 <= Real Number <= 2.0");
         canClose = false;
      }

      // check to see if drag area is a real and 
      // greater than or equal to 0.0
      inputString = dragAreaTextCtrl->GetValue();      
      if ((GmatStringUtil::ToReal(inputString,&rvalue)) && 
          (rvalue >= 0.0))
         theSpacecraft->SetRealParameter(dragAreaID, rvalue);
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
            inputString.c_str(),"Drag Area",
            "Real Number >= 0.0");
         canClose = false;
      }

      // check to see if SRP area is a real and 
      // greater than or equal to 0.0
      inputString = srpAreaTextCtrl->GetValue();      
      if ((GmatStringUtil::ToReal(inputString,&rvalue)) && 
          (rvalue >= 0.0))
         theSpacecraft->SetRealParameter(srpAreaID, rvalue);
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
            inputString.c_str(),"SRP Area",
            "Real Number >= 0.0");
         canClose = false;
      }
      // check to see if SPAD SRP scale factor is a real and
      // greater than or equal to 0.0
      inputString = spadSrpScaleFactorTextCtrl->GetValue();
      if ((GmatStringUtil::ToReal(inputString,&rvalue)) &&
          (rvalue >= 0.0))
         theSpacecraft->SetRealParameter(srpScaleFactorID, rvalue);
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
            inputString.c_str(),"SPAD SRP Scale Factor",
            "Real Number >= 0.0");
         canClose = false;
      }

      if (spadFileChanged)
      {
         try
         {
            bool success = true;

            wxString str    = spadSrpFileTextCtrl->GetValue();
            theSpadSrpFile  = str.c_str();
            std::ifstream filename(str.WX_TO_C_STRING);

            // Check if the file doesn't exist then stop
            if (!filename)
            {
               std::string errmsg = "The value \"";
               errmsg += theSpadSrpFile + "\" for field \"SPADSRPFile\" ";
               errmsg += "is not an allowed value.  The allowed values are: ";
               errmsg += "[ File must exist ]\n";
               MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
               canClose    = false;
               dataChanged = true;
               success     = false;
               theSpadSrpFile     = prevSpadSrpFile;
               return;
            }
            filename.close();

            if (theSpacecraft->SetStringParameter(srpFileID, theSpadSrpFile))
            {
               prevSpadSrpFile = str.c_str();
               success             = true;
            }
            else
            {
               // if there was an error, set it back to what it was the last time it was saved
               spadSrpFileTextCtrl->SetValue(prevSpadSrpFile.c_str());
               theSpadSrpFile         = prevSpadSrpFile;
               success                = false;
            }
            spadFileChanged = false;


            if (success)
            {
               canClose = true;
            }
            else
            {
               canClose    = false;
               dataChanged = true;
            }
         }
         catch (BaseException &ex)
         {
            canClose    = false;
            dataChanged = true;
            MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
         }
      }

      if (canClose)
         dataChanged = false;
      
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("BallisticsMassPanel::SaveData() error occurred!\n%s\n", 
          e.GetFullMessage().c_str());
      canClose = false;
      return;
   }
}


//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
/**
 * Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void BallisticsMassPanel::OnTextChange(wxCommandEvent &event)
{
   if (dryMassTextCtrl->IsModified()       || dragCoeffTextCtrl->IsModified()   ||
       dragAreaTextCtrl->IsModified()      || srpAreaTextCtrl->IsModified()     ||
       spadSrpScaleFactorTextCtrl->IsModified()                                 ||
       reflectCoeffTextCtrl->IsModified())
   {
      dataChanged = true;
      theScPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnSPADTextChange()
//------------------------------------------------------------------------------
/**
 * Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void BallisticsMassPanel::OnSPADTextChange(wxCommandEvent &event)
{
   if (spadSrpFileTextCtrl->IsModified())
   {
      dataChanged     = true;
      spadFileChanged = true;
      theScPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnBrowseButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
void BallisticsMassPanel::OnBrowseButton(wxCommandEvent &event)
{
   wxString prevFilename = spadSrpFileTextCtrl->GetValue();
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));

   if (dialog.ShowModal() == wxID_OK)
   {
      wxString filename;

      filename = dialog.GetPath().c_str();

      if (!filename.IsSameAs(prevFilename))
      {
         spadSrpFileTextCtrl->SetValue(filename);
         spadFileChanged = true;
         dataChanged     = true;
         theScPanel->EnableUpdate(true);
      }
   }
}
