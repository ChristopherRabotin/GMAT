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
#ifndef MissionTree_hpp
#define MissionTree_hpp

#include "wx/treectrl.h"
#include "wx/image.h"
#include "wx/imaglist.h"

#include "MissionTreeItemData.hpp"
#include "DecoratedTree.hpp"
#include "GuiItemManager.hpp"
#include "GuiInterpreter.hpp"

class MissionTree : public DecoratedTree
{
public:
   // constructors
   MissionTree(wxWindow *parent, const wxWindowID id,
               const wxPoint& pos, const wxSize& size,
               long style);
   // void SetMainNotebook (GmatMainNotebook *mainNotebook);
   // GmatMainNotebook *GetMainNotebook();

   void ClearMission();
   void UpdateMission(bool resetCounter);
   
protected:
   
private:

   enum ActionType
   {
      APPEND, INSERT_BEFORE, INSERT_AFTER
   };
   
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   
   // GmatMainNotebook *mainNotebook;
   wxArrayString mCommandList;
   wxWindow *parent;
   
   wxTreeItemId mMissionSeqTopItem;
   wxTreeItemId mMissionSeqSubItem;
   wxTreeItemId mFiniteBurnTreeId;
   wxTreeItemId mNewTreeId;
   
   bool before;
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
   int  mScriptEventCount;
   
   void InitializeCounter();
   void UpdateCommand();
   wxTreeItemId& UpdateCommandTree(wxTreeItemId parent, GmatCommand *cmd);
   void ExpandChildCommand(wxTreeItemId parent, GmatCommand *baseCmd,
                           GmatCommand *cmd);
   void ExpandChildCommand(wxTreeItemId parent, GmatCommand *cmd, Integer level);
   wxTreeItemId AppendCommand(wxTreeItemId parent, GmatTree::MissionIconType icon,
                              GmatTree::ItemType type, GmatCommand *cmd,
                              int *cmdCount, int endCount = 0);
   wxTreeItemId InsertCommand(wxTreeItemId parentId, wxTreeItemId currId,
                              wxTreeItemId prevId, GmatTree::MissionIconType icon,
                              GmatTree::ItemType type, const wxString &cmdName,
                              GmatCommand *prevCmd, GmatCommand *cmd, int *cmdCount,
                              bool insertBefore);
   
   void Append(const wxString &cmdName);
   void InsertBefore(const wxString &cmdName);
   void InsertAfter(const wxString &cmdName);
   void UpdateGuiManager(const wxString &cmdName);
   
   void AddDefaultMission();
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
      
   void OnDelete(wxCommandEvent &event);
   void OnRun(wxCommandEvent &event);
   void OnShowScript(wxCommandEvent &event);
   
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
   
   int GetMenuId(const wxString &cmd, ActionType action);
   GmatTree::MissionIconType GetIconId(const wxString &cmd);
   GmatTree::ItemType GetCommandId(const wxString &cmd);
   int* GetCommandCounter(const wxString &cmd);
   wxTreeItemId FindChild(wxTreeItemId parentId, const wxString &cmd);
   
   // for Debug
   void ShowCommands(const wxString &msg = "");
   void ShowSubCommands(GmatCommand* brCmd, Integer level);
   
   DECLARE_EVENT_TABLE();
   
   enum
   {
      POPUP_SWAP_BEFORE = 25000,
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
      POPUP_APPEND_FUNCTION,
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
      //----- end of MENU_EVT_RANGE
      
      //----- begin of MENU_EVT_RANGE of OnInsertBefore()
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
      POPUP_INSERT_BEFORE_FUNCTION,
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
      //----- end of MENU_EVT_RANGE
      
      //----- begin of MENU_EVT_RANGE of OnInsertAfter()
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
      POPUP_INSERT_AFTER_FUNCTION,
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
      //----- end of MENU_EVT_RANGE
      
      POPUP_VIEW_VARIABLES,
      POPUP_VIEW_GOALS, 
      
      POPUP_RUN,
      POPUP_SHOW_SCRIPT,
      
   };
};

#endif // MissionTree_hpp
