//$Header$
//------------------------------------------------------------------------------
//                              OutputTree
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
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
#include "bitmaps/folder.xpm"
#include "bitmaps/openfolder.xpm"
#include "bitmaps/file.xpm"
#include "bitmaps/report.xpm"
#include "bitmaps/openglplot.xpm"
#include "bitmaps/xyplot.xpm"
#include "bitmaps/default.xpm"
#include <wx/string.h> // for wxArrayString

#include "GuiInterpreter.hpp"
#include "OutputTree.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"
#include "GmatTreeItemData.hpp"
#include "GmatMainFrame.hpp"

//#define DEBUG_RESOURCE_TREE 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(OutputTree, wxTreeCtrl)
   EVT_TREE_ITEM_RIGHT_CLICK(-1, OutputTree::OnItemRightClick)

   EVT_TREE_ITEM_ACTIVATED(-1, OutputTree::OnItemActivated)
   EVT_TREE_BEGIN_LABEL_EDIT(-1, OutputTree::OnBeginLabelEdit)
   EVT_TREE_END_LABEL_EDIT(-1, OutputTree::OnEndLabelEdit)
//   EVT_MENU(POPUP_OPEN, OutputTree::OnOpen)
//   EVT_MENU(POPUP_CLOSE, OutputTree::OnClose)
//   EVT_MENU(POPUP_RENAME, OutputTree::OnRename)
//   EVT_MENU(POPUP_DELETE, OutputTree::OnDelete)
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
   //mainNotebook = GmatAppData::GetMainNotebook();
   //mainFrame = GmatAppData::GetMainFrame();
   //MessageInterface::ShowMessage("got main notebook\n");
  
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
 
   AddIcons();
   AddDefaultResources();

   theGuiManager->UpdateAll();
}

//------------------------------------------------------------------------------
// void UpdateResource(bool resetCounter = true)
//------------------------------------------------------------------------------
/**
 * Reset counters, delete all nodes that are not folders, add default nodes
 */
//------------------------------------------------------------------------------
void OutputTree::UpdateResource(bool resetCounter)
{
#if DEBUG_RESOURCE_TREE
   MessageInterface::ShowMessage("OutputTree::UpdateResource() entered\n");
#endif

   if (resetCounter)
   {

   }

   theGuiManager->UpdateAll();
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
              new GmatTreeItemData(wxT("Output"), GmatTree::RESOURCES_FOLDER));

   //----- Reports
   mReportItem =
      AppendItem(output, wxT("Reports"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Reports"),
                                      GmatTree::SPACECRAFT_FOLDER));
    
   SetItemImage(mReportItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);

   //----- OpenGL Plots
    mOpenGlItem =
      AppendItem(output, wxT("OpenGL Plots"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("OpenGL PLOTS"),
                                      GmatTree::FORMATIONS_FOLDER));
    
   SetItemImage(mOpenGlItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);

   //----- XY Plots
   wxTreeItemId mXyPlotItem =
      AppendItem(output,
                 wxT("XY Plots"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("XY Plots"),
                                      GmatTree::CONSTELLATIONS_FOLDER));
    
   SetItemImage(mXyPlotItem, GmatTree::ICON_OPENFOLDER,
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
   wxString title = treeItem->GetDesc();
//   int dataType = treeItem->GetDataType();
    
#if wxUSE_MENUS
   wxMenu menu;
    
//      menu.Append(POPUP_OPEN, wxT("Open"));
//      menu.Append(POPUP_CLOSE, wxT("Close"));
//      menu.AppendSeparator();
//      menu.Append(POPUP_RENAME, wxT("Rename"));
//      menu.Append(POPUP_DELETE, wxT("Delete"));
//
//      menu.Enable(POPUP_DELETE, FALSE);

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

   //    mainNotebook->CreatePage(item);
   GmatAppData::GetMainFrame()->CreateChild(item);
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
   //    mainNotebook->CreatePage(item);
   GmatAppData::GetMainFrame()->CreateChild(item);
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
   if (GmatAppData::GetMainFrame()->IsChildOpen(item))
      // close the window
      GmatAppData::GetMainFrame()->CloseActiveChild();
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
   //MessageInterface::ShowMessage("OutputTree::OnRename() entered\n");


   wxTreeItemId item = GetSelection();
   GmatTreeItemData *selItem = (GmatTreeItemData *) GetItemData(item);
   wxString oldName = selItem->GetDesc();
//   int dataType = selItem->GetDataType();

   wxString newName = oldName;
   newName = wxGetTextFromUser(wxT("New name: "), wxT("Input Text"),
                               newName, this);

   //if newName != oldName
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
   //    wxTreeItemId item = GetSelection();
   //    wxTreeItemId parentId = GetPrevVisible(item);
   //    this->Collapse(parentId);
   //    
   //    // delete from gui interpreter
   //    GmatTreeItemData *gmatItem = (GmatTreeItemData *)GetItemData(item);
   ////    theGuiInterpreter->RemoveConfiguredItem("Spacecraft", gmatItem->GetDesc());
   //    
   //    this->Delete(item);
   //    
   //
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
                               
//   int dataType = selItem->GetDataType();

   //kind of redundant because OpenPage returns false for some
   //of the default folders
   if (GmatAppData::GetMainFrame()->IsChildOpen(selItem))  //||
//       (isDefaultFolder)                                   ||
//       (isDefaultItem))
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

      wxString oldLabel = selItem->GetDesc();
//      int itemType = selItem->GetDataType();

      selItem->SetDesc(newLabel);
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
   int size = 16;
  
   wxImageList *images = new wxImageList ( size, size, true );
  
   wxBusyCursor wait;
   wxIcon icons[7];

   icons[0] = wxIcon ( folder_xpm );
   icons[1] = wxIcon ( file_xpm );
   icons[2] = wxIcon ( openfolder_xpm );
   icons[3] = wxIcon ( report_xpm );
   icons[4] = wxIcon ( openglplot_xpm );
   icons[5] = wxIcon ( xyplot_xpm );
   icons[6] = wxIcon ( default_xpm );

   int sizeOrig = icons[0].GetWidth();
   for ( size_t i = 0; i < WXSIZEOF(icons); i++ )
   {
      if ( size == sizeOrig )
      {
         images->Add(icons[i]);
      }
      else
      {
         images->Add(wxBitmap(wxBitmap(icons[i]).ConvertToImage().Rescale(size, size)));
      }
   }

   AssignImageList(images);

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
      AppendItem(item, name, GmatTree::ICON_REPORT, -1,
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
      AppendItem(item, name, GmatTree::ICON_XY_PLOT, -1,
                 new GmatTreeItemData(name, GmatTree::XY_PLOT));
      
      Expand(item);
   }
}

//------------------------------------------------------------------------------
// void OnAddOpenGlPlot(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an opengl plot to plots folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void OutputTree::OnAddOpenGlPlot(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
  
   wxString name;
   name.Printf("OpenGLPlot");

   if (theGuiInterpreter->CreateSubscriber
       ("OpenGLPlot", std::string(name.c_str())) != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_OPEN_GL_PLOT, -1,
                 new GmatTreeItemData(name, GmatTree::OPENGL_PLOT));

      Expand(item);
   }
}

