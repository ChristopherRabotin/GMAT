//$Id$
//------------------------------------------------------------------------------
//                              MissionTree
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Allison Greene
// Created: 2003/09/02
/**
 * This class provides the tree for missions.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "bitmaps/folder.xpm"
#include "bitmaps/OpenFolder.xpm"
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
#include "bitmaps/penup.xpm"
#include "bitmaps/pendown.xpm"
#include "bitmaps/optimize.xpm"
#include "bitmaps/mt_Default.xpm"
#include "bitmaps/mt_Stop.xpm"
#include "bitmaps/mt_MarkPoint.xpm"
#include "bitmaps/mt_ClearPlot.xpm"
#include "bitmaps/mt_Global.xpm"
#include "bitmaps/mt_SaveMission.xpm"
#include "bitmaps/mt_Minimize.xpm"
#include "bitmaps/mt_NonlinearConstraint.xpm"
#include "bitmaps/mt_RunSimulator.xpm"
#include "bitmaps/mt_RunEstimator.xpm"

#include <wx/tipwin.h>

#include "MissionTree.hpp"
#include "MissionTreeItemData.hpp"
#include "ViewTextFrame.hpp"

#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "GmatNotebook.hpp"
#include "MessageInterface.hpp"
#include "CommandUtil.hpp"         // for GetNextCommand()
#include "StringUtil.hpp"          // for GmatStringUtil::

// This will automatically add all viewable commands including plugin commands.
// It uses new array called viewables in the CommandFactory.
// If plugin commands need to be shown in the popup menu, just add them to
// viewables in a plugin factory constructors(LOJ: 2010.07.13)
#define __AUTO_ADD_NEW_COMMANDS__

// Should we sort the command list?
#define __SORT_COMMAND_LIST__

// Should we enable multiple mission sequence?
//#define__ENABLE_MULTIPLE_SEQUENCE__

// If actions to be saved and playback for testing
// Define this by setting the compiler parameter -D
//#define __TEST_MISSION_TREE_ACTIONS__

#ifdef __TEST_MISSION_TREE_ACTIONS__
#include <fstream>
#include "StringTokenizer.hpp"
//#define DEBUG_MISSION_TREE_ACTIONS
#endif

// For debug output
//#define DEBUG_MISSION_TREE_SHOW_CMD 1
//#define DEBUG_MISSION_TREE_APPEND 1
//#define DEBUG_MISSION_TREE_INSERT 1
//#define DEBUG_MISSION_TREE_DELETE 1
//#define DEBUG_MISSION_TREE_CHILD 1
//#define DEBUG_MISSION_TREE_FIND 2
//#define DEBUG_FIND_ITEM_PARENT 2
//#define DEBUG_MISSION_TREE_MENU 1
//#define DEBUG_MISSION_TREE 1
//#define DEBUG_ADD_ICONS
//#define DEBUG_BUILD_TREE_ITEM 1

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
   
   // wxMouseEvent event
   EVT_LEFT_DCLICK(MissionTree::OnDoubleClick)
   
   // wxTreeEvent
   EVT_TREE_ITEM_RIGHT_CLICK(-1, MissionTree::OnItemRightClick)
   EVT_TREE_ITEM_ACTIVATED(-1, MissionTree::OnItemActivated)
   EVT_TREE_BEGIN_LABEL_EDIT(-1, MissionTree::OnBeginEditLabel)
   EVT_TREE_END_LABEL_EDIT(-1, MissionTree::OnEndEditLabel)
   
   // wxCommandEvent
   EVT_MENU(POPUP_OPEN, MissionTree::OnOpen)
   EVT_MENU(POPUP_CLOSE, MissionTree::OnClose)
   
   EVT_MENU(POPUP_ADD_MISSION_SEQ, MissionTree::OnAddMissionSeq)
   
   EVT_MENU_RANGE(POPUP_APPEND_PROPAGATE, POPUP_APPEND_SWITCH,
                  MissionTree::OnAppend)
   EVT_MENU_RANGE(POPUP_INSERT_BEFORE_PROPAGATE, POPUP_INSERT_BEFORE_SWITCH,
                  MissionTree::OnInsertBefore)
   EVT_MENU_RANGE(POPUP_INSERT_AFTER_PROPAGATE, POPUP_INSERT_AFTER_SWITCH,
                  MissionTree::OnInsertAfter)
   
   EVT_MENU_RANGE(AUTO_APPEND_COMMAND, AUTO_INSERT_BEFORE_COMMAND - 1,
                  MissionTree::OnAutoAppend)
   EVT_MENU_RANGE(AUTO_INSERT_BEFORE_COMMAND, AUTO_INSERT_AFTER_COMMAND - 1,
                  MissionTree::OnAutoInsertBefore)
   EVT_MENU_RANGE(AUTO_INSERT_AFTER_COMMAND, AUTO_END - 1,
                  MissionTree::OnAutoInsertAfter)
   
   EVT_MENU(POPUP_COLLAPSE, MissionTree::OnCollapse)
   EVT_MENU(POPUP_EXPAND, MissionTree::OnExpand)
   
   EVT_MENU(POPUP_RUN, MissionTree::OnRun)
   
   EVT_MENU(POPUP_RENAME, MissionTree::OnRename)
   EVT_MENU(POPUP_DELETE, MissionTree::OnDelete)
   
   EVT_MENU(POPUP_SHOW_DETAIL, MissionTree::OnShowDetail)
   EVT_MENU(POPUP_SHOW_SCRIPT, MissionTree::OnShowScript)
   
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   EVT_MENU(POPUP_START_SAVE_ACTIONS, MissionTree::OnStartSaveActions)
   EVT_MENU(POPUP_STOP_SAVE_ACTIONS, MissionTree::OnStopSaveActions)
   EVT_MENU(POPUP_READ_ACTIONS, MissionTree::OnPlaybackActions)
   #endif
   
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
     inFiniteBurn(false),
     mShowDetailedItem(false)
{
   mParent = parent;   
   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   mViewCommands.Add("All");
   mViewAll = true;
   mUsingViewLevel = true;
   mViewLevel = 10;
   
   //-----------------------------------------------------------------
   #ifdef __AUTO_ADD_NEW_COMMANDS__
   //-----------------------------------------------------------------
   
   StringArray cmds = theGuiInterpreter->GetListOfViewableCommands();
   #ifdef DEBUG_COMMAND_LIST
   GmatStringUtil::WriteStringArray
      (cmds, "===> Here is the viewable command list", "   ");
   #endif
   for (unsigned int i = 0; i < cmds.size(); i++)
      mCommandList.Add(cmds[i].c_str());
   
   CreateCommandIdMap();
   
   //-----------------------------------------------------------------
   #else
   //-----------------------------------------------------------------
   
   mCommandList.Clear();
   mCommandList.Add("Propagate");
   mCommandList.Add("Maneuver");
   mCommandList.Add("BeginFiniteBurn");
   mCommandList.Add("EndFiniteBurn");
   mCommandList.Add("Target");
   mCommandList.Add("Optimize");
   mCommandList.Add("CallGmatFunction");
   if (GmatGlobal::Instance()->IsMatlabAvailable())
      mCommandList.Add("CallMatlabFunction");
   mCommandList.Add("Report");
   mCommandList.Add("Toggle");
   mCommandList.Add("Save");
   mCommandList.Add("Stop");
   //mCommandList.Add("GMAT");
   mCommandList.Add("Equation");
   mCommandList.Add("ScriptEvent");
   
   //-----------------------------------------------------------------
   #endif
   //-----------------------------------------------------------------
   
   // Build commands for view control since MissionTree show ControlFlow commands
   // and Vary, Achieve in sub nodes such as ControlLogic and Target node.
   mCommandListForViewControl = mCommandList;
   mCommandListForViewControl.Add("For");
   mCommandListForViewControl.Add("If");
   mCommandListForViewControl.Add("While");
   mCommandListForViewControl.Add("Achieve");
   mCommandListForViewControl.Add("Vary");
   mCommandListForViewControl.Add("CallMatlabFunction");
   
   // Should we sort the command list?
   #ifdef __SORT_COMMAND_LIST__
   mCommandList.Sort();
   #endif
   
   SetParameter(BOXCOUNT, 0);
   SetParameter(DRAWOUTLINE, 0);
   
   InitializeCounter();
   AddIcons();
   
   // Now this is called from GmatNotebook after MissionTreeToolBar is created
   //AddDefaultMission();
   
   // for auto-testing of MissionTree actions
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   mSaveActions = false;
   mPlaybackActions = false;
   mActionsOutFile = "MissionTreeActionsOut.txt";
   mResultsFile = "MissionTreeResults.txt";
   #endif
}


//------------------------------------------------------------------------------
// void SetMainFrame(GmatMainFrame *gmf)
//------------------------------------------------------------------------------
void MissionTree::SetMainFrame(GmatMainFrame *gmf)
{
   theMainFrame = gmf;
}


//------------------------------------------------------------------------------
// void SetNotebook(GmatNotebook *notebook)
//------------------------------------------------------------------------------
void MissionTree::SetNotebook(GmatNotebook *notebook)
{
   theNotebook = notebook;
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
      Collapse(mMissionSeqSubId);
   #endif

   wxString itemText = GetItemText(mMissionSeqSubId);
   if (itemText.Find("...") != wxNOT_FOUND)
   {
      itemText.Replace("...", "");
      SetItemText(mMissionSeqSubId, itemText);
   }
   
   DeleteChildren(mMissionSeqSubId);
   
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   if (mActionsOutStream.is_open())
      mActionsOutStream.close();
   if (mResultsStream.is_open())
      mResultsStream.close();
   if (mPlaybackResultsStream.is_open())
      mPlaybackResultsStream.close();
   #endif
}


//------------------------------------------------------------------------------
// void UpdateMission(bool resetCounter, bool viewAll = true, bool collapse = false)
//------------------------------------------------------------------------------
/**
 * Updates Mission Sequence
 */
//------------------------------------------------------------------------------
void MissionTree::UpdateMission(bool resetCounter, bool viewAll, bool collapse)
{
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::UpdateMission() entered, resetCounter=%d, viewAll=%d, "
       "collapse=%d, mUsingViewLevel=%d\n", resetCounter, viewAll, collapse,
       mUsingViewLevel);
   #endif
   
   if (resetCounter)
      InitializeCounter();
   
   if (mUsingViewLevel)
      mViewAll = viewAll;
   
   ClearMission();
   UpdateCommand();
   
   if (collapse)
   {
      CollapseAllChildren(mMissionSeqSubId);
      Expand(mMissionSeqSubId);
   }
}


//------------------------------------------------------------------------------
// void UpdateMissionForRename()
//------------------------------------------------------------------------------
/**
 * Updates MissionTree nodes if show detail is turned on since it needs to show
 * new resource name on the nodes.
 */
//------------------------------------------------------------------------------
void MissionTree::UpdateMissionForRename()
{
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::UpdateMissionForRename() entered\n");
   #endif
   
   if (mShowDetailedItem)
   {
      ClearMission();
      UpdateCommand();
   }
}


//------------------------------------------------------------------------------
// void ChangeNodeLabel(const wxString &oldLabel)
//------------------------------------------------------------------------------
/*
 * Sets tree node label to new label if it is different from oldLabel.
 */
//------------------------------------------------------------------------------
void MissionTree::ChangeNodeLabel(const wxString &oldLabel)
{
   #ifdef DEBUG_CHANGE_NODE_LABEL
   MessageInterface::ShowMessage
      ("MissionTree::ChangeNodeLabel() oldLabel=<%s>\n", oldLabel.c_str());
   #endif
   
   wxTreeItemId itemId = FindChild(mMissionSeqSubId, oldLabel);
   if (itemId.IsOk())
   {
      MissionTreeItemData *item = (MissionTreeItemData *)GetItemData(itemId);
      GmatCommand *cmd = item->GetCommand();
      wxString newLabel = GetCommandString(cmd, oldLabel);
      
      #ifdef DEBUG_CHANGE_NODE_LABEL
      MessageInterface::ShowMessage("   newLabel=<%s>\n", newLabel.c_str());
      #endif
      
      if (newLabel != oldLabel)
      {
         item->SetName(newLabel);
         item->SetTitle(newLabel);
         SetItemText(itemId, newLabel);
      }
   }
   else
   {
      #ifdef DEBUG_CHANGE_NODE_LABEL
      MessageInterface::ShowMessage("===> <%s> not found\n", oldLabel.c_str());
      #endif
   }
}


