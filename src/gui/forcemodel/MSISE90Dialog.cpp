//$Header$ /cygdrive/f/dev/cvs/gui/forcemodel/MSISE90Dialog.cpp,v 1.5 2004/05/11 14:45:28 uid500 Exp $
//------------------------------------------------------------------------------
//                              MSISE90Dialog
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
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, MSISE90Dialog::OnBrowse)
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
 * @param <dragForce> input drag force.
 *
 * @note Creates the MSISE90 drag dialog
 */
//------------------------------------------------------------------------------
MSISE90Dialog::MSISE90Dialog(wxWindow *parent, DragForce *dragForce)
    : GmatDialog(parent, -1, wxString(_T("MSISE90DragDialog")))
{
    if (dragForce != NULL)    
        theForce = dragForce;
       
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
// public methods
//-------------------------------

//------------------------------------------------------------------------------
// void GetForce()
//------------------------------------------------------------------------------
DragForce* MSISE90Dialog::GetForce()
{
   return theForce;
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
void MSISE90Dialog::Initialize()
{  
    if (theForce == NULL)
    {
        MessageInterface::ShowMessage("Error: The MSISE90 Drag Force is NULL.\n");
        Close();
    }
        
    useFile = false;
}

//------------------------------------------------------------------------------
// void Update()
//------------------------------------------------------------------------------
void MSISE90Dialog::Update()
{
    if (useFile)  
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
    }
    else
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
    }
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
    solarFluxTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
    avgSolarFluxTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
    geomagneticIndexTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
    fileNameTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    
    // wxButton
    browseButton = new wxButton( this, ID_BUTTON, wxT("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxRadioButton
    userInputRadioButton = new wxRadioButton(this, ID_RADIOBUTTON, wxT("User Input"), wxDefaultPosition, wxDefaultSize, 0 );
    fileInputRadioButton = new wxRadioButton(this, ID_RADIOBUTTON, wxT("File Input"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxSizer
    wxBoxSizer *mainPageSizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer *userInputFlexGridSizer = new wxFlexGridSizer( 3, 0, 0 );
    wxFlexGridSizer *fileInputPageFlexGridSizer = new wxFlexGridSizer( 3, 0, 0 );
    
    userInputFlexGridSizer->AddGrowableCol(1);
    fileInputPageFlexGridSizer->AddGrowableCol(1);

    userInputFlexGridSizer->Add( userInputRadioButton, 0, wxALIGN_LEFT|wxALL, 5 );
    userInputFlexGridSizer->Add( 100, 0, wxALIGN_CENTER|wxALL, 5 );
    userInputFlexGridSizer->Add( 100, 0, wxALIGN_CENTER|wxALL, 5 );
    userInputFlexGridSizer->Add( solarFluxStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    userInputFlexGridSizer->Add( solarFluxTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    userInputFlexGridSizer->Add( 20, 0, wxALIGN_CENTER|wxALL, 5 );
    userInputFlexGridSizer->Add( avgSolarFluxStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    userInputFlexGridSizer->Add( avgSolarFluxTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    userInputFlexGridSizer->Add( 20, 0, wxALIGN_CENTER|wxALL, 5 );
    userInputFlexGridSizer->Add( geomagneticIndexStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    userInputFlexGridSizer->Add( geomagneticIndexTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    userInputFlexGridSizer->Add( 20, 0, wxALIGN_CENTER|wxALL, 5 );

    fileInputPageFlexGridSizer->Add( fileInputRadioButton, 0, wxALIGN_CENTER|wxALL, 5 );
    fileInputPageFlexGridSizer->Add( 20, 0, wxALIGN_CENTER|wxALL, 5 );
    fileInputPageFlexGridSizer->Add( 20, 0, wxALIGN_CENTER|wxALL, 5 );
    fileInputPageFlexGridSizer->Add( fileNameStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    fileInputPageFlexGridSizer->Add( fileNameTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    fileInputPageFlexGridSizer->Add( browseButton, 0, wxALIGN_CENTER|wxALL, 5 );
    
    mainPageSizer->Add(userInputFlexGridSizer, 0, wxALIGN_CENTER|wxALL, 5);
    mainPageSizer->Add(fileInputPageFlexGridSizer, 0, wxALIGN_CENTER|wxALL, 5);
    
    theMiddleSizer->Add(mainPageSizer, 0, wxALIGN_CENTER|wxALL, 5);
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void MSISE90Dialog::LoadData()
{   
    Initialize();
    
    try
    {
       solarFluxID = theForce->GetParameterID("F107");
       solarFluxTextCtrl->
          SetValue(wxVariant(theForce->GetRealParameter(solarFluxID)));
    }
    catch (BaseException &e)
    {
       MessageInterface::ShowMessage("MSISE90Dialog::LoadData()\n" +
          e.GetMessage()); 
    }
    try
    {
       avgSolarFluxID = theForce->GetParameterID("F107A");
       avgSolarFluxTextCtrl->
          SetValue(wxVariant(theForce->GetRealParameter(avgSolarFluxID)));
    }
    catch (BaseException &e)
    {
       MessageInterface::ShowMessage("MSISE90Dialog::LoadData()\n" +
          e.GetMessage()); 
    }
    try
    {
       geomagnecticIndexID = theForce->GetParameterID("MagneticIndex");
       geomagneticIndexTextCtrl->
          SetValue(wxVariant(theForce->GetRealParameter(geomagnecticIndexID)));
    }
    catch (BaseException &e)
    {
       MessageInterface::ShowMessage("MSISE90Dialog::LoadData()\n" +
          e.GetMessage()); 
    }
    try
    {
       solarFluxFileID = theForce->GetParameterID("SolarFluxFile");
       wxString filename = theForce->GetStringParameter(solarFluxFileID).c_str();

       if (!filename.IsNull())
          fileNameTextCtrl->SetValue(filename);
    }
    catch (BaseException &e)
    {
       MessageInterface::ShowMessage("MSISE90Dialog::LoadData()\n" +
          e.GetMessage()); 
    }    
    try
    {
       inputSourceID = theForce->GetParameterID("InputSource");
       inputSourceString = theForce->GetStringParameter(inputSourceID).c_str();
    }
    catch (BaseException &e)
    {
       MessageInterface::ShowMessage("MSISE90Dialog::LoadData()\n" +
          e.GetMessage()); 
    }       
    
    if ( inputSourceString.CmpNoCase("Constant") == 0 )
    {
       useFile = false;
       userInputRadioButton->SetValue(true);
       fileInputRadioButton->SetValue(false); 
    }
    else if ( inputSourceString.CmpNoCase("File") == 0 )
    {
       useFile = true;
       userInputRadioButton->SetValue(false);
       fileInputRadioButton->SetValue(true);
    }
    
    Update();
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void MSISE90Dialog::SaveData()
{
    if (theOkButton->IsEnabled())
    {
        if (useFile)
        {
           inputSourceString = wxT("File");
           theForce->SetStringParameter(inputSourceID, inputSourceString.c_str());
           theForce->SetStringParameter(solarFluxFileID, fileNameTextCtrl->GetValue().c_str() );
        }
        else
        {
           inputSourceString = wxT("Constant");
           theForce->SetStringParameter(inputSourceID, inputSourceString.c_str());
           theForce->SetRealParameter(solarFluxID, atof(solarFluxTextCtrl->GetValue()) );
           theForce->SetRealParameter(avgSolarFluxID, atof(avgSolarFluxTextCtrl->GetValue()) );
           theForce->SetRealParameter(geomagnecticIndexID, atof(geomagneticIndexTextCtrl->GetValue()) );         
        }
    }       
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
    theOkButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnRadioButtonChange()
//------------------------------------------------------------------------------
void MSISE90Dialog::OnRadioButtonChange(wxCommandEvent& event)
{
    if ( event.GetEventObject() == userInputRadioButton )  
    {       
        useFile = false;
        Update();
        theOkButton->Enable(true);
    }
    else if ( event.GetEventObject() == fileInputRadioButton )
    { 
        
        useFile = true;
        Update();
        theOkButton->Enable(true);
    }
}

//------------------------------------------------------------------------------
// void OnBrowse()
//------------------------------------------------------------------------------
void MSISE90Dialog::OnBrowse()
{
    wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
    
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString filename;
        
        filename = dialog.GetPath().c_str();
        
        fileNameTextCtrl->SetValue(filename); 
    }
    theOkButton->Enable(true);
}

