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
#include "bitmaps/propagateevent.xpm"
#include "bitmaps/target.xpm"
#include "bitmaps/whileloop.xpm"
#include "bitmaps/varyevent.xpm"
#include "bitmaps/achieveevent.xpm"
#include "bitmaps/deltav.xpm"
#include "bitmaps/callfunction.xpm"
#include "bitmaps/nestreturn.xpm"

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
//   EVT_UPDATE_UI(-1, DecoratedTree::OnPaint)
   //ag: 10/20/2004 Commented out so that the position of the click is not
   // checked to open up a panel from the variables/goals boxes
   //loj: 11/4/04 Uncommented so that double click on Target/If/For/While folder
   // will not collapse
   EVT_LEFT_DCLICK(MissionTree::OnDoubleClick)
   
   EVT_TREE_ITEM_RIGHT_CLICK(-1, MissionTree::OnItemRightClick)
   EVT_TREE_ITEM_ACTIVATED(-1, MissionTree::OnItemActivated)
   
//   EVT_MENU_HIGHLIGHT(POPUP_SWAP_BEFORE, MissionTree::OnBefore)
//   EVT_MENU_HIGHLIGHT(POPUP_SWAP_AFTER, MissionTree::OnAfter)

   EVT_MENU(POPUP_OPEN, MissionTree::OnOpen)
   EVT_MENU(POPUP_CLOSE, MissionTree::OnClose)

   EVT_MENU(POPUP_ADD_MISSION_SEQ, MissionTree::OnAddMissionSeq)
   EVT_MENU(POPUP_ADD_MANEUVER, MissionTree::OnAddManeuver)
   EVT_MENU(POPUP_ADD_PROPAGATE, MissionTree::OnAddPropagate)
   EVT_MENU(POPUP_ADD_ACHIEVE, MissionTree::OnAddAchieve)
   EVT_MENU(POPUP_ADD_VARY, MissionTree::OnAddVary)
   EVT_MENU(POPUP_ADD_FUNCTION, MissionTree::OnAddFunction)
//   EVT_MENU(POPUP_ADD_ASSIGNMENT, MissionTree::OnAddAssignment)
//   EVT_MENU(POPUP_ADD_TOGGLE, MissionTree::OnAddToggle)
   EVT_MENU(POPUP_ADD_TARGET, MissionTree::OnAddTarget)
   EVT_MENU(POPUP_ADD_SCRIPT_EVENT, MissionTree::OnAddScriptEvent)
   EVT_MENU(POPUP_ADD_IF_STATEMENT, MissionTree::OnAddIfStatement)
   EVT_MENU(POPUP_ADD_IF_ELSE_STATEMENT, MissionTree::OnAddIfElseStatement)
   EVT_MENU(POPUP_ADD_WHILE_LOOP, MissionTree::OnAddWhileLoop)
   EVT_MENU(POPUP_ADD_FOR_LOOP, MissionTree::OnAddForLoop)
   EVT_MENU(POPUP_ADD_D0_WHILE, MissionTree::OnAddDoWhile)
   EVT_MENU(POPUP_ADD_SWITCH_CASE, MissionTree::OnAddSwitchCase)
   EVT_MENU(POPUP_ADD_ELSE_IF_STATEMENT, MissionTree::OnAddElseIfStatement)
   EVT_MENU(POPUP_ADD_ELSE_STATEMENT, MissionTree::OnAddElseStatement)

   EVT_MENU(POPUP_INSERT_MANEUVER, MissionTree::OnInsertManeuver)
   EVT_MENU(POPUP_INSERT_PROPAGATE, MissionTree::OnInsertPropagate)
   EVT_MENU(POPUP_INSERT_ACHIEVE, MissionTree::OnInsertAchieve)
   EVT_MENU(POPUP_INSERT_VARY, MissionTree::OnInsertVary)
   EVT_MENU(POPUP_INSERT_FUNCTION, MissionTree::OnInsertFunction)
//   EVT_MENU(POPUP_INSERT_ASSIGNMENT, MissionTree::OnInsertAssignment)
//   EVT_MENU(POPUP_INSERT_TOGGLE, MissionTree::OnInsertToggle)
   EVT_MENU(POPUP_INSERT_TARGET, MissionTree::OnInsertTarget)
   EVT_MENU(POPUP_INSERT_SCRIPT_EVENT, MissionTree::OnInsertScriptEvent)
   EVT_MENU(POPUP_INSERT_IF_STATEMENT, MissionTree::OnInsertIfStatement)
   EVT_MENU(POPUP_INSERT_WHILE_LOOP, MissionTree::OnInsertWhileLoop)
   EVT_MENU(POPUP_INSERT_FOR_LOOP, MissionTree::OnInsertForLoop)
   EVT_MENU(POPUP_INSERT_D0_WHILE, MissionTree::OnInsertDoWhile)
   EVT_MENU(POPUP_INSERT_SWITCH_CASE, MissionTree::OnInsertSwitchCase)

//   EVT_MENU(POPUP_VIEW_VARIABLES, MissionTree::OnViewVariables)
//   EVT_MENU(POPUP_VIEW_GOALS, MissionTree::OnViewGoals)

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
   : DecoratedTree(parent, id, pos, size, style),
     inScriptEvent(false)
{
   parent = parent;

   gridLines = false;     // change to true to show gridlines for goals/vars

   // mainNotebook = GmatAppData::GetMainNotebook();
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance(); //loj: 6/6/05 Added

   mCommandList.Clear();
   mCommandList.Add("Propagate");
   mCommandList.Add("Maneuver");
   mCommandList.Add("Target");
   mCommandList.Add("CallFunction");
//   mCommandList.Add("Assignment");
//   mCommandList.Add("Toggle");
   mCommandList.Add("ScriptEvent");
   
   if (gridLines)
   {
      // SetNodes();
      SetParameter(BOXCOUNT, 2);
      SetParameter(BOXWIDTH, 20);
      SetParameter(DRAWOUTLINE, 1);
   }
   else
   {
      SetParameter(BOXCOUNT, 0);
      SetParameter(DRAWOUTLINE, 0);
   }

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
//   mNumAssign = 0;
   mNumScriptEvent = 0;

   AddIcons();
   AddDefaultMission();

   // Put "G" for goals and "V" for variables
   // does it right-to-left
   // should spaces be put in for those that don't have
   // any text?
   if (gridLines)
   {
      SetString(-1, "V");
      SetString(-1, "G");
   }

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
//      mNumAssign = 0;
      mNumScriptEvent = 0;
      
      inScriptEvent = false;
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
   
   // Moved BeginScript to the top so it can catch all of the commands between
   // it and the EndScript
   if ((cmdTypeName == "BeginScript") || inScriptEvent)
   {
      if (!inScriptEvent) {
         mNewTreeId = AppendCommand(parent, GmatTree::MISSION_ICON_FILE,
            GmatTree::SCRIPT_COMMAND, cmd, &mNumScriptEvent,
            mNumScriptEvent);
         inScriptEvent = true;

         Expand(parent);
      }
      if (cmdTypeName == "EndScript")
         inScriptEvent = false;
   }
   else if (cmdTypeName == "Propagate")
   {
      mNewTreeId = 
         AppendCommand(parent, GmatTree::MISSION_ICON_PROPAGATE_EVENT,
                       GmatTree::PROPAGATE_COMMAND,
                       cmd, &mNumPropagate);

      Expand(parent);
   }
   else if (cmdTypeName == "Maneuver")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::MISSION_ICON_DELTA_V,
                       GmatTree::MANEUVER_COMMAND,
                       cmd, &mNumManeuver);
   
      Expand(parent);
   }
   else if (cmdTypeName == "Target")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::MISSION_ICON_TARGET,
                       GmatTree::TARGET_COMMAND,
                       cmd, &mNumTarget);
      