//------------------------------------------------------------------------------
// void SetViewAll(bool viewAll)
//------------------------------------------------------------------------------
void MissionTree::SetViewAll(bool viewAll)
{
   mViewAll = viewAll;
}


//------------------------------------------------------------------------------
// void SetViewLevel(int level)
//------------------------------------------------------------------------------
void MissionTree::SetViewLevel(int level)
{
   mUsingViewLevel = true;
   mViewLevel = level;
   if (level == 0) // Set level to 10 for showing all levels
      mViewLevel = 10;
   
   UpdateMission(true, false, false);
   
}


//------------------------------------------------------------------------------
// void SetViewCommands(const wxArrayString &viewCmds)
//------------------------------------------------------------------------------
void MissionTree::SetViewCommands(const wxArrayString &viewCmds)
{
   mViewCommands = viewCmds;
   
   #ifdef DEBUG_VIEW_COMMANDS
   MessageInterface::ShowMessage("MissionTree::SetViewOption() entered\n");
   MessageInterface::ShowMessage
      ("mCommandListForViewControl has %d commands\n", mViewCommands.GetCount());
   for (unsigned int i = 0; i < mCommandListForViewControl.size(); i++)
      MessageInterface::ShowMessage("   '%s'\n", mCommandListForViewControl[i].c_str());
   MessageInterface::ShowMessage
      ("mViewCommands has %d commands\n", mViewCommands.GetCount());
   for (unsigned int i = 0; i < mViewCommands.size(); i++)
      MessageInterface::ShowMessage("   '%s'\n", mViewCommands[i].c_str());
   #endif
   
   mUsingViewLevel = false;
   mViewAll = false;
   if (mViewCommands.GetCount() == 1 && mViewCommands[0] == "All")
      mViewAll = true;
   
   UpdateMission(true, mViewAll, false);
}


//------------------------------------------------------------------------------
// const wxArrayString& GetCommandList(bool forViewControl = false)
//------------------------------------------------------------------------------
const wxArrayString& MissionTree::GetCommandList(bool forViewControl)
{
   if (forViewControl)
      return mCommandListForViewControl;
   else
      return mCommandList;
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
   
   inScriptEvent = false;
   inFiniteBurn = false;
}


//------------------------------------------------------------------------------
// GmatCommand* CreateCommand(const wxString &cmdName)
//------------------------------------------------------------------------------
GmatCommand* MissionTree::CreateCommand(const wxString &cmdName)
{
   #ifdef DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("MissionTree::CreateCommand() entered, cmdName='%s'\n", cmdName.c_str());
   #endif
   
   GmatCommand *cmd = NULL;
   try
   {
      if (cmdName == "IfElse")
         cmd = theGuiInterpreter->CreateDefaultCommand("If");
      else if (cmdName == "Equation")
         cmd = theGuiInterpreter->CreateDefaultCommand("GMAT");
      else
         cmd = theGuiInterpreter->CreateDefaultCommand(cmdName.c_str());
   }
   catch (BaseException &be)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, be.GetFullMessage());
   }
   
   #ifdef DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("MissionTree::CreateCommand() returning <%p>\n", cmd);
   #endif
   
   return cmd;
}


//------------------------------------------------------------------------------
// bool IsAnyViewCommandInBranch(GmatCommand *branch)
//------------------------------------------------------------------------------
/**
 * Returns true if any view command found in the branch command
 */
//------------------------------------------------------------------------------
bool MissionTree::IsAnyViewCommandInBranch(GmatCommand *branch)
{
   Integer childNo = 0;
   GmatCommand* nextInBranch;
   GmatCommand* child;
   wxString typeName;
   wxString branchTypeName = branch->GetTypeName();
   
   while((child = branch->GetChildCommand(childNo)) != NULL)
   {
      nextInBranch = child;
      while ((nextInBranch != NULL) && (nextInBranch != branch))
      {
         #if DEBUG_VIEW_COMMANDS
         for (int i=0; i<=level; i++)
            MessageInterface::ShowMessage("-----");
         MessageInterface::ShowMessage
            ("----- (%p)%s\n", nextInBranch, nextInBranch->GetTypeName().c_str());
         #endif
         
         wxString typeName = (nextInBranch->GetTypeName()).c_str();
         
         if (mViewCommands.Index(typeName) != wxNOT_FOUND)
         {
            #if DEBUG_VIEW_COMMANDS
            MessageInterface::ShowMessage
               ("   Found '%s' in '%s'\n", typeName.c_str(), branchTypeName.c_str());
            #endif
            return true;
         }
         
         if (nextInBranch->GetChildCommand() != NULL)
            IsAnyViewCommandInBranch(nextInBranch);
         
         nextInBranch = nextInBranch->GetNext();
      }
      
      ++childNo;
   }
   
   return false;
}


//------------------------------------------------------------------------------
// void ShowEllipsisInPreviousNode(wxTreeItemId parent, wxTreeItemId node)
//------------------------------------------------------------------------------
void MissionTree::ShowEllipsisInPreviousNode(wxTreeItemId parent, wxTreeItemId node)
{
   wxString itemText;
   wxTreeItemId prevId = GetPrevVisible(node);
   if (prevId.IsOk())
   {
      itemText = GetItemText(prevId);
      if (itemText.Find("...") == wxNOT_FOUND)
      {
         itemText = itemText + "...";
         SetItemText(prevId, itemText);
      }
   }
   else
   {
      itemText = GetItemText(parent);
      if (itemText.Find("...") == wxNOT_FOUND)
      {
         itemText = itemText + "...";
         SetItemText(parent, itemText);
      }
   }
   #if DEBUG_BUILD_TREE_ITEM
   MessageInterface::ShowMessage
      ("   previous item = '%s'\n", itemText.c_str());
   #endif
}


//------------------------------------------------------------------------------
// wxTreeItemId BuildTreeItem(wxTreeItemId parent, GmatCommand *cmd, ...)
//------------------------------------------------------------------------------
wxTreeItemId MissionTree::BuildTreeItem(wxTreeItemId parent, GmatCommand *cmd,
                                        Integer level, bool &isLastItemHidden)
{
   wxString typeName = cmd->GetTypeName().c_str();
   wxTreeItemId node;
   
   #if DEBUG_BUILD_TREE_ITEM
   MessageInterface::ShowMessage
      ("MissionTree::BuildTreeItem() entered, inScriptEvent=%d, typeName='%s', mViewAll=%d\n",
       inScriptEvent, typeName.c_str(), mViewAll);
   #endif
   
   // if typeName not found in the view list and not showing all
   if (mViewCommands.Index(typeName) == wxNOT_FOUND && !mViewAll && !mUsingViewLevel)
   {
      if (cmd->GetTypeName() == "BeginScript")
         mScriptEventCount++;
      
      if (cmd->GetTypeName() == "EndScript")
         mScriptEventCount--;
      
      inScriptEvent = (mScriptEventCount == 0) ? false : true;
      
      bool viewCmdFoundInBranch = false;
      if (cmd->IsOfType("BranchCommand"))
      {
         if (IsAnyViewCommandInBranch(cmd))
            viewCmdFoundInBranch = true;
      }
      
      if (!viewCmdFoundInBranch)
      {
         isLastItemHidden = true;
         #if DEBUG_BUILD_TREE_ITEM
         MessageInterface::ShowMessage
            ("MissionTree::BuildTreeItem() returning '%s' node, hiding the node\n",
             node.IsOk() ? "good" : "bad");
         #endif
         return node;
      }
   }
   
   node = UpdateCommandTree(parent, cmd, level);
   
   // If it is not a branch end, then show ellipsis
   if (isLastItemHidden && !cmd->IsOfType("BranchEnd"))
      ShowEllipsisInPreviousNode(parent, node);
   
   isLastItemHidden = false;
   
   if (cmd->GetTypeName() == "BeginScript")
      mScriptEventCount++;
   
   if (cmd->GetTypeName() == "EndScript")
      mScriptEventCount--;
   
   inScriptEvent = (mScriptEventCount == 0) ? false : true;
   
   #if DEBUG_BUILD_TREE_ITEM
   MessageInterface::ShowMessage
      ("MissionTree::BuildTreeItem() returning '%s' node, showing the node\n",
       node.IsOk() ? "good" : "bad");
   #endif
   
   return node;
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
      (MissionTreeItemData *)GetItemData(mMissionSeqSubId);
   wxTreeItemId node;
   
   if (cmd->GetTypeName() == "NoOp")
      seqItemData->SetCommand(cmd);
   
   wxString typeName;
   bool isLastItemHidden = false;
   
   while (cmd != NULL)
   {
      node = BuildTreeItem(mMissionSeqSubId, cmd, 0, isLastItemHidden);
      
      if (isLastItemHidden)
      {
         cmd = cmd->GetNext();
         continue;
      }
      
      child = cmd->GetChildCommand(0);
      
      if (child != NULL)
      {
         ExpandChildCommand(node, cmd, 0);
      }
      
      cmd = cmd->GetNext();
   }
   
   Expand(mMissionSeqSubId);   
   ScrollTo(mMissionSeqSubId);
   
}


//------------------------------------------------------------------------------
// wxTreeItemId& UpdateCommandTree(wxTreeItemId parent, GmatCommand *cmd, ...)
//------------------------------------------------------------------------------
/**
 * Updates commands in the mission sequence
 */
//------------------------------------------------------------------------------
wxTreeItemId& MissionTree::UpdateCommandTree(wxTreeItemId parent,
                                             GmatCommand *cmd, Integer level)
{
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::UpdateCommandTree() entered, inScriptEvent=%d, cmd=<%p><%s>\n",
       inScriptEvent, cmd, cmd->GetTypeName().c_str());
   #endif
   
   wxString cmdTypeName = cmd->GetTypeName().c_str();
   wxTreeItemId currId;
   mNewTreeId = currId;
   
   // If ScriptEvent mode or command is NoOp or BeginMissionSequence, don't add it 
   // This is different from command unviewable list
   if (inScriptEvent || cmdTypeName == "NoOp" || cmdTypeName == "BeginMissionSequence")
   {
      #if DEBUG_MISSION_TREE
      MessageInterface::ShowMessage
         ("MissionTree::UpdateCommandTree() leaving, command '%s' ignored\n",
          cmdTypeName.c_str());
      #endif
      return mNewTreeId;
   }
   
   mNewTreeId = AppendCommand(parent, GetIconId(cmdTypeName),
                              GetCommandId(cmdTypeName), cmd,
                              GetCommandCounter(cmdTypeName),
                              *GetCommandCounter(cmdTypeName));
   
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("   mUsingViewLevel=%d, mViewLevel=%d, level=%d\n", mUsingViewLevel,
       mViewLevel, level);
   #endif
   
   if (mUsingViewLevel)
   {
      if (mViewLevel > level + 1)
         Expand(parent);
   }
   else
      Expand(parent);
   
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::UpdateCommandTree() leaving, command '%s' added to tree\n",
       cmdTypeName.c_str());
   #endif
   
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
   
   wxTreeItemId branchNode;
   wxTreeItemId node;
   Integer childNo = 0;
   GmatCommand* nextInBranch;
   GmatCommand* child;
   wxString typeName;
   bool isLastItemHidden = false;
   
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
         
         node = BuildTreeItem(parent, nextInBranch, level, isLastItemHidden);
         
         if (isLastItemHidden)
         {
            // If it is not a branch end, then show ellipsis
            if (!nextInBranch->IsOfType("BranchEnd"))
               ShowEllipsisInPreviousNode(parent, node);
            
            nextInBranch = nextInBranch->GetNext();
            continue;
         }
         
         if (nextInBranch->GetChildCommand() != NULL)
            ExpandChildCommand(node, nextInBranch, level+1);
         
         nextInBranch = nextInBranch->GetNext();
      }
      
      ++childNo;
   }
}


//------------------------------------------------------------------------------
// wxTreeItemId AppendCommand(wxTreeItemId parent, GmatTree::MissionIconType icon,
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
   
   // Show "ScriptEvent" instead of "BeginScript" to be more clear for user
   if (nodeName.Contains("BeginScript"))
      nodeName.Replace("BeginScript", "ScriptEvent");
   
   // Show "Equation" instead of "GMAT" to be more clear for user
   if (nodeName.Contains("GMAT"))
      nodeName.Replace("GMAT", "Equation");
   
   // Show command string as node label(loj: 2007.11.13)
   nodeName = GetCommandString(cmd, nodeName);
   
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
 * @param  insertBefore  true if inserting before the current item,
 *                       false if inserterting after current item
 */
