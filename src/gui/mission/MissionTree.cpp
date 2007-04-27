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
#include "bitmaps/forloop.xpm"
#include "bitmaps/if.xpm"
#include "bitmaps/scriptevent.xpm"
#include "bitmaps/varyevent.xpm"
#include "bitmaps/achieveevent.xpm"
#include "bitmaps/deltav.xpm"
#include "bitmaps/callfunction.xpm"
#include "bitmaps/nestreturn.xpm"
#include "bitmaps/saveobject.xpm"
#include "bitmaps/equalsign.xpm"
#include "bitmaps/toggle.xpm"
#include "bitmaps/beginfb.xpm"
#include "bitmaps/endfb.xpm"
#include "bitmaps/report.xpm"
#include "bitmaps/stop.xpm"

#include "MissionTree.hpp"
#include "MissionTreeItemData.hpp"
#include "ViewTextFrame.hpp"

#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "MessageInterface.hpp"
#include "CommandUtil.hpp"         // for GetNextCommand()

// Should we sort the command list?
#define __SORT_COMMAND_LIST__

// Should we enable multiple mission sequence?
//#define__ENABLE_MULTIPLE_SEQUENCE__

//#define DEBUG_MISSION_TREE_SHOW_CMD 1
//#define DEBUG_MISSION_TREE 1
//#define DEBUG_MISSION_TREE_APPEND 1
//#define DEBUG_MISSION_TREE_INSERT 1
//#define DEBUG_MISSION_TREE_DELETE 1
//#define DEBUG_MISSION_TREE_CHILD 1
//#define DEBUG_MISSION_TREE_FIND 2
//#define DEBUG_MISSION_TREE_MENU 1

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
   //EVT_UPDATE_UI(-1, DecoratedTree::OnPaint)
   //ag: 10/20/2004 Commented out so that the position of the click is not
   // checked to open up a panel from the variables/goals boxes
   //loj: 11/4/04 Uncommented so that double click on Target/If/For/While folder
   // will not collapse
   EVT_LEFT_DCLICK(MissionTree::OnDoubleClick)
   
   EVT_TREE_ITEM_RIGHT_CLICK(-1, MissionTree::OnItemRightClick)
   EVT_TREE_ITEM_ACTIVATED(-1, MissionTree::OnItemActivated)
   
   EVT_MENU(POPUP_OPEN, MissionTree::OnOpen)
   EVT_MENU(POPUP_CLOSE, MissionTree::OnClose)
   
   EVT_MENU(POPUP_ADD_MISSION_SEQ, MissionTree::OnAddMissionSeq)
   
   EVT_MENU_RANGE(POPUP_APPEND_PROPAGATE, POPUP_APPEND_SWITCH,
                  MissionTree::OnAppend)
   EVT_MENU_RANGE(POPUP_INSERT_BEFORE_PROPAGATE, POPUP_INSERT_BEFORE_SWITCH,
                  MissionTree::OnInsertBefore)
   EVT_MENU_RANGE(POPUP_INSERT_AFTER_PROPAGATE, POPUP_INSERT_AFTER_SWITCH,
                  MissionTree::OnInsertAfter)
   
   EVT_MENU(POPUP_COLLAPSE, MissionTree::OnCollapse)
   EVT_MENU(POPUP_EXPAND, MissionTree::OnExpand)
   
   EVT_MENU(POPUP_RUN, MissionTree::OnRun)
   EVT_MENU(POPUP_DELETE, MissionTree::OnDelete)
   EVT_MENU(POPUP_SHOW_SCRIPT, MissionTree::OnShowScript)

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
     inScriptEvent(false),
     inFiniteBurn(false)
{
   parent = parent;
   
   // mainNotebook = GmatAppData::GetMainNotebook();
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   
   mCommandList.Clear();
   mCommandList.Add("Propagate");
   mCommandList.Add("Maneuver");
   mCommandList.Add("BeginFiniteBurn");
   mCommandList.Add("EndFiniteBurn");
   mCommandList.Add("Target");
   mCommandList.Add("Optimize");
   mCommandList.Add("CallFunction");
   mCommandList.Add("Report");
   mCommandList.Add("Toggle");
   mCommandList.Add("Save");
   mCommandList.Add("Stop");
   mCommandList.Add("GMAT");
   mCommandList.Add("ScriptEvent");
   
   // Should we sort the command list?
   #ifdef __SORT_COMMAND_LIST__
   mCommandList.Sort();
   #endif
   
   SetParameter(BOXCOUNT, 0);
   SetParameter(DRAWOUTLINE, 0);
   
   InitializeCounter();
   AddIcons();
   AddDefaultMission();
}


//------------------------------------------------------------------------------
// void ClearMission()
//------------------------------------------------------------------------------
/**
 * Clears Mission Sequence
 */
//------------------------------------------------------------------------------
void MissionTree::ClearMission()
{
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::ClearMission() entered\n");
   #endif
   
   // Collapse, so folder icon is closed
   #ifdef __WXMSW__
      Collapse(mMissionSeqSubItem);
   #endif
   
   DeleteChildren(mMissionSeqSubItem);
}


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
      InitializeCounter();      
      inScriptEvent = false;
      inFiniteBurn = false;
   }

   ClearMission();
   
   UpdateCommand();
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void InitializeCounter()
//------------------------------------------------------------------------------
/**
 * Initializes command counter.
 */
//------------------------------------------------------------------------------
void MissionTree::InitializeCounter()
{
   mScriptEventCount = 0;
   mTempCounter = 0;
   mNumManeuver = 0;
   mNumMissionSeq = 0;
   mNumPropagate = 0;
   mNumManeuver = 0;
   mNumTarget = 0;
   mNumOptimize = 0;
   mNumAchieve = 0;
   mNumVary = 0;
   mNumSave = 0;
   mNumReport = 0;
   mNumToggle = 0;
   mNumIfStatement = 0;
   mNumWhileLoop = 0;
   mNumForLoop = 0;
   mNumDoWhile = 0;
   mNumSwitchCase = 0;
   mNumFunct = 0;
   mNumAssign = 0;
   mNumScriptEvent = 0;
   mNumFiniteBurn = 0;
   mNumStop = 0;
   mNumMinimize = 0;
   mNumNonlinearConstraint = 0;
}


//------------------------------------------------------------------------------
// void UpdateCommand()
//------------------------------------------------------------------------------
/**
 * Updates commands in the mission sequence
 */
