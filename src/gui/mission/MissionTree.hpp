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

#include "GmatMainNotebook.hpp"

class MissionTree : public wxTreeCtrl
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
   GmatMainNotebook *mainNotebook;
   wxWindow *parent;
   
   void AddDefaultMission();
   void AddIcons();

   // event handlers
   DECLARE_EVENT_TABLE();
   void OnItemRightClick(wxTreeEvent& event);
   void OnItemActivated(wxTreeEvent &event);
   void ShowMenu(wxTreeItemId id, const wxPoint& pt);

   enum
   {
        ICON_FILE
   };
};

#endif // MissionTree_hpp
