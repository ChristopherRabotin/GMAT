//$Id$
//------------------------------------------------------------------------------
//                             GmatToolBar
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2008/11/13
/**
 * This class provides the tool bar for the main frame.
 */
//------------------------------------------------------------------------------
#ifndef GmatToolBar_hpp
#define GmatToolBar_hpp

#include "gmatwxdefs.hpp"
#include "GmatTreeItemData.hpp"  // for GmatTree::

class GmatToolBar : public wxToolBar
{
public:
   GmatToolBar(wxWindow* parent, long style = wxTB_HORIZONTAL | wxNO_BORDER,
               wxWindowID id = -1,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               const wxString& name = wxPanelNameStr);
   
   void CreateToolBar(wxToolBar *toolBar);
   void AddAnimationTools(wxToolBar* toolBar);
   void AddGuiScriptSyncStatus(wxToolBar* toolBar);
   void UpdateGuiScriptSyncStatus(wxToolBar* toolBar, int guiStat, int scriptStat);

protected:
   wxStaticText *theSyncStatus;

   void LoadIcon(const wxString &filename, long bitmapType, wxBitmap **bitmap, const char* xpm[]);
};

#endif

