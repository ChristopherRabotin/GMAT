//$Header$
//------------------------------------------------------------------------------
//                              ResourceTree
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Allison Greene
// Created: 2003/08/29
//
/**
 * This class provides the resource tree and event handlers.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "bitmaps/folder.xpm"
#include "bitmaps/openfolder.xpm"
#include "bitmaps/file.xpm"
#include "bitmaps/spacecraft.xpm"
#include "bitmaps/sun.xpm"
#include "bitmaps/mercury.xpm"
#include "bitmaps/venus.xpm"    
#include "bitmaps/earth.xpm"
#include "bitmaps/mars.xpm"
#include "bitmaps/jupiter.xpm"
#include "bitmaps/saturn.xpm"
#include "bitmaps/uranus.xpm"
#include "bitmaps/neptune.xpm"
#include "bitmaps/pluto.xpm"
#include "bitmaps/report.xpm"
#include "bitmaps/network.xpm"
#include "bitmaps/burn.xpm"
#include "bitmaps/moon.xpm"

#include "GuiInterpreter.hpp"
#include "ResourceTree.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"
#include "GmatTreeItemData.hpp"
#include "ParameterCreateDialog.hpp"
#include "GmatMainFrame.hpp"

#define DEBUG_RESOURCE_TREE 0

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ResourceTree, wxTreeCtrl)
   EVT_TREE_ITEM_RIGHT_CLICK(-1, ResourceTree::OnItemRightClick)

   EVT_TREE_ITEM_ACTIVATED(-1, ResourceTree::OnItemActivated)
   EVT_TREE_BEGIN_LABEL_EDIT(-1, ResourceTree::OnBeginLabelEdit)
   EVT_TREE_END_LABEL_EDIT(-1, ResourceTree::OnEndLabelEdit)
//   EVT_TREE_BEGIN_DRAG(-1, ResourceTree::OnBeginDrag)
   //   EVT_TREE_BEGIN_RDRAG(-1, ResourceTree::OnBeginRDrag)
//   EVT_TREE_END_DRAG(-1, ResourceTree::OnEndDrag)
   
   EVT_MENU(POPUP_ADD_SC, ResourceTree::OnAddSpacecraft)
   EVT_MENU(POPUP_ADD_FORMATION, ResourceTree::OnAddFormation)
   EVT_MENU(POPUP_ADD_CONSTELLATION, ResourceTree::OnAddConstellation)
   EVT_MENU(POPUP_ADD_IMPULSIVE_BURN, ResourceTree::OnAddImpulsiveBurn)
   EVT_MENU(POPUP_ADD_PROPAGATOR, ResourceTree::OnAddPropagator)
   EVT_MENU(POPUP_ADD_BODY, ResourceTree::OnAddBody)
   EVT_MENU(POPUP_ADD_DIFF_CORR, ResourceTree::OnAddDiffCorr)
   EVT_MENU(POPUP_ADD_REPORT_FILE, ResourceTree::OnAddReportFile)
   EVT_MENU(POPUP_ADD_XY_PLOT, ResourceTree::OnAddXyPlot)
   EVT_MENU(POPUP_ADD_OPENGL_PLOT, ResourceTree::OnAddOpenGlPlot)
   EVT_MENU(POPUP_ADD_VARIABLE, ResourceTree::OnAddVariable)
   EVT_MENU(POPUP_OPEN, ResourceTree::OnOpen)
   EVT_MENU(POPUP_CLOSE, ResourceTree::OnClose)
   EVT_MENU(POPUP_RENAME, ResourceTree::OnRename)
   EVT_MENU(POPUP_DELETE, ResourceTree::OnDelete)
   
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ResourceTree(wxFrame *parent)
//------------------------------------------------------------------------------
/**
 * A constructor.
 *
 * @param <parent> parent window
 * @param <id> window id for referencing events
 * @param <pos> position
 * @param <size> size 
 * @param <style> style flags
 */
//------------------------------------------------------------------------------
ResourceTree::ResourceTree(wxWindow *parent, const wxWindowID id,
                           const wxPoint &pos, const wxSize &size, long style)
   : wxTreeCtrl(parent, id, pos, size, style)
{
   //mainNotebook = GmatAppData::GetMainNotebook();
   //mainFrame = GmatAppData::GetMainFrame();
   //MessageInterface::ShowMessage("got main notebook\n");
  
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
 
   AddIcons();
   AddDefaultResources();

   // set the counters to 0
   mNumSpacecraft = 0;
   mNumFormation = 0;
   mNumPropagator = 0;
   mNumImpulsiveBurn = 0;
   mNumReportFile = 0;
   mNumXyPlot = 0;
   mNumOpenGlPlot = 0;
   mNumDiffCorr = 0;
   mNumVariable = 0;

   theGuiManager->UpdateAll();
}

//------------------------------------------------------------------------------
// void UpdateResource(bool resetCounter = true)
//------------------------------------------------------------------------------
/**
 * Reset counters, delete all nodes that are not folders, add default nodes
 */
//------------------------------------------------------------------------------
void ResourceTree::UpdateResource(bool resetCounter)
{
#if DEBUG_RESOURCE_TREE
   MessageInterface::ShowMessage("ResourceTree::UpdateResource() entered\n");
#endif

   if (resetCounter)
   {
      mNumSpacecraft = 0;
      mNumFormation = 0;
      mNumPropagator = 0;
      mNumImpulsiveBurn = 0;
      mNumReportFile = 0;
      mNumXyPlot = 0;
      mNumOpenGlPlot = 0;
      mNumDiffCorr = 0;
      mNumVariable = 0;
   }
   
   // ag: collapse, so folder icon is closed
   Collapse(mSpacecraftItem);
   Collapse(mFormationItem);
   Collapse(mPropagatorItem);
   Collapse(mBurnItem);
   Collapse(mSolverItem);
   Collapse(mSubscriberItem);
   Collapse(mVariableItem);
    
   DeleteChildren(mSpacecraftItem);
   DeleteChildren(mFormationItem);
   DeleteChildren(mPropagatorItem);
   DeleteChildren(mBurnItem);
   DeleteChildren(mSolverItem);
   DeleteChildren(mSubscriberItem);
   DeleteChildren(mVariableItem);

   AddDefaultSpacecraft(mSpacecraftItem);
   AddDefaultFormations(mFormationItem);
   AddDefaultPropagators(mPropagatorItem);
   AddDefaultBurns(mBurnItem);
   AddDefaultSolvers(mSolverItem);
   AddDefaultSubscribers(mSubscriberItem);
   AddDefaultVariables(mVariableItem);
   
   theGuiManager->UpdateAll();
}

