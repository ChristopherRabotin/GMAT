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
   bool gridLines;

   GuiInterpreter *theGuiInterpreter;
   // GmatMainNotebook *mainNotebook;
   wxArrayString mCommandList;
   wxWindow *parent;
    
   wxTreeItemId mMissionSeqTopItem;
   wxTreeItemId mMissionSeqSubItem;
   wxTreeItemId mNewTreeId;
   
   bool before;
   int numManeuver;
   int mNumMissionSeq;
   int mNumPropagate;
   int mNumManeuver;
   int mNumTarget;
   int mNumAchieve;
   int mNumVary;
   int mNumSave;
   int mNumToggle;
   int mNumIfStatement;
   int mNumWhileLoop;
   int mNumForLoop;
   int mNumDoWhile;
   int mNumSwitchCase;
   int mNumFunct;
   int mNumAssign;

   void UpdateCommand();
   wxTreeItemId& UpdateCommandTree(wxTreeItemId parent, GmatCommand *cmd);
   void ExpandChildCommand(wxTreeItemId parent, GmatCommand *baseCmd,
                           GmatCommand *cmd);
   wxTreeItemId AppendCommand(wxTreeItemId parent, GmatTree::MissionIconType icon,
                              GmatTree::ItemType type, GmatCommand *cmd,
                              int *cmdCount, int endCount = 0);
   wxTreeItemId InsertCommand(wxTreeItemId parentId, wxTreeItemId currId,
                              wxTreeItemId prevId, GmatTree::MissionIconType icon,
                              GmatTree::ItemType type, GmatCommand *prevCmd,
                              GmatCommand *cmd, int *cmdCount);
   void AddDefaultMission();
   void AddDefaultMissionSeq(wxTreeItemId universe);
   void AddIcons();

   // event handlers
   void OnItemRightClick(wxTreeEvent& event);
   void OnItemActivated(wxTreeEvent &event);
   void OnDoubleClick(wxMouseEvent &event);
   void ShowMenu(wxTreeItemId id, const wxPoint& pt);
   void OnBefore();
   void OnAfter();
   bool CheckClickIn(wxPoint position);
    
   void OnAddMissionSeq(wxCommandEvent &event);
    
   void OnAddPropagate(wxCommandEvent &event);
   void OnAddManeuver(wxCommandEvent &event);
   void OnAddAchieve(wxCommandEvent &event);
   void OnAddVary(wxCommandEvent &event);
   void OnAddFunction(wxCommandEvent &event);
   void OnAddAssignment(wxCommandEvent &event);
   void OnAddTarget(wxCommandEvent &event);

   void OnAddIfStatement(wxCommandEvent &event);
   void OnAddWhileLoop(wxCommandEvent &event);
   void OnAddForLoop(wxCommandEvent &event);
   void OnAddDoWhile(wxCommandEvent &event);
   void OnAddSwitchCase(wxCommandEvent &event);
   void OnAddElseIfStatement(wxCommandEvent &event);
   void OnAddElseStatement(wxCommandEvent &event);
    
   void OnInsertPropagate(wxCommandEvent &event);
   void OnInsertManeuver(wxCommandEvent &event);
   void OnInsertAchieve(wxCommandEvent &event);
   void OnInsertVary(wxCommandEvent &event);
   void OnInsertFunction(wxCommandEvent &event);
   void OnInsertAssignment(wxCommandEvent &event);
   void OnInsertTarget(wxCommandEvent &event);

   void OnInsertIfStatement(wxCommandEvent &event);
   void OnInsertWhileLoop(wxCommandEvent &event);
   void OnInsertForLoop(wxCommandEvent &event);
   void OnInsertDoWhile(wxCommandEvent &event);
   void OnInsertSwitchCase(wxCommandEvent &event);
    
   void OnViewVariables();
   void OnViewGoals();

   void OnRun();  
    
   void OnDelete(); 
    
   void OnOpen();
   void OnClose();

   wxMenu* CreateAddPopupMenu();
   wxMenu* CreateInsertPopupMenu();
   wxMenu* CreateTargetPopupMenu(bool insert);
   wxMenu* AppendTargetPopupMenu(wxMenu *menu, bool insert);
   // wxMenu* CreateControlLogicPopupMenu();
   wxMenu* CreateAddControlLogicPopupMenu();
   wxMenu* CreateAddIfPopupMenu();
   wxMenu* CreateInsertControlLogicPopupMenu();
   int GetMenuId(const wxString &cmd, bool insert); //loj: 11/15/04 added
   
   // for Debug
   void ShowCommands(const wxString &msg = "");
   void ShowSubCommands(GmatCommand *baseCmd, GmatCommand *cmd, Integer level);
   
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
      
      POPUP_ADD_MISSION_SEQ,
      POPUP_ADD_COMMAND,
      POPUP_ADD_PROPAGATE,
      POPUP_ADD_MANEUVER,
      POPUP_ADD_TARGET,
      POPUP_ADD_VARY,
      POPUP_ADD_ACHIEVE,
      POPUP_ADD_FUNCTION,
      POPUP_ADD_ASSIGNMENT,
      
      POPUP_INSERT_COMMAND,
      POPUP_INSERT_PROPAGATE,
      POPUP_INSERT_MANEUVER,
      POPUP_INSERT_TARGET,
      POPUP_INSERT_VARY,
      POPUP_INSERT_ACHIEVE,
      POPUP_INSERT_FUNCTION,
      POPUP_INSERT_ASSIGNMENT,
      
      POPUP_VIEW_VARIABLES,
      POPUP_VIEW_GOALS, 
      
      POPUP_RUN,
      
      POPUP_CONTROL_LOGIC,
      POPUP_WHILE_CONTROL,
      POPUP_FOR_CONTROL,
      POPUP_DO_CONTROL,
      
      POPUP_ADD_IF_STATEMENT,
      POPUP_ADD_WHILE_LOOP,
      POPUP_ADD_FOR_LOOP,
      POPUP_ADD_D0_WHILE,
      POPUP_ADD_SWITCH_CASE,
      POPUP_ADD_ELSE_IF_STATEMENT,
      POPUP_ADD_ELSE_STATEMENT,
      
      POPUP_INSERT_IF_STATEMENT,
      POPUP_INSERT_WHILE_LOOP,
      POPUP_INSERT_FOR_LOOP,
      POPUP_INSERT_D0_WHILE,
      POPUP_INSERT_SWITCH_CASE,
   };
};

#endif // MissionTree_hpp
