//$Header$
//------------------------------------------------------------------------------
//                              OutputTree
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
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
   
   void RemoveItem(GmatTree::ItemType type, const wxString &name);
   void UpdateOutput(bool resetTree, bool removeReports);
   
private:
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   wxString theSubscriberName;
   
   //wxWindow *parent;
   wxTreeItemId mReportItem;
   wxTreeItemId mOpenGlItem;
   wxTreeItemId mXyPlotItem;
   
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
   void OnAddOpenGlPlot(wxCommandEvent &event);
   
   void OnCompareNumericValues(wxCommandEvent &event);
   void OnCompareTextLines(wxCommandEvent &event);
   
   DECLARE_EVENT_TABLE();
   
   // for popup menu
   enum
   {
      POPUP_COMPARE_NUMERIC_VALUES = 200,
      POPUP_COMPARE_TEXT_LINES
   };
};

#endif // OutputTree_hpp
