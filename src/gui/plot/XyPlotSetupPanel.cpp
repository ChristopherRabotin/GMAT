//$Header$
//------------------------------------------------------------------------------
//                              XyPlotSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/02/02
/**
 * Implements XyPlotSetupPanel class.
 */
//------------------------------------------------------------------------------
#include "XyPlotSetupPanel.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(XyPlotSetupPanel, wxPanel)
   EVT_BUTTON(XY_PLOT_ADD_X,    XyPlotSetupPanel::OnAddX)
   EVT_BUTTON(XY_PLOT_ADD_Y,    XyPlotSetupPanel::OnAddY)
   EVT_BUTTON(XY_PLOT_REMOVE_X, XyPlotSetupPanel::OnRemoveX)
   EVT_BUTTON(XY_PLOT_REMOVE_Y, XyPlotSetupPanel::OnRemoveY)
   EVT_BUTTON(XY_PLOT_CLEAR_Y,  XyPlotSetupPanel::OnClearY)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
//------------------------------------------------------------------------------
// XyPlotSetupPanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs XyPlotSetupPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the XyPlotSetupPanel GUI
 */
//------------------------------------------------------------------------------
XyPlotSetupPanel::XyPlotSetupPanel(wxWindow *parent):wxPanel(parent)
{
   CreatePanel(this);
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void CreatePanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * @param <parent> input parent.
 * @param <scName> input spacecraft name.
 *
 * @note Creates the notebook for spacecraft information
 */
//------------------------------------------------------------------------------
void XyPlotSetupPanel::CreatePanel(wxWindow *parent)
{
    // SetParent(new wxFrame(0,-1,"title"));
    // parent = GetParent();
    
    pageBoxSizer = new wxBoxSizer( wxVERTICAL );
    paramGridSizer = new wxGridSizer( 5, 0, 0 );
    paramBoxSizer = new wxBoxSizer( wxVERTICAL );
    
    wxString strs4[] = 
    {
        wxT("Cartesian X"), 
        wxT("Cartesian Y"), 
        wxT("Cartesian Z"), 
    };

    // parameters box
    titleAvailbleText = new wxStaticText( parent, XY_TEXT, wxT("Parameters"), wxDefaultPosition, wxSize(80,-1), 0 );
    paramBoxSizer->Add( titleAvailbleText, 0, wxALIGN_CENTRE|wxALL, 5 );
    paramListBox = new wxListBox( parent, XY_LISTBOX, wxDefaultPosition, wxSize(140,125), 3, strs4, wxLB_SINGLE );
    paramBoxSizer->Add( paramListBox, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    // add, remove X buttons
    xButtonsBoxSizer = new wxBoxSizer( wxVERTICAL );
    xButtonsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    addXButton = new wxButton( parent, XY_PLOT_ADD_X, wxT("<--"), wxDefaultPosition, wxDefaultSize, 0 );
    xButtonsBoxSizer->Add( addXButton, 0, wxALIGN_CENTRE|wxALL, 5 );

    removeXButton = new wxButton( parent, XY_PLOT_REMOVE_X, wxT("-->"), wxDefaultPosition, wxDefaultSize, 0 );
    xButtonsBoxSizer->Add( removeXButton, 0, wxALIGN_CENTRE|wxALL, 5 );

    // add, remove Y buttons
    yButtonsBoxSizer = new wxBoxSizer( wxVERTICAL );
    yButtonsBoxSizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    addYButton = new wxButton( parent, XY_PLOT_ADD_Y, wxT("-->"), wxDefaultPosition, wxDefaultSize, 0 );
    yButtonsBoxSizer->Add( addYButton, 0, wxALIGN_CENTRE|wxALL, 5 );

    removeYButton = new wxButton( parent, XY_PLOT_REMOVE_Y, wxT("<--"), wxDefaultPosition, wxDefaultSize, 0 );
    yButtonsBoxSizer->Add( removeYButton, 0, wxALIGN_CENTRE|wxALL, 5 );

    // X box label
    xSelelectedBoxSizer = new wxBoxSizer( wxVERTICAL );
    titleXText = new wxStaticText( parent, XY_TEXT, wxT("Selected X"), wxDefaultPosition, wxSize(80,-1), 0 );
    xSelelectedBoxSizer->Add( titleXText, 0, wxALIGN_CENTRE|wxALL, 5 );

    // Y box label
    ySelelectedBoxSizer = new wxBoxSizer( wxVERTICAL );
    titleYText = new wxStaticText( parent, XY_TEXT, wxT("Selected Y"), wxDefaultPosition, wxSize(80,-1), 0 );
    ySelelectedBoxSizer->Add( titleYText, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxString emptyList[] = {};
    
    xSelectedListBox = new wxListBox( parent, XY_LISTBOX, wxDefaultPosition, wxSize(140,125), 0, emptyList, wxLB_SINGLE );
    xSelelectedBoxSizer->Add( xSelectedListBox, 0, wxALIGN_CENTRE|wxALL, 5 );

    ySelectedListBox = new wxListBox( parent, XY_LISTBOX, wxDefaultPosition, wxSize(140,125), 0, emptyList, wxLB_SINGLE );
    ySelelectedBoxSizer->Add( ySelectedListBox, 0, wxALIGN_CENTRE|wxALL, 5 );

    // put in the order
    paramGridSizer->Add( xSelelectedBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5 );
    paramGridSizer->Add( xButtonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5 );
    paramGridSizer->Add( paramBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5 );
    paramGridSizer->Add( yButtonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5 );
    paramGridSizer->Add( ySelelectedBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    pageBoxSizer->Add( paramGridSizer, 0, wxALIGN_CENTRE|wxALL, 5 );

    parent->SetAutoLayout( TRUE );
    parent->SetSizer( pageBoxSizer);
    pageBoxSizer->Fit( parent );
    pageBoxSizer->SetSizeHints( parent );
}

//------------------------------------------------------------------------------
// void OnAddX(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnAddX(wxCommandEvent& event)
{
    // get string in first list and then search for it
    // in the second list
    wxString s = paramListBox->GetStringSelection();
    int found = xSelectedListBox->FindString(s);
    
    // if the string wasn't found in the second list, insert it
    if ( found == wxNOT_FOUND )
      xSelectedListBox->Insert(s, 0);
    
}

//------------------------------------------------------------------------------
// void OnAddY(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnAddY(wxCommandEvent& event)
{
    // get string in first list and then search for it
    // in the second list
    wxString s = paramListBox->GetStringSelection();
    int found = ySelectedListBox->FindString(s);
    
    // if the string wasn't found in the second list, insert it
    if ( found == wxNOT_FOUND )
      ySelectedListBox->Insert(s, 0);
    
}

//------------------------------------------------------------------------------
// void OnRemoveX(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnRemoveX(wxCommandEvent& event)
{
    int sel = xSelectedListBox->GetSelection();
    xSelectedListBox->Delete(sel);
}

//------------------------------------------------------------------------------
// void OnRemoveY(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnRemoveY(wxCommandEvent& event)
{
    int sel = ySelectedListBox->GetSelection();
    ySelectedListBox->Delete(sel);
}

//------------------------------------------------------------------------------
// void OnClearY(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnClearY(wxCommandEvent& event)
{
    int sel = ySelectedListBox->GetSelection();
    ySelectedListBox->Delete(sel);
}