//------------------------------------------------------------------------------
void MissionTree::UpdateCommand()
{
   #if DEBUG_MISSION_TREE_SHOW_CMD
   MessageInterface::ShowMessage("MissionTree::UpdateCommand() entered\n");
   ShowCommands("InUpdateCommand()");
   #endif
   
   GmatCommand *cmd = theGuiInterpreter->GetFirstCommand();
   GmatCommand *child;
   MissionTreeItemData *seqItemData =
      (MissionTreeItemData *)GetItemData(mMissionSeqSubItem);
   wxTreeItemId node;
   
   if (cmd->GetTypeName() == "NoOp")
      seqItemData->SetCommand(cmd);
   
   while (cmd != NULL)
   {
      node = UpdateCommandTree(mMissionSeqSubItem, cmd);
      
      if (cmd->GetTypeName() == "BeginScript")
         mScriptEventCount++;
      
      if (cmd->GetTypeName() == "EndScript")
         mScriptEventCount--;
      
      inScriptEvent = (mScriptEventCount == 0) ? false : true;
      
      child = cmd->GetChildCommand(0);
      
      if (child != NULL)
      {
         ExpandChildCommand(node, cmd, 0);
      }
      
      cmd = cmd->GetNext();
      
   }
   
   Expand(mMissionSeqSubItem);   
   ScrollTo(mMissionSeqSubItem);

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
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::UpdateCommandTree() inScriptEvent=%d, cmd=(%p)%s\n",
       inScriptEvent, cmd, cmd->GetTypeName().c_str());
   #endif
   
   wxString cmdTypeName = cmd->GetTypeName().c_str();
   wxTreeItemId currId;
   
   // if ScriptEvent mode, we don't want to add to tree
   if (inScriptEvent)
      return mNewTreeId;
   
   
   if (cmdTypeName != "NoOp")
   {      
      mNewTreeId = AppendCommand(parent, GetIconId(cmdTypeName),
                                 GetCommandId(cmdTypeName), cmd,
                                 GetCommandCounter(cmdTypeName),
                                 *GetCommandCounter(cmdTypeName));
      
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
void MissionTree::ExpandChildCommand(wxTreeItemId parent, GmatCommand *cmd,
                                     Integer level)
{
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::ExpandChildCommand() cmd=%s, level=%d\n",
       cmd->GetTypeName().c_str(), level);
   #endif
   
   wxTreeItemId node;
   Integer childNo = 0;
   GmatCommand* nextInBranch;
   GmatCommand* child;
   
   while((child = cmd->GetChildCommand(childNo)) != NULL)
   {
      nextInBranch = child;
      
      while ((nextInBranch != NULL) && (nextInBranch != cmd))
      {
         #if DEBUG_MISSION_TREE_CHILD
         for (int i=0; i<=level; i++)
            MessageInterface::ShowMessage("-----");
         MessageInterface::ShowMessage
            ("----- (%p)%s\n", nextInBranch, nextInBranch->GetTypeName().c_str());
         #endif
         
         node = UpdateCommandTree(parent, nextInBranch);
         
         //loj: 12/7/06
         // In order to handle nested ScriptEvent, mScriptEventCounter is used
         // to decide whether in script mode or not
         
         if (nextInBranch->GetTypeName() == "BeginScript")
            mScriptEventCount++;
         
         if (nextInBranch->GetTypeName() == "EndScript")
            mScriptEventCount--;
         
         inScriptEvent = (mScriptEventCount == 0) ? false : true;
         
         if (nextInBranch->GetChildCommand() != NULL)
            ExpandChildCommand(node, nextInBranch, level+1);
         
         nextInBranch = nextInBranch->GetNext();
      }
      
      ++childNo;
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
   #if DEBUG_MISSION_TREE_APPEND
   MessageInterface::ShowMessage
      ("MissionTree::AppendCommand(%s) type = \"%s\" and name = \"%s\"\n",
       GetItemText(parent).c_str(),  cmd->GetTypeName().c_str(), 
       cmd->GetName().c_str());
   #endif
   
   wxString cmdTypeName = cmd->GetTypeName().c_str();
   wxString nodeName = cmd->GetName().c_str();
   
   wxTreeItemId node;
   
   // compose node name
   if (cmdTypeName.Contains("End"))
   {
      if (nodeName.Trim() == "")
         nodeName.Printf("%s%d", cmdTypeName.c_str(), endCount);
   }
   else if (cmdTypeName.Contains("Else"))
   {
      if (nodeName.Trim() == "")
         nodeName.Printf("%s%d", cmdTypeName.c_str(), endCount);
   }
   else
   {
      if (nodeName.Trim() == "" || nodeName == cmdTypeName)
         nodeName.Printf("%s%d", cmdTypeName.c_str(), ++(*cmdCount));      
   }
   
   if (nodeName.Contains("BeginScript"))
   {
      nodeName.Replace("BeginScript", "ScriptEvent");
   }
   
   #if DEBUG_MISSION_TREE_APPEND
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
//                            GmatTree::ItemType type, const wxString &cmdName,
//                            GmatCommand *prevCmd, GmatCommand *cmd, int *cmdCount,
//                            bool insertBefore)
//------------------------------------------------------------------------------
/**
 * Inserts command to mission tree.
 *
 * @param  parentId  Parent item id
 * @param  currId  Currently selected item id
 * @param  prevId  Previous item id
 * @param  icon  Icon to be used
 * @param  type  Item type, such as GmatTree::PROPAGATE
 * @param  cmdName  Command name to be inserted
 * @param  prevCmd  Previous command pointer
 * @param  cmd  Command pointer to be inserted
 * @param  cmdCount  Command counter to be appended to command name
 * @param  insertBefore  true if inserting before, false if inserterting after
 */
//------------------------------------------------------------------------------
wxTreeItemId
MissionTree::InsertCommand(wxTreeItemId parentId, wxTreeItemId currId,
                           wxTreeItemId prevId, GmatTree::MissionIconType icon,
                           GmatTree::ItemType type, const wxString &cmdName,
                           GmatCommand *prevCmd, GmatCommand *cmd, int *cmdCount,
                           bool insertBefore)
{
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(currId); 
   GmatCommand *currCmd = currItem->GetCommand();
   wxString currTypeName = currCmd->GetTypeName().c_str();
   wxString cmdTypeName = cmd->GetTypeName().c_str();
   wxString nodeName = cmd->GetName().c_str();
   wxTreeItemId node;
   GmatCommand *endCmd = NULL;
   GmatCommand *elseCmd = NULL;
   GmatTree::ItemType endType = GmatTree::END_TARGET;
   bool cmdAdded = false;
   
   wxString prevTypeName = prevCmd->GetTypeName().c_str();
   
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage
      ("MissionTree::InsertCommand() parentId=%s, currId=%s, prevId=%s, "
       "insertBefore=%d\n", GetItemText(parentId).c_str(), GetItemText(currId).c_str(),
       GetItemText(prevId).c_str(), insertBefore);
   
   MissionTreeItemData *parentItem = (MissionTreeItemData *)GetItemData(parentId); 
   GmatCommand *parentCmd = parentItem->GetCommand();
   MessageInterface::ShowMessage
      ("   cmdName=%s, cmdTypeName=%s, cmdCount=%d\n", cmdName.c_str(),
       cmdTypeName.c_str(), *cmdCount);
   MessageInterface::ShowMessage
      ("   parentCmd=%s, prevCmd=%s, prevTypeName=%s, currCmd=%s\n",
       parentCmd->GetTypeName().c_str(), prevCmd->GetTypeName().c_str(),
       prevTypeName.c_str(), currTypeName.c_str());
   #endif
   
   // If previous command is BeginScript, find matching EndScript,
   // since commands inside EndScript, including EndScript is not shown
   // on the tree.(loj: 12/11/06)
   if (prevCmd->GetTypeName() == "BeginScript")
   {
      GmatCommand *endScript = GmatCommandUtil::GetMatchingEnd(prevCmd);
      #if DEBUG_MISSION_TREE_APPEND
      MessageInterface::ShowMessage
         ("   setting prevCmd to %s\n", prevCmd->GetTypeName().c_str());
      #endif
      prevCmd = endScript;
   }
   
   
   //@Note "BeginFiniteBurn" is not a branch command but need "EndFiniteBurn"
   
   //------------------------------------------------------------
   // Create End* command if branch command
   //------------------------------------------------------------
   if (cmdTypeName == "Target" || cmdTypeName == "For"  ||
       cmdTypeName == "While"  ||  cmdTypeName == "If"  ||
       cmdTypeName == "BeginScript" || cmdTypeName == "Optimize")
       ////cmdTypeName == "BeginFiniteBurn")
   {
      if (cmdTypeName == "Target")
      {
         endCmd = theGuiInterpreter->CreateDefaultCommand("EndTarget");
         endType = GmatTree::END_TARGET;
      }
      else if (cmdTypeName == "For")
      {
         endCmd = theGuiInterpreter->CreateDefaultCommand("EndFor");
         endType = GmatTree::END_FOR_CONTROL;
      }
      else if (cmdTypeName == "While")
      {
         endCmd = theGuiInterpreter->CreateDefaultCommand("EndWhile");
         endType = GmatTree::END_WHILE_CONTROL;
      }
      else if (cmdTypeName == "If")
      {
         endCmd = theGuiInterpreter->CreateDefaultCommand("EndIf");
         endType = GmatTree::END_IF_CONTROL;
      }
      else if (cmdTypeName == "BeginScript")
      {
         endCmd = theGuiInterpreter->CreateDefaultCommand("EndScript");
         endType = GmatTree::END_TARGET;
      }
      else if (cmdTypeName == "Optimize")
      {
         endCmd = theGuiInterpreter->CreateDefaultCommand("EndOptimize");
         endType = GmatTree::END_OPTIMIZE;
      }
//       else if (cmdTypeName == "BeginFiniteBurn")
//       {
//          endCmd = theGuiInterpreter->CreateDefaultCommand("EndFiniteBurn", "", cmd);
//          endType = GmatTree::END_FINITE_BURN;
//       }
      
      // create Else (temp code)
      if (cmdName == "IfElse")
      {
         elseCmd = theGuiInterpreter->CreateDefaultCommand("Else");
         cmd->Append(elseCmd);
      }
      
      #if DEBUG_MISSION_TREE_INSERT
      MessageInterface::ShowMessage
         ("   ==> Calling cmd->Append(%s)\n", endCmd->GetTypeName().c_str());
      #endif
      
      cmdAdded = cmd->Append(endCmd);
      
      // Since GUI appends End command, reset previous command of cmd (loj: 12/14/06)
      
      #if DEBUG_MISSION_TREE_INSERT
      MessageInterface::ShowMessage
         ("MissionTree::InsertCommand() Setting previous of %s to %s\n",
          cmdTypeName.c_str(), prevTypeName.c_str());
      #endif
      
      cmd->ForceSetPrevious(prevCmd);
      
   }
   
   //------------------------------------------------------------
   // Compose node name
   //------------------------------------------------------------
   if (cmdTypeName == "Else")
      nodeName.Printf("%s%d", cmdTypeName.c_str(), (*cmdCount));
   
   // if command has name or command has the type name append counter
   if (nodeName.Trim() == "" || nodeName == cmdTypeName)
      nodeName.Printf("%s%d", cmdTypeName.c_str(), ++(*cmdCount));
   
//    if (nodeName.Contains("BeginScript"))
//       nodeName.Replace("BeginScript", "ScriptEvent");
   
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage
      ("   cmd=%s, nodeName=%s, cmdCount=%d\n", cmdTypeName.c_str(),
       nodeName.c_str(), *cmdCount);
   #endif
   
   //------------------------------------------------------------
   // Insert command to mission sequence
   //------------------------------------------------------------
   
   if (currCmd->GetTypeName() == "NoOp")
   {
      #if DEBUG_MISSION_TREE_INSERT
      MessageInterface::ShowMessage("   ==> Calling gui->AppendCommand()\n");
      #endif
      
      // Append to base command list
      cmdAdded = theGuiInterpreter->AppendCommand(cmd);
   }
   else
   {
      #if DEBUG_MISSION_TREE_INSERT
      MessageInterface::ShowMessage("   ==> Calling gui->InsertCommand()\n");
      #endif
      
      cmdAdded = theGuiInterpreter->InsertCommand(cmd, prevCmd);
   }
   
   //loj: 9/21/05 Set to true for now.
   // Why returning false eventhough it inserted?
   cmdAdded = true; 
   
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
      else if (cmdTypeName == "EndFiniteBurn")
      {
         node = AppendItem(parentId, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, type, nodeName, cmd));
      }
      else if (prevTypeName == "NoOp"  || prevTypeName == "Target" ||
               prevTypeName == "For"   || prevTypeName == "While"  ||
               prevTypeName == "If"    || prevTypeName == "Optimize")
               ////prevTypeName == "BeginFiniteBurn")
      {
         node = InsertItem(parentId, 0, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, type, nodeName, cmd));
      }
      else if (prevTypeName.Contains("End"))
      {
         wxString currTypeName = currCmd->GetTypeName().c_str();
         
         if (insertBefore)
         {
            node = InsertItem(parentId, GetPrevSibling(currId), nodeName, icon, -1,
                              new MissionTreeItemData(nodeName, type, nodeName, cmd));
         }
         else
         {
            node = InsertItem(parentId, currId, nodeName, icon, -1,
                              new MissionTreeItemData(nodeName, type, nodeName, cmd));
         }
      }
      else
      {
         #if DEBUG_MISSION_TREE_INSERT
         MessageInterface::ShowMessage
            ("   InsertItem(parentId=%s, prevId=%s, nodeName=%s)\n",
             GetItemText(parentId).c_str(), GetItemText(prevId).c_str(),
             nodeName.c_str());
         #endif
         
         node = InsertItem(parentId, prevId, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, type, nodeName, cmd));
      }
      
      //---------------------------------------------------------
      // Append End* command
      //---------------------------------------------------------
      if (cmdTypeName == "Target" || cmdTypeName == "For"  ||
          cmdTypeName == "While"  || cmdTypeName == "If"   ||
          cmdTypeName == "Optimize")//// || cmdTypeName == "BeginFiniteBurn")
      {
         // append Else (temp code until Else is implemented)
         if (cmdName == "IfElse")
         {
            wxString elseName;
            elseName.Printf("Else%d", (*cmdCount));
            
            wxTreeItemId elseNode =
               InsertItem(node, 0, elseName, icon, -1,
                          new MissionTreeItemData(elseName, GmatTree::ELSE_CONTROL,
                                                  elseName, elseCmd));

            //SetItemImage(elseNode, GmatTree::MISSION_ICON_OPENFOLDER,
            //   wxTreeItemIcon_Expanded);
            
            wxString endName = "End" + cmdTypeName;
            endName.Printf("%s%d", endName.c_str(), *cmdCount);
            InsertItem(node, elseNode, endName, GmatTree::MISSION_ICON_NEST_RETURN, -1,
                       new MissionTreeItemData(endName, endType, endName, endCmd));
         }
         else
         {
            wxString endName;
//             if (cmdTypeName == "BeginFiniteBurn")
//                endName = "EndFiniteBurn";
//             else
               endName = "End" + cmdTypeName;
            
            endName.Printf("%s%d", endName.c_str(), *cmdCount);
            InsertItem(node, 0, endName, GmatTree::MISSION_ICON_NEST_RETURN, -1,
                       new MissionTreeItemData(endName, endType, endName, endCmd));
         }
      }
   }
   else
   {
      MessageInterface::ShowMessage
         ("*** ERROR *** Command:%s not appended or created\n",
          cmd->GetTypeName().c_str());
   }
   
   return node;
}


