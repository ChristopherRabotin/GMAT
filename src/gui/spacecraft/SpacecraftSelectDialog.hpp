//$Header$
//------------------------------------------------------------------------------
//                              SpacecraftSelectDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Declares SpacecraftSelectDialog class. This class shows dialog window where
 * spacecraft can be selected.
 * 
 */
//------------------------------------------------------------------------------
#ifndef SpacecraftSelectDialog_hpp
#define SpacecraftSelectDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"
#include <wx/string.h>    // for wxArrayString

class SpacecraftSelectDialog : public GmatDialog
{
public:
    
   SpacecraftSelectDialog(wxWindow *parent, wxArrayString &scSelList,
                          wxArrayString &scExcList);

   wxArrayString& GetSpacecraftNames();
   bool IsSpacecraftSelected();
    
private:
   wxArrayString mScNameList;
   wxArrayString mScExcList;
   wxArrayString mScSelList;
   bool mIsSpacecraftSelected;

   wxButton *addSpacecraftButton;
   wxButton *removeSpacecraftButton;
   wxButton *clearSpacecraftButton;
    
   wxListBox *scAvailableListBox;
   wxListBox *scSelectedListBox;
    
   // methods inherited from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();

   // event handling
   void OnButton(wxCommandEvent& event);
    
   DECLARE_EVENT_TABLE();
        
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_LISTBOX,
      ID_BUTTON,
   };
};

#endif

