//$Header$
//------------------------------------------------------------------------------
//                              UniversePanel
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
// Author: Monisha Butler
// Created: 2003/09/10
// Modified: 2003/09/29
// Modified: 2004/01/13 by Allison Greene for action handling
/**
 * This class allows user to specify where Universe information is 
 * coming from
 */
//------------------------------------------------------------------------------
#include "UniversePanel.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(UniversePanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)

   EVT_BUTTON(ID_BUTTON_ADD,       UniversePanel::OnAddButton)
   EVT_BUTTON(ID_BUTTON_SORT,      UniversePanel::OnSortButton)
   EVT_BUTTON(ID_BUTTON_REMOVE,    UniversePanel::OnRemoveButton)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
//------------------------------------------------------------------------------
// UniversePanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs UniversePanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the Universe GUI
 */
//------------------------------------------------------------------------------
UniversePanel::UniversePanel(wxWindow *parent):GmatPanel(parent)
{
  Create();
  Show();
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
void UniversePanel::Create()
{
   // SetParent(new wxFrame(0,-1,"title"));
   // parent = GetParent();
    
    item0 = new wxBoxSizer( wxVERTICAL );
    item1 = new wxGridSizer( 3, 0, 0 );
    item2 = new wxBoxSizer( wxVERTICAL );
    
    item3 = new wxStaticText( this, ID_TEXT, wxT("Available"), wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( item3, 0, wxALIGN_CENTRE|wxALL, 5 );
      
    wxString strs4[] = 
    {
        wxT("SLP"), 
        wxT("DE405"), 
        wxT("DE200"), 
    };

//        ag: not needed for build 2
//        wxT("Low Accuracy Analytic"), 
//        wxT("High Accuracy Analytic")

    item4 = new wxListBox( this, ID_LISTBOX, wxDefaultPosition, wxSize(140,125), 3, strs4, wxLB_SINGLE );
    item2->Add( item4, 0, wxALIGN_CENTRE|wxALL, 5 );
    item1->Add( item2, 0, wxALIGN_CENTRE|wxALL, 5 );
    wxBoxSizer *item5 = new wxBoxSizer( wxVERTICAL );
    item5->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item6 = new wxButton( this, ID_BUTTON_ADD, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item6, 0, wxALIGN_CENTRE|wxALL, 5 );

    // ag:  changed button from "sort" to "prioritize"
    item7 = new wxButton( this, ID_BUTTON_SORT, wxT("Prioritize"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item7, 0, wxALIGN_CENTRE|wxALL, 5 );

    item8 = new wxButton( this, ID_BUTTON_REMOVE, wxT("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item8, 0, wxALIGN_CENTRE|wxALL, 5 );
    item1->Add( item5, 0, wxALIGN_CENTRE|wxALL, 5 );
    wxBoxSizer *item9 = new wxBoxSizer( wxVERTICAL );
    item10 = new wxStaticText( this, ID_TEXT, wxT("Selected"), wxDefaultPosition, wxSize(80,-1), 0 );
    item9->Add( item10, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxString strs11[] = 
    {
    };
    item11 = new wxListBox( this, ID_LISTBOX, wxDefaultPosition, wxSize(140,125), 0, strs11, wxLB_SINGLE );
    item9->Add( item11, 0, wxALIGN_CENTRE|wxALL, 5 );

    item1->Add( item9, 0, wxALIGN_CENTRE|wxALL, 5 );
    item0->Add( item1, 0, wxALIGN_CENTRE|wxALL, 5 );

    theMiddleSizer->Add(item0, 0, wxALIGN_CENTER|wxALL, 5);

//      parent->SetAutoLayout( TRUE );
//      parent->SetSizer( item0);
//      item0->Fit( parent );
//      item0->SetSizeHints( parent );
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void UniversePanel::LoadData()
{
    // load data from the core engine
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void UniversePanel::SaveData()
{
    // save data to core engine
}

//------------------------------------------------------------------------------
// virtual void OnHelp()
//------------------------------------------------------------------------------
void UniversePanel::OnHelp()
{
    // open the window
    GmatPanel::OnHelp();

    // fill help text
}

//------------------------------------------------------------------------------
// virtual void OnScript()
//------------------------------------------------------------------------------
void UniversePanel::OnScript()
{
    // open the window
    GmatPanel::OnScript();

    // fill scripts
}


void UniversePanel::OnAddButton(wxCommandEvent& event)
{
    // get string in first list and then search for it
    // in the second list
    wxString s = item4->GetStringSelection();
    int found = item11->FindString(s);
    
    // if the string wasn't found in the second list, insert it
    if ( found == wxNOT_FOUND )
      item11->Insert(s, 0);
}

// moves selected item to the top of the lsit
void UniversePanel::OnSortButton(wxCommandEvent& event)
{
    // get string
    wxString s = item11->GetStringSelection();

    if (!s.IsEmpty())
    {
      // remove string
      int sel = item11->GetSelection();
      item11->Delete(sel);
      // add string to top
      item11->Insert(s, 0);
    }
}

void UniversePanel::OnRemoveButton(wxCommandEvent& event)
{
    int sel = item11->GetSelection();
    item11->Delete(sel);
}
