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
#include "GmatMainFrame.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_MISSION_TREE 1

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
   EVT_PAINT(DecoratedTree::OnPaint)
   EVT_UPDATE_UI(-1, DecoratedTree::OnPaint)
//    ag: 10/20/2004 Commented out so that the position of the click is not
//        checked to open up a panel from the variables/goals boxes
//   EVT_LEFT_DCLICK(MissionTree::OnDoubleClick)

   EVT_TREE_ITEM_RIGHT_CLICK(-1, MissionTree::OnItemRightClick)
   EVT_TREE_ITEM_ACTIVATED(-1, MissionTree::OnItemActivated)
   
   EVT_MENU_HIGHLIGHT(POPUP_SWAP_BEFORE, MissionTree::OnBefore)
   EVT_MENU_HIGHLIGHT(POPUP_SWAP_AFTER, MissionTree::OnAfter)

   EVT_MENU(POPUP_OPEN, MissionTree::OnOpen)
   EVT_MENU(POPUP_CLOSE, MissionTree::OnClose)

   EVT_MENU(POPUP_ADD_MISSION_SEQ, MissionTree::OnAddMissionSeq)
   EVT_MENU(POPUP_ADD_MANEUVER, MissionTree::OnAddManeuver)
   EVT_MENU(POPUP_ADD_PROPAGATE, MissionTree::OnAddPropagate)
   EVT_MENU(POPUP_ADD_TARGET, MissionTree::OnAddTarget)
   EVT_MENU(POPUP_ADD_ACHIEVE, MissionTree::OnAddAchieve)
   EVT_MENU(POPUP_ADD_VARY, MissionTree::OnAddVary)
   EVT_MENU(POPUP_ADD_IF_STATEMENT, MissionTree::OnAddIfStatement)
   EVT_MENU(POPUP_ADD_WHILE_LOOP, MissionTree::OnAddWhileLoop)
   EVT_MENU(POPUP_ADD_FOR_LOOP, MissionTree::OnAddForLoop)
   EVT_MENU(POPUP_ADD_D0_WHILE, MissionTree::OnAddDoWhile)
   EVT_MENU(POPUP_ADD_SWITCH_CASE, MissionTree::OnAddSwitchCase)
   EVT_MENU(POPUP_ADD_ELSE_IF_STATEMENT, MissionTree::OnAddElseIfStatement)
   EVT_MENU(POPUP_ADD_ELSE_STATEMENT, MissionTree::OnAddElseStatement)
   EVT_MENU(POPUP_ADD_FUNCTION, MissionTree::OnAddFunction)

   EVT_MENU(POPUP_INSERT_MANEUVER, MissionTree::OnInsertManeuver)
   EVT_MENU(POPUP_INSERT_PROPAGATE, MissionTree::OnInsertPropagate)
   EVT_MENU(POPUP_INSERT_TARGET, MissionTree::OnInsertTarget)
   EVT_MENU(POPUP_INSERT_ACHIEVE, MissionTree::OnInsertAchieve)
   EVT_MENU(POPUP_INSERT_VARY, MissionTree::OnInsertVary)
   EVT_MENU(POPUP_INSERT_IF_STATEMENT, MissionTree::OnInsertIfStatement)
   EVT_MENU(POPUP_INSERT_WHILE_LOOP, MissionTree::OnInsertWhileLoop)
   EVT_MENU(POPUP_INSERT_FOR_LOOP, MissionTree::OnInsertForLoop)
   EVT_MENU(POPUP_INSERT_D0_WHILE, MissionTree::OnInsertDoWhile)
   EVT_MENU(POPUP_INSERT_SWITCH_CASE, MissionTree::OnInsertSwitchCase)

   EVT_MENU(POPUP_VIEW_VARIABLES, MissionTree::OnViewVariables)
   EVT_MENU(POPUP_VIEW_GOALS, MissionTree::OnViewGoals)

   EVT_MENU(POPUP_RUN, MissionTree::OnRun)
   
   EVT_MENU(POPUP_DELETE, MissionTree::OnDelete)

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
   parent = parent;
   // mainNotebook = GmatAppData::GetMainNotebook();
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();

   // SetNodes();
   SetParameter(BOXCOUNT, 2);
   SetParameter(BOXWIDTH, 20);

   numManeuver = 0;
   mNumMissionSeq = 0;
   mNumPropagate = 0;
   mNumManeuver = 0;
   mNumTarget = 0;
   mNumAchieve = 0;
   mNumVary = 0;
   mNumSave = 0; //loj: 10/20/04 added Save, Toggle
   mNumToggle = 0;
   mNumIfStatement = 0;
   mNumWhileLoop = 0;
   mNumForLoop = 0;
   mNumDoWhile = 0;
   mNumSwitchCase = 0;
   mNumFunct = 0;

   AddIcons();
   AddDefaultMission();

   // Put "G" for goals and "V" for variables
   // does it right-to-left
   // should spaces be put in for those that don't have
   // any text?
   SetString(-1, "V");                                   
   SetString(-1, "G");  

   //    Initialize();
   //    SetNodes();
   //    ExpandAll();
}

//loj: 6/29/04 added resetCounter
//------------------------------------------------------------------------------
// void UpdateMission(bool resetCounter)
//------------------------------------------------------------------------------
/**
 * Updates Mission Sequence
 */
//------------------------------------------------------------------------------
void MissionTree::UpdateMission(bool resetCounter)
{
#if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::UpdateMission() entered\n");
#endif

   if (resetCounter)
   {
      numManeuver = 0;
      mNumMissionSeq = 0;
      mNumPropagate = 0;
      mNumManeuver = 0;
      mNumTarget = 0;
      mNumAchieve = 0;
      mNumVary = 0;
      mNumSave = 0;
      mNumToggle = 0;
      mNumIfStatement = 0;
      mNumWhileLoop = 0;
      mNumForLoop = 0;
      mNumDoWhile = 0;
      mNumSwitchCase = 0;
      mNumFunct = 0;
   }
   
   DeleteChildren(mMissionSeqSubItem);

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
#if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::UpdateCommand() entered\n");
   ShowCommands("InUpdateCommand()");
#endif

   GmatCommand *cmd = theGuiInterpreter->GetNextCommand();
   GmatCommand *child;
   MissionTreeItemData *seqItemData =
      (MissionTreeItemData *)GetItemData(mMissionSeqSubItem);
   wxTreeItemId node;
   
   if (cmd->GetTypeName() == "NoOp")
      seqItemData->SetCommand(cmd);

   while (cmd != NULL)
   {
      node = UpdateCommandTree(mMissionSeqSubItem, cmd);
      child = cmd->GetChildCommand(0);

      if (child != NULL)
      {
         ExpandChildCommand(node, cmd, child);
      }
      
      cmd = cmd->GetNext();
   }

   Expand(mMissionSeqSubItem);
}

//------------------------------------------------------------------------------
// wxTreeItemId& UpdateCommandTree(wxTreeItemId parent, GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Updates commands in the mission sequence
 */
