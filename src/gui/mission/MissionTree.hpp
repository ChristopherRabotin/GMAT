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
   
   void UpdateMission(bool resetCounter);
   
protected:
   
private:

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
                              GmatCommand *prevCmd, GmatCommand *cmd, int *cmdCount);
   
   void AppendCommand(const wxString &cmdName);
   void InsertCommand(const wxString &cmdName);
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

   void OnAddCommand(wxCommandEvent &event);
   void OnInsertCommand(wxCommandEvent &event);
      
   void OnDelete(wxCommandEvent &event);
   void OnRun(wxCommandEvent &event);
   void OnShowScript(wxCommandEvent &event);
   
   void OnCollapse(wxCommandEvent &event);
   void OnExpand(wxCommandEvent &event);
   
   void OnOpen(wxCommandEvent &event);
   void OnClose(wxCommandEvent &event);
   
   wxMenu* CreatePopupMenu(int type, bool insert);
   wxMenu* CreateTargetPopupMenu(int type, bool insert);
   wxMenu* CreateOptimizePopupMenu(int type, bool insert);
   wxMenu* AppendTargetPopupMenu(wxMenu *menu, bool insert);
   wxMenu* AppendOptimizePopupMenu(wxMenu *menu, bool insert);
   wxMenu* CreateControlLogicPopupMenu(int type, bool insert);
   
   int GetMenuId(const wxString &cmd, bool insert);
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
      POPUP_ADD_COMMAND,
      
      //----- begin of MENU_EVT_RANGE of OnAddCommand()
      POPUP_ADD_PROPAGATE,
      POPUP_ADD_MANEUVER,
      POPUP_ADD_BEGIN_FINITE_BURN,
      POPUP_ADD_END_FINITE_BURN,
      POPUP_ADD_TARGET,
      POPUP_ADD_OPTIMIZE,
      POPUP_ADD_VARY,
      POPUP_ADD_ACHIEVE,
      POPUP_ADD_MINIMIZE,
      POPUP_ADD_NON_LINEAR_CONSTRAINT,
      POPUP_ADD_REPORT,
      POPUP_ADD_FUNCTION,
      POPUP_ADD_ASSIGNMENT,
      POPUP_ADD_TOGGLE,
      POPUP_ADD_SAVE,
      POPUP_ADD_STOP,
      POPUP_ADD_SCRIPT_EVENT,
      
      POPUP_ADD_IF_STATEMENT,
      POPUP_ADD_IF_ELSE_STATEMENT,
      POPUP_ADD_ELSE_STATEMENT,
      POPUP_ADD_ELSE_IF_STATEMENT,
      POPUP_ADD_FOR_LOOP,
      POPUP_ADD_WHILE_LOOP,
      POPUP_ADD_D0_WHILE,
      POPUP_ADD_SWITCH_CASE,
      //----- end of MENU_EVT_RANGE
      
      POPUP_INSERT_COMMAND,
      
      //----- begin of MENU_EVT_RANGE of OnInsertCommand()
      POPUP_INSERT_PROPAGATE, 
      POPUP_INSERT_MANEUVER,
      POPUP_INSERT_BEGIN_FINITE_BURN,
      POPUP_INSERT_END_FINITE_BURN,
      POPUP_INSERT_TARGET,
      POPUP_INSERT_OPTIMIZE,
      POPUP_INSERT_VARY,
      POPUP_INSERT_ACHIEVE,
      POPUP_INSERT_MINIMIZE,
      POPUP_INSERT_NON_LINEAR_CONSTRAINT,
      POPUP_INSERT_REPORT,
      POPUP_INSERT_FUNCTION,
      POPUP_INSERT_ASSIGNMENT,
      POPUP_INSERT_TOGGLE,
      POPUP_INSERT_SAVE,
      POPUP_INSERT_STOP,
      POPUP_INSERT_SCRIPT_EVENT,
      
      POPUP_INSERT_IF_STATEMENT,
      POPUP_INSERT_IF_ELSE_STATEMENT,
      POPUP_INSERT_ELSE_STATEMENT,
      POPUP_INSERT_ELSE_IF_STATEMENT,
      POPUP_INSERT_FOR_LOOP,
      POPUP_INSERT_WHILE_LOOP,
      POPUP_INSERT_D0_WHILE,
      POPUP_INSERT_SWITCH_CASE,
      //----- end of MENU_EVT_RANGE
      
      POPUP_VIEW_VARIABLES,
      POPUP_VIEW_GOALS, 
      
      POPUP_COLLAPSE,
      POPUP_EXPAND,
      POPUP_RUN,
      POPUP_SHOW_SCRIPT,
      
   };
};

#endif // MissionTree_hpp
