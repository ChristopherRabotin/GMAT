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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Allison Greene
// Created: 2005/04/11
//
/**
 * This class provides the resource tree and event handlers.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "bitmaps/ClosedFolder.xpm"
#include "bitmaps/OpenFolder.xpm"
#include "bitmaps/file.xpm"
#include "bitmaps/rt_ReportFile.xpm"
#include "bitmaps/rt_OrbitView.xpm"
#include "bitmaps/rt_GroundTrackPlot.xpm"
#include "bitmaps/rt_XYPlot.xpm"
#include "bitmaps/rt_Default.xpm"
#include <wx/string.h>        // for wxArrayString

#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "OutputTree.hpp"
#include "GmatTreeItemData.hpp"
#include "ViewTextFrame.hpp"

#include "GuiInterpreter.hpp"
#include "Subscriber.hpp"
#include "ReportFile.hpp"
#include "FileUtil.hpp"       // for GmatFileUtil::Compare()
#include "MessageInterface.hpp"

//#define DEBUG_OUTPUT_TREE 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(OutputTree, wxTreeCtrl)
   EVT_TREE_ITEM_RIGHT_CLICK(-1, OutputTree::OnItemRightClick)
   EVT_TREE_ITEM_ACTIVATED(-1, OutputTree::OnItemActivated)
   EVT_TREE_BEGIN_LABEL_EDIT(-1, OutputTree::OnBeginLabelEdit)
   EVT_TREE_END_LABEL_EDIT(-1, OutputTree::OnEndLabelEdit)
   EVT_MENU(POPUP_COMPARE_NUMERIC_VALUES, OutputTree::OnCompareNumericValues)
   EVT_MENU(POPUP_COMPARE_TEXT_LINES, OutputTree::OnCompareTextLines)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// OutputTree(wxFrame *parent)
//------------------------------------------------------------------------------
/**
 * A constructor.
 *
 * @param <parent> parent window
 * @param <id> window id for referencing events
 * @param <pos> position
 * @param <size> size 
 * @param <style> style flags
 */
//------------------------------------------------------------------------------
OutputTree::OutputTree(wxWindow *parent, const wxWindowID id,
                           const wxPoint &pos, const wxSize &size, long style)
   : wxTreeCtrl(parent, id, pos, size, style)
{
   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   
   AddIcons();
   AddDefaultResources();
   
   theGuiManager->UpdateAll();
}


//------------------------------------------------------------------------------
// void RemoveItem(GmatTree::ItemType type, const wxString &name)
//------------------------------------------------------------------------------
/*
 * Removes item from the output tree.
 *
 * @param <type> The item type to removed
 * @param <name> The name of the node to be removed
 */
//------------------------------------------------------------------------------
void OutputTree::RemoveItem(GmatTree::ItemType type, const wxString &name)
{
   #if DEBUG_OUTPUT_TREE
   MessageInterface::ShowMessage
      ("OutputTree::RemoveItem() entered, type=%d, name=%s\n", type, name.c_str());
   #endif
   
   wxTreeItemId parentId;

   switch (type)
   {
   case GmatTree::OUTPUT_ORBIT_VIEW:
      parentId = mOrbitViewItem;
      break;
   case GmatTree::OUTPUT_GROUND_TRACK_PLOT:
      parentId = mGroundTrackItem;
      break;
   case GmatTree::OUTPUT_XY_PLOT:
      parentId = mXyPlotItem;
      break;
   case GmatTree::OUTPUT_REPORT:
   case GmatTree::OUTPUT_EVENT_REPORT:
      #if DEBUG_OUTPUT_TREE
      MessageInterface::ShowMessage
         ("*** OutputTree::RemoveItem() just returning, removing reports are disabled "
          "to allow users to view the reports after mission run.\n");
      #endif
      return;
   default:
      #if DEBUG_OUTPUT_TREE
      MessageInterface::ShowMessage
         ("*** OutputTree::RemoveItem() just returning, no matching type found\n");
      #endif
      return;
   }
   
   wxTreeItemId itemId = FindItem(parentId, name);
   
   if (itemId.IsOk())
   {
      // We need to collapse first and delete
      if (GetChildrenCount(parentId) == 1)
         Collapse(parentId);
      
      Delete(itemId);
      
      #if DEBUG_OUTPUT_TREE
      MessageInterface::ShowMessage
         ("   type=%d, name=%s removed\n", type, name.c_str());
      #endif
   }
   else
   {
      #if DEBUG_OUTPUT_TREE
      MessageInterface::ShowMessage
         ("   type=%d, name=%s NOT found\n", type, name.c_str());
      #endif
   }
   
   #if DEBUG_OUTPUT_TREE
   MessageInterface::ShowMessage("OutputTree::RemoveItem() returning\n");
   #endif
}


