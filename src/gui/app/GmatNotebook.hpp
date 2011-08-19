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
#include <wx/notebook.h>
#include <wx/tglbtn.h>

class GmatNotebook : public wxNotebook
{
public:
   // constructors
   GmatNotebook( wxWindow *parent, wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize, long style = 0 );
   
   void SetMissionTreeExpandLevel(int level);
   void ResetMissionTreeTools();
   
protected:
private:
   wxWindow     *mParent;
   ResourceTree *resourceTree;
   MissionTree  *missionTree;
   OutputTree   *outputTree;
   wxPanel      *mMissionTreePanel;
   
   // toolbar for mission page
   wxToolBar    *mMissionToolBar;

   int     mLastLevelClicked;
   int     mLastIncCategoryClicked;
   int     mLastExcCategoryClicked;
   
   wxPanel *CreateResourcePage();
   wxPanel *CreateMissionPage();
   wxPanel *CreateOutputPage();
   
   void CreateMissionTreeToolBar();
   void ResetMissionTreeLevel();
   void ResetMissionTreeIncludeCategory();
   void ResetMissionTreeExcludeCategory();
   int  GetOnlyOneClicked(int category);
   
   // event handlers
   void OnNotebookSelChange(wxNotebookEvent &event);
   void OnNotebookSelChanging(wxNotebookEvent &event);
   void OnViewByLevelClicked(wxCommandEvent& event);
   void OnViewByCategory(wxCommandEvent& event);
   void OnCustomView(wxCommandEvent& event);
   //void OnViewByLevelReclicked(wxCommandEvent& event);
   
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {
      // 93500 is too high and it doesn't trigger tool events
      //ID_BUTTON = 93500, 
      ID_BUTTON = 14500,
      TOOL_LEVEL_ALL,
      TOOL_LEVEL_1,
      TOOL_LEVEL_2,
      TOOL_LEVEL_3,
      TOOL_INC_PHYSICS,
      TOOL_INC_SOLVER,
      TOOL_INC_SCRIPT,
      TOOL_INC_CONTROL,
      TOOL_EXC_REPORT,
      TOOL_EXC_EQUATION,
      TOOL_EXC_PLOT,
      TOOL_EXC_CALL,
      TOOL_CUSTOM_VIEW,
   };
};

#endif // GmatNotebook_hpp
