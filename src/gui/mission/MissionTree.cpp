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

#define DEBUG_MISSION_TREE 0

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
   EVT_LEFT_DCLICK(MissionTree::OnDoubleClick) 

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
   EVT_MENU(POPUP_ADD_IF_STATEMENT, MissionTree::OnAddIfStatement)
   EVT_MENU(POPUP_ADD_WHILE_LOOP, MissionTree::OnAddWhileLoop)
   EVT_MENU(POPUP_ADD_FOR_LOOP, MissionTree::OnAddForLoop)
   EVT_MENU(POPUP_ADD_D0_WHILE, MissionTree::OnAddDoWhile)
   EVT_MENU(POPUP_ADD_SWITCH_CASE, MissionTree::OnAddSwitchCase)

   EVT_MENU(POPUP_INSERT_MANEUVER, MissionTree::OnInsertManeuver)
   EVT_MENU(POPUP_INSERT_PROPAGATE, MissionTree::OnInsertPropagate)
   EVT_MENU(POPUP_INSERT_TARGET, MissionTree::OnInsertTarget)
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
   mNumIfStatement = 0;
   mNumWhileLoop = 0;
   mNumForLoop = 0;
   mNumDoWhile = 0;
   mNumSwitchCase = 0;

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
      mNumIfStatement = 0;
      mNumWhileLoop = 0;
      mNumForLoop = 0;
      mNumDoWhile = 0;
      mNumSwitchCase = 0;
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
#endif

   GmatCommand *cmd = theGuiInterpreter->GetNextCommand();
   MissionTreeItemData *seqItemData =
      (MissionTreeItemData *)GetItemData(mMissionSeqSubItem);
    
   if (cmd->GetTypeName() == "NoOp")
      seqItemData->SetCommand(cmd);
    
   while (cmd != NULL)
   {
      UpdateCommandTree(mMissionSeqSubItem, cmd);
      cmd = cmd->GetNext();
   }

   Expand(mMissionSeqSubItem);
}

//------------------------------------------------------------------------------
// void UpdateCommandTree(wxTreeItemId treeId, GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Updates commands in the mission sequence
 */
//------------------------------------------------------------------------------
void MissionTree::UpdateCommandTree(wxTreeItemId treeId, GmatCommand *cmd)
{
   wxString objTypeName = cmd->GetTypeName().c_str();
   wxString objName = cmd->GetName().c_str(); //loj: 6/29/04 added
   
#if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::UpdateCommand() objTypeName = %s objName=%s\n",
       objTypeName.c_str(), objName.c_str());
#endif
   
   if (objTypeName == "Propagate")
   {
      if (objName == "" || objName == " ")
            objName.Printf("Propagate%d", ++mNumPropagate);

      AppendItem(treeId, objName, GmatTree::ICON_FILE, -1,
                 new MissionTreeItemData(objName,
                                         GmatTree::PROPAGATE_COMMAND,
                                         objName, cmd));
   }
   else if (objTypeName == "Maneuver")
   {
      if (objName == "" || objName == " ")
            objName.Printf("Maneuver%d", ++mNumManeuver);

      AppendItem(treeId, objName, GmatTree::ICON_FILE, -1,
                 new MissionTreeItemData(objName,
                                         GmatTree::MANEUVER_COMMAND,
                                         objName, cmd));
   }
   else if (objTypeName == "Target")
   {
      if (objName == "" || objName == " ")
            objName.Printf("Target%d", ++mNumTarget);

      wxTreeItemId newId =
         AppendItem(treeId, objName, GmatTree::ICON_FOLDER, -1,
                    new MissionTreeItemData(objName,
                                            GmatTree::TARGET_COMMAND,
                                            objName, cmd));
        
      SetItemImage(newId, GmatTree::ICON_OPENFOLDER, 
                   wxTreeItemIcon_Expanded);

      Expand(treeId);
   }
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
    