//------------------------------------------------------------------------------
//  void AddDefaultResources()
//------------------------------------------------------------------------------
/**
 * Add default folders
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultResources()
{
   wxTreeItemId resource =
      AddRoot(wxT("Resources"), -1, -1,
              new GmatTreeItemData(wxT("Resources"), GmatTree::RESOURCES_FOLDER));

   //----- Spacecraft
   mSpacecraftItem =
      AppendItem(resource, wxT("Spacecraft"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Spacecraft"),
                                      GmatTree::SPACECRAFT_FOLDER));
    
   SetItemImage(mSpacecraftItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);
    
   //----- Formations
    mFormationItem =
      AppendItem(resource, wxT("Formations"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Formations"),
                                      GmatTree::FORMATIONS_FOLDER));
    
   SetItemImage(mFormationItem, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);

   //----- Constellations
   wxTreeItemId constellationItem =
      AppendItem(resource,
                 wxT("Constellations"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Constellations"),
                                      GmatTree::CONSTELLATIONS_FOLDER));
    
   SetItemImage(constellationItem, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);

   //----- Burns
   mBurnItem =
      AppendItem(resource, wxT("Burns"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Burns"),
                                      GmatTree::BURNS_FOLDER));
    
   SetItemImage(mBurnItem, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

    
   //----- Propagators
   mPropagatorItem =
      AppendItem(resource,
                 wxT("Propagators"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Propagators"),
                                      GmatTree::PROPAGATORS_FOLDER));
    
   SetItemImage(mPropagatorItem, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);
    
   //----- Universe
   wxTreeItemId universeItem =
      AppendItem(resource, wxT("Universe"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Universe"),
                                      GmatTree::UNIVERSE_FOLDER));
    
   SetItemImage(universeItem, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);

   //----- Solver
   mSolverItem =
      AppendItem(resource, wxT("Solvers"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Solvers"),
                                      GmatTree::SOLVERS_FOLDER));
    
   SetItemImage(mSolverItem, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);

   //----- Subscribers
   mSubscriberItem =
      AppendItem(resource, wxT("Plots/Reports"), GmatTree::ICON_FOLDER,
                 -1, new GmatTreeItemData(wxT("Plots/Reports"),
                                          GmatTree::SUBSCRIBERS_FOLDER));
    
   SetItemImage(mSubscriberItem, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);

   //----- Interfaces
   wxTreeItemId interfaceItem =
      AppendItem(resource, wxT("Interfaces"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Interfaces"),
                                      GmatTree::INTERFACES_FOLDER));
    
   SetItemImage(interfaceItem, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);

   //----- Subscripts
   wxTreeItemId subscriptItem =
      AppendItem(resource, wxT("Subscripts"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Subscripts"),
                                      GmatTree::SUBSCRIPTS_FOLDER));

   SetItemImage(subscriptItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);
    
   //----- Vairables
   mVariableItem = 
      AppendItem(resource, wxT("Variables"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Variables"),
                                      GmatTree::VARIABLES_FOLDER));
   // ag:  Should the GmatTree type of variableItem change?

   SetItemImage(mVariableItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);
   
   AppendItem(resource, wxT("MATLAB Functions"), GmatTree::ICON_FOLDER,
              -1, new GmatTreeItemData(wxT("MATLAB Functions"), GmatTree::MATLAB_FUNCT_FOLDER));
    
   //----- GroundStations
   AppendItem(resource, wxT("Ground Stations"), GmatTree::ICON_FOLDER, 
              -1, new GmatTreeItemData(wxT("Ground Stations"),
                                       GmatTree::GROUNDSTATIONS_FOLDER));

   AddDefaultBodies(universeItem);
   AddDefaultSpacecraft(mSpacecraftItem);
   AddDefaultFormations(mFormationItem);
   AddDefaultConstellations(constellationItem);
   AddDefaultPropagators(mPropagatorItem);
   AddDefaultSolvers(mSolverItem);
   AddDefaultSubscribers(mSubscriberItem);
   AddDefaultInterfaces(interfaceItem);
   AddDefaultVariables(mVariableItem);
}

//------------------------------------------------------------------------------
// void AddDefaultBodies(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Add the default bodies
 *
 * @param <itemId> tree item for the solar system folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultBodies(wxTreeItemId itemId)
{
   AppendItem(itemId, wxT("Sun"), GmatTree::ICON_SUN, -1,
              new GmatTreeItemData(wxT("Sun"), GmatTree::DEFAULT_BODY));
   AppendItem(itemId, wxT("Mercury"), GmatTree::ICON_MERCURY, -1,
              new GmatTreeItemData(wxT("Mercury"), GmatTree::DEFAULT_BODY));
   AppendItem(itemId, wxT("Venus"), GmatTree::ICON_VENUS, -1,
              new GmatTreeItemData(wxT("Venus"), GmatTree::DEFAULT_BODY));

   wxTreeItemId earth = AppendItem(itemId, wxT("Earth"), GmatTree::ICON_EARTH, -1,
                                   new GmatTreeItemData(wxT("Earth"), GmatTree::DEFAULT_BODY));
   AppendItem(earth, wxT("Moon"), GmatTree::ICON_MOON, -1,
              new GmatTreeItemData(wxT("Moon"), GmatTree::DEFAULT_BODY));

   AppendItem(itemId, wxT("Mars"), GmatTree::ICON_MARS, -1,
              new GmatTreeItemData(wxT("Mars"), GmatTree::DEFAULT_BODY));
   AppendItem(itemId, wxT("Jupiter"), GmatTree::ICON_JUPITER, -1,
              new GmatTreeItemData(wxT("Jupiter"), GmatTree::DEFAULT_BODY));
   AppendItem(itemId, wxT("Saturn"), GmatTree::ICON_SATURN, -1,
              new GmatTreeItemData(wxT("Saturn"), GmatTree::DEFAULT_BODY));
   AppendItem(itemId, wxT("Uranus"), GmatTree::ICON_URANUS, -1,
              new GmatTreeItemData(wxT("Uranus"), GmatTree::DEFAULT_BODY));
   AppendItem(itemId, wxT("Neptune"), GmatTree::ICON_NEPTUNE, -1,
              new GmatTreeItemData(wxT("Neptune"), GmatTree::DEFAULT_BODY));
   AppendItem(itemId, wxT("Pluto"), GmatTree::ICON_PLUTO, -1,
              new GmatTreeItemData(wxT("Pluto"), GmatTree::DEFAULT_BODY));
}

//------------------------------------------------------------------------------
// void AddDefaultSpacecraft(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Add the default spacecraft
 *
 * @param <itemId> tree item for the spacecraft folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultSpacecraft(wxTreeItemId itemId)
{
   StringArray itemNames = theGuiInterpreter->GetListOfConfiguredItems(Gmat::SPACECRAFT);
   int size = itemNames.size();
   wxString objName;
    
   for (int i = 0; i<size; i++)
   {
      objName = wxString(itemNames[i].c_str());
      AppendItem(itemId, wxT(objName), GmatTree::ICON_SPACECRAFT, -1,
                 new GmatTreeItemData(wxT(objName), GmatTree::DEFAULT_SPACECRAFT));
   };

   if (size > 0)
      Expand(itemId);
}


//------------------------------------------------------------------------------
// void AddDefaultFormations(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Add the default formations
 *
 * @param <itemId> tree item for the formation folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultFormations(wxTreeItemId itemId)
{
   StringArray itemNames = theGuiInterpreter->GetListOfConfiguredItems(Gmat::FORMATION);
   int size = itemNames.size();
   wxString objName;
    
   for (int i = 0; i<size; i++)
   {
      objName = wxString(itemNames[i].c_str());
      
      wxTreeItemId formationItem =
      AppendItem(itemId, wxT(objName), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT(objName), GmatTree::DEFAULT_FORMATION_FOLDER));
      SetItemImage(formationItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);
      
      Formation *form = (Formation *)theGuiInterpreter->GuiInterpreter::GetFormation(itemNames[i].c_str());
      // get added spacecrafts
      int scListId = form->GetParameterID("Add");
      StringArray formSc = form->GetStringArrayParameter(scListId);
      int formSize = formSc.size();
      
      for (int j = 0; j<formSize; j++)
      {
         objName = wxString(formSc[j].c_str());
         AppendItem(formationItem, wxT(objName), GmatTree::ICON_SPACECRAFT, -1,
                 new GmatTreeItemData(wxT(objName), GmatTree::DEFAULT_SPACECRAFT));
      }   

      Expand(formationItem);
   };

   if (size > 0)
      Expand(itemId);
}

//------------------------------------------------------------------------------
// void AddDefaultConstellations(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Add the default constellations
 *
 * @param <itemId> tree item for the constellation folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultConstellations(wxTreeItemId itemId)
{
//   wxTreeItemId gps =
//      AppendItem(itemId, wxT("GPS"), -1, -1,
//                 new GmatTreeItemData(wxT("GPS"), GmatTree::DEFAULT_CONSTELLATION_FOLDER));
//   SetItemImage(gps, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded);
//
//   AppendItem(gps, wxT("GPS1"), GmatTree::ICON_SPACECRAFT, -1,
//              new GmatTreeItemData(wxT("GPS1"), GmatTree::DEFAULT_CONSTELLATION_SATELLITE));
//   AppendItem(gps, wxT("GPS2"), GmatTree::ICON_SPACECRAFT, -1,
//              new GmatTreeItemData(wxT("GPS2"), GmatTree::DEFAULT_CONSTELLATION_SATELLITE));
//   AppendItem(gps, wxT("GPS3"), GmatTree::ICON_SPACECRAFT, -1,
//              new GmatTreeItemData(wxT("GPS3"), GmatTree::DEFAULT_CONSTELLATION_SATELLITE));
//   AppendItem(gps, wxT("GPS4"), GmatTree::ICON_SPACECRAFT, -1,
//              new GmatTreeItemData(wxT("GPS4"), GmatTree::DEFAULT_CONSTELLATION_SATELLITE));
}

//------------------------------------------------------------------------------
// void AddDefaultPropagators(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Add the default propagator
 *
 * @param <itemId> tree item for the propagators folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultPropagators(wxTreeItemId itemId)
{
   StringArray itemNames = theGuiInterpreter->GetListOfConfiguredItems(Gmat::PROP_SETUP);
   int size = itemNames.size();
   wxString objName;
    
   for (int i = 0; i<size; i++)
   {
      objName = wxString(itemNames[i].c_str());
      AppendItem(itemId, wxT(objName), GmatTree::ICON_FILE, -1,
                 new GmatTreeItemData(wxT(objName), GmatTree::DEFAULT_PROPAGATOR));
   };

   if (size > 0)
      Expand(itemId);
}

//------------------------------------------------------------------------------
// void AddDefaultBurns(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Add the default burns
 *
 * @param <itemId> tree item for the burn folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultBurns(wxTreeItemId itemId)
{
   //MessageInterface::ShowMessage("ResourceTree::AddDefaultBurns() entered\n");
   StringArray itemNames = theGuiInterpreter->GetListOfConfiguredItems(Gmat::BURN);
   int size = itemNames.size();
   wxString objName;
   wxString objTypeName;
    
   for (int i = 0; i<size; i++)
   {
      Burn *burn = theGuiInterpreter->GetBurn(itemNames[i]);
      objName = wxString(itemNames[i].c_str());
      objTypeName = wxString(burn->GetTypeName().c_str());

      if (objTypeName == "ImpulsiveBurn")
      {
         //MessageInterface::ShowMessage("ResourceTree::AddDefaultBurns() objTypeName = ImpulsiveBurn\n");
         AppendItem(itemId, wxT(objName), GmatTree::ICON_BURN, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::DEFAULT_IMPULSIVE_BURN));
      }
   };

   if (size > 0)
      Expand(itemId);
}

//------------------------------------------------------------------------------
// void AddDefaultSolvers(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Add the default solvers
 *
 * @param <itemId> tree item for the solvers folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultSolvers(wxTreeItemId itemId)
{
   StringArray itemNames = theGuiInterpreter->GetListOfConfiguredItems(Gmat::SOLVER);
   int size = itemNames.size();
   wxString objName;
   wxString objTypeName;
    
   for (int i = 0; i<size; i++)
   {
      Solver *solver = theGuiInterpreter->GetSolver(itemNames[i]);
      objName = wxString(itemNames[i].c_str());
      objTypeName = wxString(solver->GetTypeName().c_str());
        
      if (objTypeName == "DifferentialCorrector")
      {
         AppendItem(itemId, wxT(objName), GmatTree::ICON_FILE, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::DEFAULT_DIFF_CORR));
      }
   };

   if (size > 0)
      Expand(itemId);
}

//------------------------------------------------------------------------------
// void AddDefaultSubscribers(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Add the default subscribers
 *
 * @param <itemId> tree item for the subscribers folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultSubscribers(wxTreeItemId itemId)
{
   StringArray itemNames = theGuiInterpreter->GetListOfConfiguredItems(Gmat::SUBSCRIBER);
   int size = itemNames.size();
   wxString objName;
   wxString objTypeName;
    
   for (int i = 0; i<size; i++)
   {
      Subscriber *sub = theGuiInterpreter->GetSubscriber(itemNames[i]);
      objName = wxString(itemNames[i].c_str());
      objTypeName = wxString(sub->GetTypeName().c_str());

      if (objTypeName == "ReportFile")
         AppendItem(itemId, wxT(objName), GmatTree::ICON_REPORT, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::DEFAULT_REPORT_FILE));
      else if (objTypeName == "XyPlot")
         AppendItem(itemId, wxT(objName), GmatTree::ICON_REPORT, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::DEFAULT_XY_PLOT));
      else if (objTypeName == "OpenGlPlot")
         AppendItem(itemId, wxT(objName), GmatTree::ICON_REPORT, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::DEFAULT_OPENGL_PLOT));
   };

   if (size > 0)
      Expand(itemId);
}

//------------------------------------------------------------------------------
// void AddDefaultInterfaces(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Add the default interfaces
 *
 * @param <itemId> tree item for the interfaces folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultInterfaces(wxTreeItemId itemId)
{
   AppendItem(itemId, wxT("TCP/IP"), GmatTree::ICON_NETWORK, -1,
              new GmatTreeItemData(wxT("TCP/IP"), GmatTree::DEFAULT_INTERFACE));
   AppendItem(itemId, wxT("Mex"), GmatTree::ICON_FILE, -1,
              new GmatTreeItemData(wxT("Mex"), GmatTree::DEFAULT_INTERFACE));
    
}

//------------------------------------------------------------------------------
// void AddDefaultVariables(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Add the default variables
 *
 * @param <itemId> tree item for the variables folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultVariables(wxTreeItemId itemId)
{
   StringArray itemNames = theGuiInterpreter->GetListOfConfiguredItems(Gmat::PARAMETER);
   int size = itemNames.size();
   wxString objName;
   Parameter *param;
    
   for (int i = 0; i<size; i++)
   {
      objName = wxString(itemNames[i].c_str());
      param = theGuiInterpreter->GetParameter(itemNames[i]);
        
      AppendItem(itemId, wxT(objName), GmatTree::ICON_FILE, -1,
                 new GmatTreeItemData(wxT(objName),
                                      GmatTree::DEFAULT_VARIABLE));
   };

   theGuiManager->UpdateParameter();
    
   //    if (size > 0)
   //        Expand(itemId);
}

//==============================================================================
//                         On Action Events
//==============================================================================

//------------------------------------------------------------------------------
// void OnItemRightClick(wxTreeEvent& event)
//------------------------------------------------------------------------------
/**
 * On right click show the pop up menu
 *
 */
