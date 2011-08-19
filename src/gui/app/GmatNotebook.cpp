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
#include "GmatNotebook.hpp"
#include "GmatAppData.hpp"
#include "TreeViewOptionDialog.hpp"
#include "MessageInterface.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "wx/collpane.h"           // for wxCollapsiblePane

#include "bitmaps/mtc_LA.xpm"
#include "bitmaps/mtc_L1.xpm"
#include "bitmaps/mtc_L2.xpm"
#include "bitmaps/mtc_L3.xpm"
#include "bitmaps/mtc_IncPhysics.xpm"
#include "bitmaps/mtc_IncSolver.xpm"
#include "bitmaps/mtc_IncScriptEvent.xpm"
#include "bitmaps/mtc_IncControlFlow.xpm"
#include "bitmaps/mtc_ExcReport.xpm"
#include "bitmaps/mtc_ExcEquation.xpm"
#include "bitmaps/mtc_ExcPlot.xpm"
#include "bitmaps/mtc_ExcCall.xpm"
#include "bitmaps/mtc_CustomView.xpm"

#define DEBUG_NOTEBOOK 0

//------------------------------
// event tables for wxWindows
//------------------------------

//------------------------------------------------------------------------------
// EVENT_TABLE(GmatNotebook, wxNotebook)
//------------------------------------------------------------------------------
/**
 * Events Table for the menu and tool bar
 *
 * @note Indexes event handler functions.
 */
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(GmatNotebook, wxNotebook)
   EVT_NOTEBOOK_PAGE_CHANGED (-1, GmatNotebook::OnNotebookSelChange)
   //EVT_NOTEBOOK_PAGE_CHANGING(-1, GmatNotebook::OnNotebookSelChanging)
   EVT_TOOL_RANGE (TOOL_LEVEL_ALL, TOOL_LEVEL_3, GmatNotebook::OnViewByLevelClicked)
   EVT_TOOL_RANGE (TOOL_INC_PHYSICS, TOOL_EXC_CALL, GmatNotebook::OnViewByCategory)
   EVT_TOOL (TOOL_CUSTOM_VIEW, GmatNotebook::OnCustomView)
   //EVT_TOOL_RCLICKED_RANGE(TOOL_LEVEL_ALL, TOOL_LEVEL_3, GmatNotebook::OnViewByLevelReclicked)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// GmatNotebook(wxWindow *parent, wxWindowID id,
//              const wxPoint &pos, const wxSize &size, long style)
//------------------------------------------------------------------------------
GmatNotebook::GmatNotebook(wxWindow *parent, wxWindowID id,
                           const wxPoint &pos, const wxSize &size, long style)
   : wxNotebook(parent, id, pos, size, style)
{
   mParent = parent;
   resourceTree = NULL;
   missionTree = NULL;
   outputTree = NULL;
   mMissionTreePanel = NULL;
   mMissionToolBar = NULL;
   wxPanel *panel = (wxPanel *)NULL;
   mLastLevelClicked = -1;
   mLastIncCategoryClicked = -1;
   mLastExcCategoryClicked = -1;
   
   // Create and add Resource, Mission, and Output Tabs
   panel = CreateResourcePage( );
   AddPage( panel, wxT("Resources") );
   
   panel = CreateMissionPage( );
   AddPage( panel, wxT("Mission") );
   
   panel = CreateOutputPage( );
   AddPage( panel, wxT("Output") );
   
}


//------------------------------------------------------------------------------
// void SetMissionTreeExpandLevel(int level)
//------------------------------------------------------------------------------
void GmatNotebook::SetMissionTreeExpandLevel(int level)
{
   if (mMissionToolBar)
   {
      ResetMissionTreeTools();
      
      if (level == 1)
         mMissionToolBar->ToggleTool(TOOL_LEVEL_1, true);
      else if (level == 2)
         mMissionToolBar->ToggleTool(TOOL_LEVEL_2, true);
      else if (level == 3)
         mMissionToolBar->ToggleTool(TOOL_LEVEL_3, true);
      else
         mMissionToolBar->ToggleTool(TOOL_LEVEL_ALL, true);
   }
}


//------------------------------------------------------------------------------
// void ResetMissionTreeTools()
//------------------------------------------------------------------------------
void GmatNotebook::ResetMissionTreeTools()
{
   ResetMissionTreeLevel();
   ResetMissionTreeIncludeCategory();
   ResetMissionTreeExcludeCategory();
}