//------------------------------------------------------------------------------
// void Append(const wxString &cmdName)
//------------------------------------------------------------------------------
void MissionTree::Append(const wxString &cmdName)
{
   #if DEBUG_MISSION_TREE_SHOW_CMD
   ShowCommands("Before Append: " + cmdName);
   #endif
   
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId lastId = GetLastChild(itemId);
   wxTreeItemId parentId = GetItemParent(itemId);
   wxTreeItemId currId = itemId;
   
   #if DEBUG_MISSION_TREE_APPEND
   MessageInterface::ShowMessage
      ("\nMissionTree::Append() cmdName=%s, itemId=%s, lastId=%s\n",
       cmdName.c_str(), GetItemText(itemId).c_str(), GetItemText(lastId).c_str());
   #endif
   
   // Append command before Else, if there is Else
   if (cmdName != "Else")
   {
      wxTreeItemId elseId = FindChild(itemId, "Else");

      if (elseId.IsOk())
         lastId = elseId;
   }
   
   #if DEBUG_MISSION_TREE_APPEND
   MessageInterface::ShowMessage
      ("   after FindChild(Else) itemId=<%s>, lastId=<%s>\n",
       GetItemText(itemId).c_str(), GetItemText(lastId).c_str());
   #endif

   // Note:
   // Previous command is the 2nd last visible command from the current node
   // For example:
   // Target
   //    If          <-- if appending command, previous command should be Maneuver
   //       Maneuver
   //    Else        <-- if appending command, previous command should be Propagate
   //       Propagate
   //    EndIf
   // EndTarget
   
   wxTreeItemId prevId = GetPrevVisible(lastId);
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);
   GmatCommand *currCmd = currItem->GetCommand();
   GmatCommand *prevCmd = currCmd;
   GmatCommand *cmd = NULL;
   
   // Since there may be commands not shown on the tree use
   // GmatCommandUtil::GetMatchingEnd() to get previous command.
   // Note: Make sure this works on other than Windows also (loj: 12/15/06)
   if (currCmd->IsOfType("BranchCommand"))
   {
      #if DEBUG_MISSION_TREE_APPEND
      MessageInterface::ShowMessage("   setting prevCmd to end->GetPrevious()\n");
      #endif
      GmatCommand *end = GmatCommandUtil::GetMatchingEnd(currCmd);
      prevCmd = end->GetPrevious();
   }
   else
   {
      // handle case prevItem is NULL
      if (prevItem != NULL)
      {
         #if DEBUG_MISSION_TREE_APPEND
         MessageInterface::ShowMessage("   setting prevCmd from prevItem\n");
         #endif
         prevCmd = prevItem->GetCommand();
      }
      else
      {
         #if DEBUG_MISSION_TREE_APPEND
         MessageInterface::ShowMessage("   setting prevCmd from currItem\n");
         #endif
         prevCmd = currItem->GetCommand();
      }
   }
   
   #if DEBUG_MISSION_TREE_APPEND
   MessageInterface::ShowMessage
      ("   currCmd=%s, prevCmd=%s\n",  currCmd->GetTypeName().c_str(),
       prevCmd->GetTypeName().c_str());
   #endif
   
   
   // Create a new command
   if (cmdName == "IfElse")
      cmd = theGuiInterpreter->CreateDefaultCommand("If");
   else
      cmd = theGuiInterpreter->CreateDefaultCommand(cmdName.c_str());
   
   
   // Insert a node to tree
   if (cmd != NULL)
   {
      UpdateGuiManager(cmdName);
      
      #if DEBUG_MISSION_TREE_APPEND
      MessageInterface::ShowMessage("   ==> Calling InsertCommand(before)\n");
      #endif
      
      // Need to set previous command of new command(loj: 12/15/06)
      cmd->ForceSetPrevious(prevCmd);
      
      wxTreeItemId node =
         InsertCommand(itemId, itemId, prevId, GetIconId(cmdName),
                       GetCommandId(cmdName), cmdName, prevCmd, cmd,
                       GetCommandCounter(cmdName), true);
      
      //SetItemImage(node, GmatTree::MISSION_ICON_OPENFOLDER, wxTreeItemIcon_Expanded);
      
      Expand(itemId);
      Expand(node);
   }
   
   #if DEBUG_MISSION_TREE_SHOW_CMD
   ShowCommands("After Append: " + cmdName);
   #endif
}


//------------------------------------------------------------------------------
// void InsertBefore(const wxString &cmdName)
//------------------------------------------------------------------------------
/*
 * Inserts a command before current selection.
 *
 * @param  cmdName  Command name to be inserted
 */
//------------------------------------------------------------------------------
void MissionTree::InsertBefore(const wxString &cmdName)
{
   #if DEBUG_MISSION_TREE_SHOW_CMD
   ShowCommands("Before InsertBefore(): " + cmdName);
   #endif
   
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentId = GetItemParent(itemId);

#ifdef __WXMAC__
   wxTreeItemId prevId = GetPrevSibling(itemId);

   // Check if no previous item then get parent item
   if (!prevId.IsOk())
   {
      if (parentId.IsOk())
         prevId = parentId;
   }
#else
   wxTreeItemId prevId = GetPrevVisible(itemId);
#endif

   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);
   
   // Do not insert anything if previous item is empty
   if (prevItem == NULL)
   {
      MessageInterface::ShowMessage
         ("\n***************  Warning ***************"
          "\nMissionTree::InsertBefore() has empty prevItem "
          "so it can't insert before this."
          "\n****************************************");
      return;
   }
   
   GmatCommand *currCmd = currItem->GetCommand();
   GmatCommand *prevCmd = currCmd->GetPrevious();
   GmatCommand *realPrevCmd = currCmd->GetPrevious();
   GmatCommand *cmd = NULL;
   
   // We want to use real previous command via cmd->GetPrevious(), not from
   // the tree, because some commands are not visible from the tree.
   
   //GmatCommand *prevCmd = prevItem->GetCommand();
   
   if (currCmd == NULL)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "*** Internal Error Occurred ***\n"
          "Current item has empty command. Cannot insert the command.\n");
      return;
   }
   
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage
      ("MissionTree::InsertBefore(%s) currCmd=%s, addr=%p\n", cmdName.c_str(),
       currCmd->GetTypeName().c_str(), currCmd);
   #endif
   
   
   if (prevCmd == NULL)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "*** Internal Error Occurred ***\n"
          "The previous command is empty. Cannot insert the command.\n");
      
      ShowCommands("Before Insert: " + cmdName);
      MessageInterface::ShowMessage
         ("InsertBefore(%s) currCmd=%s, addr=%p\n",
          cmdName.c_str(), currCmd->GetTypeName().c_str(), currCmd);
      
      return;
   }
   
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage
      ("MissionTree::InsertBefore(%s) prevCmd=%s, addr=%p\n",
       cmdName.c_str(), prevCmd->GetTypeName().c_str(), prevCmd);
   #endif
   
   // If previous command is BranchCmmand check to see we need to use matching
   // BranchEnd as previous command (loj: 12/15/06)
   if (prevCmd->IsOfType("BranchCommand"))
   {
      // check if first child is current command
      if (prevCmd->GetChildCommand(0) == currCmd)
         realPrevCmd = prevCmd;
      else
         realPrevCmd = GmatCommandUtil::GetMatchingEnd(prevCmd);
   }
   
   //if (prevCmd != NULL) //loj: 12/11/06 use realPrevCmd
   if (realPrevCmd != NULL)
   {
      if (cmdName == "IfElse")
      {
         cmd = theGuiInterpreter->CreateDefaultCommand("If");
      }
      else
      {
         cmd = theGuiInterpreter->
            CreateDefaultCommand(std::string(cmdName.c_str()));
      }
      
      // Need to set previous command (loj: 12/15/06)
      cmd->ForceSetPrevious(prevCmd);
      
      if (cmd != NULL)
      {
         UpdateGuiManager(cmdName);
         
         wxTreeItemId node =
            InsertCommand(parentId, itemId, prevId, GetIconId(cmdName),
                          GetCommandId(cmdName), cmdName, realPrevCmd, cmd,
                          GetCommandCounter(cmdName), true);
         
         //SetItemImage(node, GmatTree::MISSION_ICON_OPENFOLDER, 
         //   wxTreeItemIcon_Expanded);
         
         Expand(node);
      }
   }
   
   #if DEBUG_MISSION_TREE_SHOW_CMD
   ShowCommands("After InsertBefore(): " + cmdName);
   #endif
   
} // InsertBefore()


