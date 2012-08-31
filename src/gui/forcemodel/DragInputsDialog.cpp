//$Id$
//------------------------------------------------------------------------------
//                              DragDialog
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
#include "GmatStaticBoxSizer.hpp"
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
DragInputsDialog::DragInputsDialog(wxWindow *parent, Real *dragBuffer,
                                   const wxString& title)
   : GmatDialog(parent, -1, title)
{
   #ifdef DEBUG_DRAG_INPUT
   MessageInterface::ShowMessage
      ("DragInputsDialog::DragInputsDialog() entered, dragForce=<%p><%s>'%s'\n",
       dragForce, dragForce ? dragForce->GetTypeName().c_str() : "NULL",
       dragForce ? dragForce->GetName().c_str() : "NULL");
   #endif
   
   theForceData = dragBuffer;
   isTextModified = false;
   
   if (theForceData != NULL)
   {
      Create();
      ShowData();
   }
   else
   {
      MessageInterface::ShowMessage
         ("***  ERROR *** DragInputsDialog() input drag data is NULL\n");
   }
   
   #ifdef DEBUG_DRAG_INPUT
   MessageInterface::ShowMessage("DragInputsDialog::DragInputsDialog() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// ~DragInputsDialog()
//------------------------------------------------------------------------------
DragInputsDialog::~DragInputsDialog()
{
}


//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Update()
//------------------------------------------------------------------------------
void DragInputsDialog::Update()
{
   if (useFile)  
   {
//      fileNameStaticText->Enable(true);
//      fileNameTextCtrl->Enable(true);
//      browseButton->Enable(true);
      
      solarFluxStaticText->Enable(false);
      avgSolarFluxStaticText->Enable(false);
      geomagneticIndexStaticText->Enable(false);
      
      solarFluxTextCtrl->Enable(false);
      avgSolarFluxTextCtrl->Enable(false);
      geomagneticIndexTextCtrl->Enable(false); 
   }
   else
   {
//      fileNameStaticText->Enable(false);
//      fileNameTextCtrl->Enable(false);
//      browseButton->Enable(false);
      
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
   int bsize = 3; // border size
   
   //-----------------------------------------------------------------
   // user input
   //-----------------------------------------------------------------
   userInputRadioButton =
      new wxRadioButton(this, ID_RADIOBUTTON, wxT("User Input"),
                        wxDefaultPosition, wxDefaultSize, 0);
   solarFluxStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Solar Flux"),
                       wxDefaultPosition, wxDefaultSize, 0);
   solarFluxTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(150,-1), 0);
   avgSolarFluxStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Average Solar Flux"),
                       wxDefaultPosition, wxDefaultSize, 0);
   avgSolarFluxTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(150,-1), 0);
   geomagneticIndexStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Geomagnetic Index (Kp)"),
                       wxDefaultPosition, wxDefaultSize, 0);
   geomagneticIndexTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(150,-1), 0);
   
   //-----------------------------------------------------------------
   // file input
   //-----------------------------------------------------------------
