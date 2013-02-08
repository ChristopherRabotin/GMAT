//$Id$
//------------------------------------------------------------------------------
//                             GmatNotebook
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
#include "GmatStaticBoxSizer.hpp"
#include "MissionTreeToolBar.hpp"
#include "UndockedMissionPanel.hpp"
#include <wx/notebook.h>

class GmatNotebook : public wxNotebook
{
public:
   // constructors
   GmatNotebook( wxWindow *parent, wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize, long style = 0 );
   virtual ~GmatNotebook();
   
   MissionTreeToolBar* GetMissionTreeToolBar();
   void SetMissionTreeExpandLevel(int level);
   void CreateUndockedMissionPanel();
   void RestoreMissionPage();
   
protected:
private:
   wxWindow          *mParent;
   ResourceTree      *resourceTree;
   MissionTree       *missionTree;
   OutputTree        *outputTree;
   wxPanel           *mMissionPagePanel;
   UndockedMissionPanel *mUndockedMissionPanel;
   
   // toolbar for mission page
   MissionTreeToolBar *mMissionTreeToolBar;
   
   wxPanel *CreateResourcePage();
   wxPanel *CreateMissionPage();
   wxPanel *CreateOutputPage();
   
   // event handlers
   void OnNotebookSelChange(wxNotebookEvent &event);
   void OnNotebookSelChanging(wxNotebookEvent &event);
   void OnMouse(wxMouseEvent &event);
   
   DECLARE_EVENT_TABLE();
   
};

#endif // GmatNotebook_hpp
