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

#include "gmatwxdefs.hpp"

#include "wx/treectrl.h"
#include "wx/image.h"
#include "wx/imaglist.h"

#include "DecoratedTree.hpp"
#include "GuiInterpreter.hpp"
#include "GmatMainNotebook.hpp"

class MissionTree : public DecoratedTree
{
public:
   // constructors
   MissionTree(wxWindow *parent, const wxWindowID id,
               const wxPoint& pos, const wxSize& size,
               long style);
   void SetMainNotebook (GmatMainNotebook *mainNotebook);
   GmatMainNotebook *GetMainNotebook();

protected:

private:
   GuiInterpreter *theGuiInterpreter;
   GmatMainNotebook *mainNotebook;
   wxWindow *parent;
   bool before;
   int numManeuver;
   
   void AddDefaultMission();
   void AddIcons();

   // event handlers
   DECLARE_EVENT_TABLE();
   void OnItemRightClick(wxTreeEvent& event);
   void OnItemActivated(wxTreeEvent &event);
   void ShowMenu(wxTreeItemId id, const wxPoint& pt);
   void OnBefore();
   void OnAfter();
   void OnAddBeforeManeuver(wxCommandEvent &event);
   void OnAddBeforePropagate(wxCommandEvent &event);
   void OnAddBeforeTarget(wxCommandEvent &event);
   void OnAddAfterManeuver(wxCommandEvent &event);
   void OnAddAfterPropagate(wxCommandEvent &event);
   void OnAddAfterTarget(wxCommandEvent &event);
   void AddManeuver();

   enum
   {
        ICON_FILE
   };
   
   enum
   {
      POPUP_SWAP_BEFORE = 4000,
      POPUP_SWAP_AFTER,
      POPUP_CUT,
      POPUP_COPY,
      POPUP_PASTE,
      POPUP_DELETE,
      
      POPUP_ADD_BEFORE,
      POPUP_ADD_AFTER,
   
      POPUP_ADD_MANEUVER_BEFORE,
      POPUP_ADD_PROPAGATE_BEFORE,
      POPUP_ADD_TARGET_BEFORE,
      
      POPUP_ADD_MANEUVER_AFTER,
      POPUP_ADD_PROPAGATE_AFTER,
      POPUP_ADD_TARGET_AFTER
   };
};

#endif // MissionTree_hpp
