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
#include "PointMassSelectionDialog.hpp"

BEGIN_EVENT_TABLE(PointMassSelectionDialog, wxDialog)
   EVT_BUTTON(ID_BUTTON_ADD, PointMassSelectionDialog::OnAddButton)
   EVT_BUTTON(ID_BUTTON_SORT, PointMassSelectionDialog::OnSortButton)
   EVT_BUTTON(ID_BUTTON_REMOVE, PointMassSelectionDialog::OnRemoveButton)
   EVT_BUTTON(ID_BUTTON_ADD, PointMassSelectionDialog::OnOkButton)
   EVT_BUTTON(ID_BUTTON_SORT, PointMassSelectionDialog::OnApplyButton)
   EVT_BUTTON(ID_BUTTON_REMOVE, PointMassSelectionDialog::OnCancelButton)
   EVT_BUTTON(ID_BUTTON_REMOVE, PointMassSelectionDialog::OnHelpButton)
END_EVENT_TABLE()

PointMassSelectionDialog::PointMassSelectionDialog(wxWindow *parent, wxArrayString bodies, wxArrayString bodiesUsed)
{
    bodiesArrayString = bodies;
    usedBodiesArrayString = bodiesUsed;
    
    numOfBodies = bodiesArrayString.GetCount();
    numOfBodiesUsed = usedBodiesArrayString.GetCount();
     
    Setup(this);
}

void PointMassSelectionDialog::Setup(wxWindow *parent)
{
    // wxString
    wxString strArray1[] = {wxT("")};
    wxString strArray2[] = {wxT("")};

    // wxButton
    addButton = new wxButton( parent, ID_BUTTON_ADD, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    sortButton = new wxButton( parent, ID_BUTTON_SORT, wxT("Sort"), wxDefaultPosition, wxDefaultSize, 0 );
    removeButton = new wxButton( parent, ID_BUTTON_REMOVE, wxT("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
    okButton = new wxButton( parent, ID_BUTTON_REMOVE, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );;
    applyButton = new wxButton( parent, ID_BUTTON_REMOVE, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );;
    cancelButton = new wxButton( parent, ID_BUTTON_REMOVE, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );;
    helpButton = new wxButton( parent, ID_BUTTON_REMOVE, wxT("Help"), wxDefaultPosition, wxDefaultSize, 0 );;
    
    // wxListBox
    selectedListBox = new wxListBox( parent, ID_LISTBOX, wxDefaultPosition, wxSize(140,125), 1, strArray1, wxLB_SINGLE );
    availableListBox = new wxListBox( parent, ID_LISTBOX, wxDefaultPosition, wxSize(140,125), 5, strArray2, wxLB_SINGLE );
    
    // wx*Sizers
    item5 = new wxBoxSizer( wxVERTICAL );
    item9 = new wxBoxSizer( wxVERTICAL );
    item10 = new wxStaticText( parent, ID_TEXT, wxT("Selected"), wxDefaultPosition, wxSize(80,-1), 0 );
    item0 = new wxBoxSizer( wxVERTICAL );
    item1 = new wxGridSizer( 3, 0, 0 );
    item2 = new wxBoxSizer( wxVERTICAL );
    item3 = new wxStaticText( parent, ID_TEXT, wxT("Available"), wxDefaultPosition, wxSize(80,-1), 0 );

    // Add to wx*Sizers
    item2->Add( item3, 0, wxALIGN_CENTRE|wxALL, 5 );
    item2->Add( availableListBox, 0, wxALIGN_CENTRE|wxALL, 5 );

    item5->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item5->Add( addButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    item5->Add( sortButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    item5->Add( removeButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    item1->Add( item2, 0, wxALIGN_CENTRE|wxALL, 5 );
    item1->Add( item5, 0, wxALIGN_CENTRE|wxALL, 5 );

    item9->Add( item10, 0, wxALIGN_CENTRE|wxALL, 5 );
    item9->Add( selectedListBox, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    item1->Add( item9, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    item0->Add( item1, 0, wxALIGN_CENTRE|wxALL, 5 );

    parent->SetTitle("Point Mass Selection");
    parent->SetAutoLayout( TRUE );
    parent->SetSizer( item0);

    item0->Fit( parent );
    item0->SetSizeHints( parent );
}

void PointMassSelectionDialog::GetData()
{
    for (int i = 0; i < numOfBodies; i++)
    {
        availableListBox->Append(bodiesArrayString.Item(i));
    }
    
    for (int i = 0; i < numOfBodiesUsed; i++)
    {
        selectedListBox->Append(usedBodiesArrayString.Item(i));
    }
}

void PointMassSelectionDialog::SetData()
{

}

void PointMassSelectionDialog::OnAddButton(wxCommandEvent& event)
{    
    wxString s = availableListBox->GetStringSelection();
    selectedListBox->Append(s);
    
    usedBodiesArrayString.Add(s);
    
    applyButton->Enable(true);
}

void PointMassSelectionDialog::OnSortButton(wxCommandEvent& event)
{
    // waw:  Future implementation
}

void PointMassSelectionDialog::OnRemoveButton(wxCommandEvent& event)
{
    wxString s = selectedListBox->GetStringSelection();

    selectedListBox->SetStringSelection(s,false);
    
    usedBodiesArrayString.Remove(s);
    
    applyButton->Enable(true);
}

void PointMassSelectionDialog::OnOkButton()
{
    SetData();
    Close(true);
}

void PointMassSelectionDialog::OnApplyButton()
{
    SetData();
}

void PointMassSelectionDialog::OnCancelButton()
{
    Close(true);
}

void PointMassSelectionDialog::OnHelpButton()
{
    // waw: Future implementation
}