//   fileInputRadioButton =
//      new wxRadioButton(this, ID_RADIOBUTTON, wxT("File Input (To be implemented)"),
//                        wxDefaultPosition, wxDefaultSize, 0);
//   //@todo Reading flux file has not been implemented in the base code
//   // so disable it for now
//   fileInputRadioButton->Disable();
//   fileNameStaticText =
//      new wxStaticText(this, ID_TEXT, wxT("File Name"),
//                       wxDefaultPosition, wxDefaultSize, 0);
//   fileNameTextCtrl =
//      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
//                     wxDefaultPosition, wxSize(200,-1), 0);
//   browseButton =
//      new wxButton(this, ID_BUTTON, wxT("Browse"),
//                   wxDefaultPosition, wxDefaultSize, 0);
   
   //-----------------------------------------------------------------
   // add to sizer
   //-----------------------------------------------------------------
   wxFlexGridSizer *userInputFlexGridSizer = new wxFlexGridSizer(2, 0, 0);
   
   userInputFlexGridSizer->Add(userInputRadioButton, 0, wxALIGN_LEFT|wxALL, bsize);
   userInputFlexGridSizer->Add(100, 0, wxALIGN_LEFT|wxALL, bsize);
   userInputFlexGridSizer->Add(solarFluxStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   userInputFlexGridSizer->Add(solarFluxTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   userInputFlexGridSizer->Add(avgSolarFluxStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   userInputFlexGridSizer->Add(avgSolarFluxTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   userInputFlexGridSizer->Add(geomagneticIndexStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   userInputFlexGridSizer->Add(geomagneticIndexTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   
//   wxBoxSizer *fileInputSizer = new wxBoxSizer(wxHORIZONTAL);
//
//   fileInputSizer->Add(fileNameStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
//   fileInputSizer->Add(fileNameTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
//   fileInputSizer->Add(browseButton, 0, wxALIGN_LEFT|wxALL, bsize);
   
   GmatStaticBoxSizer *userInputGroup = new GmatStaticBoxSizer(wxVERTICAL, this, "");
   GmatStaticBoxSizer *fileInputGroup = new GmatStaticBoxSizer(wxVERTICAL, this, "");
   
   userInputGroup->Add(userInputFlexGridSizer, 0, wxALIGN_LEFT|wxALL, bsize);
//   fileInputGroup->Add(fileInputRadioButton, 0, wxALIGN_LEFT|wxALL, bsize);
//   fileInputGroup->Add(fileInputSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   wxBoxSizer *mainPageSizer = new wxBoxSizer(wxVERTICAL);
   mainPageSizer->Add(userInputGroup, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize);
   mainPageSizer->Add(fileInputGroup, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize);
   
   theMiddleSizer->Add(mainPageSizer, 0, wxALIGN_CENTER|wxALL, bsize);
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void DragInputsDialog::LoadData()
{
   #ifdef DEBUG_DRAG_LOAD
   MessageInterface::ShowMessage("DragInputsDialog::LoadData() entered\n");
   #endif
   
   useFile = false;
   userInputRadioButton->SetValue(true);

   solarFluxTextCtrl->SetValue(wxVariant(theForceData[0]));
   avgSolarFluxTextCtrl->SetValue(wxVariant(theForceData[1]));
   geomagneticIndexTextCtrl->SetValue(wxVariant(theForceData[2]));

   Update();
   
   #ifdef DEBUG_DRAG_LOAD
   MessageInterface::ShowMessage("DragInputsDialog::LoadData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void DragInputsDialog::SaveData()
{
   #ifdef DEBUG_DRAG_SAVE
   MessageInterface::ShowMessage("DragInputsDialog::SaveData() entered\n");
   #endif
   
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
//         std::string fileName = fileNameTextCtrl->GetValue().c_str();
//         if (fileName == "")
//         {
//            MessageInterface::PopupMessage
//               (Gmat::ERROR_, "The flux file name is empty, please enter file name or "
//                "switch to user input");
//            canClose = false;
//            return;
//         }
//
//         inputSourceString = wxT("File");
//         theForce->SetStringParameter(inputSourceID, inputSourceString.c_str());
//         theForce->SetStringParameter(solarFluxFileID,
//                                      fileNameTextCtrl->GetValue().c_str());
         #ifdef DEBUG_DRAG_SAVE
//         MessageInterface::ShowMessage
//            ("   ==> Saved filename%s\n", fileNameTextCtrl->GetValue().c_str());
         #endif
      }
      else
      {
         if (isTextModified)
         {
            theForceData[0] = flux;
            theForceData[1] = avgFlux;
            theForceData[2] = magIndex;
            
            isTextModified = false;
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
   }
   
   #ifdef DEBUG_DRAG_SAVE
   MessageInterface::ShowMessage("DragInputsDialog::SaveData() leaving\n");
   #endif
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
   if (event.GetEventObject() == userInputRadioButton)  
   {       
      useFile = false;
      Update();
      EnableUpdate(true);
   }
//   else if (event.GetEventObject() == fileInputRadioButton)
//   {
//      useFile = true;
//      Update();
//      EnableUpdate(true);
//   }
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
        
//      fileNameTextCtrl->SetValue(filename);
   }
   
   EnableUpdate(true);
}

