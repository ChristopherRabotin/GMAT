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

#include "GmatMainNotebook.hpp"
#include "GuiInterpreter.hpp"
#include "DecoratedTree.hpp"

class MissionTree : public DecoratedTree
{
public:
   // constructors
   MissionTree(wxWindow *parent, const wxWindowID id,
               const wxPoint& pos, const wxSize& size,
               long style);
//   void SetMainNotebook (GmatMainNotebook *mainNotebook);
//   GmatMainNotebook *GetMainNotebook();

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
      POPUP_SWAP_BEFORE = 25000,
      POPUP_SWAP_AFTER,
      POPUP_CUT,
      POPUP_COPY,
      POPUP_PASTE,
      POPUP_DELETE,
      
      POPUP_ADD_BEFORE,
      POPUP_ADD_AFTER,
   
      POPUP_ADD_BEFORE_MANEUVER,
      POPUP_ADD_BEFORE_PROPAGATE,
      POPUP_ADD_BEFORE_TARGET,
      
      POPUP_ADD_AFTER_MANEUVER,
      POPUP_ADD_AFTER_PROPAGATE,
      POPUP_ADD_AFTER_TARGET
   };
};

#endif // MissionTree_hpp
