//$Header:
//------------------------------------------------------------------------------
//                              MSISE90Panel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/04/06
// Modified:
/**
 * This class allows user to edit MSISE90 drag parameters.
 */
//------------------------------------------------------------------------------

#include "MSISE90Dialog.hpp"
#include "MessageInterface.hpp"

#include <wx/variant.h>

//------------------------------------------------------------------------------
// Event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MSISE90Dialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatDialog::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_RADIOBUTTON(ID_RADIOBUTTON, MSISE90Dialog::OnRadioButtonChange)
   EVT_TEXT(ID_TEXTCTRL, MSISE90Dialog::OnTextChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// MSISE90Dialog(wxWindow *parent, const wxString name, DragForce *dragForce)
//------------------------------------------------------------------------------
/**
 * Constructs MSISE90 object.
 *
 * @param <parent> input parent.
 * @param <name> input drag name.
 *
 * @note Creates the MSISE90 drag panel
 */
//------------------------------------------------------------------------------
MSISE90Dialog::MSISE90Dialog(wxWindow *parent, wxString name, DragForce *dragForce)
    : GmatDialog(parent, -1, wxString(_T("MSISE90DragDialog")))
{
//    if (dragForce != NULL)    
//        theDragForce = dragForce;
       
    Create();
    Show();
}

//------------------------------------------------------------------------------
// ~MSISE90()
//------------------------------------------------------------------------------
MSISE90Dialog::~MSISE90Dialog()
{
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
void MSISE90Dialog::Initialize()
{  
//    if (theDragForce == NULL)
//        theDragForce = new DragForce();
}

//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void MSISE90Dialog::Create()
{
    // wxStaticText
    solarFluxStaticText = new wxStaticText( this, ID_TEXT, wxT("Solar Flux"), wxDefaultPosition, wxDefaultSize, 0 );
    avgSolarFluxStaticText = new wxStaticText( this, ID_TEXT, wxT("Average Solar Flux"), wxDefaultPosition, wxDefaultSize, 0 );
    geomagneticIndexStaticText = new wxStaticText( this, ID_TEXT, wxT("Geomagnetic Index"), wxDefaultPosition, wxDefaultSize, 0 );
    fileNameStaticText = new wxStaticText( this, ID_TEXT, wxT("File Name"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxTextCtrl
    solarFluxTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    avgSolarFluxTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    geomagneticIndexTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    fileNameTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(160,-1), 0 );
    
    // wxButton
    browseButton = new wxButton( this, ID_BUTTON, wxT("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxRadioButton
    userInputRadioButton = new wxRadioButton(this, ID_RADIOBUTTON, wxT("User Input"), wxDefaultPosition, wxDefaultSize, 0 );
    fileInputRadioButton = new wxRadioButton(this, ID_RADIOBUTTON, wxT("File Input"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxSizer
    wxFlexGridSizer *pageFlexGridSizer = new wxFlexGridSizer( 2, 0, 0 );
    wxFlexGridSizer *innerPageFlexGridSizer = new wxFlexGridSizer( 3, 0, 0 );

    pageFlexGridSizer->Add( userInputRadioButton, 0, wxALIGN_LEFT|wxALL, 5 );
    pageFlexGridSizer->Add( 20, 0, wxALIGN_CENTER|wxALL, 5 );
    pageFlexGridSizer->Add( solarFluxStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    pageFlexGridSizer->Add( solarFluxTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    pageFlexGridSizer->Add( avgSolarFluxStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    pageFlexGridSizer->Add( avgSolarFluxTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    pageFlexGridSizer->Add( geomagneticIndexStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    pageFlexGridSizer->Add( geomagneticIndexTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    pageFlexGridSizer->Add( innerPageFlexGridSizer, 0, wxALIGN_CENTER|wxALL, 5 );
    pageFlexGridSizer->Add( 20, 0, wxALIGN_CENTER|wxALL, 5 );
    
    innerPageFlexGridSizer->Add( fileInputRadioButton, 0, wxALIGN_CENTER|wxALL, 5 );
    innerPageFlexGridSizer->Add( 20, 0, wxALIGN_CENTER|wxALL, 5 );
    innerPageFlexGridSizer->Add( 20, 0, wxALIGN_CENTER|wxALL, 5 );
    innerPageFlexGridSizer->Add( fileNameStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    innerPageFlexGridSizer->Add( fileNameTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    innerPageFlexGridSizer->Add( browseButton, 0, wxALIGN_CENTER|wxALL, 5 );
    
    theMiddleSizer->Add(pageFlexGridSizer, 0, wxALIGN_CENTER|wxALL, 5);
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void MSISE90Dialog::LoadData()
{
//    solarFluxID = theDragForce->GetParameterID("SolarFlux");
//    avgSolarFluxID = theDragForce->GetParameterID("AverageSolarFlux");
//    geomagnecticIndexID = theDragForce->GetParameterID("MagneticIndex");
//    solarFluxFileID = theDragForce->GetParameterID("SolarFluxFile");
//    
//    solarFluxTextCtrl->SetValue(wxVariant(theDragForce->GetRealParameter(solarFluxID)));
//    avgSolarFluxTextCtrl->SetValue(wxVariant(theDragForce->GetRealParameter(avgSolarFluxID)));
//    geomagneticIndexTextCtrl->SetValue(wxVariant(theDragForce->GetRealParameter(geomagnecticIndexID)));
//    fileNameTextCtrl->SetValue(theDragForce->GetStringParameter(solarFluxFileID).c_str());
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void MSISE90Dialog::SaveData()
{
//    if (applyButton->IsEnabled())
//        ;
}

//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void MSISE90Dialog::ResetData()
{

}

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void MSISE90Dialog::OnTextChange()
{
//    applyButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnRadioButtonChange()
//------------------------------------------------------------------------------
void MSISE90Dialog::OnRadioButtonChange(wxCommandEvent& event)
{
    if ( event.GetEventObject() == userInputRadioButton )  
    {
        fileNameStaticText->Enable(false);
        fileNameTextCtrl->Enable(false);
        browseButton->Enable(false);
        
        solarFluxStaticText->Enable(true);
        avgSolarFluxStaticText->Enable(true);
        geomagneticIndexStaticText->Enable(true);
    
        solarFluxTextCtrl->Enable(true);
        avgSolarFluxTextCtrl->Enable(true);
        geomagneticIndexTextCtrl->Enable(true);
        
//        applyButton->Enable(true);
    }
    else if ( event.GetEventObject() == fileInputRadioButton )
    {
        fileNameStaticText->Enable(true);
        fileNameTextCtrl->Enable(true);
        browseButton->Enable(true);
        
        solarFluxStaticText->Enable(false);
        avgSolarFluxStaticText->Enable(false);
        geomagneticIndexStaticText->Enable(false);
        
        solarFluxTextCtrl->Enable(false);
        avgSolarFluxTextCtrl->Enable(false);
        geomagneticIndexTextCtrl->Enable(false); 
        
//        applyButton->Enable(true);
    }
}

