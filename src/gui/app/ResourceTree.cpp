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
#include "bitmaps/function.xpm"
#include "bitmaps/matlabfunction.xpm"
#include "bitmaps/array.xpm"
#include "bitmaps/coordinatesystem.xpm"
#include "bitmaps/openglplot.xpm"
#include "bitmaps/propagator.xpm"
#include "bitmaps/variable.xpm"
#include "bitmaps/xyplot.xpm"
#include "bitmaps/default.xpm"
#include "bitmaps/tank.xpm"
#include "bitmaps/thruster.xpm"
#include <wx/string.h> // for wxArrayString

#include "GuiInterpreter.hpp"
#include "ResourceTree.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"
#include "GmatTreeItemData.hpp"
#include "ParameterCreateDialog.hpp"
#include "CoordSysCreateDialog.hpp"
#include "GmatMainFrame.hpp"

//#define DEBUG_RESOURCE_TREE 1

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
   EVT_MENU(POPUP_ADD_FUELTANK, ResourceTree::OnAddFuelTank)
   EVT_MENU(POPUP_ADD_THRUSTER, ResourceTree::OnAddThruster)
   EVT_MENU(POPUP_ADD_FORMATION, ResourceTree::OnAddFormation)
   EVT_MENU(POPUP_ADD_CONSTELLATION, ResourceTree::OnAddConstellation)
   EVT_MENU(POPUP_ADD_IMPULSIVE_BURN, ResourceTree::OnAddImpulsiveBurn)
   EVT_MENU(POPUP_ADD_FINITE_BURN, ResourceTree::OnAddFiniteBurn)
   EVT_MENU(POPUP_ADD_PROPAGATOR, ResourceTree::OnAddPropagator)
   EVT_MENU(POPUP_ADD_BODY, ResourceTree::OnAddBody)
   EVT_MENU(POPUP_ADD_DIFF_CORR, ResourceTree::OnAddDiffCorr)
   EVT_MENU(POPUP_ADD_REPORT_FILE, ResourceTree::OnAddReportFile)
   EVT_MENU(POPUP_ADD_XY_PLOT, ResourceTree::OnAddXyPlot)
   EVT_MENU(POPUP_ADD_OPENGL_PLOT, ResourceTree::OnAddOpenGlPlot)
   EVT_MENU(POPUP_ADD_VARIABLE, ResourceTree::OnAddVariable)
   EVT_MENU(POPUP_ADD_MATLAB_FUNCT, ResourceTree::OnAddMatlabFunction)
   EVT_MENU(POPUP_ADD_GMAT_FUNCT, ResourceTree::OnAddGmatFunction)
   EVT_MENU(POPUP_ADD_COORD_SYS, ResourceTree::OnAddCoordSys)
   EVT_MENU(POPUP_ADD_BARYCENTER, ResourceTree::OnAddBarycenter)
   EVT_MENU(POPUP_ADD_LIBRATION, ResourceTree::OnAddLibration)
   EVT_MENU(POPUP_OPEN, ResourceTree::OnOpen)
   EVT_MENU(POPUP_CLOSE, ResourceTree::OnClose)
   EVT_MENU(POPUP_RENAME, ResourceTree::OnRename)
   EVT_MENU(POPUP_DELETE, ResourceTree::OnDelete)
   EVT_MENU(POPUP_ADD_SCRIPT, ResourceTree::OnAddScript)