//------------------------------------------------------------------------------
wxTreeItemId
MissionTree::InsertCommand(wxTreeItemId parentId, wxTreeItemId currId,
                           wxTreeItemId prevId, GmatTree::MissionIconType icon,
                           GmatTree::ItemType type, const wxString &cmdName,
                           GmatCommand *prevCmd, GmatCommand *cmd, int *cmdCount,
                           bool insertBefore)
{
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage
      ("MissionTree::InsertCommand() parentId=%s, currId=%s, prevId=%s, "
       "insertBefore=%d\n", GetItemText(parentId).c_str(), GetItemText(currId).c_str(),
       GetItemText(prevId).c_str(), insertBefore);
   #endif
   
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
   
   // Show "Equation" instead of "GMAT" to be more clear for user
   if (currTypeName == "GMAT")
      currTypeName = "Equation";
   if (cmdTypeName == "GMAT")
      cmdTypeName = "Equation";
   if (prevTypeName == "GMAT")
      prevTypeName = "Equation";
   
   // Show "ScriptEvent" instead of "BeginScript" to be more clear for user
   if (currTypeName == "BeginScript")
      currTypeName = "ScriptEvent";
   if (cmdTypeName == "BeginScript")
      cmdTypeName = "ScriptEvent";
   if (prevTypeName == "BeginScript")
      prevTypeName = "ScriptEvent";
   
   #if DEBUG_MISSION_TREE_INSERT
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
   // on the tree.
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
       cmdTypeName == "ScriptEvent" || cmdTypeName == "Optimize")
   {
      #if DEBUG_MISSION_TREE_INSERT
      MessageInterface::ShowMessage
         ("   Creating End* for '%s'\n", cmdTypeName.c_str());
      #endif
      
      if (cmdTypeName == "Target")
      {
         endCmd = CreateCommand("EndTarget");
         endType = GmatTree::END_TARGET;
      }
      else if (cmdTypeName == "For")
      {
         endCmd = CreateCommand("EndFor");
         endType = GmatTree::END_FOR_CONTROL;
      }
      else if (cmdTypeName == "While")
      {
         endCmd = CreateCommand("EndWhile");
         endType = GmatTree::END_WHILE_CONTROL;
      }
      else if (cmdTypeName == "If")
      {
         endCmd = CreateCommand("EndIf");
         endType = GmatTree::END_IF_CONTROL;
      }
      else if (cmdTypeName == "ScriptEvent")
      {
         endCmd = CreateCommand("EndScript");
         endType = GmatTree::END_SCRIPT_EVENT;
      }
      else if (cmdTypeName == "Optimize")
      {
         endCmd = CreateCommand("EndOptimize");
         endType = GmatTree::END_OPTIMIZE;
      }
      
      #if DEBUG_MISSION_TREE_INSERT
      if (endCmd != NULL)
         MessageInterface::ShowMessage
            ("   '%s' created\n", endCmd->GetTypeName().c_str());
      #endif
      
      // create Else (temp code)
      if (cmdName == "IfElse")
      {
         elseCmd = CreateCommand("Else");
         cmd->Append(elseCmd);
      }
      
      #if DEBUG_MISSION_TREE_INSERT
      MessageInterface::ShowMessage
         ("   ==> Calling cmd->Append(%s)\n", endCmd->GetTypeName().c_str());
      #endif
      
      cmdAdded = cmd->Append(endCmd);
      
      #if DEBUG_MISSION_TREE_INSERT
      WriteCommand("   ", "previous of ", endCmd , " is ", endCmd->GetPrevious());
      #endif
      
   }
   
   //------------------------------------------------------------
   // Compose node name
   //------------------------------------------------------------
   if (cmdTypeName == "Else")
      nodeName.Printf("%s%d", cmdTypeName.c_str(), (*cmdCount));
   
   // if command has name or command has the type name append counter
   if (nodeName.Trim() == "" || nodeName == cmdTypeName)
      nodeName.Printf("%s%d", cmdTypeName.c_str(), ++(*cmdCount));
   
   // Show command string as node label(loj: 2007.11.13)
   nodeName = GetCommandString(cmd, nodeName);
   
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage
      ("   cmd=%s, nodeName=%s, cmdCount=%d\n", cmdTypeName.c_str(),
       nodeName.c_str(), *cmdCount);
   #endif
   
   //------------------------------------------------------------
   // Insert command to mission sequence
   //------------------------------------------------------------
   
   #if DEBUG_MISSION_TREE_INSERT
   WriteCommand("   ==> before appending/inserting: ", "previous of ", cmd, " is ",
                cmd->GetPrevious());
   #endif
   
   if (currCmd->GetTypeName() == "NoOp" ||
       currCmd->GetTypeName() == "BeginMissionSequence")
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
   
   //------------------------------------------------------------
   // We need to set real previous command after command is
   // appended/inserted, since cmd->AppendCommand() or
   // cmd->InsertCommand() resets previous command.
   // So when ScriptEvent is modified, the old ScriptEvent is
   // deleted and new one can be insterted into correct place.
   //------------------------------------------------------------
   #if DEBUG_MISSION_TREE_INSERT
   WriteCommand("   ==>", " Resetting previous of ", cmd, "to ", prevCmd);
   #endif
   
   cmd->ForceSetPrevious(prevCmd);
   
   #if DEBUG_MISSION_TREE_INSERT
   WriteCommand("   ==> after  appending/inserting: ", "previous of ", cmd, " is ",
                cmd->GetPrevious());
   #endif
   
   //MessageInterface::ShowMessage("   ==> cmdAdded = %d\n", cmdAdded);   
   // Why returning false eventhough it inserted?
   cmdAdded = true; 
   
   //------------------------------------------------------------
   // Insert command to mission tree
   //------------------------------------------------------------
   if (cmdAdded)
   {
      if (currCmd->GetTypeName() == "NoOp" ||
          currCmd->GetTypeName() == "BeginMissionSequence")
      {
         node = AppendItem(currId, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, type, nodeName, cmd));
      }
      else if (prevTypeName == "NoOp"     || prevTypeName == "BeginMissionSequence" ||
               prevTypeName == "Target"   || prevTypeName == "For"    ||
               prevTypeName == "While"    || prevTypeName == "If"     ||
               prevTypeName == "Optimize" || prevTypeName == "Else")
      {
         wxTreeItemId realParentId = parentId;
         
         if (insertBefore)
            realParentId = prevId;
         
         #if DEBUG_MISSION_TREE_INSERT
         MessageInterface::ShowMessage
            ("   previous type is NoOp, BranchCommand or Else\n");
         MessageInterface::ShowMessage
            ("   ==> inserting %s after %s\n", nodeName.c_str(),
             GetItemText(parentId).c_str());
         #endif
         
         node = InsertItem(realParentId, 0, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, type, nodeName, cmd));
         
      }
      else if (prevTypeName.Contains("End") && prevTypeName != "EndScript" &&
               prevTypeName != "EndFiniteBurn")
      {
         // Since EndScript and EndFiniteBurn is not part of branch command
         // handle separately here
         
         wxTreeItemId realPrevId = GetItemParent(prevId);
         
         #if DEBUG_MISSION_TREE_INSERT
         MessageInterface::ShowMessage("   previous type contains End\n");
         MessageInterface::ShowMessage
            ("   ==> inserting %s after %s->%s\n", nodeName.c_str(),
             GetItemText(parentId).c_str(), GetItemText(realPrevId).c_str());
         #endif
         
         node = InsertItem(parentId, realPrevId, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, type, nodeName, cmd));
      }
      else
      {
         wxTreeItemId realParentId = parentId;
         wxTreeItemId realPrevId = prevId;
         
         if (insertBefore)
         {
            if (GetChildrenCount(GetItemParent(prevId)) > 0)
            {
               realParentId = GetItemParent(prevId);
            }
            else
            {
               realParentId = prevId;
               realPrevId = GetLastChild(prevId);
            }
         }
         
         
         #if DEBUG_MISSION_TREE_INSERT
         MessageInterface::ShowMessage
            ("   ==> inserting %s after %s->%s\n", nodeName.c_str(),
             GetItemText(realParentId).c_str(), GetItemText(realPrevId).c_str());
         #endif
         
         node = InsertItem(realParentId, realPrevId, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, type, nodeName, cmd));
      }
      
      //---------------------------------------------------------
      // Append End* command
      //---------------------------------------------------------
      if (cmdTypeName == "Target" || cmdTypeName == "For"  ||
          cmdTypeName == "While"  || cmdTypeName == "If"   ||
          cmdTypeName == "Optimize")
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
            
            wxString endName = "End" + cmdTypeName;
            wxString tmpName;
            tmpName.Printf("%s%d", endName.c_str(), *cmdCount);
            InsertItem(node, elseNode, tmpName, GmatTree::MISSION_ICON_NEST_RETURN, -1,
                       new MissionTreeItemData(tmpName, endType, tmpName, endCmd));
         }
         else
         {
            wxString endName = "End" + cmdTypeName;
            wxString tmpName;
            tmpName.Printf("%s%d", endName.c_str(), *cmdCount);
            InsertItem(node, 0, tmpName, GmatTree::MISSION_ICON_NEST_RETURN, -1,
                       new MissionTreeItemData(tmpName, endType, tmpName, endCmd));
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
/*
 * Appends a command to the end of the branch identified by parent.
 * The parent is the current selection, assuming Append menu item only appears
 * on branch command, such as Target, If, For, Optimize.
 * It sets parent item id, current item id, and previous item id and pass them
 * to InsertCommand() with insertBefore flag to true.
 */
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
   wxString itemText = GetItemText(itemId);
   bool elseFound = false;
   
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   if (mSaveActions)
   {
      wxString str;
      str.Printf("Append %s to %s\n", cmdName.c_str(), itemText.c_str());
      WriteActions(str);
   }
   #endif
   
   #if DEBUG_MISSION_TREE_APPEND
   MessageInterface::ShowMessage
      ("\nMissionTree::Append() cmdName=%s, itemId=%s, lastId=%s\n",
       cmdName.c_str(), GetItemText(itemId).c_str(), GetItemText(lastId).c_str());
   #endif
   
   // Append command before Else, if there is Else and current selection is If
   if (cmdName != "Else" && itemText.Contains("If"))
   {
      wxTreeItemId elseId = FindChild(itemId, "Else");
      if (elseId.IsOk() && GetItemText(elseId) != "")
      {
         lastId = elseId;
         elseFound = true;
      }
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
   
   wxTreeItemId prevId;
   
   if (lastId.IsOk() && GetItemText(lastId) != "")
      prevId = GetPrevVisible(lastId);
   else
      prevId = currId;
   
   #if DEBUG_MISSION_TREE_APPEND
   MessageInterface::ShowMessage("   prevId=%s\n", GetItemText(prevId).c_str());
   #endif
   
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);
   GmatCommand *currCmd = currItem->GetCommand();
   GmatCommand *prevCmd = prevItem->GetCommand();
   GmatCommand *cmd = NULL;
   
   #if DEBUG_MISSION_TREE_APPEND
   WriteCommand("   ", "currCmd = ", currCmd, ", prevCmd = ", prevCmd);
   #endif
   
   // For BranchCommand, use GmatCommandUtil::GetMatchingEnd() to get
   // previous command, since there may be commands not shown on the tree.
   // Note: Make sure this works on other than Windows also (loj: 12/15/06)
   if (currCmd->IsOfType("BranchCommand") && !elseFound)
   {
      parentId = itemId;
      currId = GetLastChild(itemId);
      
      GmatCommand *end = GmatCommandUtil::GetMatchingEnd(currCmd);
      GmatCommand *realPrevCmd = end->GetPrevious();
      std::string realPrevType = realPrevCmd->GetTypeName();
      
      #if DEBUG_MISSION_TREE_APPEND
      WriteCommand("   ", "end = ", end, ", realPrevCmd = ", realPrevCmd);
      #endif
      
      // Here is tricky one.
      // We don't want to reset prevCmd if currCmd is If and previous of EndIf is Else,
      // since EndIf->GetPrevious() will return Else if there is Else
      if ((realPrevType != "Else") ||
          (realPrevType == "Else" && currCmd->GetTypeName() == "Else"))
      {
         #if DEBUG_MISSION_TREE_APPEND
         MessageInterface::ShowMessage("   setting prevCmd to end->GetPrevious()\n");
         #endif
         
         prevCmd = realPrevCmd;
      }
      
      #if DEBUG_MISSION_TREE_APPEND
      WriteCommand("   ", "prevCmd = ", prevCmd);
      #endif
      
      // If previous command is BranchCommand and not current command
      if (prevCmd->IsOfType("BranchCommand") && prevCmd != currCmd)
      {
         #if DEBUG_MISSION_TREE_APPEND
         MessageInterface::ShowMessage
            ("   previous command is %s and not %s\n", 
             prevCmd->GetTypeName().c_str(), currCmd->GetTypeName().c_str());
         #endif
         
         prevCmd = GmatCommandUtil::GetMatchingEnd(prevCmd);
         
         #if DEBUG_MISSION_TREE_APPEND
         MessageInterface::ShowMessage
            ("   so setting prevCmd to %s\n", prevCmd->GetTypeName().c_str(),
             currCmd->GetTypeName().c_str(), prevCmd->GetTypeName().c_str());
         #endif
      }
   }
   else if (currId == mMissionSeqSubId)
   {
      #if DEBUG_MISSION_TREE_APPEND
      MessageInterface::ShowMessage("   current item is MissionSequence\n");
      #endif
      
      parentId = itemId;
      if (lastId.IsOk() && GetItemText(lastId) != "")
         currId = lastId;
      prevId = currId;
      prevCmd = prevItem->GetCommand();
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
   cmd = CreateCommand(cmdName);
   
   // Insert a node to tree
   if (cmd != NULL)
   {
      UpdateGuiManager(cmdName);
      
      if (currCmd->GetTypeName() == "NoOp" ||
          currCmd->GetTypeName() == "BeginMissionSequence")
      {
         // Use GetLastCommand() to get last command since some command
         // doesn't appear on the tree, such as EndScript
         prevCmd = GmatCommandUtil::GetLastCommand(prevCmd);
         #if DEBUG_MISSION_TREE_APPEND
         WriteCommand("   ==>", " new prevCmd = ", prevCmd);
         #endif
         
         if (prevCmd->IsOfType("BranchCommand"))
             prevCmd = GmatCommandUtil::GetMatchingEnd(prevCmd);
      }
      
      // Need to set previous command of new command
      cmd->ForceSetPrevious(prevCmd);
      
      #if DEBUG_MISSION_TREE_APPEND
      MessageInterface::ShowMessage("   ==> Calling InsertCommand(before)\n");
      #endif

      // Added to tree node if visible command
      // Always insert before
      wxTreeItemId node =
         InsertCommand(parentId, currId, prevId, GetIconId(cmdName),
                       GetCommandId(cmdName), cmdName, prevCmd, cmd,
                       GetCommandCounter(cmdName), true);
      
      Expand(itemId);
      Expand(node);
   }
   
   #if DEBUG_MISSION_TREE_SHOW_CMD
   ShowCommands("After Append: " + cmdName);
   #endif
   
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   if (mSaveActions || mPlaybackActions)
      WriteResults();
   #endif
}


//------------------------------------------------------------------------------
// void DeleteCommand(const wxString &cmdName)
//------------------------------------------------------------------------------
/*
 * Deletes a command from the tree and command sequence.
 *
 * @param  cmdName  Command name to be deleted
 */
//------------------------------------------------------------------------------
void MissionTree::DeleteCommand(const wxString &cmdName)
{
   #if DEBUG_MISSION_TREE_DELETE
   MessageInterface::ShowMessage
      ("MissionTree::Delete() entered, cmdName='%s'\n", cmdName.c_str());
   #endif
   
   #if DEBUG_MISSION_TREE_SHOW_CMD
   ShowCommands("Before Delete(): " + cmdName);
   #endif
   
   // get selected item
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentId = GetItemParent(itemId);
   
   #if DEBUG_MISSION_TREE_DELETE
   MessageInterface::ShowMessage
      ("   itemId=%s, parentId=%s\n", GetItemText(itemId).c_str(),
       GetItemText(parentId).c_str());
   #endif
   
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   if (mSaveActions)
   {
      wxString str;
      str.Printf("Delete %s from %s\n", GetItemText(itemId).c_str(),
                 GetItemText(parentId).c_str());
      WriteActions(str);
   }
   #endif
   
   
   // delete from gui interpreter
   MissionTreeItemData *missionItem = (MissionTreeItemData *)GetItemData(itemId);
   if (missionItem == NULL)
   {
      // write error message
      MessageInterface::ShowMessage
         ("\n*** ERROR *** could not delete %s due to NULL item\n",
         cmdName.c_str());
      return;
   }
   
   GmatCommand *theCmd = missionItem->GetCommand();  
   if (theCmd == NULL)
   {
      // write error message
      MessageInterface::ShowMessage
         ("\n*** ERROR *** could not delete %s due to NULL command\n",
         cmdName.c_str());
      return;
   }
   
   // save command type to check if there is no more of this command
   std::string cmdType = theCmd->GetTypeName();
   
   #if DEBUG_MISSION_TREE_DELETE
   MessageInterface::ShowMessage
      ("   Calling theGuiInterpreter->DeleteCommand(%s)\n", theCmd->GetTypeName().c_str());
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
   
   GmatCommand *tmp = theGuiInterpreter->DeleteCommand(theCmd);
   if (tmp)
   {
      #if DEBUG_MISSION_TREE_DELETE
      MessageInterface::ShowMessage("   About to delete <%p>\n", tmp);
      #endif
      delete tmp;
      tmp = NULL;
   }
   
   // reset counter if there is no more of this command
   #if DEBUG_MISSION_TREE_DELETE
   MessageInterface::ShowMessage
      ("   Checking if the command counter needs to be reset\n");
   #endif
   std::string seqString = theGuiInterpreter->GetScript();
   if (seqString.find(cmdType) == seqString.npos)
   {
      #if DEBUG_MISSION_TREE_DELETE
      MessageInterface::ShowMessage
         ("   Resetting the command counter of '%s'\n", cmdType.c_str());
      #endif
      int *cmdCounter = GetCommandCounter(cmdType.c_str());
      *cmdCounter = 0;
   }
   
   // delete from tree - if parent only has 1 child collapse
   #if DEBUG_MISSION_TREE_DELETE
   MessageInterface::ShowMessage
      ("   Checking if the parent item needs to be collapsed\n");
   #endif
   if (GetChildrenCount(parentId) <= 1)
   {
      #if DEBUG_MISSION_TREE_DELETE
      MessageInterface::ShowMessage
         ("   About to collapse parent tree item '%s'\n", GetItemText(parentId).c_str());
      #endif
      this->Collapse(parentId);
   }
   
   #if DEBUG_MISSION_TREE_DELETE
   MessageInterface::ShowMessage
      ("   About to delete tree item '%s'\n", GetItemText(itemId).c_str());
   #endif
   this->Delete(itemId);
   
   #if DEBUG_MISSION_TREE_SHOW_CMD
   ShowCommands("After Delete()");
   #endif
   
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   if (mSaveActions || mPlaybackActions)
      WriteResults();
   #endif
   
   #if DEBUG_MISSION_TREE_DELETE
   MessageInterface::ShowMessage
      ("MissionTree::Delete() leaving, cmdName='%s'\n", cmdName.c_str());
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

   #ifdef __TEST_MISSION_TREE_ACTIONS__
   if (mSaveActions)
   {
      wxString str;
      str.Printf("Insert %s before %s\n", cmdName.c_str(), GetItemText(itemId).c_str());
      WriteActions(str);
   }
   #endif
   
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
      
   if (currCmd == NULL)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "*** Internal Error Occurred ***\n"
          "Current item has empty command. Cannot insert the command.\n");
      return;
   }
   
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage
      ("MissionTree::InsertBefore(%s) currCmd=%s(%p)\n", cmdName.c_str(),
       currCmd->GetTypeName().c_str(), currCmd);
   WriteCommand("   ", "prevCmd = ", prevCmd, ", realPrevCmd = ", realPrevCmd);
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
      ("MissionTree::InsertBefore(%s) prevCmd=%s(%p)\n",
       cmdName.c_str(), prevCmd->GetTypeName().c_str(), prevCmd);
   #endif
   
   // If previous command is BranchCmmand check to see we need to use matching
   // BranchEnd as previous command
   if (prevCmd->IsOfType("BranchCommand"))
   {
      // check if first child is current command
      if (prevCmd->GetChildCommand(0) == currCmd)
         realPrevCmd = prevCmd;
      else
         realPrevCmd = GmatCommandUtil::GetMatchingEnd(prevCmd);
   }
   
   #if DEBUG_MISSION_TREE_INSERT
   WriteCommand("   ", "realPrevCmd = ", realPrevCmd);
   #endif
   
   if (realPrevCmd != NULL)
   {
      cmd = CreateCommand(cmdName);      
      
      if (cmd != NULL)
      {
         // Set previous command to realPrevCmd (loj: 2007.05.16)
         cmd->ForceSetPrevious(realPrevCmd);
         
         #if DEBUG_MISSION_TREE_INSERT
         WriteCommand("   ", "cmd->GetPrevious() = ", cmd->GetPrevious());
         #endif
         
         UpdateGuiManager(cmdName);
         
         wxTreeItemId node =
            InsertCommand(parentId, itemId, prevId, GetIconId(cmdName),
                          GetCommandId(cmdName), cmdName, realPrevCmd, cmd,
                          GetCommandCounter(cmdName), true);
         
         Expand(parentId);
         Expand(prevId);
         Expand(node);
      }
   }
   
   #if DEBUG_MISSION_TREE_SHOW_CMD
   ShowCommands("After InsertBefore(): " + cmdName);
   #endif
   
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   if (mSaveActions || mPlaybackActions)
      WriteResults();
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
   
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   if (mSaveActions)
   {
      wxString str;
      str.Printf("Insert %s after %s\n", cmdName.c_str(), GetItemText(itemId).c_str());
      WriteActions(str);
   }
   #endif
   
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
      ("MissionTree::InsertAfter(%s) currCmd=%s(%p)\n", cmdName.c_str(),
       currCmd->GetTypeName().c_str(), currCmd);
   #endif
      
   GmatCommand *prevCmd = currCmd;   
   GmatCommand *cmd = NULL;
   
   if (currCmd != NULL)
   {
      cmd = CreateCommand(cmdName);
      
      // Need to set previous command
      cmd->ForceSetPrevious(currCmd);
      
      // Set parentId, itemId, prevId properly to pass to InsertCommand()
      //if (currCmd->IsOfType("BranchCommand"))
      // Else is BranchEnd, but we want it to be parent when inserting (loj: 2007.05.22)
      if (currCmd->IsOfType("BranchCommand") || currCmd->GetTypeName() == "Else")
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
         
         Expand(parentId);
         Expand(node);
      }
   }
   
   #if DEBUG_MISSION_TREE_SHOW_CMD
   ShowCommands("After InsertAfter(): " + cmdName);
   #endif
   
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   if (mSaveActions || mPlaybackActions)
      WriteResults();
   #endif
} // InsertAfter()


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
   
   wxTreeItemId mission =
      AddRoot(wxT("Mission"), -1, -1,
              new MissionTreeItemData(wxT("Mission"), GmatTree::MISSIONS_FOLDER));
   
   //-----------------------------------------------------------------
   #ifdef __ENABLE_MULTIPLE_SEQUENCE__
   //-----------------------------------------------------------------
   mMissionSeqTopId =
      AppendItem(mission, wxT("Mission Sequence"), GmatTree::MISSION_ICON_FOLDER, -1,
                 new MissionTreeItemData(wxT("Mission Sequence"),
                                         GmatTree::MISSION_SEQ_TOP_FOLDER));
   
   SetItemImage(mMissionSeqTopId, GmatTree::MISSION_ICON_OPENFOLDER,
               wxTreeItemIcon_Expanded);
   
   AddDefaultMissionSeq(mMissionSeqTopId);
   
   //-----------------------------------------------------------------
   #else
   //-----------------------------------------------------------------
   
   mMissionSeqSubId =
      AppendItem(mission, wxT("Mission Sequence"), GmatTree::MISSION_ICON_FOLDER, -1,
                 new MissionTreeItemData(wxT("Mission Sequence"),
                                         GmatTree::MISSION_SEQ_SUB_FOLDER));
   
   SetItemImage(mMissionSeqSubId, GmatTree::MISSION_ICON_OPENFOLDER,
               wxTreeItemIcon_Expanded);
   
   //-----------------------------------------------------------------
   #endif
   //-----------------------------------------------------------------
   
   UpdateCommand();
   theNotebook->SetMissionTreeExpandLevel(10); // level > 3 expands all
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
                 new MissionTreeItemData(wxT(objName),
                                         GmatTree::MISSION_SEQ_COMMAND));
   };
   #endif
   
   wxString name;   
   name.Printf("Sequence%d", ++mNumMissionSeq);
   
   mMissionSeqSubId =
      AppendItem(item, name, GmatTree::MISSION_ICON_FOLDER, -1,
                 new MissionTreeItemData(name, GmatTree::MISSION_SEQ_SUB_FOLDER));
   
   SetItemImage(mMissionSeqSubId, GmatTree::MISSION_ICON_OPENFOLDER,
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
   #ifdef DEBUG_ADD_ICONS
   MessageInterface::ShowMessage
      ("ResourceTree::AddIcons() entered, GmatTree::MISSION_ICON_COUNT=%d\n",
       GmatTree::MISSION_ICON_COUNT);
   #endif
   
   int sizeW = 16;
   int sizeH = 16;
   
   wxImageList *images = new wxImageList ( sizeW, sizeH, true );
   wxBitmap* bitmaps[GmatTree::MISSION_ICON_COUNT];
   int index = 0;
   long bitmapType = wxBITMAP_TYPE_PNG;
   
   // Show hourglass temporarily busy cursor
   wxBusyCursor wait;
   
   // Icons should follow the order in GmatTreeItemData::MissionIconType.
   theGuiManager->LoadIcon("propagateevent", bitmapType, &bitmaps[index], propagateevent_xpm);
   theGuiManager->LoadIcon("target", bitmapType, &bitmaps[++index], target_xpm);
   theGuiManager->LoadIcon("folder", bitmapType, &bitmaps[++index], folder_xpm);
   theGuiManager->LoadIcon("file", bitmapType, &bitmaps[++index], file_xpm);
   theGuiManager->LoadIcon("OpenFolder", bitmapType, &bitmaps[++index], OpenFolder_xpm);
   
   theGuiManager->LoadIcon("whileloop", bitmapType, &bitmaps[++index], whileloop_xpm);
   theGuiManager->LoadIcon("forloop", bitmapType, &bitmaps[++index], forloop_xpm);
   theGuiManager->LoadIcon("if", bitmapType, &bitmaps[++index], if_xpm);
   theGuiManager->LoadIcon("scriptevent", bitmapType, &bitmaps[++index], scriptevent_xpm);
   theGuiManager->LoadIcon("varyevent", bitmapType, &bitmaps[++index], varyevent_xpm);
   
   theGuiManager->LoadIcon("achieveevent", bitmapType, &bitmaps[++index], achieveevent_xpm);
   theGuiManager->LoadIcon("deltav", bitmapType, &bitmaps[++index], deltav_xpm);
   theGuiManager->LoadIcon("callfunction", bitmapType, &bitmaps[++index], callfunction_xpm);
   theGuiManager->LoadIcon("nestreturn", bitmapType, &bitmaps[++index], nestreturn_xpm);
   theGuiManager->LoadIcon("saveobject", bitmapType, &bitmaps[++index], saveobject_xpm);
   
   theGuiManager->LoadIcon("equalsign", bitmapType, &bitmaps[++index], equalsign_xpm);
   theGuiManager->LoadIcon("toggle", bitmapType, &bitmaps[++index], toggle_xpm);
   theGuiManager->LoadIcon("beginfb", bitmapType, &bitmaps[++index], beginfb_xpm);
   theGuiManager->LoadIcon("endfb", bitmapType, &bitmaps[++index], endfb_xpm);
   theGuiManager->LoadIcon("report", bitmapType, &bitmaps[++index], report_xpm);
   
   theGuiManager->LoadIcon("mt_Stop", bitmapType, &bitmaps[++index], mt_Stop_xpm);
   theGuiManager->LoadIcon("penup", bitmapType, &bitmaps[++index], penup_xpm);
   theGuiManager->LoadIcon("pendown", bitmapType, &bitmaps[++index], pendown_xpm);
   theGuiManager->LoadIcon("mt_MarkPoint", bitmapType, &bitmaps[++index], mt_MarkPoint_xpm);
   theGuiManager->LoadIcon("mt_ClearPlot", bitmapType, &bitmaps[++index], mt_ClearPlot_xpm);

   theGuiManager->LoadIcon("mt_Global", bitmapType, &bitmaps[++index], mt_Global_xpm);   
   theGuiManager->LoadIcon("mt_SaveMission", bitmapType, &bitmaps[++index], mt_SaveMission_xpm);   
   theGuiManager->LoadIcon("optimize", bitmapType, &bitmaps[++index], optimize_xpm);
   theGuiManager->LoadIcon("mt_Minimize", bitmapType, &bitmaps[++index], mt_Minimize_xpm);
   theGuiManager->LoadIcon("mt_NonlinearConstraint", bitmapType, &bitmaps[++index], mt_NonlinearConstraint_xpm);
   
   theGuiManager->LoadIcon("mt_RunSimulator", bitmapType, &bitmaps[++index], mt_RunSimulator_xpm);
   theGuiManager->LoadIcon("mt_RunEstimator", bitmapType, &bitmaps[++index], mt_RunEstimator_xpm);
   theGuiManager->LoadIcon("mt_Default", bitmapType, &bitmaps[++index], mt_Default_xpm);
   
   // Rescale if bitmap size is not 16x16 and use high quality scale (LOJ: 2011.04.22)
   int w, h;
   for ( size_t i = 0; i < WXSIZEOF(bitmaps); i++ )
   {
      w = bitmaps[i]->GetWidth();
      h = bitmaps[i]->GetHeight();
      
      #ifdef DEBUG_ADD_ICONS
      MessageInterface::ShowMessage("   bitmaps[%2d], w=%d, h=%d\n", i, w, h);
      #endif
      
      wxImage image = bitmaps[i]->ConvertToImage();
      if (w != sizeW || h != sizeH)
      {
         #ifdef DEBUG_ADD_ICONS
         MessageInterface::ShowMessage("   rescaling image to %d x %d\n", sizeW, sizeH);
         #endif
         
         image.Rescale(sizeW, sizeH, wxIMAGE_QUALITY_HIGH);
      }
      
      images->Add(image);
      //images->Add(bitmaps[i]->ConvertToImage().Rescale(sizeW, sizeH));
   }
   
   AssignImageList(images);
   
   #ifdef DEBUG_ADD_ICONS
   MessageInterface::ShowMessage
      ("ResourceTree::AddIcons() exiting, %d icons added\n", index + 1);
   #endif
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
   //wxWidgets-2.6.3 does not need this but wxWidgets-2.8.0 needs to SelectItem
   SelectItem(event.GetItem());
   mLastClickPoint = event.GetPoint();
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
   MessageInterface::ShowMessage
      ("MissionTree::OnItemActivated() item=%s parent=%s\n",
       item->GetTitle().c_str(), parent->GetTitle().c_str());
   #endif
   
   // Since VaryPanel is used for both Target and Optimize,
   // set proper id indicating Optimize Vary
   if ((item->GetItemType() == GmatTree::VARY) &&
       (parent->GetItemType() == GmatTree::OPTIMIZE))
      item->SetItemType(GmatTree::OPTIMIZE_VARY);
   
   theMainFrame->CreateChild(item);
}


