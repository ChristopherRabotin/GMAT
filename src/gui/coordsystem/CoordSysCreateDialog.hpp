//$Header$
//------------------------------------------------------------------------------
//                              CoordSysCreateDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2005/03/03
//
/**
 * Declares CoordSysCreateDialog class. This class shows dialog window where a
 * coordinate system can be created.
 * 
 */
//------------------------------------------------------------------------------
#ifndef CoordSysCreateDialog_hpp
#define CoordSysCreateDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"
#include "CoordPanel.hpp"

class CoordSysCreateDialog : public GmatDialog
{
public:
    
   CoordSysCreateDialog(wxWindow *parent);

   wxString& GetCoordName()
      { return mCoordName; }
   bool IsCoordCreated()
      { return mIsCoordCreated; }

protected:
   bool mIsCoordCreated;
   bool mIsTextModified;
   
   wxString mCoordName;
   CoordPanel *mCoordPanel;
   
   wxStaticText *nameStaticText;
   wxTextCtrl *nameTextCtrl;
   
   // abstract methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   // event handling
   void OnTextUpdate(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXTCTRL = 46000,
      ID_COMBO,
      ID_TEXT,
      ID_BUTTON
   };
   
private:
   wxString wxFormatName;
   
   wxTextCtrl *epochTextCtrl;
   wxTextCtrl *intervalTextCtrl;

   wxComboBox *originComboBox;
   wxComboBox *typeComboBox;
   wxComboBox *primaryComboBox;
   wxComboBox *formatComboBox;
   wxComboBox *secondaryComboBox;
   
   wxComboBox *xComboBox;
   wxComboBox *yComboBox;
   wxComboBox *zComboBox;
   
};

#endif