//------------------------------------------------------------------------------
// void CreateMissionTreeToolBar()
//------------------------------------------------------------------------------
void GmatNotebook::CreateMissionTreeToolBar()
{
   // Load toolbar icons
   GuiItemManager *guiManager = GuiItemManager::GetInstance();
   const int NUM_ICONS = 13;
   wxBitmap* bitmaps[NUM_ICONS];
   int index = -1;
   long bitmapType = wxBITMAP_TYPE_PNG;
   
   // Do not change the order, this order is how it appears in the toolbar
   
   guiManager->LoadIcon("mtc_LA", bitmapType, &bitmaps[++index], mtc_LA_xpm);
   guiManager->LoadIcon("mtc_L1", bitmapType, &bitmaps[++index], mtc_L1_xpm);
   guiManager->LoadIcon("mtc_L2", bitmapType, &bitmaps[++index], mtc_L2_xpm);
   guiManager->LoadIcon("mtc_L3", bitmapType, &bitmaps[++index], mtc_L3_xpm);
   guiManager->LoadIcon("mtc_IncPhysics", bitmapType, &bitmaps[++index], mtc_IncPhysics_xpm);
   guiManager->LoadIcon("mtc_IncSolver", bitmapType, &bitmaps[++index], mtc_IncSolver_xpm);
   guiManager->LoadIcon("mtc_IncScriptEvent", bitmapType, &bitmaps[++index], mtc_IncScriptEvent_xpm);
   guiManager->LoadIcon("mtc_IncControlFlow", bitmapType, &bitmaps[++index], mtc_IncControlFlow_xpm);
   guiManager->LoadIcon("mtc_ExcReport", bitmapType, &bitmaps[++index], mtc_ExcReport_xpm);
   guiManager->LoadIcon("mtc_ExcEquation", bitmapType, &bitmaps[++index], mtc_ExcEquation_xpm);
   guiManager->LoadIcon("mtc_ExcPlot", bitmapType, &bitmaps[++index], mtc_ExcPlot_xpm);
   guiManager->LoadIcon("mtc_ExcCall", bitmapType, &bitmaps[++index], mtc_ExcCall_xpm);
   guiManager->LoadIcon("mtc_CustomView", bitmapType, &bitmaps[++index], mtc_CustomView_xpm);
   
   // Use wxTB_FLAT to show separator divider
   mMissionToolBar = new wxToolBar(mMissionTreePanel, -1, wxDefaultPosition, wxDefaultSize,
                                   wxTB_VERTICAL|wxTB_FLAT);
   
   index = -1;
   mMissionToolBar->AddCheckTool(TOOL_LEVEL_ALL, wxT("LevelA"), *bitmaps[++index],
                                 *bitmaps[index], wxT("Expand All Levels"));
   mMissionToolBar->AddCheckTool(TOOL_LEVEL_1, wxT("Level1"), *bitmaps[++index],
                                 *bitmaps[index], wxT("Expand One Level"));
   mMissionToolBar->AddCheckTool(TOOL_LEVEL_2, wxT("Level2"), *bitmaps[++index],
                                 *bitmaps[index], wxT("Expand Two Levels"));
   mMissionToolBar->AddCheckTool(TOOL_LEVEL_3, wxT("Level3"), *bitmaps[++index],
                                 *bitmaps[index], wxT("Expand Three Levels"));
   mMissionToolBar->AddSeparator();
   mMissionToolBar->AddCheckTool(TOOL_INC_PHYSICS, wxT("IncPhysics"), *bitmaps[++index],
                                 *bitmaps[index], wxT("Include Physics Related commands"));
   mMissionToolBar->AddCheckTool(TOOL_INC_SOLVER, wxT("IncSolver"), *bitmaps[++index],
                                 *bitmaps[index], wxT("Include Solver Related commands"));
   mMissionToolBar->AddCheckTool(TOOL_INC_SCRIPT, wxT("IncScript"), *bitmaps[++index],
                                 *bitmaps[index], wxT("Include ScriptEvent"));
   mMissionToolBar->AddCheckTool(TOOL_INC_CONTROL, wxT("IncControl"), *bitmaps[++index],
                                 *bitmaps[index], wxT("Include Control Flow"));
   mMissionToolBar->AddSeparator();
   mMissionToolBar->AddCheckTool(TOOL_EXC_REPORT, wxT("ExcReport"), *bitmaps[++index],
                                 *bitmaps[index], wxT("Exclude Report"));
   mMissionToolBar->AddCheckTool(TOOL_EXC_EQUATION, wxT("ExcEquation"), *bitmaps[++index],
                                 *bitmaps[index], wxT("Exclude Equation"));
   mMissionToolBar->AddCheckTool(TOOL_EXC_PLOT, wxT("ExcPlot"), *bitmaps[++index],
                                 *bitmaps[index], wxT("Exclude Toggle, PenUp/Down, ClearPlot, and MarkPoint commands"));
   mMissionToolBar->AddCheckTool(TOOL_EXC_CALL, wxT("ExcCall"), *bitmaps[++index],
                                 *bitmaps[index], wxT("Exclude CallFunction, Save commands"));
   mMissionToolBar->AddSeparator();
   mMissionToolBar->AddTool(TOOL_CUSTOM_VIEW, wxT("CustomizeView"), *bitmaps[++index],
                            wxT("Customize Mission Tree View"));
   mMissionToolBar->Realize();
   
   for (int i = 0; i < NUM_ICONS; i++)
      delete bitmaps[i];
}


