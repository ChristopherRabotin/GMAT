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
// Modified:
//    2011.12.13 Linda Jun
//       - Modified to automatically generate menu ids and command counter
/**
 * This class provides the tree for missions.
 */
//------------------------------------------------------------------------------
#ifndef MissionTree_hpp
#define MissionTree_hpp

#include "wx/treectrl.h"
#include "wx/image.h"
#include "wx/imaglist.h"

#include "MissionTreeItemData.hpp"
#include "DecoratedTree.hpp"
#include "GuiItemManager.hpp"
#include "GuiInterpreter.hpp"
#include "ViewTextDialog.hpp"

#include <map>

#ifdef __TEST_MISSION_TREE_ACTIONS__
#include <fstream>            // for ofstream for saving actions
#endif

class GmatMainFrame;
class GmatNotebook;

class MissionTree : public DecoratedTree
{
public:
   MissionTree(wxWindow *parent, const wxWindowID id, const wxPoint& pos,
               const wxSize& size, long style);
   
   void SetMainFrame(GmatMainFrame *gmf);
   void SetNotebook(GmatNotebook *notebook);
   void ClearMission();
   void AddDefaultMission();
   void UpdateMission(bool resetCounter, bool viewAll = true, bool collapse = false);
   void UpdateMissionForRename();
   void ChangeNodeLabel(const wxString &oldLabel);
   void SetViewAll(bool viewAll = true);
   void SetViewLevel(int level);
   void SetViewCommands(const wxArrayString &viewCmds);
   const wxArrayString& GetCommandList(bool forViewControl = false);
   
protected:
   
private:

   enum ActionType
   {
      APPEND, INSERT_BEFORE, INSERT_AFTER
   };
   
   GmatMainFrame  *theMainFrame;
   GmatNotebook   *theNotebook;
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   wxArrayString   mViewCommands;
   bool            mViewAll;
   bool            mUsingViewLevel;
   int             mViewLevel;
   
   // For debug or DEBUG_MISSION_TREE = ON in the startup file
   bool            mWriteMissionSeq;
   
   wxArrayString mCommandList;
   wxArrayString mCommandListForViewControl;
   wxWindow *mParent;
   
   wxTreeItemId mMissionSeqTopId;
   wxTreeItemId mMissionSeqSubId;
   wxTreeItemId mFiniteBurnTreeId;
   wxTreeItemId mNewTreeId;
   
   wxPoint      mLastClickPoint;
   
   bool inScriptEvent;
   bool inFiniteBurn;
   bool mShowDetailedItem;
   int  mScriptEventCount;
   int  mNumMissionSeq;
   
   void InitializeCounter();
   GmatCommand* CreateCommand(const wxString &cmdTypeName);
	GmatCommand* CreateEndCommand(const wxString &cmdTypeName, GmatTree::ItemType &endType);
   void UpdateCommand();
   
   bool IsAnyViewCommandInBranch(GmatCommand *branch);
   void ShowEllipsisInPreviousNode(wxTreeItemId parent, wxTreeItemId node);
   wxTreeItemId BuildTreeItem(wxTreeItemId parent, GmatCommand *cmd,
                              Integer level, bool &isPrevItemHidden);
   wxTreeItemId BuildTreeItemInBranch(wxTreeItemId parent, GmatCommand *branch,
                              Integer level, bool &isPrevItemHidden);
   
   wxTreeItemId& UpdateCommandTree(wxTreeItemId parent, GmatCommand *cmd,
                                   Integer level);
   void ExpandNode(wxTreeItemId node, const wxString &cmdType);
   void ExpandChildCommand(wxTreeItemId parent, GmatCommand *baseCmd,
                           GmatCommand *cmd);
   void ExpandChildCommand(wxTreeItemId parent, GmatCommand *cmd, Integer level);
   wxTreeItemId AppendCommand(wxTreeItemId parent, GmatTree::MissionIconType icon,
                              GmatTree::ItemType type, GmatCommand *cmd,
                              int cmdCount);
   wxTreeItemId InsertNodeToBranch(wxTreeItemId parentId, wxTreeItemId currId,
                              wxTreeItemId prevId, GmatTree::MissionIconType icon,
                              const wxString &nodeName, GmatTree::ItemType itemType,
                              GmatCommand *cmd, GmatCommand *currCmd, GmatCommand *prevCmd,
                              bool insertBefore);
   wxTreeItemId InsertNodeAfter(wxTreeItemId parentId, wxTreeItemId currId,
                              wxTreeItemId prevId, GmatTree::MissionIconType icon,
                              const wxString &nodeName, GmatTree::ItemType itemType,
                              GmatCommand *cmd, bool insertBefore);
   wxTreeItemId InsertCommand(wxTreeItemId parentId, wxTreeItemId currId,
                              wxTreeItemId prevId, GmatTree::MissionIconType icon,
                              GmatTree::ItemType type, const wxString &cmdName,
                              GmatCommand *prevCmd, GmatCommand *cmd, int cmdCount,
                              bool insertBefore);
   
   void Append(const wxString &cmdTypeName);
   void InsertBefore(const wxString &cmdTypeName);
   void InsertAfter(const wxString &cmdTypeName);
   void DeleteCommand(const wxString &cmdName);
   void UpdateGuiManager(const wxString &cmdTypeName);
   
   void AddDefaultMissionSeq(wxTreeItemId universe);
   void AddIcons();
   
   // event handlers
   void OnItemRightClick(wxTreeEvent& event);
   void OnItemActivated(wxTreeEvent &event);
   
   void OnDoubleClick(wxMouseEvent &event);
   
