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

#include "GmatMainNotebook.hpp"
#include "GuiInterpreter.hpp"

class ResourceTree : public wxTreeCtrl
{
public:

    // constructors
    ResourceTree(wxWindow *parent, const wxWindowID id,
                 const wxPoint& pos, const wxSize& size,
                 long style);
//    void SetMainNotebook (GmatMainNotebook *mainNotebook);
//    GmatMainNotebook *GetMainNotebook();

    void UpdateResources();
    
private:
    GuiInterpreter *theGuiInterpreter;
    GmatMainNotebook *mainNotebook;
    wxWindow *parent;
    wxTreeItemId mSpacecraftItem;
    wxTreeItemId mPropagatorItem;
    wxTreeItemId mReportItem;
    wxTreeItemId mBurnItem;
    wxTreeItemId mSubscriberItem;
    wxTreeItemId mSolverItem;

    int mNumSpacecraft;
    int mNumPropagator;
    int mNumImpulsiveBurn;
    int mNumDiffCorr;
    int mNumReportFile;
    int mNumXyPlot;
    int mNumOpenGlPlot;
    
    // event handlers
    void AddDefaultResources();
    void AddDefaultBodies(wxTreeItemId universe);
    void AddDefaultSpacecraft(wxTreeItemId spacecraft);
    void AddDefaultFormations(wxTreeItemId formation);
    void AddDefaultConstellations(wxTreeItemId constellation);
    void AddDefaultPropagators(wxTreeItemId propagator);
    void AddDefaultSolvers(wxTreeItemId solver);
    void AddDefaultSubscribers(wxTreeItemId subscriber);
    void AddDefaultInterfaces(wxTreeItemId interfaceTree);
    void AddIcons();
    void OnItemRightClick(wxTreeEvent& event);
    void OnItemActivated(wxTreeEvent &event);
    void ShowMenu(wxTreeItemId id, const wxPoint& pt);
    void OnEndLabelEdit(wxTreeEvent &event);
    void OnBeginLabelEdit(wxTreeEvent &event);
    void OnOpen(wxCommandEvent &event);
    void OnClose(wxCommandEvent &event);
    void OnDelete(wxCommandEvent &event);
    void OnRename(wxCommandEvent &event);
    
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
        
        POPUP_OPEN,
        POPUP_CLOSE,
        POPUP_RENAME,
        POPUP_DELETE
    };


};

#endif // ResourceTree_hpp