//------------------------------------------------------------------------------
// wxTreeItemId FindItem(wxTreeItemId parentId, const wxString &name)
//------------------------------------------------------------------------------
/*
 * Finds item from the output tree.
 *
 * @param <parentId> The parent node item
 * @param <name> The name of item to find from the parent node
 *
 * @return id of the item if found, uninitialized itemId otherwise
 */
//------------------------------------------------------------------------------
wxTreeItemId OutputTree::FindItem(wxTreeItemId parentId, const wxString &name)
{
   #if DEBUG_OUTPUT_TREE
   MessageInterface::ShowMessage
      ("OutputTree::FindItem() parentId=%s, name=%s\n",
       GetItemText(parentId).c_str(), name.c_str());
   #endif
   
   wxTreeItemId itemId;
   
   if (ItemHasChildren(parentId))
   {
      wxString itemText;
      wxTreeItemIdValue cookie;
      wxTreeItemId childId = GetFirstChild(parentId, cookie);
      
      while (childId.IsOk())
      {
         itemText = GetItemText(childId);
         if (itemText == name)
            return childId;
         
         childId = GetNextChild(parentId, cookie);
      }
   }
   
   return itemId;
}


//------------------------------------------------------------------------------
// void UpdateOutput(bool resetTree, bool removeReports, bool removePlots)
//------------------------------------------------------------------------------
/**
 * Updates output tree.
 *
 * @param  resetTree  Set this to true if just clearing tree
 * @param  removeReports  Set this to true if reports should be removed from the tree
 * @param  removePlots  Set this to true if plots should be removed from the tree
 */