//------------------------------------------------------------------------------
void ResourceTree::OnItemRightClick(wxTreeEvent& event)
{
   ShowMenu(event.GetItem(), event.GetPoint());
}

//------------------------------------------------------------------------------
// void ShowMenu(wxTreeItemId itemId, const wxPoint& pt)
//------------------------------------------------------------------------------
/**
 * Create the popup menu
 *
 * @param <itemId> tree item that was right clicked
 * @param <pt> location of right click
 */
//------------------------------------------------------------------------------
void ResourceTree::ShowMenu(wxTreeItemId itemId, const wxPoint& pt)
{
   GmatTreeItemData *treeItem = (GmatTreeItemData *)GetItemData(itemId);
   wxString title = treeItem->GetDesc();
   int dataType = treeItem->GetDataType();
    
#if wxUSE_MENUS
   wxMenu menu;
    
   if (strcmp(title, wxT("Spacecraft")) == 0)
      menu.Append(POPUP_ADD_SC, wxT("Add Spacecraft..."));
   else if (strcmp(title, wxT("Formations")) == 0)
      menu.Append(POPUP_ADD_FORMATION, wxT("Add Formation..."));
   else if (strcmp(title, wxT("Constellations")) == 0)
   {
      menu.Append(POPUP_ADD_CONSTELLATION, wxT("Add Constellation..."));
      menu.Enable(POPUP_ADD_CONSTELLATION, FALSE);
   }   
   else if (strcmp(title, wxT("Burns")) == 0)
      menu.Append(POPUP_ADD_BURN, wxT("Add"), CreatePopupMenu(Gmat::BURN));
   else if (strcmp(title, wxT("Propagators")) == 0)
      menu.Append(POPUP_ADD_PROPAGATOR, wxT("Add Propagator..."));
   else if (strcmp(title, wxT("Solvers")) == 0)
      menu.Append(POPUP_ADD_SOLVER, wxT("Add"), CreatePopupMenu(Gmat::SOLVER));
   else if (strcmp(title, wxT("Universe")) == 0)
      menu.Append(POPUP_ADD_BODY, wxT("Add Body..."));
   else if (strcmp(title, wxT("Plots/Reports")) == 0)
      menu.Append(POPUP_ADD_SUBSCRIBER, _T("Add"), CreatePopupMenu(Gmat::SUBSCRIBER));
   else if (strcmp(title, wxT("Variables")) == 0)
      menu.Append(POPUP_ADD_VARIABLE, wxT("Add Variable..."));
//   else if ((dataType == GmatTree::DEFAULT_FORMATION_FOLDER)     ||
//            (dataType == GmatTree::CREATED_FORMATION_FOLDER)     ||
//            (dataType == GmatTree::DEFAULT_CONSTELLATION_FOLDER) ||
//            (dataType == GmatTree::CREATED_CONSTELLATION_FOLDER))
//   {
//      menu.Append(POPUP_ADD_SC, wxT("Add Spacecraft..."));
//      menu.AppendSeparator();
//      menu.Append(POPUP_OPEN, wxT("Open"));
//      menu.Append(POPUP_CLOSE, wxT("Close"));
//      menu.AppendSeparator();
//      menu.Append(POPUP_RENAME, wxT("Rename"));
//      menu.Append(POPUP_DELETE, wxT("Delete"));
//   }
   else if (dataType == GmatTree::DEFAULT_PROPAGATOR)
   {
      menu.Append(POPUP_OPEN, wxT("Open"));
      menu.Append(POPUP_CLOSE, wxT("Close"));
   }
   else if (dataType == GmatTree::MATLAB_FUNCT_FOLDER)
   {
      menu.Append(POPUP_ADD_MATLAB_FUNCT, wxT("Add MATLAB Function"));
      menu.Enable(POPUP_ADD_MATLAB_FUNCT, FALSE);
   }   
   else
   {
      menu.Append(POPUP_OPEN, wxT("Open"));
      menu.Append(POPUP_CLOSE, wxT("Close"));
      menu.AppendSeparator();
      menu.Append(POPUP_RENAME, wxT("Rename"));
      menu.Append(POPUP_DELETE, wxT("Delete"));
        
      menu.Enable(POPUP_DELETE, FALSE);
   }

   //loj: 2/13/04 I just thought about this for the future build
   //      switch (dataType)
   //      {
   //      case GmatTree::SPACECRAFT_FOLDER:
   //          menu.Append(POPUP_CLEAR_SC, wxT("Clear"));
   //      case GmatTree::BURNS_FOLDER:
   //          menu.Append(POPUP_CLEAR_BURN, wxT("Clear"));
   //      case GmatTree::PROPAGATORS_FOLDER:
   //           menu.Append(POPUP_CLEAR_PROPAGATOR, wxT("Clear"));
   //      case GmatTree::SOLVERS_FOLDER:
   //          menu.Append(POPUP_CLEAR_SOLVER, wxT("Clear"));
   //      case GmatTree::SUBSCRIBERS_FOLDER:
   //          menu.Append(POPUP_CLEAR_SUBSCRIBER, wxT("Clear"));
   //          break;
   //      default:
   //          break;
   //      }
    
   PopupMenu(&menu, pt);
#endif // wxUSE_MENUS
}