   void ShowMenu(wxTreeItemId id, const wxPoint& pt);
   bool CheckClickIn(wxPoint position);
   
   void OnAddMissionSeq(wxCommandEvent &event);
   void OnPopupAppend(wxCommandEvent &event);
   
   void OnAppend(wxCommandEvent &event);
   void OnInsertBefore(wxCommandEvent &event);
   void OnInsertAfter(wxCommandEvent &event);
   
   void OnBeginEditLabel(wxTreeEvent& event);
   void OnEndEditLabel(wxTreeEvent& event);
   void OnRename(wxCommandEvent &event);
   void OnDelete(wxCommandEvent &event);
   
   void OnRun(wxCommandEvent &event);
   void OnShowDetail(wxCommandEvent &event);
   void OnShowMissionSequence(wxCommandEvent &event);
   void OnShowScript(wxCommandEvent &event);
   void OnShowCommandSummary(wxCommandEvent &event);
   void OnShowMissionSummaryAll(wxCommandEvent &event);
   void OnShowMissionSummaryPhysics(wxCommandEvent &event);
   void OnDockUndockMissionTree(wxCommandEvent &event);
   
   void OnCollapse(wxCommandEvent &event);
   void OnExpand(wxCommandEvent &event);
   
   void OnOpen(wxCommandEvent &event);
   void OnClose(wxCommandEvent &event);
   
   wxMenu* CreateSubMenu(GmatTree::ItemType type, ActionType action);
   wxMenu* CreateTargetSubMenu(GmatTree::ItemType type, ActionType action);
   wxMenu* CreateOptimizeSubMenu(GmatTree::ItemType type, ActionType action);
   wxMenu* CreateControlLogicSubMenu(GmatTree::ItemType type, ActionType action);
   
   wxString GetCommandString(GmatCommand *cmd, const wxString &currStr);
   GmatTree::ItemType GetCommandId(const wxString &cmd);
   void ResetCommandCounter(const wxString &cmd, bool resetAll = false);
   int  GetCommandCounter(const wxString &cmd);
   
   void CreateCommandIdMap();
   void CreateCommandCounterMap();
   void CreateMenuIds(const wxString &cmd, int index);
   int  GetMenuId(const wxString &cmd, ActionType action);
   
   GmatTree::MissionIconType GetIconId(const wxString &cmd);
   wxTreeItemId FindChild(wxTreeItemId parentId, const wxString &cmd);
   wxTreeItemId FindElse(wxTreeItemId parentId);
   bool IsInsideSolver(wxTreeItemId itemId, GmatTree::ItemType &itemType);
   
   // for Debug
   void ShowCommands(const wxString &msg = "");
   void ShowSubCommands(GmatCommand* brCmd, Integer level);
   void WriteNode(int count, const std::string &prefix,
                  const std::string &title1, wxTreeItemId itemId1,
                  const std::string &title2 = "", wxTreeItemId itemId2 = 0,
                  const std::string &title3 = "", wxTreeItemId itemId3 = 0);
   void WriteCommand(const std::string &prefix,
                     const std::string &title1, GmatCommand *cmd1,
                     const std::string &title2 = "",
                     GmatCommand *cmd2 = NULL);
   
   //--------------------------------------------------
   // for auto-testing of MissionTree actions
   //--------------------------------------------------
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   bool mSaveActions;
   bool mPlaybackActions;
   std::string mActionsOutFile;
   std::string mResultsFile;
   std::ofstream mActionsOutStream;
   std::ofstream mResultsStream;
   std::ofstream mPlaybackResultsStream;
   
   void OnStartSaveActions(wxCommandEvent &event);
   void OnStopSaveActions(wxCommandEvent &event);
   void OnPlaybackActions(wxCommandEvent &event);
   
   void WriteActions(const wxString &str);
   void WriteResults();
   #endif
   //--------------------------------------------------
   
   
   DECLARE_EVENT_TABLE();
   
   enum
   {
      MT_RUN = 100,
      MT_DELETE,
      MT_RENAME,
      MT_OPEN,
      MT_CLOSE,
      MT_COLLAPSE,
      MT_EXPAND,
      
      MT_SHOW_DETAIL,
      MT_SHOW_MISSION_SEQUENCE,
      MT_SHOW_SCRIPT,
      MT_COMMAND_SUMMARY,
      MT_MISSION_SUMMARY_ALL,
      MT_MISSION_SUMMARY_PHYSICS,
      MT_DOCK_MISSION_TREE,
      MT_UNDOCK_MISSION_TREE,
      
      //----- for auto testing actions
      MT_START_SAVE_ACTIONS = 200,
      MT_STOP_SAVE_ACTIONS,
      MT_READ_ACTIONS,
      
      //----- for mission sequence actions
      MT_ADD_MISSION_SEQ = 300,
      
      //----- for intermediate id for showing sub menus
      MT_CONTROL_LOGIC = 400,
      MT_APPEND,
      MT_INSERT_BEFORE,
      MT_INSERT_AFTER,
      
      //----- for command action menus
      MT_BEGIN_APPEND = 1000,
      MT_END_APPEND = 1999,
      MT_BEGIN_INSERT_BEFORE = 2000,
      MT_END_INSERT_BEFORE = 2999,
      MT_BEGIN_INSERT_AFTER = 3000,
      MT_END_INSERT_AFTER = 3999
   };
   
   std::map<wxString, int> cmdCounterMap;
   std::map<wxString, int> cmdIdMap;
   std::map<int, wxString> idCmdMap;
};

#endif // MissionTree_hpp