//------------------------------------------------------------------------------
wxTreeItemId& MissionTree::UpdateCommandTree(wxTreeItemId parent,
                                             GmatCommand *cmd)
{
   wxString cmdTypeName = cmd->GetTypeName().c_str();
   
   if (cmdTypeName == "Propagate")
   {
      mNewTreeId = 
         AppendCommand(parent, GmatTree::ICON_FILE, GmatTree::PROPAGATE_COMMAND,
                       cmd, &mNumPropagate);

      Expand(parent);
   }
   else if (cmdTypeName == "Maneuver")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::ICON_FILE, GmatTree::MANEUVER_COMMAND,
                       cmd, &mNumManeuver);
   
      Expand(parent);
   }
   else if (cmdTypeName == "Target")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::ICON_FOLDER, GmatTree::TARGET_COMMAND,
                       cmd, &mNumTarget);
      
      SetItemImage(mNewTreeId, GmatTree::ICON_OPENFOLDER,
                   wxTreeItemIcon_Expanded);
      
      Expand(parent);
   }
   else if (cmdTypeName == "EndTarget")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::ICON_FILE, GmatTree::END_TARGET_COMMAND,
                       cmd, &mNumTarget, mNumTarget);
      
      Expand(parent);
   }
   else if (cmdTypeName == "Achieve")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::ICON_FILE, GmatTree::ACHIEVE_COMMAND,
                       cmd, &mNumAchieve);
      Expand(parent);
   }
   else if (cmdTypeName == "Vary")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::ICON_FILE, GmatTree::VARY_COMMAND,
                       cmd, &mNumVary);
      Expand(parent);
   }
   else if (cmdTypeName.CmpNoCase("Save") == 0) //loj: actual command is "save"
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::ICON_FILE, GmatTree::SAVE_COMMAND,
                       cmd, &mNumSave, mNumSave);
      
      Expand(parent);
   }
   else if (cmdTypeName.CmpNoCase("Toggle") == 0)
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::ICON_FILE, GmatTree::TOGGLE_COMMAND,
                       cmd, &mNumToggle, mNumToggle);
      
      Expand(parent);
   }
   else if (cmdTypeName == "For")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::ICON_FOLDER, GmatTree::FOR_CONTROL,
                       cmd, &mNumForLoop);
        
      SetItemImage(mNewTreeId, GmatTree::ICON_OPENFOLDER,
                   wxTreeItemIcon_Expanded);

      Expand(parent);
   }
   else if (cmdTypeName == "EndFor")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::ICON_FILE, GmatTree::END_FOR_CONTROL,
                       cmd, &mNumForLoop, mNumForLoop);
      
      Expand(parent);
   }
   else if (cmdTypeName == "If")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::ICON_FOLDER, GmatTree::IF_CONTROL,
                       cmd, &mNumIfStatement);
        
      SetItemImage(mNewTreeId, GmatTree::ICON_OPENFOLDER,
                   wxTreeItemIcon_Expanded);

      Expand(parent);
   }
   else if (cmdTypeName == "EndIf")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::ICON_FILE, GmatTree::END_IF_CONTROL,
                       cmd, &mNumIfStatement, mNumIfStatement);
        
      Expand(parent);
   }
   else if (cmdTypeName == "While")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::ICON_FOLDER, GmatTree::WHILE_CONTROL,
                       cmd, &mNumWhileLoop);
        
      SetItemImage(mNewTreeId, GmatTree::ICON_OPENFOLDER,
                   wxTreeItemIcon_Expanded);

      Expand(parent);
   }
   else if (cmdTypeName == "EndWhile")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::ICON_FILE, GmatTree::END_WHILE_CONTROL,
                       cmd, &mNumWhileLoop, mNumWhileLoop);
      
      Expand(parent);
   }

   return mNewTreeId;
}

//------------------------------------------------------------------------------
// void ExpandChildCommand(wxTreeItemId parent, GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Expands child commands in the mission sequence
 */
//------------------------------------------------------------------------------
void MissionTree::ExpandChildCommand(wxTreeItemId parent, GmatCommand *baseCmd,
                                     GmatCommand *cmd)
{
#if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::ExpandChildCommand() baseCmd=%s cmd=%s\n",
       baseCmd->GetTypeName().c_str(), cmd->GetTypeName().c_str());
#endif
   
   wxTreeItemId node;
   GmatCommand *childCmd;
   Integer i = 0;

   while (cmd != baseCmd)
   {
      node = UpdateCommandTree(parent, cmd);
      childCmd = cmd->GetChildCommand(i);

      while (childCmd != NULL)
      {
         ExpandChildCommand(node, cmd, childCmd);
         ++i;
         childCmd = cmd->GetChildCommand(i);
      }

      cmd = cmd->GetNext();
      i = 0;
   }
}

//------------------------------------------------------------------------------
// MissionTree::AppendCommand(wxTreeItemId parent, GmatTree::IconType icon,
//                            GmatTree::ItemType type, GmatCommand *cmd,
//                            int *cmdCount, int endCount)
//------------------------------------------------------------------------------
/**
 * Appends command to command list and/or command tree.
 */
//------------------------------------------------------------------------------
wxTreeItemId
MissionTree::AppendCommand(wxTreeItemId parent, GmatTree::IconType icon,
                           GmatTree::ItemType type, GmatCommand *cmd,
                           int *cmdCount, int endCount)
{
   wxString cmdTypeName = cmd->GetTypeName().c_str();
   wxString nodeName = cmd->GetName().c_str();
   
   wxTreeItemId node;
   
   // compose node name
   if (cmdTypeName.Contains("End"))
   {
      if (nodeName.Trim() == "")
         nodeName.Printf("%s%d", cmdTypeName.c_str(), endCount);
   }
   else
   {
      if (nodeName.Trim() == "")
         nodeName.Printf("%s%d", cmdTypeName.c_str(), ++(*cmdCount));      
   }

#if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::AppendCommand() cmdTypeName=%s, nodeName=%s\n",
       cmdTypeName.c_str(), nodeName.c_str());
#endif
   
   node = AppendItem(parent, nodeName, icon, -1,
                     new MissionTreeItemData(nodeName, type, nodeName, cmd));
   
   return node;
}

//------------------------------------------------------------------------------
// wxTreeItemId InsertCommand(wxTreeItemId parentId, wxTreeItemId currId,
//                            wxTreeItemId prevId, GmatTree::IconType icon,
//                            GmatTree::ItemType type, GmatCommand *prevCmd,
//                            GmatCommand *cmd, int *cmdCount, int endCount = 0)
//------------------------------------------------------------------------------
/**
 * Inserts tree item.
 */
//------------------------------------------------------------------------------
wxTreeItemId
MissionTree::InsertCommand(wxTreeItemId parentId, wxTreeItemId currId,
                           wxTreeItemId prevId, GmatTree::IconType icon,
                           GmatTree::ItemType type, GmatCommand *prevCmd,
                           GmatCommand *cmd, int *cmdCount, int endCount)
{
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(currId);   
   GmatCommand *currCmd = currItem->GetCommand();
   wxString typeName = cmd->GetTypeName().c_str();
   wxString nodeName = cmd->GetName().c_str();
   wxTreeItemId node;
   bool cmdAdded = false;
   
   //-----------------------------------
   // Compose node name
   //-----------------------------------
   if (typeName.Contains("End"))
   {
      if (nodeName.Trim() == "")
         nodeName.Printf("%s%d", typeName.c_str(), endCount);
   }
   else if (nodeName.Trim() == "")
   {
      nodeName.Printf("%s%d", typeName.c_str(), ++(*cmdCount));
   }
   
#if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::InsertCommand() currCmd=%s, nodeName=%s\n",
       currCmd->GetTypeName().c_str(), nodeName.c_str());
#endif
   
   //------------------------------------------------------------
   // Insert to Top command
   //------------------------------------------------------------
   if (currCmd->GetTypeName() == "NoOp")
   {
#if DEBUG_MISSION_TREE
      MessageInterface::ShowMessage("----- Appending command...\n");
#endif
      // Append to base command list
      cmdAdded = theGuiInterpreter->AppendCommand(cmd);

      if (cmdAdded)
      {
         node = AppendItem(currId, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, type, nodeName, cmd));
      }

      return node;
   }

   
   //------------------------------------------------------------
   // Insert to Branch command
   //------------------------------------------------------------
   wxString prevTypeName = prevCmd->GetTypeName().c_str();

   // Insert to base command list
#if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("----- Inserting command...\n");
#endif

   cmdAdded = theGuiInterpreter->InsertCommand(cmd, prevCmd);
   
   if (cmdAdded)
   {
      //if (currId == prevId)
      if ((prevTypeName == "NoOp" || prevTypeName == "Target" ||
           prevTypeName == "For"  ||  prevTypeName == "While" ||
           prevTypeName == "If")) //loj: add Do, Switch later
      {
         node = InsertItem(parentId, 0, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, type, nodeName, cmd));
      }
      else
      {
         node = InsertItem(parentId, prevId, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, type, nodeName, cmd));
      }
   }
   
   return node;
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
   wxTreeItemId mission = AddRoot(wxT("Mission"), -1, -1,
                                  new MissionTreeItemData(wxT("Mission"),
                                                          GmatTree::MISSIONS_FOLDER));

   //----- Mission Sequence
   mMissionSeqTopItem =
      AppendItem(mission, wxT("MissionSequence"), GmatTree::ICON_FOLDER,
                 -1, new MissionTreeItemData(wxT("MissionSequence"),
                                             GmatTree::MISSION_SEQ_TOP_FOLDER));
    
   SetItemImage(mMissionSeqTopItem, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

   AddDefaultMissionSeq(mMissionSeqTopItem);

   UpdateCommand();

}

