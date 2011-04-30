//$Id$
//------------------------------------------------------------------------------
//                              SpaceObjectSelectDialog
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Linda Jun
// Created: 2004/08/04
//
/**
 * Declares SpaceObjectSelectDialog class. This class shows dialog window where
 * spacecraft can be selected.
 * 
 */
//------------------------------------------------------------------------------
#ifndef SpaceObjectSelectDialog_hpp
#define SpaceObjectSelectDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"
#include <wx/string.h>    // for wxArrayString

class SpaceObjectSelectDialog : public GmatDialog
{
public:
    
   SpaceObjectSelectDialog(wxWindow *parent, wxArrayString &soSelList,
                          wxArrayString &soExcList);
   ~SpaceObjectSelectDialog();
   
   wxArrayString& GetSpaceObjectNames();
   bool HasSelectionChanged();
   
private:
   wxArrayString mSoNameList;
   wxArrayString mSoExcList;
   wxArrayString mSoSelList;
   bool mHasSelectionChanged;
   
   wxButton *addSpaceObjectButton;
   wxButton *removeSpaceObjectButton;
   wxButton *clearSpaceObjectButton;
   
   wxListBox *spaceObjAvailableListBox;
   wxListBox *spaceObjSelectedListBox;
    
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