//      SetItemImage(mNewTreeId, GmatTree::MISSION_ICON_OPENFOLDER,
//                   wxTreeItemIcon_Expanded);

      Expand(parent);
   }
   else if (cmdTypeName == "EndTarget")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::MISSION_ICON_NEST_RETURN,
                       GmatTree::END_TARGET_COMMAND, cmd, &mNumTarget, mNumTarget);

      Expand(parent);
   }
   else if (cmdTypeName == "Achieve")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::MISSION_ICON_ACHIEVE, GmatTree::ACHIEVE_COMMAND,
                       cmd, &mNumAchieve);
      Expand(parent);
   }
   else if (cmdTypeName == "Vary")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::MISSION_ICON_VARY, GmatTree::VARY_COMMAND,
                       cmd, &mNumVary);
      Expand(parent);
   }
   else if (cmdTypeName.CmpNoCase("Save") == 0) //loj: actual command is "save"
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::MISSION_ICON_FILE, GmatTree::SAVE_COMMAND,
                       cmd, &mNumSave, mNumSave);

      Expand(parent);
   }
   else if (cmdTypeName == "Toggle")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::MISSION_ICON_FILE, GmatTree::TOGGLE_COMMAND,
                       cmd, &mNumToggle, mNumToggle);

      Expand(parent);
   }
   else if (cmdTypeName == "For")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::MISSION_ICON_FOLDER, GmatTree::FOR_CONTROL,
                       cmd, &mNumForLoop);
        
      SetItemImage(mNewTreeId, GmatTree::MISSION_ICON_OPENFOLDER,
                   wxTreeItemIcon_Expanded);

      Expand(parent);
   }
   else if (cmdTypeName == "EndFor")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::MISSION_ICON_NEST_RETURN,
                       GmatTree::END_FOR_CONTROL, cmd, &mNumForLoop, mNumForLoop);
      
      Expand(parent);
   }
   else if (cmdTypeName == "If")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::MISSION_ICON_FOLDER, GmatTree::IF_CONTROL,
                       cmd, &mNumIfStatement);
        
      SetItemImage(mNewTreeId, GmatTree::MISSION_ICON_OPENFOLDER,
                   wxTreeItemIcon_Expanded);

      Expand(parent);
   }
   else if (cmdTypeName == "EndIf")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::MISSION_ICON_NEST_RETURN,
                       GmatTree::END_IF_CONTROL, cmd, &mNumIfStatement,
                       mNumIfStatement);
        
      Expand(parent);
   }
   else if (cmdTypeName == "While")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::MISSION_ICON_WHILE, GmatTree::WHILE_CONTROL,
                       cmd, &mNumWhileLoop);

      Expand(parent);
   }
   else if (cmdTypeName == "EndWhile")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::MISSION_ICON_NEST_RETURN,
                       GmatTree::END_WHILE_CONTROL,
                       cmd, &mNumWhileLoop, mNumWhileLoop);
      
      Expand(parent);
   }
   else if (cmdTypeName == "CallFunction")
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::MISSION_ICON_FUNCTION, GmatTree::CALL_FUNCTION_COMMAND,
                       cmd, &mNumFunct, mNumFunct);

      Expand(parent);
   }
//   else if (cmdTypeName == "Assignment")
//   {
//      mNewTreeId =
//         AppendCommand(parent, GmatTree::MISSION_ICON_FILE, GmatTree::ASSIGNMENT_COMMAND,
//                       cmd, &mNumAssign, mNumAssign);
//
//      Expand(parent);
//   }
   else if (cmdTypeName == "NoOp")
   {

   }
   else
   {
      mNewTreeId =
         AppendCommand(parent, GmatTree::MISSION_ICON_FILE, GmatTree::SCRIPT_COMMAND,
                       cmd, &mNumScriptEvent, mNumScriptEvent);

      Expand(parent);
   }
//   else if (cmdTypeName == "ScriptEvent")
//   {
//      mNewTreeId =
//         AppendCommand(parent, GmatTree::MISSION_ICON_FILE, GmatTree::FREE_FORM_SCRIPT_COMMAND,
//                       cmd, &mNumScriptEvent, mNumScriptEvent);
//
//      Expand(parent);
//   }


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
// MissionTree::AppendCommand(wxTreeItemId parent, GmatTree::MissionIconType icon,
//                            GmatTree::ItemType type, GmatCommand *cmd,
//                            int *cmdCount, int endCount)
//------------------------------------------------------------------------------
/**
 * Appends command to command list and/or command tree.
 */
