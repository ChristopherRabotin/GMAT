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
#include "GuiInterpreter.hpp"
#include "ResourceTree.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"
#include "GmatTreeItemData.hpp"
#include "ParameterCreateDialog.hpp"
#include "CoordSysCreateDialog.hpp"
#include "GmatMainFrame.hpp"
#include "RunScriptFolderDialog.hpp"
#include "FileManager.hpp"            // for GetPathname()
#include "FileUtil.hpp"               // for Compare()
#include "GmatGlobal.hpp"             // for SetBatchMode()
#include "StringUtil.hpp"             // for GmatStringUtil::
#include <sstream>
#include <fstream>
#include <ctime>                      // for clock()
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>

//define __ENABLE_CONSTELLATIONS__

//#define DEBUG_RESOURCE_TREE 1
//#define DEBUG_RENAME 1
//#define DEBUG_DELETE 1
//#define DEBUG_COMPARE_REPORT 1
//#define DEBUG_RUN_SCRIPT_FOLDER 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ResourceTree, wxTreeCtrl)
   EVT_TREE_ITEM_RIGHT_CLICK(-1, ResourceTree::OnItemRightClick)

   EVT_TREE_ITEM_ACTIVATED(-1, ResourceTree::OnItemActivated)
   EVT_TREE_BEGIN_LABEL_EDIT(-1, ResourceTree::OnBeginLabelEdit)
   EVT_TREE_END_LABEL_EDIT(-1, ResourceTree::OnEndLabelEdit)
   //EVT_TREE_BEGIN_DRAG(-1, ResourceTree::OnBeginDrag)
   //EVT_TREE_BEGIN_RDRAG(-1, ResourceTree::OnBeginRDrag)
   //EVT_TREE_END_DRAG(-1, ResourceTree::OnEndDrag)
   
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
   EVT_MENU(POPUP_ADD_SQP, ResourceTree::OnAddSqp)
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
   EVT_MENU(POPUP_CLONE, ResourceTree::OnClone)
   EVT_MENU(POPUP_ADD_SCRIPT, ResourceTree::OnAddScript)
   EVT_MENU(POPUP_ADD_SCRIPT_FOLDER, ResourceTree::OnAddScriptFolder)
   EVT_MENU(POPUP_RUN_SCRIPTS_FROM_FOLDER, ResourceTree::OnRunScriptsFromFolder)
   EVT_MENU(POPUP_QUIT_RUN_SCRIPTS_FROM_FOLDER, ResourceTree::OnQuitRunScriptsFromFolder)
   EVT_MENU(POPUP_REMOVE_SCRIPT_FOLDER, ResourceTree::OnRemoveScriptFolder)
   EVT_MENU(POPUP_REMOVE_ALL_SCRIPTS, ResourceTree::OnRemoveAllScripts)
   EVT_MENU(POPUP_REMOVE_SCRIPT, ResourceTree::OnRemoveScript)
   EVT_MENU(POPUP_BUILD_SCRIPT, ResourceTree::OnScriptBuildObject)
   EVT_MENU(POPUP_BUILD_AND_RUN_SCRIPT, ResourceTree::OnScriptBuildAndRun)

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
   mScriptFolderRunning = false;
   mHasUserInterrupted = false;
   mScriptAdded = false;
   
   AddIcons();
   AddDefaultResources();
   
   ResetResourceCounter();
   theGuiManager->UpdateAll();
}


//------------------------------------------------------------------------------
// void ClearResource(bool leaveScripts)
//------------------------------------------------------------------------------
void ResourceTree::ClearResource(bool leaveScripts)
{
   // ag: collapse, so folder icon is closed
   // djc: Under Linux, this crashes so it only applies to Windows
   #ifdef __WXMSW__
      Collapse(mSpacecraftItem);
      Collapse(mFormationItem);
      Collapse(mPropagatorItem);
      Collapse(mBurnItem);
      Collapse(mSolverItem);
      Collapse(mSubscriberItem);
      Collapse(mVariableItem);
      Collapse(mFunctionItem);
      Collapse(mCoordSysItem);
      Collapse(mSpecialPointsItem);
   #endif
      
   DeleteChildren(mSpacecraftItem);
   DeleteChildren(mUniverseItem);
   DeleteChildren(mFormationItem);
   DeleteChildren(mPropagatorItem);
   DeleteChildren(mBurnItem);
   DeleteChildren(mSolverItem);
   DeleteChildren(mSubscriberItem);
   DeleteChildren(mVariableItem);
   DeleteChildren(mFunctionItem);
   DeleteChildren(mCoordSysItem);
   
   
   //----- Hardware is child of spacecraft
   mHardwareItem =
      AppendItem(mSpacecraftItem, wxT("Hardware"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Hardware"),
                                      GmatTree::HARDWARE_FOLDER));

   SetItemImage(mHardwareItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);

   //----- SpecialPoint is child of Universe
   mSpecialPointsItem =
      AppendItem(mUniverseItem, wxT("Special Points"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Special Points"),
                                      GmatTree::SPECIAL_POINT_FOLDER));

   SetItemImage(mSpecialPointsItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);

   //--------------- Boundry Value Solvers is a child of solvers
   mBoundarySolverItem =
      AppendItem(mSolverItem, wxT("Boundary Value Solvers"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Boundary Value Solvers"),
                                      GmatTree::BOUNDARY_SOLVER_FOLDER));

   SetItemImage(mBoundarySolverItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);


   //--------------- Optimizers is a child of solvers
   mOptimizerItem =
      AppendItem(mSolverItem, wxT("Optimizers"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Optimizers"),
                                      GmatTree::OPTIMIZER_FOLDER));

   SetItemImage(mOptimizerItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);

}


//------------------------------------------------------------------------------
// void UpdateResource(bool restartCounter)
//------------------------------------------------------------------------------
/**
 * Delete all nodes that are not folders, add default nodes
 *
 * @param restartCounter  Restarting the counter from zero if true.
 */
//------------------------------------------------------------------------------
void ResourceTree::UpdateResource(bool restartCounter)
{
   #if DEBUG_RESOURCE_TREE
   MessageInterface::ShowMessage("ResourceTree::UpdateResource() entered\n");
   #endif
   
   if (restartCounter)
   {
      ResetResourceCounter();
      theGuiInterpreter->ResetConfigurationChanged();
   }

   ClearResource(true);

   AddDefaultBodies(mUniverseItem);
   AddDefaultSpecialPoints(mSpecialPointsItem);
   AddDefaultSpacecraft(mSpacecraftItem, restartCounter);
   AddDefaultHardware(mHardwareItem, restartCounter);
   AddDefaultFormations(mFormationItem, restartCounter);
   AddDefaultPropagators(mPropagatorItem, restartCounter);
   AddDefaultBurns(mBurnItem, restartCounter);
   AddDefaultSolvers(mSolverItem, restartCounter);
   AddDefaultSubscribers(mSubscriberItem, restartCounter);
   AddDefaultVariables(mVariableItem);
   AddDefaultFunctions(mFunctionItem);
   AddDefaultCoordSys(mCoordSysItem);

   theGuiManager->UpdateAll();
   ScrollTo(mSpacecraftItem);
}


//------------------------------------------------------------------------------
// bool AddScriptItem(wxString path)
//------------------------------------------------------------------------------
/*
 * Adds script file path and name to resource tree.
 *
 * @param  path  script full path name
 *
 * @return true if script file successfully added, false otherwise
 */
//------------------------------------------------------------------------------
bool ResourceTree::AddScriptItem(wxString path)
{
   #if DEBUG_RESOURCE_TREE_ADD_SCRIPT
   MessageInterface::ShowMessage
      ("ResourceTree::AddScriptItem() path=<%s>\n", path.c_str());
   #endif

   wxTreeItemIdValue cookie;
   wxString scriptPath, childText;
   wxTreeItemId childId = GetFirstChild(mScriptItem, cookie);
   wxTreeItemId scriptId;
   bool scriptAdded = false;
   bool hasSameName = false;
   bool hasSamePath = false;
   
   // extract file name
   wxFileName fn(path);
   wxString filename = fn.GetName();

   #if DEBUG_RESOURCE_TREE_ADD_SCRIPT
   MessageInterface::ShowMessage("   filename=%s\n", filename.c_str());
   #endif
   
   // find child with same path
   while (childId.IsOk())
   {
      childText = GetItemText(childId);
      
      #if DEBUG_RESOURCE_TREE_ADD_SCRIPT
      MessageInterface::ShowMessage
         ("ResourceTree::OnAddScript() childText=<%s>\n", childText.c_str());
      #endif
      
      scriptPath = ((GmatTreeItemData *)GetItemData(childId))->GetDesc();
      
      if (childText == filename)
      {
         hasSameName = true;
         if (scriptPath == path)
         {
            hasSamePath = true;
            break;
         }
      }
      
      childId = GetNextChild(mScriptItem, cookie);
   }
   
   // if same path not found, add to tree
   if (!hasSamePath)
   {
      if (hasSameName)
      {
         // add path to tree
         scriptId =
            AppendItem(mScriptItem, path, GmatTree::ICON_DEFAULT, -1,
                       new GmatTreeItemData(path, GmatTree::SCRIPT_FILE));
      }
      else
      {
         // add filename to tree
         scriptId =
            AppendItem(mScriptItem, filename, GmatTree::ICON_DEFAULT, -1,
                       new GmatTreeItemData(path, GmatTree::SCRIPT_FILE));
      }
      
      Expand(mScriptItem);
      scriptAdded = true;
   }
   else
   {
      //MessageInterface::PopupMessage
      //   (Gmat::INFO_, "The script file \"%s\" is already in the tree", path.c_str());
      
      scriptAdded = true;
   }
   
   return scriptAdded;
}


//------------------------------------------------------------------------------
// void UpdateFormation()
//------------------------------------------------------------------------------
/**
 * Updates Formation node.
 */
//------------------------------------------------------------------------------
void ResourceTree::UpdateFormation()
{
   DeleteChildren(mFormationItem);
   AddDefaultFormations(mFormationItem);
}


//------------------------------------------------------------------------------
// void UpdateVariable()
//------------------------------------------------------------------------------
/**
 * Updates Variable node.
 */
//------------------------------------------------------------------------------
void ResourceTree::UpdateVariable()
{
   DeleteChildren(mVariableItem);
   AddDefaultVariables(mVariableItem);
}