//   EVT_MENU(POPUP_NEW_SCRIPT, ResourceTree::OnNewScript)
   EVT_MENU(POPUP_REMOVE_ALL_SCRIPTS, ResourceTree::OnRemoveAllScripts)
   EVT_MENU(POPUP_REMOVE_SCRIPT, ResourceTree::OnRemoveScript)

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
   mNumFuelTank = 0;
   mNumThruster = 0;
   mNumFormation = 0;
   mNumPropagator = 0;
   mNumImpulsiveBurn = 0;
   mNumFiniteBurn = 0;
   mNumReportFile = 0;
   mNumXyPlot = 0;
   mNumOpenGlPlot = 0;
   mNumDiffCorr = 0;
   mNumVariable = 0;
   mNumFunct = 0;
   mNumCoordSys = 0;
   mNumScripts = 0;
   mNumBarycenter = 0;
   mNumLibration = 0;

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
      mNumFuelTank = 0;
      mNumThruster = 0;
      mNumFormation = 0;
      mNumPropagator = 0;
      mNumImpulsiveBurn = 0;
      mNumFiniteBurn = 0;
      mNumReportFile = 0;
      mNumXyPlot = 0;
      mNumOpenGlPlot = 0;
      mNumDiffCorr = 0;
      mNumVariable = 0;
      mNumFunct = 0;
      mNumCoordSys = 0;
      mNumBarycenter = 0;
      mNumLibration = 0;
   }
   
   // ag: collapse, so folder icon is closed
   Collapse(mSpacecraftItem);
   Collapse(mFormationItem);
   Collapse(mPropagatorItem);
   Collapse(mBurnItem);
   Collapse(mSolverItem);
   Collapse(mSubscriberItem);
   Collapse(mVariableItem);
   Collapse(mFunctItem);
   Collapse(mCoordSysItem);
   Collapse(mSpecialPointsItem);

   DeleteChildren(mSpacecraftItem);
   DeleteChildren(mUniverseItem);

   //----- Hardware is child of spacecraft
   mHardwareItem =
      AppendItem(mSpacecraftItem, wxT("Hardware"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Hardware"),
                                      GmatTree::HARDWARE_FOLDER));

   SetItemImage(mHardwareItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);

   //----- SpecialPoint is child of Universe (loj: 5/6/05 Added)
   mSpecialPointsItem =
      AppendItem(mUniverseItem, wxT("Special Points"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Special Points"),
                                      GmatTree::SPECIAL_POINTS_FOLDER));

   SetItemImage(mSpecialPointsItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);

   DeleteChildren(mFormationItem);
   DeleteChildren(mPropagatorItem);
   DeleteChildren(mBurnItem);
   DeleteChildren(mSolverItem);
   DeleteChildren(mSubscriberItem);
   DeleteChildren(mVariableItem);
   DeleteChildren(mFunctItem);
   DeleteChildren(mCoordSysItem);

   AddDefaultBodies(mUniverseItem);
   AddDefaultSpecialPoints(mSpecialPointsItem);
   AddDefaultSpacecraft(mSpacecraftItem);
   AddDefaultHardware(mHardwareItem);
   AddDefaultFormations(mFormationItem);
   AddDefaultPropagators(mPropagatorItem);
   AddDefaultBurns(mBurnItem);
   AddDefaultSolvers(mSolverItem);
   AddDefaultSubscribers(mSubscriberItem);
   AddDefaultVariables(mVariableItem);
   AddDefaultFunctions(mFunctItem);
   AddDefaultCoordSys(mCoordSysItem);

   theGuiManager->UpdateAll();
   ScrollTo(mSpacecraftItem);
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
    
   //----- Universe (loj: 5/6/05 made mUniverseItem member data)
   mUniverseItem =
      AppendItem(resource, wxT("Universe"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Universe"),
                                      GmatTree::UNIVERSE_FOLDER));
    
   SetItemImage(mUniverseItem, GmatTree::ICON_OPENFOLDER, 
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
   mScriptItem =
      AppendItem(resource, wxT("Scripts"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Scripts"),
                                      GmatTree::SCRIPTS_FOLDER));

   SetItemImage(mScriptItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);

// ag: commented out 3/25/05 to try adding scripts to main frame
//   wxTreeItemId subscriptItem =
//      AppendItem(resource, wxT("Subscripts"), GmatTree::ICON_FOLDER, -1,
//                 new GmatTreeItemData(wxT("Subscripts"),
//                                      GmatTree::SUBSCRIPTS_FOLDER));
//
//   SetItemImage(subscriptItem, GmatTree::ICON_OPENFOLDER,
//                wxTreeItemIcon_Expanded);

   //----- Vairables
   mVariableItem = 
      AppendItem(resource, wxT("Variables/Arrays"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Variables/Arrays"),
                                      GmatTree::VARIABLES_FOLDER));
   // ag:  Should the GmatTree type of variableItem change?

   SetItemImage(mVariableItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);

   //----- Coordinate System
   mCoordSysItem =
   AppendItem(resource, wxT("Coordinate Systems"), GmatTree::ICON_FOLDER,
              -1, new GmatTreeItemData(wxT("Coordinate Systems"),
                                       GmatTree::COORD_SYS_FOLDER));
   SetItemImage(mCoordSysItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);

   //----- Matlab functions
   mFunctItem =
      AppendItem(resource, wxT("Functions"), GmatTree::ICON_FOLDER,
              -1, new GmatTreeItemData(wxT("Functions"), GmatTree::FUNCT_FOLDER));
   SetItemImage(mFunctItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);
                
   //----- GroundStations
   AppendItem(resource, wxT("Ground Stations"), GmatTree::ICON_FOLDER, 
              -1, new GmatTreeItemData(wxT("Ground Stations"),
                                       GmatTree::GROUNDSTATIONS_FOLDER));

   AddDefaultBodies(mUniverseItem);
   AddDefaultSpecialPoints(mSpecialPointsItem);
   AddDefaultSpacecraft(mSpacecraftItem);
   AddDefaultHardware(mHardwareItem);
   AddDefaultFormations(mFormationItem);
   AddDefaultConstellations(constellationItem);
   AddDefaultPropagators(mPropagatorItem);
   AddDefaultSolvers(mSolverItem);
   AddDefaultSubscribers(mSubscriberItem);
   AddDefaultInterfaces(interfaceItem);
   AddDefaultVariables(mVariableItem);
   AddDefaultFunctions(mFunctItem);
   AddDefaultCoordSys(mCoordSysItem);
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
              new GmatTreeItemData(wxT("Sun"), GmatTree::CELESTIAL_BODY));
   AppendItem(itemId, wxT("Mercury"), GmatTree::ICON_MERCURY, -1,
              new GmatTreeItemData(wxT("Mercury"), GmatTree::CELESTIAL_BODY));
   AppendItem(itemId, wxT("Venus"), GmatTree::ICON_VENUS, -1,
              new GmatTreeItemData(wxT("Venus"), GmatTree::CELESTIAL_BODY));

   wxTreeItemId earth = AppendItem(itemId, wxT("Earth"), GmatTree::ICON_EARTH, -1,
                                   new GmatTreeItemData(wxT("Earth"), GmatTree::CELESTIAL_BODY));
   AppendItem(earth, wxT("Moon"), GmatTree::ICON_MOON, -1,
              new GmatTreeItemData(wxT("Moon"), GmatTree::CELESTIAL_BODY));

   AppendItem(itemId, wxT("Mars"), GmatTree::ICON_MARS, -1,
              new GmatTreeItemData(wxT("Mars"), GmatTree::CELESTIAL_BODY));
   AppendItem(itemId, wxT("Jupiter"), GmatTree::ICON_JUPITER, -1,
              new GmatTreeItemData(wxT("Jupiter"), GmatTree::CELESTIAL_BODY));
   AppendItem(itemId, wxT("Saturn"), GmatTree::ICON_SATURN, -1,
              new GmatTreeItemData(wxT("Saturn"), GmatTree::CELESTIAL_BODY));
   AppendItem(itemId, wxT("Uranus"), GmatTree::ICON_URANUS, -1,
              new GmatTreeItemData(wxT("Uranus"), GmatTree::CELESTIAL_BODY));
   AppendItem(itemId, wxT("Neptune"), GmatTree::ICON_NEPTUNE, -1,
              new GmatTreeItemData(wxT("Neptune"), GmatTree::CELESTIAL_BODY));
   AppendItem(itemId, wxT("Pluto"), GmatTree::ICON_PLUTO, -1,
              new GmatTreeItemData(wxT("Pluto"), GmatTree::CELESTIAL_BODY));

   //loj: 5/6/05 Commented out
   //----- Space Points
