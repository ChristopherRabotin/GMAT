//$Header$
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
   void UpdateResource(bool resetCounter);
   void OnAddScript(wxCommandEvent &event);
   void OnNewScript();
   void AddScriptItem(wxString path);
    
private:
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;

   //GmatMainNotebook *mainNotebook;
   //GmatMainFrame *mainFrame;

   //wxWindow *parent;
   wxTreeItemId mDraggedItem;
   wxTreeItemId mSpacecraftItem;
   wxTreeItemId mHardwareItem;
   wxTreeItemId mFormationItem;
   wxTreeItemId mPropagatorItem;
   wxTreeItemId mReportItem;
   wxTreeItemId mBurnItem;
   wxTreeItemId mSubscriberItem;
   wxTreeItemId mSolverItem;
   wxTreeItemId mVariableItem;
   wxTreeItemId mFunctItem;
   wxTreeItemId mCoordSysItem;
   wxTreeItemId mPredefinedFunctItem;
   wxTreeItemId mSpecialPointsItem;
   wxTreeItemId mScriptItem;
   wxTreeItemId mUniverseItem;
   wxTreeItemId mSampleScriptItem;

   int mNumSpacecraft;
   int mNumFuelTank;
   int mNumThruster;
   int mNumFormation;
   int mNumPropagator;
   int mNumImpulsiveBurn;
   int mNumFiniteBurn;
   int mNumDiffCorr;
   int mNumReportFile;
   int mNumXyPlot;
   int mNumOpenGlPlot;
   int mNumVariable;
   int mNumFunct;
   int mNumCoordSys;
   int mNumScripts;
   int mNumBarycenter;
   int mNumLibration;
   int mNumSampleScripts;

   void AddDefaultResources();
   void AddDefaultBodies(wxTreeItemId itemId);
   void AddDefaultSpacecraft(wxTreeItemId itemId);
   void AddDefaultHardware(wxTreeItemId itemId);
   void AddDefaultFormations(wxTreeItemId itemId);
   void AddDefaultConstellations(wxTreeItemId itemId);
   void AddDefaultPropagators(wxTreeItemId itemId);
   void AddDefaultBurns(wxTreeItemId itemId);
   void AddDefaultSolvers(wxTreeItemId itemId);
   void AddDefaultSubscribers(wxTreeItemId itemId);
   void AddDefaultInterfaces(wxTreeItemId itemId);
   void AddDefaultVariables(wxTreeItemId itemId);
   void AddDefaultFunctions(wxTreeItemId itemId);
   void AddDefaultCoordSys(wxTreeItemId itemId);
   void AddDefaultScripts(wxTreeItemId itemId);
   void AddDefaultSpecialPoints(wxTreeItemId itemId);
   void AddDefaultSampleScripts(wxTreeItemId itemId);

   // event handlers
   void OnItemRightClick(wxTreeEvent& event);
   void ShowMenu(wxTreeItemId id, const wxPoint& pt);
   void OnItemActivated(wxTreeEvent &event);
   void OnOpen(wxCommandEvent &event);
   void OnClose(wxCommandEvent &event);
   void OnRename(wxCommandEvent &event);
   void OnDelete(wxCommandEvent &event);
   void OnEndLabelEdit(wxTreeEvent &event);
   void OnBeginLabelEdit(wxTreeEvent &event);

   void OnBeginDrag(wxTreeEvent &event);
   //    void OnBeginRDrag(wxTreeEvent &event);
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
   void OnAddVariable(wxCommandEvent &event);
   void OnAddMatlabFunction(wxCommandEvent &event);
   void OnAddGmatFunction(wxCommandEvent &event);
   void OnAddCoordSys(wxCommandEvent &event);
   void OnAddBarycenter(wxCommandEvent &event);
   void OnAddLibration(wxCommandEvent &event);

//   void OnAddScript(wxCommandEvent &event);
//   void OnNewScript(wxCommandEvent &event);
   void OnRemoveAllScripts(wxCommandEvent &event);
   void OnRemoveScript(wxCommandEvent &event);

   wxMenu* CreatePopupMenu(Gmat::ObjectType type);
    
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
      POPUP_ADD_DIFF_CORR,
      
      POPUP_ADD_SUBSCRIBER,
      POPUP_ADD_REPORT_FILE,
      POPUP_ADD_XY_PLOT,
      POPUP_ADD_OPENGL_PLOT,
      POPUP_ADD_VARIABLE,
      
      POPUP_OPEN,
      POPUP_CLOSE,
      POPUP_RENAME,
      POPUP_DELETE,
      
      POPUP_ADD_MATLAB_FUNCT,
      POPUP_ADD_COORD_SYS,
      POPUP_ADD_GMAT_FUNCT,

      POPUP_ADD_SCRIPT,
//      POPUP_NEW_SCRIPT,
      POPUP_REMOVE_ALL_SCRIPTS,
      POPUP_REMOVE_SCRIPT,

      POPUP_ADD_SPECIAL_POINT,
      POPUP_ADD_BARYCENTER,
      POPUP_ADD_LIBRATION,
   };
};

#endif // ResourceTree_hpp