//------------------------------------------------------------------------------
// GmatBase* GetObject(const std::string &name)
//------------------------------------------------------------------------------
GmatBase* ResourceTree::GetObject(const std::string &name)
{
   return theGuiInterpreter->GetConfiguredObject(name);
}


//------------------------------------------------------------------------------
// void AddNode(GmatTree::ItemType itemType, const wxString &name)
//------------------------------------------------------------------------------
/**
 * Adds the node to the tree item.
 *
 * @param itemType  tree item type
 * @param name  name of the item to be added
 */
//------------------------------------------------------------------------------
void ResourceTree::AddNode(GmatTree::ItemType itemType, const wxString &name)
{
   #if DEBUG_RESOURCE_TREE
   MessageInterface::ShowMessage("ResourceTree::AddNode() entered\n");
   #endif
   
   wxTreeItemId itemId = GetTreeItemId(itemType);
   GmatTree::IconType iconType = GetTreeItemIcon(itemType);
   
   AppendItem(itemId, name, iconType, -1, new GmatTreeItemData(name, itemType));
   
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
                                      GmatTree::FORMATION_FOLDER));
   
   SetItemImage(mFormationItem, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);

   #ifdef __ENABLE_CONSTELLATIONS__
   //----- Constellations
   wxTreeItemId constellationItem =
      AppendItem(resource,
                 wxT("Constellations"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Constellations"),
                                      GmatTree::CONSTELLATION_FOLDER));
   
   SetItemImage(constellationItem, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);
   #endif
   
   //----- Burns
   mBurnItem =
      AppendItem(resource, wxT("Burns"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Burns"),
                                      GmatTree::BURN_FOLDER));
   
   SetItemImage(mBurnItem, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

   
   //----- Propagators
   mPropagatorItem =
      AppendItem(resource,
                 wxT("Propagators"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Propagators"),
                                      GmatTree::PROPAGATOR_FOLDER));
   
   SetItemImage(mPropagatorItem, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);
   
   //----- Universe
   mUniverseItem =
      AppendItem(resource, wxT("Solar System"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Solar System"),
                                      GmatTree::UNIVERSE_FOLDER));
   
   SetItemImage(mUniverseItem, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);
   
   //----- Solver
   mSolverItem =
      AppendItem(resource, wxT("Solvers"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Solvers"),
                                      GmatTree::SOLVER_FOLDER));
   
   SetItemImage(mSolverItem, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);

   //----- Subscribers
   mSubscriberItem =
      AppendItem(resource, wxT("Plots/Reports"), GmatTree::ICON_FOLDER,
                 -1, new GmatTreeItemData(wxT("Plots/Reports"),
                                          GmatTree::SUBSCRIBER_FOLDER));
   
   SetItemImage(mSubscriberItem, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);
   
   //----- Interfaces
   wxTreeItemId interfaceItem =
      AppendItem(resource, wxT("Interfaces"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Interfaces"),
                                      GmatTree::INTERFACE_FOLDER));
   
   SetItemImage(interfaceItem, GmatTree::ICON_OPENFOLDER, 
                wxTreeItemIcon_Expanded);
   
   //----- Scripts
   mScriptItem =
      AppendItem(resource, wxT("Scripts"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Scripts"),
                                      GmatTree::SCRIPT_FOLDER));
   
   SetItemImage(mScriptItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);
   
   //----- Vairables
   mVariableItem = 
      AppendItem(resource, wxT("Variables/Arrays"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Variables/Arrays"),
                                      GmatTree::VARIABLE_FOLDER));
   // ag:  Should the GmatTree type of variableItem change?
   
   SetItemImage(mVariableItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);
   
   //----- Coordinate System
   mCoordSysItem =
   AppendItem(resource, wxT("Coordinate Systems"), GmatTree::ICON_FOLDER,
              -1, new GmatTreeItemData(wxT("Coordinate Systems"),
                                       GmatTree::COORD_SYSTEM_FOLDER));
   SetItemImage(mCoordSysItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);
   
   //----- Functions
   mFunctionItem =
      AppendItem(resource, wxT("Functions"), GmatTree::ICON_FOLDER,
              -1, new GmatTreeItemData(wxT("Functions"), GmatTree::FUNCTION_FOLDER));
   SetItemImage(mFunctionItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);
   
   AddDefaultBodies(mUniverseItem);
   AddDefaultSpecialPoints(mSpecialPointsItem);
   AddDefaultSpacecraft(mSpacecraftItem);
   AddDefaultHardware(mHardwareItem);
   AddDefaultFormations(mFormationItem);

   #ifdef __ENABLE__CONSTELLATIONS__
   AddDefaultConstellations(constellationItem);
   #endif
   
   AddDefaultPropagators(mPropagatorItem);
   AddDefaultSolvers(mSolverItem);
   AddDefaultSubscribers(mSubscriberItem);
   AddDefaultInterfaces(interfaceItem);
   AddDefaultVariables(mVariableItem);
   AddDefaultFunctions(mFunctionItem);
   AddDefaultCoordSys(mCoordSysItem);
   
   theGuiInterpreter->ResetConfigurationChanged(true, false);
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
   
   wxTreeItemId earth =
      AppendItem(itemId, wxT("Earth"), GmatTree::ICON_EARTH, -1,
                 new GmatTreeItemData(wxT("Earth"), GmatTree::CELESTIAL_BODY));
   AppendItem(earth, wxT("Luna"), GmatTree::ICON_MOON, -1,
              new GmatTreeItemData(wxT("Luna"), GmatTree::CELESTIAL_BODY));
   
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

}