#if wxUSE_MENUS
   wxMenu menu;

   if (dataType == GmatTree::MISSION_SEQ_TOP_FOLDER)
   {
      menu.Append(POPUP_ADD_MISSION_SEQ, wxT("Add Mission Sequence"));
   }
   else if (dataType == GmatTree::TARGET_COMMAND)
   {   
      menu.Append(POPUP_OPEN, wxT("Open"));
      menu.Append(POPUP_CLOSE, wxT("Close"));
      menu.AppendSeparator();
      menu.Append(POPUP_ADD_COMMAND, wxT("Add"), CreatePopupMenu());
      menu.Append(POPUP_INSERT_COMMAND, wxT("Insert"), 
                  CreateInsertPopupMenu());
      menu.Append(POPUP_DELETE, wxT("Delete"));
      menu.Append(POPUP_RENAME, wxT("Rename"));
      menu.AppendSeparator();
      menu.Append(POPUP_VIEW_VARIABLES, wxT("View Variables"));
      menu.Append(POPUP_VIEW_GOALS, wxT("View Goals")); 
   } 
   else if ((dataType == GmatTree::IF_CONTROL)    ||
            (dataType == GmatTree::WHILE_CONTROL) ||
            (dataType == GmatTree::DO_CONTROL) ||
            (dataType == GmatTree::FOR_CONTROL))   
   {
      menu.Append(POPUP_OPEN, wxT("Open"));
      menu.Append(POPUP_CLOSE, wxT("Close"));
      menu.AppendSeparator();
      menu.Append(POPUP_ADD_COMMAND, wxT("Add"), CreatePopupMenu());
      menu.Append(POPUP_INSERT_COMMAND, wxT("Insert"), 
                  CreateInsertPopupMenu());
      menu.Append(POPUP_DELETE, wxT("Delete"));
      menu.Append(POPUP_RENAME, wxT("Rename"));
   } 
   else if (dataType == GmatTree::MISSION_SEQ_SUB_FOLDER)
   {
      menu.Append(POPUP_ADD_COMMAND, wxT("Add"), CreatePopupMenu());
      // ag: can't delete because sys. doesn't handle multiple sequences yet
      // menu.Append(POPUP_DELETE, wxT("Delete"));
      menu.Append(POPUP_RENAME, wxT("Rename"));
      menu.AppendSeparator();
      menu.Append(POPUP_RUN, wxT("Run"));
   }
   else 
   {
      menu.Append(POPUP_OPEN, wxT("Open"));
      menu.Append(POPUP_CLOSE, wxT("Close"));
      menu.AppendSeparator();
      menu.Append(POPUP_INSERT_COMMAND, wxT("Insert"), 
                  CreateInsertPopupMenu());
      menu.Append(POPUP_DELETE, wxT("Delete"));
      menu.Append(POPUP_RENAME, wxT("Rename"));
   }


   //loj: 2/13/04 for future build
   //      // if command is the first command, disable the Insert popup menu
   //      wxTreeItemID firstItemId = GetFirstChild(GmatTree::MISSION_SEQ_SUB_FOLDER);
   //      if (id == firstItemId)
   //          // disable Insert menu item
        
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
   wxTreeItemId item = GetSelection();
   wxString name;
   name.Printf("Propagate%d", ++mNumPropagate);

   GmatCommand *cmd =
      //theGuiInterpreter->CreateCommand("Propagate", std::string(name.c_str()));//loj:6/15/04
      theGuiInterpreter->CreateDefaultCommand("Propagate", std::string(name.c_str()));
    
   if (cmd != NULL)
   {
      if (theGuiInterpreter->AppendCommand(cmd))
      {
         AppendItem(item, name, GmatTree::ICON_FILE, -1,
                    new MissionTreeItemData(name, GmatTree::PROPAGATE_COMMAND,
                                            name, cmd));
            
         Expand(item);
      }
   }
}

//------------------------------------------------------------------------------
// void OnAddManeuver(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddManeuver(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   wxString name;
   name.Printf("Maneuver%d", ++mNumManeuver);
  
   GmatCommand *cmd =
      //theGuiInterpreter->CreateCommand("Maneuver", std::string(name.c_str())); //loj: 6/15/04
      theGuiInterpreter->CreateDefaultCommand("Maneuver", std::string(name.c_str()));
    
   if (cmd != NULL)
   {
      if (theGuiInterpreter->AppendCommand(cmd))
      {   
         AppendItem(item, name, GmatTree::ICON_FILE, -1,
                    new MissionTreeItemData(name, GmatTree::MANEUVER_COMMAND, 
                                            name, cmd));

         Expand(item);
      }
   }
}