//------------------------------------------------------------------------------
// void OnDoubleClick(wxMouseEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles double click on an item.
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
                                 item->GetTitle().c_str(), parent->GetTitle().c_str());
   #endif
   
   // Since VaryPanel is used for both Target and Optimize,
   // set proper id indicating Optimize Vary
   if ((item->GetItemType() == GmatTree::VARY) &&
       (parent->GetItemType() == GmatTree::OPTIMIZE))
      item->SetItemType(GmatTree::OPTIMIZE_VARY);
   
   // Show panel here. because OnItemActivated() always collapse the node.
   theMainFrame->CreateChild(item);

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
   wxString title = treeItem->GetTitle();
   GmatTree::ItemType itemType = treeItem->GetItemType();
   wxTreeItemId parent = GetItemParent(id);
   //MissionTreeItemData *parentItem = (MissionTreeItemData *)GetItemData(parent);
   //int parentDataType = parentItem->GetItemType();
   
   #if DEBUG_MISSION_TREE_MENU
   MessageInterface::ShowMessage
      ("MissionTree::ShowMenu() itemType=%d\n", itemType);
   #endif
   
#if wxUSE_MENUS
   wxMenu menu;
   
   if (itemType == GmatTree::MISSION_SEQ_TOP_FOLDER)
   {
      menu.Append(POPUP_ADD_MISSION_SEQ, wxT("Add Mission Sequence"));
      menu.Enable(POPUP_ADD_MISSION_SEQ, FALSE);
   }
   else if (itemType == GmatTree::MISSION_SEQ_SUB_FOLDER)
   {
      menu.Append(POPUP_COLLAPSE, wxT("Collapse"));
      menu.Append(POPUP_EXPAND, wxT("Expand"));
      menu.AppendSeparator();
      menu.Append(POPUP_APPEND, wxT("Append"), CreatePopupMenu(itemType, APPEND));
      
      // If multiple sequence is enabled
      #ifdef __ENABLE_MULTIPLE_SEQUENCE__
      menu.Append(POPUP_DELETE, wxT("Delete"));
      #endif
      
      menu.Enable(POPUP_RENAME, FALSE);
      menu.AppendSeparator();
      menu.Append(POPUP_RUN, wxT("Run"));
      menu.AppendSeparator();
      menu.AppendCheckItem(POPUP_SHOW_DETAIL, wxT("Show Detail"));
      menu.Check(POPUP_SHOW_DETAIL, mShowDetailedItem);
      
      menu.AppendSeparator();
      menu.Append(POPUP_SHOW_SCRIPT, wxT("Show Script"));
      
      //----- for auto testing actions
      #ifdef __TEST_MISSION_TREE_ACTIONS__
      menu.AppendSeparator();
      menu.Append(POPUP_START_SAVE_ACTIONS, wxT("Start Save Actions"));
      menu.Append(POPUP_STOP_SAVE_ACTIONS, wxT("Stop Save Actions"));
      menu.Append(POPUP_READ_ACTIONS, wxT("Playback Actions"));
      #endif
   }
   else
   {
      // add to non-EndBranch item
      if (itemType < GmatTree::BEGIN_NO_PANEL)
      {
         menu.Append(POPUP_OPEN, wxT("Open"));
         menu.Append(POPUP_CLOSE, wxT("Close"));
         menu.AppendSeparator();
      }
      
      if (itemType == GmatTree::TARGET)
      {
         menu.Append(POPUP_APPEND, wxT("Append"),
                     CreateTargetPopupMenu(itemType, APPEND));
         menu.Append(POPUP_INSERT_BEFORE, wxT("Insert Before"),
                     CreatePopupMenu(itemType, INSERT_BEFORE));         
         menu.Append(POPUP_INSERT_AFTER, wxT("Insert After"),
                     CreateTargetPopupMenu(itemType, INSERT_AFTER));
      }
      else if (itemType == GmatTree::OPTIMIZE)
      {
         menu.Append(POPUP_APPEND, wxT("Append"),
                     CreateOptimizePopupMenu(itemType, APPEND));
         menu.Append(POPUP_INSERT_BEFORE, wxT("Insert Before"),
                     CreatePopupMenu(itemType, INSERT_BEFORE));         
         menu.Append(POPUP_INSERT_AFTER, wxT("Insert After"),
                     CreateOptimizePopupMenu(itemType, INSERT_AFTER));         
      }
      else if (itemType == GmatTree::END_TARGET)
      {
         menu.Append(POPUP_INSERT_BEFORE, wxT("Insert Before"),
                     CreateTargetPopupMenu(itemType, INSERT_BEFORE));
         menu.Append(POPUP_INSERT_AFTER, wxT("Insert After"),
                     CreatePopupMenu(itemType, INSERT_AFTER));
      }
      else if (itemType == GmatTree::END_OPTIMIZE)
      {
         menu.Append(POPUP_INSERT_BEFORE, wxT("Insert Before"),
                     CreateOptimizePopupMenu(itemType, INSERT_BEFORE));
         menu.Append(POPUP_INSERT_AFTER, wxT("Insert After"),
                     CreatePopupMenu(itemType, INSERT_AFTER));
      }
      else 
      {
         GmatTree::ItemType itemType;
         if (IsInsideSolver(id, itemType))
         {
            if (itemType == GmatTree::TARGET)
            {
               menu.Append(POPUP_INSERT_BEFORE, wxT("Insert Before"),
                           CreateTargetPopupMenu(itemType, INSERT_BEFORE));
               menu.Append(POPUP_INSERT_AFTER, wxT("Insert After"),
                           CreateTargetPopupMenu(itemType, INSERT_AFTER));
            }
            else if (itemType == GmatTree::OPTIMIZE)
            {
               menu.Append(POPUP_INSERT_BEFORE, wxT("Insert Before"),
                           CreateOptimizePopupMenu(itemType, INSERT_BEFORE));
               menu.Append(POPUP_INSERT_AFTER, wxT("Insert After"),
                           CreateOptimizePopupMenu(itemType, INSERT_AFTER));
            }
         }
         else
         {
            menu.Append(POPUP_INSERT_BEFORE, wxT("Insert Before"),
                        CreatePopupMenu(itemType, INSERT_BEFORE));
            menu.Append(POPUP_INSERT_AFTER, wxT("Insert After"),
                        CreatePopupMenu(itemType, INSERT_AFTER));
         }
      }
      
      // Append is allowed for the control logic
      if ((itemType == GmatTree::IF_CONTROL) ||
          (itemType == GmatTree::ELSE_CONTROL) ||
          (itemType == GmatTree::FOR_CONTROL) ||
          (itemType == GmatTree::WHILE_CONTROL))
      {
         GmatTree::ItemType itemType;
         if (IsInsideSolver(id, itemType))
         {
            if (itemType == GmatTree::TARGET)
            {
               menu.Append(POPUP_APPEND, wxT("Append"),
                           CreateTargetPopupMenu(itemType, APPEND));   
            }
            else if (itemType == GmatTree::OPTIMIZE)
            {
               menu.Append(POPUP_APPEND, wxT("Append"),
                           CreateOptimizePopupMenu(itemType, APPEND));   
            }
            else
            {
               menu.Append(POPUP_APPEND, wxT("Append"),
                           CreatePopupMenu(itemType, APPEND));
            }
         }
      }
      
      // Delete applies to all, except End branch
      if (itemType < GmatTree::BEGIN_NO_PANEL || itemType == GmatTree::STOP)
      {
         menu.AppendSeparator();
         menu.Append(POPUP_RENAME, wxT("Rename"));
         menu.Append(POPUP_DELETE, wxT("Delete"));
      }
      
      //menu.Enable(POPUP_RENAME, FALSE);
      
   }
   
   PopupMenu(&menu, pt);
   
   