//------------------------------------------------------------------------------
void OutputTree::UpdateOutput(bool resetTree, bool removeReports, bool removePlots)
{
   #if DEBUG_OUTPUT_TREE
   MessageInterface::ShowMessage
      ("OutputTree::UpdateOutput() resetTree=%d, removeReports=%d, removePlots=%d\n",
       resetTree, removeReports, removePlots);
   #endif
   
   if (removeReports)
   {
      Collapse(mReportItem);
      Collapse(mEventsItem);
   }
   
   if (removePlots)
   {
      Collapse(mOrbitViewItem);
      Collapse(mGroundTrackItem);
      Collapse(mXyPlotItem);
   }
   
   // delete all old children
   if (removeReports)
   {
      DeleteChildren(mReportItem);
      DeleteChildren(mEventsItem);
   }
   
   if (removePlots)
   {
      DeleteChildren(mOrbitViewItem);
      DeleteChildren(mGroundTrackItem);
      DeleteChildren(mXyPlotItem);
   }
   
   if (resetTree)    // do not load subscribers
      return;
   
   // get list of report files, opengl plots, and xy plots
   StringArray listOfSubs = theGuiInterpreter->GetListOfObjects(Gmat::SUBSCRIBER);
   
   // put each subscriber in the proper folder
   for (unsigned int i=0; i<listOfSubs.size(); i++)
   {
      Subscriber *sub =
         (Subscriber*)theGuiInterpreter->GetConfiguredObject(listOfSubs[i]);
      
      wxString objName = wxString(listOfSubs[i].c_str());
      wxString objTypeName = wxString(sub->GetTypeName().c_str());
      
      if (objTypeName.Trim() == "ReportFile")
      {
         AppendItem(mReportItem, objName, GmatTree::OUTPUT_ICON_REPORTFILE, -1,
                    new GmatTreeItemData(objName, GmatTree::OUTPUT_REPORT));
      }
      else if (objTypeName.Trim() == "OrbitView" &&
               sub->GetBooleanParameter("ShowPlot"))
      {
         AppendItem(mOrbitViewItem, objName, GmatTree::OUTPUT_ICON_ORBITVIEW, -1,
                    new GmatTreeItemData(objName, GmatTree::OUTPUT_ORBIT_VIEW));
      }
      else if (objTypeName.Trim() == "GroundTrackPlot" &&
               sub->GetBooleanParameter("ShowPlot"))
      {
         AppendItem(mGroundTrackItem, objName, GmatTree::OUTPUT_ICON_GROUNDTRACK, -1,
                    new GmatTreeItemData(objName, GmatTree::OUTPUT_GROUND_TRACK_PLOT));
      }
      else if (objTypeName.Trim() == "XYPlot" &&
               sub->GetBooleanParameter("ShowPlot"))
      {
         AppendItem(mXyPlotItem, objName, GmatTree::OUTPUT_ICON_XYPLOT, -1,
                    new GmatTreeItemData(objName, GmatTree::OUTPUT_XY_PLOT));
      }
   }

   // get list of Event Locators
   StringArray listOfEls = theGuiInterpreter->GetListOfObjects(Gmat::EVENT_LOCATOR);
   for (UnsignedInt i = 0; i < listOfEls.size(); ++i)
   {
      EventLocator *el =
         (EventLocator*)theGuiInterpreter->GetConfiguredObject(listOfEls[i]);
      if (el != NULL)
      {
         wxString objName = wxString(listOfEls[i].c_str());
         AppendItem(mEventsItem, objName, GmatTree::OUTPUT_ICON_REPORTFILE, -1,
               new GmatTreeItemData(objName, GmatTree::OUTPUT_EVENT_REPORT));
      }
   }

   Expand(mReportItem);
   Expand(mOrbitViewItem);
   Expand(mGroundTrackItem);
   Expand(mXyPlotItem);
   Expand(mEventsItem);
}

//------------------------------------------------------------------------------
//  void AddDefaultResources()
//------------------------------------------------------------------------------
/**
 * Add default folders
 */
//------------------------------------------------------------------------------
void OutputTree::AddDefaultResources()
{
   wxTreeItemId output =
      AddRoot(wxT("Output"), -1, -1,
              new GmatTreeItemData(wxT("Output"), GmatTree::OUTPUT_FOLDER));
   
   //----- Reports
   mReportItem =
      AppendItem(output, wxT("Reports"), GmatTree::OUTPUT_ICON_CLOSEDFOLDER, -1,
                 new GmatTreeItemData(wxT("Reports"),
                                      GmatTree::REPORTS_FOLDER));
   
   SetItemImage(mReportItem, GmatTree::OUTPUT_ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);
   
   //----- Orbit Views
   mOrbitViewItem =
      AppendItem(output, wxT("Orbit Views"), GmatTree::OUTPUT_ICON_CLOSEDFOLDER, -1,
                 new GmatTreeItemData(wxT("Orbit Views"),
                                      GmatTree::ORBIT_VIEWS_FOLDER));
   
   SetItemImage(mOrbitViewItem, GmatTree::OUTPUT_ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);
   
   //----- Ground Track Plots
   mGroundTrackItem =
      AppendItem(output, wxT("Ground Track Plots"), GmatTree::OUTPUT_ICON_CLOSEDFOLDER, -1,
                 new GmatTreeItemData(wxT("Ground Track Plots"),
                                      GmatTree::GROUND_TRACK_FOLDER));
   
   SetItemImage(mGroundTrackItem, GmatTree::OUTPUT_ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);
   
   //----- XY Plots
   mXyPlotItem =
      AppendItem(output, wxT("XY Plots"), GmatTree::OUTPUT_ICON_CLOSEDFOLDER, -1,
                 new GmatTreeItemData(wxT("XY Plots"),
                                      GmatTree::XY_PLOTS_FOLDER));
    
   SetItemImage(mXyPlotItem, GmatTree::OUTPUT_ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);

   //----- Event Reports
   mEventsItem =
      AppendItem(output, wxT("Events"), GmatTree::OUTPUT_ICON_CLOSEDFOLDER, -1,
                 new GmatTreeItemData(wxT("Events"),
                                      GmatTree::EVENTS_FOLDER));

   SetItemImage(mXyPlotItem, GmatTree::OUTPUT_ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);
}