//------------------------------------------------------------------------------
// void :OnAddTarget(wxCommandEvent &event)
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
         wxTreeItemId targetId =
            AppendItem(item, name, GmatTree::ICON_FOLDER, -1,
                       new MissionTreeItemData(name, GmatTree::TARGET_COMMAND,
                                               name, cmd));
            
         SetItemImage(targetId, GmatTree::ICON_OPENFOLDER, 
                      wxTreeItemIcon_Expanded);            

         Expand(item);
      }
   }
}

//------------------------------------------------------------------------------
// void OnAddIfStatement(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddIfStatement(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   wxString name;
   wxString endName;
   name.Printf("If%d", ++mNumIfStatement);
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
      AppendItem(item, name, GmatTree::ICON_FOLDER, -1,
                 new MissionTreeItemData(name, GmatTree::IF_CONTROL, 
                                         name, NULL));
   SetItemImage(targetId, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);    
   //            AppendItem(item, endName, GmatTree::ICON_FILE, -1,
   //                       new MissionTreeItemData(endName, GmatTree::END_CONTROL, 
   //                                               endName, NULL));

   Expand(item);
   //        }
   //    }
}

//------------------------------------------------------------------------------
// void OnAddWhileLoop(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddWhileLoop(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   wxString name;
   wxString endName;
   name.Printf("While%d", ++mNumWhileLoop);
   //    endName.Printf("End While%d", mNumWhileLoop);
   //    ++mNumWhileLoop;
   // ag: need gui interpreter to get control logic
   //    GmatCommand *cmd =
   //        theGuiInterpreter->CreateCommand("Maneuver", std::string(name.c_str()));
   //    
   //    if (cmd != NULL)
   //    {
   //        if (theGuiInterpreter->AppendCommand(cmd))
   //        {   
   wxTreeItemId targetId =
      AppendItem(item, name, GmatTree::ICON_FOLDER, -1,
                 new MissionTreeItemData(name, GmatTree::WHILE_CONTROL, 
                                         name, NULL));
   SetItemImage(targetId, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);    
   //            AppendItem(item, endName, GmatTree::ICON_FILE, -1,
   //                       new MissionTreeItemData(endName, GmatTree::END_CONTROL, 
   //                                               endName, NULL));
   //
   Expand(item);
   //        }
   //    }
}

//------------------------------------------------------------------------------
// void OnAddForLoop(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddForLoop(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   wxString name;
   wxString endName;
   name.Printf("For%d", ++mNumForLoop);
   //    endName.Printf("End For%d", mNumForLoop);
   //    ++mNumForLoop;
   // ag: need gui interpreter to get control logic
   //    GmatCommand *cmd =
   //        theGuiInterpreter->CreateCommand("Maneuver", std::string(name.c_str()));
   //    
   //    if (cmd != NULL)
   //    {
   //        if (theGuiInterpreter->AppendCommand(cmd))
   //        {   
   wxTreeItemId targetId =
      AppendItem(item, name, GmatTree::ICON_FOLDER, -1,
                 new MissionTreeItemData(name, GmatTree::FOR_CONTROL, 
                                         name, NULL));
   SetItemImage(targetId, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);    
   //            AppendItem(item, endName, GmatTree::ICON_FILE, -1,
   //                       new MissionTreeItemData(endName, GmatTree::END_CONTROL, 
   //                                               endName, NULL));

   Expand(item);
   //        }
   //    }
}

