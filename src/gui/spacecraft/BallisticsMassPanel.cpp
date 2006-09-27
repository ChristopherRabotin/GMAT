//$Header$
//------------------------------------------------------------------------------
//                            BallisticsMassPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
//
// **Legal**
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
 * @param <parent> input parent.
 *
 * @note Creates the Universe GUI
 */
//------------------------------------------------------------------------------
BallisticsMassPanel::BallisticsMassPanel(wxWindow *parent,
                                         Spacecraft *spacecraft,
                                         wxButton *applyButton,
                                         wxButton *okButton)
   : wxPanel(parent)
{
    this->theSpacecraft = spacecraft;
    this->theApplyButton = applyButton;
    this->theOkButton = okButton;
    
    canClose = true;
    dataChanged = false;
    
    Create();
}

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
 *
 * @note Creates the page for ballistics and mass information
 */
//------------------------------------------------------------------------------
void BallisticsMassPanel::Create()
{
    wxStaticText *emptyText = new wxStaticText( this, ID_TEXT,
                            wxT(""),
                            wxDefaultPosition, wxDefaultSize, 0 );

    wxStaticBox *item1 = new wxStaticBox( this, -1, wxT("") );
    wxStaticBoxSizer *item0 = new wxStaticBoxSizer( item1, wxVERTICAL );

    wxFlexGridSizer *item2 = new wxFlexGridSizer( 3, 0, 0 );

    wxStaticText *dryMassStaticText = new wxStaticText( this, ID_TEXT, 
                            wxT("Dry Mass"), 
                            wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( dryMassStaticText, 0, wxALIGN_CENTER|wxALL, 5 );

    dryMassTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( dryMassTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *dryMassUnitsText = new wxStaticText( this, ID_TEXT,
                            wxT("Kg"),
                            wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( dryMassUnitsText, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *dragCoeffStaticText = new wxStaticText( this, ID_TEXT, 
                            wxT("Coefficient of Drag"), wxDefaultPosition, 
                            wxDefaultSize, 0 );
    item2->Add( dragCoeffStaticText, 0, wxALIGN_CENTER|wxALL, 5 );

    dragCoeffTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( dragCoeffTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    item2->Add( emptyText, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *reflectCoeffStaticText = new wxStaticText( this, ID_TEXT, 
                            wxT("Coefficient of Reflectivity"), 
                            wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( reflectCoeffStaticText, 0, wxALIGN_CENTER|wxALL, 5 );

    reflectCoeffTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( reflectCoeffTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    item2->Add( emptyText, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *dragAreaStaticText = new wxStaticText( this, ID_TEXT, 
                            wxT("Drag Area"), 
                            wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( dragAreaStaticText, 0, wxALIGN_CENTER|wxALL, 5 );

    dragAreaTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( dragAreaTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    wxStaticText *dragAreaUnitsText = new wxStaticText( this, ID_TEXT,
                            wxT("m^2"),
                            wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( dragAreaUnitsText, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *srpAreaStaticText = new wxStaticText( this, ID_TEXT, 
                            wxT("SRP Area"), 
                            wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( srpAreaStaticText, 0, wxALIGN_CENTER|wxALL, 5 );

    srpAreaTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( srpAreaTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    wxStaticText *srpAreaUnitsText = new wxStaticText( this, ID_TEXT,
                            wxT("m^2"),
                            wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( srpAreaUnitsText, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );

    this->SetAutoLayout( TRUE );  
    this->SetSizer( item0 );
    item0->Fit( this );
    item0->SetSizeHints( this );
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void BallisticsMassPanel::LoadData()
{
   try
   {
      int dryMassID = theSpacecraft->GetParameterID("DryMass");
      int coeffDragID = theSpacecraft->GetParameterID("Cd");
      int dragAreaID = theSpacecraft->GetParameterID("DragArea");
      int reflectCoeffID = theSpacecraft->GetParameterID("Cr");
      int srpAreaID = theSpacecraft->GetParameterID("SRPArea");
      
      Real mass = theSpacecraft->GetRealParameter(dryMassID);
      Real dragCoeff = theSpacecraft->GetRealParameter(coeffDragID);
      Real dragArea = theSpacecraft->GetRealParameter(dragAreaID);
      Real reflectCoeff = theSpacecraft->GetRealParameter(reflectCoeffID);
      Real srpArea = theSpacecraft->GetRealParameter(srpAreaID);
      
      GuiItemManager *theGuiManager = GuiItemManager::GetInstance();
      
      dryMassTextCtrl->SetValue(theGuiManager->ToWxString(mass));
      dragCoeffTextCtrl->SetValue(theGuiManager->ToWxString(dragCoeff));
      reflectCoeffTextCtrl->SetValue(theGuiManager->ToWxString(reflectCoeff));
      dragAreaTextCtrl->SetValue(theGuiManager->ToWxString(dragArea));
      srpAreaTextCtrl->SetValue(theGuiManager->ToWxString(srpArea));
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage(e.GetMessage());
   }
   
   dataChanged = false;
   
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void BallisticsMassPanel::SaveData()
{
    canClose = true;
    dataChanged = false;
    
    int dryMassID = theSpacecraft->GetParameterID("DryMass");
    int coeffDragID = theSpacecraft->GetParameterID("Cd");
    int dragAreaID = theSpacecraft->GetParameterID("DragArea");
    int reflectCoeffID = theSpacecraft->GetParameterID("Cr");
    int srpAreaID = theSpacecraft->GetParameterID("SRPArea");

    wxString massStr = dryMassTextCtrl->GetValue();
    wxString dragCoeffStr = dragCoeffTextCtrl->GetValue();
    wxString dragAreaStr = dragAreaTextCtrl->GetValue();
    wxString srpAreaStr = srpAreaTextCtrl->GetValue();
    wxString reflectCoeffStr = reflectCoeffTextCtrl->GetValue();
    
    if (atof(massStr) < 0)
    {
       MessageInterface::PopupMessage
         (Gmat::WARNING_, "Mass can not be negative");
       canClose=false;
       return;
    }

    if (atof(srpAreaStr) < 0)
    {
       MessageInterface::PopupMessage
         (Gmat::WARNING_, "SRP Area can not be negative");
       canClose=false;
       return;
    }
    
    if (atof(dragAreaStr) < 0)
    {
       MessageInterface::PopupMessage
         (Gmat::WARNING_, "Drag Area can not be negative");
       canClose=false;
       return;
    }

    theSpacecraft->SetRealParameter(dryMassID, atof(massStr));
    theSpacecraft->SetRealParameter(coeffDragID, atof(dragCoeffStr));
    theSpacecraft->SetRealParameter(dragAreaID, atof(dragAreaStr));
    theSpacecraft->SetRealParameter(srpAreaID, atof(srpAreaStr));
    theSpacecraft->SetRealParameter(reflectCoeffID, atof(reflectCoeffStr));
}


//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void BallisticsMassPanel::OnTextChange(wxCommandEvent &event)
{
   if (dryMassTextCtrl->IsModified() || dragCoeffTextCtrl->IsModified() ||
       dragAreaTextCtrl->IsModified() || srpAreaTextCtrl->IsModified() ||
       reflectCoeffTextCtrl->IsModified())
   {
      dataChanged = true;
      theApplyButton->Enable();
      theOkButton->Enable();
   }
}
