//$Header$
//------------------------------------------------------------------------------
//                              DragDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun (NASA/GSFC)
// Created: 2007/01/19
//
/**
 * This class allows user to edit Jacchia-Roberts or MSISE90 drag parameters.
 */
//------------------------------------------------------------------------------

#include "DragInputsDialog.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"          // for ToReal()
#include <wx/variant.h>

//------------------------------------------------------------------------------
// Event tables and other macros for wxWindows
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(DragInputsDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON, DragInputsDialog::OnBrowse)
   EVT_RADIOBUTTON(ID_RADIOBUTTON, DragInputsDialog::OnRadioButtonChange)
   EVT_TEXT(ID_TEXTCTRL, DragInputsDialog::OnTextChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// DragInputsDialog(wxWindow *parent, DragForce *dragForce,
//                  const wxString& title = "")
//------------------------------------------------------------------------------
/**
 * Constructs DragInputsDialog object.
 *
 * @param  parent  Parent window
 * @param  dragForce  Input drag force
 * @param  title  Title of the dialog
 *
 * @note Creates the JacchiaRoberts drag dialog
 */
//------------------------------------------------------------------------------
DragInputsDialog::DragInputsDialog(wxWindow *parent, DragForce *dragForce,
                                   const wxString& title)
   : GmatDialog(parent, -1, title)
{
   mObject = (GmatBase*)dragForce;   
   theForce = dragForce;
   isTextModified = false;
   
   if (theForce != NULL)
   {
      Create();
      ShowData();
   }
   else
   {
      MessageInterface::ShowMessage
         ("***  ERROR *** DragInputsDialog() input drag force is NULL\n");
   }
}


//------------------------------------------------------------------------------
// ~DragInputsDialog()
//------------------------------------------------------------------------------
DragInputsDialog::~DragInputsDialog()
{
}


//------------------------------------------------------------------------------
// void GetForce()
//------------------------------------------------------------------------------
DragForce* DragInputsDialog::GetForce()
{
   return theForce;
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
void DragInputsDialog::Initialize()
{  
   if (theForce != NULL)
      theForce->Initialize();
   
   useFile = false;
}

//------------------------------------------------------------------------------
// void Update()
//------------------------------------------------------------------------------
void DragInputsDialog::Update()
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
void DragInputsDialog::Create()
{
   // wxStaticText
   solarFluxStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Solar Flux"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   avgSolarFluxStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Average Solar Flux"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   geomagneticIndexStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Geomagnetic Index (Kp)"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   fileNameStaticText =
      new wxStaticText( this, ID_TEXT, wxT("File Name"),
                        wxDefaultPosition, wxDefaultSize, 0 );
    
   // wxTextCtrl
   solarFluxTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(150,-1), 0 );
   avgSolarFluxTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(150,-1), 0 );
   geomagneticIndexTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(150,-1), 0 );
   fileNameTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(200,-1), 0 );
    
   // wxButton
   browseButton =
      new wxButton( this, ID_BUTTON, wxT("Browse"),
                    wxDefaultPosition, wxDefaultSize, 0 );
    
   // wxRadioButton
   userInputRadioButton =
      new wxRadioButton(this, ID_RADIOBUTTON, wxT("User Input"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   fileInputRadioButton =
      new wxRadioButton(this, ID_RADIOBUTTON, wxT("File Input"),
                        wxDefaultPosition, wxDefaultSize, 0 );
    
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
void DragInputsDialog::LoadData()
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
      MessageInterface::ShowMessage("DragInputsDialog::LoadData()\n" +
                                    e.GetFullMessage()); 
   }
   
   try
   {
      avgSolarFluxID = theForce->GetParameterID("F107A");
      avgSolarFluxTextCtrl->
         SetValue(wxVariant(theForce->GetRealParameter(avgSolarFluxID)));
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage("DragInputsDialog::LoadData()\n" +
                                    e.GetFullMessage()); 
   }
   
   try
   {
      geomagnecticIndexID = theForce->GetParameterID("MagneticIndex");
      geomagneticIndexTextCtrl->
         SetValue(wxVariant(theForce->GetRealParameter(geomagnecticIndexID)));
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage("DragInputsDialog::LoadData()\n" +
                                    e.GetFullMessage()); 
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
      MessageInterface::ShowMessage("DragInputsDialog::LoadData()\n" +
                                    e.GetFullMessage()); 
   }
   
   try
   {
      inputSourceID = theForce->GetParameterID("InputSource");
      inputSourceString = theForce->GetStringParameter(inputSourceID).c_str();
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage("DragInputsDialog::LoadData()\n" +
                                    e.GetFullMessage()); 
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
void DragInputsDialog::SaveData()
{
   canClose = true;
   
   Real flux, avgFlux, magIndex;
      
   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   if (isTextModified)
   {
      std::string str;
         
      str = solarFluxTextCtrl->GetValue();
      CheckReal(flux, str, "Solar Flux (F10.7)", "Real Number >= 0.0");
         
      str = avgSolarFluxTextCtrl->GetValue();
      CheckReal(avgFlux, str, "Average Solar Flux (F10.7A)", "Real Number >= 0.0");
         
      str = geomagneticIndexTextCtrl->GetValue();
      CheckReal(magIndex, str, "Magnetic Index)", "0.0 <= Real Number <= 9.0");
         
      if (!canClose)
         return;
      
   }
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      if (useFile)
      {
         inputSourceString = wxT("File");
         theForce->SetStringParameter(inputSourceID, inputSourceString.c_str());
         theForce->SetStringParameter(solarFluxFileID,
                                      fileNameTextCtrl->GetValue().c_str() );
         
         //MessageInterface::ShowMessage("Saved filename%s\n",
         //   fileNameTextCtrl->GetValue().c_str());
      }
      else
      {
         if (isTextModified)
         {
            inputSourceString = wxT("Constant");
            theForce->SetStringParameter(inputSourceID, inputSourceString.c_str());
            
            theForce->SetRealParameter(solarFluxID, flux);
            theForce->SetRealParameter(avgSolarFluxID, avgFlux);
            theForce->SetRealParameter(geomagnecticIndexID, magIndex);
            
            isTextModified = false;
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
   }

}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void DragInputsDialog::ResetData()
{
}


//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void DragInputsDialog::OnTextChange(wxCommandEvent &event)
{
   if (((wxTextCtrl*)event.GetEventObject())->IsModified())
   {
      EnableUpdate(true);
      isTextModified = true;
   }
}


//------------------------------------------------------------------------------
// void OnRadioButtonChange()
//------------------------------------------------------------------------------
void DragInputsDialog::OnRadioButtonChange(wxCommandEvent& event)
{
   if ( event.GetEventObject() == userInputRadioButton )  
   {       
      useFile = false;
      Update();
      EnableUpdate(true);
   }
   else if ( event.GetEventObject() == fileInputRadioButton )
   {       
      useFile = true;
      Update();
      EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnBrowse()
//------------------------------------------------------------------------------
void DragInputsDialog::OnBrowse(wxCommandEvent &event)
{
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
    
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString filename;
        
      filename = dialog.GetPath().c_str();
        
      fileNameTextCtrl->SetValue(filename); 
   }
   
   EnableUpdate(true);
}

