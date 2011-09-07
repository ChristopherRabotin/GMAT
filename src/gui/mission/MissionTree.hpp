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
   
   wxArrayString mCommandList;
   wxArrayString mCommandListForViewControl;
   wxWindow *mParent;
   
   wxTreeItemId mMissionSeqTopId;
   wxTreeItemId mMissionSeqSubId;
   wxTreeItemId mFiniteBurnTreeId;
   wxTreeItemId mNewTreeId;
   
   wxPoint      mLastClickPoint;
   
   int mTempCounter;
   int mNumMissionSeq;
   int mNumPropagate;
   int mNumManeuver;
   int mNumTarget;
   int mNumOptimize;
   int mNumAchieve;
   int mNumVary;
   int mNumSave;
   int mNumToggle;
   int mNumReport;
   int mNumIfStatement;
   int mNumWhileLoop;
   int mNumForLoop;
   int mNumDoWhile;
   int mNumSwitchCase;
   int mNumFunct;
   int mNumAssign;
   int mNumFiniteBurn;
   int mNumScriptEvent;
   int mNumStop;
   int mNumMinimize;
   int mNumNonlinearConstraint;
   
   bool inScriptEvent;
   bool inFiniteBurn;
   bool mShowDetailedItem;
   int  mScriptEventCount;
   
   void InitializeCounter();
   GmatCommand* CreateCommand(const wxString &cmdName);
   void UpdateCommand();
   
   bool IsAnyViewCommandInBranch(GmatCommand *branch);
   void ShowEllipsisInPreviousNode(wxTreeItemId parent, wxTreeItemId node);
   wxTreeItemId BuildTreeItem(wxTreeItemId parent, GmatCommand *cmd,
                              Integer level, bool &isPrevItemHidden);
   
   wxTreeItemId& UpdateCommandTree(wxTreeItemId parent, GmatCommand *cmd,
                                   Integer level);
   void ExpandNode(wxTreeItemId node, const wxString &cmdType);
   void ExpandChildCommand(wxTreeItemId parent, GmatCommand *baseCmd,
                           GmatCommand *cmd);
   void ExpandChildCommand(wxTreeItemId parent, GmatCommand *cmd, Integer level);
   wxTreeItemId AppendCommand(wxTreeItemId parent, GmatTree::MissionIconType icon,
                              GmatTree::ItemType type, GmatCommand *cmd,
                              int *cmdCount, int endCount = 0);
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
                              GmatCommand *prevCmd, GmatCommand *cmd, int *cmdCount,
                              bool insertBefore);
   
   void Append(const wxString &cmdName);
   void DeleteCommand(const wxString &cmdName);
   void InsertBefore(const wxString &cmdName);
   void InsertAfter(const wxString &cmdName);
   void UpdateGuiManager(const wxString &cmdName);
   
   void AddDefaultMissionSeq(wxTreeItemId universe);
   void AddIcons();
   
   // event handlers
   void OnItemRightClick(wxTreeEvent& event);
   void OnItemActivated(wxTreeEvent &event);
   
   void OnDoubleClick(wxMouseEvent &event);
   
   void ShowMenu(wxTreeItemId id, const wxPoint& pt);
   bool CheckClickIn(wxPoint position);
   
   void OnAddMissionSeq(wxCommandEvent &event);
   
   void OnAppend(wxCommandEvent &event);
   void OnInsertBefore(wxCommandEvent &event);
   void OnInsertAfter(wxCommandEvent &event);
   
   void OnAutoAppend(wxCommandEvent &event);
   void OnAutoInsertBefore(wxCommandEvent &event);
   void OnAutoInsertAfter(wxCommandEvent &event);
   
   void OnBeginEditLabel(wxTreeEvent& event);
   void OnEndEditLabel(wxTreeEvent& event);
   void OnRename(wxCommandEvent &event);
   void OnDelete(wxCommandEvent &event);
   
   void OnRun(wxCommandEvent &event);
   void OnShowDetail(wxCommandEvent &event);
   void OnShowScript(wxCommandEvent &event);
   void OnShowCommandSummary(wxCommandEvent &event);
   void OnShowMissionSummaryAll(wxCommandEvent &event);
   void OnShowMissionSummaryPhysics(wxCommandEvent &event);
   void OnDockUndockMissionTree(wxCommandEvent &event);
   
   void OnCollapse(wxCommandEvent &event);
   void OnExpand(wxCommandEvent &event);
   
   void OnOpen(wxCommandEvent &event);
   void OnClose(wxCommandEvent &event);
   
   wxMenu* CreatePopupMenu(int type, ActionType action);
   wxMenu* CreateTargetPopupMenu(int type, ActionType action);
   wxMenu* CreateOptimizePopupMenu(int type, ActionType action);
   wxMenu* AppendTargetPopupMenu(wxMenu *menu, ActionType action);
   wxMenu* AppendOptimizePopupMenu(wxMenu *menu, ActionType action);
   wxMenu* CreateControlLogicPopupMenu(int type, ActionType action);
   
   wxString GetCommandString(GmatCommand *cmd, const wxString &currStr);
   GmatTree::ItemType GetCommandId(const wxString &cmd);
   int* GetCommandCounter(const wxString &cmd);
   
   void CreateCommandIdMap();
   void CreateMenuIds(const wxString &cmd, int index);
   int  GetMenuId(const wxString &cmd, ActionType action);
   
   GmatTree::MissionIconType GetIconId(const wxString &cmd);
   wxTreeItemId FindChild(wxTreeItemId parentId, const wxString &cmd);
   bool IsInsideSolver(wxTreeItemId itemId, GmatTree::ItemType &itemType);
   
   // for Debug
   void ShowCommands(const wxString &msg = "");
   void ShowSubCommands(GmatCommand* brCmd, Integer level);
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
      POPUP_SWAP_BEFORE = 1000,
      POPUP_SWAP_AFTER,
      POPUP_CUT,
      POPUP_COPY,
      POPUP_PASTE,
      POPUP_DELETE,
      POPUP_RENAME,
      POPUP_OPEN,
      POPUP_CLOSE,
      
      POPUP_CONTROL_LOGIC,
      
      POPUP_ADD_MISSION_SEQ,
      
      POPUP_COLLAPSE,
      POPUP_EXPAND,
      
      POPUP_APPEND,
      POPUP_INSERT_BEFORE,
      POPUP_INSERT_AFTER,
      
      //----- begin of MENU_EVT_RANGE of OnAppend()
      POPUP_APPEND_COMMAND = 2000,
      POPUP_APPEND_PROPAGATE,
      POPUP_APPEND_MANEUVER,
      POPUP_APPEND_BEGIN_FINITE_BURN,
      POPUP_APPEND_END_FINITE_BURN,
      POPUP_APPEND_TARGET,
      POPUP_APPEND_OPTIMIZE,
      POPUP_APPEND_VARY,
      POPUP_APPEND_ACHIEVE,
      POPUP_APPEND_MINIMIZE,
      POPUP_APPEND_NON_LINEAR_CONSTRAINT,
      POPUP_APPEND_REPORT,
      POPUP_APPEND_CALL_GMAT_FUNCTION,
      POPUP_APPEND_CALL_MATLAB_FUNCTION,
      POPUP_APPEND_ASSIGNMENT,
      POPUP_APPEND_TOGGLE,
      POPUP_APPEND_SAVE,
      POPUP_APPEND_STOP,
      POPUP_APPEND_SCRIPT_EVENT,
      
      POPUP_APPEND_IF,
      POPUP_APPEND_IF_ELSE,
      POPUP_APPEND_ELSE,
      POPUP_APPEND_ELSE_IF,
      POPUP_APPEND_FOR,
      POPUP_APPEND_WHILE,
      POPUP_APPEND_D0_WHILE,
      POPUP_APPEND_SWITCH,
      POPUP_APPEND_UNKNOWN,
      //----- end of MENU_EVT_RANGE
      
      //----- begin of MENU_EVT_RANGE of OnInsertBefore()
      POPUP_INSERT_BEFORE_COMMAND = 3000,
      POPUP_INSERT_BEFORE_PROPAGATE, 
      POPUP_INSERT_BEFORE_MANEUVER,
      POPUP_INSERT_BEFORE_BEGIN_FINITE_BURN,
      POPUP_INSERT_BEFORE_END_FINITE_BURN,
      POPUP_INSERT_BEFORE_TARGET,
      POPUP_INSERT_BEFORE_OPTIMIZE,
      POPUP_INSERT_BEFORE_VARY,
      POPUP_INSERT_BEFORE_ACHIEVE,
      POPUP_INSERT_BEFORE_MINIMIZE,
      POPUP_INSERT_BEFORE_NON_LINEAR_CONSTRAINT,
      POPUP_INSERT_BEFORE_REPORT,
      POPUP_INSERT_BEFORE_CALL_GMAT_FUNCTION,
      POPUP_INSERT_BEFORE_CALL_MATLAB_FUNCTION,
      POPUP_INSERT_BEFORE_ASSIGNMENT,
      POPUP_INSERT_BEFORE_TOGGLE,
      POPUP_INSERT_BEFORE_SAVE,
      POPUP_INSERT_BEFORE_STOP,
      POPUP_INSERT_BEFORE_SCRIPT_EVENT,
      
      POPUP_INSERT_BEFORE_IF,
      POPUP_INSERT_BEFORE_IF_ELSE,
      POPUP_INSERT_BEFORE_ELSE,
      POPUP_INSERT_BEFORE_ELSE_IF,
      POPUP_INSERT_BEFORE_FOR,
      POPUP_INSERT_BEFORE_WHILE,
      POPUP_INSERT_BEFORE_D0_WHILE,
      POPUP_INSERT_BEFORE_SWITCH,
      POPUP_INSERT_BEFORE_UNKNOWN,
      //----- end of MENU_EVT_RANGE
      
      //----- begin of MENU_EVT_RANGE of OnInsertAfter()
      POPUP_INSERT_AFTER_COMMAND = 4000,
      POPUP_INSERT_AFTER_PROPAGATE, 
      POPUP_INSERT_AFTER_MANEUVER,
      POPUP_INSERT_AFTER_BEGIN_FINITE_BURN,
      POPUP_INSERT_AFTER_END_FINITE_BURN,
      POPUP_INSERT_AFTER_TARGET,
      POPUP_INSERT_AFTER_OPTIMIZE,
      POPUP_INSERT_AFTER_VARY,
      POPUP_INSERT_AFTER_ACHIEVE,
      POPUP_INSERT_AFTER_MINIMIZE,
      POPUP_INSERT_AFTER_NON_LINEAR_CONSTRAINT,
      POPUP_INSERT_AFTER_REPORT,
      POPUP_INSERT_AFTER_CALL_GMAT_FUNCTION,
      POPUP_INSERT_AFTER_CALL_MATLAB_FUNCTION,
      POPUP_INSERT_AFTER_ASSIGNMENT,
      POPUP_INSERT_AFTER_TOGGLE,
      POPUP_INSERT_AFTER_SAVE,
      POPUP_INSERT_AFTER_STOP,
      POPUP_INSERT_AFTER_SCRIPT_EVENT,
      
      POPUP_INSERT_AFTER_IF,
      POPUP_INSERT_AFTER_IF_ELSE,
      POPUP_INSERT_AFTER_ELSE,
      POPUP_INSERT_AFTER_ELSE_IF,
      POPUP_INSERT_AFTER_FOR,
      POPUP_INSERT_AFTER_WHILE,
      POPUP_INSERT_AFTER_D0_WHILE,
      POPUP_INSERT_AFTER_SWITCH,
      POPUP_INSERT_AFTER_UNKNOWN,
      //----- end of MENU_EVT_RANGE
      
      POPUP_VIEW_VARIABLES,
      POPUP_VIEW_GOALS, 
      
      POPUP_RUN,
      POPUP_SHOW_DETAIL,
      POPUP_SHOW_SCRIPT,
      POPUP_COMMAND_SUMMARY,
      POPUP_MISSION_SUMMARY_ALL,
      POPUP_MISSION_SUMMARY_PHYSICS,
      POPUP_DOCK_MISSION_TREE,
      POPUP_UNDOCK_MISSION_TREE,
      
      //----- for auto testing actions
      POPUP_START_SAVE_ACTIONS,
      POPUP_STOP_SAVE_ACTIONS,
      POPUP_READ_ACTIONS,
      
      //----- for auto menu actions
      AUTO_APPEND_COMMAND = 5000,
      AUTO_INSERT_BEFORE_COMMAND = 6000,
      AUTO_INSERT_AFTER_COMMAND = 7000,
      AUTO_END = 8000,
   };
   
   std::map<wxString, int> cmdIdMap;
   std::map<int, wxString> idCmdMap;
};

#endif // MissionTree_hpp
