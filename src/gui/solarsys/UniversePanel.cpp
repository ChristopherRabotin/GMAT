//$Header$
//------------------------------------------------------------------------------
//                              Universe
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Monisha Butler
// Created: 2003/09/10
// Modified: 2003/09/29
/**
 * This class allows user to specify where Universe information is 
 * coming from
 */
//------------------------------------------------------------------------------
#include "UniversePanel.hpp"

BEGIN_EVENT_TABLE(Universe, wxPanel)
   EVT_BUTTON(ID_BUTTON_ADD,       Universe::OnAddButton)
   EVT_BUTTON(ID_BUTTON_SORT,      Universe::OnSortButton)
   EVT_BUTTON(ID_BUTTON_REMOVE,    Universe::OnRemoveButton)
END_EVENT_TABLE()
//SolarSystem::SolarSystem(wxWindow *parent):wxDialog(parent,-1, wxString("Solar System"))

Universe::Universe(wxWindow *parent):wxPanel(parent)
{
   CreateUniverse(this);
}

void Universe::CreateUniverse(wxWindow *parent)
{
   // SetParent(new wxFrame(0,-1,"title"));
   // parent = GetParent();
    
    item0 = new wxBoxSizer( wxVERTICAL );

    item1 = new wxGridSizer( 3, 0, 0 );

    item2 = new wxBoxSizer( wxVERTICAL );
    
    item3 = new wxStaticText( parent, ID_TEXT, wxT("Available"), wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( item3, 0, wxALIGN_CENTRE|wxALL, 5 );
      
      wxString strs4[] = 
    {
        wxT("SLP"), 
        wxT("DE405"), 
        wxT("DE200"), 
        wxT("Low Accuracy Analytic"), 
        wxT("High Accuracy Analytic")
    };
      
    item4 = new wxListBox( parent, ID_LISTBOX, wxDefaultPosition, wxSize(140,125), 5, strs4, wxLB_SINGLE );
    item2->Add( item4, 0, wxALIGN_CENTRE|wxALL, 5 );

    item1->Add( item2, 0, wxALIGN_CENTRE|wxALL, 5 );

    item5 = new wxBoxSizer( wxVERTICAL );

    item5->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    item6 = new wxButton( parent, ID_BUTTON_ADD, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item6, 0, wxALIGN_CENTRE|wxALL, 5 );

    item7 = new wxButton( parent, ID_BUTTON_SORT, wxT("Sort"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item7, 0, wxALIGN_CENTRE|wxALL, 5 );

    item8 = new wxButton( parent, ID_BUTTON_REMOVE, wxT("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item8, 0, wxALIGN_CENTRE|wxALL, 5 );

    item1->Add( item5, 0, wxALIGN_CENTRE|wxALL, 5 );

    item9 = new wxBoxSizer( wxVERTICAL );

    item10 = new wxStaticText( parent, ID_TEXT, wxT("Selected"), wxDefaultPosition, wxSize(80,-1), 0 );
    item9->Add( item10, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxString strs11[] = 
    {
        wxT("")
    };
    item11 = new wxListBox( parent, ID_LISTBOX, wxDefaultPosition, wxSize(140,125), 1, strs11, wxLB_SINGLE );
    item9->Add( item11, 0, wxALIGN_CENTRE|wxALL, 5 );

    item1->Add( item9, 0, wxALIGN_CENTRE|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTRE|wxALL, 5 );

    
   
        parent->SetAutoLayout( TRUE );
        parent->SetSizer( item0);
        item0->Fit( parent );
        item0->SetSizeHints( parent );
     
   
}

void Universe::OnAddButton(wxCommandEvent& event)
{
    //int n = item4->GetSelection();
    wxString s = item4->GetStringSelection();
    item11->Append(s);
    
}

void Universe::OnSortButton(wxCommandEvent& event)
{

}

void Universe::OnRemoveButton(wxCommandEvent& event)
{
     wxString m = item11->GetStringSelection();
     item11->SetStringSelection(m,FALSE);

}
