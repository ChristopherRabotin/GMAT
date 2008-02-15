//$Id$
//------------------------------------------------------------------------------
//                              ResourceTree
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2003/08/28
//
/**
 * This class provides the resource tree and event handlers.
 */
//------------------------------------------------------------------------------
#ifndef ResourceTree_hpp
#define ResourceTree_hpp

#include "gmatwxdefs.hpp"
#include "wx/treectrl.h"
#include "wx/image.h"
#include "wx/imaglist.h"

#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "GmatTreeItemData.hpp"

class ResourceTree : public wxTreeCtrl
{
public:

   // constructor
   ResourceTree(wxWindow *parent, const wxWindowID id,
                const wxPoint& pos, const wxSize& size,
                long style);
   
   void ClearResource(bool leaveScripts);
   void UpdateResource(bool resetCounter);
   bool AddScriptItem(wxString path);
   void UpdateFormation();
   void UpdateVariable();
   void OnAddScript(wxCommandEvent &event);
   bool WasScriptAdded() { return mScriptAdded; }
   
private:
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   bool mScriptFolderRunning;
   bool mHasUserInterrupted;
   bool mScriptAdded;
   
   // for script error log
   int mBuildErrorCount;
   wxArrayString mFailedScriptsList;
   
   wxTreeItemId mDraggedItem;
   wxTreeItemId mSpacecraftItem;
   wxTreeItemId mHardwareItem;
   wxTreeItemId mFormationItem;
   wxTreeItemId mPropagatorItem;
   wxTreeItemId mReportItem;
   wxTreeItemId mBurnItem;
   wxTreeItemId mSubscriberItem;
   wxTreeItemId mSolverItem;
   wxTreeItemId mBoundarySolverItem;
   wxTreeItemId mOptimizerItem;
   wxTreeItemId mVariableItem;
   wxTreeItemId mFunctionItem;
   wxTreeItemId mCoordSysItem;
   wxTreeItemId mPredefinedFunctItem;
   wxTreeItemId mSpecialPointsItem;
   wxTreeItemId mScriptItem;
   wxTreeItemId mScriptFolderItem;
   wxTreeItemId mUniverseItem;
   
   // objects
   GmatBase* GetObject(const std::string &name);
   void UpdateGuiItem(GmatTree::ItemType itemType);
   
   // resource tree
   void AddNode(GmatTree::ItemType, const wxString &name);
   
   void AddDefaultResources();
   void AddDefaultBodies(wxTreeItemId itemId);
   void AddDefaultSpacecraft(wxTreeItemId itemId, bool resetCounter = true);
   void AddDefaultHardware(wxTreeItemId itemId, bool resetCounter = true);
   void AddDefaultFormations(wxTreeItemId itemId, bool resetCounter = true);
   void AddDefaultConstellations(wxTreeItemId itemId);
   void AddDefaultPropagators(wxTreeItemId itemId, bool resetCounter = true);
   void AddDefaultBurns(wxTreeItemId itemId, bool resetCounter = true);
   void AddDefaultSolvers(wxTreeItemId itemId, bool resetCounter = true);
   void AddDefaultSubscribers(wxTreeItemId itemId, bool resetCounter = true);
   void AddDefaultInterfaces(wxTreeItemId itemId);
   void AddDefaultVariables(wxTreeItemId itemId);
   void AddDefaultFunctions(wxTreeItemId itemId);
   void AddDefaultCoordSys(wxTreeItemId itemId);
   void AddDefaultScripts(wxTreeItemId itemId);
   void AddDefaultSpecialPoints(wxTreeItemId itemId, bool incLibCounter = true,
                                bool resetCounter = true);

   // event handlers
   void OnItemRightClick(wxTreeEvent& event);
   void OnItemActivated(wxTreeEvent &event);
   void OnOpen(wxCommandEvent &event);
   void OnClose(wxCommandEvent &event);
   void OnRename(wxCommandEvent &event);
   void OnDelete(wxCommandEvent &event);
   void OnClone(wxCommandEvent &event);
   void OnEndLabelEdit(wxTreeEvent &event);
   void OnBeginLabelEdit(wxTreeEvent &event);

   void OnBeginDrag(wxTreeEvent &event);
   //void OnBeginRDrag(wxTreeEvent &event);
   void OnEndDrag(wxTreeEvent &event);    
   