//==============================================================================
//                         On Action Events
//==============================================================================

//------------------------------------------------------------------------------
// void OnItemRightClick(wxTreeEvent& event)
//------------------------------------------------------------------------------
/**
 * On right click show the pop up menu
 *
 */
//------------------------------------------------------------------------------
void OutputTree::OnItemRightClick(wxTreeEvent& event)
{
   //wxWidgets-2.6.3 does not need this but wxWidgets-2.8.0 needs to SelectItem
   SelectItem(event.GetItem());
   ShowMenu(event.GetItem(), event.GetPoint());
}


//------------------------------------------------------------------------------
// void ShowMenu(wxTreeItemId itemId, const wxPoint& pt)
//------------------------------------------------------------------------------
/**
 * Create the popup menu
 *
 * @param <itemId> tree item that was right clicked
 * @param <pt> location of right click
 */
//------------------------------------------------------------------------------
void OutputTree::ShowMenu(wxTreeItemId itemId, const wxPoint& pt)
{
   GmatTreeItemData *treeItem = (GmatTreeItemData *)GetItemData(itemId);
   theSubscriberName = treeItem->GetName();
   GmatTree::ItemType itemType = treeItem->GetItemType();
   
   #if DEBUG_OUTPUT_TREE
   MessageInterface::ShowMessage
      ("OutputTree::ShowMenu() theSubscriberName=%s\n", theSubscriberName.c_str());
   #endif
   
#if wxUSE_MENUS
   wxMenu menu;
   
   if (itemType == GmatTree::OUTPUT_REPORT)
   {
      menu.Append(POPUP_COMPARE_NUMERIC_VALUES, wxT("Compare Numeric Values"));
      menu.Append(POPUP_COMPARE_TEXT_LINES, wxT("Compare Text Lines"));
   }
   
   PopupMenu(&menu, pt);
#endif // wxUSE_MENUS
}


//------------------------------------------------------------------------------
// void OnItemActivated(wxTreeEvent &event)
//------------------------------------------------------------------------------
/**
 * Double click on tree item
 *
 * @param <event> tree event
 */
//------------------------------------------------------------------------------
void OutputTree::OnItemActivated(wxTreeEvent &event)
{
   // get some info about this item
   wxTreeItemId itemId = event.GetItem();
   GmatTreeItemData *item = (GmatTreeItemData *)GetItemData(itemId);
   item->SetTitle(GetItemText(itemId));
   GmatAppData::Instance()->GetMainFrame()->CreateChild(item);
}


//------------------------------------------------------------------------------
// void OnOpen(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Open chosen from popup menu
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void OutputTree::OnOpen(wxCommandEvent &event)
{
   // Get info from selected item
   GmatTreeItemData *item = (GmatTreeItemData *) GetItemData(GetSelection());
   GmatAppData::Instance()->GetMainFrame()->CreateChild(item);
}