//------------------------------------------------------------------------------
// void InsertAfter(const wxString &cmdName)
//------------------------------------------------------------------------------
/*
 * Inserts a command after current selection.
 *
 * @param  cmdName  Command name to be inserted
 */
//------------------------------------------------------------------------------
void MissionTree::InsertAfter(const wxString &cmdName)
{
   #if DEBUG_MISSION_TREE_SHOW_CMD
   ShowCommands("Before InsertAfter(): " + cmdName);
   #endif
   
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentId = GetItemParent(itemId);
   
#ifdef __WXMAC__
   wxTreeItemId prevId = GetPrevSibling(itemId);

   // Check if no previous item then get parent item
   if (!prevId.IsOk())
   {
      if (parentId.IsOk())
         prevId = parentId;
   }
#else
   wxTreeItemId prevId = GetPrevVisible(itemId);
#endif
   
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(itemId);
   
   GmatCommand *currCmd = currItem->GetCommand();
   
   if (currCmd == NULL)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "*** Internal Error Occurred ***\n"
          "Current item has empty command. Cannot insert the command.\n");
      return;
   }
   
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage
      ("MissionTree::InsertAfter(%s) currCmd=%s, addr=%p\n", cmdName.c_str(),
       currCmd->GetTypeName().c_str(), currCmd);
   #endif
   
   
   GmatCommand *prevCmd = currCmd;
   
   GmatCommand *cmd = NULL;
   if (currCmd != NULL)
   {
      if (cmdName == "IfElse")
      {
         cmd = theGuiInterpreter->CreateDefaultCommand("If");
      }
      else
      {
         cmd = theGuiInterpreter->
            CreateDefaultCommand(std::string(cmdName.c_str()));
      }
      
      // Need to set previous command
      cmd->ForceSetPrevious(currCmd);
      
      // Set parentId, itemId, prevId properly to pass to InsertCommand()
      if (currCmd->IsOfType("BranchCommand"))
      {
         parentId = itemId;
      }
      else if (currCmd->IsOfType("BranchEnd"))
      {
         prevId = itemId;
         itemId = parentId;
         parentId = GetItemParent(itemId);
      }
      else
      {
         prevId = itemId;
      }
      
      
      if (cmd != NULL)
      {
         UpdateGuiManager(cmdName);
         
         wxTreeItemId node =
            InsertCommand(parentId, itemId, prevId, GetIconId(cmdName),
                          GetCommandId(cmdName), cmdName, prevCmd, cmd,
                          GetCommandCounter(cmdName), false);
         
         Expand(node);
      }
   }
   
   #if DEBUG_MISSION_TREE_SHOW_CMD
   ShowCommands("After InsertAfter(): " + cmdName);
   #endif
}


//------------------------------------------------------------------------------
// void UpdateGuiManager(const wxString &cmdName)
//------------------------------------------------------------------------------
/*
 * Calls GuiItemManager to update corresponding object list to dynamically
 * show new objects.
 *
 * @param  cmdName  Command name
 */
//------------------------------------------------------------------------------
void MissionTree::UpdateGuiManager(const wxString &cmdName)
{
   if (cmdName == "Maneuver" || cmdName == "BeginFiniteBurn" ||
       cmdName == "Vary")
      theGuiManager->UpdateBurn();
   
   if (cmdName == "Target" || cmdName == "Optimize" || cmdName == "Vary" ||
       cmdName == "Achieve" || cmdName == "Minimize")
      theGuiManager->UpdateSolver();
   
   if (cmdName == "Report")
      theGuiManager->UpdateSubscriber();
   
   // Always update parameter, since it is used in many commands
   theGuiManager->UpdateParameter();
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
   //----- Mission Sequence
   
   wxTreeItemId mission = AddRoot(wxT("Mission"), -1, -1,
                                  new MissionTreeItemData(wxT("Mission"),
                                                          GmatTree::MISSIONS_FOLDER));
   
   //-----------------------------------------------------------------
   #ifdef __ENABLE_MULTIPLE_SEQUENCE__
   //-----------------------------------------------------------------
   mMissionSeqTopItem =
      AppendItem(mission, wxT("Mission Sequence"), GmatTree::MISSION_ICON_FOLDER,
                 -1, new MissionTreeItemData(wxT("Mission Sequence"),
                                             GmatTree::MISSION_SEQ_TOP_FOLDER));
   
   SetItemImage(mMissionSeqTopItem, GmatTree::MISSION_ICON_OPENFOLDER,
               wxTreeItemIcon_Expanded);
   
   AddDefaultMissionSeq(mMissionSeqTopItem);
   
   //-----------------------------------------------------------------
   #else
   //-----------------------------------------------------------------
   
   mMissionSeqSubItem =
      AppendItem(mission, wxT("Mission Sequence"), GmatTree::MISSION_ICON_FOLDER,
                 -1, new MissionTreeItemData(wxT("Mission Sequence"),
                                             GmatTree::MISSION_SEQ_SUB_FOLDER));
   
   SetItemImage(mMissionSeqSubItem, GmatTree::MISSION_ICON_OPENFOLDER,
               wxTreeItemIcon_Expanded);
   
   //-----------------------------------------------------------------
   #endif
   //-----------------------------------------------------------------
   
   UpdateCommand();
   
   theGuiInterpreter->ResetConfigurationChanged(false, true);
   
}

//------------------------------------------------------------------------------
// void AddDefaultMissionSeq(wxTreeItemId item)
//------------------------------------------------------------------------------
void MissionTree::AddDefaultMissionSeq(wxTreeItemId item)
{
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::AddDefaultMission() entered\n");
   #endif
   
   #ifdef __ENABLE_MULTIPLE_SEQUENCE
   StringArray itemNames = theGuiInterpreter->GetListOfConfiguredItems(Gmat::MISSION_SEQ);
    
   int size = itemNames.size();
   for (int i = 0; i<size; i++)
   {
       wxString objName = wxString(itemNames[i].c_str());
       AppendItem(item, wxT(objName), GmatTree::MISSION_ICON_FOLDER, -1,
                  new MissionTreeItemData(wxT(objName), GmatTree::MISSION_SEQ_COMMAND));
   };
   #endif
   
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
   wxIcon icons[22];

   // Icons should follow the order in GmatTreeItemData::MissionIconType.
   icons[0] = wxIcon ( propagateevent_xpm );
   icons[1] = wxIcon ( target_xpm );
   icons[2] = wxIcon ( folder_xpm );
   icons[3] = wxIcon ( file_xpm );
   icons[4] = wxIcon ( openfolder_xpm );
   icons[5] = wxIcon ( openfolder_xpm );
   icons[6] = wxIcon ( whileloop_xpm );
   icons[7] = wxIcon ( forloop_xpm );
   icons[8] = wxIcon ( if_xpm );
   icons[9] = wxIcon ( scriptevent_xpm );
   icons[10] = wxIcon ( varyevent_xpm );
   icons[11] = wxIcon ( achieveevent_xpm );
   icons[12] = wxIcon ( deltav_xpm );
   icons[13]= wxIcon ( callfunction_xpm );
   icons[14]= wxIcon ( nestreturn_xpm );
   icons[15]= wxIcon ( saveobject_xpm );
   icons[16]= wxIcon ( equalsign_xpm );
   icons[17]= wxIcon ( toggle_xpm );
   icons[18]= wxIcon ( beginfb_xpm );
   icons[19]= wxIcon ( endfb_xpm );
   icons[20]= wxIcon ( report_xpm );
   icons[21]= wxIcon ( stop_xpm );

//   int sizeOrig = icons[0].GetWidth();
   for ( size_t i = 0; i < WXSIZEOF(icons); i++ )
   {
          // 30/01/06 - arg - always resize because linux is not showing unscaled
          // icons correctly
//      if ( size == sizeOrig )
//      {
//         images->Add(icons[i]);
//      }
//      else
//      {
         images->Add(wxBitmap(wxBitmap(icons[i]).ConvertToImage().Rescale(size, size)));
//      }
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
   MissionTreeItemData *parent = (MissionTreeItemData *)GetItemData(GetItemParent(itemId));
   
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::OnItemActivated() item=%s parent=%s\n",
                                 item->GetDesc().c_str(), parent->GetDesc().c_str());
   #endif

   // Since VaryPanel is used for both Target and Optimize,
   // set proper id indicating Optimize Vary
   if ((item->GetDataType() == GmatTree::VARY) &&
       (parent->GetDataType() == GmatTree::OPTIMIZE))
      item->SetDataType(GmatTree::OPTIMIZE_VARY);
   
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
   MissionTreeItemData *parent = (MissionTreeItemData *)GetItemData(GetItemParent(itemId));

   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::OnDoubleClick() item=%s parent=%s\n",
                                 item->GetDesc().c_str(), parent->GetDesc().c_str());
   #endif
   
   // Since VaryPanel is used for both Target and Optimize,
   // set proper id indicating Optimize Vary
   if ((item->GetDataType() == GmatTree::VARY) &&
       (parent->GetDataType() == GmatTree::OPTIMIZE))
      item->SetDataType(GmatTree::OPTIMIZE_VARY);
   
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
   
   #if DEBUG_MISSION_TREE_MENU
   MessageInterface::ShowMessage
      ("MissionTree::ShowMenu() dataType=%d\n", dataType);
   #endif
   