//------------------------------------------------------------------------------
// void ResetMissionTreeLevel()
//------------------------------------------------------------------------------
void GmatNotebook::ResetMissionTreeLevel()
{
   mMissionToolBar->ToggleTool(TOOL_LEVEL_ALL, false);
   mMissionToolBar->ToggleTool(TOOL_LEVEL_1, false);
   mMissionToolBar->ToggleTool(TOOL_LEVEL_2, false);
   mMissionToolBar->ToggleTool(TOOL_LEVEL_3, false);
   mLastLevelClicked = -1;
}


//------------------------------------------------------------------------------
// void ResetMissionTreeIncludeCategory()
//------------------------------------------------------------------------------
void GmatNotebook::ResetMissionTreeIncludeCategory()
{
   mMissionToolBar->ToggleTool(TOOL_INC_PHYSICS, false);
   mMissionToolBar->ToggleTool(TOOL_INC_SOLVER, false);
   mMissionToolBar->ToggleTool(TOOL_INC_SCRIPT, false);
   mMissionToolBar->ToggleTool(TOOL_INC_CONTROL, false);
   mLastIncCategoryClicked = -1;
}


//------------------------------------------------------------------------------
// void ResetMissionTreeExcludeCategory()
//------------------------------------------------------------------------------
void GmatNotebook::ResetMissionTreeExcludeCategory()
{
   mMissionToolBar->ToggleTool(TOOL_EXC_REPORT, false);
   mMissionToolBar->ToggleTool(TOOL_EXC_EQUATION, false);
   mMissionToolBar->ToggleTool(TOOL_EXC_PLOT, false);
   mMissionToolBar->ToggleTool(TOOL_EXC_CALL, false);
   mLastExcCategoryClicked = -1;
}


//------------------------------------------------------------------------------
// int GetOnlyOneClicked(int category)
//------------------------------------------------------------------------------
int GmatNotebook::GetOnlyOneClicked(int category)
{
   int numClicked = 0;
   int onlyOneClicked = -1;
   
   if (category == 1)
   {
      if (mMissionToolBar->GetToolState(TOOL_INC_PHYSICS))
      {
         onlyOneClicked = TOOL_INC_PHYSICS;
         numClicked++;
      }
      if (mMissionToolBar->GetToolState(TOOL_INC_SOLVER))
      {
         onlyOneClicked = TOOL_INC_SOLVER;
         numClicked++;
      }
      if (mMissionToolBar->GetToolState(TOOL_INC_SCRIPT))
      {
         onlyOneClicked = TOOL_INC_SCRIPT;
         numClicked++;
      }
      if (mMissionToolBar->GetToolState(TOOL_INC_CONTROL))
      {
         onlyOneClicked = TOOL_INC_CONTROL;
         numClicked++;
      }
   }
   else
   {
      if (mMissionToolBar->GetToolState(TOOL_EXC_REPORT))
      {
         onlyOneClicked = TOOL_EXC_REPORT;
         numClicked++;
      }
      if (mMissionToolBar->GetToolState(TOOL_EXC_EQUATION))
      {
         onlyOneClicked = TOOL_EXC_EQUATION;
         numClicked++;
      }
      if (mMissionToolBar->GetToolState(TOOL_EXC_PLOT))
      {
         onlyOneClicked = TOOL_EXC_PLOT;
         numClicked++;
      }
      if (mMissionToolBar->GetToolState(TOOL_EXC_CALL))
      {
         onlyOneClicked = TOOL_EXC_CALL;
         numClicked++;
      }
   }
   
   #ifdef DEBUG_MISSION_VIEW_CONTROL
   MessageInterface::ShowMessage
      ("There are %d tools clicked, onlyOneClicked = %d\n", numClicked,
       onlyOneClicked);
   #endif
   
   if (numClicked == 1)
      return onlyOneClicked;
   else
      return -1;
}