//------------------------------------------------------------------------------
// void OnClose(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Close chosen from popup menu
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void OutputTree::OnClose(wxCommandEvent &event)
{
   // Get info from selected item
   GmatTreeItemData *item = (GmatTreeItemData *) GetItemData(GetSelection());
   
   // if its open, its activated
   GmatAppData *gmatAppData = GmatAppData::Instance();
   if (gmatAppData->GetMainFrame()->IsChildOpen(item))
      gmatAppData->GetMainFrame()->CloseActiveChild();
   else
      return;
}

//------------------------------------------------------------------------------
// void OnRename(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Rename chosen from popup menu
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void OutputTree::OnRename(wxCommandEvent &event)
{
   #if DEBUG_OUTPUT_TREE
   MessageInterface::ShowMessage("OutputTree::OnRename() entered\n");
   #endif
   
   wxTreeItemId item = GetSelection();
   GmatTreeItemData *selItem = (GmatTreeItemData *) GetItemData(item);
   wxString oldName = selItem->GetName();
   wxString newName = oldName;
   newName = wxGetTextFromUser(wxT("New name: "), wxT("Input Text"),
                               newName, this);
   
   if ( !newName.IsEmpty() && !(newName.IsSameAs(oldName)))
   {
      
   }

}


//------------------------------------------------------------------------------
// void OnDelete(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Delete chosen from popup menu
 *
 * @param <event> command event
 * @todo Finish this when items can be deleted from interpreter
 */
//------------------------------------------------------------------------------
void OutputTree::OnDelete(wxCommandEvent &event)
{
   event.Skip();
}


//------------------------------------------------------------------------------
// void OnBeginLabelEdit(wxTreeEvent &event)
//------------------------------------------------------------------------------
/**
 * Rename chosen from popup menu
 *
 * @param <event> tree event
 */
//------------------------------------------------------------------------------
void OutputTree::OnBeginLabelEdit(wxTreeEvent &event)
{
  
   GmatTreeItemData *selItem = (GmatTreeItemData *)
      GetItemData(event.GetItem());
                               
   //kind of redundant because OpenPage returns false for some
   //of the default folders
   if (GmatAppData::Instance()->GetMainFrame()->IsChildOpen(selItem))
   {
      event.Veto();
   }
}


//------------------------------------------------------------------------------
// void OnEndLabelEdit(wxTreeEvent &event)
//------------------------------------------------------------------------------
/**
 * Finished changing label on the tree
 *
 * @param <event> tree event
 */
//------------------------------------------------------------------------------
void OutputTree::OnEndLabelEdit(wxTreeEvent &event)
{
   wxString newLabel = event.GetLabel();
   
   // check to see if label is a single word
   if (newLabel.IsWord())
   {
      GmatTreeItemData *selItem = (GmatTreeItemData *)
         GetItemData(GetSelection());
      
      wxString oldLabel = selItem->GetName();
      selItem->SetName(newLabel);
   }
   else
   {
      event.Veto();
   }
}


//------------------------------------------------------------------------------
// void AddIcons()
//------------------------------------------------------------------------------
/**
 * Add icons for items in the tree
 */