#if wxUSE_MENUS
   wxMenu menu;

   if (dataType == GmatTree::MISSION_SEQ_TOP_FOLDER)
   {
      menu.Append(POPUP_ADD_MISSION_SEQ, wxT("Add Mission Sequence"));
      menu.Enable(POPUP_ADD_MISSION_SEQ, FALSE);
   }
   else if (dataType == GmatTree::MISSION_SEQ_SUB_FOLDER)
   {
      menu.Append(POPUP_COLLAPSE, wxT("Collapse"));
      menu.Append(POPUP_EXPAND, wxT("Expand"));
      menu.AppendSeparator();
      menu.Append(POPUP_APPEND, wxT("Append"), CreatePopupMenu(dataType, APPEND));
      
      // If multiple sequence is enabled
      #ifdef __ENABLE_MULTIPLE_SEQUENCE__
      menu.Append(POPUP_DELETE, wxT("Delete"));
      #endif
      
      menu.Append(POPUP_RENAME, wxT("Rename"));
      menu.Enable(POPUP_RENAME, FALSE);
      menu.AppendSeparator();
      menu.Append(POPUP_RUN, wxT("Run"));
      menu.AppendSeparator();
      menu.Append(POPUP_SHOW_SCRIPT, wxT("Show Script"));
   }
   else
   {
      menu.Append(POPUP_OPEN, wxT("Open"));
      menu.Append(POPUP_CLOSE, wxT("Close"));
      menu.AppendSeparator();
      
      if (dataType == GmatTree::TARGET)
      {
         menu.Append(POPUP_APPEND, wxT("Append"),
                     CreateTargetPopupMenu(dataType, APPEND));         
         menu.Append(POPUP_INSERT_BEFORE, wxT("Insert Before"),
                     CreatePopupMenu(dataType, INSERT_BEFORE));         
         menu.Append(POPUP_INSERT_AFTER, wxT("Insert After"),
                     CreateTargetPopupMenu(dataType, INSERT_AFTER));
      }
      else if (dataType == GmatTree::OPTIMIZE)
      {
         menu.Append(POPUP_APPEND, wxT("Append"),
                     CreateOptimizePopupMenu(dataType, APPEND));         
         menu.Append(POPUP_INSERT_BEFORE, wxT("Insert Before"),
                     CreatePopupMenu(dataType, INSERT_BEFORE));         
         menu.Append(POPUP_INSERT_AFTER, wxT("Insert After"),
                     CreateOptimizePopupMenu(dataType, INSERT_AFTER));         
      }
      else if (dataType == GmatTree::END_TARGET)
      {
         menu.Append(POPUP_INSERT_BEFORE, wxT("Insert Before"),
                     CreateTargetPopupMenu(dataType, INSERT_BEFORE));
         menu.Append(POPUP_INSERT_AFTER, wxT("Insert After"),
                     CreatePopupMenu(dataType, INSERT_AFTER));
      }
      else if (dataType == GmatTree::END_OPTIMIZE)
      {
         menu.Append(POPUP_INSERT_BEFORE, wxT("Insert Before"),
                     CreateOptimizePopupMenu(dataType, INSERT_BEFORE));
         menu.Append(POPUP_INSERT_AFTER, wxT("Insert After"),
                     CreatePopupMenu(dataType, INSERT_AFTER));
      }
      else 
      {
         if (parentDataType == GmatTree::TARGET)
         {
            menu.Append(POPUP_INSERT_BEFORE, wxT("Insert Before"),
                        CreateTargetPopupMenu(dataType, INSERT_BEFORE));
            menu.Append(POPUP_INSERT_AFTER, wxT("Insert After"),
                        CreateTargetPopupMenu(dataType, INSERT_AFTER));
         }
         else if (parentDataType == GmatTree::OPTIMIZE)
         {
            menu.Append(POPUP_INSERT_BEFORE, wxT("Insert Before"),
                        CreateOptimizePopupMenu(dataType, INSERT_BEFORE));
            menu.Append(POPUP_INSERT_AFTER, wxT("Insert After"),
                        CreateOptimizePopupMenu(dataType, INSERT_AFTER));
         }
         else
         {
            menu.Append(POPUP_INSERT_BEFORE, wxT("Insert Before"),
                        CreatePopupMenu(dataType, INSERT_BEFORE));
            menu.Append(POPUP_INSERT_AFTER, wxT("Insert After"),
                        CreatePopupMenu(dataType, INSERT_AFTER));
         }
      }
      
      // Append is allowed for the control logic
      if ((dataType == GmatTree::IF_CONTROL) ||
          (dataType == GmatTree::ELSE_CONTROL) ||
          (dataType == GmatTree::FOR_CONTROL) ||
          (dataType == GmatTree::WHILE_CONTROL))
      {
         menu.Append(POPUP_APPEND, wxT("Append"),
                     CreatePopupMenu(dataType, APPEND));
      }
      
      // Delete and Rename applies to all, except End branch
      if (dataType < GmatTree::END_TARGET)
      {
         menu.AppendSeparator();
         menu.Append(POPUP_RENAME, wxT("Rename"));
         menu.Append(POPUP_DELETE, wxT("Delete"));
      }
      
      menu.Enable(POPUP_RENAME, FALSE);
      
   }
   
   PopupMenu(&menu, pt);
#endif // wxUSE_MENUS
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
      AppendItem(item, name, GmatTree::MISSION_ICON_FOLDER, -1,
                 new MissionTreeItemData(name, GmatTree::MISSION_SEQ_SUB_FOLDER));
    
   SetItemImage(mMissionSeqSubItem, GmatTree::MISSION_ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);

   Expand(item);
}


//------------------------------------------------------------------------------
// void OnAppend(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAppend(wxCommandEvent &event)
{
   switch (event.GetId())
   {
   case POPUP_APPEND_PROPAGATE:
      Append("Propagate");
      break;
   case POPUP_APPEND_MANEUVER:
      Append("Maneuver");
      break;
   case POPUP_APPEND_BEGIN_FINITE_BURN:
      Append("BeginFiniteBurn");
      break;
   case POPUP_APPEND_END_FINITE_BURN:
      Append("EndFiniteBurn");
      break;
   case POPUP_APPEND_TARGET:
      Append("Target");
      break;
   case POPUP_APPEND_OPTIMIZE:
      Append("Optimize");
      break;
   case POPUP_APPEND_VARY:
      Append("Vary");
      break;
   case POPUP_APPEND_ACHIEVE:
      Append("Achieve");
      break;
   case POPUP_APPEND_MINIMIZE:
      Append("Minimize");
      break;
   case POPUP_APPEND_NON_LINEAR_CONSTRAINT:
      Append("NonlinearConstraint");
      break;
   case POPUP_APPEND_FUNCTION:
      Append("CallFunction");
      break;
   case POPUP_APPEND_ASSIGNMENT:
      Append("GMAT");
      break;
   case POPUP_APPEND_REPORT:
      Append("Report");
      break;
   case POPUP_APPEND_TOGGLE:
      Append("Toggle");
      break;
   case POPUP_APPEND_SAVE:
      Append("Save");
      break;
   case POPUP_APPEND_STOP:
      Append("Stop");
      break;
   case POPUP_APPEND_SCRIPT_EVENT:
      Append("BeginScript");
      break;
   case POPUP_APPEND_IF:
      Append("If");
      break;
   case POPUP_APPEND_IF_ELSE:
      Append("IfElse");
      break;
   case POPUP_APPEND_ELSE:
      Append("Else");
      break;
   case POPUP_APPEND_ELSE_IF:
      //Append("ElseIf");
      break;
   case POPUP_APPEND_FOR:
      Append("For");
      break;
   case POPUP_APPEND_WHILE:
      Append("While");
      break;
   case POPUP_APPEND_D0_WHILE:
      //Append("Do");
      break;
   case POPUP_APPEND_SWITCH:
      //Append("Switch");
      break;
   default:
      break;
   }
}


//------------------------------------------------------------------------------
// void OnInsertBefore(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertBefore(wxCommandEvent &event)
{
   switch (event.GetId())
   {
   case POPUP_INSERT_BEFORE_PROPAGATE:
      InsertBefore("Propagate");
      break;
   case POPUP_INSERT_BEFORE_MANEUVER:
      InsertBefore("Maneuver");
      break;
   case POPUP_INSERT_BEFORE_BEGIN_FINITE_BURN:
      InsertBefore("BeginFiniteBurn");
      break;
   case POPUP_INSERT_BEFORE_END_FINITE_BURN:
      InsertBefore("EndFiniteBurn");
      break;
   case POPUP_INSERT_BEFORE_TARGET:
      InsertBefore("Target");
      break;
   case POPUP_INSERT_BEFORE_OPTIMIZE:
      InsertBefore("Optimize");
      break;
   case POPUP_INSERT_BEFORE_VARY:
      InsertBefore("Vary");
      break;
   case POPUP_INSERT_BEFORE_ACHIEVE:
      InsertBefore("Achieve");
      break;
   case POPUP_INSERT_BEFORE_MINIMIZE:
      InsertBefore("Minimize");
      break;
   case POPUP_INSERT_BEFORE_NON_LINEAR_CONSTRAINT:
      InsertBefore("NonlinearConstraint");
      break;
   case POPUP_INSERT_BEFORE_FUNCTION:
      InsertBefore("CallFunction");
      break;
   case POPUP_INSERT_BEFORE_ASSIGNMENT:
      InsertBefore("GMAT");
      break;
   case POPUP_INSERT_BEFORE_REPORT:
      InsertBefore("Report");
      break;
   case POPUP_INSERT_BEFORE_TOGGLE:
      InsertBefore("Toggle");
      break;
   case POPUP_INSERT_BEFORE_SAVE:
      InsertBefore("Save");
      break;
   case POPUP_INSERT_BEFORE_STOP:
      InsertBefore("Stop");
      break;
   case POPUP_INSERT_BEFORE_SCRIPT_EVENT:
      InsertBefore("BeginScript");
      break;
   case POPUP_INSERT_BEFORE_IF:
      InsertBefore("If");
      break;
   case POPUP_INSERT_BEFORE_IF_ELSE:
      InsertBefore("IfElse");
      break;
   case POPUP_INSERT_BEFORE_ELSE:
      InsertBefore("Else");
      break;
   case POPUP_INSERT_BEFORE_ELSE_IF:
      //InsertBefore("ElseIf");
      break;
   case POPUP_INSERT_BEFORE_FOR:
      InsertBefore("For");
      break;
   case POPUP_INSERT_BEFORE_WHILE:
      InsertBefore("While");
      break;
   case POPUP_INSERT_BEFORE_D0_WHILE:
      //InsertBefore("Do");
      break;
   case POPUP_INSERT_BEFORE_SWITCH:
      //InsertBefore("Switch");
      break;
   default:
      break;
   }
}


