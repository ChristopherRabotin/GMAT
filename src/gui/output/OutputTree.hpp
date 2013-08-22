//$Id$
//------------------------------------------------------------------------------
//                              OutputTree
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Allison Greene
// Created: 2005/04/11
//
/**
 * This class provides the resource tree and event handlers.
 */
//------------------------------------------------------------------------------
#ifndef OutputTree_hpp
#define OutputTree_hpp

#include "gmatwxdefs.hpp"

#include "wx/treectrl.h"
#include "wx/image.h"
#include "wx/imaglist.h"

#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "GmatTreeItemData.hpp"

class OutputTree : public wxTreeCtrl
{
public:

   // constructor
   OutputTree(wxWindow *parent, const wxWindowID id, const wxPoint& pos,
              const wxSize& size, long style);
   
   void RemoveItem(GmatTree::ItemType type, const wxString &name, bool forceRemove = false);
   void UpdateOutput(bool resetTree, bool removeReports, bool removePlots);
   
private:
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   wxString theSubscriberName;
   
   wxTreeItemId mReportItem;
   wxTreeItemId mEphemFileItem;
   wxTreeItemId mOrbitViewItem;
   wxTreeItemId mGroundTrackItem;
   wxTreeItemId mXyPlotItem;
   wxTreeItemId mEventsItem;
   
   void AddDefaultResources();
   wxTreeItemId FindItem(wxTreeItemId parentId, const wxString &name);
   
   // event handlers
   void OnItemRightClick(wxTreeEvent& event);
   void ShowMenu(wxTreeItemId id, const wxPoint& pt);
   void OnItemActivated(wxTreeEvent &event);
   void OnOpen(wxCommandEvent &event);
   void OnClose(wxCommandEvent &event);
   void OnRename(wxCommandEvent &event);
   void OnDelete(wxCommandEvent &event);
   void OnEndLabelEdit(wxTreeEvent &event);
   void OnBeginLabelEdit(wxTreeEvent &event);
   
   void AddIcons();   
   void OnAddReportFile(wxCommandEvent &event);
   void OnAddXyPlot(wxCommandEvent &event);
   void OnAddOrbitView(wxCommandEvent &event);
   
   void OnCompareTextLines(wxCommandEvent &event);
   void OnCompareNumericLines(wxCommandEvent &event);
   void OnCompareNumericColumns(wxCommandEvent &event);
   
   DECLARE_EVENT_TABLE();
   
   // for popup menu
   enum
   {
      POPUP_COMPARE_TEXT_LINES = 200,
      POPUP_COMPARE_NUMERIC_LINES,
      POPUP_COMPARE_NUMERIC_COLUMNS,
   };
};

#endif // OutputTree_hpp
