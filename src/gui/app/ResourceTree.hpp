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
    void SetMainNotebook (GmatMainNotebook *mainNotebook);
    GmatMainNotebook *GetMainNotebook();

    void UpdateResources();
    
private:
    GuiInterpreter *theGuiInterpreter;
    GmatMainNotebook *mainNotebook;
    wxWindow *parent;
    wxTreeItemId mSpacecraftItem;
    wxTreeItemId mPropagatorItem;
    
    void AddDefaultResources();
    void AddDefaultBodies(wxTreeItemId universe);
    void AddDefaultSpacecraft(wxTreeItemId spacecraft);
    void AddDefaultFormations(wxTreeItemId formation);
    void AddDefaultConstellations(wxTreeItemId constellation);
    void AddDefaultPropagators(wxTreeItemId propagator);
    void AddDefaultSolvers(wxTreeItemId solver);
    void AddDefaultPlots(wxTreeItemId plot);
    //loj: why interface causing parse error? changed to interfaceTree
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
    void OnAddPropagator(wxCommandEvent &event);
    void OnAddConstellation(wxCommandEvent &event);
    void OnAddFormation(wxCommandEvent &event);
    void OnAddSc(wxCommandEvent &event);

    DECLARE_EVENT_TABLE();
   
    enum
    {
        ICON_FILE,
        ICON_FOLDER,
        ICON_OPENFOLDER,
        ICON_SPACECRAFT,
        ICON_EARTH
    };

    // for popup menu
    enum
    {
        POPUP_ADD_SC,
        POPUP_ADD_FORMATION,
        POPUP_ADD_CONSTELLATION,
        POPUP_ADD_PROPAGATOR,
        POPUP_ADD_BODY,
        POPUP_OPEN,
        POPUP_CLOSE,
        POPUP_RENAME,
        POPUP_DELETE
    };


};

#endif // ResourceTree_hpp