//    mSpecialPointsItem =
//       AppendItem(itemId, wxT("Special Points"), GmatTree::ICON_FOLDER, -1,
//                  new GmatTreeItemData(wxT("Special Points"),
//                                       GmatTree::SPECIAL_POINTS_FOLDER));
    
//    SetItemImage(mSpecialPointsItem, GmatTree::ICON_OPENFOLDER,
//                 wxTreeItemIcon_Expanded);



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
                 new GmatTreeItemData(wxT(objName), GmatTree::SPACECRAFT));
   };

   if (size > 0)
      Expand(itemId);
}

//------------------------------------------------------------------------------
// void AddDefaultHardware(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Add the default hardware
 *
 * @param <itemId> tree item for the hardware folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultHardware(wxTreeItemId itemId)
{
   StringArray itemNames = theGuiInterpreter->GetListOfConfiguredItems(Gmat::HARDWARE);
   int size = itemNames.size();
   wxString objName;
   wxString objTypeName;
    
   for (int i = 0; i<size; i++)
   {
      Hardware *hw = theGuiInterpreter->GetHardware(itemNames[i]);
      objName = wxString(itemNames[i].c_str());
      objTypeName = wxString(hw->GetTypeName().c_str());

      if (objTypeName == "FuelTank")
         AppendItem(itemId, wxT(objName), GmatTree::ICON_TANK, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::FUELTANK));
      else if (objTypeName == "Thruster")
         AppendItem(itemId, wxT(objName), GmatTree::ICON_THRUSTER, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::THRUSTER));
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
                 new GmatTreeItemData(wxT(objName), GmatTree::FORMATION_FOLDER));
      SetItemImage(formationItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);
      
      Formation *form = (Formation *)theGuiInterpreter->
         GuiInterpreter::GetFormation(itemNames[i].c_str());
      // get added spacecrafts
      int scListId = form->GetParameterID("Add");
      StringArray formSc = form->GetStringArrayParameter(scListId);
      int formSize = formSc.size();
      
      for (int j = 0; j<formSize; j++)
      {
         objName = wxString(formSc[j].c_str());
         AppendItem(formationItem, wxT(objName), GmatTree::ICON_SPACECRAFT, -1,
                 new GmatTreeItemData(wxT(objName), GmatTree::SPACECRAFT));
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
//                 new GmatTreeItemData(wxT("GPS"), GmatTree::CONSTELLATION_FOLDER));
//   SetItemImage(gps, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded);
//
//   AppendItem(gps, wxT("GPS1"), GmatTree::ICON_SPACECRAFT, -1,
//              new GmatTreeItemData(wxT("GPS1"), GmatTree::CONSTELLATION_SATELLITE));
//   AppendItem(gps, wxT("GPS2"), GmatTree::ICON_SPACECRAFT, -1,
//              new GmatTreeItemData(wxT("GPS2"), GmatTree::CONSTELLATION_SATELLITE));
//   AppendItem(gps, wxT("GPS3"), GmatTree::ICON_SPACECRAFT, -1,
//              new GmatTreeItemData(wxT("GPS3"), GmatTree::CONSTELLATION_SATELLITE));
//   AppendItem(gps, wxT("GPS4"), GmatTree::ICON_SPACECRAFT, -1,
//              new GmatTreeItemData(wxT("GPS4"), GmatTree::CONSTELLATION_SATELLITE));
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
      AppendItem(itemId, wxT(objName), GmatTree::ICON_PROPAGATOR, -1,
                 new GmatTreeItemData(wxT(objName), GmatTree::PROPAGATOR));
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
                                         GmatTree::IMPULSIVE_BURN));
      }
      else if (objTypeName == "FiniteBurn")
      {
         //MessageInterface::ShowMessage("ResourceTree::AddDefaultBurns() objTypeName = ImpulsiveBurn\n");
         AppendItem(itemId, wxT(objName), GmatTree::ICON_BURN, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::FINITE_BURN));
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
         AppendItem(itemId, wxT(objName), GmatTree::ICON_DEFAULT, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::DIFF_CORR));
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
                                         GmatTree::REPORT_FILE));
      else if (objTypeName == "XYPlot")
         AppendItem(itemId, wxT(objName), GmatTree::ICON_XY_PLOT, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::XY_PLOT));
      else if (objTypeName == "OpenGLPlot")
         AppendItem(itemId, wxT(objName), GmatTree::ICON_OPEN_GL_PLOT, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::OPENGL_PLOT));
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
              new GmatTreeItemData(wxT("TCP/IP"), GmatTree::INTERFACE));
   AppendItem(itemId, wxT("Mex"), GmatTree::ICON_DEFAULT, -1,
              new GmatTreeItemData(wxT("Mex"), GmatTree::INTERFACE));
    
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

      // append only user parameters
      // all system parameters works as Object.Property
      if (param->GetKey() == GmatParam::USER_PARAM)
      {
         AppendItem(itemId, wxT(objName), GmatTree::ICON_VARIABLE, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::VARIABLE));
      }
   };

   //theGuiManager->UpdateParameter(); //loj: 4/8/05 Not needed here
    
   //    if (size > 0)
   //        Expand(itemId);
}