//------------------------------------------------------------------------------
wxTreeItemId
MissionTree::AppendCommand(wxTreeItemId parent, GmatTree::MissionIconType icon,
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

   if (nodeName.Contains("BeginScript"))
   {
      nodeName.Replace("BeginScript", "ScriptEvent");
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
//                            wxTreeItemId prevId, GmatTree::MissionIconType icon,
//                            GmatTree::ItemType type, GmatCommand *prevCmd,
//                            GmatCommand *cmd, int *cmdCount)
//------------------------------------------------------------------------------
/**
 * Inserts tree item.
 */
//------------------------------------------------------------------------------
wxTreeItemId
MissionTree::InsertCommand(wxTreeItemId parentId, wxTreeItemId currId,
                           wxTreeItemId prevId, GmatTree::MissionIconType icon,
                           GmatTree::ItemType type, GmatCommand *prevCmd,
                           GmatCommand *cmd, int *cmdCount)
{
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(currId); 
   GmatCommand *currCmd = currItem->GetCommand();
   wxString typeName = cmd->GetTypeName().c_str();
   wxString nodeName = cmd->GetName().c_str();
   wxTreeItemId node;
   GmatCommand *endCmd = NULL;
   GmatTree::ItemType endType = GmatTree::END_TARGET_COMMAND;
   bool cmdAdded = false;

#if DEBUG_MISSION_TREE
   MissionTreeItemData *parentItem = (MissionTreeItemData *)GetItemData(parentId); 
   GmatCommand *parentCmd = parentItem->GetCommand();
   MessageInterface::ShowMessage
      ("MissionTree::InsertCommand() parentCmd=%s, prevCmd=%s, currCmd=%s\n",
       parentCmd->GetTypeName().c_str(), prevCmd->GetTypeName().c_str(),
       currCmd->GetTypeName().c_str());
#endif
   
   
   //------------------------------------------------------------
   // Create End* command if branch command
   //------------------------------------------------------------
   if (typeName == "Target" || typeName == "For"  ||  typeName == "While" ||
       typeName == "If" || typeName == "BeginScript") //loj: add Do, Switch later
   {
      if (typeName == "Target")
      {
         endCmd = theGuiInterpreter->CreateCommand("EndTarget");
         endType = GmatTree::END_TARGET_COMMAND;
      }
      else if (typeName == "For")
      {
         endCmd = theGuiInterpreter->CreateCommand("EndFor");
         endType = GmatTree::END_FOR_CONTROL;
      }
      else if (typeName == "While")
      {
         endCmd = theGuiInterpreter->CreateCommand("EndWhile");
         endType = GmatTree::END_WHILE_CONTROL;
      }
      else if (typeName == "If")
      {
         endCmd = theGuiInterpreter->CreateCommand("EndIf");
         endType = GmatTree::END_IF_CONTROL;
      }
      if (typeName == "BeginScript")
      {
         endCmd = theGuiInterpreter->CreateCommand("EndScript");
         endType = GmatTree::END_TARGET_COMMAND;
      }

#if DEBUG_MISSION_TREE
      MessageInterface::ShowMessage("----- Appending End*...\n");
#endif
      cmdAdded = cmd->Append(endCmd);
   }
   
   //------------------------------------------------------------
   // Compose node name
   //------------------------------------------------------------
   if (nodeName.Trim() == "")
   {
      nodeName.Printf("%s%d", typeName.c_str(), ++(*cmdCount));
   }

   if (nodeName.Contains("BeginScript"))
   {
      nodeName.Replace("BeginScript", "ScriptEvent");
   }

#if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::InsertCommand() cmd=%s, nodeName=%s\n",
       typeName.c_str(), nodeName.c_str());
#endif
   
   //------------------------------------------------------------
   // Insert command to mission sequence
   //------------------------------------------------------------
   wxString prevTypeName = prevCmd->GetTypeName().c_str();
   
   if (currCmd->GetTypeName() == "NoOp")
   {
#if DEBUG_MISSION_TREE
      MessageInterface::ShowMessage("----- Appending command...\n");
#endif
      // Append to base command list
      cmdAdded = theGuiInterpreter->AppendCommand(cmd);
   }
   else
   {
#if DEBUG_MISSION_TREE
      MessageInterface::ShowMessage("----- Inserting command...\n");
#endif
      cmdAdded = theGuiInterpreter->InsertCommand(cmd, prevCmd);
   }
   
   //------------------------------------------------------------
   // Insert command to mission tree
   //------------------------------------------------------------
   if (cmdAdded)
   {
      if (currCmd->GetTypeName() == "NoOp")
      {
         node = AppendItem(currId, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, type, nodeName, cmd));
      }
      else if (prevTypeName == "NoOp" ||
          prevTypeName == "Target" || prevTypeName == "For"  ||
          prevTypeName == "While" || prevTypeName == "If") //loj: add Do, Switch later
      {
         node = InsertItem(parentId, 0, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, type, nodeName, cmd));
      }
      else if (prevTypeName.Contains("End"))
      {
         wxString currTypeName = currCmd->GetTypeName().c_str();

         // work around for now - check if current command is if, while, for
         if (currTypeName == "If" || currTypeName == "While" || 
             currTypeName == "For" || currTypeName == "Target")
         {
           // subtract 2 from number of children (subtracting 1 gives the end)
           node = InsertItem(parentId, GetPrevSibling(GetLastChild(currId)),
                       nodeName, icon, -1,
                       new MissionTreeItemData(nodeName, type, nodeName, cmd));
         }
         else
         {
           node = InsertItem(parentId, GetPrevSibling(currId), nodeName, icon, -1,
                       new MissionTreeItemData(nodeName, type, nodeName, cmd));
         }
      }
      else
      {
         node = InsertItem(parentId, prevId, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, type, nodeName, cmd));
      }
      
      if (typeName == "Target" || typeName == "For"  ||  typeName == "While" ||
          typeName == "If") //loj: add Do, Switch later
      {
         //---------------------------------------------------
         // Append End* command
         //---------------------------------------------------
         wxString endName = "End" + typeName;
         endName.Printf("%s%d", endName.c_str(), *cmdCount);
         InsertItem(node, 0, endName, GmatTree::MISSION_ICON_NEST_RETURN, -1,
                    new MissionTreeItemData(endName, endType, endName, endCmd));
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
      AppendItem(mission, wxT("MissionSequence"), GmatTree::MISSION_ICON_FOLDER,
                 -1, new MissionTreeItemData(wxT("MissionSequence"),
                                             GmatTree::MISSION_SEQ_TOP_FOLDER));
    
   SetItemImage(mMissionSeqTopItem, GmatTree::MISSION_ICON_OPENFOLDER,
               wxTreeItemIcon_Expanded);

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
   //    AppendItem(item, wxT(objName), GmatTree::MISSION_ICON_FOLDER, -1,
   //                     new MissionTreeItemData(wxT(objName), GmatTree::MISSION_SEQ_COMMAND));
   //};

   wxString name;
    
   name.Printf("Sequence%d", ++mNumMissionSeq);
  
   mMissionSeqSubItem =
      AppendItem(item, name, GmatTree::MISSION_ICON_FOLDER, -1,
                 new MissionTreeItemData(name, 
                                         GmatTree::MISSION_SEQ_SUB_FOLDER));
    
   SetItemImage(mMissionSeqSubItem, GmatTree::MISSION_ICON_OPENFOLDER,
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
   wxIcon icons[12];

   icons[0] = wxIcon ( propagateevent_xpm );
   icons[1] = wxIcon ( target_xpm );
   icons[2] = wxIcon ( folder_xpm );
   icons[3] = wxIcon ( file_xpm );
   icons[4] = wxIcon ( openfolder_xpm );
   icons[5] = wxIcon ( openfolder_xpm );
   icons[6] = wxIcon ( whileloop_xpm );
   icons[7] = wxIcon ( varyevent_xpm );
   icons[8] = wxIcon ( achieveevent_xpm );
   icons[9] = wxIcon ( deltav_xpm );
   icons[10]= wxIcon ( callfunction_xpm );
   icons[11]= wxIcon ( nestreturn_xpm );

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
   // get some info about this item
   wxTreeItemId itemId = event.GetItem();
   MissionTreeItemData *item = (MissionTreeItemData *)GetItemData(itemId);
   
#if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::OnItemActivated() item=%s\n",
                                 item->GetDesc().c_str());
#endif
   
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
   //wxPoint position = event.GetPosition();
   //MessageInterface::ShowMessage("Event position is %d %d\n", position.x, position.y );
   
   wxTreeItemId itemId = GetSelection();
   MissionTreeItemData *item = (MissionTreeItemData *)GetItemData(itemId);

#if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::OnDoubleClick() item=%s\n", item->GetDesc().c_str());
#endif
   
   //loj: 11/04/04 added
   // Show panel here. because OnItemActivated() always collapse the node.
   GmatAppData::GetMainFrame()->CreateChild(item);

   //CheckClickIn(position);
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
      else if ((dataType == GmatTree::WHILE_CONTROL) ||
               (dataType == GmatTree::DO_CONTROL) ||
               (dataType == GmatTree::FOR_CONTROL)) //||
//               (dataType == GmatTree::ELSE_IF_CONTROL) ||
//               (dataType == GmatTree::ELSE_CONTROL))   
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
//void MissionTree::OnBefore ()
//{
//   before = true;
//}
//
////------------------------------------------------------------------------------
////------------------------------------------------------------------------------
//void MissionTree::OnAfter ()
//{
//   before = false;
//}

//------------------------------------------------------------------------------
// void OnAddMissionSeq(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddMissionSeq(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   wxString name;
    
   name.Printf("Sequence%d", ++mNumMissionSeq);
  
   mMissionSeqSubItem =
      AppendItem(item, name, GmatTree::MISSION_ICON_FOLDER, -1,
                 new MissionTreeItemData(name, GmatTree::MISSION_SEQ_SUB_FOLDER));
    
   SetItemImage(mMissionSeqSubItem, GmatTree::MISSION_ICON_OPENFOLDER,
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
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);
   GmatCommand *prevCmd = NULL;

   //loj: 12/6/04 - handle case prevItem is NULL
   if (prevItem != NULL)
      prevCmd = prevItem->GetCommand();
   else
      prevCmd = currItem->GetCommand();
   
   GmatCommand *cmd = theGuiInterpreter->CreateDefaultCommand("Propagate");

   if (cmd != NULL)
   {
      InsertCommand(itemId, itemId, prevId, GmatTree::MISSION_ICON_PROPAGATE_EVENT,
                    GmatTree::PROPAGATE_COMMAND, prevCmd, cmd, &mNumPropagate);
      
      Expand(itemId);
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
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);
   GmatCommand *prevCmd = NULL;
   
   //loj: 12/6/04 - handle case prevItem is NULL
   if (prevItem != NULL)
      prevCmd = prevItem->GetCommand();
   else
      prevCmd = currItem->GetCommand();

#if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::OnAddManeuver() prevCmd=%s\n",
                                 prevCmd->GetTypeName().c_str());
