//$Header$
//------------------------------------------------------------------------------
//                              GmatSplitterWindow
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Allison Greene
// Created: 2003/08/28
//
/**
 * This class provides the splitter window to divide the main panel and
 * handles the splitter window events
 */
//------------------------------------------------------------------------------
#include "GmatSplitterWindow.hpp"

//-------------------------------
// event tables for wxWindows
//-------------------------------

//------------------------------------------------------------------------------
// EVENT_TABLE(GmatSplitterWindow, wxSplitterWindow)
//------------------------------------------------------------------------------
/**
 * Events Table for Splitter Window
 *
 * @note Indexes event handler functions.
 */
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(GmatSplitterWindow, wxSplitterWindow)
   EVT_SPLITTER_SASH_POS_CHANGED(-1, GmatSplitterWindow::OnPositionChanged)
   EVT_SPLITTER_SASH_POS_CHANGING(-1, GmatSplitterWindow::OnPositionChanging)
END_EVENT_TABLE()

//-------------------------------
// public methods
//-------------------------------

//------------------------------------------------------------------------------
// GmatSplitterWindow(wxFrame *parent)
//------------------------------------------------------------------------------
/**
 * Constructs GmatSplitterWindow object.
 *
 * @param <parent> input parent.
 */
//------------------------------------------------------------------------------
GmatSplitterWindow::GmatSplitterWindow(wxWindow *parent)
                   :wxSplitterWindow(parent, -1, wxDefaultPosition, wxDefaultSize,
                                     wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN)
{

}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnPositionChanged(wxSplitterEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles splitter bar change.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatSplitterWindow::OnPositionChanged(wxSplitterEvent &event)
{
   event.Skip();
}

//------------------------------------------------------------------------------
// void OnPositionChanging(wxSplitterEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles splitter bar changing.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatSplitterWindow::OnPositionChanging(wxSplitterEvent &event)
{
   event.Skip();
}