//------------------------------------------------------------------------------
// void OnAddDoWhile(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddDoWhile(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   wxString name;
   wxString endName;
   name.Printf("Do%d", ++mNumDoWhile);
   //    endName.Printf("While%d", mNumDoWhile);
   //    ++mNumDoWhile;
   // ag: need gui interpreter to get control logic
   //    GmatCommand *cmd =
   //        theGuiInterpreter->CreateCommand("Maneuver", std::string(name.c_str()));
   //    
   //    if (cmd != NULL)
   //    {
   //        if (theGuiInterpreter->AppendCommand(cmd))
   //        {   
   wxTreeItemId targetId =
      AppendItem(item, name, GmatTree::ICON_FOLDER, -1,
                 new MissionTreeItemData(name, GmatTree::DO_CONTROL, 
                                         name, NULL));
   SetItemImage(targetId, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);    
   //            AppendItem(item, endName, GmatTree::ICON_FILE, -1,
   //                       new MissionTreeItemData(endName, GmatTree::END_CONTROL, 
   //                                               endName, NULL));

   Expand(item);
   //        }
   //    }
}

//------------------------------------------------------------------------------
// void OnAddSwitchCase(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddSwitchCase(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   wxString name;
   name.Printf("Switch%d", ++mNumSwitchCase);

   // ag: need gui interpreter to get control logic
   //    GmatCommand *cmd =
   //        theGuiInterpreter->CreateCommand("Maneuver", std::string(name.c_str()));
   //    
   //    if (cmd != NULL)
   //    {
   //        if (theGuiInterpreter->AppendCommand(cmd))
   //        {   
   AppendItem(item, name, GmatTree::ICON_FILE, -1,
              new MissionTreeItemData(name, GmatTree::SWITCH_CONTROL, 
                                      name, NULL));

   Expand(item);
   //        }
   //    }
}

//------------------------------------------------------------------------------
// void OnInsertPropagate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertPropagate(wxCommandEvent &event)
{
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentItemId = GetItemParent(itemId);
   wxString name;
   name.Printf("Propagate%d", ++mNumPropagate); 
 
   wxTreeItemId prevItemId = GetPrevSibling(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevItemId);

   if (prevItem == NULL)
   {
      prevItemId = GetPrevVisible(itemId);
      prevItem = (MissionTreeItemData *)GetItemData(prevItemId);   
   }   

   GmatCommand *prevCmd = prevItem->GetCommand();

   if (prevCmd != NULL)
   {
      GmatCommand *cmd =
         //theGuiInterpreter->CreateCommand("Propagate", std::string(name.c_str()));//loj:6/15/04
         theGuiInterpreter->CreateDefaultCommand("Propagate", std::string(name.c_str()));

      if (theGuiInterpreter->InsertCommand(cmd, prevCmd))
      {
         if (prevCmd->GetTypeName() == "NoOp" ) // ag: || prevCmd->GetTypeName() == "Target")
         {
            InsertItem(parentItemId, 0, name, GmatTree::ICON_FILE, -1,
                       new MissionTreeItemData(name, GmatTree::PROPAGATE_COMMAND,
                                               name, cmd));
         }
         else
         {
            InsertItem(parentItemId, prevItemId, name, GmatTree::ICON_FILE, -1,
                       new MissionTreeItemData(name, GmatTree::PROPAGATE_COMMAND,
                                               name, cmd));
         }
      }
   }
}

//------------------------------------------------------------------------------
// void OnInsertManeuver(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertManeuver(wxCommandEvent &event)
{
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentItemId = GetItemParent(itemId);
   wxString name;
   name.Printf("Maneuver%d", ++mNumManeuver);

   wxTreeItemId prevItemId = GetPrevSibling(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevItemId);
   
   if (prevItem == NULL)
   {
      prevItemId = GetPrevVisible(itemId);
      prevItem = (MissionTreeItemData *)GetItemData(prevItemId);   
   }   

   GmatCommand *prevCmd = prevItem->GetCommand();

   if (prevCmd != NULL)
   {
      GmatCommand *cmd =
         //theGuiInterpreter->CreateCommand("Maneuver", std::string(name.c_str()));//loj: 6/15/04
         theGuiInterpreter->CreateDefaultCommand("Maneuver", std::string(name.c_str()));
      
      if (theGuiInterpreter->InsertCommand(cmd, prevCmd))
      {  
         if (prevCmd->GetTypeName() == "NoOp") // ag: ||prevCmd->GetTypeName() == "Target")
         {
            InsertItem(parentItemId, 0, name, GmatTree::ICON_FILE, -1,
                       new MissionTreeItemData(name, GmatTree::MANEUVER_COMMAND,
                                               name, cmd));
         }
         else
         {
            InsertItem(parentItemId, prevItemId, name, GmatTree::ICON_FILE, -1,
                       new MissionTreeItemData(name, GmatTree::MANEUVER_COMMAND,
                                               name, cmd));
         }
      }
   }
}