#endif
   
   GmatCommand *cmd = theGuiInterpreter->CreateDefaultCommand("Maneuver");

   if (cmd != NULL)
   {
      InsertCommand(itemId, itemId, prevId, GmatTree::MISSION_ICON_DELTA_V,
                    GmatTree::MANEUVER_COMMAND, prevCmd, cmd, &mNumManeuver);

      Expand(itemId);
      theGuiManager->UpdateBurn(); //loj: 6/6/05 Added
   }
   
#if DEBUG_MISSION_TREE
   ShowCommands("After OnAddManeuver()");
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
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   
   GmatCommand *prevCmd = NULL;

   //loj: 12/6/04 - handle case prevItem is NULL
   if (prevItem != NULL)
      prevCmd = prevItem->GetCommand();
   else
      prevCmd = currItem->GetCommand();

   GmatCommand *cmd = theGuiInterpreter->CreateDefaultCommand("Achieve");
    
   if (cmd != NULL)
   {
      InsertCommand(itemId, itemId, prevId, GmatTree::MISSION_ICON_ACHIEVE,
                    GmatTree::ACHIEVE_COMMAND, prevCmd, cmd, &mNumAchieve);

      Expand(itemId);
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
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   
   GmatCommand *prevCmd = NULL;

   //loj: 12/6/04 - handle case prevItem is NULL
   if (prevItem != NULL)
      prevCmd = prevItem->GetCommand();
   else
      prevCmd = currItem->GetCommand();

   GmatCommand *cmd = theGuiInterpreter->CreateDefaultCommand("Vary");
   
   if (cmd != NULL)
   {
      InsertCommand(itemId, itemId, prevId, GmatTree::MISSION_ICON_VARY,
                    GmatTree::VARY_COMMAND, prevCmd, cmd, &mNumVary);

      Expand(itemId);
   }

#if DEBUG_MISSION_TREE
   ShowCommands("After OnAddVary()");
#endif
}

//------------------------------------------------------------------------------
// void OnAddFunction(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddFunction(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnAddFunction()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId lastId = GetLastChild(itemId);
   wxTreeItemId prevId = GetPrevVisible(lastId);
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);
   GmatCommand *prevCmd = NULL;

   //loj: 12/6/04 - handle case prevItem is NULL
   if (prevItem != NULL)
      prevCmd = prevItem->GetCommand();
   else
      prevCmd = currItem->GetCommand();

#if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("Adding call function to gui interpreter");
#endif
   GmatCommand *cmd = theGuiInterpreter->CreateCommand("CallFunction");

   if (cmd != NULL)
   {
      InsertCommand(itemId, itemId, prevId, GmatTree::MISSION_ICON_FUNCTION,
                    GmatTree::CALL_FUNCTION_COMMAND, prevCmd, cmd, &mNumFunct);

      Expand(itemId);
   }

#if DEBUG_MISSION_TREE
   ShowCommands("After OnAddFunction()");
#endif


}