//------------------------------------------------------------------------------
// void OnItemActivated(wxTreeEvent &event)
//------------------------------------------------------------------------------
/**
 * Double click on tree item
 *
 * @param <event> tree event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnItemActivated(wxTreeEvent &event)
{
   // get some info about this item
   wxTreeItemId itemId = event.GetItem();
   GmatTreeItemData *item = (GmatTreeItemData *)GetItemData(itemId);

   //    mainNotebook->CreatePage(item);
   GmatAppData::GetMainFrame()->CreateChild(item);
}

//------------------------------------------------------------------------------
// void OnOpen(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Open chosen from popup menu
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnOpen(wxCommandEvent &event)
{
   // Get info from selected item
   GmatTreeItemData *item = (GmatTreeItemData *) GetItemData(GetSelection());
   //    mainNotebook->CreatePage(item);
   GmatAppData::GetMainFrame()->CreateChild(item);
}

//------------------------------------------------------------------------------
// void OnClose(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Close chosen from popup menu
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnClose(wxCommandEvent &event)
{
   // Get info from selected item
   GmatTreeItemData *item = (GmatTreeItemData *) GetItemData(GetSelection());
   
   // if its open, its activated
   if (GmatAppData::GetMainFrame()->IsChildOpen(item))
      // close the window
      GmatAppData::GetMainFrame()->CloseActiveChild();
   else
      return;
}

//------------------------------------------------------------------------------
// void OnRename(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Rename chosen from popup menu
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnRename(wxCommandEvent &event)
{
   //MessageInterface::ShowMessage("ResourceTree::OnRename() entered\n");


   wxTreeItemId item = GetSelection();
   GmatTreeItemData *selItem = (GmatTreeItemData *) GetItemData(item);
   wxString oldName = selItem->GetDesc();
   int dataType = selItem->GetDataType();


   wxString newName = oldName;
   newName = wxGetTextFromUser(wxT("New name: "), wxT("Input Text"),
                               newName, this);
   if ( !newName.IsEmpty())
   {
      Gmat::ObjectType objType;
        
      switch (dataType)
      {
      case GmatTree::DEFAULT_SPACECRAFT:
      case GmatTree::CREATED_SPACECRAFT:
         objType = Gmat::SPACECRAFT;
         break;
      case GmatTree::DEFAULT_IMPULSIVE_BURN:
      case GmatTree::CREATED_IMPULSIVE_BURN:
         objType = Gmat::BURN;
         break;
      case GmatTree::DEFAULT_DIFF_CORR:
      case GmatTree::CREATED_DIFF_CORR:
         objType = Gmat::SOLVER;
         break;
      case GmatTree::DEFAULT_REPORT_FILE:
      case GmatTree::CREATED_REPORT_FILE:
      case GmatTree::DEFAULT_XY_PLOT:
      case GmatTree::CREATED_XY_PLOT:
      case GmatTree::DEFAULT_OPENGL_PLOT:
      case GmatTree::CREATED_OPENGL_PLOT:
         objType = Gmat::SUBSCRIBER;
         break;
      default:
         objType = Gmat::UNKNOWN_OBJECT;
         //            MessageInterface::ShowMessage("ResourceTree::OnRename() unknown object.\n");
         break;
      }


      // update item only if successful
      if (theGuiInterpreter->
          RenameConfiguredItem(objType, oldName.c_str(), newName.c_str()))
      {
         SetItemText(item, newName);
         GmatTreeItemData *selItem = (GmatTreeItemData *) GetItemData(item);
         selItem->SetDesc(newName);
         //theGuiManager->UpdateSpacecraft();
         theGuiManager->UpdateAll(); //loj: 8/5/04 Let's try UpdateAll
      }
   }


   //loj: It looks better to change name using EditLabel, but How do I get new name?
   //loj:(void) EditLabel(item);


}

//------------------------------------------------------------------------------
// void OnDelete(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Delete chosen from popup menu
 *
 * @param <event> command event
 * @todo Finish this when items can be deleted from interpreter
 */
