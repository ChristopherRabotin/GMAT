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

class ResourceTree : public wxTreeCtrl
{
public:

   // constructor
   ResourceTree(wxWindow *parent, const wxWindowID id,
                const wxPoint& pos, const wxSize& size,
                long style);
   void UpdateResource(bool resetCounter);
    
private:
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;

   //GmatMainNotebook *mainNotebook;
   //GmatMainFrame *mainFrame;

   //wxWindow *parent;
   wxTreeItemId mDraggedItem;
   wxTreeItemId mSpacecraftItem;
   wxTreeItemId mFormationItem;
   wxTreeItemId mPropagatorItem;
   wxTreeItemId mReportItem;
   wxTreeItemId mBurnItem;
   wxTreeItemId mSubscriberItem;
   wxTreeItemId mSolverItem;
   wxTreeItemId mVariableItem;
   wxTreeItemId mMatlabFunctItem;
   wxTreeItemId mCoordSysItem;
   wxTreeItemId mPredefinedFunctItem;
   wxTreeItemId specialPointsItem;

   int mNumSpacecraft;
   int mNumFormation;
   int mNumPropagator;
   int mNumImpulsiveBurn;
   int mNumDiffCorr;
   int mNumReportFile;
   int mNumXyPlot;
   int mNumOpenGlPlot;
   int mNumVariable;
   int mNumMatlabFunct;
   int mNumCoordSys;

   void AddDefaultResources();
   void AddDefaultBodies(wxTreeItemId itemId);
   void AddDefaultSpacecraft(wxTreeItemId itemId);
   void AddDefaultFormations(wxTreeItemId itemId);
   void AddDefaultConstellations(wxTreeItemId itemId);
   void AddDefaultPropagators(wxTreeItemId itemId);
   void AddDefaultBurns(wxTreeItemId itemId);
   void AddDefaultSolvers(wxTreeItemId itemId);
   void AddDefaultSubscribers(wxTreeItemId itemId);
   void AddDefaultInterfaces(wxTreeItemId itemId);
   void AddDefaultVariables(wxTreeItemId itemId);
   void AddDefaultMatlabFunctions(wxTreeItemId itemId);
   void AddDefaultCoordSys(wxTreeItemId itemId);

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
   void OnAddPropagator(wxCommandEvent &event);
   void OnAddConstellation(wxCommandEvent &event);
   void OnAddFormation(wxCommandEvent &event);
   void OnAddSpacecraft(wxCommandEvent &event);
   void OnAddReportFile(wxCommandEvent &event);
   void OnAddXyPlot(wxCommandEvent &event);
   void OnAddOpenGlPlot(wxCommandEvent &event);
   void OnAddDiffCorr(wxCommandEvent &event);
   void OnAddVariable(wxCommandEvent &event);
   void OnAddMatlabFunction(wxCommandEvent &event);
   void OnAddGmatFunction(wxCommandEvent &event);
   void OnAddCoordSys(wxCommandEvent &event);

   wxMenu* CreatePopupMenu(Gmat::ObjectType type);
    
   DECLARE_EVENT_TABLE();

   // for popup menu
   enum
   {
      POPUP_ADD_SC = 23000,
      POPUP_ADD_FORMATION,
      POPUP_ADD_CONSTELLATION,
        
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
   };
};

#endif // ResourceTree_hpp