//------------------------------------------------------------------------------
// void OnInsertAfter(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertAfter(wxCommandEvent &event)
{
   switch (event.GetId())
   {
   case POPUP_INSERT_AFTER_PROPAGATE:
      InsertAfter("Propagate");
      break;
   case POPUP_INSERT_AFTER_MANEUVER:
      InsertAfter("Maneuver");
      break;
   case POPUP_INSERT_AFTER_BEGIN_FINITE_BURN:
      InsertAfter("BeginFiniteBurn");
      break;
   case POPUP_INSERT_AFTER_END_FINITE_BURN:
      InsertAfter("EndFiniteBurn");
      break;
   case POPUP_INSERT_AFTER_TARGET:
      InsertAfter("Target");
      break;
   case POPUP_INSERT_AFTER_OPTIMIZE:
      InsertAfter("Optimize");
      break;
   case POPUP_INSERT_AFTER_VARY:
      InsertAfter("Vary");
      break;
   case POPUP_INSERT_AFTER_ACHIEVE:
      InsertAfter("Achieve");
      break;
   case POPUP_INSERT_AFTER_MINIMIZE:
      InsertAfter("Minimize");
      break;
   case POPUP_INSERT_AFTER_NON_LINEAR_CONSTRAINT:
      InsertAfter("NonlinearConstraint");
      break;
   case POPUP_INSERT_AFTER_FUNCTION:
      InsertAfter("CallFunction");
      break;
   case POPUP_INSERT_AFTER_ASSIGNMENT:
      InsertAfter("GMAT");
      break;
   case POPUP_INSERT_AFTER_REPORT:
      InsertAfter("Report");
      break;
   case POPUP_INSERT_AFTER_TOGGLE:
      InsertAfter("Toggle");
      break;
   case POPUP_INSERT_AFTER_SAVE:
      InsertAfter("Save");
      break;
   case POPUP_INSERT_AFTER_STOP:
      InsertAfter("Stop");
      break;
   case POPUP_INSERT_AFTER_SCRIPT_EVENT:
      InsertAfter("BeginScript");
      break;
   case POPUP_INSERT_AFTER_IF:
      InsertAfter("If");
      break;
   case POPUP_INSERT_AFTER_IF_ELSE:
      InsertAfter("IfElse");
      break;
   case POPUP_INSERT_AFTER_ELSE:
      InsertAfter("Else");
      break;
   case POPUP_INSERT_AFTER_ELSE_IF:
      //InsertAfter("ElseIf");
      break;
   case POPUP_INSERT_AFTER_FOR:
      InsertAfter("For");
      break;
   case POPUP_INSERT_AFTER_WHILE:
      InsertAfter("While");
      break;
   case POPUP_INSERT_AFTER_D0_WHILE:
      //InsertAfter("Do");
      break;
   case POPUP_INSERT_AFTER_SWITCH:
      //InsertAfter("Switch");
      break;
   default:
      break;
   }
}


//---------------------------------
// Crete popup menu
//---------------------------------

//------------------------------------------------------------------------------
// wxMenu* CreatePopupMenu(int type, ActionType action)
//------------------------------------------------------------------------------
/*
 * Creates popup menu. It will create proper menu id depends on the input
 * action.
 *
 * @param  type  Command type id
 * @param  action  One of APPEND, INSERT_BEFORE, INSERT_AFTER
 */
//------------------------------------------------------------------------------
wxMenu* MissionTree::CreatePopupMenu(int type, ActionType action)
{
   #if DEBUG_MISSION_TREE_MENU
   MessageInterface::ShowMessage
      ("MissionTree::CreatePopupMenu() type=%d, insert=%d\n", type, action);
   #endif
   
   unsigned int i;
   wxMenu *menu = new wxMenu;
   
   for (i=0; i<mCommandList.GetCount(); i++)
      menu->Append(GetMenuId(mCommandList[i], action), mCommandList[i]);
   
   menu->Append(POPUP_CONTROL_LOGIC, "Control Logic",
                CreateControlLogicPopupMenu(type, action));
   
   return menu;
}


//------------------------------------------------------------------------------
// wxMenu* CreateTargetPopupMenu(int type, ActionType action)
//------------------------------------------------------------------------------
wxMenu* MissionTree::CreateTargetPopupMenu(int type, ActionType action)
{
   wxMenu *menu;
   
   menu = CreatePopupMenu(type, action);
   menu = AppendTargetPopupMenu(menu, action);
   
   return menu;
}


//------------------------------------------------------------------------------
// wxMenu* CreateOptimizePopupMenu(int type, ActionType action)
//------------------------------------------------------------------------------
wxMenu* MissionTree::CreateOptimizePopupMenu(int type, ActionType action)
{
   wxMenu *menu;
   
   menu = CreatePopupMenu(type, action);
   menu = AppendOptimizePopupMenu(menu, action);

   return menu;
}


//------------------------------------------------------------------------------
// wxMenu* AppendTargetPopupMenu(wxMenu *menu, ActionType action)
//------------------------------------------------------------------------------
wxMenu* MissionTree::AppendTargetPopupMenu(wxMenu *menu, ActionType action)
{
   switch (action)
   {
   case APPEND:
      menu->Append(POPUP_APPEND_VARY, wxT("Vary"));
      menu->Append(POPUP_APPEND_ACHIEVE, wxT("Achieve"));
      break;
      
   case INSERT_BEFORE:
      menu->Append(POPUP_INSERT_BEFORE_VARY, wxT("Vary"));
      menu->Append(POPUP_INSERT_BEFORE_ACHIEVE, wxT("Achieve"));
      break;
      
   case INSERT_AFTER:
      menu->Append(POPUP_INSERT_AFTER_VARY, wxT("Vary"));
      menu->Append(POPUP_INSERT_AFTER_ACHIEVE, wxT("Achieve"));
      break;
      
   default:
      break;
   }
   
   return menu;
}


//------------------------------------------------------------------------------
// wxMenu* AppendOptimizePopupMenu(wxMenu *menu, ActionType action)
//------------------------------------------------------------------------------
wxMenu* MissionTree::AppendOptimizePopupMenu(wxMenu *menu, ActionType action)
{
   switch (action)
   {
   case APPEND:
      menu->Append(POPUP_APPEND_VARY, wxT("Vary"));
      menu->Append(POPUP_APPEND_MINIMIZE, wxT("Minimize"));
      menu->Append(POPUP_APPEND_NON_LINEAR_CONSTRAINT, wxT("NonlinearConstraint"));
      break;
      
   case INSERT_BEFORE:
      menu->Append(POPUP_INSERT_BEFORE_VARY, wxT("Vary"));
      menu->Append(POPUP_INSERT_BEFORE_MINIMIZE, wxT("Minimize"));
      menu->Append(POPUP_INSERT_BEFORE_NON_LINEAR_CONSTRAINT, wxT("NonlinearConstraint"));
      break;
      
   case INSERT_AFTER:
      menu->Append(POPUP_INSERT_AFTER_VARY, wxT("Vary"));
      menu->Append(POPUP_INSERT_AFTER_MINIMIZE, wxT("Minimize"));
      menu->Append(POPUP_INSERT_AFTER_NON_LINEAR_CONSTRAINT, wxT("NonlinearConstraint"));
      break;
      
   default:
      break;
   }
   
   return menu;
}


//------------------------------------------------------------------------------
// wxMenu* CreateControlLogicPopupMenu(int type, ActionType action)
//------------------------------------------------------------------------------
wxMenu* MissionTree::CreateControlLogicPopupMenu(int type, ActionType action)
{
   #if DEBUG_MISSION_TREE_MENU
   MessageInterface::ShowMessage
      ("MissionTree::CreateControlLogicMenu() type=%d, action=%d\n",
       type, action);
   #endif
   
   wxMenu *menu = new wxMenu;
   bool addElse = false;
   
   if (type == GmatTree::IF_CONTROL || type == GmatTree::END_IF_CONTROL)
   {
      addElse = true;
      wxTreeItemId itemId = GetSelection();
      wxTreeItemId parentId = itemId;
      
      if (type == GmatTree::END_IF_CONTROL)
         parentId = GetItemParent(itemId);
      
      // show only one Else
      if (FindChild(parentId, "Else").IsOk())
         addElse = false;
   }

   switch (action)
   {
   case APPEND:
      menu->Append(POPUP_APPEND_IF, wxT("If"));
      menu->Append(POPUP_APPEND_IF_ELSE, wxT("If-Else"));
      
      if (addElse)
         menu->Append(POPUP_APPEND_ELSE, wxT("Else"));
      
      menu->Append(POPUP_APPEND_FOR, wxT("For"));
      menu->Append(POPUP_APPEND_WHILE, wxT("While")); 
      break;
      
   case INSERT_BEFORE:
      menu->Append(POPUP_INSERT_BEFORE_IF, wxT("If"));
      menu->Append(POPUP_INSERT_BEFORE_IF_ELSE, wxT("If-Else"));
      
      if (addElse)
         menu->Append(POPUP_INSERT_BEFORE_ELSE, wxT("Else"));
      
      menu->Append(POPUP_INSERT_BEFORE_FOR, wxT("For"));
      menu->Append(POPUP_INSERT_BEFORE_WHILE, wxT("While"));
      break;
      
   case INSERT_AFTER:
      menu->Append(POPUP_INSERT_AFTER_IF, wxT("If"));
      menu->Append(POPUP_INSERT_AFTER_IF_ELSE, wxT("If-Else"));
      
      if (addElse)
         menu->Append(POPUP_INSERT_AFTER_ELSE, wxT("Else"));
      
      menu->Append(POPUP_INSERT_AFTER_FOR, wxT("For"));
      menu->Append(POPUP_INSERT_AFTER_WHILE, wxT("While")); 
      break;
      
   default:
      break;
   }
   
   return menu;
}


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
   
   // save command type to check if there is no more of this command
   std::string cmdType = theCmd->GetTypeName();
   
   #if DEBUG_MISSION_TREE_DELETE
   MessageInterface::ShowMessage
      ("MissionTree::OnDelete() calling theGuiInterpreter->DeleteCommand(%s)\n",
       theCmd->GetTypeName().c_str());
   MessageInterface::ShowMessage
      ("   Previous of %s is %s\n", theCmd->GetTypeName().c_str(),
       theCmd->GetPrevious()->GetTypeName().c_str());
   if (theCmd->GetNext() == NULL)
      MessageInterface::ShowMessage
         ("   Next of %s is NULL\n", theCmd->GetTypeName().c_str());
   else
      MessageInterface::ShowMessage
         ("   Next of %s is %s\n", theCmd->GetTypeName().c_str(),
          theCmd->GetNext()->GetTypeName().c_str());
   #endif
   
   theGuiInterpreter->DeleteCommand(theCmd);
   delete theCmd;
   
   // reset counter if if there is no more of this command
   std::string seqString = theGuiInterpreter->GetScript();
   if (seqString.find(cmdType) == seqString.npos)
   {
      int *cmdCounter = GetCommandCounter(cmdType.c_str());
      *cmdCounter = 0;
   }
   
   // delete from tree - if parent only has 1 child collapse
   wxTreeItemId parentId = GetItemParent(itemId);
   if (GetChildrenCount(parentId) <= 1)
      this->Collapse(parentId);
   
   this->Delete(itemId);
   
   #if DEBUG_MISSION_TREE_SHOW_CMD
   MessageInterface::ShowMessage("MissionTree::OnDelete() leaving\n");
   ShowCommands("InOnDelete()");
   #endif
}


