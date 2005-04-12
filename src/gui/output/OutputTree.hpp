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
   OutputTree(wxWindow *parent, const wxWindowID id,
                const wxPoint& pos, const wxSize& size,
                long style);
   void UpdateResource(bool resetCounter);

private:
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;

   //GmatMainNotebook *mainNotebook;
   //GmatMainFrame *mainFrame;

   //wxWindow *parent;
   wxTreeItemId mReportItem;
   wxTreeItemId mOpenGlItem;
   wxTreeItemId mXyPlotItem;

   void AddDefaultResources();

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


   DECLARE_EVENT_TABLE();

   // for popup menu
   enum
   {
   };
};

#endif // OutputTree_hpp