//------------------------------------------------------------------------------
// void OnInsertTarget(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertTarget(wxCommandEvent &event)
{
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentItemId = GetItemParent(itemId);
   wxString name;
   name.Printf("Target%d", ++mNumTarget); 
 
   wxTreeItemId prevItemId = GetPrevSibling(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevItemId);
  
   if (prevItem == NULL)
   {
      prevItemId = GetPrevVisible(itemId);
      prevItem = (MissionTreeItemData *)GetItemData(prevItemId);   
   }   
  
   GmatCommand *prevCmd = prevItem->GetCommand();
  
   if (prevCmd != NULL)
   {
      GmatCommand *cmd =
         theGuiInterpreter->CreateCommand("Target", std::string(name.c_str()));
        
      wxTreeItemId targetId;

      if (theGuiInterpreter->InsertCommand(cmd, prevCmd))
      {
         if (prevCmd->GetTypeName() == "NoOp") //ag: ||prevCmd->GetTypeName() == "Target")
         {
            targetId = InsertItem(parentItemId, 0, name, GmatTree::ICON_FOLDER, -1,
                                  new MissionTreeItemData(name, GmatTree::TARGET_COMMAND,
                                                          name, cmd));
         }
         else
         {
            targetId = InsertItem(parentItemId, prevItemId, name, GmatTree::ICON_FOLDER, -1,
                                  new MissionTreeItemData(name, GmatTree::TARGET_COMMAND,
                                                          name, cmd));
         }
            
         SetItemImage(targetId, GmatTree::ICON_OPENFOLDER, 
                      wxTreeItemIcon_Expanded);
      
         Expand(itemId);
      }
   }
}

//------------------------------------------------------------------------------
// void OnInsertIfStatement(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertIfStatement(wxCommandEvent &event)
{
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentItemId = GetItemParent(itemId);
   wxString name;
   name.Printf("If%d", ++mNumIfStatement); 
 
   wxTreeItemId prevItemId = GetPrevSibling(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevItemId);

   if (prevItem == NULL)
   {
      prevItemId = GetPrevVisible(itemId);
      prevItem = (MissionTreeItemData *)GetItemData(prevItemId);   
   }   
  
   GmatCommand *prevCmd = prevItem->GetCommand();
   wxTreeItemId targetId;

   if (prevCmd != NULL)
   {
      // ag: Need GUI interpreter to handle control logic
      //        GmatCommand *cmd =
      //            theGuiInterpreter->CreateCommand("Target", std::string(name.c_str()));
        
      //        wxTreeItemId targetId;

      //        if (theGuiInterpreter->InsertCommand(cmd, prevCmd))
      //        {
      if (prevCmd->GetTypeName() == "NoOp") //ag: ||prevCmd->GetTypeName() == "Target")
      {
         targetId = InsertItem(parentItemId, 0, name, GmatTree::ICON_FOLDER, -1,
                               new MissionTreeItemData(name, GmatTree::IF_CONTROL,
                                                       name, NULL));
      }
      else
      {
         targetId = InsertItem(parentItemId, prevItemId, name, GmatTree::ICON_FOLDER, -1,
                               new MissionTreeItemData(name, GmatTree::IF_CONTROL,
                                                       name, NULL));
      }
      
      SetItemImage(targetId, GmatTree::ICON_OPENFOLDER, 
                   wxTreeItemIcon_Expanded);
      
      Expand(itemId);
      //        }
   }
}