//------------------------------------------------------------------------------
// void OnNoetbookSelChange(wxNotebookEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles notebook page change
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void GmatNotebook::OnNotebookSelChange(wxNotebookEvent &event)
{
   int sel = event.GetSelection();
   
#if DEBUG_NOTEBOOK
   MessageInterface::ShowMessage("GmatNotebook::OnNotebookSelChange sel=%d\n", sel);
#endif
   
   if (sel == 0)
   {
      resourceTree->UpdateResource(false); //loj: 6/29/04 added false
   }

   event.Skip(); // need this

//   GmatMainFrame *theMainFrame = GmatAppData::GetMainFrame();
//
//   if (theMainFrame != NULL)
//   {
//      theMainFrame->MinimizeChildren(sel);
//      theMainFrame->Cascade();
//   }
}

//------------------------------------------------------------------------------
// void OnNoetbookSelChanging(wxNotebookEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles notebook page change
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void GmatNotebook::OnNotebookSelChanging(wxNotebookEvent &event)
{
//   int sel = event.GetSelection();
//
//   GmatMainFrame *theMainFrame = GmatAppData::GetMainFrame();
//
//   if (theMainFrame != NULL)
//   {
//      theMainFrame->Cascade();
//      theMainFrame->MinimizeChildren(sel);
//   }
//   else
      event.Skip(); // need this
}


//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// wxPanel *CreateResourcePage()
//------------------------------------------------------------------------------
/**
 * Adds a tree with resource information to notebook.
 *
 * @return wxPanel object with resource information.
 */
//------------------------------------------------------------------------------
wxPanel *GmatNotebook::CreateResourcePage()
{
   wxGridSizer *sizer = new wxGridSizer( 1, 0, 0 );
   wxPanel *panel = new wxPanel(this);
   
   //We don't want to edit labels
   long style = wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT|
      wxSUNKEN_BORDER|wxTR_SINGLE|wxTR_FULL_ROW_HIGHLIGHT;
   
   resourceTree = new ResourceTree(panel, -1, wxDefaultPosition,
                                   wxDefaultSize, style);
   
   // set to GmatAppData
   GmatAppData::Instance()->SetResourceTree(resourceTree);
   
   sizer->Add( resourceTree, 0, wxGROW|wxALL, 0 );
   
   panel->SetAutoLayout( TRUE );
   panel->SetSizer( sizer );
   sizer->Fit( panel );
   sizer->SetSizeHints( panel );
   
   return panel;
}

//------------------------------------------------------------------------------
// wxPanel *CreateMissionPage()
//------------------------------------------------------------------------------
/**
 * Adds a tree with missions information to notebook.
 *
 * @return wxPanel object with missions information.
 */
//------------------------------------------------------------------------------
wxPanel *GmatNotebook::CreateMissionPage()
{
   //set the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   pConfig->SetPath(wxT("/GmatNotebook MissionPage"));
   
   mMissionTreePanel = new wxPanel(this);
   
   long style = wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT|
      wxSUNKEN_BORDER|wxTR_SINGLE|wxTR_FULL_ROW_HIGHLIGHT |wxTR_EXTENDED;
   // If we allow the the user to be able to edit labels in the tree control. 
   // add wxTR_EDIT_LABELS;
   // @note - This option confuses with double click on the active node.
   // It opens the panel and tried to rename the node. So getting warning message.
   missionTree = new MissionTree(mMissionTreePanel, -1, wxDefaultPosition,
                                 wxDefaultSize, style);
   
   // set to GmatAppData
   GmatAppData::Instance()->SetMissionTree(missionTree);
   missionTree->SetNotebook(this);
   
   int bsize = 1;
   
   CreateMissionTreeToolBar();
   
   // Add default mission to tree
   missionTree->AddDefaultMission();
   mLastLevelClicked = TOOL_LEVEL_ALL;
   
   bsize = 5;
   
   GmatStaticBoxSizer *treeCtrlSizer = new GmatStaticBoxSizer( wxVERTICAL, mMissionTreePanel, "" );
   treeCtrlSizer->Add(mMissionToolBar, 0, wxGROW|wxBOTTOM, bsize);
   
   wxBoxSizer *treeSizer = new wxBoxSizer(wxHORIZONTAL);
   treeSizer->Add( missionTree, 1, wxGROW|wxALIGN_CENTER|wxALL, 0 );
   treeSizer->Add( treeCtrlSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, 0 );
   
   mMissionTreePanel->SetAutoLayout( TRUE );
   mMissionTreePanel->SetSizer( treeSizer );
   
   treeSizer->SetSizeHints( mMissionTreePanel );
   
   return mMissionTreePanel;
}