//---------------------------------------------------------------------------
// void MissionTree::OnRun()
//--------------------------------------------------------------------------
void MissionTree::OnRun(wxCommandEvent &event)
{
   theGuiInterpreter->RunMission();
}


//---------------------------------------------------------------------------
// void MissionTree::OnShowScript()
//--------------------------------------------------------------------------
void MissionTree::OnShowScript(wxCommandEvent &event)
{
   std::string str = theGuiInterpreter->GetScript();

   ViewTextFrame *vtf =
      new ViewTextFrame(GmatAppData::GetMainFrame(), _T("Show Script"),
       50, 50, 800, 500, "Temporary", "Script");
   
   vtf->AppendText(str.c_str());
   vtf->Show(true);
   
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
// void OnCollapse(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * This method collapses MissionTree.
 */
//------------------------------------------------------------------------------
void MissionTree::OnCollapse(wxCommandEvent &event)
{
   // Get selected item
   GmatTreeItemData *currItem = (GmatTreeItemData *) GetItemData(GetSelection());
   wxTreeItemId selectId = currItem->GetId();
   wxTreeItemId currId = currItem->GetId();
   
   int numChildren = GetChildrenCount(currId);
   if (numChildren > 0)
   {
      wxTreeItemIdValue cookie;
      wxTreeItemId childId = GetFirstChild(currId, cookie);
      
      while (childId.IsOk())
      {
         Collapse(childId);
         childId = GetNextChild(currId, cookie);
      }
   }
   
   ScrollTo(selectId);
}


//------------------------------------------------------------------------------
// void OnExpand(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * This method expands MissionTree.
 */
//------------------------------------------------------------------------------
void MissionTree::OnExpand(wxCommandEvent &event)
{
   // Get selected item
   GmatTreeItemData *currItem = (GmatTreeItemData *) GetItemData(GetSelection());
   wxTreeItemId currId = currItem->GetId();
   
   ExpandAll();
   ScrollTo(currId);
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
   GmatTreeItemData *currItem = (GmatTreeItemData *) GetItemData(GetSelection());
   wxTreeItemId currId = currItem->GetId();
   
   int numChildren = GetChildrenCount(currId);
   if (numChildren > 0)
   {
      wxTreeItemIdValue cookie;
      MissionTreeItemData *item;
      wxTreeItemId childId = GetFirstChild(currId, cookie);
      
      while (childId.IsOk())
      {
         item = (MissionTreeItemData *)GetItemData(childId);
         
         #if DEBUG_MISSION_TREE_DELETE
         MessageInterface::ShowMessage
            ("MissionTree::OnClose() while-loop, item->GetDesc(): \"%s\"\n",
             item->GetDesc().c_str());
         #endif
         
         if (GmatAppData::GetMainFrame()->IsChildOpen(item))
            GmatAppData::GetMainFrame()->CloseActiveChild();
         
         childId = GetNextChild(currId, cookie);
         
         #if DEBUG_MISSION_TREE_DELETE
         MessageInterface::ShowMessage
            ("MissionTree::OnClose() childId=<%s>\n", GetItemText(childId).c_str());
         #endif
      }
   }
   
   
   // delete selected item panel, if its open, its activated
   if (GmatAppData::GetMainFrame()->IsChildOpen(currItem))
      GmatAppData::GetMainFrame()->CloseActiveChild();
   else
      return;
}


//------------------------------------------------------------------------------
// int GetMenuId(const wxString &cmd, ActionType action)
//------------------------------------------------------------------------------
int MissionTree::GetMenuId(const wxString &cmd, ActionType action)
{
   #if DEBUG_MISSION_TREE_MENU
   MessageInterface::ShowMessage("MissionTree::GetMenuId() cmd=%s, insert=%d\n",
                                 cmd.c_str(), insert);
   #endif
   
   int id = 0;
   
   for (unsigned int i=0; i<mCommandList.Count(); i++)
   {
      if (action == APPEND)
      {
         if (cmd == "Propagate")
            return POPUP_APPEND_PROPAGATE;
         else if (cmd == "Maneuver")
            return POPUP_APPEND_MANEUVER;
         else if (cmd == "BeginFiniteBurn")
            return POPUP_APPEND_BEGIN_FINITE_BURN;
         else if (cmd == "EndFiniteBurn")
            return POPUP_APPEND_END_FINITE_BURN;
         else if (cmd == "Target")
            return POPUP_APPEND_TARGET;
         else if (cmd == "Optimize")
            return POPUP_APPEND_OPTIMIZE;
         else if (cmd == "CallFunction")
            return POPUP_APPEND_FUNCTION;
         else if (cmd == "GMAT")
            return POPUP_APPEND_ASSIGNMENT;
         else if (cmd == "Report")
            return POPUP_APPEND_REPORT;
         else if (cmd == "Toggle")
            return POPUP_APPEND_TOGGLE;
         else if (cmd == "Save")
            return POPUP_APPEND_SAVE;
         else if (cmd == "Stop")
            return POPUP_APPEND_STOP;
         else if (cmd == "ScriptEvent")
            return POPUP_APPEND_SCRIPT_EVENT;
         else
            MessageInterface::ShowMessage
               ("MissionTree::GetMenuId() Unknown command:%s\n", cmd.c_str());
      }
      else if (action == INSERT_BEFORE)
      {
         if (cmd == "Propagate")
            return POPUP_INSERT_BEFORE_PROPAGATE;
         else if (cmd == "Maneuver")
            return POPUP_INSERT_BEFORE_MANEUVER;
         else if (cmd == "BeginFiniteBurn")
            return POPUP_INSERT_BEFORE_BEGIN_FINITE_BURN;
         else if (cmd == "EndFiniteBurn")
            return POPUP_INSERT_BEFORE_END_FINITE_BURN;
         else if (cmd == "Target")
            return POPUP_INSERT_BEFORE_TARGET;
         else if (cmd == "Optimize")
            return POPUP_INSERT_BEFORE_OPTIMIZE;
         else if (cmd == "CallFunction")
            return POPUP_INSERT_BEFORE_FUNCTION;
         else if (cmd == "GMAT")
            return POPUP_INSERT_BEFORE_ASSIGNMENT;
         else if (cmd == "Report")
            return POPUP_INSERT_BEFORE_REPORT;
         else if (cmd == "Toggle")
            return POPUP_INSERT_BEFORE_TOGGLE;
         else if (cmd == "Save")
            return POPUP_INSERT_BEFORE_SAVE;
         else if (cmd == "Stop")
            return POPUP_INSERT_BEFORE_STOP;
         else if (cmd == "ScriptEvent")
            return POPUP_INSERT_BEFORE_SCRIPT_EVENT;
         else
            MessageInterface::ShowMessage
               ("MissionTree::GetMenuId() Unknown command:%s\n", cmd.c_str());
      }
      else if (action == INSERT_AFTER)
      {
         if (cmd == "Propagate")
            return POPUP_INSERT_AFTER_PROPAGATE;
         else if (cmd == "Maneuver")
            return POPUP_INSERT_AFTER_MANEUVER;
         else if (cmd == "BeginFiniteBurn")
            return POPUP_INSERT_AFTER_BEGIN_FINITE_BURN;
         else if (cmd == "EndFiniteBurn")
            return POPUP_INSERT_AFTER_END_FINITE_BURN;
         else if (cmd == "Target")
            return POPUP_INSERT_AFTER_TARGET;
         else if (cmd == "Optimize")
            return POPUP_INSERT_AFTER_OPTIMIZE;
         else if (cmd == "CallFunction")
            return POPUP_INSERT_AFTER_FUNCTION;
         else if (cmd == "GMAT")
            return POPUP_INSERT_AFTER_ASSIGNMENT;
         else if (cmd == "Report")
            return POPUP_INSERT_AFTER_REPORT;
         else if (cmd == "Toggle")
            return POPUP_INSERT_AFTER_TOGGLE;
         else if (cmd == "Save")
            return POPUP_INSERT_AFTER_SAVE;
         else if (cmd == "Stop")
            return POPUP_INSERT_AFTER_STOP;
         else if (cmd == "ScriptEvent")
            return POPUP_INSERT_AFTER_SCRIPT_EVENT;
         else
            MessageInterface::ShowMessage
               ("MissionTree::GetMenuId() Unknown command:%s\n", cmd.c_str());
      }
   }
   
   return id;
}


//------------------------------------------------------------------------------
// GmatTree::MissionIconType GetIconId(const wxString &cmd)
//------------------------------------------------------------------------------
GmatTree::MissionIconType MissionTree::GetIconId(const wxString &cmd)
{
   if (cmd == "Propagate")
      return GmatTree::MISSION_ICON_PROPAGATE;
   if (cmd == "Maneuver")
      return GmatTree::MISSION_ICON_DELTA_V;
   if (cmd == "BeginFiniteBurn")
      return GmatTree::MISSION_ICON_BEGIN_FB;
   if (cmd == "EndFiniteBurn")
      return GmatTree::MISSION_ICON_END_FB;
   if (cmd == "Target")
      return GmatTree::MISSION_ICON_TARGET;
   if (cmd == "EndTarget")
      return GmatTree::MISSION_ICON_NEST_RETURN;
   if (cmd == "Optimize")
      return GmatTree::MISSION_NO_ICON;
   if (cmd == "EndOptimize")
      return GmatTree::MISSION_ICON_NEST_RETURN;
   if (cmd == "Achieve")
      return GmatTree::MISSION_ICON_ACHIEVE;
   if (cmd == "Minimize")
      return GmatTree::MISSION_NO_ICON;
   if (cmd == "NonlinearConstraint")
      return GmatTree::MISSION_NO_ICON;
   if (cmd == "Vary")
      return GmatTree::MISSION_ICON_VARY;
   if (cmd == "Save")
      return GmatTree::MISSION_ICON_SAVE;
   if (cmd == "GMAT")
      return GmatTree::MISSION_ICON_ASSIGNMENT;
   if (cmd == "Report")
      return GmatTree::MISSION_ICON_REPORT;
   if (cmd == "Toggle")
      return GmatTree::MISSION_ICON_TOGGLE;
   if (cmd == "For")
      return GmatTree::MISSION_ICON_FOR;
   if (cmd == "EndFor")
      return GmatTree::MISSION_ICON_NEST_RETURN;
   if (cmd == "If")
      return GmatTree::MISSION_ICON_IF;
   if (cmd == "IfElse")
      return GmatTree::MISSION_ICON_IF;
   if (cmd == "Else")
      return GmatTree::MISSION_ICON_IF;
   if (cmd == "EndIf")
      return GmatTree::MISSION_ICON_NEST_RETURN;
   if (cmd == "While")
      return GmatTree::MISSION_ICON_WHILE;
   if (cmd == "EndWhile")
      return GmatTree::MISSION_ICON_NEST_RETURN;
   if (cmd == "CallFunction")
      return GmatTree::MISSION_ICON_FUNCTION;
   if (cmd == "Stop")
      return GmatTree::MISSION_ICON_STOP;
   if (cmd == "BeginScript")
      return GmatTree::MISSION_ICON_SCRIPTEVENT;
   
   return GmatTree::MISSION_ICON_FILE;
}


//------------------------------------------------------------------------------
// GmatTree::ItemType GetCommandId(const wxString &cmd)
//------------------------------------------------------------------------------
GmatTree::ItemType MissionTree::GetCommandId(const wxString &cmd)
{
   if (cmd == "Propagate")
      return GmatTree::PROPAGATE;
   if (cmd == "Maneuver")
      return GmatTree::MANEUVER;
   if (cmd == "BeginFiniteBurn")
      return GmatTree::BEGIN_FINITE_BURN;
   if (cmd == "EndFiniteBurn")
      return GmatTree::END_FINITE_BURN;
   if (cmd == "Target")
      return GmatTree::TARGET;
   if (cmd == "EndTarget")
      return GmatTree::END_TARGET;
   if (cmd == "Optimize")
      return GmatTree::OPTIMIZE;
   if (cmd == "EndOptimize")
      return GmatTree::END_OPTIMIZE;
   if (cmd == "Achieve")
      return GmatTree::ACHIEVE;
   if (cmd == "Minimize")
      return GmatTree::MINIMIZE;
   if (cmd == "NonlinearConstraint")
      return GmatTree::NON_LINEAR_CONSTRAINT;
   if (cmd == "Vary")
      return GmatTree::VARY;
   if (cmd == "Save")
      return GmatTree::SAVE;
   if (cmd == "Toggle")
      return GmatTree::TOGGLE;
   if (cmd == "Report")
      return GmatTree::REPORT;
   if (cmd == "For")
      return GmatTree::FOR_CONTROL;
   if (cmd == "EndFor")
      return GmatTree::END_FOR_CONTROL;
   if (cmd == "If")
      return GmatTree::IF_CONTROL;
   if (cmd == "IfElse")
      return GmatTree::IF_CONTROL;
   if (cmd == "Else")
      return GmatTree::ELSE_CONTROL;
   if (cmd == "EndIf")
      return GmatTree::END_IF_CONTROL;
   if (cmd == "While")
      return GmatTree::WHILE_CONTROL;
   if (cmd == "EndWhile")
      return GmatTree::END_WHILE_CONTROL;
   if (cmd == "CallFunction")
      return GmatTree::CALL_FUNCTION;
   if (cmd == "Stop")
      return GmatTree::STOP;
   if (cmd == "GMAT")
      return GmatTree::ASSIGNMENT;
   
   return GmatTree::SCRIPT_EVENT;
}


//------------------------------------------------------------------------------
// int* GetCommandCounter(const wxString &cmd)
//------------------------------------------------------------------------------
int* MissionTree::GetCommandCounter(const wxString &cmd)
{
   if (cmd == "Propagate")
      return &mNumPropagate;
   if (cmd == "Maneuver")
      return &mNumManeuver;
   if (cmd == "BeginFiniteBurn")
      return &mNumFiniteBurn;
   if (cmd == "Target")
      return &mNumTarget;
   if (cmd == "EndTarget")
      return &mNumTarget;
   if (cmd == "Optimize")
      return &mNumOptimize;
   if (cmd == "EndOptimize")
      return &mNumOptimize;
   if (cmd == "Achieve")
      return &mNumAchieve;
   if (cmd == "Vary")
      return &mNumVary;
   if (cmd == "Save")
      return &mNumSave;
   if (cmd == "Toggle")
      return &mNumToggle;
   if (cmd == "Report")
      return &mNumReport;
   if (cmd == "For")
      return &mNumForLoop;
   if (cmd == "EndFor")
      return &mNumForLoop;
   if (cmd == "If")
      return &mNumIfStatement;
   if (cmd == "IfElse")
      return &mNumIfStatement;
   if (cmd == "Else")
      return &mNumIfStatement;
   if (cmd == "EndIf")
      return &mNumIfStatement;
   if (cmd == "While")
      return &mNumWhileLoop;
   if (cmd == "EndWhile")
      return &mNumWhileLoop;
   if (cmd == "CallFunction")
      return &mNumFunct;
   if (cmd == "GMAT")
      return &mNumAssign;
   if (cmd == "Stop")
      return &mNumStop;
   if (cmd == "Minimize")
      return &mNumMinimize;
   if (cmd == "NonlinearConstraint")
      return &mNumNonlinearConstraint;
   if (cmd == "BeginScript")
      return &mNumScriptEvent;
   
   return &mTempCounter;
}


//------------------------------------------------------------------------------
// wxTreeItemId FindChild(wxTreeItemId parentId, const wxString &cmd)
//------------------------------------------------------------------------------
wxTreeItemId MissionTree::FindChild(wxTreeItemId parentId, const wxString &cmd)
{
   #if DEBUG_MISSION_TREE_FIND
   MessageInterface::ShowMessage
      ("\nMissionTree::FindChild() parentId=<%s>, cmd=<%s>\n",
       GetItemText(parentId).c_str(), cmd.c_str());
   #endif
   
   wxTreeItemIdValue cookie;
   MissionTreeItemData *item;
   wxTreeItemId childId = GetFirstChild(parentId, cookie);

   #if DEBUG_MISSION_TREE_FIND
   MessageInterface::ShowMessage 
      ("MissionTree::FindChild() childId=<%s>\n", GetItemText(childId).c_str());
   #endif
   
   while (childId.IsOk())
   {
      item = (MissionTreeItemData *)GetItemData(childId);
       
      #if DEBUG_MISSION_TREE
      MessageInterface::ShowMessage("MissionTree::FindChild() while-loop, "
                    "item->GetDesc(): \"%s\"\n",item->GetDesc().c_str());
      #endif

      if (item->GetDesc().Contains(cmd))
         return childId;
      
      childId = GetNextChild(parentId, cookie);
      
      #if DEBUG_MISSION_TREE_FIND > 1
      MessageInterface::ShowMessage("MissionTree::FindChild() childId=<%s>\n", 
                                    GetItemText(childId).c_str());
      #endif
   }
   
   return childId;

}


// for Debug
//------------------------------------------------------------------------------
// void ShowCommands(const wxString &msg = "")
//------------------------------------------------------------------------------
void MissionTree::ShowCommands(const wxString &msg)
{
   MessageInterface::ShowMessage("-------------------->%s\n", msg.c_str());
   
   GmatCommand *cmd = theGuiInterpreter->GetFirstCommand();;
   
   while (cmd != NULL)
   {
      MessageInterface::ShowMessage
         ("----- (%p) %s\n", cmd, cmd->GetTypeName().c_str());
      
      if ((cmd->GetChildCommand(0)) != NULL)
         ShowSubCommands(cmd, 0);
      
      cmd = cmd->GetNext();
   }
   
   MessageInterface::ShowMessage("<--------------------\n");
}


//------------------------------------------------------------------------------
// void ShowSubCommands(GmatCommand* brCmd, Integer level)
//------------------------------------------------------------------------------
void MissionTree::ShowSubCommands(GmatCommand* brCmd, Integer level)
{
   GmatCommand* current = brCmd;
   Integer childNo = 0;
   GmatCommand* nextInBranch;
   GmatCommand* child;
   
   while((child = current->GetChildCommand(childNo)) != NULL)
   {
      nextInBranch = child;
      while ((nextInBranch != NULL) && (nextInBranch != current))
      {
         for (int i=0; i<=level; i++)
            MessageInterface::ShowMessage("-----");
         
         MessageInterface::ShowMessage
            ("----- (%p) %s\n", nextInBranch, nextInBranch->GetTypeName().c_str());
         
         if (nextInBranch->GetChildCommand() != NULL)
            ShowSubCommands(nextInBranch, level+1);
         
         nextInBranch = nextInBranch->GetNext();
      }
      
      ++childNo;
   }
   
}