//------------------------------------------------------------------------------
void ResourceTree::OnDelete(wxCommandEvent &event)
{
   event.Skip();
   //    wxTreeItemId item = GetSelection();
   //    wxTreeItemId parentId = GetPrevVisible(item);
   //    this->Collapse(parentId);
   //    
   //    // delete from gui interpreter
   //    GmatTreeItemData *gmatItem = (GmatTreeItemData *)GetItemData(item);
   ////    theGuiInterpreter->RemoveConfiguredItem("Spacecraft", gmatItem->GetDesc());
   //    
   //    this->Delete(item);
   //    
   //
}

//------------------------------------------------------------------------------
// void OnBeginLabelEdit(wxTreeEvent &event)
//------------------------------------------------------------------------------
/**
 * Rename chosen from popup menu
 *
 * @param <event> tree event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnBeginLabelEdit(wxTreeEvent &event)
{
  
   GmatTreeItemData *selItem = (GmatTreeItemData *)
      GetItemData(event.GetItem());
                               
   int dataType = selItem->GetDataType();
   bool isDefaultFolder = ((dataType == GmatTree::RESOURCES_FOLDER)      ||
                           (dataType == GmatTree::SPACECRAFT_FOLDER)     ||
                           (dataType == GmatTree::FORMATIONS_FOLDER)     ||
                           (dataType == GmatTree::CONSTELLATIONS_FOLDER) ||
                           (dataType == GmatTree::BURNS_FOLDER)          ||
                           (dataType == GmatTree::PROPAGATORS_FOLDER)    ||
                           (dataType == GmatTree::UNIVERSE_FOLDER)       ||
                           (dataType == GmatTree::SOLVERS_FOLDER)        ||
                           (dataType == GmatTree::SUBSCRIBERS_FOLDER)    ||
                           (dataType == GmatTree::INTERFACES_FOLDER)     ||
                           (dataType == GmatTree::SUBSCRIPTS_FOLDER)     ||
                           (dataType == GmatTree::VARIABLES_FOLDER)      ||
                           (dataType == GmatTree::GROUNDSTATIONS_FOLDER));
                         
   bool isDefaultItem = ((dataType == GmatTree::DEFAULT_PROPAGATOR)  ||
                         (dataType == GmatTree::DEFAULT_BODY)        ||
                         (dataType == GmatTree::DEFAULT_DIFF_CORR)   ||
                         (dataType == GmatTree::DEFAULT_REPORT_FILE) ||
                         (dataType == GmatTree::DEFAULT_XY_PLOT)     ||
                         (dataType == GmatTree::DEFAULT_OPENGL_PLOT) ||
                         (dataType == GmatTree::DEFAULT_INTERFACE));

   //kind of redundant because OpenPage returns false for some
   //of the default folders
   if ((GmatAppData::GetMainFrame()->IsChildOpen(selItem))  ||
       (isDefaultFolder)                                   ||
       (isDefaultItem))
   {
      event.Veto();
   }
}

//------------------------------------------------------------------------------
// void OnEndLabelEdit(wxTreeEvent &event)
//------------------------------------------------------------------------------
/**
 * Finished changing label on the tree
 *
 * @param <event> tree event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnEndLabelEdit(wxTreeEvent &event)
{
   wxString newLabel = event.GetLabel();
  
   // check to see if label is a single word
   if (newLabel.IsWord())
   {
      GmatTreeItemData *selItem = (GmatTreeItemData *)
         GetItemData(GetSelection());

      wxString oldLabel = selItem->GetDesc();
      int itemType = selItem->GetDataType();

      selItem->SetDesc(newLabel);
                  
      // if label refers to an object reset the object name
      if ((itemType == GmatTree::DEFAULT_SPACECRAFT)  ||
          (itemType == GmatTree::CREATED_SPACECRAFT))
      {
         const std::string stdOldLabel = oldLabel.c_str();
         const std::string stdNewLabel = newLabel.c_str();
     
         Spacecraft *theSpacecraft =
            theGuiInterpreter->GetSpacecraft(stdOldLabel);

         theSpacecraft->SetName(stdNewLabel);

         // if (resetName)
         //     selItem->SetDesc(label);
         //  else
         //     selItem->SetDesc(oldLabel);
      
      }
     
 
   }
   else
   {
      event.Veto();
   }
}

//------------------------------------------------------------------------------
// void AddIcons()
//------------------------------------------------------------------------------
/**
 * Add icons for items in the tree
 */
