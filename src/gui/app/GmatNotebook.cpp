//$Header$
//------------------------------------------------------------------------------
//                             GmatNotebook
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Allison Greene
// Created: 2003/08/28
/**
 * This class provides the notebook for the left side of the main frame.
 */
//------------------------------------------------------------------------------
#include "GmatNotebook.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------

//------------------------------------------------------------------------------
// EVENT_TABLE(GmatNotebook, wxNotebook)
//------------------------------------------------------------------------------
/**
 * Events Table for the menu and tool bar
 *
 * @note Indexes event handler functions.
 */
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(GmatNotebook, wxNotebook)
END_EVENT_TABLE()

GmatNotebook::GmatNotebook(wxWindow *parent, wxWindowID id,
                   const wxPoint &pos, const wxSize &size, long style)
                   :wxNotebook(parent, id, pos, size, style)
{
  this->parent = parent;
    
  wxPanel *panel = (wxPanel *)NULL;
  
  // Create and add Resource, Mission, and Output Tabs
  panel = CreateResourcePage( );
  AddPage( panel, wxT("Resources") );
  
  panel = CreateMissionPage( );
  AddPage( panel, wxT("Mission") );

  panel = CreateOutputPage( );
  AddPage( panel, wxT("Output") );

}

//------------------------------------------------------------------------------
// void SetMainNotebook (GmatMainNotebook *gmatMainNotebook)
//------------------------------------------------------------------------------
/**
 * Sets the main notebook for GmatNotebook, ResourceTree, and MissionTree
 *
 * @param <gmatMainNotebook> input GmatMainNotebook.
 */
//------------------------------------------------------------------------------
void GmatNotebook::SetMainNotebook (GmatMainNotebook *gmatMainNotebook)
{
   this->mainNotebook = gmatMainNotebook;
   resourceTree->SetMainNotebook(this->mainNotebook);
   missionTree->SetMainNotebook(this->mainNotebook);
}

//------------------------------------------------------------------------------
// GmatMainNotebook *GetMainNotebook()
//------------------------------------------------------------------------------
/**
 * Gets current instance of GmatMainNotebook.
 *
 * @return GmatMainNotebook object.
 */
//------------------------------------------------------------------------------
GmatMainNotebook *GmatNotebook::GetMainNotebook()
{
   return this->mainNotebook;
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// wxPanel *CreateResourcePage()
//------------------------------------------------------------------------------
/**
 * Adds a tree with resource information to notebook.
 *
 * @return wxPanel object with resource information.
 */
//------------------------------------------------------------------------------
wxPanel *GmatNotebook::CreateResourcePage()
{
    wxGridSizer *sizer = new wxGridSizer( 1, 0, 0 );
    wxPanel *panel = new wxPanel(this);
    
    long style = wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT|
                wxSUNKEN_BORDER|wxTR_SINGLE|wxTR_FULL_ROW_HIGHLIGHT|
                wxTR_EDIT_LABELS;

    resourceTree = new ResourceTree(panel, -1, wxDefaultPosition,
                   wxDefaultSize, style);

    sizer->Add( resourceTree, 0, wxGROW|wxALL, 0 );

    panel->SetAutoLayout( TRUE );
    panel->SetSizer( sizer );
    sizer->Fit( panel );
    sizer->SetSizeHints( panel );
    
    return panel;
}

//------------------------------------------------------------------------------
// wxPanel *CreateMissionPage()
//------------------------------------------------------------------------------
/**
 * Adds a tree with missions information to notebook.
 *
 * @return wxPanel object with missions information.
 */
//------------------------------------------------------------------------------
wxPanel *GmatNotebook::CreateMissionPage()
{
    wxGridSizer *sizer = new wxGridSizer( 1, 0, 0 );
    wxPanel *panel = new wxPanel(this);

    long style = wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT|
                wxSUNKEN_BORDER|wxTR_SINGLE|wxTR_FULL_ROW_HIGHLIGHT;

    missionTree = new MissionTree(panel, -1, wxDefaultPosition,
                  wxDefaultSize, style);

    sizer->Add( missionTree, 0, wxGROW|wxALL, 0 );

    panel->SetAutoLayout( TRUE );
    panel->SetSizer( sizer );
    sizer->Fit( panel );
    sizer->SetSizeHints( panel );

    return panel;
}

//------------------------------------------------------------------------------
// wxPanel *CreateOutputPage()
//------------------------------------------------------------------------------
/**
 * Adds a tree with output information to notebook.
 *
 * @return wxPanel object with output information.
 */
//------------------------------------------------------------------------------
wxPanel *GmatNotebook::CreateOutputPage()
{
    wxPanel *panel = new wxPanel(this);
    return panel;
}