//------------------------------------------------------------------------------
// void AddDefaultFunctions(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Add the default interfaces
 *
 * @param <itemId> tree item for the interfaces folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultFunctions(wxTreeItemId itemId)
{
   StringArray itemNames = GmatAppData::GetGuiInterpreter()
                     ->GetListOfConfiguredItems(Gmat::FUNCTION);
   int size = itemNames.size();
   wxString objName;
   wxString objTypeName;

   for (int i = 0; i<size; i++)
   {
      Function *funct = theGuiInterpreter->GetFunction(itemNames[i]);
      objName = wxString(itemNames[i].c_str());
      objTypeName = wxString(funct->GetTypeName().c_str());

      if (objTypeName == "MatlabFunction")
         AppendItem(itemId, wxT(objName), GmatTree::ICON_MATLAB_FUNCTION, -1,
                 new GmatTreeItemData(wxT(objName), GmatTree::MATLAB_FUNCTION));
      else if (objTypeName == "GmatFunction")
         AppendItem(itemId, wxT(objName), GmatTree::ICON_FUNCTION, -1,
                 new GmatTreeItemData(wxT(objName), GmatTree::GMAT_FUNCTION));

   };
 
   //----- Predefined functions
   mPredefinedFunctItem =
      AppendItem(itemId, wxT("Predefined Functions"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Functions"),
                                      GmatTree::PREDEFINED_FUNCTIONS_FOLDER));
    
   SetItemImage(mPredefinedFunctItem, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);
                
   if (size > 0)
      Expand(itemId);

}

//------------------------------------------------------------------------------
// void AddDefaultCoordSys(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Add the default coordinate system
 *
 * @param <itemId> tree item for the coordinate system folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultCoordSys(wxTreeItemId itemId)
{
   StringArray itemNames = GmatAppData::GetGuiInterpreter()->
      GetListOfConfiguredItems(Gmat::COORDINATE_SYSTEM);
   int size = itemNames.size();
   wxString objName;

   for (int i = 0; i<size; i++)
   {
      objName = wxString(itemNames[i].c_str());
      AppendItem(itemId, wxT(objName), GmatTree::ICON_COORDINATE_SYSTEM, -1,
                 new GmatTreeItemData(wxT(objName), GmatTree::COORD_SYSTEM));
   };
   
   if (size > 0)
      Expand(itemId);
   
}

//------------------------------------------------------------------------------
// void AddDefaultSpecialPoints(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Add the default special points
 *
 * @param <itemId> tree item for the special points folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultSpecialPoints(wxTreeItemId itemId)
{
   /// @todo:  need to wait for method in gui interpreter
   //loj: 5/6/05 uncommented 
   StringArray itemNames = GmatAppData::GetGuiInterpreter()->
      GetListOfConfiguredItems(Gmat::CALCULATED_POINT);
   int size = itemNames.size();
   wxString objName;
   wxString objTypeName;

   for (int i = 0; i<size; i++)
   {
      CalculatedPoint *cp = theGuiInterpreter->GetCalculatedPoint(itemNames[i]);
      objName = wxString(itemNames[i].c_str());
      objTypeName = wxString(cp->GetTypeName().c_str());

      if (objTypeName == "Barycenter")
         AppendItem(itemId, wxT(objName), GmatTree::ICON_DEFAULT, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::BARYCENTER));
      else if (objTypeName == "LibrationPoint")
         AppendItem(itemId, wxT(objName), GmatTree::ICON_DEFAULT, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::LIBRATION_POINT));
   };
   
   if (size > 0)
      Expand(itemId);

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
      menu.Append(POPUP_ADD_SC, wxT("Add Spacecraft"));
   else if (strcmp(title, wxT("Hardware")) == 0)
   {
      menu.Append(POPUP_ADD_HARDWARE, wxT("Add"), CreatePopupMenu(Gmat::HARDWARE));
   }
   else if (strcmp(title, wxT("Formations")) == 0)
      menu.Append(POPUP_ADD_FORMATION, wxT("Add Formation"));
   else if (strcmp(title, wxT("Constellations")) == 0)
   {
      menu.Append(POPUP_ADD_CONSTELLATION, wxT("Add Constellation"));
      menu.Enable(POPUP_ADD_CONSTELLATION, FALSE);
   }   
   else if (strcmp(title, wxT("Burns")) == 0)
      menu.Append(POPUP_ADD_BURN, wxT("Add"), CreatePopupMenu(Gmat::BURN));
   else if (strcmp(title, wxT("Propagators")) == 0)
      menu.Append(POPUP_ADD_PROPAGATOR, wxT("Add Propagator"));
   else if (strcmp(title, wxT("Solvers")) == 0)
      menu.Append(POPUP_ADD_SOLVER, wxT("Add"), CreatePopupMenu(Gmat::SOLVER));
   else if (strcmp(title, wxT("Universe")) == 0)
      menu.Append(POPUP_ADD_BODY, wxT("Add Body"));
   else if (strcmp(title, wxT("Plots/Reports")) == 0)
      menu.Append(POPUP_ADD_SUBSCRIBER, _T("Add"), CreatePopupMenu(Gmat::SUBSCRIBER));
   else if (strcmp(title, wxT("Variables/Arrays")) == 0)
      menu.Append(POPUP_ADD_VARIABLE, wxT("Add Variable"));