//------------------------------------------------------------------------------
void ResourceTree::AddIcons()
{
   int size = 16;
  
   wxImageList *images = new wxImageList ( size, size, true );
  
   wxBusyCursor wait;
   wxIcon icons[18];

   icons[0] = wxIcon ( folder_xpm );
   icons[1] = wxIcon ( file_xpm );
   icons[2] = wxIcon ( openfolder_xpm );
   icons[3] = wxIcon ( spacecrapt_xpm );
   icons[4] = wxIcon ( sun_xpm );
   icons[5] = wxIcon ( mercury_xpm );
   icons[6] = wxIcon ( venus_xpm );
   icons[7] = wxIcon ( earth_xpm );
   icons[8] = wxIcon ( mars_xpm );
   icons[9] = wxIcon ( jupiter_xpm );
   icons[10] = wxIcon ( saturn_xpm );
   icons[11] = wxIcon ( uranus_xpm );
   icons[12] = wxIcon ( neptune_xpm );
   icons[13] = wxIcon ( pluto_xpm );
   icons[14] = wxIcon ( report_xpm );
   icons[15] = wxIcon ( network_xpm );
   icons[16] = wxIcon ( burn_xpm );
   icons[17] = wxIcon ( moon_xpm );
 
   int sizeOrig = icons[0].GetWidth();
   for ( size_t i = 0; i < WXSIZEOF(icons); i++ )
   {
      if ( size == sizeOrig )
      {
         images->Add(icons[i]);
      }
      else
      {
         images->Add(wxBitmap(wxBitmap(icons[i]).ConvertToImage().Rescale(size, size)));
      }
   }

   AssignImageList(images);

}

