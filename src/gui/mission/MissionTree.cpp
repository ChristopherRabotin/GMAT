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
#include "bitmaps/folder.xpm"
#include "bitmaps/openfolder.xpm"
#include "bitmaps/file.xpm"

#include "MissionTree.hpp"
#include "MissionTreeItemData.hpp"

#include "GmatAppData.hpp"
#include "Propagate.hpp"
#include "Maneuver.hpp"
#include "Target.hpp"
#include "MessageInterface.hpp"

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

   EVT_MENU(POPUP_ADD_MISSION_SEQ, MissionTree::OnAddMissionSeq)
   EVT_MENU(POPUP_ADD_MANEUVER, MissionTree::OnAddManeuver)
   EVT_MENU(POPUP_ADD_PROPAGATE, MissionTree::OnAddPropagate)
   EVT_MENU(POPUP_ADD_TARGET, MissionTree::OnAddTarget)
   
   EVT_MENU(POPUP_INSERT_MANEUVER, MissionTree::OnInsertManeuver)
   EVT_MENU(POPUP_INSERT_PROPAGATE, MissionTree::OnInsertPropagate)
   EVT_MENU(POPUP_INSERT_TARGET, MissionTree::OnInsertTarget)

   EVT_MENU(POPUP_VIEW_VARIABLES, MissionTree::OnViewVariables)
   EVT_MENU(POPUP_VIEW_GOALS, MissionTree::OnViewGoals)

   EVT_MENU(POPUP_RUN, MissionTree::OnRun)

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
    : DecoratedTree(parent, id, pos, size, style)
{
    this->parent = parent;
    mainNotebook = GmatAppData::GetMainNotebook();
    theGuiInterpreter = GmatAppData::GetGuiInterpreter();

//    this->SetNodes();
//    this->SetParameter(BOXCOUNT, 2);
//    this->SetParameter(BOXWIDTH, 20);

    numManeuver = 0;
    mNumMissionSeq = 0;
    mNumPropagate = 0;
    mNumManeuver = 0;
    mNumTarget = 0;

    AddIcons();
    AddDefaultMission();
}

// ag:  Removed because GmatAppData stores MainNotebook
////------------------------------------------------------------------------------
//// void SetMainNotebook (GmatMainNotebook *mainNotebook)
////------------------------------------------------------------------------------
///**
// * Sets GmatMainNotebook object.
// *
// * @param <mainNotebook> input GmatMainNotebook.
// */
////------------------------------------------------------------------------------
//void MissionTree::SetMainNotebook (GmatMainNotebook *mainNotebook)
//{
//  this->mainNotebook = mainNotebook;
//}
//
////------------------------------------------------------------------------------
//// GmatMainNotebook *GetMainNotebook()
////------------------------------------------------------------------------------
///**
// * Gets the GmatMainNotebook object.
// *
// * @return GmatMainNotebook object.
// */
////------------------------------------------------------------------------------
//GmatMainNotebook *MissionTree::GetMainNotebook()
//{
//   return this->mainNotebook;
//}

//------------------------------------------------------------------------------
// void UpdateMissionSeq()
//------------------------------------------------------------------------------
/**
 * Updates Mission Sequence
 */