//------------------------------------------------------------------------------
// void AddDefaultSpacecraft(wxTreeItemId itemId, bool restartCounter = true)
//------------------------------------------------------------------------------
/**
 * Add the default spacecraft
 *
 * @param <itemId> tree item for the spacecraft folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultSpacecraft(wxTreeItemId itemId, bool restartCounter)
{
   StringArray itemNames = theGuiInterpreter->GetListOfObjects(Gmat::SPACECRAFT);
   int size = itemNames.size();
   wxString objName;
   
   //MessageInterface::ShowMessage
   //   ("ResourceTree::AddDefaultSpacecraft() size=%d\n", size);
   
   for (int i = 0; i<size; i++)
   {
      if (restartCounter)
         ++mNumSpacecraft;

      objName = wxString(itemNames[i].c_str());
      
      //MessageInterface::ShowMessage
      //   ("ResourceTree::AddDefaultSpacecraft() objName=%s\n", objName.c_str());
          
      AppendItem(itemId, wxT(objName), GmatTree::ICON_SPACECRAFT, -1,
                 new GmatTreeItemData(wxT(objName), GmatTree::SPACECRAFT));
   };
   
   if (size == 0)
      mNumSpacecraft = 0;
   
   if (size > 0)
      Expand(itemId);   
}


//------------------------------------------------------------------------------
// void AddDefaultHardware(wxTreeItemId itemId, bool restartCounter = true)
//------------------------------------------------------------------------------
/**
 * Add the default hardware
 *
 * @param <itemId> tree item for the hardware folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultHardware(wxTreeItemId itemId, bool restartCounter)
{
   StringArray itemNames = theGuiInterpreter->GetListOfObjects(Gmat::HARDWARE);
   int size = itemNames.size();
   wxString objName;
   wxString objTypeName;
   int numFuelTank = 0;
   int numThruster = 0;
   
   for (int i = 0; i<size; i++)
   {
      GmatBase *hw = GetObject(itemNames[i]);
      objName = wxString(itemNames[i].c_str());
      objTypeName = wxString(hw->GetTypeName().c_str());
      
      if (objTypeName == "FuelTank") 
      {
         numFuelTank++;

         if (restartCounter)
            ++mNumFuelTank;
         
         AppendItem(itemId, wxT(objName), GmatTree::ICON_TANK, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::FUELTANK));
      }
      else if (objTypeName == "Thruster")
      {
         numThruster++;

         if (restartCounter)
            ++mNumThruster;
         
         AppendItem(itemId, wxT(objName), GmatTree::ICON_THRUSTER, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::THRUSTER));
      }
   };

   if (numFuelTank == 0)
      mNumFuelTank = 0;

   if (numThruster == 0)
      mNumThruster = 0;

   if (size > 0)
      Expand(itemId);
}


//------------------------------------------------------------------------------
// void AddDefaultFormations(wxTreeItemId itemId, bool restartCounter = true)
//------------------------------------------------------------------------------
/**
 * Add the default formations
 *
 * @param <itemId> tree item for the formation folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultFormations(wxTreeItemId itemId, bool restartCounter)
{
   StringArray itemNames = theGuiInterpreter->GetListOfObjects(Gmat::FORMATION);
   int size = itemNames.size();
   wxString objName;
   
   for (int i = 0; i<size; i++)
   {
      if (restartCounter)
         ++mNumFormation;
      
      objName = wxString(itemNames[i].c_str());
      
      wxTreeItemId formationItem =
      AppendItem(itemId, wxT(objName), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT(objName), GmatTree::FORMATION));
      SetItemImage(formationItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);
      
      GmatBase *form = GetObject(itemNames[i].c_str());
      
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

   if (size == 0)
      mNumFormation = 0;
   
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
// void AddDefaultPropagators(wxTreeItemId itemId, bool restartCounter = true)
//------------------------------------------------------------------------------
/**
 * Add the default propagator
 *
 * @param <itemId> tree item for the propagators folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultPropagators(wxTreeItemId itemId, bool restartCounter)
{
   StringArray itemNames = theGuiInterpreter->GetListOfObjects(Gmat::PROP_SETUP);
   int size = itemNames.size();
   wxString objName;
   
   for (int i = 0; i<size; i++)
   {
      if (restartCounter)
         ++mNumPropagator;
      
      objName = wxString(itemNames[i].c_str());
      AppendItem(itemId, wxT(objName), GmatTree::ICON_PROPAGATOR, -1,
                 new GmatTreeItemData(wxT(objName), GmatTree::PROPAGATOR));
   };

   if (size == 0)
      mNumPropagator = 0;
   
   if (size > 0)
      Expand(itemId);
}

//------------------------------------------------------------------------------
// void AddDefaultBurns(wxTreeItemId itemId, bool restartCounter = true)
//------------------------------------------------------------------------------
/**
 * Add the default burns
 *
 * @param <itemId> tree item for the burn folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultBurns(wxTreeItemId itemId, bool restartCounter)
{
   //MessageInterface::ShowMessage("ResourceTree::AddDefaultBurns() entered\n");
   StringArray itemNames = theGuiInterpreter->GetListOfObjects(Gmat::BURN);
   int size = itemNames.size();
   wxString objName;
   wxString objTypeName;
   int numImpBurn = 0;
   int numFiniteBurn = 0;
   
   for (int i = 0; i<size; i++)
   {
      GmatBase *burn = GetObject(itemNames[i]);
      objName = wxString(itemNames[i].c_str());
      objTypeName = wxString(burn->GetTypeName().c_str());

      if (objTypeName == "ImpulsiveBurn")
      {
         numImpBurn++;

         if (restartCounter)
            ++mNumImpulsiveBurn;
         
         //MessageInterface::ShowMessage
         //   ("ResourceTree::AddDefaultBurns() objTypeName = ImpulsiveBurn\n");
         
         AppendItem(itemId, wxT(objName), GmatTree::ICON_BURN, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::IMPULSIVE_BURN));
      }
      else if (objTypeName == "FiniteBurn")
      {
         numFiniteBurn++;

         if (restartCounter)
            ++mNumFiniteBurn;
         
         //MessageInterface::ShowMessage
         //   ("ResourceTree::AddDefaultBurns() objTypeName = ImpulsiveBurn\n");
         
         AppendItem(itemId, wxT(objName), GmatTree::ICON_BURN, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::FINITE_BURN));
      }
   };

   if (numImpBurn == 0)
      mNumImpulsiveBurn = 0;
   
   if (numFiniteBurn == 0)
      mNumFiniteBurn = 0;
   
   if (size > 0)
      Expand(itemId);
}

//------------------------------------------------------------------------------
// void AddDefaultSolvers(wxTreeItemId itemId, bool restartCounter = true)
//------------------------------------------------------------------------------
/**
 * Add the default solvers
 *
 * @param <itemId> tree item for the solvers folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultSolvers(wxTreeItemId itemId, bool restartCounter)
{
   StringArray itemNames = theGuiInterpreter->GetListOfObjects(Gmat::SOLVER);
   int size = itemNames.size();
   wxString objName;
   wxString objTypeName;
   int numDiffCorr = 0;
   int numSqp = 0;
   
   for (int i = 0; i<size; i++)
   {
      GmatBase *solver = GetObject(itemNames[i]);
      objName = wxString(itemNames[i].c_str());
      objTypeName = wxString(solver->GetTypeName().c_str());

      /// @todo:  need to create different types for the solvers and check strings
      if (objTypeName == "DifferentialCorrector")
      {
         numDiffCorr++;

         if (restartCounter)
            ++mNumDiffCorr;
         
         AppendItem(mBoundarySolverItem, wxT(objName), GmatTree::ICON_DEFAULT, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::DIFF_CORR));
      }
      else if (objTypeName == "Broyden")
      {
         AppendItem(mBoundarySolverItem, wxT(objName), GmatTree::ICON_DEFAULT, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::BROYDEN));
      }
      else if (objTypeName == "Quasi-Newton")
      {
         AppendItem(mOptimizerItem, wxT(objName), GmatTree::ICON_DEFAULT, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::QUASI_NEWTON));
      }
      else if (objTypeName == "FminconOptimizer")
      {
         numSqp++;

         if (restartCounter)
            ++mNumSqp;
         
         AppendItem(mOptimizerItem, wxT(objName), GmatTree::ICON_DEFAULT, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::SQP));
        
//         AppendItem(mOptimizerItem, wxT(objName), GmatTree::ICON_DEFAULT, -1,
//                    new GmatTreeItemData(wxT(objName), GmatTree::SQP));
      }
   };

   if (numDiffCorr == 0)
      mNumDiffCorr = 0;

   if (numSqp == 0)
      mNumSqp = 0;
   
   if (size > 0)
   {
      Expand(mBoundarySolverItem);
      Expand(mOptimizerItem);
      Expand(itemId);
   }
}


//------------------------------------------------------------------------------
// void AddDefaultSubscribers(wxTreeItemId itemId, bool restartCounter)
//------------------------------------------------------------------------------
/**
 * Add the default subscribers
 *
 * @param <itemId> tree item for the subscribers folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultSubscribers(wxTreeItemId itemId, bool restartCounter)
{
   StringArray itemNames = theGuiInterpreter->GetListOfObjects(Gmat::SUBSCRIBER);
   int size = itemNames.size();
   wxString objName;
   wxString objTypeName;
   int numReportFile = 0;
   int numXyPlot = 0;
   int numGlPlot = 0;
   
   for (int i = 0; i<size; i++)
   {
      GmatBase *sub = GetObject(itemNames[i]);
      objName = wxString(itemNames[i].c_str());
      objTypeName = wxString(sub->GetTypeName().c_str());

      if (objTypeName == "ReportFile")
      {
         numReportFile++;
         
         if (restartCounter)
            ++mNumReportFile;
         
         AppendItem(itemId, wxT(objName), GmatTree::ICON_REPORT, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::REPORT_FILE));
      }
      else if (objTypeName == "XYPlot")
      {
         numXyPlot++;
         
         if (restartCounter)
             ++mNumXyPlot;
         
         AppendItem(itemId, wxT(objName), GmatTree::ICON_XY_PLOT, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::XY_PLOT));
      }
      else if (objTypeName == "OpenGLPlot")
      {
         numGlPlot++;
         
         if (restartCounter)
            ++mNumOpenGlPlot;
         
         AppendItem(itemId, wxT(objName), GmatTree::ICON_OPEN_GL_PLOT, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::OPENGL_PLOT));
      }
   }

   if (numReportFile == 0)
      mNumReportFile = 0;

   if (numXyPlot == 0)
      mNumXyPlot = 0;

   if (numGlPlot == 0)
      mNumOpenGlPlot = 0;
   
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
   #ifdef __USE_MATLAB__
   AppendItem(itemId, wxT("Matlab"), GmatTree::ICON_DEFAULT, -1,
              new GmatTreeItemData(wxT("Matlab"), GmatTree::MATLAB_INTERFACE));
   AppendItem(itemId, wxT("Matlab Server"), GmatTree::ICON_DEFAULT, -1,
              new GmatTreeItemData(wxT("Matlab Server"), GmatTree::MATLAB_SERVER));
   #endif
   
   Expand(itemId);
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
   StringArray itemNames = theGuiInterpreter->GetListOfObjects(Gmat::PARAMETER);
   int size = itemNames.size();
   wxString objName;
   Parameter *param;
   
   for (int i = 0; i<size; i++)
   {
      objName = wxString(itemNames[i].c_str());
      param = (Parameter*)GetObject(itemNames[i]);
      
      // append only user parameters
      // all system parameters works as Object.Property
      if (param->GetKey() == GmatParam::USER_PARAM)
      {
         AppendItem(itemId, wxT(objName), GmatTree::ICON_VARIABLE, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::VARIABLE));
      }
   };
   
   if (size > 0)
      Expand(itemId);
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
   StringArray itemNames = GmatAppData::GetGuiInterpreter()->
      GetListOfObjects(Gmat::FUNCTION);
   int size = itemNames.size();
   wxString objName;
   wxString objTypeName;

   for (int i = 0; i<size; i++)
   {
      GmatBase *funct = GetObject(itemNames[i]);
      objName = wxString(itemNames[i].c_str());
      objTypeName = wxString(funct->GetTypeName().c_str());

      if (objTypeName == "MatlabFunction")
      {
         AppendItem(itemId, wxT(objName), GmatTree::ICON_MATLAB_FUNCTION, -1,
                 new GmatTreeItemData(wxT(objName), GmatTree::MATLAB_FUNCTION));
         
         #ifndef __USE_MATLAB__
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "MATLAB Interface is disabled.  GMAT will not run\n"
             "if any CallFunction uses MATLAB function: %s\n", objName.c_str());
         #endif
      }
      else if (objTypeName == "GmatFunction")
      {
         AppendItem(itemId, wxT(objName), GmatTree::ICON_FUNCTION, -1,
                 new GmatTreeItemData(wxT(objName), GmatTree::GMAT_FUNCTION));
      }
   }
   
   //----- Predefined functions
   mPredefinedFunctItem =
      AppendItem(itemId, wxT("Predefined Functions"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Functions"),
                                      GmatTree::PREDEFINED_FUNCTION_FOLDER));
    
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
      GetListOfObjects(Gmat::COORDINATE_SYSTEM);
   int size = itemNames.size();
   wxString objName;

   for (int i = 0; i<size; i++)
   {
      objName = wxString(itemNames[i].c_str());
      if (objName == "EarthMJ2000Eq" || objName == "EarthMJ2000Ec" ||
          objName == "EarthFixed")
      {
         AppendItem(itemId, wxT(objName), GmatTree::ICON_COORDINATE_SYSTEM, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::COORD_SYSTEM));
      }
      else
      {
         AppendItem(itemId, wxT(objName), GmatTree::ICON_COORDINATE_SYSTEM, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::USER_COORD_SYSTEM));
      }
   };
   
   if (size > 0)
      Expand(itemId);
   
}

//------------------------------------------------------------------------------
// void AddDefaultSpecialPoints(wxTreeItemId itemId, bool incLibCounter = true,
//                              bool restartCounter = true)
//------------------------------------------------------------------------------
/**
 * Add the default special points
 *
 * @param <itemId> tree item for the special points folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultSpecialPoints(wxTreeItemId itemId, bool incLibCounter,
                                           bool restartCounter)
{
   StringArray itemNames = GmatAppData::GetGuiInterpreter()->
      GetListOfObjects(Gmat::CALCULATED_POINT);
   int size = itemNames.size();
   wxString objName;
   wxString objTypeName;
   int numBary = 0;
   int numLib = 0;
   
   for (int i = 0; i<size; i++)
   {
      GmatBase *cp = GetObject(itemNames[i]);
      objName = wxString(itemNames[i].c_str());
      objTypeName = wxString(cp->GetTypeName().c_str());

      if (objTypeName == "Barycenter")
      {
         numBary++;

         if (restartCounter)
            ++mNumBarycenter;
         
         AppendItem(itemId, wxT(objName), GmatTree::ICON_DEFAULT, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::BARYCENTER));
      }
      else if (objTypeName == "LibrationPoint")
      {
         if (incLibCounter)
         {
            numLib++;

            if (restartCounter)
               ++mNumLibration;
         }
         
         AppendItem(itemId, wxT(objName), GmatTree::ICON_DEFAULT, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::LIBRATION_POINT));
      }
   };

   if (numBary == 0)
      mNumBarycenter = 0;

   if (numLib == 0)
      mNumLibration = 0;
   
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
   //wxWidgets-2.6.3 does not need this but wxWidgets-2.8.0 needs to SelectItem
   SelectItem(event.GetItem());
   ShowMenu(event.GetItem(), event.GetPoint());
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
   #if DEBUG_RENAME
   MessageInterface::ShowMessage("ResourceTree::OnRename() entered\n");
   #endif
   
   wxTreeItemId item = GetSelection();
   GmatTreeItemData *selItem = (GmatTreeItemData *) GetItemData(item);
   wxString oldName = selItem->GetDesc();
   GmatTree::ItemType itemType = selItem->GetItemType();
   
   wxString newName = oldName;
   newName = wxGetTextFromUser(wxT("New name: "), wxT("Input Text"),
                               newName, this);
   
   if ( !newName.IsEmpty() && !(newName.IsSameAs(oldName)))
   {
      Gmat::ObjectType objType = GetObjectType(itemType);
      
      #if DEBUG_RENAME
      MessageInterface::ShowMessage
         ("ResourceTree::OnRename() objType = %d\n", objType);
      #endif
      
      if (objType == Gmat::UNKNOWN_OBJECT)
         return;
      
      // update item only if successful
      if (theGuiInterpreter->
          RenameObject(objType, oldName.c_str(), newName.c_str()))
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
         
         //UpdateResource(false);
         //UpdateResource(true);
      }
      else
      {
         MessageInterface::ShowMessage
            ("ResourceTree::OnRename() Unable to rename %s to %s.\n",
             oldName.c_str(), newName.c_str());
      }
   }
   
   #if DEBUG_RENAME
   MessageInterface::ShowMessage("ResourceTree::OnRename() rename completed\n");
   #endif
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
   
   wxTreeItemId item = GetSelection();
   GmatTreeItemData *selItem = (GmatTreeItemData *) GetItemData(item);
   GmatTree::ItemType itemType = selItem->GetItemType();
   
   Gmat::ObjectType objType = GetObjectType(itemType);
   if (objType == Gmat::UNKNOWN_OBJECT)
      return;

   #if DEBUG_DELETE
   MessageInterface::ShowMessage
      ("ResourceTree::OnDelete() name=%s\n", selItem->GetDesc().c_str());
   #endif
   
   // delete item if object successfully deleted
   //if (theGuiInterpreter->RemoveItemIfNotUsed(objType, selItem->GetDesc().c_str()))
   if (theGuiInterpreter->RemoveObjectIfNotUsed(objType, selItem->GetDesc().c_str()))
   {
      //wxTreeItemId parentId = GetPrevVisible(item);
      wxTreeItemId parentId = GetItemParent(item);
      //this->Collapse(parentId);
      this->Delete(item);
      
      theGuiManager->UpdateAll();
      UpdateResourceCounter(parentId);
      
      //loj: 2/14/06 I think we can just delete a node without updating the tree.
      //UpdateResource(false);
   }
   else
   {
      wxLogWarning(selItem->GetDesc() +
                   " cannot be deleted.\n It is currently used in other object(s).");
      wxLog::FlushActive();
   }
}


//------------------------------------------------------------------------------
// void OnClone(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Clone chosen item
 *
 * @param <event> command event
 * @todo Finish this when all items can be cloned in sandbox
 */
