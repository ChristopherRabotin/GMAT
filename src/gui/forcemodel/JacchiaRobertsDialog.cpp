//$Header$
//------------------------------------------------------------------------------
//                              JacchiaRobertsDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/05/18
// Modified:
/**
 * This class allows user to edit Jacchia-Roberts drag parameters.
 */
//------------------------------------------------------------------------------

#include "JacchiaRobertsDialog.hpp"
#include "MessageInterface.hpp"

#include <wx/variant.h>

//------------------------------------------------------------------------------
// Event tables and other macros for wxWindows
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(JacchiaRobertsDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, JacchiaRobertsDialog::OnBrowse)
   EVT_RADIOBUTTON(ID_RADIOBUTTON, JacchiaRobertsDialog::OnRadioButtonChange)
   EVT_TEXT(ID_TEXTCTRL, JacchiaRobertsDialog::OnTextChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// JacchiaRobertsDialog(wxWindow *parent, const wxString name, DragForce *dragForce)
//------------------------------------------------------------------------------
/**
 * Constructs JacchiaRobertsDialog object.
 *
 * @param <parent> input parent.
 * @param <dragForce> input drag force.
 *
 * @note Creates the JacchiaRoberts drag dialog
 */
//------------------------------------------------------------------------------
JacchiaRobertsDialog::JacchiaRobertsDialog(wxWindow *parent, DragForce *dragForce)
    : GmatDialog(parent, -1, wxString(_T("JacchiaRobertsDialog")))
{
    if (dragForce != NULL)    
        theForce = dragForce;
       
    Create();
    Show();
}

//------------------------------------------------------------------------------
// ~JacchiaRobertsDialog()
//------------------------------------------------------------------------------
JacchiaRobertsDialog::~JacchiaRobertsDialog()
{
}

//-------------------------------
// public methods
//-------------------------------

//------------------------------------------------------------------------------
// void GetForce()
//------------------------------------------------------------------------------
DragForce* JacchiaRobertsDialog::GetForce()
{
   return theForce;
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
void JacchiaRobertsDialog::Initialize()
{  
    if (theForce == NULL)
        theForce->Initialize();
        
    useFile = false;
}

//------------------------------------------------------------------------------
// void Update()
//------------------------------------------------------------------------------
void JacchiaRobertsDialog::Update()
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
void JacchiaRobertsDialog::Create()
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
void JacchiaRobertsDialog::LoadData()
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
void JacchiaRobertsDialog::SaveData()
{
    if (theOkButton->IsEnabled())
    {
        if (useFile)
        {
           inputSourceString = wxT("File");
           theForce->SetStringParameter(inputSourceID, inputSourceString.c_str());
           MessageInterface::ShowMessage("Saved input source string\n");
           theForce->SetStringParameter(solarFluxFileID, fileNameTextCtrl->GetValue().c_str() );
           MessageInterface::ShowMessage("Saved filename%s\n", fileNameTextCtrl->GetValue().c_str());
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
void JacchiaRobertsDialog::ResetData()
{

}

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void JacchiaRobertsDialog::OnTextChange()
{
    theOkButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnRadioButtonChange()
//------------------------------------------------------------------------------
void JacchiaRobertsDialog::OnRadioButtonChange(wxCommandEvent& event)
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
void JacchiaRobertsDialog::OnBrowse()
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