//------------------------------------------------------------------------------
void OutputTree::AddIcons()
{
   #ifdef DEBUG_ADD_ICONS
   MessageInterface::ShowMessage
      ("OutputTree::AddIcons() entered, GmatTree::OUTPUT_ICON_COUNT=%d\n",
       GmatTree::OUTPUT_ICON_COUNT);
   #endif
   
   int sizeW = 16;
   int sizeH = 16;
   
   wxImageList *images = new wxImageList ( sizeW, sizeH, true );
   wxBitmap* bitmaps[GmatTree::OUTPUT_ICON_COUNT];
   int index = 0;
   long bitmapType = wxBITMAP_TYPE_PNG;

   // Note:
   // Add icons by the order of enum ResourceIconType in GmatTreeItemData.hpp

   theGuiManager->LoadIcon("ClosedFolder", bitmapType, &bitmaps[index], ClosedFolder_xpm); 
   theGuiManager->LoadIcon("OpenFolder", bitmapType, &bitmaps[++index], OpenFolder_xpm);
   theGuiManager->LoadIcon("rt_ReportFile", bitmapType, &bitmaps[++index], rt_ReportFile_xpm);
   theGuiManager->LoadIcon("rt_OrbitView", bitmapType, &bitmaps[++index], rt_OrbitView_xpm);
   theGuiManager->LoadIcon("rt_GroundTrackPlot", bitmapType, &bitmaps[++index], rt_GroundTrackPlot_xpm);
   theGuiManager->LoadIcon("rt_XYPlot", bitmapType, &bitmaps[++index], rt_XYPlot_xpm);
   theGuiManager->LoadIcon("file", bitmapType, &bitmaps[++index], file_xpm);   
   theGuiManager->LoadIcon("rt_Default", bitmapType, &bitmaps[++index], rt_Default_xpm);
   
   // Let's always rescale all icons since size of icon look different on different platforms
   for ( size_t i = 0; i < WXSIZEOF(bitmaps); i++ )
      images->Add(bitmaps[i]->ConvertToImage().Rescale(sizeW, sizeH));
   
   AssignImageList(images);
   
   #ifdef DEBUG_ADD_ICONS
   MessageInterface::ShowMessage
      ("OutputTree::AddIcons() exiting, %d icons added\n", index + 1);
   #endif
}

//---------------------------------
// Add items to tree
//---------------------------------
//------------------------------------------------------------------------------
// void OnAddReportFile(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a report file to reports folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void OutputTree::OnAddReportFile(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
  
   wxString name;
   name.Printf("ReportFile");

   if (theGuiInterpreter->CreateSubscriber
       ("ReportFile", std::string(name.c_str())) != NULL)
   {
      AppendItem(item, name, GmatTree::OUTPUT_ICON_REPORTFILE, -1,
                 new GmatTreeItemData(name, GmatTree::REPORT_FILE));

      Expand(item);
   }
}


//------------------------------------------------------------------------------
// void OnAddXyPlot(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an xy plot to plots folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void OutputTree::OnAddXyPlot(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
  
   wxString name;
   name.Printf("XYPlot");

   if (theGuiInterpreter->CreateSubscriber
       ("XYPlot", std::string(name.c_str())) != NULL)
   {
      AppendItem(item, name, GmatTree::OUTPUT_ICON_XYPLOT, -1,
                 new GmatTreeItemData(name, GmatTree::XY_PLOT));
      
      Expand(item);
   }
}


//------------------------------------------------------------------------------
// void OnAddOrbitView(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an opengl plot to plots folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void OutputTree::OnAddOrbitView(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   
   wxString name;
   name.Printf("OrbitView");
   
   if (theGuiInterpreter->CreateSubscriber
       ("OrbitView", std::string(name.c_str())) != NULL)
   {
      AppendItem(item, name, GmatTree::OUTPUT_ICON_ORBITVIEW, -1,
                 new GmatTreeItemData(name, GmatTree::ORBIT_VIEW));

      Expand(item);
   }
}


