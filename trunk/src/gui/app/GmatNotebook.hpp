//$Header$
//------------------------------------------------------------------------------
//                             GmatNotebook
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Allison Greene
// Created: 2003/08/28
/**
 * This class provides the notebook for the left side of the main frame.
 */
//------------------------------------------------------------------------------
#ifndef GmatNotebook_hpp
#define GmatNotebook_hpp

#include "gmatwxdefs.hpp"

#include "ResourceTree.hpp"
#include "MissionTree.hpp"
#include "OutputTree.hpp"

#include "wx/notebook.h"

class GmatNotebook : public wxNotebook
{
public:
   // constructors
   GmatNotebook( wxWindow *parent, wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize, long style = 0 );
   //void SetMainNotebook (GmatMainNotebook *gmatMainNotebook);
   //GmatMainNotebook *GetMainNotebook();

protected:
private:
   wxWindow *parent;
   //GmatMainNotebook *mainNotebook;
   ResourceTree *resourceTree;
   MissionTree *missionTree;
   OutputTree *outputTree;

   wxPanel *CreateResourcePage();
   wxPanel *CreateMissionPage();
   wxPanel *CreateOutputPage();

   // event handlers
   void OnNotebookSelChange(wxNotebookEvent &event);
   void OnNotebookSelChanging(wxNotebookEvent &event);

   DECLARE_EVENT_TABLE();

};

#endif // GmatNotebook_hpp