//------------------------------------------------------------------------------
// wxPanel *CreateOutputPage()
//------------------------------------------------------------------------------
/**
 * Adds a tree with output information to notebook.
 *
 * @return wxPanel object with output information.
 */
//------------------------------------------------------------------------------
wxPanel *GmatNotebook::CreateOutputPage()
{
   wxGridSizer *sizer = new wxGridSizer( 1, 0, 0 );
   wxPanel *panel = new wxPanel(this);

   long style = wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT|
      wxSUNKEN_BORDER|wxTR_SINGLE|wxTR_FULL_ROW_HIGHLIGHT;

   outputTree = new OutputTree(panel, -1, wxDefaultPosition,
                                   wxDefaultSize, style);

   // set to GmatAppData
   GmatAppData::Instance()->SetOutputTree(outputTree);

   sizer->Add( outputTree, 0, wxGROW|wxALL, 0 );

   panel->SetAutoLayout( TRUE );
   panel->SetSizer( sizer );
   sizer->Fit( panel );
   sizer->SetSizeHints( panel );

   return panel;
}


//------------------------------------------------------------------------------
// void OnViewByLevelClicked(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatNotebook::OnViewByLevelClicked(wxCommandEvent& event)
{
   int eventId = event.GetId();
   
   if (eventId == mLastLevelClicked)
   {
      // stay toggled on
      mMissionToolBar->ToggleTool(eventId, true);
      return;
   }
   
   mLastLevelClicked = eventId;
   int viewLevel = 0;
   
   switch (eventId)
   {
   case TOOL_LEVEL_ALL:
      viewLevel = 10;
      mMissionToolBar->ToggleTool(TOOL_LEVEL_1, false);
      mMissionToolBar->ToggleTool(TOOL_LEVEL_2, false);
      mMissionToolBar->ToggleTool(TOOL_LEVEL_3, false);
      break;
   case TOOL_LEVEL_1:
      viewLevel = 1;
      mMissionToolBar->ToggleTool(TOOL_LEVEL_ALL, false);
      mMissionToolBar->ToggleTool(TOOL_LEVEL_2, false);
      mMissionToolBar->ToggleTool(TOOL_LEVEL_3, false);
      break;
   case TOOL_LEVEL_2:
      viewLevel = 2;
      mMissionToolBar->ToggleTool(TOOL_LEVEL_1, false);
      mMissionToolBar->ToggleTool(TOOL_LEVEL_ALL, false);
      mMissionToolBar->ToggleTool(TOOL_LEVEL_3, false);
      break;
   case TOOL_LEVEL_3:
      viewLevel = 3;
      mMissionToolBar->ToggleTool(TOOL_LEVEL_1, false);
      mMissionToolBar->ToggleTool(TOOL_LEVEL_2, false);
      mMissionToolBar->ToggleTool(TOOL_LEVEL_ALL, false);
      break;
   default:
      break;
   }
   
   ResetMissionTreeIncludeCategory();
   ResetMissionTreeExcludeCategory();
   missionTree->SetViewLevel(viewLevel);
}