//------------------------------------------------------------------------------
// void OnInsertWhileLoop(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertWhileLoop(wxCommandEvent &event)
{
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentItemId = GetItemParent(itemId);
   wxString name;
   name.Printf("While%d", ++mNumWhileLoop); 
 
   wxTreeItemId prevItemId = GetPrevSibling(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevItemId);

   if (prevItem == NULL)
   {
      prevItemId = GetPrevVisible(itemId);
      prevItem = (MissionTreeItemData *)GetItemData(prevItemId);   
   }   
  
   GmatCommand *prevCmd = prevItem->GetCommand();
   wxTreeItemId targetId;
  
   if (prevCmd != NULL)
   {
      // ag: Need GUI interpreter to handle control logic
      //        GmatCommand *cmd =
      //            theGuiInterpreter->CreateCommand("Target", std::string(name.c_str()));
        
      //        wxTreeItemId targetId;

      //        if (theGuiInterpreter->InsertCommand(cmd, prevCmd))
      //        {
      if (prevCmd->GetTypeName() == "NoOp") //ag: ||prevCmd->GetTypeName() == "Target")
      {
         targetId = InsertItem(parentItemId, 0, name, GmatTree::ICON_FOLDER, -1,
                               new MissionTreeItemData(name, GmatTree::WHILE_CONTROL,
                                                       name, NULL));
      }
      else
      {
         targetId = InsertItem(parentItemId, prevItemId, name, GmatTree::ICON_FOLDER, -1,
                               new MissionTreeItemData(name, GmatTree::WHILE_CONTROL,
                                                       name, NULL));
      }
      
      SetItemImage(targetId, GmatTree::ICON_OPENFOLDER, 
                   wxTreeItemIcon_Expanded);
      
      Expand(itemId);
      //        }
   }
}

//------------------------------------------------------------------------------
// void OnInsertForLoop(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertForLoop(wxCommandEvent &event)
{
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentItemId = GetItemParent(itemId);
   wxString name;
   name.Printf("For%d", ++mNumForLoop); 
 
   wxTreeItemId prevItemId = GetPrevSibling(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevItemId);
 
   if (prevItem == NULL)
   {
      prevItemId = GetPrevVisible(itemId);
      prevItem = (MissionTreeItemData *)GetItemData(prevItemId);   
   }   
   
   GmatCommand *prevCmd = prevItem->GetCommand();
   wxTreeItemId targetId;

   if (prevCmd != NULL)
   {
      // ag: Need GUI interpreter to handle control logic
      //        GmatCommand *cmd =
      //            theGuiInterpreter->CreateCommand("Target", std::string(name.c_str()));
        
      //        wxTreeItemId targetId;

      //        if (theGuiInterpreter->InsertCommand(cmd, prevCmd))
      //        {
      if (prevCmd->GetTypeName() == "NoOp") //ag: ||prevCmd->GetTypeName() == "Target")
      {
         targetId = InsertItem(parentItemId, 0, name, GmatTree::ICON_FOLDER, -1,
                               new MissionTreeItemData(name, GmatTree::FOR_CONTROL,
                                                       name, NULL));
      }
      else
      {
         targetId = InsertItem(parentItemId, prevItemId, name, GmatTree::ICON_FOLDER, -1,
                               new MissionTreeItemData(name, GmatTree::FOR_CONTROL,
                                                       name, NULL));
      }
      
      SetItemImage(targetId, GmatTree::ICON_OPENFOLDER, 
                   wxTreeItemIcon_Expanded);
      
      Expand(itemId);
      //        }
   }
}

