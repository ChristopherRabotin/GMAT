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
#include "gmatdefs.hpp"
#include "GmatDialog.hpp"
#include <wx/string.h> // for wxArrayString

class CelesBodySelectDialog : public GmatDialog
{
public:
    
    CelesBodySelectDialog(wxWindow *parent, wxArrayString &bodiesToExclude, 
                          wxArrayString &bodiesToHide);

    wxArrayString& GetBodyNames()
        { return mBodyNames; }
    bool IsBodySelected()
        { return mIsBodySelected; }
    
private:
    static const int MAX_LIST_SIZE = 30;
    wxArrayString mBodyNames;
    wxArrayString mBodiesToExclude;
    wxArrayString mBodiesToHide;
    bool mIsBodySelected;

    wxButton *addBodyButton;
    wxButton *removeBodyButton;
    wxButton *clearBodyButton;
    
    wxListBox *bodyListBox;
    wxListBox *bodySelectedListBox;
    
    // methods inherited from GmatDialog
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
    virtual void ResetData();

    // event handling
    void OnButton(wxCommandEvent& event);
    
    DECLARE_EVENT_TABLE()
        
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 9300,
        ID_LISTBOX,
        ID_BUTTON,
    };
};

#endif

