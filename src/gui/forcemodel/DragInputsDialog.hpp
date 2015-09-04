//$Id$
//------------------------------------------------------------------------------
//                              DragInputsDialog
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Waka Waktola
// Created: 2004/04/06
// Modified:
/**
 * This class allows user to edit Jacchia-Roberts drag parameters.
 */
//------------------------------------------------------------------------------

#ifndef DragInputsDialog_hpp
#define DragInputsDialog_hpp

#include "GmatDialog.hpp"
#include "DragForce.hpp"

class DragInputsDialog : public GmatDialog
{
public:
   
   DragInputsDialog(wxWindow *parent, Real *dragBuffer, std::vector<std::string> * dragStringBuffer, const wxString &title = "");
   virtual ~DragInputsDialog();
   
private:
   
   bool isTextModified;
   void Update();
   
   wxStaticText *solarFluxStaticText;
   wxStaticText *avgSolarFluxStaticText;
   wxStaticText *geomagneticIndexStaticText;
   wxStaticText *fileNameStaticText;
   
   wxStaticText *historicIndexStaticText;
   wxStaticText *predictedIndexStaticText;

   wxTextCtrl *cssiFileTextCtrl;
   wxTextCtrl *schattenFileTextCtrl;
 
   wxStaticText *cssiSpaceWeatherStaticText;
   wxStaticText *schattenFileStaticText;
   wxStaticText *schattenErrorStaticText;
   wxStaticText *schattenTimingStaticText;

   wxTextCtrl *solarFluxTextCtrl;
   wxTextCtrl *avgSolarFluxTextCtrl;
   wxTextCtrl *geomagneticIndexTextCtrl;
   wxTextCtrl *fileNameTextCtrl;
   
   wxButton *browseButton;
   
   wxRadioButton *userConstantInputRadioButton;
   wxRadioButton *userFileInputRadioButton;
   
   Real *theForceData;
   std::vector<std::string> *  theForceStringArray;   

   wxString inputSourceString;
    
   wxComboBox * historicFileComboBox;
   wxComboBox * predictedFileComboBox;
   wxComboBox * schattenErrorComboBox;
   wxComboBox * schattenTimingComboBox;

   wxBitmapButton *cssiFileButton;
   wxBitmapButton *schattenFileButton;

   wxArrayString historicInputsArray;
   wxArrayString predictedInputsArray;
   wxArrayString schattentimingArray;
   wxArrayString schattenErrorArray;

   std::string predictedModelString;
   std::string historicModelString;
   // Methods inherited from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   // Event-handling Methods
   void OnTextChange(wxCommandEvent &event);
   void OnBrowse(wxCommandEvent &event);
   void OnModelComboChange(wxCommandEvent &event);
   void OnCSSIFileSelect(wxCommandEvent &event);
   void OnSchattenFileSelect(wxCommandEvent &event);

   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 46000,
      ID_TEXTCTRL,
      ID_BUTTON,
      ID_CONSTANT_RADIO,
      ID_HISTORIC,
      ID_MODEL,
      ID_BUTTON_CSSI,
      ID_BUTTON_SCHATTEN,
      ID_FILE_RADIO,
      ID_TEXTCTRL_CSSI,
      ID_TEXTCTRL_SCHATTEN,
      ID_ERROR_SCHATTEN,
      ID_TIMING_SCHATTEN
   };
};

#endif