//------------------------------------------------------------------------------
// void OnCompareNumericValues(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Compare reports.
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void OutputTree::OnCompareNumericValues(wxCommandEvent &event)
{
   //MessageInterface::ShowMessage("OutputTree::OnCompareNumericValues() entered\n");

   ReportFile *theReport =
      (ReportFile*) theGuiInterpreter->GetConfiguredObject(theSubscriberName.c_str());

   if (!theReport)
   {
      MessageInterface::ShowMessage
         ("OutputTree::OnCompareNumericValues() The ReportFile: %s is NULL.\n",
          theSubscriberName.c_str());
      return;
   }
   
   std::string filename1 = theReport->GetPathAndFileName();
   StringArray colTitles = theReport->GetRefObjectNameArray(Gmat::PARAMETER);
   wxString filename2 =
      wxFileSelector("Choose a file to open", "", "", "report",
                     "Report files (*.report)|*.report|"
                     "Text files (*.txt)|*.txt|All files (*.*)|*.*");
   
   if (filename2.empty())
      return;
   
   Real tol = GmatFileUtil::CompareAbsTol;
   wxString tolStr;
   tolStr.Printf("%e", tol);
   tolStr = wxGetTextFromUser("Enter absolute tolerance to be used in flagging: ",
                              "Tolerance", tolStr, this);
   
   if (!tolStr.ToDouble(&tol))
   {
      wxMessageBox("Entered Invalid Tolerance", "Error", wxOK, this);
      return;
   }
   
   StringArray output =
      GmatFileUtil::Compare(filename1.c_str(), filename2.c_str(), colTitles, tol);
   
   ViewTextFrame *compWindow = GmatAppData::Instance()->GetCompareWindow();
   if (compWindow == NULL)
   {
      compWindow = 
         new ViewTextFrame(GmatAppData::Instance()->GetMainFrame(),
                           _T("Compare Utility"), 50, 50, 800, 500, "Permanent");
      GmatAppData::Instance()->SetCompareWindow(compWindow);
      wxString msg;
      msg.Printf(_T("GMAT Build Date: %s %s\n\n"),  __DATE__, __TIME__);  
      compWindow->AppendText(msg);
   }
   
   compWindow->Show(true);
   
   for (unsigned int i=0; i<output.size(); i++)
      compWindow->AppendText(wxString(output[i].c_str()));
}


//------------------------------------------------------------------------------
// void OnCompareTextLines(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Compare reports.
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void OutputTree::OnCompareTextLines(wxCommandEvent &event)
{
   //MessageInterface::ShowMessage("OutputTree::OnCompareTextLines() entered\n");

   ReportFile *theReport =
      (ReportFile*) theGuiInterpreter->GetConfiguredObject(theSubscriberName.c_str());
   
   if (!theReport)
   {
      MessageInterface::ShowMessage
         ("OutputTree::OnCompareTextLines() The ReportFile: %s is NULL.\n",
          theSubscriberName.c_str());
      return;
   }
   
   std::string filename1 = theReport->GetPathAndFileName();
   StringArray colTitles = theReport->GetRefObjectNameArray(Gmat::PARAMETER);
   wxString filename2 =
      wxFileSelector("Choose a file to open", "", "", "report",
                     "Report files (*.report)|*.report|"
                     "Text files (*.txt)|*.txt|All files (*.*)|*.*");
   
   if (filename2.empty())
      return;
   
   int file1DiffCount = 0;
   int file2DiffCount = 0;
   int file3DiffCount = 0;
   
   StringArray output =
      GmatFileUtil::CompareLines(1, filename1.c_str(), filename2.c_str(), "", "",
                                 file1DiffCount, file2DiffCount, file3DiffCount);
   
   ViewTextFrame *compWindow = GmatAppData::Instance()->GetCompareWindow();
   if (compWindow == NULL)
   {
      compWindow =
         new ViewTextFrame(GmatAppData::Instance()->GetMainFrame(),
                           _T("Compare Utility"), 50, 50, 800, 500, "Permanent");
      GmatAppData::Instance()->SetCompareWindow(compWindow);
      wxString msg;
      msg.Printf(_T("GMAT Build Date: %s %s\n\n"),  __DATE__, __TIME__);      
      compWindow->AppendText(msg);
   }
   
   compWindow->Show(true);
   
   for (unsigned int i=0; i<output.size(); i++)
      compWindow->AppendText(wxString(output[i].c_str()));
}