//------------------------------------------------------------------------------
// void OnAddAssignment(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddAssignment(wxCommandEvent &event)
{
//#if DEBUG_MISSION_TREE
//   ShowCommands("Before OnAddAssignment()");
//#endif
//   wxTreeItemId itemId = GetSelection();
//   wxTreeItemId lastId = GetLastChild(itemId);
//   wxTreeItemId prevId = GetPrevVisible(lastId);
//   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(itemId);
//   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);
//   GmatCommand *prevCmd = NULL;
//
//   //loj: 12/6/04 - handle case prevItem is NULL
//   if (prevItem != NULL)
//      prevCmd = prevItem->GetCommand();
//   else
//      prevCmd = currItem->GetCommand();
//
//#if DEBUG_MISSION_TREE
//   MessageInterface::ShowMessage("Adding assignment to gui interpreter");
//#endif
//   GmatCommand *cmd = theGuiInterpreter->CreateCommand("Assignment");
//
//   if (cmd != NULL)
//   {
//      InsertCommand(itemId, itemId, prevId, GmatTree::MISSION_ICON_FILE,
//                    GmatTree::ASSIGNMENT_COMMAND, prevCmd, cmd, &mNumAssign);
//
//      Expand(itemId);
//   }
//
//#if DEBUG_MISSION_TREE
//   ShowCommands("After OnAddAssignment()");
//#endif
//

}

//------------------------------------------------------------------------------
// void OnAddToggle(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddToggle(wxCommandEvent &event)
{
//#if DEBUG_MISSION_TREE
//   ShowCommands("Before OnAddToggle()");
//#endif
//   wxTreeItemId itemId = GetSelection();
//   wxTreeItemId lastId = GetLastChild(itemId);
//   wxTreeItemId prevId = GetPrevVisible(lastId);
//   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(itemId);
//   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);
//   GmatCommand *prevCmd = NULL;
//
//   //loj: 12/6/04 - handle case prevItem is NULL
//   if (prevItem != NULL)
//      prevCmd = prevItem->GetCommand();
//   else
//      prevCmd = currItem->GetCommand();
//
//#if DEBUG_MISSION_TREE
//   MessageInterface::ShowMessage("Adding toggle to gui interpreter");
//#endif
//   GmatCommand *cmd = theGuiInterpreter->CreateDefaultCommand("Toggle");
//
//   if (cmd != NULL)
//   {
//      InsertCommand(itemId, itemId, prevId, GmatTree::MISSION_ICON_FILE,
//                    GmatTree::TOGGLE_COMMAND, prevCmd, cmd, &mNumToggle);
//
//      Expand(itemId);
//   }
//
//#if DEBUG_MISSION_TREE
//   ShowCommands("After OnAddToggle()");
//#endif
//
//
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
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   
   GmatCommand *prevCmd = NULL;

   //loj: 12/6/04 - handle case prevItem is NULL
   if (prevItem != NULL)
      prevCmd = prevItem->GetCommand();
   else
      prevCmd = currItem->GetCommand();

#if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::OnAddTarget() prevCmd=%s\n",
                                 prevCmd->GetTypeName().c_str());
#endif
   
   GmatCommand *cmd = theGuiInterpreter->CreateDefaultCommand("Target");

   if (cmd != NULL)
   {
      wxTreeItemId node =
         InsertCommand(itemId, itemId, prevId, GmatTree::MISSION_ICON_TARGET,
                       GmatTree::TARGET_COMMAND, prevCmd, cmd, &mNumTarget);
      
      //SetItemImage(node, GmatTree::MISSION_ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

      Expand(itemId);
      Expand(node);
   }

#if DEBUG_MISSION_TREE
   ShowCommands("After OnAddTarget()");
#endif
}

//------------------------------------------------------------------------------
// void OnAddScriptEvent(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddScriptEvent(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnAddScriptEvent()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId lastId = GetLastChild(itemId);
   wxTreeItemId prevId = GetPrevVisible(lastId);
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   
   GmatCommand *prevCmd = NULL;

   //loj: 12/6/04 - handle case prevItem is NULL
   if (prevItem != NULL)
      prevCmd = prevItem->GetCommand();
   else
      prevCmd = currItem->GetCommand();

#if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::OnAddScriptEvent() prevCmd=%s\n",
                                 prevCmd->GetTypeName().c_str());
#endif
   
   GmatCommand *cmd = theGuiInterpreter->CreateCommand("BeginScript");

   if (cmd != NULL)
   {
      wxTreeItemId node =
         InsertCommand(itemId, itemId, prevId, GmatTree::MISSION_ICON_FILE,
                       GmatTree::SCRIPT_COMMAND, prevCmd, cmd,
                       &mNumScriptEvent);

      //SetItemImage(node, GmatTree::MISSION_ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

      Expand(itemId);
      Expand(node);
   }

#if DEBUG_MISSION_TREE
   ShowCommands("After OnAddScriptEvent()");
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
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   
   GmatCommand *prevCmd = NULL;
  
   //loj: 12/6/04 - handle case prevItem is NULL
   if (prevItem != NULL)
      prevCmd = prevItem->GetCommand();
   else
      prevCmd = currItem->GetCommand();

   GmatCommand *cmd = theGuiInterpreter->CreateCommand("If");
   
   if (cmd != NULL)
   {
      wxTreeItemId node =
         InsertCommand(itemId, itemId, prevId, GmatTree::MISSION_ICON_FOLDER,
                       GmatTree::IF_CONTROL, prevCmd, cmd, &mNumIfStatement);
         
      SetItemImage(node, GmatTree::MISSION_ICON_OPENFOLDER, wxTreeItemIcon_Expanded);
      
      Expand(itemId);
      Expand(node);
   }

#if DEBUG_MISSION_TREE
   ShowCommands("After OnAddIfStatement()");
#endif
}

//------------------------------------------------------------------------------
// void OnAddIfElseStatement(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddIfElseStatement(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnAddIfElseStatement()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId lastId = GetLastChild(itemId);
   wxTreeItemId prevId = GetPrevVisible(lastId);
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   
   GmatCommand *prevCmd = NULL;
  
   //loj: 12/6/04 - handle case prevItem is NULL
   if (prevItem != NULL)
      prevCmd = prevItem->GetCommand();
   else
      prevCmd = currItem->GetCommand();

   GmatCommand *cmd = theGuiInterpreter->CreateCommand("If");
   GmatCommand *elseCmd = theGuiInterpreter->CreateCommand("Else");
   wxTreeItemId node;
   
   if (cmd != NULL)
   {
      node =
         InsertCommand(itemId, itemId, prevId, GmatTree::MISSION_ICON_FOLDER,
                       GmatTree::IF_CONTROL, prevCmd, cmd, &mNumIfStatement);
         
      SetItemImage(node, GmatTree::MISSION_ICON_OPENFOLDER, wxTreeItemIcon_Expanded);
      
      Expand(itemId);
      Expand(node);
   }

   if (elseCmd != NULL)
   {
      // able to insert into command sequence
      wxTreeItemId elseNode =
         InsertCommand(node, node, node, GmatTree::MISSION_ICON_FILE,
                       GmatTree::ELSE_CONTROL, cmd, elseCmd, &mNumIfStatement);
         
      Expand(node);
   }