//---------------------------------
// Add items to tree
//---------------------------------

//------------------------------------------------------------------------------
// void OnAddBody(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a body to solar system folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddBody(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   AppendItem(item, wxT("New Body"), GmatTree::ICON_EARTH, -1,
              new GmatTreeItemData(wxT("New Body"), GmatTree::CREATED_BODY));
}

//------------------------------------------------------------------------------
// void OnAddSpacecraft(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a spacecraft to spacecraft folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddSpacecraft(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
  
   wxString withName;
   withName.Printf("Spacecraft%d", ++mNumSpacecraft);
  
   const std::string stdWithName = withName.c_str();

   Spacecraft *sc = theGuiInterpreter->
      CreateSpacecraft("Spacecraft", stdWithName);

   if (sc != NULL)
   {
      wxString newName = wxT(sc->GetName().c_str());
  
      AppendItem(item, newName, GmatTree::ICON_SPACECRAFT, -1,
                 new GmatTreeItemData(newName, GmatTree::CREATED_SPACECRAFT));

      theGuiManager->UpdateSpacecraft();
  
      Expand(item);
   }
}


//------------------------------------------------------------------------------
// void OnAddFormation(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a formation to formation folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddFormation(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
  
   wxString withName;
   withName.Printf("Formation%d", ++mNumFormation);
  
   const std::string stdWithName = withName.c_str();

   Formation *form = theGuiInterpreter->
      CreateFormation("Formation", stdWithName);

   if (form != NULL)
   {
      wxString newName = wxT(form->GetName().c_str());
  
      wxTreeItemId formationItem =
      AppendItem(item, newName, GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(newName, GmatTree::CREATED_FORMATION_FOLDER));
      SetItemImage(formationItem, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);

      theGuiManager->UpdateFormation(); //loj: 8/5/04 added
  
      Expand(item);
   }
}

//------------------------------------------------------------------------------
// void OnAddConstellation(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a constellation to constellation folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddConstellation(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   wxTreeItemId constellation =
      AppendItem(item, wxT("New Constellation"), -1, -1,
                 new GmatTreeItemData(wxT("New Constellation"),
                                      GmatTree::CREATED_CONSTELLATION_FOLDER));

   SetItemImage(constellation, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

   AppendItem(constellation, wxT("GPS1"), GmatTree::ICON_SPACECRAFT, -1,
              new GmatTreeItemData(wxT("GPS1"), GmatTree::CREATED_CONSTELLATION_SATELLITE));
   AppendItem(constellation, wxT("GPS2"), GmatTree::ICON_SPACECRAFT, -1,
              new GmatTreeItemData(wxT("GPS2"), GmatTree::CREATED_CONSTELLATION_SATELLITE));

   Expand(item);
}

//------------------------------------------------------------------------------
// void OnAddPropagator(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a propagator to propagator folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddPropagator(wxCommandEvent &event)
{
   //MessageInterface::ShowMessage("ResourceTree::OnAddPropagator() entered\n");
   wxTreeItemId item = GetSelection();
  
   wxString name;
   name.Printf("Propagator%d", ++mNumPropagator);

   PropSetup *propSetup =
      theGuiInterpreter->CreateDefaultPropSetup(std::string(name.c_str()));

   if (propSetup != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_FILE, -1,
                 new GmatTreeItemData(name, GmatTree::CREATED_PROPAGATOR));

      Expand(item);
   }
   else
   {
      MessageInterface::ShowMessage
         ("ResourceTree::OnAddPropagator() propSetup is NULL\n");
   }
}

//------------------------------------------------------------------------------
// void OnAddImpulsiveBurn(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an impulsive burn to burn folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddImpulsiveBurn(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
  
   wxString name;
   name.Printf("ImpulsiveBurn%d", ++mNumImpulsiveBurn);
  
   if (theGuiInterpreter->CreateBurn
       ("ImpulsiveBurn", std::string(name.c_str())) != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_BURN, -1,
                 new GmatTreeItemData(name, GmatTree::CREATED_IMPULSIVE_BURN));

      Expand(item);
   }
}