   void AddIcons();   
   void OnAddBody(wxCommandEvent &event);
   void OnAddImpulsiveBurn(wxCommandEvent &event);
   void OnAddFiniteBurn(wxCommandEvent &event);
   void OnAddPropagator(wxCommandEvent &event);
   void OnAddConstellation(wxCommandEvent &event);
   void OnAddFormation(wxCommandEvent &event);
   void OnAddSpacecraft(wxCommandEvent &event);
   void OnAddFuelTank(wxCommandEvent &event);
   void OnAddThruster(wxCommandEvent &event);
   void OnAddReportFile(wxCommandEvent &event);
   void OnAddXyPlot(wxCommandEvent &event);
   void OnAddOpenGlPlot(wxCommandEvent &event);
   void OnAddDiffCorr(wxCommandEvent &event);
   void OnAddSqp(wxCommandEvent &event);
   void OnAddVariable(wxCommandEvent &event);
   void OnAddArray(wxCommandEvent &event);
   void OnAddString(wxCommandEvent &event);
   void OnAddMatlabFunction(wxCommandEvent &event);
   void OnAddGmatFunction(wxCommandEvent &event);
   void OnAddCoordSys(wxCommandEvent &event);
   void OnAddBarycenter(wxCommandEvent &event);
   void OnAddLibration(wxCommandEvent &event);

   //void OnAddScript(wxCommandEvent &event);
   //void OnNewScript(wxCommandEvent &event);
   void OnRemoveAllScripts(wxCommandEvent &event);
   void OnRemoveScript(wxCommandEvent &event);
   void OnScriptBuildObject(wxCommandEvent& event);
   void OnScriptBuildAndRun(wxCommandEvent& event);
   
   // script folder
   void OnAddScriptFolder(wxCommandEvent &event);
   void OnRunScriptsFromFolder(wxCommandEvent& event);
   void OnQuitRunScriptsFromFolder(wxCommandEvent& event);
   void OnRemoveScriptFolder(wxCommandEvent& event);
   
   bool BuildScript(const wxString &filename, bool readBack = false,
                    const wxString &savePath = "", bool openScript = true,
                    bool multiScripts = false);
   
   // menu
   void ShowMenu(wxTreeItemId id, const wxPoint& pt);
   wxMenu* CreatePopupMenu(GmatTree::ItemType type);
   Gmat::ObjectType GetObjectType(GmatTree::ItemType itemType);
   wxTreeItemId GetTreeItemId(GmatTree::ItemType type);
   GmatTree::IconType GetTreeItemIcon(GmatTree::ItemType type);
   
   // compare
   void CompareScriptRunResult(Real absTol, const wxString &replaceStr,
                               const wxString &dir1, const wxString &dir2,
                               wxTextCtrl *textCtrl);
   
   DECLARE_EVENT_TABLE();

   // for popup menu
   enum
   {
      POPUP_ADD_SC = 23000,
      POPUP_ADD_FORMATION,
      POPUP_ADD_CONSTELLATION,
      
      POPUP_ADD_HARDWARE,
      POPUP_ADD_THRUSTER,
      POPUP_ADD_FUELTANK,
      
      POPUP_ADD_PROPAGATOR,
      POPUP_ADD_BODY,
      
      POPUP_ADD_BURN,
      POPUP_ADD_IMPULSIVE_BURN,
      POPUP_ADD_FINITE_BURN,
      
      POPUP_ADD_SOLVER,
      POPUP_ADD_BOUNDARY_SOLVER,
      POPUP_ADD_OPTIMIZER,
      POPUP_ADD_DIFF_CORR,
      POPUP_ADD_BROYDEN,
      POPUP_ADD_QUASI_NEWTON,
      POPUP_ADD_SQP,
      
      POPUP_ADD_SUBSCRIBER,
      POPUP_ADD_REPORT_FILE,
      POPUP_ADD_XY_PLOT,
      POPUP_ADD_OPENGL_PLOT,
      POPUP_ADD_VARIABLE,
      POPUP_ADD_ARRAY,
      POPUP_ADD_STRING,
      
      POPUP_ADD_COORD_SYS,
      
      POPUP_OPEN_MATLAB,
      POPUP_CLOSE_MATLAB,
      
      POPUP_START_SERVER,
      POPUP_STOP_SERVER,
      
      POPUP_ADD_FUNCTION,
      POPUP_ADD_MATLAB_FUNCT,
      POPUP_ADD_GMAT_FUNCT,
      
      POPUP_ADD_SCRIPT,
      POPUP_ADD_SCRIPT_FOLDER,
      POPUP_RUN_SCRIPTS_FROM_FOLDER,
      POPUP_QUIT_RUN_SCRIPTS_FROM_FOLDER,
      POPUP_REMOVE_SCRIPT_FOLDER,
      POPUP_BUILD_SCRIPT,
      POPUP_BUILD_AND_RUN_SCRIPT,
      POPUP_REMOVE_ALL_SCRIPTS,
      POPUP_REMOVE_SCRIPT,

      POPUP_ADD_SPECIAL_POINT,
      POPUP_ADD_BARYCENTER,
      POPUP_ADD_LIBRATION,

      POPUP_OPEN,
      POPUP_CLOSE,
      POPUP_RENAME,
      POPUP_DELETE,
      POPUP_CLONE,
   };
};

#endif // ResourceTree_hpp