//   else if ((dataType == GmatTree::FORMATION_FOLDER)     ||
//            (dataType == GmatTree::CONSTELLATION_FOLDER))
//   {
//      menu.Append(POPUP_ADD_SC, wxT("Add Spacecraft..."));
//      menu.AppendSeparator();
//      menu.Append(POPUP_OPEN, wxT("Open"));
//      menu.Append(POPUP_CLOSE, wxT("Close"));
//      menu.AppendSeparator();
//      menu.Append(POPUP_RENAME, wxT("Rename"));
//      menu.Append(POPUP_DELETE, wxT("Delete"));
//   }
//   else if (dataType == GmatTree::PROPAGATOR)
//   {
//      menu.Append(POPUP_OPEN, wxT("Open"));
//      menu.Append(POPUP_CLOSE, wxT("Close"));
//   }
   else if (dataType == GmatTree::FUNCT_FOLDER)
   {
      menu.Append(POPUP_ADD_MATLAB_FUNCT, wxT("Add MATLAB Function"));
      menu.Append(POPUP_ADD_GMAT_FUNCT, wxT("Add GMAT Function"));
//#if defined __USE_MATLAB__
//      menu.Enable(POPUP_ADD_MATLAB_FUNCT, TRUE);
//#else
//      menu.Enable(POPUP_ADD_MATLAB_FUNCT, FALSE);
//#endif
   }
   else if (strcmp(title, wxT("Scripts")) == 0)
   {
      menu.Append(POPUP_ADD_SCRIPT, wxT("Add Script"));
//      menu.Append(POPUP_NEW_SCRIPT, wxT("New"));
      menu.AppendSeparator();
      menu.Append(POPUP_REMOVE_ALL_SCRIPTS, wxT("Remove All"));
   }
   else if (dataType == GmatTree::SCRIPT_FILE)
   {
      menu.Append(POPUP_OPEN, wxT("Open"));
      menu.Append(POPUP_CLOSE, wxT("Close"));
      menu.AppendSeparator();
      menu.Append(GmatScript::MENU_SCRIPT_BUILD_OBJECT, wxT("Build"));
      menu.Append(GmatScript::MENU_SCRIPT_BUILD_AND_RUN, wxT("Build and Run"));
      menu.AppendSeparator();
      menu.Append(POPUP_REMOVE_SCRIPT, wxT("Remove"));
   }
   else if (dataType == GmatTree::COORD_SYS_FOLDER)
   {
      menu.Append(POPUP_ADD_COORD_SYS, wxT("Add Coordinate System"));
//      menu.Enable(POPUP_ADD_COORD_SYS, FALSE);
   }   
   else if (strcmp(title, wxT("Special Points")) == 0)
   {
      wxMenu *spMenu = new wxMenu;
      spMenu->Append(POPUP_ADD_BARYCENTER, wxT("Barycenter"));
      spMenu->Append(POPUP_ADD_LIBRATION, wxT("Libration Point"));
      menu.Append(POPUP_ADD_SPECIAL_POINT, _T("Add"), spMenu);
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

   //if newName != oldName
   if ( !newName.IsEmpty() && !(newName.IsSameAs(oldName)))
   {
      Gmat::ObjectType objType;
      
      switch (dataType)
      {
      case GmatTree::SPACECRAFT:
         objType = Gmat::SPACECRAFT;
         break;
      case GmatTree::FORMATION_FOLDER:
         objType = Gmat::FORMATION;
         break;
      case GmatTree::IMPULSIVE_BURN:
         objType = Gmat::BURN;
         break;
      case GmatTree::FINITE_BURN:
         objType = Gmat::BURN;
         break;
      case GmatTree::PROPAGATOR:
         objType = Gmat::PROPAGATOR;
         break;
      case GmatTree::DIFF_CORR:
         objType = Gmat::SOLVER;
         break;
      case GmatTree::REPORT_FILE:
      case GmatTree::XY_PLOT:
      case GmatTree::OPENGL_PLOT:
         objType = Gmat::SUBSCRIBER;
         break;
      case GmatTree::VARIABLE:
         objType = Gmat::PARAMETER;
         break;
      case GmatTree::MATLAB_FUNCTION:
      case GmatTree::GMAT_FUNCTION:
         objType = Gmat::FUNCTION;
         break;
      default:
         objType = Gmat::UNKNOWN_OBJECT;
         MessageInterface::ShowMessage
            ("ResourceTree::OnRename() unknown object type.\n");
         break;
      }


      // update item only if successful
      if (theGuiInterpreter->
          RenameConfiguredItem(objType, oldName.c_str(), newName.c_str()))
      {
         SetItemText(item, newName);
         GmatAppData::GetMainFrame()->RenameChild(selItem, newName);
         GmatTreeItemData *selItem = (GmatTreeItemData *) GetItemData(item);
         selItem->SetDesc(newName);
         theGuiManager->UpdateAll();

         // update formation which may use new spacecraft name
         if (objType == Gmat::SPACECRAFT)
         {
            Collapse(mSpacecraftItem);
            DeleteChildren(mSpacecraftItem);
            AddDefaultSpacecraft(mSpacecraftItem);

            Collapse(mFormationItem);
            DeleteChildren(mFormationItem);
            AddDefaultFormations(mFormationItem);
         }
         
         // update variables which may use new object name
         Collapse(mVariableItem);
         DeleteChildren(mVariableItem);
         AddDefaultVariables(mVariableItem);
         UpdateResource(false);
      }
      else
      {
         MessageInterface::ShowMessage
            ("ResourceTree::OnRename() Unable to rename %s to %s.\n",
             oldName.c_str(), newName.c_str());
      }
   }

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
                           (dataType == GmatTree::HARDWARE_FOLDER)       ||
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
                         
   bool isDefaultItem = ((dataType == GmatTree::PROPAGATOR)  ||
                         (dataType == GmatTree::CELESTIAL_BODY)      ||
                         (dataType == GmatTree::DIFF_CORR)   ||
                         (dataType == GmatTree::REPORT_FILE) ||
                         (dataType == GmatTree::XY_PLOT)     ||
                         (dataType == GmatTree::OPENGL_PLOT) ||
                         (dataType == GmatTree::INTERFACE));

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
      if (itemType == GmatTree::SPACECRAFT)
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
   wxIcon icons[29];

   icons[0] = wxIcon ( folder_xpm );
   icons[1] = wxIcon ( file_xpm );
   icons[2] = wxIcon ( openfolder_xpm );
   icons[3] = wxIcon ( spacecraft_xpm );
   icons[4] = wxIcon ( tank_xpm );
   icons[5] = wxIcon ( thruster_xpm );
   icons[6] = wxIcon ( sun_xpm );
   icons[7] = wxIcon ( mercury_xpm );
   icons[8] = wxIcon ( venus_xpm );
   icons[9] = wxIcon ( earth_xpm );
   icons[10] = wxIcon ( mars_xpm );
   icons[11] = wxIcon ( jupiter_xpm );
   icons[12] = wxIcon ( saturn_xpm );
   icons[13] = wxIcon ( uranus_xpm );
   icons[14] = wxIcon ( neptune_xpm );
   icons[15] = wxIcon ( pluto_xpm );
   icons[16] = wxIcon ( report_xpm );
   icons[17] = wxIcon ( network_xpm );
   icons[18] = wxIcon ( burn_xpm );
   icons[19] = wxIcon ( moon_xpm );
   icons[20] = wxIcon ( matlabfunction_xpm );
   icons[21] = wxIcon ( function_xpm );
   icons[22] = wxIcon ( array_xpm );
   icons[23] = wxIcon ( coordinatesystem_xpm );
   icons[24] = wxIcon ( openglplot_xpm );
   icons[25] = wxIcon ( propagator_xpm );
   icons[26] = wxIcon ( variable_xpm );
   icons[27] = wxIcon ( xyplot_xpm );
   icons[28] = wxIcon ( default_xpm );

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
              new GmatTreeItemData(wxT("New Body"), GmatTree::CELESTIAL_BODY));
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
                 new GmatTreeItemData(newName, GmatTree::SPACECRAFT));

      theGuiManager->UpdateSpacecraft();
  
      Expand(item);
   }
}