#if DEBUG_MISSION_TREE
   ShowCommands("After OnAddIfElseStatement()");
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
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   
   GmatCommand *prevCmd = NULL;

   //loj: 12/6/04 - handle case prevItem is NULL
   if (prevItem != NULL)
      prevCmd = prevItem->GetCommand();
   else
      prevCmd = currItem->GetCommand();

   GmatCommand *cmd = theGuiInterpreter->CreateCommand("While");
   
   if (cmd != NULL)
   {
      wxTreeItemId node =
         InsertCommand(itemId, itemId, prevId, GmatTree::MISSION_ICON_WHILE,
                       GmatTree::WHILE_CONTROL, prevCmd, cmd, &mNumWhileLoop);
      
      Expand(itemId);
      Expand(node);
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
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   
   GmatCommand *prevCmd = NULL;

   //loj: 12/6/04 - handle case prevItem is NULL
   if (prevItem != NULL)
      prevCmd = prevItem->GetCommand();
   else
      prevCmd = currItem->GetCommand();

   GmatCommand *cmd = theGuiInterpreter->CreateCommand("For");
   
   if (cmd != NULL)
   {
      wxTreeItemId node =
         InsertCommand(itemId, itemId, prevId, GmatTree::MISSION_ICON_FOLDER,
                       GmatTree::FOR_CONTROL, prevCmd, cmd, &mNumForLoop);
         
      SetItemImage(node, GmatTree::MISSION_ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

      Expand(itemId);
      Expand(node);
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
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   
   GmatCommand *prevCmd = NULL;

   //loj: 12/6/04 - handle case prevItem is NULL
   if (prevItem != NULL)
      prevCmd = prevItem->GetCommand();
   else
      prevCmd = currItem->GetCommand();

   GmatCommand *cmd = theGuiInterpreter->CreateCommand("Do");
   
   if (cmd != NULL)
   {
      wxTreeItemId node =
         InsertCommand(itemId, itemId, prevId, GmatTree::MISSION_ICON_FOLDER,
                       GmatTree::DO_CONTROL, prevCmd, cmd, &mNumDoWhile);
         
      SetItemImage(node, GmatTree::MISSION_ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

      Expand(itemId);
      Expand(node);
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
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);
   GmatCommand *prevCmd = NULL;

   //loj: 12/6/04 - handle case prevItem is NULL
   if (prevItem != NULL)
      prevCmd = prevItem->GetCommand();
   else
      prevCmd = currItem->GetCommand();

   GmatCommand *cmd = theGuiInterpreter->CreateCommand("Switch");
      
   if (cmd != NULL)
   {
      wxTreeItemId node =
         InsertCommand(itemId, itemId, prevId, GmatTree::MISSION_ICON_FOLDER,
                       GmatTree::SWITCH_CONTROL, prevCmd, cmd, &mNumSwitchCase);
         
      SetItemImage(node, GmatTree::MISSION_ICON_OPENFOLDER, wxTreeItemIcon_Expanded);
         
      Expand(itemId);
      Expand(node);
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
      //MessageInterface::ShowMessage("The cur sibling is %s\n", itemData->GetDesc().c_str());
      itemId = GetNextSibling(itemId);  
      itemData = (GmatTreeItemData *)GetItemData(itemId);
      //MessageInterface::ShowMessage("The next sibling is %s\n", itemData->GetDesc().c_str());
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
      InsertItem(parent, itemId, name, GmatTree::MISSION_ICON_FOLDER, -1,
                 new MissionTreeItemData(name, GmatTree::ELSE_IF_CONTROL, 
                                         name, NULL));
   SetItemImage(targetId, GmatTree::MISSION_ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);    
   //            AppendItem(item, endName, GmatTree::MISSION_ICON_NEST_RETURN, -1,
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
      InsertItem(parent, itemId, name, GmatTree::MISSION_ICON_FOLDER, -1,
                 new MissionTreeItemData(name, GmatTree::ELSE_CONTROL, 
                                         name, NULL));
   SetItemImage(targetId, GmatTree::MISSION_ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);    
   //            AppendItem(itemId, endName, GmatTree::MISSION_ICON_NEST_RETURN, -1,
   //                       new MissionTreeItemData(endName, GmatTree::END_CONTROL, 
   //                                               endName, NULL));

   Expand(itemId);
   //        }
   //    }

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
         InsertCommand(parentId, itemId, prevId, GmatTree::MISSION_ICON_PROPAGATE_EVENT,
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
      GmatCommand *cmd = theGuiInterpreter->CreateDefaultCommand("Maneuver");
      
      if (cmd != NULL)
      {  
         InsertCommand(parentId, itemId, prevId, GmatTree::MISSION_ICON_DELTA_V,
                       GmatTree::MANEUVER_COMMAND, prevCmd, cmd, &mNumManeuver);
      }
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnInsertManeuver()");
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
         InsertCommand(parentId, itemId, prevId, GmatTree::MISSION_ICON_ACHIEVE,
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
         InsertCommand(parentId, itemId, prevId, GmatTree::MISSION_ICON_VARY,
                       GmatTree::VARY_COMMAND, prevCmd, cmd, &mNumVary);
      }
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnInsertVary()");
#endif
}

//------------------------------------------------------------------------------
// void OnInsertFunction(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertFunction(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnInsertFunction()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentId = GetItemParent(itemId);
   wxTreeItemId prevId = GetPrevVisible(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   

   GmatCommand *prevCmd = prevItem->GetCommand();

   if (prevCmd != NULL)
   {     
      GmatCommand *cmd = theGuiInterpreter->CreateDefaultCommand("CallFunction");
            
      if (cmd != NULL)
      {  
         InsertCommand(parentId, itemId, prevId, GmatTree::MISSION_ICON_FUNCTION,
                       GmatTree::CALL_FUNCTION_COMMAND, prevCmd, cmd, &mNumFunct);
      }
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnInsertFunction()");
#endif
}

//------------------------------------------------------------------------------
// void OnInsertAssignment(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertAssignment(wxCommandEvent &event)
{
//#if DEBUG_MISSION_TREE
//   ShowCommands("Before OnInsertAssignment()");
//#endif
//   wxTreeItemId itemId = GetSelection();
//   wxTreeItemId parentId = GetItemParent(itemId);
//   wxTreeItemId prevId = GetPrevVisible(itemId);
//   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);
//
//   GmatCommand *prevCmd = prevItem->GetCommand();
//
//   if (prevCmd != NULL)
//   {
//      GmatCommand *cmd = theGuiInterpreter->CreateDefaultCommand("Assignment");
//
//      if (cmd != NULL)
//      {
//         InsertCommand(parentId, itemId, prevId, GmatTree::MISSION_ICON_FILE,
//                       GmatTree::ASSIGNMENT_COMMAND, prevCmd, cmd, &mNumAssign);
//      }
//   }
//#if DEBUG_MISSION_TREE
//   ShowCommands("After OnInsertAssignment()");
//#endif
}

//------------------------------------------------------------------------------
// void OnInsertToggle(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertToggle(wxCommandEvent &event)
{
//#if DEBUG_MISSION_TREE
//   ShowCommands("Before OnInsertToggle()");
//#endif
//   wxTreeItemId itemId = GetSelection();
//   wxTreeItemId parentId = GetItemParent(itemId);
//   wxTreeItemId prevId = GetPrevVisible(itemId);
//   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);
//
//   GmatCommand *prevCmd = prevItem->GetCommand();
//
//   if (prevCmd != NULL)
//   {
//      GmatCommand *cmd = theGuiInterpreter->CreateDefaultCommand("Toggle");
//
//      if (cmd != NULL)
//      {
//         InsertCommand(parentId, itemId, prevId, GmatTree::MISSION_ICON_FILE,
//                       GmatTree::TOGGLE_COMMAND, prevCmd, cmd, &mNumToggle);
//      }
//   }
//#if DEBUG_MISSION_TREE
//   ShowCommands("After OnInsertToggle()");
//#endif
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

      if (cmd != NULL)
      {
         wxTreeItemId node =
            InsertCommand(parentId, itemId, prevId, GmatTree::MISSION_ICON_TARGET,
                          GmatTree::TARGET_COMMAND, prevCmd, cmd, &mNumTarget);
      
//         SetItemImage(node, GmatTree::MISSION_ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

         Expand(node);
      }
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnInsertTarget()");
#endif
}

//------------------------------------------------------------------------------
// void OnInsertScriptEvent(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertScriptEvent(wxCommandEvent &event)
{
#if DEBUG_MISSION_TREE
   ShowCommands("Before OnInsertScriptEvent()");
#endif
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentId = GetItemParent(itemId); 
   wxTreeItemId prevId = GetPrevVisible(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   
  
   GmatCommand *prevCmd = prevItem->GetCommand();

   if (prevCmd != NULL)
   {
      GmatCommand *cmd = theGuiInterpreter->CreateCommand("BeginScript");

      if (cmd != NULL)
      {
         wxTreeItemId node =
            InsertCommand(parentId, itemId, prevId, GmatTree::MISSION_ICON_FILE,
                          GmatTree::SCRIPT_COMMAND, prevCmd, cmd,
                          &mNumScriptEvent);

//         SetItemImage(node, GmatTree::MISSION_ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

         Expand(node);
      }
   }
#if DEBUG_MISSION_TREE
   ShowCommands("After OnInsertScriptEvent()");
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

      if (cmd != NULL)
      {
         wxTreeItemId node =
            InsertCommand(parentId, itemId, prevId, GmatTree::MISSION_ICON_FOLDER,
                          GmatTree::IF_CONTROL, prevCmd, cmd, &mNumIfStatement);
         
         SetItemImage(node, GmatTree::MISSION_ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

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
      
      if (cmd != NULL)
      {
         wxTreeItemId node =
            InsertCommand(parentId, itemId, prevId, GmatTree::MISSION_ICON_WHILE,
                          GmatTree::WHILE_CONTROL,
                          prevCmd, cmd, &mNumWhileLoop);
         
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
      
      if (cmd != NULL)
      {
         wxTreeItemId node =
            InsertCommand(parentId, itemId, prevId, GmatTree::MISSION_ICON_FOLDER,
                          GmatTree::FOR_CONTROL, prevCmd, cmd, &mNumForLoop);
         
         SetItemImage(node, GmatTree::MISSION_ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

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
      
      if (cmd != NULL)
      {
         wxTreeItemId node =
            InsertCommand(parentId, itemId, prevId, GmatTree::MISSION_ICON_FOLDER,
                          GmatTree::DO_CONTROL, prevCmd, cmd, &mNumDoWhile);
         
         SetItemImage(node, GmatTree::MISSION_ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

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
      
      if (cmd != NULL)
      {
         // ag: switch the GmatTree type to SWITCH_CONTROL from DO_CONTROL 11/10/04
         wxTreeItemId node =
            InsertCommand(parentId, itemId, prevId, GmatTree::MISSION_ICON_FOLDER,
                          GmatTree::SWITCH_CONTROL, prevCmd, cmd, &mNumSwitchCase);
         
         SetItemImage(node, GmatTree::MISSION_ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

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

   for (i=0; i<mCommandList.GetCount(); i++)
      menu->Append(GetMenuId(mCommandList[i], false), mCommandList[i]);

//   menu->Enable(POPUP_ADD_TOGGLE, FALSE);

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
   
   for (i=0; i<mCommandList.GetCount(); i++)
      menu->Append(GetMenuId(mCommandList[i], true), mCommandList[i]);
   
//   menu->Enable(POPUP_INSERT_TOGGLE, FALSE);
   menu->Append(POPUP_CONTROL_LOGIC, "Control Logic", CreateInsertControlLogicPopupMenu());

   return menu;
}

//------------------------------------------------------------------------------
// wxMenu* CreateTargetPopupMenu(bool insert)
//------------------------------------------------------------------------------
wxMenu* MissionTree::CreateTargetPopupMenu(bool insert)
{
   wxMenu *menu;
   
   if (insert)
      menu = CreateInsertPopupMenu();
   else
      menu = CreateAddPopupMenu();

   menu = AppendTargetPopupMenu(menu, insert);

   return menu;
}

//------------------------------------------------------------------------------
// wxMenu* AppendTargetPopupMenu(wxMenu *menu, bool insert)
//------------------------------------------------------------------------------
wxMenu* MissionTree::AppendTargetPopupMenu(wxMenu *menu, bool insert)
{
   if (insert)
   {
      menu->Append(POPUP_INSERT_VARY, wxT("Vary"));
      menu->Append(POPUP_INSERT_ACHIEVE, wxT("Achieve"));
   }
   else
   {
      menu->Append(POPUP_ADD_VARY, wxT("Vary"));
      menu->Append(POPUP_ADD_ACHIEVE, wxT("Achieve"));
   }

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
   menu->Append(POPUP_ADD_IF_ELSE_STATEMENT, wxT("If/else"));
   menu->Enable(POPUP_ADD_IF_ELSE_STATEMENT, FALSE);
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
   wxMenu *menu = new wxMenu;
   
//     StringArray items = theGuiInterpreter->GetListOfFactoryItems(Gmat::COMMAND);

//     for (i=0; i<mCommandList.GetCount(); i++)
//     {
//        if (items[i] == "Propagate")
//           menu->Append(POPUP_ADD_PROPAGATE, wxT("Propagate")); //wxT(items[i].c_str()));
//        else if (items[i] == "Maneuver")
//           menu->Append(POPUP_ADD_MANEUVER, wxT("Maneuver"));
//        else if (items[i] == "Target")
//           menu->Append(POPUP_ADD_TARGET, wxT("Target"));
//        else if (items[i] == "CallFunction")
//           menu->Append(POPUP_ADD_FUNCTION, wxT("Function"));
//     }
   
   menu = CreateAddPopupMenu(); //loj: 11/15/04 added

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
   
   //menu->Append(POPUP_CONTROL_LOGIC, "Control Logic", popupMenu);

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
   menu->Append(POPUP_INSERT_IF_ELSE_STATEMENT, wxT("If/else"));
   menu->Enable(POPUP_INSERT_IF_ELSE_STATEMENT, FALSE);
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
//void MissionTree::OnViewVariables()
//{
//   MissionTreeItemData *item = new MissionTreeItemData(wxT("Variables"),
//                                                       GmatTree::VIEW_SOLVER_VARIABLES);
//   GmatAppData::GetMainFrame()->CreateChild(item);
//}
//
////------------------------------------------------------------------------------
//// void OnViewGoals()
////------------------------------------------------------------------------------
//void MissionTree::OnViewGoals()
//{
//   MissionTreeItemData *item = new MissionTreeItemData(wxT("Goals"),
//                                                       GmatTree::VIEW_SOLVER_GOALS);
//   GmatAppData::GetMainFrame()->CreateChild(item);
//}

//------------------------------------------------------------------------------
// void OnDelete()
//------------------------------------------------------------------------------
void MissionTree::OnDelete(wxCommandEvent &event)
{
   // get selected item
   wxTreeItemId itemId = GetSelection();
   
   // if panel is open close it
   OnClose(event);
   
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
void MissionTree::OnRun(wxCommandEvent &event)
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
void MissionTree::OnOpen(wxCommandEvent &event)
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
void MissionTree::OnClose(wxCommandEvent &event)
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
// int GetMenuId(const wxString &cmd, bool insert)
//------------------------------------------------------------------------------
int MissionTree::GetMenuId(const wxString &cmd, bool insert)
{
   //MessageInterface::ShowMessage("MissionTree::GetMenuId() cmd=%s, insert=%d\n",
   //                              cmd.c_str(), insert);
   int id = 0;
   
   for (unsigned int i=0; i<mCommandList.Count(); i++)
   {
      if (insert)
      {
         if (cmd == "Propagate")
            return POPUP_INSERT_PROPAGATE;
         else if (cmd == "Maneuver")
            return POPUP_INSERT_MANEUVER;
         else if (cmd == "Target")
            return POPUP_INSERT_TARGET;
         else if (cmd == "CallFunction")
            return POPUP_INSERT_FUNCTION;
//         else if (cmd == "Assignment")
//            return POPUP_INSERT_ASSIGNMENT;
//         else if (cmd == "Toggle")
//            return POPUP_INSERT_TOGGLE;
         else if (cmd == "ScriptEvent")
            return POPUP_INSERT_SCRIPT_EVENT;
         else
            MessageInterface::ShowMessage
               ("MissionTree::GetMenuId() Unknown command:%s\n", cmd.c_str());
      }
      else
      {
         if (cmd == "Propagate")
            return POPUP_ADD_PROPAGATE;
         else if (cmd == "Maneuver")
            return POPUP_ADD_MANEUVER;
         else if (cmd == "Target")
            return POPUP_ADD_TARGET;
         else if (cmd == "CallFunction")
            return POPUP_ADD_FUNCTION;
//         else if (cmd == "Assignment")
//            return POPUP_ADD_ASSIGNMENT;
//         else if (cmd == "Toggle")
//            return POPUP_ADD_TOGGLE;
         else if (cmd == "ScriptEvent")
            return POPUP_ADD_SCRIPT_EVENT;
         else
            MessageInterface::ShowMessage
               ("MissionTree::GetMenuId() Unknown command:%s\n", cmd.c_str());
      }
   }

   return id;
}

// for Debug
//------------------------------------------------------------------------------
// void ShowCommands(const wxString &msg = "")
//------------------------------------------------------------------------------
void MissionTree::ShowCommands(const wxString &msg)
{
   MessageInterface::ShowMessage("-------------------->%s\n", msg.c_str());
   
   GmatCommand *cmd = theGuiInterpreter->GetNextCommand();
   GmatCommand *child;
   
   while (cmd != NULL)
   {
      MessageInterface::ShowMessage("----- %s\n", cmd->GetTypeName().c_str());
      child = cmd->GetChildCommand(0);

      if (child != NULL)
      {
         ShowSubCommands(cmd, child, 0);
      }
      
      cmd = cmd->GetNext();
   }
   
   MessageInterface::ShowMessage("<--------------------\n");
}


//------------------------------------------------------------------------------
// void ShowSubCommands(GmatCommand *baseCmd, GmatCommand *cmd, Integer level)
//------------------------------------------------------------------------------
void MissionTree::ShowSubCommands(GmatCommand *baseCmd, GmatCommand *cmd,
                                  Integer level)
{
   GmatCommand *childCmd;
   Integer childNo = 0;

   if (cmd != baseCmd)
      ++level;
   
   while (cmd != baseCmd)
   {
      for (int i=0; i<level; i++)
         MessageInterface::ShowMessage("-----");
      
      MessageInterface::ShowMessage("----- %s\n", cmd->GetTypeName().c_str());
      childCmd = cmd->GetChildCommand(childNo);
      
      while (childCmd != NULL)
      {
         ShowSubCommands(cmd, childCmd, level);
         ++childNo;
         childCmd = cmd->GetChildCommand(childNo);
      }
      
      cmd = cmd->GetNext();
      childNo = 0;
   }
}