//------------------------------------------------------------------------------
void MissionTree::UpdateMissionSeq()
{
    MessageInterface::ShowMessage("MissionTree::UpdateMissionSeq() entered\n");

    this->DeleteChildren(mMissionSeqSubItem);
    
    UpdateCommand();
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void UpdateCommand()
//------------------------------------------------------------------------------
/**
 * Updates commands in the mission sequence
 */
//------------------------------------------------------------------------------
void MissionTree::UpdateCommand()
{
    MessageInterface::ShowMessage("MissionTree::UpdateCommand() entered\n");

    GmatCommand *cmd = theGuiInterpreter->GetNextCommand();
    wxString objTypeName;
    
    while (cmd != NULL)
    {
        objTypeName = cmd->GetTypeName().c_str();
        MessageInterface::ShowMessage("MissionTree::UpdateCommand() objTypeName = " +
                                      std::string(objTypeName.c_str()) + "\n");
        
        if (objTypeName == "Propagate")
        {
            this->AppendItem(mMissionSeqSubItem, objTypeName, GmatTree::ICON_FILE, -1,
                             new MissionTreeItemData(objTypeName,
                                                     GmatTree::PROPAGATE_COMMAND,
                                                     objTypeName, (Propagate*)cmd));
        }
        else if (objTypeName == "Maneuver")
        {
            this->AppendItem(mMissionSeqSubItem, objTypeName, GmatTree::ICON_FILE, -1,
                             new MissionTreeItemData(objTypeName,
                                                     GmatTree::MANEUVER_COMMAND,
                                                     objTypeName, (Maneuver*)cmd));
        }
        else if (objTypeName == "Target")
        {
            this->AppendItem(mMissionSeqSubItem, objTypeName, GmatTree::ICON_FILE, -1,
                             new MissionTreeItemData(objTypeName,
                                                     GmatTree::TARGET_COMMAND,
                                                     objTypeName, (Target*)cmd));
        }

        cmd = cmd->GetNext();
    }

    Expand(mMissionSeqSubItem);
}

//------------------------------------------------------------------------------
// void AddDefaultMission()
//------------------------------------------------------------------------------
/**
 * Adds a default mission to tree.
 */
//------------------------------------------------------------------------------
void MissionTree::AddDefaultMission()
{
    wxTreeItemId mission = this->AddRoot(wxT("Mission"), -1, -1,
                                  new MissionTreeItemData(wxT("Mission"),
                                  GmatTree::MISSIONS_FOLDER));

    //----- Mission Sequence
    mMissionSeqTopItem =
        this->AppendItem(mission, wxT("MissionSequence"), GmatTree::ICON_FOLDER,
                         -1, new MissionTreeItemData(wxT("MissionSequence"),
                         GmatTree::MISSION_SEQ_TOP_FOLDER));
    
    SetItemImage(mMissionSeqTopItem, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

    AddDefaultMissionSeq(mMissionSeqTopItem);

    UpdateCommand();

    //loj: moved to UpdateCommand()
//      //loj: until Command *cmd is resolved, use GmatBase
//      GmatBase *cmd = (GmatBase*)theGuiInterpreter->GetNextCommand();
//      wxString objTypeName;
    
//      while (cmd != NULL)
//      {
//          objTypeName = cmd->GetTypeName().c_str();
        
//          if (objTypeName == "Propagate")
//          {
//              this->AppendItem(mMissionSeqSubItem, objTypeName, GmatTree::ICON_FILE, -1,
//                               new MissionTreeItemData(objTypeName,
//                                                       GmatTree::PROPAGATE_COMMAND,
//                                                       objTypeName, (Propagate*)cmd));
//          }
//          else if (objTypeName == "Maneuver")
//          {
//              this->AppendItem(mMissionSeqSubItem, objTypeName, GmatTree::ICON_FILE, -1,
//                               new MissionTreeItemData(objTypeName,
//                                                       GmatTree::MANEUVER_COMMAND,
//                                                       objTypeName, (Maneuver*)cmd));
//          }
//          else if (objTypeName == "Target")
//          {
//              this->AppendItem(mMissionSeqSubItem, objTypeName, GmatTree::ICON_FILE, -1,
//                               new MissionTreeItemData(objTypeName,
//                                                       GmatTree::TARGET_COMMAND,
//                                                       objTypeName, (Target*)cmd));
//          }

//          //loj: The following has to be resolved
//          //cmd = cmd->GetNext();
//      }

//      Expand(mMissionSeqSubItem);

}

//------------------------------------------------------------------------------
// void AddDefaultMissionSeq(wxTreeItemId item)
//------------------------------------------------------------------------------
void MissionTree::AddDefaultMissionSeq(wxTreeItemId item)
{    
    MessageInterface::ShowMessage("MissionTree::AddDefaultMission() entered\n");

    //loj: for multiple mission sequence for b2
    //StringArray itemNames = theGuiInterpreter->GetListOfConfiguredItems(Gmat::MISSION_SEQ);
    
    //int size = itemNames.size();
    //for (int i = 0; i<size; i++)
    //{
    //    wxString objName = wxString(itemNames[i].c_str());
    //    this->AppendItem(item, wxT(objName), GmatTree::ICON_FOLDER, -1,
    //                     new MissionTreeItemData(wxT(objName), GmatTree::MISSION_SEQ_COMMAND));
    //};

    wxString name;
    
    name.Printf("Sequence%d", ++mNumMissionSeq);
  
    mMissionSeqSubItem =
        this->AppendItem(item, name, GmatTree::ICON_FOLDER, -1,
                         new MissionTreeItemData(name, 
                                                 GmatTree::MISSION_SEQ_SUB_FOLDER));
    
    SetItemImage(mMissionSeqSubItem, GmatTree::ICON_OPENFOLDER, 
                 wxTreeItemIcon_Expanded);

    Expand(item);
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
  wxIcon icons[3];

  icons[0] = wxIcon ( folder_xpm );
  icons[1] = wxIcon ( file_xpm );
  icons[2] = wxIcon ( openfolder_xpm );

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
    MissionTreeItemData *item = (MissionTreeItemData *)GetItemData(itemId);

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
    MissionTreeItemData *treeItem = (MissionTreeItemData *)GetItemData(id);
    wxString title = treeItem->GetDesc();
    int dataType = treeItem->GetDataType();
    
#if wxUSE_MENUS
    wxMenu menu;
    
    if (dataType == GmatTree::MISSION_SEQ_TOP_FOLDER)
    {
        menu.Append(POPUP_ADD_MISSION_SEQ, wxT("Add Mission Sequence"));
    }
    else if (dataType == GmatTree::TARGET_COMMAND)
    {
        menu.Append(POPUP_ADD_COMMAND, wxT("Add"), CreatePopupMenu());
        menu.Append(POPUP_INSERT_COMMAND, wxT("Insert"), 
                    CreateInsertPopupMenu());
        menu.Append(POPUP_DELETE, wxT("Delete"));
        menu.Append(POPUP_RENAME, wxT("Rename"));
        menu.AppendSeparator();
        menu.Append(POPUP_VIEW_VARIABLES, wxT("View Variables"));
        menu.Append(POPUP_VIEW_GOALS, wxT("View Goals")); 
    } 
    else if (dataType == GmatTree::MISSION_SEQ_SUB_FOLDER)
    {
        menu.Append(POPUP_ADD_COMMAND, wxT("Add"), CreatePopupMenu());
        menu.Append(POPUP_DELETE, wxT("Delete"));
        menu.Append(POPUP_RENAME, wxT("Rename"));
        menu.AppendSeparator();
        menu.Append(POPUP_RUN, wxT("Run"));
    }
    else 
    {
        menu.Append(POPUP_INSERT_COMMAND, wxT("Insert"), 
                    CreateInsertPopupMenu());
        menu.Append(POPUP_DELETE, wxT("Delete"));
        menu.Append(POPUP_RENAME, wxT("Rename"));
    }
    
    PopupMenu(&menu, pt);
#endif // wxUSE_MENUS
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MissionTree::OnBefore ()
{
    before = true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MissionTree::OnAfter ()
{
    before = false;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MissionTree::OnAddMissionSeq(wxCommandEvent &event)
{
    wxTreeItemId item = GetSelection();
    wxString name;
    
    name.Printf("Sequence%d", ++mNumMissionSeq);
  
    mMissionSeqSubItem =
        this->AppendItem(item, name, GmatTree::ICON_FOLDER, -1,
                         new MissionTreeItemData(name, GmatTree::MISSION_SEQ_SUB_FOLDER));
    
    SetItemImage(mMissionSeqSubItem, GmatTree::ICON_OPENFOLDER, 
                 wxTreeItemIcon_Expanded);

    Expand(item);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MissionTree::OnAddPropagate(wxCommandEvent &event)
{
    wxTreeItemId item = GetSelection();
    wxString name;
    name.Printf("Propagate%d", ++mNumPropagate);

    GmatCommand *cmd =
        theGuiInterpreter->CreateCommand("Propagate", std::string(name.c_str()));
    
    if (cmd != NULL)
    {
        if (theGuiInterpreter->AppendCommand(cmd))
        {
            this->AppendItem(item, name, GmatTree::ICON_FILE, -1,
                             new MissionTreeItemData(name, GmatTree::PROPAGATE_COMMAND,
                                                     name, cmd));
            
            Expand(item);
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MissionTree::OnAddManeuver(wxCommandEvent &event)
{
    wxTreeItemId item = GetSelection();
    wxString name;
    name.Printf("Maneuver%d", ++mNumManeuver);
  
    GmatCommand *cmd =
        theGuiInterpreter->CreateCommand("Maneuver", std::string(name.c_str()));
    
    if (cmd != NULL)
    {
        if (theGuiInterpreter->AppendCommand(cmd))
        {   
            this->AppendItem(item, name, GmatTree::ICON_FILE, -1,
                  new MissionTreeItemData(name, GmatTree::MANEUVER_COMMAND, 
                  name, cmd));
            
            Expand(item);
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MissionTree::OnAddTarget(wxCommandEvent &event)
{
    wxTreeItemId item = GetSelection();
    wxString name;
    name.Printf("Target%d", ++mNumTarget);
  
    GmatCommand *cmd =
        theGuiInterpreter->CreateCommand("Target", std::string(name.c_str()));
    
    if (cmd != NULL)
    {
        if (theGuiInterpreter->AppendCommand(cmd))
        {
            wxTreeItemId targetId = this->AppendItem(item, name, GmatTree::ICON_FOLDER, -1,
                             new MissionTreeItemData(name, GmatTree::TARGET_COMMAND,
                                                     name, cmd));
            
            SetItemImage(targetId, GmatTree::ICON_OPENFOLDER, 
                 wxTreeItemIcon_Expanded);            

            Expand(item);
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MissionTree::OnInsertPropagate(wxCommandEvent &event)
{
    wxTreeItemId item = GetSelection();
    wxTreeItemId itemParent = GetItemParent(item);
    wxString name;
    name.Printf("Propagate%d", ++mNumPropagate); 
 
    item = GetPrevSibling(item);
    MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(item);

    if (prevItem->GetDataType() != GmatTree::MISSION_SEQ_SUB_FOLDER)
    {
        GmatCommand *prevCmd = prevItem->GetCommand();

        if (prevCmd != NULL)
        {
            GmatCommand *cmd =
                theGuiInterpreter->CreateCommand("Propagate", std::string(name.c_str()));

            if (theGuiInterpreter->InsertCommand(cmd, prevCmd))
            {
                this->InsertItem(itemParent, item, name, GmatTree::ICON_FILE, -1,
                                 new MissionTreeItemData(name, GmatTree::PROPAGATE_COMMAND,
                                                         name, cmd));
            }
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MissionTree::OnInsertManeuver(wxCommandEvent &event)
{
    wxTreeItemId item = GetSelection();
    wxTreeItemId itemParent = GetItemParent(item);
    wxString name;
    name.Printf("Maneuver%d", ++mNumManeuver);

    item = GetPrevSibling(item);
    MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(item);
  
    GmatCommand *prevCmd = prevItem->GetCommand();

    if (prevItem->GetDataType() != GmatTree::MISSION_SEQ_SUB_FOLDER)
    {
        if (prevCmd != NULL)
        {
            GmatCommand *cmd =
                theGuiInterpreter->CreateCommand("Maneuver", std::string(name.c_str()));
      
            if (theGuiInterpreter->InsertCommand(cmd, prevCmd))
            {  
                this->InsertItem(itemParent, item, name, GmatTree::ICON_FILE, -1,
                                 new MissionTreeItemData(name, GmatTree::MANEUVER_COMMAND,
                                                         name, cmd));
            }
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MissionTree::OnInsertTarget(wxCommandEvent &event)
{
    wxTreeItemId item = GetSelection();
    wxTreeItemId itemParent = GetItemParent(item);
    wxString name;
    name.Printf("Target%d", ++mNumTarget); 
 
    item = GetPrevSibling(item);
    MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(item);
  
    if (prevItem->GetDataType() != GmatTree::MISSION_SEQ_SUB_FOLDER)
    {
        GmatCommand *prevCmd = prevItem->GetCommand();
  
        if (prevCmd != NULL)
        {
            GmatCommand *cmd =
                theGuiInterpreter->CreateCommand("Target", std::string(name.c_str()));
      
            if (theGuiInterpreter->InsertCommand(cmd, prevCmd))
            {
                wxTreeItemId targetId =
                    this->InsertItem(itemParent, item, name, GmatTree::ICON_FOLDER, -1,
                                     new MissionTreeItemData(name, GmatTree::TARGET_COMMAND,
                                                             name, cmd));
  
                SetItemImage(targetId, GmatTree::ICON_OPENFOLDER, 
                             wxTreeItemIcon_Expanded);
      
                Expand(item);
            }
        }
    }
}

//---------------------------------
// Crete popup menu
//---------------------------------

//------------------------------------------------------------------------------
// wxMenu* CreatePopupMenu()
//------------------------------------------------------------------------------
wxMenu* MissionTree::CreatePopupMenu()
{
    //MessageInterface::ShowMessage("MissionTree::CreatePopupMenu() entered\n");
    unsigned int i;
    wxMenu *menu = new wxMenu;

    StringArray items = theGuiInterpreter->GetListOfFactoryItems(Gmat::COMMAND);


    for (i=0; i<items.size(); i++)
    {
        //MessageInterface::ShowMessage("command = " + items[i] + "\n");
        
        if (items[i] == "Propagate")
        {
            menu->Append(POPUP_ADD_PROPAGATE, wxT("Propagate")); //wxT(items[i].c_str()));
        }
        else if (items[i] == "Maneuver")
        {
            menu->Append(POPUP_ADD_MANEUVER, wxT("Maneuver"));
        }
        else if (items[i] == "Target")
        {
            menu->Append(POPUP_ADD_TARGET, wxT("Target"));
        }            
    }

    return menu;
}

//------------------------------------------------------------------------------
// wxMenu* CreateInsertPopupMenu()
//------------------------------------------------------------------------------
wxMenu* MissionTree::CreateInsertPopupMenu()
{
    //MessageInterface::ShowMessage("MissionTree::CreatePopupMenu() entered\n");
    unsigned int i;
    wxMenu *menu = new wxMenu;

    StringArray items = theGuiInterpreter->GetListOfFactoryItems(Gmat::COMMAND);

    for (i=0; i<items.size(); i++)
    {
        //MessageInterface::ShowMessage("command = " + items[i] + "\n");

        if (items[i] == "Propagate")
        {
            menu->Append(POPUP_INSERT_PROPAGATE, wxT("Propagate")); //wxT(items[i].c_str()));
        }
        else if (items[i] == "Maneuver")
        {
            menu->Append(POPUP_INSERT_MANEUVER, wxT("Maneuver"));
        }
        else if (items[i] == "Target")
        {
            menu->Append(POPUP_INSERT_TARGET, wxT("Target"));
        }
    }

    return menu;
}

//------------------------------------------------------------------------------
// void OnViewVariables()
//------------------------------------------------------------------------------
void MissionTree::OnViewVariables()
{
    MissionTreeItemData *item = new MissionTreeItemData(wxT("Variables"), 
                                                        GmatTree::VIEW_SOLVER_VARIABLES);
    mainNotebook->CreatePage(item);
}

//------------------------------------------------------------------------------
// void OnViewGoals()
//------------------------------------------------------------------------------
void MissionTree::OnViewGoals()
{
    MissionTreeItemData *item = new MissionTreeItemData(wxT("Goals"),
                                                        GmatTree::VIEW_SOLVER_GOALS);
    mainNotebook->CreatePage(item);
}

//---------------------------------------------------------------------------
// void MissionTree::OnRun()
//--------------------------------------------------------------------------
void MissionTree::OnRun()
{
    theGuiInterpreter->RunMission();
}