//------------------------------------------------------------------------------
// void OnAddFuelTank(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a spacecraft to spacecraft folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddFuelTank(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
  
   wxString withName;
   withName.Printf("FuelTank%d", ++mNumFuelTank);
   
   const std::string stdWithName = withName.c_str();

   Hardware *hw = theGuiInterpreter->CreateHardware("FuelTank", stdWithName);

   if (hw != NULL)
   {
      wxString newName = wxT(hw->GetName().c_str());
  
      AppendItem(item, newName, GmatTree::ICON_TANK, -1,
                 new GmatTreeItemData(newName, GmatTree::FUELTANK));

      //loj: 2/9/05 todo
      //theGuiManager->UpdateHardware();
  
      Expand(item);
   }
}

//------------------------------------------------------------------------------
// void OnAddThruster(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a spacecraft to spacecraft folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddThruster(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
  
   wxString withName;
   withName.Printf("Thruster%d", ++mNumThruster);
  
   const std::string stdWithName = withName.c_str();

   Hardware *hw = theGuiInterpreter->CreateHardware("Thruster", stdWithName);

   if (hw != NULL)
   {
      wxString newName = wxT(hw->GetName().c_str());
  
      AppendItem(item, newName, GmatTree::ICON_THRUSTER, -1,
                 new GmatTreeItemData(newName, GmatTree::THRUSTER));

      ///@todo loj: 2/9/05 - UpdateHardware()
      //theGuiManager->UpdateHardware();
  
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
                 new GmatTreeItemData(newName, GmatTree::FORMATION_FOLDER));
      SetItemImage(formationItem, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);

      theGuiManager->UpdateFormation();
  
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
                                      GmatTree::CONSTELLATION_FOLDER));

   SetItemImage(constellation, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

   AppendItem(constellation, wxT("GPS1"), GmatTree::ICON_SPACECRAFT, -1,
              new GmatTreeItemData(wxT("GPS1"), GmatTree::CONSTELLATION_SATELLITE));
   AppendItem(constellation, wxT("GPS2"), GmatTree::ICON_SPACECRAFT, -1,
              new GmatTreeItemData(wxT("GPS2"), GmatTree::CONSTELLATION_SATELLITE));

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
      AppendItem(item, name, GmatTree::ICON_PROPAGATOR, -1,
                 new GmatTreeItemData(name, GmatTree::PROPAGATOR));

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
                 new GmatTreeItemData(name, GmatTree::IMPULSIVE_BURN));

      Expand(item);
   }
}