//------------------------------------------------------------------------------
// void AddDefaultMissionSeq(wxTreeItemId item)
//------------------------------------------------------------------------------
void MissionTree::AddDefaultMissionSeq(wxTreeItemId item)
{
#if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::AddDefaultMission() entered\n");
#endif
   
   //loj: for multiple mission sequence for b2
   //StringArray itemNames = theGuiInterpreter->GetListOfConfiguredItems(Gmat::MISSION_SEQ);
    
   //int size = itemNames.size();
   //for (int i = 0; i<size; i++)
   //{
   //    wxString objName = wxString(itemNames[i].c_str());
   //    AppendItem(item, wxT(objName), GmatTree::ICON_FOLDER, -1,
   //                     new MissionTreeItemData(wxT(objName), GmatTree::MISSION_SEQ_COMMAND));
   //};

   wxString name;
    
   name.Printf("Sequence%d", ++mNumMissionSeq);
  
   mMissionSeqSubItem =
      AppendItem(item, name, GmatTree::ICON_FOLDER, -1,
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
 * On a double click sends the TreeItemData to GmatMainFrame to open a new
 * window.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void MissionTree::OnItemActivated(wxTreeEvent &event)
{
   //MessageInterface::ShowMessage("MissionTree::OnItemActivated() entered\n");

   // get some info about this item
   wxTreeItemId itemId = event.GetItem();
   MissionTreeItemData *item = (MissionTreeItemData *)GetItemData(itemId);

   GmatAppData::GetMainFrame()->CreateChild(item);
}

//------------------------------------------------------------------------------
// void OnDoubleClick(wxMouseEvent &event)
//------------------------------------------------------------------------------
/**
 * On a double click ...
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void MissionTree::OnDoubleClick(wxMouseEvent &event)
{
   //MessageInterface::ShowMessage("MissionTree::OnDoubleClick() entered\n");
   wxPoint position = event.GetPosition();
   //MessageInterface::ShowMessage("Event position is %d %d\n", position.x, position.y );
   CheckClickIn(position);
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
   wxTreeItemId parent = GetItemParent(id);
   MissionTreeItemData *parentItem = (MissionTreeItemData *)GetItemData(parent);
   int parentDataType = parentItem->GetDataType();
   
#if wxUSE_MENUS
   wxMenu menu;

   if (dataType == GmatTree::MISSION_SEQ_TOP_FOLDER)
   {
      menu.Append(POPUP_ADD_MISSION_SEQ, wxT("Add Mission Sequence"));
      menu.Enable(POPUP_ADD_MISSION_SEQ, FALSE);
   }
   else if (dataType == GmatTree::MISSION_SEQ_SUB_FOLDER)
   {
      menu.Append(POPUP_ADD_COMMAND, wxT("Add"), CreateAddPopupMenu());
      // ag: can't delete because sys. doesn't handle multiple sequences yet
      // menu.Append(POPUP_DELETE, wxT("Delete"));
      menu.Append(POPUP_RENAME, wxT("Rename"));
      menu.Enable(POPUP_RENAME, FALSE);
      menu.AppendSeparator();
      menu.Append(POPUP_RUN, wxT("Run"));
   }
   else
   {
      menu.Append(POPUP_OPEN, wxT("Open"));
      menu.Append(POPUP_CLOSE, wxT("Close"));
      menu.AppendSeparator();
      
      if (dataType == GmatTree::TARGET_COMMAND)
      {
         menu.Append(POPUP_ADD_COMMAND, wxT("Add"), CreateTargetPopupMenu(false));
         if (parentDataType == GmatTree::TARGET_COMMAND)
            menu.Append(POPUP_INSERT_COMMAND, wxT("Insert"), CreateTargetPopupMenu(true));
         else
            menu.Append(POPUP_INSERT_COMMAND, wxT("Insert"), CreateInsertPopupMenu());
         menu.Append(POPUP_DELETE, wxT("Delete"));
         menu.Append(POPUP_RENAME, wxT("Rename"));
         menu.Enable(POPUP_RENAME, FALSE);
         menu.AppendSeparator();
         menu.Append(POPUP_VIEW_VARIABLES, wxT("View Variables"));
         menu.Append(POPUP_VIEW_GOALS, wxT("View Goals")); 
      } 
      else if (dataType == GmatTree::END_TARGET_COMMAND)
      {
         menu.Append(POPUP_ADD_COMMAND, wxT("Add"), CreateAddPopupMenu());
         menu.Append(POPUP_INSERT_COMMAND, wxT("Insert"), CreateTargetPopupMenu(true));
         menu.Append(POPUP_DELETE, wxT("Delete"));
         menu.Append(POPUP_RENAME, wxT("Rename"));
         menu.Enable(POPUP_RENAME, FALSE);
      } 
      else if ((dataType == GmatTree::WHILE_CONTROL) ||
               (dataType == GmatTree::DO_CONTROL) ||
               (dataType == GmatTree::FOR_CONTROL) ||
               (dataType == GmatTree::ELSE_IF_CONTROL) ||
               (dataType == GmatTree::ELSE_CONTROL))   
      {
         menu.Append(POPUP_ADD_COMMAND, wxT("Add"), CreateAddPopupMenu());
         menu.Append(POPUP_INSERT_COMMAND, wxT("Insert"), CreateInsertPopupMenu());
         menu.Append(POPUP_DELETE, wxT("Delete"));
         menu.Append(POPUP_RENAME, wxT("Rename"));
         menu.Enable(POPUP_RENAME, FALSE);
      } 
      else if (dataType == GmatTree::IF_CONTROL) 
      {
         menu.Append(POPUP_ADD_COMMAND, wxT("Add"), CreateAddIfPopupMenu());
         menu.Append(POPUP_INSERT_COMMAND, wxT("Insert"), CreateInsertPopupMenu());
         menu.Append(POPUP_DELETE, wxT("Delete"));
         menu.Append(POPUP_RENAME, wxT("Rename"));
         menu.Enable(POPUP_RENAME, FALSE);
      } 
      else 
      {
         if (parentDataType == GmatTree::TARGET_COMMAND)
            menu.Append(POPUP_INSERT_COMMAND, wxT("Insert"), CreateTargetPopupMenu(true));
         else
            menu.Append(POPUP_INSERT_COMMAND, wxT("Insert"), CreateInsertPopupMenu());
         menu.Append(POPUP_DELETE, wxT("Delete"));
         menu.Append(POPUP_RENAME, wxT("Rename"));
         menu.Enable(POPUP_RENAME, FALSE);
      }
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
// void OnAddMissionSeq(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddMissionSeq(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   wxString name;
    
   name.Printf("Sequence%d", ++mNumMissionSeq);
  
   mMissionSeqSubItem =
      AppendItem(item, name, GmatTree::ICON_FOLDER, -1,
                 new MissionTreeItemData(name, GmatTree::MISSION_SEQ_SUB_FOLDER));
    
   SetItemImage(mMissionSeqSubItem, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);

   Expand(item);
}

//------------------------------------------------------------------------------
// void OnAddPropagate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddPropagate(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnAddPropagate()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId lastId = GetLastChild(itemId);
   wxTreeItemId prevId = GetPrevVisible(lastId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   

   GmatCommand *prevCmd = prevItem->GetCommand();

   if (prevCmd != NULL)
   {
      GmatCommand *cmd = theGuiInterpreter->CreateDefaultCommand("Propagate");

      if (cmd != NULL)
      {
         InsertCommand(itemId, itemId, prevId, GmatTree::ICON_FILE,
                       GmatTree::PROPAGATE_COMMAND, prevCmd, cmd, &mNumPropagate);
      
         Expand(itemId);
      }
   }
   
#if DEBUG_MISSION_TREE
   ShowCommands("After OnAddPropagate()");
#endif
}

//------------------------------------------------------------------------------
// void OnAddManeuver(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddManeuver(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnAddManeuver()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId lastId = GetLastChild(itemId);
   wxTreeItemId prevId = GetPrevVisible(lastId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   

   GmatCommand *prevCmd = prevItem->GetCommand();

#if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::OnAddManeuver() prevCmd=%s\n",
                                 prevCmd->GetTypeName().c_str());
#endif
   
   if (prevCmd != NULL)
   {
      GmatCommand *cmd = theGuiInterpreter->CreateDefaultCommand("Maneuver");

      if (cmd != NULL)
      {
         InsertCommand(itemId, itemId, prevId, GmatTree::ICON_FILE,
                       GmatTree::MANEUVER_COMMAND, prevCmd, cmd, &mNumManeuver);
      
         Expand(itemId);
      }
   }
   
#if DEBUG_MISSION_TREE
   ShowCommands("After OnAddManeuver()");
#endif
}

//------------------------------------------------------------------------------
// void OnAddTarget(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddTarget(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnAddTarget()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId lastId = GetLastChild(itemId);
   wxTreeItemId prevId = GetPrevVisible(lastId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   

   GmatCommand *prevCmd = prevItem->GetCommand();

#if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::OnAddTarget() prevCmd=%s\n",
                                 prevCmd->GetTypeName().c_str());
#endif
   
   if (prevCmd != NULL)
   {
      GmatCommand *cmd = theGuiInterpreter->CreateCommand("Target");
      GmatCommand *endCmd = theGuiInterpreter->CreateCommand("EndTarget");

      if (cmd != NULL && endCmd != NULL)
      {
         wxTreeItemId node =
            InsertCommand(itemId, itemId, prevId, GmatTree::ICON_FOLDER,
                          GmatTree::TARGET_COMMAND, prevCmd, cmd, &mNumTarget);
      
         SetItemImage(node, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

         // Append EndTarget
         InsertCommand(node, node, node, GmatTree::ICON_FILE,
                       GmatTree::END_TARGET_COMMAND,
                       cmd, endCmd, &mNumTarget, mNumTarget);

         Expand(itemId);
         Expand(node);
      }
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnAddTarget()");
#endif
}

//------------------------------------------------------------------------------
// void OnAddAchieve(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddAchieve(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnAddAchieve()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId lastId = GetLastChild(itemId);
   wxTreeItemId prevId = GetPrevVisible(lastId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   

   GmatCommand *prevCmd = prevItem->GetCommand();

   if (prevCmd != NULL)
   {
      GmatCommand *cmd = theGuiInterpreter->CreateDefaultCommand("Achieve");
    
      if (cmd != NULL)
      {
         InsertCommand(itemId, itemId, prevId, GmatTree::ICON_FILE,
                       GmatTree::ACHIEVE_COMMAND, prevCmd, cmd, &mNumAchieve);

         Expand(itemId);
      }
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnAddAchieve()");
#endif
}

//------------------------------------------------------------------------------
// void :OnAddVary(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddVary(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnAddVary()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId lastId = GetLastChild(itemId);
   wxTreeItemId prevId = GetPrevVisible(lastId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   

   GmatCommand *prevCmd = prevItem->GetCommand();

   if (prevCmd != NULL)
   {
      GmatCommand *cmd = theGuiInterpreter->CreateDefaultCommand("Vary");
   
      if (cmd != NULL)
      {
         InsertCommand(itemId, itemId, prevId, GmatTree::ICON_FILE,
                       GmatTree::VARY_COMMAND, prevCmd, cmd, &mNumVary);

         Expand(itemId);
      }
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnAddVary()");
#endif
}

//------------------------------------------------------------------------------
// void OnAddIfStatement(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddIfStatement(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnAddIfStatement()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId lastId = GetLastChild(itemId);
   wxTreeItemId prevId = GetPrevVisible(lastId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   
  
   GmatCommand *prevCmd = prevItem->GetCommand();

   if (prevCmd != NULL)
   {
      //loj: 10/14/04 finished the code
      GmatCommand *cmd = theGuiInterpreter->CreateCommand("If");
      GmatCommand *endCmd = theGuiInterpreter->CreateCommand("EndIf");
   
      if (cmd != NULL && endCmd != NULL)
      {
         wxTreeItemId node =
            InsertCommand(itemId, itemId, prevId, GmatTree::ICON_FOLDER,
                          GmatTree::IF_CONTROL, prevCmd, cmd, &mNumIfStatement);
         
         SetItemImage(node, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded); 

         // Append EndIf 
         InsertCommand(node, node, node, GmatTree::ICON_FILE,
                       GmatTree::END_IF_CONTROL,
                       cmd, endCmd, &mNumIfStatement, mNumIfStatement);
      
         Expand(itemId);
         Expand(node);
      }
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnAddIfStatement()");
#endif
}

//------------------------------------------------------------------------------
// void OnAddWhileLoop(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddWhileLoop(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnAddWhileLoop()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId lastId = GetLastChild(itemId);
   wxTreeItemId prevId = GetPrevVisible(lastId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   

   GmatCommand *prevCmd = prevItem->GetCommand();

   if (prevCmd != NULL)
   {
      //loj: 10/14/04 finished the code
      GmatCommand *cmd = theGuiInterpreter->CreateCommand("While");
      GmatCommand *endCmd = theGuiInterpreter->CreateCommand("EndWhile");
   
      if (cmd != NULL && endCmd != NULL)
      {
         wxTreeItemId node =
            InsertCommand(itemId, itemId, prevId, GmatTree::ICON_FOLDER,
                          GmatTree::WHILE_CONTROL, prevCmd, cmd, &mNumWhileLoop);
      
         SetItemImage(node, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded); 

         // Append EndWhile
         InsertCommand(node, node, node, GmatTree::ICON_FILE,
                       GmatTree::END_WHILE_CONTROL, 
                       cmd, endCmd, &mNumWhileLoop, mNumWhileLoop);
         
         Expand(itemId);
         Expand(node);
      }
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnAddWhileLoop()");
#endif
}

//------------------------------------------------------------------------------
// void OnAddForLoop(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddForLoop(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnAddForLoop()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId lastId = GetLastChild(itemId);
   wxTreeItemId prevId = GetPrevVisible(lastId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   

   GmatCommand *prevCmd = prevItem->GetCommand();

   if (prevCmd != NULL)
   {
      //loj: 10/14/04 finished the code
      GmatCommand *cmd = theGuiInterpreter->CreateCommand("For");
      GmatCommand *endCmd = theGuiInterpreter->CreateCommand("EndFor");
   
      if (cmd != NULL && endCmd != NULL)
      {
         wxTreeItemId node =
            InsertCommand(itemId, itemId, prevId, GmatTree::ICON_FOLDER,
                          GmatTree::FOR_CONTROL, prevCmd, cmd, &mNumForLoop);
         
         SetItemImage(node, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded); 

         // Append EndFor
         InsertCommand(node, node, node, GmatTree::ICON_FILE,
                       GmatTree::END_FOR_CONTROL, 
                       cmd, endCmd, &mNumForLoop, mNumForLoop);
         
         Expand(itemId);
         Expand(node);
      }
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnAddForLoop()");
#endif
}

//------------------------------------------------------------------------------
// void OnAddDoWhile(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddDoWhile(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnAddDoWhile()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId lastId = GetLastChild(itemId);
   wxTreeItemId prevId = GetPrevVisible(lastId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   

   GmatCommand *prevCmd = prevItem->GetCommand();

   if (prevCmd != NULL)
   {
      //loj: 10/14/04 finished the code
      GmatCommand *cmd = theGuiInterpreter->CreateCommand("Do");
      GmatCommand *endCmd = theGuiInterpreter->CreateCommand("EndDo");
   
      if (cmd != NULL && endCmd != NULL)
      {
         wxTreeItemId node =
            InsertCommand(itemId, itemId, prevId, GmatTree::ICON_FOLDER,
                          GmatTree::DO_CONTROL, prevCmd, cmd, &mNumDoWhile);
         
         SetItemImage(node, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded); 

         // Append EndDo
         InsertCommand(node, node, node, GmatTree::ICON_FILE,
                       GmatTree::END_DO_CONTROL, 
                       cmd, endCmd, &mNumDoWhile, mNumDoWhile);
         
         Expand(itemId);
         Expand(node);
      }  
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnAddDoWhile()");
#endif
}

//------------------------------------------------------------------------------
// void OnAddSwitchCase(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddSwitchCase(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnAddSwitchCase()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId lastId = GetLastChild(itemId);
   wxTreeItemId prevId = GetPrevVisible(lastId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   

   GmatCommand *prevCmd = prevItem->GetCommand();

   if (prevCmd != NULL)
   {
      //loj: 10/14/04 finished the code
      GmatCommand *cmd = theGuiInterpreter->CreateCommand("Switch");
      GmatCommand *endCmd = theGuiInterpreter->CreateCommand("EndSwitch");
      
      if (cmd != NULL && endCmd != NULL)
      {
         wxTreeItemId node =
            InsertCommand(itemId, itemId, prevId, GmatTree::ICON_FOLDER,
                          GmatTree::SWITCH_CONTROL, prevCmd, cmd, &mNumSwitchCase);
         
         SetItemImage(node, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded); 
         
         // Append EndSwitch
         InsertCommand(node, node, node, GmatTree::ICON_FILE,
                       GmatTree::END_SWITCH_CONTROL, 
                       cmd, endCmd, &mNumSwitchCase, mNumSwitchCase);
         
         Expand(itemId);
         Expand(node);
      }  
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnAddSwitchCase()");
#endif
}

//------------------------------------------------------------------------------
// void OnAddElseIfStatement(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddElseIfStatement(wxCommandEvent &event)
{
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parent = GetItemParent(itemId);
   
   // go through the items in the command sequence to find either
   // last else or the end
   GmatTreeItemData *itemData = (GmatTreeItemData *)GetItemData(itemId);
   while ( (itemData->GetDataType() != GmatTree::ELSE_CONTROL) &&
           (itemData->GetDataType() != GmatTree::END_IF_CONTROL) )
   {
//      MessageInterface::ShowMessage("The cur sibling is %s\n", itemData->GetDesc().c_str());
      itemId = GetNextSibling(itemId);  
      itemData = (GmatTreeItemData *)GetItemData(itemId);
      MessageInterface::ShowMessage("The next sibling is %s\n", itemData->GetDesc().c_str());
   }   
   
   // add the "else if" to the sibling before that last else or end
   itemId = GetPrevSibling(itemId);
   
   wxString name;
   name.Printf("Else If%d", ++mNumIfStatement);
   //    endName.Printf("End If%d", mNumIfStatement);
   //    ++mNumIfStatement;
   // ag: need gui interpreter to get control logic
   //    GmatCommand *cmd =
   //        theGuiInterpreter->CreateCommand("Maneuver", std::string(name.c_str()));
   //    
   //    if (cmd != NULL)
   //    {
   //        if (theGuiInterpreter->AppendCommand(cmd))
   //        {   
   wxTreeItemId targetId =
      InsertItem(parent, itemId, name, GmatTree::ICON_FOLDER, -1,
                 new MissionTreeItemData(name, GmatTree::ELSE_IF_CONTROL, 
                                         name, NULL));
   SetItemImage(targetId, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);    
   //            AppendItem(item, endName, GmatTree::ICON_FILE, -1,
   //                       new MissionTreeItemData(endName, GmatTree::END_CONTROL, 
   //                                               endName, NULL));

   Expand(itemId);
   //        }
   //    }
}

//------------------------------------------------------------------------------
// void OnAddElseStatement(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddElseStatement(wxCommandEvent &event)
{
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parent = GetItemParent(itemId);
   
   // go through the items in the command sequence to find either
   // last else or the end
   GmatTreeItemData *itemData = (GmatTreeItemData *)GetItemData(itemId);
   while ( (itemData->GetDataType() != GmatTree::ELSE_CONTROL) &&
           (itemData->GetDataType() != GmatTree::END_IF_CONTROL) )
   {
//      MessageInterface::ShowMessage("The cur sibling is %s\n", itemData->GetDesc().c_str());
      itemId = GetNextSibling(itemId);  
      itemData = (GmatTreeItemData *)GetItemData(itemId);
//      MessageInterface::ShowMessage("The next sibling is %s\n", itemData->GetDesc().c_str());
   }   
   
   if (itemData->GetDataType() == GmatTree::ELSE_CONTROL)
      // need to output message to user that they can't have 2 elses
      return;
   
   // add the "else if" to the sibling before that last else or end
   itemId = GetPrevSibling(itemId);
   
   wxString name;
   name.Printf("Else%d", mNumIfStatement);
   //    endName.Printf("End If%d", mNumIfStatement);
   //    ++mNumIfStatement;
   // ag: need gui interpreter to get control logic
   //    GmatCommand *cmd =
   //        theGuiInterpreter->CreateCommand("Maneuver", std::string(name.c_str()));
   //    
   //    if (cmd != NULL)
   //    {
   //        if (theGuiInterpreter->AppendCommand(cmd))
   //        {   
   wxTreeItemId targetId =
      InsertItem(parent, itemId, name, GmatTree::ICON_FOLDER, -1,
                 new MissionTreeItemData(name, GmatTree::ELSE_CONTROL, 
                                         name, NULL));
   SetItemImage(targetId, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);    
   //            AppendItem(itemId, endName, GmatTree::ICON_FILE, -1,
   //                       new MissionTreeItemData(endName, GmatTree::END_CONTROL, 
   //                                               endName, NULL));

   Expand(itemId);
   //        }
   //    }

}

//------------------------------------------------------------------------------
// void OnAddFunction(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddFunction(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   wxString name;
   name.Printf("CallFunction%d", ++mNumFunct);

//   GmatCommand *cmd =
//      theGuiInterpreter->CreateCommand("CallFunction", std::string(name.c_str()));
//
//   if (cmd != NULL)
//   {
//      if (theGuiInterpreter->AppendCommand(cmd))
//      {
//         wxTreeItemId targetId =
            AppendItem(item, name, GmatTree::ICON_FILE, -1,
                       new MissionTreeItemData(name, GmatTree::CALL_FUNCTION_COMMAND,
                                               name, NULL));

//      }
//   }
}

//------------------------------------------------------------------------------
// void OnInsertPropagate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertPropagate(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnInsertPropagate()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentId = GetItemParent(itemId);
   wxTreeItemId prevId = GetPrevVisible(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   

   GmatCommand *prevCmd = prevItem->GetCommand();

   if (prevCmd != NULL)
   {
      GmatCommand *cmd = theGuiInterpreter->CreateDefaultCommand("Propagate");

      if (cmd != NULL)
      {
         InsertCommand(parentId, itemId, prevId, GmatTree::ICON_FILE,
                       GmatTree::PROPAGATE_COMMAND, prevCmd, cmd, &mNumPropagate);
      }
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnInsertPropagate()");
#endif
}

//------------------------------------------------------------------------------
// void OnInsertManeuver(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertManeuver(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnInsertManeuver()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentId = GetItemParent(itemId);
   wxTreeItemId prevId = GetPrevVisible(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   

   GmatCommand *prevCmd = prevItem->GetCommand();

   if (prevCmd != NULL)
   {
      GmatCommand *cmd = theGuiInterpreter->CreateDefaultCommand("Maneuver");;
      
      if (cmd != NULL)
      {  
         InsertCommand(parentId, itemId, prevId, GmatTree::ICON_FILE,
                       GmatTree::MANEUVER_COMMAND, prevCmd, cmd, &mNumManeuver);
      }
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnInsertManeuver()");
#endif
}

//------------------------------------------------------------------------------
// void OnInsertTarget(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertTarget(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnInsertTarget()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentId = GetItemParent(itemId); 
   wxTreeItemId prevId = GetPrevVisible(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   
  
   GmatCommand *prevCmd = prevItem->GetCommand();

   if (prevCmd != NULL)
   {
      GmatCommand *cmd = theGuiInterpreter->CreateCommand("Target");
      GmatCommand *endCmd = theGuiInterpreter->CreateCommand("EndTarget");

      if (cmd != NULL && endCmd != NULL)
      {
         wxTreeItemId node =
            InsertCommand(parentId, itemId, prevId, GmatTree::ICON_FOLDER,
                          GmatTree::TARGET_COMMAND, prevCmd, cmd, &mNumTarget);
      
         SetItemImage(node, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

         // Append EndTarget
         InsertCommand(node, node, node, GmatTree::ICON_FILE,
                       GmatTree::END_TARGET_COMMAND,
                       cmd, endCmd, &mNumTarget, mNumTarget);

         Expand(node);
      }
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnInsertTarget()");
#endif
}

//------------------------------------------------------------------------------
// void OnInsertAchieve(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertAchieve(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnInsertAchieve()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentId = GetItemParent(itemId);
   wxTreeItemId prevId = GetPrevVisible(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   

   GmatCommand *prevCmd = prevItem->GetCommand();

   if (prevCmd != NULL)
   {
      GmatCommand *cmd = theGuiInterpreter->CreateDefaultCommand("Achieve");
      
      if (cmd != NULL)
      {  
         InsertCommand(parentId, itemId, prevId, GmatTree::ICON_FILE,
                       GmatTree::ACHIEVE_COMMAND, prevCmd, cmd, &mNumAchieve);
      }
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnInsertAchieve()");
#endif
}

//------------------------------------------------------------------------------
// void OnInsertVary(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertVary(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnInsertVary()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentId = GetItemParent(itemId);
   wxTreeItemId prevId = GetPrevVisible(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   

   GmatCommand *prevCmd = prevItem->GetCommand();

   if (prevCmd != NULL)
   {     
      GmatCommand *cmd = theGuiInterpreter->CreateDefaultCommand("Vary");
            
      if (cmd != NULL)
      {  
         InsertCommand(parentId, itemId, prevId, GmatTree::ICON_FILE,
                       GmatTree::VARY_COMMAND, prevCmd, cmd, &mNumVary);
      }
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnInsertVary()");
#endif
}

//------------------------------------------------------------------------------
// void OnInsertIfStatement(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertIfStatement(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnInsertIfStatement()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentId = GetItemParent(itemId);
   wxTreeItemId prevId = GetPrevVisible(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   
  
   GmatCommand *prevCmd = prevItem->GetCommand();

   if (prevCmd != NULL)
   {
      //loj: 10/15/04 finished the code
      GmatCommand *cmd = theGuiInterpreter->CreateCommand("If");
      GmatCommand *endCmd = theGuiInterpreter->CreateCommand("EndIf");

      if (cmd != NULL && endCmd != NULL)
      {
         wxTreeItemId node =
            InsertCommand(parentId, itemId, prevId, GmatTree::ICON_FOLDER,
                          GmatTree::IF_CONTROL, prevCmd, cmd, &mNumIfStatement);
         
         SetItemImage(node, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

         // Append EndIf
         InsertCommand(node, node, node, GmatTree::ICON_FILE,
                       GmatTree::END_IF_CONTROL,
                       cmd, endCmd, &mNumIfStatement, mNumIfStatement);

         Expand(node);
      }
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnInsertIfStatement()");
#endif
}

//------------------------------------------------------------------------------
// void OnInsertWhileLoop(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertWhileLoop(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnInsertWhileLoop()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentId = GetItemParent(itemId);
   wxTreeItemId prevId = GetPrevVisible(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   
  
   GmatCommand *prevCmd = prevItem->GetCommand();
  
   if (prevCmd != NULL)
   {
      //loj: 10/15/04 finished the code
      GmatCommand *cmd = theGuiInterpreter->CreateCommand("While");
      GmatCommand *endCmd = theGuiInterpreter->CreateCommand("EndWhile");
      
      if (cmd != NULL && endCmd != NULL)
      {
         wxTreeItemId node =
            InsertCommand(parentId, itemId, prevId, GmatTree::ICON_FOLDER,
                          GmatTree::WHILE_CONTROL,
                          prevCmd, cmd, &mNumWhileLoop);
         
         SetItemImage(node, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

         // Append EndWhile
         InsertCommand(node, node, node, GmatTree::ICON_FILE,
                       GmatTree::END_WHILE_CONTROL,
                       cmd, endCmd, &mNumWhileLoop, mNumWhileLoop);

         Expand(node);
      }
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnInsertWhileLoop()");
#endif
}

//------------------------------------------------------------------------------
// void OnInsertForLoop(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertForLoop(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnInsertForLoop()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentId = GetItemParent(itemId);
   wxTreeItemId prevId = GetPrevVisible(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   
   
   GmatCommand *prevCmd = prevItem->GetCommand();

   if (prevCmd != NULL)
   {
      //loj: 10/15/04 finished the code
      GmatCommand *cmd = theGuiInterpreter->CreateCommand("For");
      GmatCommand *endCmd = theGuiInterpreter->CreateCommand("EndFor");
      
      if (cmd != NULL && endCmd != NULL)
      {
         wxTreeItemId node =
            InsertCommand(parentId, itemId, prevId, GmatTree::ICON_FOLDER,
                          GmatTree::FOR_CONTROL, prevCmd, cmd, &mNumForLoop);
         
         SetItemImage(node, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

         // Append EndFor
         InsertCommand(node, node, node, GmatTree::ICON_FILE,
                       GmatTree::END_FOR_CONTROL,
                       cmd, endCmd, &mNumForLoop, mNumForLoop);

         Expand(node);
      }
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnInsertForLoop()");
#endif
}

//------------------------------------------------------------------------------
// void OnInsertDoWhile(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertDoWhile(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnInsertDoWhile()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentId = GetItemParent(itemId);
   wxTreeItemId prevId = GetPrevSibling(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);

   GmatCommand *prevCmd = prevItem->GetCommand();

   if (prevCmd != NULL)
   {
      //loj: 10/15/04 finished the code
      GmatCommand *cmd = theGuiInterpreter->CreateCommand("Do");
      GmatCommand *endCmd = theGuiInterpreter->CreateCommand("EndDo");
      
      if (cmd != NULL && endCmd != NULL)
      {
         wxTreeItemId node =
            InsertCommand(parentId, itemId, prevId, GmatTree::ICON_FOLDER,
                          GmatTree::DO_CONTROL, prevCmd, cmd, &mNumDoWhile);
         
         SetItemImage(node, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

         // Append EndDo
         InsertCommand(node, node, node, GmatTree::ICON_FILE,
                       GmatTree::END_DO_CONTROL,
                       cmd, endCmd, &mNumDoWhile, mNumDoWhile);

         Expand(node);
      }
   }     
#if DEBUG_MISSION_TREE
   ShowCommands("After OnInsertDoWhile()");
#endif
}

//------------------------------------------------------------------------------
// void OnInsertSwitchCase(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertSwitchCase(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnInsertSwitchCase()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentId = GetItemParent(itemId);
   wxTreeItemId prevId = GetPrevSibling(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);

   GmatCommand *prevCmd = prevItem->GetCommand();
   
   if (prevCmd != NULL)
   {
      //loj: 10/15/04 finished the code
      GmatCommand *cmd = theGuiInterpreter->CreateCommand("Switch");
      GmatCommand *endCmd = theGuiInterpreter->CreateCommand("EndSwitch");
      
      if (cmd != NULL && endCmd != NULL)
      {
         wxTreeItemId node =
            InsertCommand(parentId, itemId, prevId, GmatTree::ICON_FOLDER,
                          GmatTree::DO_CONTROL, prevCmd, cmd, &mNumSwitchCase);
         
         SetItemImage(node, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

         InsertCommand(node, node, node, GmatTree::ICON_FILE, GmatTree::END_DO_CONTROL,
                       cmd, endCmd, &mNumSwitchCase, mNumSwitchCase);

         Expand(node);
      }
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnInsertSwitchCase()");
#endif
}

//---------------------------------
// Crete popup menu
//---------------------------------

//------------------------------------------------------------------------------
// wxMenu* CreateAddPopupMenu()
//------------------------------------------------------------------------------
wxMenu* MissionTree::CreateAddPopupMenu()
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
         menu->Append(POPUP_ADD_PROPAGATE, wxT("Propagate"));
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

   menu->Append(POPUP_CONTROL_LOGIC, "Control Logic", CreateAddControlLogicPopupMenu());
   menu->Append(POPUP_ADD_FUNCTION, wxT("Function"));

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

   menu->Append(POPUP_CONTROL_LOGIC, "Control Logic", CreateInsertControlLogicPopupMenu());

   return menu;
}

//------------------------------------------------------------------------------
// wxMenu* CreateTargetPopupMenu(bool insert)
//------------------------------------------------------------------------------
wxMenu* MissionTree::CreateTargetPopupMenu(bool insert)
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
         if (insert)
            menu->Append(POPUP_INSERT_PROPAGATE, wxT("Propagate"));
         else
            menu->Append(POPUP_ADD_PROPAGATE, wxT("Propagate"));
      }
      else if (items[i] == "Maneuver")
      {
         if (insert)
            menu->Append(POPUP_INSERT_MANEUVER, wxT("Maneuver"));
         else
            menu->Append(POPUP_ADD_MANEUVER, wxT("Maneuver"));
      }
      else if (items[i] == "Target")
      {
         if (insert)
            menu->Append(POPUP_INSERT_TARGET, wxT("Target"));
         else
            menu->Append(POPUP_ADD_TARGET, wxT("Target"));
      }
      else if (items[i] == "Vary")
      {
         if (insert)
            menu->Append(POPUP_INSERT_VARY, wxT("Vary"));
         else
            menu->Append(POPUP_ADD_VARY, wxT("Vary"));
      }
      else if (items[i] == "Achieve")
      {
         if (insert)
            menu->Append(POPUP_INSERT_ACHIEVE, wxT("Achieve"));
         else
            menu->Append(POPUP_ADD_ACHIEVE, wxT("Achieve"));
      }
   }

   menu->Append(POPUP_CONTROL_LOGIC, "Control Logic", CreateInsertControlLogicPopupMenu());

   return menu;
}

////------------------------------------------------------------------------------
//// wxMenu* CreateControlLogicPopupMenu()
////------------------------------------------------------------------------------
//wxMenu* MissionTree::CreateControlLogicPopupMenu()
//{
//    //MessageInterface::ShowMessage("MissionTree::CreateControlLogicMenu() entered\n");
//    //unsigned int i;
//    wxMenu *menu = new wxMenu;
//
//    //    StringArray items = theGuiInterpreter->GetListOfFactoryItems(Gmat::COMMAND);
//
//    // for (i=0; i<items.size(); i++)
//    // {
//        //MessageInterface::ShowMessage("command = " + items[i] + "\n");
//
//    //    if (items[i] == "Propagate")
//    //    {
//                menu->Append(POPUP_WHILE_CONTROL, wxT("While")); //wxT(items[i].c_str()));
//                menu->Enable(POPUP_WHILE_CONTROL, false);
//    //    }
//    //    else if (items[i] == "Maneuver")
//    //     {
//                menu->Append(POPUP_FOR_CONTROL, wxT("For"));
//                menu->Enable(POPUP_FOR_CONTROL, false);
//    //    }
//    //    else if (items[i] == "Target")
//    //    {
//              menu->Append(POPUP_DO_CONTROL, wxT("Do"));
//              menu->Enable(POPUP_DO_CONTROL, false);
//    //    }
//    // }
//
//    return menu;
//}

//------------------------------------------------------------------------------
// wxMenu* CreateAddControlLogicPopupMenu()
//------------------------------------------------------------------------------
wxMenu* MissionTree::CreateAddControlLogicPopupMenu()
{
   //MessageInterface::ShowMessage("MissionTree::CreateControlLogicMenu() entered\n");
   //unsigned int i;
   wxMenu *menu = new wxMenu;

   //    StringArray items = theGuiInterpreter->GetListOfFactoryItems(Gmat::COMMAND);

    // for (i=0; i<items.size(); i++)
    // {
        //MessageInterface::ShowMessage("command = " + items[i] + "\n");

   menu->Append(POPUP_ADD_IF_STATEMENT, wxT("If"));
   menu->Append(POPUP_ADD_WHILE_LOOP, wxT("While")); 
   menu->Append(POPUP_ADD_FOR_LOOP, wxT("For"));
//   menu->Append(POPUP_ADD_D0_WHILE, wxT("Do While"));
   menu->Append(POPUP_ADD_SWITCH_CASE, wxT("Switch")); 
   
//   menu->Enable(POPUP_ADD_IF_STATEMENT, FALSE);
//   menu->Enable(POPUP_ADD_WHILE_LOOP, FALSE);
//   menu->Enable(POPUP_ADD_FOR_LOOP, FALSE);
   menu->Enable(POPUP_ADD_SWITCH_CASE, FALSE);
       
   //    if (items[i] == "Propagate")
   //    {
   //    }
   //    else if (items[i] == "Maneuver")
   //     {
   //    }
   //    else if (items[i] == "Target")
   //    {
   //    }
   // }

   return menu;
}

//------------------------------------------------------------------------------
// wxMenu* CreateAddControlIfLogicPopupMenu()
//------------------------------------------------------------------------------
wxMenu* MissionTree::CreateAddIfPopupMenu()
{
   //MessageInterface::ShowMessage("MissionTree::CreateControlLogicMenu() entered\n");
   //unsigned int i;
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

   //-----------------------------------------------------------  
   wxMenu *popupMenu = new wxMenu;

   //    StringArray items = theGuiInterpreter->GetListOfFactoryItems(Gmat::COMMAND);

    // for (i=0; i<items.size(); i++)
    // {
        //MessageInterface::ShowMessage("command = " + items[i] + "\n");
   popupMenu->Append(POPUP_ADD_ELSE_IF_STATEMENT, wxT("Else If"));
   popupMenu->Append(POPUP_ADD_ELSE_STATEMENT, wxT("Else"));
   popupMenu->AppendSeparator();
   popupMenu->Append(POPUP_ADD_IF_STATEMENT, wxT("If"));
   popupMenu->Append(POPUP_ADD_WHILE_LOOP, wxT("While")); 
   popupMenu->Append(POPUP_ADD_FOR_LOOP, wxT("For"));
//   popupMenu->Append(POPUP_ADD_D0_WHILE, wxT("Do While"));
   popupMenu->Append(POPUP_ADD_SWITCH_CASE, wxT("Switch")); 

   //loj: 10/15/04 disable control statements that aren't ready yet
   popupMenu->Enable(POPUP_ADD_ELSE_IF_STATEMENT, FALSE);
   popupMenu->Enable(POPUP_ADD_ELSE_STATEMENT, FALSE);
   popupMenu->Enable(POPUP_ADD_SWITCH_CASE, FALSE);
   
   //    if (items[i] == "Propagate")
   //    {
   //    }
   //    else if (items[i] == "Maneuver")
   //     {
   //    }
   //    else if (items[i] == "Target")
   //    {
   //    }
   // }
   menu->Append(POPUP_CONTROL_LOGIC, "Control Logic", popupMenu);

   return menu;
}


//------------------------------------------------------------------------------
// wxMenu* CreateInsertControlLogicPopupMenu()
//------------------------------------------------------------------------------
wxMenu* MissionTree::CreateInsertControlLogicPopupMenu()
{
   //MessageInterface::ShowMessage("MissionTree::CreateControlLogicMenu() entered\n");
   //unsigned int i;
   wxMenu *menu = new wxMenu;

   //StringArray items = theGuiInterpreter->GetListOfFactoryItems(Gmat::COMMAND);

    // for (i=0; i<items.size(); i++)
    // {
        //MessageInterface::ShowMessage("command = " + items[i] + "\n");

   menu->Append(POPUP_INSERT_IF_STATEMENT, wxT("If"));
   menu->Append(POPUP_INSERT_WHILE_LOOP, wxT("While")); 
   menu->Append(POPUP_INSERT_FOR_LOOP, wxT("For"));
//   menu->Append(POPUP_INSERT_D0_WHILE, wxT("Do While"));
   menu->Append(POPUP_INSERT_SWITCH_CASE, wxT("Switch")); 
   
//   menu->Enable(POPUP_INSERT_IF_STATEMENT, FALSE);
//   menu->Enable(POPUP_INSERT_WHILE_LOOP, FALSE);
//   menu->Enable(POPUP_INSERT_FOR_LOOP, FALSE);
   menu->Enable(POPUP_INSERT_SWITCH_CASE, FALSE);
       
   //    if (items[i] == "Propagate")
   //    {
   //    }
   //    else if (items[i] == "Maneuver")
   //     {
   //    }
   //    else if (items[i] == "Target")
   //    {
   //    }
   // }

   return menu;
}

//------------------------------------------------------------------------------
// void OnViewVariables()
//------------------------------------------------------------------------------
void MissionTree::OnViewVariables()
{
   MissionTreeItemData *item = new MissionTreeItemData(wxT("Variables"), 
                                                       GmatTree::VIEW_SOLVER_VARIABLES);
   GmatAppData::GetMainFrame()->CreateChild(item);
}

//------------------------------------------------------------------------------
// void OnViewGoals()
//------------------------------------------------------------------------------
void MissionTree::OnViewGoals()
{
   MissionTreeItemData *item = new MissionTreeItemData(wxT("Goals"),
                                                       GmatTree::VIEW_SOLVER_GOALS);
   GmatAppData::GetMainFrame()->CreateChild(item);
}

//------------------------------------------------------------------------------
// void OnDelete()
//------------------------------------------------------------------------------
void MissionTree::OnDelete()
{
   // get selected item
   wxTreeItemId itemId = GetSelection();
   
   // if panel is open close it
   OnClose();
   
   // delete from gui interpreter
   MissionTreeItemData *missionItem = (MissionTreeItemData *)GetItemData(itemId);
   if (missionItem == NULL)
      return;   // error
     
   GmatCommand *theCmd = missionItem->GetCommand();  
   if (theCmd == NULL)
      return;  //error 
   
   theGuiInterpreter->DeleteCommand(theCmd);
   
   // delete from tree - if parent only has 1 child collapse
   wxTreeItemId parentId = GetItemParent(itemId);
   if (GetChildrenCount(parentId) <= 1)
      this->Collapse(parentId);
      
   this->Delete(itemId);  
}


//---------------------------------------------------------------------------
// void MissionTree::OnRun()
//--------------------------------------------------------------------------
void MissionTree::OnRun()
{
   theGuiInterpreter->RunMission();
}

//---------------------------------------------------------------------------
// bool MissionTree::CheckClickIn(wxPoint position)
//--------------------------------------------------------------------------
bool MissionTree::CheckClickIn(wxPoint position)
{
   //MessageInterface::ShowMessage("Click position is %d %d\n", position.x, position.y );
   //MissionTreeItemData *missionTreeItem = (MissionTreeItemData*) GetFirstVisibleItem();
   wxTreeItemId visibleItemId = GetFirstVisibleItem();
   MissionTreeItemData *missionTreeItemData = 
      (MissionTreeItemData*) GetItemData(visibleItemId);
   //MessageInterface::ShowMessage("Got first visible");

   // loop through all the visible items on the mission tree
   // to compare the event click with the position of the box
   while (missionTreeItemData != NULL)
   {
      int dataType = missionTreeItemData->GetDataType();
      // don't have to open any panels for top folders
      if ((dataType != GmatTree::MISSIONS_FOLDER)       &&
          (dataType != GmatTree::MISSION_SEQ_TOP_FOLDER)&&
          (dataType != GmatTree::MISSION_SEQ_SUB_FOLDER)&&
          (dataType != GmatTree::MISSION_SEQ_COMMAND))
      {
         // get the surrounding box to compare click and commands
         wxRect bound;
         int w, h;
       
         GetBoundingRect(visibleItemId, bound, TRUE);
         GetSize(&w, &h);
       
         // compare event click to see if it is in the box or the
         // icon which is size 16
         if ((position.x >= (bound.x - 16)) &&
             (position.x <= w-offset) &&
             (position.y <= bound.y+rowHeight+1) &&
             (position.y >= bound.y-1))
         {
            //MessageInterface::ShowMessage("\nInside a rect\n");
              
            // set this item selected
            SelectItem(visibleItemId);
              
            // now that we know it is in a box, check to see
            // which box it is in
            // we only need to compare the left and the right, because
            // we already know it is within the top and the bottom
          
            // get box width
            int boxWidth = GetParameter(BOXWIDTH);
 
            // box count is 2, rightmost is for variables
            // next is goals, and the rest is the cmd panel
            int boxNum = 0;
          
            // check if in variables
            if ((position.x <= w-offset-boxWidth*boxNum) &&
                (position.x >= w-offset-boxWidth*(++boxNum)))
            {
               //MessageInterface::ShowMessage("\nInside variables");
               MissionTreeItemData *item =
                  new MissionTreeItemData(
                                          wxT("Variables"),
                                          GmatTree::VIEW_SOLVER_VARIABLES);
               GmatAppData::GetMainFrame()->CreateChild(item);
            }
            else if ((position.x <= w-offset-boxWidth*boxNum) &&
                     (position.x >= w-offset-boxWidth*(++boxNum)))
            {
               //MessageInterface::ShowMessage("\nInside goals");
               MissionTreeItemData *item =
                  new MissionTreeItemData(wxT("Goals"),
                                          GmatTree::VIEW_SOLVER_GOALS);
               GmatAppData::GetMainFrame()->CreateChild(item);
            }
            else
            {
               //MessageInterface::ShowMessage("\nOpen regular panel");
               //mainNotebook->CreatePage(missionTreeItemData);
               GmatAppData::GetMainFrame()->CreateChild(missionTreeItemData);
            }
            
            // get out of while loop
            break;
         }
      }
      //MessageInterface::ShowMessage("Not equal to null");
      visibleItemId = GetNextVisible(visibleItemId);
      //MessageInterface::ShowMessage("Got next visible id");
      missionTreeItemData = (MissionTreeItemData*) GetItemData(visibleItemId);
      //MessageInterface::ShowMessage("Got next visible data");
   }

   return false;
}

//------------------------------------------------------------------------------
// void OnOpen(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Open chosen from popup menu
 */
//------------------------------------------------------------------------------
void MissionTree::OnOpen()
{
   // Get info from selected item
   GmatTreeItemData *item = (GmatTreeItemData *) GetItemData(GetSelection());
   //    mainNotebook->CreatePage(item);
   GmatAppData::GetMainFrame()->CreateChild(item);
}

//------------------------------------------------------------------------------
// void OnClose()
//------------------------------------------------------------------------------
/**
 * Close chosen from popup menu
 */
//------------------------------------------------------------------------------
void MissionTree::OnClose()
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

// for Debug
//------------------------------------------------------------------------------
// void ShowCommands(const wxString &msg = "")
//------------------------------------------------------------------------------
void MissionTree::ShowCommands(const wxString &msg)
{
   MessageInterface::ShowMessage("--------------------%s\n", msg.c_str());
   
   GmatCommand *cmd = theGuiInterpreter->GetNextCommand();
   GmatCommand *child;
   
   while (cmd != NULL)
   {
      MessageInterface::ShowMessage("--- cmd=%s\n", cmd->GetTypeName().c_str());
      child = cmd->GetChildCommand(0);

      if (child != NULL)
      {
         ShowSubCommands(cmd, child);
      }
      
      cmd = cmd->GetNext();
   }
}

//------------------------------------------------------------------------------
// void ShowSubCommands(GmatCommand *baseCmd, GmatCommand *cmd)
//------------------------------------------------------------------------------
void MissionTree::ShowSubCommands(GmatCommand *baseCmd, GmatCommand *cmd)
{
   GmatCommand *childCmd;
   Integer i = 0;

   while (cmd != baseCmd)
   {
      MessageInterface::ShowMessage("------- cmd=%s\n", cmd->GetTypeName().c_str());
      childCmd = cmd->GetChildCommand(i);

      while (childCmd != NULL)
      {
         ShowSubCommands(cmd, childCmd);
         ++i;
         childCmd = cmd->GetChildCommand(i);
      }

      cmd = cmd->GetNext();
      i = 0;
   }
}