//------------------------------------------------------------------------------
void ResourceTree::OnClone(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   GmatTreeItemData *selItem = (GmatTreeItemData *) GetItemData(item);
   wxString name = selItem->GetDesc();
   GmatTree::ItemType itemType = selItem->GetItemType();
   
   if ( (itemType == GmatTree::SPACECRAFT) ||
        (itemType == GmatTree::FUELTANK) ||
        (itemType == GmatTree::THRUSTER) ||
        (itemType == GmatTree::IMPULSIVE_BURN) ||
        (itemType == GmatTree::FINITE_BURN) ||
        (itemType == GmatTree::PROPAGATOR) ||
        (itemType == GmatTree::REPORT_FILE) ||
        (itemType == GmatTree::XY_PLOT) ||
        (itemType == GmatTree::OPENGL_PLOT) ||
        (itemType == GmatTree::DIFF_CORR) ||
        (itemType == GmatTree::SQP) ||
        (itemType == GmatTree::BARYCENTER) ||
        (itemType == GmatTree::VARIABLE) ||
        (itemType == GmatTree::LIBRATION_POINT) )
   {
      const std::string stdName = name.c_str();
      std::string newName = theGuiInterpreter->AddClone(stdName);
      
      if (newName != "")
         AddNode(itemType, newName.c_str());
      else
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "Cannot clone %s.\n", stdName.c_str());      
   }
   else
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "Cannot clone this object type yet.\n");
   }

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
                               
   int itemType = selItem->GetItemType();
   bool isDefaultFolder = ((itemType == GmatTree::RESOURCES_FOLDER)     ||
                           (itemType == GmatTree::SPACECRAFT_FOLDER)    ||
                           (itemType == GmatTree::HARDWARE_FOLDER)      ||
                           (itemType == GmatTree::FORMATION_FOLDER)     ||
                           (itemType == GmatTree::CONSTELLATION_FOLDER) ||
                           (itemType == GmatTree::BURN_FOLDER)          ||
                           (itemType == GmatTree::PROPAGATOR_FOLDER)    ||
                           (itemType == GmatTree::UNIVERSE_FOLDER)      ||
                           (itemType == GmatTree::SOLVER_FOLDER)        ||
                           (itemType == GmatTree::SUBSCRIBER_FOLDER)    ||
                           (itemType == GmatTree::INTERFACE_FOLDER)     ||
                           (itemType == GmatTree::VARIABLE_FOLDER));
                         
   bool isDefaultItem = ((itemType == GmatTree::PROPAGATOR)      ||
                         (itemType == GmatTree::CELESTIAL_BODY)  ||
                         (itemType == GmatTree::DIFF_CORR)       ||
                         (itemType == GmatTree::REPORT_FILE)     ||
                         (itemType == GmatTree::XY_PLOT)         ||
                         (itemType == GmatTree::OPENGL_PLOT)     ||
                         (itemType == GmatTree::INTERFACE));
   
   //kind of redundant because OpenPage returns false for some
   //of the default folders
   if ((GmatAppData::GetMainFrame()->IsChildOpen(selItem))  ||
       (isDefaultFolder)                                    ||
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
      int itemType = selItem->GetItemType();

      selItem->SetDesc(newLabel);
       
      // if label refers to an object reset the object name
      if (itemType == GmatTree::SPACECRAFT)
      {
         const std::string stdOldLabel = oldLabel.c_str();
         const std::string stdNewLabel = newLabel.c_str();
     
         //Spacecraft *theSpacecraft =
         //   theGuiInterpreter->GetSpacecraft(stdOldLabel);
         
         GmatBase *theSpacecraft = GetObject(stdOldLabel);
         
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
// void OnBeginDrag(wxTreeEvent& event)
//------------------------------------------------------------------------------
void ResourceTree::OnBeginDrag(wxTreeEvent& event)
{
   // need to explicitly allow drag
   if ( event.GetItem() != GetRootItem() )
   {
      mDraggedItem = event.GetItem();
        
      // Get info from selected item
      GmatTreeItemData *theItem = (GmatTreeItemData *) GetItemData(mDraggedItem);
      int draggedId = theItem->GetItemType();
        
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


//------------------------------------------------------------------------------
// void OnEndDrag(wxTreeEvent& event)
//------------------------------------------------------------------------------
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
   int destId = theItem->GetItemType();

   if ((destId == GmatTree::FORMATION )  ||
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

   //Spacecraft *sc = theGuiInterpreter->CreateSpacecraft("Spacecraft", stdWithName);
   GmatBase *sc = theGuiInterpreter->CreateObject("Spacecraft", stdWithName);

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
   
   //Hardware *hw = theGuiInterpreter->CreateHardware("FuelTank", stdWithName);
   GmatBase *hw = theGuiInterpreter->CreateObject("FuelTank", stdWithName);
   
   if (hw != NULL)
   {
      wxString newName = wxT(hw->GetName().c_str());
  
      AppendItem(item, newName, GmatTree::ICON_TANK, -1,
                 new GmatTreeItemData(newName, GmatTree::FUELTANK));

      theGuiManager->UpdateHardware();
  
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
   
   //Hardware *hw = theGuiInterpreter->CreateHardware("Thruster", stdWithName);
   GmatBase *hw = theGuiInterpreter->CreateObject("Thruster", stdWithName);
   
   if (hw != NULL)
   {
      wxString newName = wxT(hw->GetName().c_str());
  
      AppendItem(item, newName, GmatTree::ICON_THRUSTER, -1,
                 new GmatTreeItemData(newName, GmatTree::THRUSTER));
      
      theGuiManager->UpdateHardware();
      
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

   //Formation *form = theGuiInterpreter->
   //   CreateFormation("Formation", stdWithName);
   GmatBase *form = theGuiInterpreter->
      CreateObject("Formation", stdWithName);
   
   if (form != NULL)
   {
      wxString newName = wxT(form->GetName().c_str());
  
      wxTreeItemId formationItem =
      AppendItem(item, newName, GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(newName, GmatTree::FORMATION));
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

   PropSetup *propSetup = (PropSetup*)
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

   Burn *burn = (Burn*)theGuiInterpreter->
      CreateObject("ImpulsiveBurn", std::string(name.c_str()));
   
   //if (theGuiInterpreter->CreateBurn
   //    ("ImpulsiveBurn", std::string(name.c_str())) != NULL)
   if (burn != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_BURN, -1,
                 new GmatTreeItemData(name, GmatTree::IMPULSIVE_BURN));

      Expand(item);
      
      theGuiManager->UpdateBurn();
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
  
   Burn *burn = (Burn*)theGuiInterpreter->
      CreateObject("FiniteBurn", std::string(name.c_str()));
   
   //if (theGuiInterpreter->CreateBurn
   //    ("FiniteBurn", std::string(name.c_str())) != NULL)
   if (burn != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_BURN, -1,
                 new GmatTreeItemData(name, GmatTree::FINITE_BURN));

      theGuiManager->UpdateBurn();
      
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
   
   //if (theGuiInterpreter->CreateSolver
   if (theGuiInterpreter->CreateObject
       ("DifferentialCorrector", std::string(name.c_str())) != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_DEFAULT, -1,
                 new GmatTreeItemData(name, GmatTree::DIFF_CORR));
      theGuiManager->UpdateSolver();
      Expand(item);
   }
}

//------------------------------------------------------------------------------
// void OnAddSqp(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a sqp to solvers folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddSqp(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
  
   wxString name;
   name.Printf("SQP%d", ++mNumSqp);

   //if (theGuiInterpreter->CreateSolver
   if (theGuiInterpreter->CreateObject
       ("FminconOptimizer", std::string(name.c_str())) != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_DEFAULT, -1,
                 new GmatTreeItemData(name, GmatTree::SQP));
      
      theGuiManager->UpdateSolver();
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
      theGuiManager->UpdateSubscriber();
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
      theGuiManager->UpdateSubscriber();
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
      theGuiManager->UpdateSubscriber();
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
          //CreateFunction("MatlabFunction", stdWithName))
          CreateObject("MatlabFunction", stdWithName))
      {
         AppendItem(item, withName, GmatTree::ICON_MATLAB_FUNCTION, -1,
                    new GmatTreeItemData(withName, GmatTree::MATLAB_FUNCTION));

         Expand(item);
         theGuiManager->UpdateFunction();
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

   //Get name from the user first
   withName = wxGetTextFromUser(wxT("Name: "), wxT("GMAT function"),
                                withName, this);

   if (!withName.IsEmpty())
   {
      ++mNumFunct;
      const std::string stdWithName = withName.c_str();

      if (GmatAppData::GetGuiInterpreter()->
          //CreateFunction("GmatFunction", stdWithName))
          CreateObject("GmatFunction", stdWithName))
      {
         AppendItem(item, withName, GmatTree::ICON_FUNCTION, -1,
                    new GmatTreeItemData(withName, GmatTree::GMAT_FUNCTION));

         Expand(item);
         theGuiManager->UpdateFunction();
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
                 new GmatTreeItemData(name, GmatTree::USER_COORD_SYSTEM));
      
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

   //if (theGuiInterpreter->CreateCalculatedPoint("Barycenter", name.c_str()) != NULL)
   if (theGuiInterpreter->CreateObject("Barycenter", name.c_str()) != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_DEFAULT, -1,
                 new GmatTreeItemData(name, GmatTree::BARYCENTER));

      Expand(item);
      
      theGuiManager->UpdateCelestialPoint();
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

   //MessageInterface::ShowMessage
   //   ("ResourceTree::OnAddLibration() name=%s\n", name.c_str());
   
   //if (theGuiInterpreter->CreateCalculatedPoint("LibrationPoint", name.c_str()) != NULL)
   if (theGuiInterpreter->CreateObject("LibrationPoint", name.c_str()) != NULL)
   {
      DeleteChildren(mSpecialPointsItem);
      AddDefaultSpecialPoints(mSpecialPointsItem, true, false);
      
      Expand(item);
      
      theGuiManager->UpdateCelestialPoint();
   }
}


//------------------------------------------------------------------------------
// void OnAddScript()
//------------------------------------------------------------------------------
/**
 * Add a script to the folder and opens it.
 *
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddScript(wxCommandEvent &event)
{
   //---------------- debug
   //OnAddScriptFolder(event);
   //wxString cwd = wxGetCwd();
   //
   ////loj: 3/14/06 Why I need this to get the same precision as script folder?
   //wxDirDialog dirDialog(this, "Select a script directory", wxGetCwd());
   //
   //if (dirDialog.ShowModal() == wxID_OK)
   //{
   //}
   //---------------- debug
   
   mScriptAdded = false;
   
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""),
         _T("Script files (*.script, *.m)|*.script;*.m|"\
            "Text files (*.txt, *.text)|*.txt;*.text|"\
            "All files (*.*)|*.*"));
   
   
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString path = dialog.GetPath().c_str();
      mScriptAdded = AddScriptItem(path);
      if (mScriptAdded)
      {
         // need to set the filename to MainFrame
         GmatAppData::GetMainFrame()->SetScriptFileName(path.c_str());
      }
   }
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
      GmatTree::ItemType itemType =
         ((GmatTreeItemData *)GetItemData(lastChild))->GetItemType();
      
      // close window
      GmatAppData::GetMainFrame()->RemoveChild(name, itemType);
      
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
   GmatTree::ItemType itemType = ((GmatTreeItemData *)GetItemData(item))->GetItemType();
   wxTreeItemId parentItem = GetItemParent(item);
   
   //Collapse(parentItem); //loj: 8/4/05 Do we really want to collapse and expand?
   
   // close window
   GmatAppData::GetMainFrame()->RemoveChild(name, itemType);
   // delete item
   Delete(item);
   
   //Expand(parentItem);
}


//------------------------------------------------------------------------------
// bool OnScriptBuildObject(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ResourceTree::OnScriptBuildObject(wxCommandEvent& event)
{
   //MessageInterface::ShowMessage("===> OnScriptBuildObject()\n");
   
   // Get info from selected item
   GmatTreeItemData *item = (GmatTreeItemData *) GetItemData(GetSelection());
   wxString filename = item->GetDesc();
   
   BuildScript(filename);
}


//------------------------------------------------------------------------------
// bool OnScriptBuildAndRun(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ResourceTree::OnScriptBuildAndRun(wxCommandEvent& event)
{
   //MessageInterface::ShowMessage("===> ResourceTree::OnScriptBuildAndRun()\n");
   
   // Get info from selected item
   GmatTreeItemData *item = (GmatTreeItemData *) GetItemData(GetSelection());
   wxString filename = item->GetDesc();
   
   if (BuildScript(filename))
      GmatAppData::GetMainFrame()->RunCurrentMission();
   
}


//------------------------------------------------------------------------------
// void OnAddScriptFolder()
//------------------------------------------------------------------------------
/**
 * Add a script folder to the folder and open it
 *
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddScriptFolder(wxCommandEvent &event)
{
   wxDirDialog dialog(this, "Select a script directory", wxGetCwd());
   
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString dirname = dialog.GetPath();

      #if DEBUG_RESOURCE_TREE
      MessageInterface::ShowMessage("OnAddScriptFolder() dirname=%s\n",
                                    dirname.c_str());
      #endif
      
      // add item to tree
      wxTreeItemId newItem =
         AppendItem(mScriptItem, dirname, GmatTree::ICON_FOLDER, -1,
                    new GmatTreeItemData(dirname, GmatTree::ADDED_SCRIPT_FOLDER));
      
      Expand(mScriptItem);
      
      // add files under script diretory
      wxDir dir(dirname);
      wxString filename;
      wxString filepath;
      
      //How do I specify multiple file ext?
      //bool cont = dir.GetFirst(&filename, "*.script, *.m");
      bool cont = dir.GetFirst(&filename);
      while (cont)
      {
         if (filename.Contains(".script") || filename.Contains(".m"))
         {
            filepath = dirname + "/" + filename;
            
            // remove any backup files
            if (filename.Last() == 't' || filename.Last() == 'm')
            {
               // read first item to elliminate Matlab/Gmat function
               std::ifstream ifs(filepath.c_str());
               std::string item;
               ifs >> item;

               if (item != "function")
               {
                  AppendItem(newItem, filename, GmatTree::ICON_DEFAULT, -1,
                             new GmatTreeItemData(filepath, GmatTree::SCRIPT_FILE));
               }
               
               ifs.close();
            }
         }
         
         cont = dir.GetNext(&filename);
      }
   }
}


//------------------------------------------------------------------------------
// void OnRunScriptsFromFolder()
//------------------------------------------------------------------------------
/**
 * Build and run all script files in the folder.
 *
 */
//------------------------------------------------------------------------------
void ResourceTree::OnRunScriptsFromFolder(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   wxString filename;
   wxTreeItemIdValue cookie;
   wxTreeItemId scriptId = GetFirstChild(item, cookie);
   int numScripts = 0;
   
   // find only script file, exclude script folder
   while (scriptId.IsOk())
   {
      #if DEBUG_RUN_SCRIPT_FOLDER
      MessageInterface::ShowMessage
         ("ResourceTree::OnRunScriptsFromFolder() scriptText=<%s>\n",
          GetItemText(scriptId).c_str());
      #endif
      
      if (GetItemImage(scriptId) != GmatTree::ICON_FOLDER)
         numScripts++;
      
      scriptId = GetNextChild(item, cookie);
   }
   
   int runCount = numScripts;
   int repeatCount = 1;
   
   Real absTol = GmatFileUtil::CompareAbsTol;
   wxString compareDir1  = ((GmatTreeItemData*)GetItemData(item))->GetDesc();
   
   RunScriptFolderDialog dlg(this, numScripts, absTol, compareDir1);
   dlg.ShowModal();
   
   if (!dlg.RunScripts())
      return;
   
   runCount = dlg.GetNumScriptsToRun();
   repeatCount = dlg.GetNumTimesToRun();
   bool runFromSavedScripts = dlg.RunFromSavedScripts();
   bool compare = dlg.CompareResults();
   bool saveCompareResults = dlg.SaveCompareResults();
   bool builtOk = false;
   
   // for current output path
   FileManager *fm = FileManager::Instance();
   std::string oldOutPath = fm->GetFullPathname(FileManager::OUTPUT_PATH);
   std::string oldLogFile = MessageInterface::GetLogFileName();
   bool hasOutDirChanged = dlg.HasOutDirChanged();
   bool createRunFolder = dlg.CreateRunFolder();
   wxString sep = fm->GetPathSeparator().c_str();
   
   wxString currPath = dlg.GetCurrentOutDir() + sep;
   wxString savePath = dlg.GetSaveScriptsDir() + sep;
   
   // if running from saved scripts folder, set path to save path
   if (runFromSavedScripts)
   {
      currPath = savePath;
      if (!::wxDirExists(currPath))
         ::wxMkdir(currPath);
   }
   
   int count = 0;
   mHasUserInterrupted = false;
   scriptId = GetFirstChild(item, cookie);
   
   wxTextCtrl *textCtrl = NULL;
   wxString tempStr;
   
   // create CompareReport Panel
   if (compare)
   {
      GmatMdiChildFrame *textFrame =
         GmatAppData::GetMainFrame()->GetChild("CompareReport");
      
      if (textFrame == NULL)
      {
         GmatTreeItemData *compareItem =
            new GmatTreeItemData("CompareReport", GmatTree::COMPARE_REPORT);
      
         textFrame = GmatAppData::GetMainFrame()->CreateChild(compareItem);
      }
      
      textCtrl = textFrame->GetScriptTextCtrl();
      textCtrl->SetMaxLength(320000); // make long enough
      textFrame->Show();
      wxString msg;
      msg.Printf(_T("GMAT Build Date: %s %s\n\n"),  __DATE__, __TIME__);      
      textCtrl->AppendText(msg);
      
      //loj: Why Do I need to do this to show whole TextCtrl?
      // textFrame->Layout() didn't work.
      int w, h;
      textFrame->GetSize(&w, &h);
      textFrame->SetSize(w+1, h+1);
   }
   
   // Set batch mode to true, so that PopupMessage will be rerouted to ShowMessage
   GmatGlobal::Instance()->SetBatchMode(true);
   bool appendLog = false;
   mBuildErrorCount = 0;
   mFailedScriptsList.Clear();
   mScriptFolderRunning = true;
   wxArrayString failedToRunScripts;
   wxArrayString runInterruptedScripts;
   
   clock_t t1 = clock();
   
   while (scriptId.IsOk())
   {     
      if (GetItemImage(scriptId) == GmatTree::ICON_FOLDER)
      {
         scriptId = GetNextChild(item, cookie);
         continue;
      }
      
      if (mHasUserInterrupted)
         break;
      
      count++;
      
      if (count > runCount)
         break;

      // Set main frame title to script file name
      filename = ((GmatTreeItemData*)GetItemData(scriptId))->GetDesc();
      wxString titleText;
      titleText.Printf("%s - General Mission Analysis Tool (GMAT)", filename.c_str());      
      GmatAppData::GetMainFrame()->SetTitle(titleText);
      
      // Set main frame status bar text
      wxString text;
      text.Printf("Running script %d out of %d: %s\n", count, runCount,
                  filename.c_str());
      GmatAppData::GetMainFrame()->SetStatusText(text, 1);
      
      if (compare)
         textCtrl->AppendText(text);
      
      for (int i=0; i<repeatCount; i++)
      {
         wxString outPath = currPath;
         
         if (createRunFolder)
         {
            outPath << "Run_" << i+1;
            if (!::wxDirExists(outPath))
               ::wxMkdir(outPath);
            outPath = outPath + "/";
         }
         
         // Set output path
         fm->SetAbsPathname(FileManager::OUTPUT_PATH, outPath.c_str());
         
         // Change log path and append log messages
         MessageInterface::SetLogPath(outPath.c_str(), appendLog);
         
         MessageInterface::ShowMessage
            ("Starting script %d out of %d: %s\n", count, runCount, filename.c_str());
         
         MessageInterface::ShowMessage
            ("==> Run Count: %d\n", i+1);
         
         // Set main frame status bar repeat count
         wxString text;
         text.Printf("Repeat Count %d", i+1);
         GmatAppData::GetMainFrame()->SetStatusText(text, 0);
         
         if (compare)
         {
            tempStr.Printf("%d", i+1);
            textCtrl->AppendText("==> Run Count: " + tempStr + "\n");
         }
         
         try
         {
            // Create objects from script only first time, to test re-run
            if (i == 0)
               builtOk = BuildScript(filename, runFromSavedScripts, savePath);
            
            if (builtOk)
            {
               Integer retval = GmatAppData::GetMainFrame()->RunCurrentMission();
               
               // if run failed, save to report
               if (retval == -2)
                  runInterruptedScripts.Add(filename);
               else if (retval <= -3)
                  failedToRunScripts.Add(filename);
               
               if (compare)
               {
                  absTol = dlg.GetAbsTolerance();
                  CompareScriptRunResult(absTol, dlg.GetReplaceString(), compareDir1,
                                         dlg.GetCompareDirectory(), textCtrl);
               }
            }
         }
         catch(BaseException &e)
         {
            MessageInterface::ShowMessage
               ("*** Error running: %s\n   %s\n", e.GetFullMessage().c_str());
            
            if (compare)
               textCtrl->AppendText(e.GetFullMessage().c_str());
         }
      }
      
      scriptId = GetNextChild(item, cookie);
      appendLog = true;
   }
   
   mScriptFolderRunning = false;
   
   clock_t t2 = clock();
   MessageInterface::ShowMessage
      ("===> Grand Total Run Time: %f seconds\n", (Real)(t2-t1)/CLOCKS_PER_SEC);
   
   // save compare results to a file
   if (compare && saveCompareResults)
      textCtrl->SaveFile(dlg.GetSaveFilename());
   
   // reset output path
   if (hasOutDirChanged)
   {
      fm->SetAbsPathname(FileManager::OUTPUT_PATH, oldOutPath);
      //MessageInterface::SetLogFile(oldLogFile);
   }
   
   // Report completion
   wxString text;
   text.Printf("Finished running %d scripts\n", runCount);
   GmatAppData::GetMainFrame()->SetStatusText(text, 1);
   GmatAppData::GetMainFrame()->SetStatusText("", 0);
   
   // Set batch mode to false
   GmatGlobal::Instance()->SetBatchMode(false);
   
   // Popup errors found message
   wxString msg1;
   wxString msg2;
   wxString msg3;
   
   if (mBuildErrorCount > 0)
   {
      wxString scriptNames1;
      msg1 = "Script errors were found in the following script(s):\n";
      
      for (int i=0; i<mBuildErrorCount; i++)
         scriptNames1 = scriptNames1 + mFailedScriptsList[i] + "\n";

      msg1 = msg1 + scriptNames1;
   }
   
   if (failedToRunScripts.GetCount() > 0)
   {
      wxString scriptNames2;
      msg2 = "\nThe following script(s) failed to run to completion:\n";
      
      for (unsigned int i=0; i<failedToRunScripts.GetCount(); i++)
         scriptNames2 = scriptNames2 + failedToRunScripts[i] + "\n";
      
      msg2 = msg2 + scriptNames2;
   }
   
   if (runInterruptedScripts.GetCount() > 0)
   {
      wxString scriptNames3;
      msg3 = "\nThe following script(s) were interrupted by user:\n";
      
      for (unsigned int i=0; i<runInterruptedScripts.GetCount(); i++)
         scriptNames3 = scriptNames3 + runInterruptedScripts[i] + "\n";
      
      msg3 = msg3 + scriptNames3;
   }
   
   if (msg1 != "" || msg2 != "" || msg3 != "")
      MessageInterface::PopupMessage(Gmat::ERROR_, msg1 + msg2 + msg3);
   
}


//------------------------------------------------------------------------------
// void OnQuitRunScriptsFromFolder()
//------------------------------------------------------------------------------
/**
 * Sets user interrupt flag.
 *
 */
//------------------------------------------------------------------------------
void ResourceTree::OnQuitRunScriptsFromFolder(wxCommandEvent &event)
{
   int answer = wxMessageBox("Do you really want to quit after the current run?",
                             "Confirm", wxYES_NO, this);
   
   if (answer == wxYES)
      mHasUserInterrupted = true;
}


//------------------------------------------------------------------------------
// void OnRemoveScriptFolder()
//------------------------------------------------------------------------------
/**
 * Sets user interrupt flag.
 *
 */
//------------------------------------------------------------------------------
void ResourceTree::OnRemoveScriptFolder(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   
   OnRemoveAllScripts(event);
   
   Delete(item);
   Collapse(mScriptItem);
}


//------------------------------------------------------------------------------
// bool BuildScript(const wxString &filename, bool readBack = false,
//                  const wxString &savePath = "")
//------------------------------------------------------------------------------
/**
 * Creates objects from script file.
 *
 * @param <filename> input script file name
 * @param <readBack> true will read scripts, save, and read back in
 * @param <newPath> new path to be used for saving scripts
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool ResourceTree::BuildScript(const wxString &filename, bool readBack,
                               const wxString &savePath)
{
   #if DEBUG_RESOURCE_TREE
   MessageInterface::ShowMessage
      ("===> ResourceTree::BuildScript() filename=%s, readBack=%d\n   savePath=%s\n",
       filename.c_str(), readBack, savePath.c_str());
   #endif
   
   // Set the filename to mainframe, so save will not bring up the file dialog
   bool fileSet = GmatAppData::GetMainFrame()->SetScriptFileName(filename.c_str());
   
   if (fileSet)
   {
      // Interpret script
      bool status = GmatAppData::GetMainFrame()->
         InterpretScript(filename, readBack, savePath, false, true);
   
      if (!status)
      {
         mBuildErrorCount++;
         mFailedScriptsList.Add(filename);
      }
      
      return status;
   }
   
   return false;
}


//------------------------------------------------------------------------------
// void ResetResourceCounter()
//------------------------------------------------------------------------------
void ResourceTree::ResetResourceCounter()
{
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
   mNumSqp = 0;
   mNumVariable = 0;
   mNumFunct = 0;
   mNumCoordSys = 0;
   mNumScripts = 0;
   mNumBarycenter = 0;
   mNumLibration = 0;
}


//------------------------------------------------------------------------------
// void UpdateResourceCounter(wxTreeItemId itemId)
//------------------------------------------------------------------------------
void ResourceTree::UpdateResourceCounter(wxTreeItemId itemId)
{
   GmatTreeItemData *item = (GmatTreeItemData *)GetItemData(itemId);
   wxString name = item->GetDesc();

   #if DEBUG_DELETE
   MessageInterface::ShowMessage
      ("ResourceTree::UpdateResourceCounter() item=%s\n", name.c_str());
   #endif
   
   StringArray itemNames;
   int size;
   wxString objTypeName;
   
   if (itemId == mSpacecraftItem)
   {
      itemNames = theGuiInterpreter->GetListOfObjects(Gmat::SPACECRAFT);
      size = itemNames.size();
      
      if (size == 0)
         mNumSpacecraft = 0;
   }
   else if (itemId == mHardwareItem)
   {
      int numFuelTank = 0;
      int numThruster = 0;
      itemNames = theGuiInterpreter->GetListOfObjects(Gmat::HARDWARE);
      size = itemNames.size();
      
      for (int i = 0; i<size; i++)
      {
         GmatBase *hw = GetObject(itemNames[i]);
         objTypeName = wxString(hw->GetTypeName().c_str());

         if (objTypeName == "FuelTank")
            numFuelTank++;

         else if (objTypeName == "Thruster")
            numThruster++;
      }
      
      if (numFuelTank == 0)
         mNumFuelTank = 0;
      
      if (numThruster == 0)
         mNumThruster = 0;
   }
   else if (itemId == mBurnItem)
   {
      int numImpBurn = 0;
      int numFiniteBurn = 0;
      itemNames = theGuiInterpreter->GetListOfObjects(Gmat::BURN);
      size = itemNames.size();
         
      for (int i = 0; i<size; i++)
      {
         GmatBase *burn = GetObject(itemNames[i]);
         objTypeName = wxString(burn->GetTypeName().c_str());
         
         if (objTypeName == "ImpulsiveBurn")
            numImpBurn++;
         else if (objTypeName == "FiniteBurn")
            numFiniteBurn++;
      };

      if (numImpBurn == 0)
         mNumImpulsiveBurn = 0;
   
      if (numFiniteBurn == 0)
         mNumFiniteBurn = 0;
   }
   else if (itemId == mPropagatorItem)
   {
      itemNames = theGuiInterpreter->GetListOfObjects(Gmat::PROP_SETUP);
      size = itemNames.size();
      
      if (size == 0)
         mNumPropagator = 0;
   }
   else if (itemId == mBoundarySolverItem)
   {
      itemNames = theGuiInterpreter->GetListOfObjects(Gmat::SOLVER);
      size = itemNames.size();
      
      int numDiffCorr = 0;
   
      for (int i = 0; i<size; i++)
      {
         GmatBase *solver = GetObject(itemNames[i]);
         objTypeName = wxString(solver->GetTypeName().c_str());

         if (objTypeName == "DifferentialCorrector")
            numDiffCorr++;
      }
      
      if (numDiffCorr == 0)
         mNumDiffCorr = 0;
   }
   else if (itemId == mOptimizerItem)
   {
      itemNames = theGuiInterpreter->GetListOfObjects(Gmat::SOLVER);
      size = itemNames.size();
      
      int numSqp = 0;
   
      for (int i = 0; i<size; i++)
      {
         GmatBase *solver = GetObject(itemNames[i]);
         objTypeName = wxString(solver->GetTypeName().c_str());

         if (objTypeName == "FminconOptimizer")
            numSqp++;
      }
      
      if (numSqp == 0)
         mNumSqp = 0;
   }
   else if (itemId == mSubscriberItem)
   {
      itemNames = theGuiInterpreter->GetListOfObjects(Gmat::SUBSCRIBER);
      size = itemNames.size();
      int numReportFile = 0;
      int numXyPlot = 0;
      int numGlPlot = 0;
      
      for (int i=0; i<size; i++)
      {
         GmatBase *sub = GetObject(itemNames[i]);
         objTypeName = wxString(sub->GetTypeName().c_str());
         
         if (objTypeName == "ReportFile")
            numReportFile++;
         else if (objTypeName == "XYPlot")
            numXyPlot++;
         else if (objTypeName == "OpenGLPlot")
            numGlPlot++;
      }
      
      if (numReportFile == 0)
         mNumReportFile = 0;

      if (numXyPlot == 0)
         mNumXyPlot = 0;

      if (numGlPlot == 0)
         mNumOpenGlPlot = 0;
   }
   else if (itemId == mSpecialPointsItem)
   {
      itemNames = theGuiInterpreter->GetListOfObjects(Gmat::CALCULATED_POINT);
      size = itemNames.size();
      int numBary = 0;
      int numLib = 0;
      
      for (int i=0; i<size; i++)
      {
         GmatBase *cp = GetObject(itemNames[i]);
         objTypeName = wxString(cp->GetTypeName().c_str());

         if (objTypeName == "Barycenter")
            numBary++;
         
         else if (objTypeName == "LibrationPoint")
            numLib++;

      }

      if (numBary == 0)
         mNumBarycenter = 0;

      if (numLib == 0)
         mNumLibration = 0;
   }
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
   GmatTree::ItemType itemType = treeItem->GetItemType();
   
   //MessageInterface::ShowMessage
   //   ("===> ResourceTree::ShowMenu() title=%s, itemType=%d\n", title.c_str(),
   //    itemType);
   
#if wxUSE_MENUS
   wxMenu menu;

   switch (itemType)
   {
   case GmatTree::SPACECRAFT_FOLDER:
      menu.Append(POPUP_ADD_SC, wxT("Add Spacecraft"));
      break;
   case GmatTree::HARDWARE_FOLDER:
      menu.Append(POPUP_ADD_HARDWARE, wxT("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::FORMATION_FOLDER:
      menu.Append(POPUP_ADD_FORMATION, wxT("Add Formation"));
      break;
   case GmatTree::BURN_FOLDER:
      menu.Append(POPUP_ADD_BURN, wxT("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::PROPAGATOR_FOLDER:
      menu.Append(POPUP_ADD_PROPAGATOR, wxT("Add Propagator"));
      break;
   case GmatTree::BOUNDARY_SOLVER_FOLDER:
      menu.Append(POPUP_ADD_BOUNDARY_SOLVER, wxT("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::OPTIMIZER_FOLDER:
      menu.Append(POPUP_ADD_OPTIMIZER, wxT("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::UNIVERSE_FOLDER:
      menu.Append(POPUP_ADD_BODY, wxT("Add Body"));
      menu.Enable(POPUP_ADD_BODY, false);
      break;
   case GmatTree::SPECIAL_POINT_FOLDER:
      menu.Append(POPUP_ADD_SPECIAL_POINT, _T("Add"), CreatePopupMenu(itemType));
      break;
      
   #ifdef __USE_MATLAB__
   case GmatTree::MATLAB_INTERFACE:
      menu.Append(GmatMenu::MENU_TOOLS_MATLAB_OPEN, wxT("Open"));
      menu.Append(GmatMenu::MENU_TOOLS_MATLAB_CLOSE, wxT("Close"));
      break;
   case GmatTree::MATLAB_SERVER:
      menu.Append(GmatMenu::MENU_START_SERVER, wxT("Start"));
      menu.Append(GmatMenu::MENU_STOP_SERVER, wxT("Stop"));
      break;
   #endif
      
   case GmatTree::SUBSCRIBER_FOLDER:
      menu.Append(POPUP_ADD_SUBSCRIBER, wxT("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::VARIABLE_FOLDER:
      menu.Append(POPUP_ADD_VARIABLE, wxT("Add Variable"));
      break;
   case GmatTree::FUNCTION_FOLDER:
      menu.Append(POPUP_ADD_FUNCTION, wxT("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::SCRIPT_FOLDER:
      menu.Append(POPUP_ADD_SCRIPT, wxT("Add Script"));
      menu.Append(POPUP_ADD_SCRIPT_FOLDER, wxT("Add Script Folder"));
      menu.AppendSeparator();
      menu.Append(POPUP_REMOVE_ALL_SCRIPTS, wxT("Remove All"));
      break;
   case GmatTree::ADDED_SCRIPT_FOLDER:
      menu.Append(POPUP_RUN_SCRIPTS_FROM_FOLDER, wxT("Run Scripts"));
      menu.Append(POPUP_QUIT_RUN_SCRIPTS_FROM_FOLDER, wxT("Quit Running Scripts"));
      
      if (mScriptFolderRunning)
      {
         menu.Enable(POPUP_RUN_SCRIPTS_FROM_FOLDER, false);
         menu.Enable(POPUP_QUIT_RUN_SCRIPTS_FROM_FOLDER, true);
      }
      else
      {
         menu.Enable(POPUP_RUN_SCRIPTS_FROM_FOLDER, true);
         menu.Enable(POPUP_QUIT_RUN_SCRIPTS_FROM_FOLDER, false);
      }
      menu.AppendSeparator();
      menu.Append(POPUP_REMOVE_ALL_SCRIPTS, wxT("Remove All"));
      break;
   case GmatTree::SCRIPT_FILE:
      menu.Append(POPUP_OPEN, wxT("Open"));
      menu.Append(POPUP_CLOSE, wxT("Close"));
      menu.AppendSeparator();
      menu.Append(POPUP_BUILD_SCRIPT, wxT("Build"));
      menu.Append(POPUP_BUILD_AND_RUN_SCRIPT, wxT("Build and Run"));
      menu.AppendSeparator();
      menu.Append(POPUP_REMOVE_SCRIPT, wxT("Remove"));
      break;
   case GmatTree::COORD_SYSTEM_FOLDER:
      menu.Append(POPUP_ADD_COORD_SYS, wxT("Add Coordinate System"));
      break;
   default:
      break;
   }
   
   // menu items applies to most non-folder items
   if (itemType >= GmatTree::BEGIN_OF_RESOURCE &&
       itemType <= GmatTree::END_OF_RESOURCE)
   {
      switch (itemType)
      {
      case GmatTree::CELESTIAL_BODY:
      case GmatTree::COORD_SYSTEM:
         menu.Append(POPUP_OPEN, wxT("Open"));
         menu.Append(POPUP_CLOSE, wxT("Close"));
         break;
      case GmatTree::SCRIPT_FILE:
         break; // nothing to add
      default:
         menu.Append(POPUP_OPEN, wxT("Open"));
         menu.Append(POPUP_CLOSE, wxT("Close"));
         menu.AppendSeparator();
         menu.Append(POPUP_RENAME, wxT("Rename"));
         menu.Append(POPUP_DELETE, wxT("Delete"));
         menu.AppendSeparator();
         menu.Append(POPUP_CLONE, wxT("Clone"));
         break;
      }
   }
   
   PopupMenu(&menu, pt);
#endif // wxUSE_MENUS
}


//------------------------------------------------------------------------------
// wxMenu* CreatePopupMenu(GmatTree::ItemType itemType)
//------------------------------------------------------------------------------
/**
 * Create the popup menu
 *
 * @param <itemType> Type of object to create and add to tree
 * @return <wxMenu> Return popup menu
 */
//------------------------------------------------------------------------------
wxMenu* ResourceTree::CreatePopupMenu(GmatTree::ItemType itemType)
{
   wxMenu *menu = new wxMenu;
   
   switch (itemType)
   {
   case GmatTree::HARDWARE_FOLDER:
      menu->Append(POPUP_ADD_FUELTANK, wxT("Fuel Tank"));
      menu->Append(POPUP_ADD_THRUSTER, wxT("Thruster"));
      break;
   case GmatTree::BURN_FOLDER:
      menu->Append(POPUP_ADD_IMPULSIVE_BURN, wxT("ImpulsiveBurn"));
      menu->Append(POPUP_ADD_FINITE_BURN, wxT("FiniteBurn"));
      break;
   case GmatTree::BOUNDARY_SOLVER_FOLDER:
      menu->Append(POPUP_ADD_DIFF_CORR, wxT("DifferentialCorrector"));
      menu->Append(POPUP_ADD_BROYDEN, wxT("Broyden"));
      menu->Enable(POPUP_ADD_BROYDEN, false);
      break;
   case GmatTree::OPTIMIZER_FOLDER:
      menu->Append(POPUP_ADD_QUASI_NEWTON, wxT("Quasi-Newton"));
      menu->Append(POPUP_ADD_SQP, wxT("SQP (fmincon)"));
      menu->Enable(POPUP_ADD_QUASI_NEWTON, false);
      break;
   case GmatTree::SUBSCRIBER_FOLDER:
      menu->Append(POPUP_ADD_REPORT_FILE, wxT("ReportFile"));
      menu->Append(POPUP_ADD_XY_PLOT, wxT("XYPlot"));
      menu->Append(POPUP_ADD_OPENGL_PLOT, wxT("OpenGLPlot"));
      break;
   case GmatTree::FUNCTION_FOLDER:
      #ifdef __USE_MATLAB__
      menu->Append(POPUP_ADD_MATLAB_FUNCT, wxT("MATLAB Function"));
      #endif
      
      menu->Append(POPUP_ADD_GMAT_FUNCT, wxT("GMAT Function"));
      break;
   case GmatTree::SPECIAL_POINT_FOLDER:
      menu->Append(POPUP_ADD_BARYCENTER, wxT("Barycenter"));
      menu->Append(POPUP_ADD_LIBRATION, wxT("Libration Point"));
      break;
   default:
      break;
   }

   return menu;
}


//------------------------------------------------------------------------------
// Gmat::ObjectType GetObjectType(GmatTree::ItemType itemType)
//------------------------------------------------------------------------------
Gmat::ObjectType ResourceTree::GetObjectType(GmatTree::ItemType itemType)
{
   Gmat::ObjectType objType;
   
   switch (itemType)
   {
   case GmatTree::SPACECRAFT:
      objType = Gmat::SPACECRAFT;
      break;
   case GmatTree::FORMATION:
      objType = Gmat::FORMATION;
      break;
   case GmatTree::IMPULSIVE_BURN:
      objType = Gmat::BURN;
      break;
   case GmatTree::FINITE_BURN:
      objType = Gmat::BURN;
      break;
   case GmatTree::PROPAGATOR:
      objType = Gmat::PROP_SETUP;
      break;
   case GmatTree::DIFF_CORR:
   case GmatTree::SQP:
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
   case GmatTree::FUELTANK:
   case GmatTree::THRUSTER:
      objType = Gmat::HARDWARE;
      break;
   case GmatTree::BARYCENTER:
   case GmatTree::LIBRATION_POINT:
      objType = Gmat::CALCULATED_POINT;
      break;
   case GmatTree::COORD_SYSTEM:
   case GmatTree::USER_COORD_SYSTEM:
      objType = Gmat::COORDINATE_SYSTEM;
      break;
      
   default:
      objType = Gmat::UNKNOWN_OBJECT;
      MessageInterface::ShowMessage
         ("ResourceTree::GetObjectType() unknown object type.\n");
      break;
   }
   
   return objType;
   
}


//------------------------------------------------------------------------------
// wxTreeItemId GetTreeItemId(GmatTree::ItemType itemType)
//------------------------------------------------------------------------------
wxTreeItemId ResourceTree::GetTreeItemId(GmatTree::ItemType itemType)
{
   switch (itemType)
   {
   case GmatTree::SPACECRAFT:
      return mSpacecraftItem;
   case GmatTree::FORMATION:
      return mFormationItem;
   case GmatTree::IMPULSIVE_BURN:
   case GmatTree::FINITE_BURN:
      return mBurnItem;
   case GmatTree::PROPAGATOR:
      return mPropagatorItem;
   case GmatTree::DIFF_CORR:
      return mBoundarySolverItem;
   case GmatTree::SQP:
      return mOptimizerItem;
   case GmatTree::REPORT_FILE:
   case GmatTree::XY_PLOT:
   case GmatTree::OPENGL_PLOT:
      return mSubscriberItem;
   case GmatTree::VARIABLE:
      return mVariableItem;
   case GmatTree::MATLAB_FUNCTION:
   case GmatTree::GMAT_FUNCTION:
      return mFunctionItem;
   case GmatTree::FUELTANK:
   case GmatTree::THRUSTER:
      return mHardwareItem;
   case GmatTree::BARYCENTER:
   case GmatTree::LIBRATION_POINT:
      return mSpecialPointsItem;
   case GmatTree::COORD_SYSTEM:
   case GmatTree::USER_COORD_SYSTEM:
      return mCoordSysItem;
   default:
      MessageInterface::ShowMessage
         ("ResourceTree::GetObjectType() unknown object type.\n");
      break;
   }
   
   wxTreeItemId nullId;
   return nullId;
   
}


//------------------------------------------------------------------------------
// GmatTree::IconType GetTreeItemIcon(GmatTree::ItemType itemType)
//------------------------------------------------------------------------------
GmatTree::IconType ResourceTree::GetTreeItemIcon(GmatTree::ItemType itemType)
{
   switch (itemType)
   {
   case GmatTree::SPACECRAFT:
      return GmatTree::ICON_SPACECRAFT;
   case GmatTree::IMPULSIVE_BURN:
   case GmatTree::FINITE_BURN:
      return GmatTree::ICON_BURN;
   case GmatTree::PROPAGATOR:
      return GmatTree::ICON_PROPAGATOR;
   case GmatTree::REPORT_FILE:
      return GmatTree::ICON_REPORT;
   case GmatTree::XY_PLOT:
      return GmatTree::ICON_XY_PLOT;
   case GmatTree::OPENGL_PLOT:
      return GmatTree::ICON_OPEN_GL_PLOT;
   case GmatTree::VARIABLE:
      return GmatTree::ICON_VARIABLE;
   case GmatTree::MATLAB_FUNCTION:
      return GmatTree::ICON_MATLAB_FUNCTION;
   case GmatTree::GMAT_FUNCTION:
      return GmatTree::ICON_FUNCTION;
   case GmatTree::FUELTANK:
      return GmatTree::ICON_TANK;
   case GmatTree::THRUSTER:
      return GmatTree::ICON_THRUSTER;
   case GmatTree::COORD_SYSTEM:
   case GmatTree::USER_COORD_SYSTEM:
      return GmatTree::ICON_COORDINATE_SYSTEM;
   case GmatTree::BARYCENTER:
   case GmatTree::LIBRATION_POINT:
   case GmatTree::DIFF_CORR:
   case GmatTree::SQP:
      return GmatTree::ICON_DEFAULT;
   default:
      if (itemType >= GmatTree::RESOURCES_FOLDER &&
          itemType <= GmatTree::END_OF_RESOURCE_FOLDER)
         return GmatTree::ICON_FOLDER;
      else
         return GmatTree::ICON_DEFAULT;
   }
}


//------------------------------------------------------------------------------
// void CompareScriptRunResult(Real absTol, const wxString &replaceStr.
//                             const wxString &dir1, const wxString &dir2,
//                             wxTextCtrl *textCtrl)
//------------------------------------------------------------------------------
void ResourceTree::CompareScriptRunResult(Real absTol, const wxString &replaceStr,
                                          const wxString &dir1, const wxString &dir2,
                                          wxTextCtrl *textCtrl)
{
   #if DEBUG_COMPARE_REPORT
   MessageInterface::ShowMessage
      ("ResourceTree::CompareScriptRunResult() absTol=%g, replaceStr=%s\n"
       "   dir1=%s\n   dir2=%s\n   textCtrl=%d\n", absTol, replaceStr.c_str(),
       dir1.c_str(), dir2.c_str(), textCtrl);
   #endif
   
   if (textCtrl == NULL)
   {
      MessageInterface::ShowMessage
         ("ResourceTree::CompareScriptRunResult() textCtrl is NULL\n");
      return;
   }
   
   StringArray itemNames =
      theGuiInterpreter->GetListOfObjects(Gmat::SUBSCRIBER);
   int size = itemNames.size();
   std::string objName;
   //ReportFile *reportFile;
   GmatBase *reportFile;
   int reportCount = 0;
   
   for (int i = 0; i<size; i++)
   {
      GmatBase *sub = GetObject(itemNames[i]);
      objName = itemNames[i];
      
      if (sub->GetTypeName() == "ReportFile")
      {
         reportFile = GetObject(objName);
         
         if (!reportFile)
         {
            MessageInterface::ShowMessage
               ("ResourceTree::CompareScriptRunResult() The ReportFile: %s is NULL.\n",
                objName.c_str());
            return;
         }
         
         std::string filename1 = reportFile->GetStringParameter("Filename");
         StringArray colTitles = reportFile->GetRefObjectNameArray(Gmat::PARAMETER);
         //MessageInterface::ShowMessage("===> colTitles.size=%d\n", colTitles.size());
         
         #if DEBUG_COMPARE_REPORT
         MessageInterface::ShowMessage("   filename1=%s\n", filename1.c_str());
         #endif
         
         wxString filename2 = filename1.c_str();
         wxFileName fname(filename2);
         wxString name2 = fname.GetFullName();
         size_t numReplaced = name2.Replace("GMAT", replaceStr.c_str());
         
         if (numReplaced == 0)
         {
            textCtrl->AppendText
               ("***Cannot compare results. The report file doesn't contain GMAT.\n");
            MessageInterface::ShowMessage
               ("ResourceTree::CompareScriptRunResult() Cannot compare results.\n"
                "The report file doesn't contain GMAT.\n\n");
            return;
         }
         
         if (numReplaced > 1)
         {
            textCtrl->AppendText
               ("***Cannot compare results. The report file name contains more "
                "than 1 GMAT string.\n");
            MessageInterface::ShowMessage
               ("ResourceTree::CompareScriptRunResult() Cannot compare results.\n"
                "The report file name contains more than 1 GMAT string.\n\n");
            return;
         }
         
         // set filename2
         filename2 = dir2 + "/" + name2;

         #if DEBUG_COMPARE_REPORT
         MessageInterface::ShowMessage("   filename2=%s\n", filename2.c_str());
         #endif
         
         StringArray output =
            GmatFileUtil::Compare(filename1.c_str(), filename2.c_str(), colTitles, absTol);
         
         // append text
         for (unsigned int i=0; i<output.size(); i++)
            textCtrl->AppendText(wxString(output[i].c_str()));
         
         textCtrl->Show();
         
         reportCount++;
      }
   }
   
   if (reportCount == 0)
   {
      textCtrl->AppendText("** There is no report file to compare.\n\n");
      MessageInterface::ShowMessage("** There is no report file to compare.\n");
   }
   else
   {
      textCtrl->AppendText
         ("========================================================\n\n");
   }
}

