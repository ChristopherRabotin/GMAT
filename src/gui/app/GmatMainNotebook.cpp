//$Header$
//------------------------------------------------------------------------------
//                              GmatMainNotebook
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Allison Greene
// Created: 2003/10/03
/**
 * This class provides the notebook for the right side of the main panel.
 */
//------------------------------------------------------------------------------
#include "GmatMainNotebook.hpp"
#include "GmatTreeItemData.hpp"
#include "SolarSystemWindow.hpp"
#include "SpacecraftPanel.hpp"
#include "UniversePanel.hpp"
#include "PropagationConfigPanel.hpp"
#include "PropagatorSetupPanel.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------

//------------------------------------------------------------------------------
// EVENT_TABLE(GmatMainNotebook, wxNotebook)
//------------------------------------------------------------------------------
/**
 * Events Table for the menu and tool bar
 *
 * @note Indexes event handler functions.
 */
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(GmatMainNotebook, wxNotebook)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// GmatMainNotebook(wxWindow *parent, wxWindowID id,
//                 const wxPoint &pos, const wxSize &size, long style)
//------------------------------------------------------------------------------
/**
 * Constructs GmatMainNotebook object.
 *
 * @param <parent> input parent object.
 * @param <id> input id.
 * @param <pos> input position.
 * @param <size> input size.
 * @param <style> input style.
 *
 * @note Creates the notebook object and starts a list to track the open pages.
 */
//------------------------------------------------------------------------------
GmatMainNotebook::GmatMainNotebook(wxWindow *parent, wxWindowID id,
                   const wxPoint &pos, const wxSize &size, long style)
                   :wxNotebook(parent, id, pos, size, style)
{
  this->parent = parent;
  curPages = new wxList();
}

//------------------------------------------------------------------------------
// void CreatePage(GmatTreeItemData *item)
//------------------------------------------------------------------------------
/**
 * Adds a page to notebook based on tree item type.
 *
 * @param <item> input GmatTreeItemData.
 */
//------------------------------------------------------------------------------
void GmatMainNotebook::CreatePage(GmatTreeItemData *item)
{
    wxGridSizer *sizer = new wxGridSizer( 1, 0, 0 );
    wxScrolledWindow *panel = new wxScrolledWindow(this);

    int dataType = item->GetDataType();

    if (OpenPage(item))
    {
      curPages->Append( (wxObject *) item);
      
      if ((dataType == UNIVERSE_FOLDER)             ||
          (dataType == DEFAULT_FORMATION_FOLDER)    ||
          (dataType == CREATED_FORMATION_FOLDER)    ||
          (dataType == DEFAULT_CONSTELLATION_FOLDER)||
          (dataType == CREATED_CONSTELLATION_FOLDER))
      {
        sizer->Add( new UniversePanel(panel), 0, wxGROW|wxALL, 0 );
      }
      else if ((dataType == DEFAULT_BODY)   ||
               (dataType == CREATED_BODY))
      {
        sizer->Add( new SolarSystemWindow(panel), 0, wxGROW|wxALL, 0 );
      }
      else if ((dataType == DEFAULT_SPACECRAFT )  ||
               (dataType == CREATED_SPACECRAFT ))
      {
        sizer->Add( new SpacecraftPanel(panel), 0, wxGROW|wxALL, 0 );
      }
      else if ((dataType == DEFAULT_PROPAGATOR)   ||
               (dataType == CREATED_PROPAGATOR))
      {
        sizer->Add( new PropagationConfigPanel(panel), 0,
                    wxGROW|wxALL, 0 );
      }
      else if (dataType == DEFAULT_PROPAGATE_COMMAND)
      {
        sizer->Add( new PropagatorSetupPanel(panel),
                    0, wxGROW|wxALL, 0 );
      }

      panel->SetScrollRate( 5, 5 );
      panel->SetAutoLayout( TRUE );
      panel->SetSizer( sizer );
      sizer->Fit( panel );
      sizer->SetSizeHints( panel );

      AddPage( panel, item->GetDesc(), TRUE, -1);

    }
}

//------------------------------------------------------------------------------
// void ClosePage()
//------------------------------------------------------------------------------
/**
 * Closes the current page.
 */
//------------------------------------------------------------------------------
void GmatMainNotebook::ClosePage()
{
  int curPageNum = this->GetSelection();

  if (curPageNum != -1)
  {
    // want to remove from curPages list too
    curPages->DeleteNode(curPages->Item(curPageNum));
    this->DeletePage(curPageNum);
  }
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// bool OpenPage(GmatTreeItemData *item)
//------------------------------------------------------------------------------
/**
 * Determines if page should be opened.  If the page is already opened, sets that
 * page as the selected page.
 *
 * @param <item> input GmatTreeItemData.
 *
 * @return True if page should be opened, false if it should not be opened.
 */
//------------------------------------------------------------------------------
bool GmatMainNotebook::OpenPage(GmatTreeItemData *item)
{
  bool result = TRUE;
  int dataType = item->GetDataType();

  if ((dataType == RESOURCES_FOLDER)     ||
      (dataType == SPACECRAFT_FOLDER)    ||
      (dataType == FORMATIONS_FOLDER)    ||
      (dataType == CONSTELLATIONS_FOLDER)||
      (dataType == PROPAGATORS_FOLDER)   ||
      (dataType == SOLVERS_FOLDER)       ||
      (dataType == PLOTS_FOLDER)         ||
      (dataType == INTERFACES_FOLDER))
  {
    result = FALSE;
  }
  else
  {
    int sel = curPages->IndexOf((wxObject*) item);

    if (sel == -1)   // NOT_FOUND
    {
      result = TRUE;
    }
    else
    {
      this->SetSelection(sel);
      result = FALSE;
    }
  }
  
  return result;
}

