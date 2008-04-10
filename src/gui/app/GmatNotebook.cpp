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
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"

#define DEBUG_NOTEBOOK 0

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
   EVT_NOTEBOOK_PAGE_CHANGED(-1, GmatNotebook::OnNotebookSelChange)
//   EVT_NOTEBOOK_PAGE_CHANGING(-1, GmatNotebook::OnNotebookSelChanging)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// GmatNotebook(wxWindow *parent, wxWindowID id,
//              const wxPoint &pos, const wxSize &size, long style)
//------------------------------------------------------------------------------
GmatNotebook::GmatNotebook(wxWindow *parent, wxWindowID id,
                           const wxPoint &pos, const wxSize &size, long style)
   : wxNotebook(parent, id, pos, size, style)
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
// void OnNoetbookSelChange(wxNotebookEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles notebook page change
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void GmatNotebook::OnNotebookSelChange(wxNotebookEvent &event)
{
   int sel = event.GetSelection();
   
#if DEBUG_NOTEBOOK
   MessageInterface::ShowMessage("GmatNotebook::OnNotebookSelChange sel=%d\n", sel);
#endif
   
   if (sel == 0)
   {
      resourceTree->UpdateResource(false); //loj: 6/29/04 added false
   }

   event.Skip(); // need this

//   GmatMainFrame *theMainFrame = GmatAppData::GetMainFrame();
//
//   if (theMainFrame != NULL)
//   {
//      theMainFrame->MinimizeChildren(sel);
//      theMainFrame->Cascade();
//   }
}

//------------------------------------------------------------------------------
// void OnNoetbookSelChanging(wxNotebookEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles notebook page change
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void GmatNotebook::OnNotebookSelChanging(wxNotebookEvent &event)
{
//   int sel = event.GetSelection();
//
//   GmatMainFrame *theMainFrame = GmatAppData::GetMainFrame();
//
//   if (theMainFrame != NULL)
//   {
//      theMainFrame->Cascade();
//      theMainFrame->MinimizeChildren(sel);
//   }
//   else
      event.Skip(); // need this
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
   
   //We don't want to edit labels
   long style = wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT|
      wxSUNKEN_BORDER|wxTR_SINGLE|wxTR_FULL_ROW_HIGHLIGHT;
   
   resourceTree = new ResourceTree(panel, -1, wxDefaultPosition,
                                   wxDefaultSize, style);
   
   // set to GmatAppData
   GmatAppData::Instance()->SetResourceTree(resourceTree);
   
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
      wxSUNKEN_BORDER|wxTR_SINGLE|wxTR_FULL_ROW_HIGHLIGHT |wxTR_EXTENDED;

   missionTree = new MissionTree(panel, -1, wxDefaultPosition,
                                 wxDefaultSize, style);

   // set to GmatAppData
   GmatAppData::Instance()->SetMissionTree(missionTree);

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
   wxGridSizer *sizer = new wxGridSizer( 1, 0, 0 );
   wxPanel *panel = new wxPanel(this);

   long style = wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT|
      wxSUNKEN_BORDER|wxTR_SINGLE|wxTR_FULL_ROW_HIGHLIGHT;

   outputTree = new OutputTree(panel, -1, wxDefaultPosition,
                                   wxDefaultSize, style);

   // set to GmatAppData
   GmatAppData::Instance()->SetOutputTree(outputTree);

   sizer->Add( outputTree, 0, wxGROW|wxALL, 0 );

   panel->SetAutoLayout( TRUE );
   panel->SetSizer( sizer );
   sizer->Fit( panel );
   sizer->SetSizeHints( panel );

   return panel;
}