//------------------------------------------------------------------------------
// void OnAddDiffCorr(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a differential corrector to solvers folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddDiffCorr(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
  
   wxString name;
   name.Printf("DC%d", ++mNumDiffCorr);

   if (theGuiInterpreter->CreateSolver
       ("DifferentialCorrector", std::string(name.c_str())) != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_FILE, -1,
                 new GmatTreeItemData(name, GmatTree::CREATED_DIFF_CORR));

      Expand(item);
   }
}

//------------------------------------------------------------------------------
// void OnAddReportFile(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a report file to reports folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddReportFile(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
  
   wxString name;
   name.Printf("ReportFile%d", ++mNumReportFile);

   if (theGuiInterpreter->CreateSubscriber
       ("ReportFile", std::string(name.c_str())) != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_REPORT, -1,
                 new GmatTreeItemData(name, GmatTree::CREATED_REPORT_FILE));

      Expand(item);
   }
}

//------------------------------------------------------------------------------
// void OnAddXyPlot(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an xy plot to plots folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddXyPlot(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
  
   wxString name;
   name.Printf("XYPlot%d", ++mNumXyPlot);

   if (theGuiInterpreter->CreateSubscriber
       ("XyPlot", std::string(name.c_str())) != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_REPORT, -1,
                 new GmatTreeItemData(name, GmatTree::CREATED_XY_PLOT));
      
      Expand(item);
   }
}

//------------------------------------------------------------------------------
// void OnAddOpenGlPlot(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an opengl plot to plots folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddOpenGlPlot(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
  
   wxString name;
   name.Printf("OpenGlPlot%d", ++mNumOpenGlPlot);

   if (theGuiInterpreter->CreateSubscriber
       ("OpenGlPlot", std::string(name.c_str())) != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_REPORT, -1,
                 new GmatTreeItemData(name, GmatTree::CREATED_OPENGL_PLOT));

      Expand(item);
   }
}

//------------------------------------------------------------------------------
// void OnAddVariable(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a variable to variable folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddVariable(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
    
   // show dialog to create user parameter
   ParameterCreateDialog paramDlg(this);
   paramDlg.ShowModal();

   if (paramDlg.IsParamCreated())
   {
      wxString name = paramDlg.GetParamName();
      AppendItem(item, name, GmatTree::ICON_FILE, -1,
                 new GmatTreeItemData(name, GmatTree::CREATED_VARIABLE));
        
      Expand(item);
   }
}

//---------------------------------
// Crete popup menu
//---------------------------------

//------------------------------------------------------------------------------
// wxMenu* CreatePopupMenu(Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Create the popup menu
 *
 * @param <type> Type of object to create and add to tree
 * @return <wxMenu> Return popup menu
 */
//------------------------------------------------------------------------------
wxMenu* ResourceTree::CreatePopupMenu(Gmat::ObjectType type)
{
   unsigned int i;
   wxMenu *menu = new wxMenu;
   StringArray items = theGuiInterpreter->GetListOfFactoryItems(type);

   switch (type)
   {
   case Gmat::BURN:
      for (i=0; i<items.size(); i++)
      {        
         if (items[i] == "ImpulsiveBurn")
         {
            menu->Append(POPUP_ADD_IMPULSIVE_BURN, wxT("ImpulsiveBurn"));
         }
      }
      break;
   case Gmat::SOLVER:
      for (i=0; i<items.size(); i++)
      {        
         if (items[i] == "DifferentialCorrector")
         {
            menu->Append(POPUP_ADD_DIFF_CORR, wxT("DifferentialCorrector"));
         }
      }
      break;
   case Gmat::SUBSCRIBER:
      for (i=0; i<items.size(); i++)
      {        
         if (items[i] == "ReportFile")
         {
            menu->Append(POPUP_ADD_REPORT_FILE, wxT("ReportFile"));
         }
         else if (items[i] == "XyPlot")
         {
            menu->Append(POPUP_ADD_XY_PLOT, wxT("XYPlot"));
         }
         else if (items[i] == "OpenGlPlot")
         {
            menu->Append(POPUP_ADD_OPENGL_PLOT, wxT("OpenGLPlot"));
         }
      }
      break;
   default:
      break;
   }
    
   return menu;
}

void ResourceTree::OnBeginDrag(wxTreeEvent& event)
{
   // need to explicitly allow drag
   if ( event.GetItem() != GetRootItem() )
   {
      mDraggedItem = event.GetItem();
        
      // Get info from selected item
      GmatTreeItemData *theItem = (GmatTreeItemData *) GetItemData(mDraggedItem);
      int draggedId = theItem->GetDataType();
        
      if ((draggedId == GmatTree::DEFAULT_SPACECRAFT )            ||
          (draggedId == GmatTree::CREATED_SPACECRAFT )            ||
          (draggedId == GmatTree::DEFAULT_FORMATION_SPACECRAFT )  ||
          (draggedId == GmatTree::CREATED_FORMATION_SPACECRAFT ))
      {
         event.Allow();
      }
      else
      {
         event.Skip();
      }      
   }
}

void ResourceTree::OnEndDrag(wxTreeEvent& event)
{
   wxTreeItemId itemSrc = mDraggedItem,
      itemDst = event.GetItem();
   mDraggedItem = (wxTreeItemId)0l;

   if ( !itemDst.IsOk())
   {
      // error
      return;
   }
    

   // Get info from selected item
   GmatTreeItemData *theItem = (GmatTreeItemData *) GetItemData(itemDst);
   int destId = theItem->GetDataType();

   if ((destId == GmatTree::DEFAULT_FORMATION_FOLDER )  ||
       (destId == GmatTree::CREATED_FORMATION_FOLDER )  ||
       (destId == GmatTree::SPACECRAFT_FOLDER ))
   {
      wxString text = GetItemText(itemSrc);

      AppendItem(itemDst, text, GmatTree::ICON_SPACECRAFT, -1,
                 new GmatTreeItemData(text, GmatTree::CREATED_SPACECRAFT));
               
      if (GetChildrenCount(mSpacecraftItem) <= 1)
      {           
         Collapse(mSpacecraftItem);
      }
    
      Delete(itemSrc);
   }   
}