//------------------------------------------------------------------------------
// void OnViewByCategory(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatNotebook::OnViewByCategory(wxCommandEvent& event)
{
   wxArrayString cmdsToInclude;
   
   // Expand Level, Include category and Exclude category are mutually exclusive.
   // So for example, if an include button is clicked and then an exclude
   // button is clicked it will toggle off the includ button.
   
   int eventId = event.GetId();
   
   #ifdef DEBUG_MISSION_VIEW_CONTROL
   MessageInterface::ShowMessage
      ("GmatNotebook::OnViewByCategory() eventId = %d, mLastIncCategoryClicked = %d, "
       "mLastExcCategoryClicked = %d\n", eventId, mLastIncCategoryClicked,
       mLastExcCategoryClicked);
   #endif
   
   switch (eventId)
   {
   case TOOL_INC_PHYSICS:
   case TOOL_INC_SOLVER:
   case TOOL_INC_SCRIPT:
   case TOOL_INC_CONTROL:
      // Stay toggled on if the last one reclicked      
      if (eventId == mLastIncCategoryClicked)
      {
         mMissionToolBar->ToggleTool(eventId, true);
         return;
      }
      
      ResetMissionTreeLevel();
      ResetMissionTreeExcludeCategory();
      
      // Include Physics based
      if (mMissionToolBar->GetToolState(TOOL_INC_PHYSICS))
      {
         cmdsToInclude.Add("Propagate");
         cmdsToInclude.Add("Maneuver");
         cmdsToInclude.Add("BeginFiniteBurn");
         cmdsToInclude.Add("EndFiniteBurn");
      }
      
      // Include Solver related
      if (mMissionToolBar->GetToolState(TOOL_INC_SOLVER))
      {
         cmdsToInclude.Add("Target");
         cmdsToInclude.Add("Optimize");
         cmdsToInclude.Add("Vary");
         cmdsToInclude.Add("Achieve");
         cmdsToInclude.Add("NonlinearConstraint");
         cmdsToInclude.Add("Minimize");
      }
      
      // Include ScriptEvent
      if (mMissionToolBar->GetToolState(TOOL_INC_SCRIPT))
      {
         cmdsToInclude.Add("BeginScript");
      }
      
      // Include ControlFlow
      if (mMissionToolBar->GetToolState(TOOL_INC_CONTROL))
      {
         cmdsToInclude.Add("If");
         cmdsToInclude.Add("While");
         cmdsToInclude.Add("For");
      }
      
      mLastIncCategoryClicked = GetOnlyOneClicked(1);
      missionTree->SetViewCommands(cmdsToInclude);
      break;
      
   case TOOL_EXC_REPORT:
   case TOOL_EXC_EQUATION:
   case TOOL_EXC_PLOT:
   case TOOL_EXC_CALL:
      // Stay toggled on if the same one reclicked      
      if (eventId == mLastExcCategoryClicked)
      {
         mMissionToolBar->ToggleTool(eventId, true);
         return;
      }
      
      ResetMissionTreeLevel();
      ResetMissionTreeIncludeCategory();
      cmdsToInclude = missionTree->GetCommandList(true);
      
      // Exclude Report
      if (mMissionToolBar->GetToolState(TOOL_EXC_REPORT))
         cmdsToInclude.Remove("Report");
      
      // Exclude Equation
      if (mMissionToolBar->GetToolState(TOOL_EXC_EQUATION))
         cmdsToInclude.Remove("GMAT");
      
      // Exclude Subscriber related
      if (mMissionToolBar->GetToolState(TOOL_EXC_PLOT))
      {
         cmdsToInclude.Remove("ClearPlot");
         cmdsToInclude.Remove("MarkPoint");
         cmdsToInclude.Remove("PenUp");
         cmdsToInclude.Remove("PenDown");
         cmdsToInclude.Remove("Toggle");
      }
      
      // Exclude CallGMATFunction, CallMatlabFunction, Save, SaveMission
      if (mMissionToolBar->GetToolState(TOOL_EXC_CALL))
      {
         cmdsToInclude.Remove("CallGmatFunction");
         cmdsToInclude.Remove("CallMatlabFunction");
         cmdsToInclude.Remove("Save");
         cmdsToInclude.Remove("SaveMission");
      }
      
      mLastExcCategoryClicked = GetOnlyOneClicked(2);
      missionTree->SetViewCommands(cmdsToInclude);
      break;
      
   default:
      break;
   }
   
}


//------------------------------------------------------------------------------
// void OnCustomView(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatNotebook::OnCustomView(wxCommandEvent& event)
{
   int x = 0, y = 0, w = 0, h = 0;
   GetPosition(&x, &y);
   GetSize(&w, &h);
   
   #ifdef DEBUG_MORE_VIEW_OPTIONS
   MessageInterface::ShowMessage
      ("GmatNotebook::OnCustomView() entered, x=%d, y=%d, w=%d, h=%d\n",
       x, y, w, h);
   #endif
   
   TreeViewOptionDialog optionDlg(this, missionTree,
                                  "MissionTree Customize View",
                                  wxPoint(x+w+10, y+75), wxDefaultSize,
                                  wxRESIZE_BORDER|wxDEFAULT_DIALOG_STYLE);
   
   optionDlg.ShowModal();
}


