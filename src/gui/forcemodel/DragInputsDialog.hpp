//$Id$
//------------------------------------------------------------------------------
//                              DragInputsDialog
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
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
   
   DragInputsDialog(wxWindow *parent, Real *dragBuffer, const wxString &title = "");
   virtual ~DragInputsDialog();
   
private:
   
   bool isTextModified;
   bool useFile;
   
   void Update();
   
   wxStaticText *solarFluxStaticText;
   wxStaticText *avgSolarFluxStaticText;
   wxStaticText *geomagneticIndexStaticText;
   wxStaticText *fileNameStaticText;
    
   wxTextCtrl *solarFluxTextCtrl;
   wxTextCtrl *avgSolarFluxTextCtrl;
   wxTextCtrl *geomagneticIndexTextCtrl;
   wxTextCtrl *fileNameTextCtrl;
   
   wxButton *browseButton;
   
   wxRadioButton *userInputRadioButton;
   wxRadioButton *fileInputRadioButton;
   
   Real *theForceData;
      
   wxString inputSourceString;
   
   // Methods inherited from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   // Event-handling Methods
   void OnTextChange(wxCommandEvent &event);
   void OnRadioButtonChange(wxCommandEvent& event);
   void OnBrowse(wxCommandEvent &event);
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 46000,
      ID_TEXTCTRL,
      ID_BUTTON,
      ID_RADIOBUTTON
   };
};

#endif

