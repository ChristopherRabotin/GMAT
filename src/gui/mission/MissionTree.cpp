//$Header$
//------------------------------------------------------------------------------
//                              MissionTree
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Allison Greene
// Created: 2003/09/02
/**
 * This class provides the tree for missions.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "bitmaps/file.xpm"

#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "MissionTree.hpp"
#include "MessageInterface.hpp"
#include "Command.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------

//------------------------------------------------------------------------------
// EVENT_TABLE(MissionTree, wxTreeCtrl)
//------------------------------------------------------------------------------
/**
 * Events Table for the menu and tool bar
 *
 * @note Indexes event handler functions.
 */
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(MissionTree, wxTreeCtrl)
   EVT_TREE_ITEM_RIGHT_CLICK(-1, MissionTree::OnItemRightClick)
   EVT_TREE_ITEM_ACTIVATED(-1, MissionTree::OnItemActivated)
   
   EVT_MENU_HIGHLIGHT(POPUP_SWAP_BEFORE, MissionTree::OnBefore)
   EVT_MENU_HIGHLIGHT(POPUP_SWAP_AFTER, MissionTree::OnAfter)

   EVT_MENU(POPUP_ADD_MANEUVER_BEFORE, MissionTree::OnAddManeuverBefore)
   EVT_MENU(POPUP_ADD_PROPAGATE_BEFORE, MissionTree::OnAddPropagateBefore)
   EVT_MENU(POPUP_ADD_TARGET_BEFORE, MissionTree::OnAddTargetBefore)
   
   EVT_MENU(POPUP_ADD_MANEUVER_AFTER, MissionTree::OnAddManeuverAfter)
   EVT_MENU(POPUP_ADD_PROPAGATE_AFTER, MissionTree::OnAddPropagateAfter)
   EVT_MENU(POPUP_ADD_TARGET_AFTER, MissionTree::OnAddTargetAfter)

END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// MissionTree(wxWindow *parent, const wxWindowID id,
//              const wxPoint &pos, const wxSize &size, long style)
//------------------------------------------------------------------------------
/**
 * Constructs MissionTree object.
 *
 * @param <parent> input parent.
 * @param <id> input id.
 * @param <pos> input position.
 * @param <size> input size.
 * @param <style> input style.
 *
 * @note Creates the tree for missions and adds a default mission.
 */
//------------------------------------------------------------------------------
MissionTree::MissionTree(wxWindow *parent, const wxWindowID id,
              const wxPoint &pos, const wxSize &size, long style)
              :DecoratedTree(parent, id, pos, size, style)
{
    theGuiInterpreter = GmatAppData::GetGuiInterpreter();
    this->parent = parent;
//    this->SetNodes();
    this->SetParameter(BOXCOUNT, 2);
    this->SetParameter(BOXWIDTH, 20);
    AddIcons();
    AddDefaultMission();
}

//------------------------------------------------------------------------------
// void SetMainNotebook (GmatMainNotebook *mainNotebook)
//------------------------------------------------------------------------------
/**
 * Sets GmatMainNotebook object.
 *
 * @param <mainNotebook> input GmatMainNotebook.
 */
//------------------------------------------------------------------------------
void MissionTree::SetMainNotebook (GmatMainNotebook *mainNotebook)
{
  this->mainNotebook = mainNotebook;
}

//------------------------------------------------------------------------------
// GmatMainNotebook *GetMainNotebook()
//------------------------------------------------------------------------------
/**
 * Gets the GmatMainNotebook object.
 *
 * @return GmatMainNotebook object.
 */
