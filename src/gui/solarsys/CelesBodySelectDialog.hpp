//$Header$
//------------------------------------------------------------------------------
//                              CelesBodySelectDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Declares CelesBodySelectDialog class. This class shows dialog window where
 * celestial bodies can be selected.
 * 
 */
//------------------------------------------------------------------------------
#ifndef CelesBodySelectDialog_hpp
#define CelesBodySelectDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"
#include "gmatdefs.hpp"
#include "RgbColor.hpp"
#include <wx/string.h> // for wxArrayString

class CelesBodySelectDialog : public GmatDialog
{
public:
    
   CelesBodySelectDialog(wxWindow *parent, wxArrayString &bodiesToExclude,
                         wxArrayString &bodiesToHide, bool showCalPoints = false);

   wxArrayString& GetBodyNames()
      { return mBodyNames; }
   UnsignedIntArray& GetBodyColors()
      { return mBodyColors; }
   bool IsBodySelected()
      { return mIsBodySelected; }

   void SetBodyColors(const wxArrayString &bodyNames,
                      const UnsignedIntArray &bodyColors);
   
private:
   static const int MAX_LIST_SIZE = 30;
   wxArrayString mBodyNames;
   wxArrayString mBodiesToExclude;
   wxArrayString mBodiesToHide;

   UnsignedIntArray mBodyColors; //loj: 12/15/04 Added
   
   bool mIsBodySelected;
   bool mShowCalPoints;
   
   wxString mSelBodyName;
   std::map<wxString, RgbColor> mBodyColorMap;
   
   wxColour mSelBodyColor;
   
   wxButton *addBodyButton;
   wxButton *removeBodyButton;
   wxButton *clearBodyButton;
   wxButton *mBodyColorButton;
   
   wxListBox *bodyListBox;
   wxListBox *bodySelectedListBox;
   
   // methods inherited from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   void ShowBodyOption(const wxString &name, bool show = true);
   
   // event handling
   void OnButton(wxCommandEvent& event);
   void OnColorButtonClick(wxCommandEvent& event);
   void OnSelectBody(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE()
      
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_LISTBOX,
      ID_BODY_SEL_LISTBOX,
      ID_BUTTON,
      ID_COLOR_BUTTON,
   };
};

#endif

