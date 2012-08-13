//$Id$
//------------------------------------------------------------------------------
//                            BallisticsMassPanel
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
// Author: Allison Greene
// Created: 2004/04/01
/**
 * This class contains information needed to setup users spacecraft ballistics
 * and mass through GUI
 * 
 */
//------------------------------------------------------------------------------
#include "BallisticsMassPanel.hpp"
#include "GuiItemManager.hpp"
#include "MessageInterface.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "StringUtil.hpp"  // for ToReal()
#include <wx/config.h>

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(BallisticsMassPanel, wxPanel)
   EVT_TEXT(ID_TEXTCTRL, BallisticsMassPanel::OnTextChange)
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
   
   canClose = true;
   dataChanged = false;
   
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
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Spacecraft Ballistic Mass"));

    wxStaticText *emptyText = new wxStaticText( this, ID_TEXT,
                            wxT(""),
                            wxDefaultPosition, wxDefaultSize, 0 );

    wxStaticBox *item1 = new wxStaticBox( this, -1, wxT("") );
    wxStaticBoxSizer *item0 = new wxStaticBoxSizer( item1, wxVERTICAL );
    GmatStaticBoxSizer *optionsSizer = new GmatStaticBoxSizer( wxVERTICAL, this, "Options" );
    item0->Add(optionsSizer, 1, wxALIGN_LEFT|wxGROW);
    wxFlexGridSizer *item2 = new wxFlexGridSizer( 3, 0, 0 );
    item2->AddGrowableCol(1);

    wxStaticText *dryMassStaticText = new wxStaticText( this, ID_TEXT, 
                            wxT("Dry "GUI_ACCEL_KEY"Mass"), 
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
                            wxT("Coefficient of "GUI_ACCEL_KEY"Drag"), wxDefaultPosition, 
                            wxDefaultSize, 0 );
    item2->Add( dragCoeffStaticText, 0, wxALIGN_LEFT|wxALL, 5 );

    dragCoeffTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
    dragCoeffTextCtrl->SetToolTip(pConfig->Read(_T("DragCoefficientHint")));
    item2->Add( dragCoeffTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    item2->Add( emptyText, 0, wxALIGN_LEFT|wxALL, 5 );

    wxStaticText *reflectCoeffStaticText = new wxStaticText( this, ID_TEXT, 
                            wxT("Coefficient of "GUI_ACCEL_KEY"Reflectivity"), 
                            wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( reflectCoeffStaticText, 0, wxALIGN_LEFT|wxALL, 5 );

    reflectCoeffTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
    reflectCoeffTextCtrl->SetToolTip(pConfig->Read(_T("ReflectivityCoefficientHint")));
    item2->Add( reflectCoeffTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    item2->Add( emptyText, 0, wxALIGN_LEFT|wxALL, 5 );

    wxStaticText *dragAreaStaticText = new wxStaticText( this, ID_TEXT, 
                            wxT("Drag "GUI_ACCEL_KEY"Area"), 
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
                            wxT(GUI_ACCEL_KEY"SRP Area"), 
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

    optionsSizer->Add( item2, 0, wxALIGN_LEFT|wxALL, 5 );

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
      
      Real mass         = theSpacecraft->GetRealParameter(dryMassID);
      Real dragCoeff    = theSpacecraft->GetRealParameter(coeffDragID);
      Real dragArea     = theSpacecraft->GetRealParameter(dragAreaID);
      Real reflectCoeff = theSpacecraft->GetRealParameter(reflectCoeffID);
      Real srpArea      = theSpacecraft->GetRealParameter(srpAreaID);
      
      GuiItemManager *theGuiManager = GuiItemManager::GetInstance();
      
      dryMassTextCtrl->SetValue(theGuiManager->ToWxString(mass));
      dragCoeffTextCtrl->SetValue(theGuiManager->ToWxString(dragCoeff));
      dragAreaTextCtrl->SetValue(theGuiManager->ToWxString(dragArea));
      reflectCoeffTextCtrl->SetValue(theGuiManager->ToWxString(reflectCoeff));
      srpAreaTextCtrl->SetValue(theGuiManager->ToWxString(srpArea));
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
       reflectCoeffTextCtrl->IsModified())
   {
      dataChanged = true;
      theScPanel->EnableUpdate(true);
   }
}