//------------------------------------------------------------------------------
// void OnInsertDoWhile(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertDoWhile(wxCommandEvent &event)
{
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentItemId = GetItemParent(itemId);
   wxString name;
   name.Printf("Do%d", ++mNumDoWhile); 
 
   wxTreeItemId prevItemId = GetPrevSibling(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevItemId);

   if (prevItem == NULL)
   {
      prevItemId = GetPrevVisible(itemId);
      prevItem = (MissionTreeItemData *)GetItemData(prevItemId);   
   }   
  
   GmatCommand *prevCmd = prevItem->GetCommand();
   wxTreeItemId targetId;

   if (prevCmd != NULL)
   {
      // ag: Need GUI interpreter to handle control logic
      //        GmatCommand *cmd =
      //            theGuiInterpreter->CreateCommand("Target", std::string(name.c_str()));
        
      //        wxTreeItemId targetId;

      //        if (theGuiInterpreter->InsertCommand(cmd, prevCmd))
      //        {
      if (prevCmd->GetTypeName() == "NoOp") //ag: ||prevCmd->GetTypeName() == "Target")
      {
         targetId = InsertItem(parentItemId, 0, name, GmatTree::ICON_FOLDER, -1,
                               new MissionTreeItemData(name, GmatTree::DO_CONTROL,
                                                       name, NULL));
      }
      else
      {
         targetId = InsertItem(parentItemId, prevItemId, name, GmatTree::ICON_FOLDER, -1,
                               new MissionTreeItemData(name, GmatTree::DO_CONTROL,
                                                       name, NULL));
      }
      
      SetItemImage(targetId, GmatTree::ICON_OPENFOLDER, 
                   wxTreeItemIcon_Expanded);
      
      Expand(itemId);
      //        }
   }
}

//------------------------------------------------------------------------------
// void OnInsertSwitchCase(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertSwitchCase(wxCommandEvent &event)
{
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentItemId = GetItemParent(itemId);
   wxString name;
   name.Printf("Switch%d", ++mNumSwitchCase); 
 
   wxTreeItemId prevItemId = GetPrevSibling(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevItemId);

   if (prevItem == NULL)
   {
      prevItemId = GetPrevVisible(itemId);
      prevItem = (MissionTreeItemData *)GetItemData(prevItemId);   
   }   
  
   GmatCommand *prevCmd = prevItem->GetCommand();
   wxTreeItemId targetId;

   if (prevCmd != NULL)
   {
      // ag: Need GUI interpreter to handle control logic
      //        GmatCommand *cmd =
      //            theGuiInterpreter->CreateCommand("Target", std::string(name.c_str()));
        
      //        wxTreeItemId targetId;

      //        if (theGuiInterpreter->InsertCommand(cmd, prevCmd))
      //        {
      if (prevCmd->GetTypeName() == "NoOp") //ag: ||prevCmd->GetTypeName() == "Target")
      {
         targetId = InsertItem(parentItemId, 0, name, GmatTree::ICON_FOLDER, -1,
                               new MissionTreeItemData(name, GmatTree::SWITCH_CONTROL,
                                                       name, NULL));
      }
      else
      {
         targetId = InsertItem(parentItemId, prevItemId, name, GmatTree::ICON_FOLDER, -1,
                               new MissionTreeItemData(name, GmatTree::SWITCH_CONTROL,
                                                       name, NULL));
      }
      
      SetItemImage(targetId, GmatTree::ICON_OPENFOLDER, 
                   wxTreeItemIcon_Expanded);
      
      Expand(itemId);
      //        }
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

   menu->Append(POPUP_CONTROL_LOGIC, "Control Logic", CreateAddControlLogicPopupMenu());

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

   menu->Append(POPUP_ADD_IF_STATEMENT, wxT("If Statement"));
   menu->Append(POPUP_ADD_WHILE_LOOP, wxT("While Loop")); 
   menu->Append(POPUP_ADD_FOR_LOOP, wxT("For Loop"));
   menu->Append(POPUP_ADD_D0_WHILE, wxT("Do While"));
   menu->Append(POPUP_ADD_SWITCH_CASE, wxT("Switch Case")); 
       
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

   menu->Append(POPUP_INSERT_IF_STATEMENT, wxT("If Statement"));
   menu->Append(POPUP_INSERT_WHILE_LOOP, wxT("While Loop")); 
   menu->Append(POPUP_INSERT_FOR_LOOP, wxT("For Loop"));
   menu->Append(POPUP_INSERT_D0_WHILE, wxT("Do While"));
   menu->Append(POPUP_INSERT_SWITCH_CASE, wxT("Switch Case")); 
       
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