//------------------------------------------------------------------------------
// void OnAddFiniteBurn(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an finite burn to burn folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddFiniteBurn(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
  
   wxString name;
   name.Printf("FiniteBurn%d", ++mNumFiniteBurn);
  
   if (theGuiInterpreter->CreateBurn
       ("FiniteBurn", std::string(name.c_str())) != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_BURN, -1,
                 new GmatTreeItemData(name, GmatTree::FINITE_BURN));

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
      AppendItem(item, name, GmatTree::ICON_DEFAULT, -1,
                 new GmatTreeItemData(name, GmatTree::DIFF_CORR));

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
                 new GmatTreeItemData(name, GmatTree::REPORT_FILE));

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
       ("XYPlot", std::string(name.c_str())) != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_XY_PLOT, -1,
                 new GmatTreeItemData(name, GmatTree::XY_PLOT));
      
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
   name.Printf("OpenGLPlot%d", ++mNumOpenGlPlot);

   if (theGuiInterpreter->CreateSubscriber
       ("OpenGLPlot", std::string(name.c_str())) != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_OPEN_GL_PLOT, -1,
                 new GmatTreeItemData(name, GmatTree::OPENGL_PLOT));

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
      //wxString name = paramDlg.GetParamName();
      wxArrayString names = paramDlg.GetParamNames();

      for (unsigned int i=0; i<names.GetCount(); i++)
      {
         AppendItem(item, names[i], GmatTree::ICON_VARIABLE, -1,
                    new GmatTreeItemData(names[i], GmatTree::VARIABLE));
      }
      
      theGuiManager->UpdateParameter(); //loj: 4/8/05 Added
      Expand(item);
   }
}

//------------------------------------------------------------------------------
// void OnAddMatlabFunction(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a matlab function to the folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddMatlabFunction(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();

   wxString withName;
   //withName.Printf("MatlabFunction%d", ++mNumMatlabFunct);

   //Get name from the user first
   withName = wxGetTextFromUser(wxT("Name: "), wxT("MATLAB function"),
                                withName, this);

   if (!withName.IsEmpty())
   {
      ++mNumFunct;
      const std::string stdWithName = withName.c_str();

      if (GmatAppData::GetGuiInterpreter()->
          CreateFunction("MatlabFunction", stdWithName))
      {
         AppendItem(item, withName, GmatTree::ICON_MATLAB_FUNCTION, -1,
                    new GmatTreeItemData(withName, GmatTree::MATLAB_FUNCTION));

         Expand(item);
      }

      SelectItem(GetLastChild(item));
   }
   //OnRename(event);
}

//------------------------------------------------------------------------------
// void OnAddGmatFunction(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a gmat function to the folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddGmatFunction(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();

   wxString withName;
   //withName.Printf("MatlabFunction%d", ++mNumMatlabFunct);

   //Get name from the user first
   withName = wxGetTextFromUser(wxT("Name: "), wxT("GMAT function"),
                                withName, this);

   if (!withName.IsEmpty())
   {
      ++mNumFunct;
      const std::string stdWithName = withName.c_str();

      if (GmatAppData::GetGuiInterpreter()->
          CreateFunction("GmatFunction", stdWithName))
      {
         AppendItem(item, withName, GmatTree::ICON_FUNCTION, -1,
                    new GmatTreeItemData(withName, GmatTree::GMAT_FUNCTION));

         Expand(item);
      }

      SelectItem(GetLastChild(item));
   }
   //OnRename(event);
}


//------------------------------------------------------------------------------
// void OnAddCoordSys(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a coordinate system to the folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddCoordSys(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();

   // show dialog to create user parameter
   CoordSysCreateDialog coordDlg(this);
   coordDlg.ShowModal();

   if (coordDlg.IsCoordCreated())
   {
      //wxString name = coordDlg.GetParamName();
      wxString name = coordDlg.GetCoordName();

      AppendItem(item, name, GmatTree::ICON_COORDINATE_SYSTEM, -1,
                    new GmatTreeItemData(name, GmatTree::COORD_SYSTEM));

      Expand(item);

      theGuiManager->UpdateCoordSystem();
   }
}