//------------------------------------------------------------------------------
GmatMainNotebook *MissionTree::GetMainNotebook()
{
   return this->mainNotebook;
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void AddDefaultMission()
//------------------------------------------------------------------------------
/**
 * Adds a default mission to tree.
 */
//------------------------------------------------------------------------------
void MissionTree::AddDefaultMission()
{
    wxTreeItemId resource = this->AddRoot(wxT("Mission"), -1, -1,
                                  new GmatTreeItemData(wxT("Mission"),
                                  MISSIONS_FOLDER));

    // hard code mission in for now
    this->AppendItem(resource, wxT("Propagate"), -1, -1,
          new GmatTreeItemData(wxT("Propagate"), DEFAULT_PROPAGATE_COMMAND));
}

//------------------------------------------------------------------------------
// void AddIcons()
//------------------------------------------------------------------------------
/**
 * Adds icons to a list, so that they can be used in the tree.
 */
//------------------------------------------------------------------------------
void MissionTree::AddIcons()
{
  int size = 16;

  wxImageList *images = new wxImageList ( size, size, true );

  wxBusyCursor wait;
  wxIcon icons[1];

  icons[0] = wxIcon ( file_xpm );

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

//------------------------------------------------------------------------------
// void OnItemRightClick(wxTreeEvent& event)
//------------------------------------------------------------------------------
/**
 * Brings up popup menu on a right click.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void MissionTree::OnItemRightClick(wxTreeEvent& event)
{
    ShowMenu(event.GetItem(), event.GetPoint());
}

//------------------------------------------------------------------------------
// void OnItemActivated(wxTreeEvent &event)
//------------------------------------------------------------------------------
/**
 * On a double click sends the TreeItemData to GmatMainNotebook to open a new
 * page.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void MissionTree::OnItemActivated(wxTreeEvent &event)
{
    // get some info about this item
    wxTreeItemId itemId = event.GetItem();
    GmatTreeItemData *item = (GmatTreeItemData *)GetItemData(itemId);

    mainNotebook->CreatePage(item);
}

//------------------------------------------------------------------------------
// void ShowMenu(wxTreeItemId id, const wxPoint& pt)
//------------------------------------------------------------------------------
/**
 * Creates and shows a popup menu.
 *
 * @param <id> input TreeItemId.
 * @param <pt> input location for popup menu.
 */
//------------------------------------------------------------------------------
void MissionTree::ShowMenu(wxTreeItemId id, const wxPoint& pt)
{
#if wxUSE_MENUS
    wxMenu *menu = new wxMenu;

    wxMenu *menuAddBefore = new wxMenu;;
    
    menuAddBefore->Append(POPUP_ADD_MANEUVER_BEFORE, wxT("Maneuver"), wxT(""), FALSE);
    menuAddBefore->Append(POPUP_ADD_PROPAGATE_BEFORE, wxT("Propagate"), wxT(""), FALSE);
    menuAddBefore->Append(POPUP_ADD_TARGET_BEFORE, wxT("Target"), wxT(""), FALSE);

    wxMenu *menuAddAfter = new wxMenu;;
    
    menuAddAfter->Append(POPUP_ADD_MANEUVER_AFTER, wxT("Maneuver"), wxT(""), FALSE);
    menuAddAfter->Append(POPUP_ADD_PROPAGATE_AFTER, wxT("Propagate"), wxT(""), FALSE);
    menuAddAfter->Append(POPUP_ADD_TARGET_AFTER, wxT("Target"), wxT(""), FALSE);

    menu->Append(POPUP_ADD_BEFORE, wxT("Insert Before"), menuAddBefore, wxT(""));
    menu->Append(POPUP_ADD_AFTER, wxT("Insert After"), menuAddAfter, wxT(""));
    menu->AppendSeparator();
    
    menu->Append(POPUP_SWAP_BEFORE, wxT("Swap Before"), wxT(""), FALSE);
    menu->Append(POPUP_SWAP_AFTER, wxT("Swap After"), wxT(""), FALSE);
    menu->AppendSeparator();
    
    menu->Append(POPUP_CUT, wxT("Cut"), wxT(""), FALSE);
    menu->Append(POPUP_COPY, wxT("Copy"), wxT(""), FALSE);
    menu->Append(POPUP_PASTE, wxT("Paste"), wxT(""), FALSE);
    menu->Append(POPUP_DELETE, wxT("Delete"), wxT(""), FALSE);
    
    PopupMenu(menu, pt);
    //PopupMenu(&menuAddEvent, pt);
#endif // wxUSE_MENUS
}

void MissionTree::OnBefore ()
{
    before = true;
}

void MissionTree::OnAfter ()
{
    before = false;
}

void MissionTree::OnAddManeuverBefore(wxCommandEvent &event)
{
  wxTreeItemId item = GetSelection();
  wxTreeItemId itemParent = GetItemParent(item);

  item = GetPrevSibling(item);
  
  this->InsertItem(itemParent, item, wxT("Maneuver"), -1, -1,
        new GmatTreeItemData(wxT("Maneuver"), MANEUVER_COMMAND));
}

void MissionTree::OnAddPropagateBefore(wxCommandEvent &event)
{
  wxTreeItemId item = GetSelection();
  wxTreeItemId itemParent = GetItemParent(item);
  
  item = GetPrevSibling(item);
  
  this->InsertItem(itemParent, item, wxT("Propagate"), -1, -1,
        new GmatTreeItemData(wxT("Propagate"), PROPAGATE_COMMAND));
}

void MissionTree::OnAddTargetBefore(wxCommandEvent &event)
{
  wxTreeItemId item = GetSelection();
  wxTreeItemId itemParent = GetItemParent(item);
  
  item = GetPrevSibling(item);
  
  this->InsertItem(itemParent, item, wxT("Target"), -1, -1,
        new GmatTreeItemData(wxT("Target"), TARGET_COMMAND));
}

void MissionTree::OnAddManeuverAfter(wxCommandEvent &event)
{
  wxTreeItemId item = GetSelection();
  wxTreeItemId itemParent = GetItemParent(item);

  this->InsertItem(itemParent, item, wxT("Maneuver"), -1, -1,
        new GmatTreeItemData(wxT("Maneuver"), MANEUVER_COMMAND));
}

void MissionTree::OnAddPropagateAfter(wxCommandEvent &event)
{
  wxTreeItemId item = GetSelection();
  wxTreeItemId itemParent = GetItemParent(item);
  
  this->InsertItem(itemParent, item, wxT("Propagate"), -1, -1,
        new GmatTreeItemData(wxT("Propagate"), PROPAGATE_COMMAND));
}

void MissionTree::OnAddTargetAfter(wxCommandEvent &event)
{
  wxTreeItemId item = GetSelection();
  wxTreeItemId itemParent = GetItemParent(item);
  
  this->InsertItem(itemParent, item, wxT("Target"), -1, -1,
        new GmatTreeItemData(wxT("Target"), TARGET_COMMAND));
}

//------------------------------------------------------------------------------
// void AddManeuver(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::AddManeuver()
{
  wxTreeItemId item = GetSelection();
  
  wxString withName;
  withName.Printf("Maneuver%d", numManeuver++);
  
/*  Command *theManeuver = theGuiInterpreter->
                  CreateCommand((const std::string) withName);

  wxString newName = wxT(theManeuver->GetName().c_str());
  
  this->AppendItem(item, newName, -1, -1,
              new GmatTreeItemData(newName, MANEUVER_COMMAND));
*/
}


