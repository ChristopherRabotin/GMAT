//$Header$
//------------------------------------------------------------------------------
//                              BarycenterPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Allison Greene
// Created: 2005/04/21
/**
 * This class allows user to specify where Universe information is 
 * coming from
 */
//------------------------------------------------------------------------------
#include "BarycenterPanel.hpp"
#include "MessageInterface.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(BarycenterPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)

   EVT_BUTTON(ID_BUTTON_ADD,       BarycenterPanel::OnAddButton)
   EVT_BUTTON(ID_BUTTON_REMOVE,    BarycenterPanel::OnRemoveButton)

   EVT_LISTBOX(ID_AVAILABLE_LIST, BarycenterPanel::OnAvailableSelectionChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
//------------------------------------------------------------------------------
// BarycenterPanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs BarycenterPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the Universe GUI
 */
//------------------------------------------------------------------------------
BarycenterPanel::BarycenterPanel(wxWindow *parent):GmatPanel(parent)
{
   Create();
   Show();
}

BarycenterPanel::~BarycenterPanel()
{
}


//-------------------------------
// private methods
//-------------------------------
//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * @param <scName> input spacecraft name.
 *
 * @note Creates the notebook for spacecraft information
 */
//------------------------------------------------------------------------------
void BarycenterPanel::Create()
{
   wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
   wxFlexGridSizer *item1 = new wxFlexGridSizer( 3, 0, 0 );
   wxBoxSizer *item2 = new wxBoxSizer( wxVERTICAL );
    
   item3 = new wxStaticText( this, ID_TEXT, wxT("Celestial Bodies"), wxDefaultPosition,
                             wxSize(80,-1), 0 );
   item2->Add( item3, 0, wxALIGN_CENTRE|wxALL, 5 );

   wxString availableStrs [] = 
   {
   };

   availableListBox = new wxListBox( this, ID_AVAILABLE_LIST, wxDefaultPosition, 
                                     wxSize(140,125), 0,
                                     availableStrs, wxLB_SINGLE );
   item2->Add( availableListBox, 0, wxALIGN_CENTRE|wxALL, 5 );
   item1->Add( item2, 0, wxALIGN_CENTRE|wxALL, 5 );
   wxBoxSizer *item5 = new wxBoxSizer( wxVERTICAL );
   item5->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
   addButton = new wxButton( this, ID_BUTTON_ADD, wxT("-->"), wxDefaultPosition,
                             wxSize(20,20), 0 );
   item5->Add( addButton, 0, wxALIGN_CENTRE|wxALL, 5 );

   removeButton = new wxButton( this, ID_BUTTON_REMOVE, wxT("<--"),
                                wxDefaultPosition, wxSize(20,20), 0 );
   item5->Add( removeButton, 0, wxALIGN_CENTRE|wxALL, 5 );
   removeButton->Enable(false);
   item1->Add( item5, 0, wxALIGN_CENTRE|wxALL, 5 );
   wxBoxSizer *item9 = new wxBoxSizer( wxVERTICAL );
   item10 = new wxStaticText( this, ID_TEXT, wxT("Selected"), wxDefaultPosition, 
                              wxSize(80,-1), 0 );
   item9->Add( item10, 0, wxALIGN_CENTRE|wxALL, 5 );

   wxString strs11[] = 
   {
   };
   selectedListBox = new wxListBox( this, ID_SELECTED_LIST, wxDefaultPosition, 
                                    wxSize(140,125), 0, strs11, wxLB_SINGLE );
   item9->Add( selectedListBox, 0, wxALIGN_CENTRE|wxALL, 5 );

   item1->Add( item9, 0, wxALIGN_CENTRE|wxALL, 5 );
   item0->Add( item1, 0, wxALIGN_CENTRE|wxALL, 5 );

   theMiddleSizer->Add(item0, 0, wxALIGN_CENTER|wxALL, 5);
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void BarycenterPanel::LoadData()
{
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void BarycenterPanel::SaveData()
{
}

//------------------------------------------------------------------------------
// void OnAddButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void BarycenterPanel::OnAddButton(wxCommandEvent& event)
{
   // get string in first list and then search for it
   // in the second list
   wxString s = availableListBox->GetStringSelection();
   int found = selectedListBox->FindString(s);
    
    // if the string wasn't found in the second list, insert it
   if ( found == wxNOT_FOUND )
   {
      selectedListBox->Insert(s, 0);
      selectedListBox->SetSelection(0);
      theApplyButton->Enable();
   }

   removeButton->Enable(false);
    
   if (selectedListBox->GetCount() > 0)
      removeButton->Enable(true);

}

// moves selected item to the top of the lsit
//------------------------------------------------------------------------------
// void OnSortButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void BarycenterPanel::OnSortButton(wxCommandEvent& event)
{
   // get string
   wxString s = selectedListBox->GetStringSelection();

   if (!s.IsEmpty())
   {
      // remove string
      int sel = selectedListBox->GetSelection();
      selectedListBox->Delete(sel);
      // add string to top
      selectedListBox->Insert(s, 0);
   }
    
   theApplyButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnRemoveButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void BarycenterPanel::OnRemoveButton(wxCommandEvent& event)
{
   int sel = selectedListBox->GetSelection();

   selectedListBox->Delete(sel);

   removeButton->Enable(false);
    
   if (selectedListBox->GetCount() > 0)
      removeButton->Enable(true);

   theApplyButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnAvailableSelectionChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void BarycenterPanel::OnAvailableSelectionChange(wxCommandEvent& event)
{
   // get string
   wxString s = availableListBox->GetStringSelection();

   if (selectedListBox->FindString(s) == wxNOT_FOUND)
   {
      addButton->Enable(true);
   }
}