//------------------------------------------------------------------------------
// void OnAddBaryCenter(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a barycenter to special points folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddBarycenter(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();

   wxString name;
   name.Printf("Barycenter%d", ++mNumBarycenter);

   if (theGuiInterpreter->CreateCalculatedPoint("Barycenter", name.c_str()) != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_DEFAULT, -1,
                 new GmatTreeItemData(name, GmatTree::BARYCENTER));

      Expand(item);
      
      theGuiManager->UpdateCelestialPoint(); //loj: 5/6/05 Added
   }
}

//------------------------------------------------------------------------------
// void OnAddLibration(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a libration point to special points folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddLibration(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();

   wxString name;
   name.Printf("Libration%d", ++mNumLibration);

   if (theGuiInterpreter->CreateCalculatedPoint("LibrationPoint", name.c_str()) != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_DEFAULT, -1,
                 new GmatTreeItemData(name, GmatTree::LIBRATION_POINT));

      Expand(item);
      
      theGuiManager->UpdateCelestialPoint(); //loj: 5/6/05 Added
   }
}


//------------------------------------------------------------------------------
// void OnAddScript()
//------------------------------------------------------------------------------
/**
 * Add a script to the folder and open it
 *
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddScript()
{
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""),
         _T("Script files (*.script, *.m)|*.script;*.m|"\
            "Text files (*.txt, *.text)|*.txt;*.text|"\
            "All files (*.*)|*.*"));

   if (dialog.ShowModal() == wxID_OK)
   {
      wxString filename = dialog.GetFilename().c_str();
      wxString path = dialog.GetPath().c_str();

      // add item to tree
      wxTreeItemId newItem = AppendItem(mScriptItem, filename, GmatTree::ICON_DEFAULT, -1,
                    new GmatTreeItemData(path, GmatTree::SCRIPT_FILE));

      // open item
      GmatAppData::GetMainFrame()->CreateChild(
                  (GmatTreeItemData *)GetItemData(newItem));

      Expand(mScriptItem);
   }
}

//------------------------------------------------------------------------------
// void OnNewScript()
//------------------------------------------------------------------------------
/**
 * Add a script to the folder and open it
 */
//------------------------------------------------------------------------------
void ResourceTree::OnNewScript()
{
   wxString name;
   name.Printf("Script%d.script", ++mNumScripts);

   // add item to tree
   wxTreeItemId newItem = AppendItem(mScriptItem, name, GmatTree::ICON_DEFAULT, -1,
                    new GmatTreeItemData(name, GmatTree::SCRIPT_FILE));

   // open item
   GmatAppData::GetMainFrame()->CreateChild(
                  (GmatTreeItemData *)GetItemData(newItem));

   Expand(mScriptItem);
}

//------------------------------------------------------------------------------
// void OnRemoveAllScripts(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a script to the folder and open it
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnRemoveAllScripts(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();

   Collapse(item);

   while (GetChildrenCount(item) > 0)
   {
      wxTreeItemId lastChild = GetLastChild(item);
      wxString name = ((GmatTreeItemData *)GetItemData(lastChild))->GetDesc();

      // close window
      GmatAppData::GetMainFrame()->RemoveChild(name);

      // delete item
      Delete(lastChild);
   }

   // arg: should script counter go back to 1?

}

//------------------------------------------------------------------------------
// void OnRemoveScript(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a script to the folder and open it
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnRemoveScript(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   wxString name = ((GmatTreeItemData *)GetItemData(item))->GetDesc();
   wxTreeItemId parentItem = GetParent(item);

   Collapse(parentItem);

   // close window
   GmatAppData::GetMainFrame()->RemoveChild(name);
   // delete item
   Delete(item);

   Expand(parentItem);
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
         else if (items[i] == "FiniteBurn")
         {
            menu->Append(POPUP_ADD_FINITE_BURN, wxT("FiniteBurn"));
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
         else if (items[i] == "XYPlot")
         {
            menu->Append(POPUP_ADD_XY_PLOT, wxT("XYPlot"));
         }
         else if (items[i] == "OpenGLPlot")
         {
            menu->Append(POPUP_ADD_OPENGL_PLOT, wxT("OpenGLPlot"));
         }
      }
      break;
   case Gmat::HARDWARE:
      menu->Append(POPUP_ADD_FUELTANK, wxT("Fuel Tank"));
      menu->Append(POPUP_ADD_THRUSTER, wxT("Thruster"));
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
        
      if ((draggedId == GmatTree::SPACECRAFT )            ||
          (draggedId == GmatTree::FORMATION_SPACECRAFT ))
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

   if ((destId == GmatTree::FORMATION_FOLDER )  ||
       (destId == GmatTree::SPACECRAFT_FOLDER ))
   {
      wxString text = GetItemText(itemSrc);

      AppendItem(itemDst, text, GmatTree::ICON_SPACECRAFT, -1,
                 new GmatTreeItemData(text, GmatTree::SPACECRAFT));
               
      if (GetChildrenCount(mSpacecraftItem) <= 1)
      {           
         Collapse(mSpacecraftItem);
      }
    
      Delete(itemSrc);
   }   
}

void ResourceTree::AddScriptItem(wxString path)
{
   wxString filename = path;
   /// @todo change from windows directory structure
   int pos = filename.Find('\\', true);
   int numChars = filename.Length();

   if ((pos > 0) && (pos < numChars))
   {
      filename = filename.Right(numChars-pos-1);
   }

   // add item to tree
   AppendItem(mScriptItem, filename, GmatTree::ICON_DEFAULT, -1,
                    new GmatTreeItemData(path, GmatTree::SCRIPT_FILE));
}

