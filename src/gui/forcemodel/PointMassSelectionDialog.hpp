//------------------------------------------------------------------------------
//                              PointMassSelectionPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/12/04
// Modified:
/**
 * This class allows user to select Point Masses.
 */
//------------------------------------------------------------------------------

#ifndef PointMassSelectionDialog_hpp
#define PointMassSelectionDialog_hpp

// gui includes
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/button.h>
#include <wx/string.h>

//???
#include <wx/statline.h>
#include <wx/spinbutt.h>
#include <wx/spinctrl.h>
#include <wx/splitter.h>
#include <wx/grid.h>
#include <wx/control.h>

#include "gmatwxdefs.hpp"

// base includes
#include "gmatdefs.hpp"

class PointMassSelectionDialog : public wxDialog
{
public:
    PointMassSelectionDialog(wxWindow *parent, wxArrayString bodies, wxArrayString bodiesUsed);
    
private:
    wxGridSizer *item1;
    
    wxBoxSizer *item0;
    wxBoxSizer *item2;
    wxBoxSizer *item5;
    wxBoxSizer *item9;

    wxStaticText *item3;
    wxStaticText *item10;

    wxListBox *availableListBox;
    wxListBox *selectedListBox;

    wxButton *addButton;
    wxButton *sortButton;
    wxButton *removeButton;
    wxButton *okButton;
    wxButton *applyButton;
    wxButton *cancelButton;
    wxButton *helpButton;
    
    wxArrayString bodiesArrayString;
    wxArrayString usedBodiesArrayString;
    
    Integer numOfBodies;
    Integer numOfBodiesUsed;
    
    void GetData();
    void SetData();
    void Setup(wxWindow *parent);
    void OnAddButton(wxCommandEvent& event);
    void OnSortButton(wxCommandEvent& event);
    void OnRemoveButton(wxCommandEvent& event);
    void OnOkButton();
    void OnApplyButton();
    void OnCancelButton();
    void OnHelpButton();

    DECLARE_EVENT_TABLE();

    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 10003,
        ID_LISTBOX,
        ID_BUTTON,
        ID_BUTTON_ADD,
        ID_BUTTON_SORT,
        ID_BUTTON_REMOVE,
        ID_BUTTON_OK,
        ID_BUTTON_APPLY,
        ID_BUTTON_CANCEL,
        ID_BUTTON_HELP  
    };
};

#endif