#endif // wxUSE_MENUS
}


//------------------------------------------------------------------------------
// void OnAddMissionSeq(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddMissionSeq(wxCommandEvent &event)
{
   wxTreeItemId itemId = GetSelection();
   wxString name;
   
   name.Printf("Sequence%d", ++mNumMissionSeq);
   
   mMissionSeqSubId =
      AppendItem(itemId, name, GmatTree::MISSION_ICON_FOLDER, -1,
                 new MissionTreeItemData(name, GmatTree::MISSION_SEQ_SUB_FOLDER));
    
   SetItemImage(mMissionSeqSubId, GmatTree::MISSION_ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);

   Expand(itemId);
}


//------------------------------------------------------------------------------
// void OnAppend(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAppend(wxCommandEvent &event)
{
   #ifdef DEBUG_MISSION_TREE_APPEND
   MessageInterface::ShowMessage
      ("MissionTree::OnAppend() entered, event id = %d, itemStr = '%s'\n",
       event.GetId(), event.GetString().c_str());
   #endif
   
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
   case POPUP_APPEND_CALL_GMAT_FUNCTION:
      Append("CallGmatFunction");
      break;
   case POPUP_APPEND_CALL_MATLAB_FUNCTION:
      Append("CallMatlabFunction");
      break;
   case POPUP_APPEND_ASSIGNMENT:
      Append("Equation");
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
   
   #ifdef DEBUG_MISSION_TREE_APPEND
   MessageInterface::ShowMessage("==> MissionTree::OnAppend() leaving\n");
   #endif
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
   case POPUP_INSERT_BEFORE_CALL_GMAT_FUNCTION:
      InsertBefore("CallGmatFunction");
      break;
   case POPUP_INSERT_BEFORE_CALL_MATLAB_FUNCTION:
      InsertBefore("CallMatlabFunction");
      break;
   case POPUP_INSERT_BEFORE_ASSIGNMENT:
      //InsertBefore("GMAT");
      InsertBefore("Equation");
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
   case POPUP_INSERT_AFTER_CALL_GMAT_FUNCTION:
      InsertAfter("CallGmatFunction");
      break;
   case POPUP_INSERT_AFTER_CALL_MATLAB_FUNCTION:
      InsertAfter("CallMatlabFunction");
      break;
   case POPUP_INSERT_AFTER_ASSIGNMENT:
      //InsertAfter("GMAT");
      InsertAfter("Equation");
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


//------------------------------------------------------------------------------
// void OnAutoAppend(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAutoAppend(wxCommandEvent &event)
{
   #ifdef DEBUG_MISSION_TREE_APPEND
   MessageInterface::ShowMessage
      ("=====> MissionTree::OnAutoAppend() entered, event id = %d\n", event.GetId());
   #endif
   
   int menuId = event.GetId();   
   Append(idCmdMap[menuId]);
   
   #ifdef DEBUG_MISSION_TREE_APPEND
   MessageInterface::ShowMessage("=====> MissionTree::OnAutoAppend() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnAutoInsertBefore(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAutoInsertBefore(wxCommandEvent &event)
{
   #ifdef DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage
      ("MissionTree::OnAutoInsertBefore() entered, event id = %d\n", event.GetId());
   #endif
   
   int menuId = event.GetId();   
   InsertBefore(idCmdMap[menuId]);
   
   #ifdef DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage("MissionTree::OnAutoInsertBefore() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnAutoInsertAfter(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAutoInsertAfter(wxCommandEvent &event)
{
   #ifdef DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage
      ("MissionTree::OnAutoInsertAfter() entered, event id = %d\n", event.GetId());
   #endif
   
   wxString cmdString = event.GetString();
   int menuId = event.GetId();
   InsertAfter(idCmdMap[menuId]);
   
   #ifdef DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage("MissionTree::OnAutoInsertAfter() leaving\n");
   #endif
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
      ("MissionTree::CreatePopupMenu() type=%d, action=%d\n", type, action);
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
      wxTreeItemId elseId = FindChild(parentId, "Else");
      if (elseId.IsOk() && GetItemText(elseId) != "")
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
// void OnBeginEditLabel(wxTreeEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles EVT_TREE_BEGIN_LABEL_EDIT for beginning editing a label.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void MissionTree::OnBeginEditLabel(wxTreeEvent& event)
{
   // if panel is currently opened give warning and veto
   wxTreeItemId itemId = GetSelection();
   GmatTreeItemData *selItem = (GmatTreeItemData *) GetItemData(itemId);
   if (theMainFrame->IsChildOpen(selItem))
   {
      wxLogWarning(selItem->GetTitle() + " cannot be renamed while panel is opened");
      wxLog::FlushActive();
      event.Veto();
   }
}


//------------------------------------------------------------------------------
// void OnEndEditLabel(wxTreeEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles EVT_TREE_END_LABEL_EDIT for ending editing a label.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void MissionTree::OnEndEditLabel(wxTreeEvent& event)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage("OnEndEditLabel() entered\n");
   #endif
   
   wxString newLabel = event.GetLabel();
   wxTreeItemId itemId = event.GetItem();
   MissionTreeItemData *item = (MissionTreeItemData *)GetItemData(itemId);
   GmatCommand *cmd = item->GetCommand();
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("   old cmd name = '%s'\n", cmd->GetName().c_str());
   #endif
   
   item->SetName(newLabel);
   item->SetTitle(newLabel);
   cmd->SetName(newLabel.c_str());
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("   new cmd name = '%s'\n", cmd->GetName().c_str());
   #endif
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage("OnEndEditLabel() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnRename()
//------------------------------------------------------------------------------
void MissionTree::OnRename(wxCommandEvent &event)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage("OnRename() entered\n");
   #endif
   
   // get selected item
   wxTreeItemId itemId = GetSelection();
   GmatTreeItemData *selItem = (GmatTreeItemData *) GetItemData(itemId);
   MissionTreeItemData *item = (MissionTreeItemData *)selItem;
   GmatCommand *cmd = item->GetCommand();
   wxString cmdName = GetItemText(itemId);
   
   // if panel is currently opened give warning and return
   // Bug 547 fix (loj: 2008.11.25)
   if (theMainFrame->IsChildOpen(selItem))
   {
      wxLogWarning(selItem->GetTitle() + " cannot be renamed while panel is opened");
      wxLog::FlushActive();
      return;
   }
   
   // Do we want to use rename dialog here?
   //=================================================================
   #if 1
   //=================================================================
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("   mLastClickPoint.x=%d, mLastClickPoint.y=%d\n",
       mLastClickPoint.x, mLastClickPoint.y);
   #endif
   
   mLastClickPoint.y += 100;
   ViewTextDialog renameDlg(this, wxT("Rename"), true, mLastClickPoint,
                            wxSize(100, -1), wxDEFAULT_DIALOG_STYLE);
   renameDlg.AppendText(cmdName);
   renameDlg.ShowModal();
   
   if (renameDlg.HasTextChanged())
   {
      wxString newName = renameDlg.GetText();
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage
         ("  Setting command name to '%s'\n", newName.c_str());
      #endif
      SetItemText(itemId, newName);
      item->SetName(newName);
      item->SetTitle(newName);
      cmd->SetName(newName.c_str());
   }
   
   //=================================================================
   #else
   //=================================================================
   
   //@note
   // To enable this function, xTR_EDIT_LABELS style must be set when creating
   // MissionTree in GmatNotebook
   EditLabel(itemId);   
   
   //=================================================================
   #endif
   //=================================================================
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage("OnRename() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnDelete()
//------------------------------------------------------------------------------
void MissionTree::OnDelete(wxCommandEvent &event)
{
   // get selected item
   wxTreeItemId itemId = GetSelection();
   GmatTreeItemData *selItem = (GmatTreeItemData *) GetItemData(itemId);
   wxString cmdName = GetItemText(itemId);
   
   // if panel is currently opened give warning and return
   // Bug 547 fix (loj: 2008.11.25)
   if (theMainFrame->IsChildOpen(selItem))
   {
      wxLogWarning(selItem->GetTitle() + " cannot be deleted "
                   "while panel is opened");
      wxLog::FlushActive();
      return;
   }
   
   DeleteCommand(cmdName);
   
}


//---------------------------------------------------------------------------
// void OnRun()
//--------------------------------------------------------------------------
void MissionTree::OnRun(wxCommandEvent &event)
{
   theGuiInterpreter->RunMission();
}


//---------------------------------------------------------------------------
// void OnShowDetail()
//--------------------------------------------------------------------------
void MissionTree::OnShowDetail(wxCommandEvent &event)
{
   mShowDetailedItem = event.IsChecked();
   UpdateMission(true);
}


//---------------------------------------------------------------------------
// void OnShowScript()
//--------------------------------------------------------------------------
void MissionTree::OnShowScript(wxCommandEvent &event)
{
   std::string str = theGuiInterpreter->GetScript();
   
   if (str == "")
      return;
   
   ViewTextFrame *vtf =
      new ViewTextFrame(theMainFrame, _T("Show Script"),
       50, 50, 800, 500, "Temporary", "Script");
   
   vtf->AppendText(str.c_str());
   vtf->Show(true);
   
}


//---------------------------------------------------------------------------
// bool CheckClickIn(wxPoint position)
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
      GmatTree::ItemType itemType = missionTreeItemData->GetItemType();
      // don't have to open any panels for top folders
      if ((itemType != GmatTree::MISSIONS_FOLDER)       &&
          (itemType != GmatTree::MISSION_SEQ_TOP_FOLDER)&&
          (itemType != GmatTree::MISSION_SEQ_SUB_FOLDER)&&
          (itemType != GmatTree::MISSION_SEQ_COMMAND))
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
                  new MissionTreeItemData(wxT("Variables"),
                                          GmatTree::VIEW_SOLVER_VARIABLES);
               theMainFrame->CreateChild(item);
            }
            else if ((position.x <= w-offset-boxWidth*boxNum) &&
                     (position.x >= w-offset-boxWidth*(++boxNum)))
            {
               //MessageInterface::ShowMessage("\nInside goals");
               MissionTreeItemData *item =
                  new MissionTreeItemData(wxT("Goals"),
                                          GmatTree::VIEW_SOLVER_GOALS);
               theMainFrame->CreateChild(item);
            }
            else
            {
               //MessageInterface::ShowMessage("\nOpen regular panel");
               theMainFrame->CreateChild(missionTreeItemData);
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
   theMainFrame->CreateChild(item);
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
            ("MissionTree::OnClose() while-loop, item->GetTitle(): \"%s\"\n",
             item->GetTitle().c_str());
         #endif
         
         if (theMainFrame->IsChildOpen(item))
            theMainFrame->CloseActiveChild();
         
         childId = GetNextChild(currId, cookie);
         
         #if DEBUG_MISSION_TREE_DELETE
         MessageInterface::ShowMessage
            ("MissionTree::OnClose() childId=<%s>\n", GetItemText(childId).c_str());
         #endif
      }
   }
   
   
   // delete selected item panel, if its open, its activated
   if (theMainFrame->IsChildOpen(currItem))
      theMainFrame->CloseActiveChild();
   else
      return;
   
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
      return GmatTree::MISSION_ICON_OPTIMIZE;
   if (cmd == "EndOptimize")
      return GmatTree::MISSION_ICON_NEST_RETURN;
   if (cmd == "Achieve")
      return GmatTree::MISSION_ICON_ACHIEVE;
   if (cmd == "Minimize")
      return GmatTree::MISSION_ICON_MINIMIZE;
   if (cmd == "NonlinearConstraint")
      return GmatTree::MISSION_ICON_NONLINEAR_CONSTRAINT;
   if (cmd == "Vary")
      return GmatTree::MISSION_ICON_VARY;
   if (cmd == "Save")
      return GmatTree::MISSION_ICON_SAVE;
   if (cmd == "GMAT")
      return GmatTree::MISSION_ICON_ASSIGNMENT;
   if (cmd == "Equation")
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
   if (cmd == "CallGmatFunction")
      return GmatTree::MISSION_ICON_CALL_FUNCTION;
   if (cmd == "CallMatlabFunction")
      return GmatTree::MISSION_ICON_CALL_FUNCTION;
   if (cmd == "Stop")
      return GmatTree::MISSION_ICON_STOP;
   if (cmd == "BeginScript")
      return GmatTree::MISSION_ICON_SCRIPTEVENT;
   if (cmd == "ScriptEvent")
      return GmatTree::MISSION_ICON_SCRIPTEVENT;
   if (cmd == "PenUp")
      return GmatTree::MISSION_ICON_PEN_UP;
   if (cmd == "PenDown")
      return GmatTree::MISSION_ICON_PEN_DOWN;
   if (cmd == "MarkPoint")
      return GmatTree::MISSION_ICON_MARK_POINT;
   if (cmd == "ClearPlot")
      return GmatTree::MISSION_ICON_CLEAR_PLOT;
   if (cmd == "Global")
      return GmatTree::MISSION_ICON_GLOBAL;
   if (cmd == "SaveMission")
      return GmatTree::MISSION_ICON_SAVE_MISSION;
   if (cmd == "Minimize")
      return GmatTree::MISSION_ICON_MINIMIZE;
   if (cmd == "NonlinearConstraint")
      return GmatTree::MISSION_ICON_NONLINEAR_CONSTRAINT;
   if (cmd == "RunSimulator")
      return GmatTree::MISSION_ICON_RUN_SIMULATOR;
   if (cmd == "RunEstimator")
      return GmatTree::MISSION_ICON_RUN_ESTIMATOR;
   
   return GmatTree::MISSION_ICON_DEFAULT;
}


//------------------------------------------------------------------------------
// wxString GetCommandString(GmatCommand *cmd, const wxString &currStr)
//------------------------------------------------------------------------------
/*
 * Returns command string if command is not a BranchCommand or Begin/EndScript.
 */
//------------------------------------------------------------------------------
wxString MissionTree::GetCommandString(GmatCommand *cmd, const wxString &currStr)
{
   if (!mShowDetailedItem)
      return currStr;
   
   if (cmd->GetTypeName() == "BeginScript" || cmd->GetTypeName() == "EndScript")
      return currStr;
   
   wxString cmdString;
   cmdString = cmd->GetGeneratingString(Gmat::NO_COMMENTS).c_str();
   
   #ifdef DEBUG_CMD_STRING
   MessageInterface::ShowMessage("GetCommandString() cmdString='%s'\n", cmdString.c_str());
   #endif
   
   if (cmdString == ";")
      return currStr;
   else
      return cmdString;
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
   if (cmd == "CallGmatFunction")
      return GmatTree::CALL_FUNCTION;
   if (cmd == "CallMatlabFunction")
      return GmatTree::CALL_FUNCTION;
   if (cmd == "Stop")
      return GmatTree::STOP;
   if (cmd == "GMAT")
      return GmatTree::ASSIGNMENT;
   if (cmd == "Equation")
      return GmatTree::ASSIGNMENT;
   if (cmd == "BeginScript")
      return GmatTree::SCRIPT_EVENT;
   if (cmd == "ClearPlot" || cmd == "MarkPoint" || cmd == "PenUp" || cmd == "PenDown")
      return GmatTree::XY_PLOT_ACTION;
   
   return GmatTree::OTHER_COMMAND;
}


//------------------------------------------------------------------------------
// void CreateCommandIdMap()
//------------------------------------------------------------------------------
void MissionTree::CreateCommandIdMap()
{
   wxString cmd;
   int cmdIndex = 0;
   
   for (unsigned int i=0; i<mCommandList.size(); i++)
   {
      cmd = mCommandList[i];
      CreateMenuIds(cmd, i);
      cmdIndex++;
   }
}


//------------------------------------------------------------------------------
// void CreateMenuIds(const wxString &cmd, int index)
//------------------------------------------------------------------------------
void MissionTree::CreateMenuIds(const wxString &cmd, int index)
{
   int id;
   wxString str, realCmd;
   realCmd = cmd;
   
   // If command to show is ScriptEvent, we want to create BeginScript
   if (cmd == "ScriptEvent")
      realCmd = "BeginScript";
   
   // Create id for append
   str = "AP*" + cmd;
   id = index + AUTO_APPEND_COMMAND + 1;
   cmdIdMap.insert(std::make_pair(str, id));
   idCmdMap.insert(std::make_pair(id, realCmd));
   
   // Create id for insert before
   str = "IB*" + cmd;
   id = index + AUTO_INSERT_BEFORE_COMMAND + 1;
   cmdIdMap.insert(std::make_pair(str, id));
   idCmdMap.insert(std::make_pair(id, realCmd));
   
   // Create id for insert after
   str = "IA*" + cmd;
   id = index + AUTO_INSERT_AFTER_COMMAND + 1;
   cmdIdMap.insert(std::make_pair(str, id));
   idCmdMap.insert(std::make_pair(id, realCmd));
}


//------------------------------------------------------------------------------
// int GetMenuId(const wxString &cmd, ActionType action)
//------------------------------------------------------------------------------
int MissionTree::GetMenuId(const wxString &cmd, ActionType action)
{
   #if DEBUG_MISSION_TREE_MENU
   MessageInterface::ShowMessage
      ("MissionTree::GetMenuId() cmd='%s', action=%d\n", cmd.c_str(), action);
   #endif
   
   int id = -1;
   
   //-----------------------------------------------------------------
   #ifdef __AUTO_ADD_NEW_COMMANDS__
   //-----------------------------------------------------------------
   wxString cmdStr = cmd;
   
   // Add prefix to command string
   if (action == APPEND)
      cmdStr = "AP*" + cmdStr;
   else if (action == INSERT_BEFORE)
      cmdStr = "IB*" + cmdStr;
   else if (action == INSERT_AFTER)
      cmdStr = "IA*" + cmdStr;
   
   // check if command string is valid
   if (cmdIdMap.find(cmdStr) == cmdIdMap.end())
   {
      #if DEBUG_MISSION_TREE_MENU
      MessageInterface::ShowMessage
         ("MissionTree::GetMenuId() The '%s' is not recognized command\n", cmdStr.c_str());
      #endif
      return id;
   }
   
   id = cmdIdMap[cmdStr];
   
   #if DEBUG_MISSION_TREE_MENU
   MessageInterface::ShowMessage("MissionTree::GetMenuId() returning %d\n", id);
   #endif
   
   return id;
   
   //-----------------------------------------------------------------
   #else
   //-----------------------------------------------------------------
   
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
         else if (cmd == "CallGmatFunction")
            return POPUP_APPEND_CALL_GMAT_FUNCTION;
         else if (cmd == "CallMatlabFunction")
            return POPUP_APPEND_CALL_MATLAB_FUNCTION;
         else if (cmd == "GMAT")
            return POPUP_APPEND_ASSIGNMENT;
         else if (cmd == "Equation")
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
         {
            MessageInterface::ShowMessage
               ("MissionTree::GetMenuId() Unknown append command: %s\n", cmd.c_str());
            return POPUP_APPEND_UNKNOWN;
         }
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
         else if (cmd == "CallGmatFunction")
            return POPUP_INSERT_BEFORE_CALL_GMAT_FUNCTION;
         else if (cmd == "CallMatlabFunction")
            return POPUP_INSERT_BEFORE_CALL_MATLAB_FUNCTION;
         else if (cmd == "GMAT")
            return POPUP_INSERT_BEFORE_ASSIGNMENT;
         else if (cmd == "Equation")
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
         {
            MessageInterface::ShowMessage
               ("MissionTree::GetMenuId() Unknown command:%s\n", cmd.c_str());
            return POPUP_INSERT_BEFORE_UNKNOWN;
         }
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
         else if (cmd == "CallGmatFunction")
            return POPUP_INSERT_AFTER_CALL_GMAT_FUNCTION;
         else if (cmd == "CallMatlabFunction")
            return POPUP_INSERT_AFTER_CALL_MATLAB_FUNCTION;
         else if (cmd == "GMAT")
            return POPUP_INSERT_AFTER_ASSIGNMENT;
         else if (cmd == "Equation")
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
         {
            MessageInterface::ShowMessage
               ("MissionTree::GetMenuId() Unknown command:%s\n", cmd.c_str());
            return POPUP_INSERT_AFTER_UNKNOWN;
         }
      }
   }
   
   return id;
   
   //-----------------------------------------------------------------
   #endif
   //-----------------------------------------------------------------
   
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
   if (cmd == "CallGmatFunction")
      return &mNumFunct;
   if (cmd == "CallMatlabFunction")
      return &mNumFunct;
   if (cmd == "GMAT")
      return &mNumAssign;
   if (cmd == "Equation")
      return &mNumAssign;
   if (cmd == "Stop")
      return &mNumStop;
   if (cmd == "Minimize")
      return &mNumMinimize;
   if (cmd == "NonlinearConstraint")
      return &mNumNonlinearConstraint;
   if (cmd == "BeginScript")
      return &mNumScriptEvent;
   if (cmd == "ScriptEvent")
      return &mNumScriptEvent;
   
   return &mTempCounter;
}


//------------------------------------------------------------------------------
// wxTreeItemId FindChild(wxTreeItemId parentId, const wxString &cmd)
//------------------------------------------------------------------------------
/*
 * Finds a item from the parent node of the tree.
 *
 * @param <parentId> Parent item id
 * @param <cmd> Name of the command to find
 */
//------------------------------------------------------------------------------
wxTreeItemId MissionTree::FindChild(wxTreeItemId parentId, const wxString &cmd)
{
   #if DEBUG_MISSION_TREE_FIND
   MessageInterface::ShowMessage
      ("\nMissionTree::FindChild() parentId=<%s>, cmd=<%s>\n",
       GetItemText(parentId).c_str(), cmd.c_str());
   #endif
   
   MissionTreeItemData *parentItem = (MissionTreeItemData *)GetItemData(parentId);
   GmatTree::ItemType parentType = parentItem->GetItemType();
   unsigned int itemCount = 0;
   wxTreeItemId firstItem;
   
   // if parent is top node, then get total count
   if (parentType == GmatTree::MISSION_SEQ_SUB_FOLDER)
   {
      itemCount = GetCount();
      firstItem = GetFirstVisibleItem();
   }
   else
   {
      itemCount = GetChildrenCount(parentId);
      firstItem = parentId;
   }
   
   wxTreeItemId item = firstItem;
   wxString itemText;
   
   for (unsigned int i=0; i<itemCount; i++)
   {
      itemText = GetItemText(item);
      
      #if DEBUG_MISSION_TREE_FIND > 1
      MessageInterface::ShowMessage("   item=<%s>\n", itemText.c_str());
      #endif
      
      if (itemText.Contains(cmd))
         return item;
      
      item = GetNextVisible(item);
   }
   
   #if DEBUG_MISSION_TREE_FIND
   MessageInterface::ShowMessage
      ("MissionTree::FindChild() returning unknown child\n");
   #endif
   
   wxTreeItemId nullId;
   return nullId;
   
}


//------------------------------------------------------------------------------
// bool IsInsideSolver(wxTreeItemId itemId, GmatTree::ItemType &itemType)
//------------------------------------------------------------------------------
/*
 * Checks if an item is inside of solver (Target, Optimize) branch.
 *
 */
//------------------------------------------------------------------------------
bool MissionTree::IsInsideSolver(wxTreeItemId itemId, GmatTree::ItemType &itemType)
{
   #if DEBUG_FIND_ITEM_PARENT
   MessageInterface::ShowMessage
      ("MissionTree::IsInsideSolver() itemId=%s\n", GetItemText(itemId).c_str());
   #endif
   
   wxTreeItemId parentId = GetItemParent(itemId);
   MissionTreeItemData *parentItem;
   GmatTree::ItemType parentType;
   
   // go through parents
   while (parentId.IsOk() && GetItemText(parentId) != "")
   {
      #if DEBUG_FIND_ITEM_PARENT > 1
      MessageInterface::ShowMessage("   parent=%s\n", GetItemText(parentId).c_str());
      #endif
      
      parentItem = (MissionTreeItemData *)GetItemData(parentId);
      parentType = parentItem->GetItemType();
      
      if (parentType == GmatTree::TARGET || parentType == GmatTree::OPTIMIZE)
      {
         #if DEBUG_FIND_ITEM_PARENT
         MessageInterface::ShowMessage
            ("MissionTree::IsInsideSolver() returning true, parent=%s\n",
             GetItemText(parentId).c_str());
         #endif
         
         itemType = parentType;
         return true;
      }
      
      parentId = GetItemParent(parentId);
   }
   
   #if DEBUG_FIND_ITEM_PARENT
   MessageInterface::ShowMessage("MissionTree::IsInsideSolver() returning false\n");
   #endif
   
   return false;
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


//------------------------------------------------------------------------------
// void WriteCommand(const std::string &prefix = "",
//                   const std::string &title1, GmatCommand *cmd1,
//                   const std::string &title2 = "", GmatCommand *cmd2 = NULL)
//------------------------------------------------------------------------------
/*
 * Writes command info to message window.
 */
//------------------------------------------------------------------------------
void MissionTree::WriteCommand(const std::string &prefix,
                               const std::string &title1, GmatCommand *cmd1,
                               const std::string &title2, GmatCommand *cmd2)
{
   if (title2 == "")
   {
      if (cmd1 == NULL)
         MessageInterface::ShowMessage
            ("%s%sNULL(%p)\n", prefix.c_str(), title1.c_str(), cmd1);
      else
         MessageInterface::ShowMessage
            ("%s%s%s(%p)\n", prefix.c_str(), title1.c_str(),
             cmd1->GetTypeName().c_str(), cmd1);
   }
   else
   {
      if (cmd1 == NULL)
         MessageInterface::ShowMessage
            ("%s%sNULL(%p)%s%s(%p)\n", prefix.c_str(), title1.c_str(),
             cmd1, title2.c_str(), cmd2->GetTypeName().c_str(), cmd2);
      else if (cmd2 == NULL)
         MessageInterface::ShowMessage
            ("%s%s%s(%p)%sNULL(%p)\n", prefix.c_str(), title1.c_str(),
             cmd1->GetTypeName().c_str(), cmd1, title2.c_str(), cmd2);
      else
         MessageInterface::ShowMessage
            ("%s%s%s(%p)%s%s(%p)\n", prefix.c_str(), 
             title1.c_str(), cmd1->GetTypeName().c_str(), cmd1,
             title2.c_str(), cmd2->GetTypeName().c_str(), cmd2);
   }
}


#ifdef __TEST_MISSION_TREE_ACTIONS__
//------------------------------------------------------------------------------
// void OnStartSaveActions()
//------------------------------------------------------------------------------
/**
 * Start saving actions on the tree to a text file for used in testing
 */
//------------------------------------------------------------------------------
void MissionTree::OnStartSaveActions(wxCommandEvent &event)
{
   #ifdef DEBUG_MISSION_TREE_ACTIONS
   MessageInterface::ShowMessage
      ("MissionTree::OnStartSaveActions() mSaveActions=%d\n", mSaveActions);
   #endif
   
   mSaveActions = true;
   mPlaybackActions = false;
   
   if (mActionsOutStream.is_open())
      mActionsOutStream.close();
   
   if (mResultsStream.is_open())
      mResultsStream.close();
   
   mActionsOutStream.open(mActionsOutFile.c_str());
   mResultsStream.open(mResultsFile.c_str());
}


//------------------------------------------------------------------------------
// void OnStopSaveActions()
//------------------------------------------------------------------------------
/**
 * Stops saving actions on the tree to a text file.
 */
//------------------------------------------------------------------------------
void MissionTree::OnStopSaveActions(wxCommandEvent &event)
{
   #ifdef DEBUG_MISSION_TREE_ACTIONS
   MessageInterface::ShowMessage
      ("MissionTree::OnStopSaveActions() mSaveActions=%d\n", mSaveActions);
   #endif
   
   mSaveActions = false;
   mActionsOutStream.close();
   mResultsStream.close();
}


//------------------------------------------------------------------------------
// void OnPlaybackActions()
//------------------------------------------------------------------------------
/**
 * Reads actions from a text file, parses, and initiate actions.
 */
//------------------------------------------------------------------------------
void MissionTree::OnPlaybackActions(wxCommandEvent &event)
{
   #ifdef DEBUG_MISSION_TREE_ACTIONS
   MessageInterface::ShowMessage
      ("MissionTree::OnPlaybackActions() mSaveActions=%d\n", mSaveActions);
   #endif
   
   wxString actionsInFile =
      wxFileSelector("Choose a file to open", "", "", "txt",
                     "Text files (*.txt)|*.txt", wxOPEN);
   
   if (actionsInFile.empty())
      return;
   
   // clear command sequence and mission tree first
   #ifdef DEBUG_MISSION_TREE_ACTIONS
   MessageInterface::ShowMessage("   clearing command sequence and mission tree\n");
   #endif
   
   ClearMission();
   theGuiInterpreter->ClearCommandSeq();
   InitializeCounter();
   
   wxString playbackResultsFile;
   
   // compose playback results file
   int dot = actionsInFile.Find('.');
   if (dot == wxNOT_FOUND)
      playbackResultsFile = actionsInFile + "PbResults.txt";
   else
      playbackResultsFile = actionsInFile.Mid(0, dot) + "PbResults.txt";
   
   #ifdef DEBUG_MISSION_TREE_ACTIONS
   MessageInterface::ShowMessage
      ("   playback action file = %s\n   playback results file = %s\n",
       actionsInFile.c_str(), playbackResultsFile.c_str());
   #endif
   
   mSaveActions = false;
   mPlaybackActions = true;
   
   // close output streams
   if (mPlaybackResultsStream.is_open())
      mPlaybackResultsStream.close();
   
   // open streams
   mPlaybackResultsStream.open(playbackResultsFile.c_str());
   
   if (!mPlaybackResultsStream.is_open())
   {
      MessageInterface::ShowMessage
         ("\n*** ERROR *** Playback stopped due to error opening the file %s\n",
          playbackResultsFile.c_str());
      return;
   }
   
   std::ifstream actionsInStream(actionsInFile.c_str());
   
   if (!actionsInStream.is_open())
   {
      MessageInterface::ShowMessage
         ("\n*** ERROR *** Playback stopped due to error opening the file %s\n",
          actionsInFile.c_str());
      return;
   }
   
   //-----------------------------------------------------------------
   // read in lines
   //-----------------------------------------------------------------
   char buffer[1024];
   StringArray lines;
   while (!actionsInStream.eof())
   {
      actionsInStream.getline(buffer, 1023);
      
      #ifdef DEBUG_MISSION_TREE_ACTIONS
      MessageInterface::ShowMessage("   <%s>\n", buffer);
      #endif
      
      if (buffer[0] != '\0')
         lines.push_back(buffer);
   }
   
   actionsInStream.close();
   
   //--------------------------------------------------------------
   // Find the first item
   //--------------------------------------------------------------
   wxTreeItemId firstItemId = GetFirstVisibleItem();
   
   if (firstItemId.IsOk())
   {
      #ifdef DEBUG_MISSION_TREE_ACTIONS
      MessageInterface::ShowMessage
         ("   first item is <%s>\n", GetItemText(firstItemId).c_str());
      #endif
   }
   else
   {
      MessageInterface::ShowMessage
         ("\n*** ERROR *** Playback stopped due to first item not found\n");
      return;
   }
   
   //-----------------------------------------------------------------
   // parse lines into actions
   //-----------------------------------------------------------------
   int lineCount = lines.size();
   StringTokenizer stk;
   StringArray actions;
   
   for (int i=0; i<lineCount; i++)
   {
      stk.Set(lines[i], " ");
      actions = stk.GetAllTokens();
      
      #ifdef DEBUG_MISSION_TREE_ACTIONS
      MessageInterface::ShowMessage("\n");
      for (UnsignedInt i=0; i<actions.size(); i++)
         MessageInterface::ShowMessage("<%s>", actions[i].c_str());
      MessageInterface::ShowMessage("\n");
      #endif
      
      //------------------------------------------
      // Sample actions:
      // Append Optimize to Mission Sequence
      // Append If to Optimize1
      // Append Equation to If1
      // Append While to Optimize1
      // Insert Maneuver after While1
      // Delete Stop1 from Mission Sequence
      // Delete Report2 from Optimize1
      //------------------------------------------
      
      //--------------------------------------------------------------
      // Find select item
      //--------------------------------------------------------------
      wxString selCmd = actions[3].c_str();
      wxString cmd = actions[1].c_str();
      
      #ifdef DEBUG_MISSION_TREE_ACTIONS
      MessageInterface::ShowMessage
         ("   selCmd = %s, cmd = %s\n", selCmd.c_str(), cmd.c_str());
      #endif
      
      bool itemFound = false;
      wxTreeItemId itemId;
      
      //--------------------------------------------------------------
      // Select item
      //--------------------------------------------------------------
      if (actions[0] == "Delete")
      {
         itemId = FindChild(firstItemId, cmd);
      }
      else
      {
         if (selCmd == "Mission")
            itemId = firstItemId;
         else
            itemId = FindChild(firstItemId, selCmd);
      }
      
      if (itemId.IsOk() && GetItemText(itemId) != "")
      {
         SelectItem(itemId);
         itemFound = true;
      }
      
      #ifdef DEBUG_MISSION_TREE_ACTIONS
      wxTreeItemId selId = GetSelection();
      MessageInterface::ShowMessage
         ("   GetSelection()=%s\n", GetItemText(selId).c_str());
      #endif
      
      
      //--------------------------------------------------------------
      // Do actions
      //--------------------------------------------------------------
      if (itemFound)
      {
         if (actions[0] == "Append")
         {
            Append(cmd);
         }
         else if (actions[0] == "Insert")
         {
            if (actions[2] == "before")
               InsertBefore(cmd);
            else
               InsertAfter(cmd);
         }
         else if (actions[0] == "Delete")
         {
            DeleteCommand(cmd);
         }
         else
         {
            MessageInterface::ShowMessage
               ("\n*** ERROR *** Playback stopped due to unknown action \"%s\"\n",
                actions[0].c_str());
         }
      }
      else
      {
         MessageInterface::ShowMessage
            ("\n*** ERROR *** Playback stopped due to %s not found\n",
             selCmd.c_str());
         return;
      }
   }
   
   // close playback results stream
   mPlaybackResultsStream.close();
   
   #ifdef DEBUG_MISSION_TREE_ACTIONS
   ShowCommands("After Playback");
   #endif
}


//------------------------------------------------------------------------------
// void WriteActions(const wxString &str)
//------------------------------------------------------------------------------
void MissionTree::WriteActions(const wxString &str)
{
   #ifdef DEBUG_MISSION_TREE_ACTIONS
   MessageInterface::ShowMessage("\n..........%s", str.c_str());
   #endif
   
   // write actions
   mActionsOutStream << str;
}


//------------------------------------------------------------------------------
// void WriteResults()
//------------------------------------------------------------------------------
void MissionTree::WriteResults()
{
   // write results
   GmatCommand *cmd = theGuiInterpreter->GetFirstCommand();
   
   if (mSaveActions)
      mResultsStream << GmatCommandUtil::GetCommandSeqString(cmd, false);
   else if (mPlaybackActions)
      mPlaybackResultsStream << GmatCommandUtil::GetCommandSeqString(cmd, false);
}
#endif

