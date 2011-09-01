//$Id$
//------------------------------------------------------------------------------
//                              ResourceTree
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Allison Greene
// Created: 2003/08/29
//    2010.03.23 Thomas Grubb 
//      - Removed Clone menu item from the Formation popup menu
//    2010.03.16 Thomas Grubb 
//      - Moved the GroundStation folder in the mission tree to appear below 
//        Formation folder
//
/**
 * This class provides the resource tree and event handlers.
 */
//------------------------------------------------------------------------------
#include "bitmaps/ClosedFolder.xpm"
#include "bitmaps/OpenFolder.xpm"
#include "bitmaps/file.xpm"
#include "bitmaps/Groundstation.xpm"
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
#include "bitmaps/moon.xpm"
#include "bitmaps/moon_generic.xpm"
#include "bitmaps/planet_generic.xpm"
#include "bitmaps/asteroid.xpm"
#include "bitmaps/comet.xpm"
#include "bitmaps/function.xpm"
#include "bitmaps/matlabfunction.xpm"
#include "bitmaps/coordinatesystem.xpm"
#include "bitmaps/openglplot.xpm"
#include "bitmaps/propagator.xpm"
#include "bitmaps/variable.xpm"
#include "bitmaps/array.xpm"
#include "bitmaps/string.xpm"
#include "bitmaps/xyplot.xpm"
#include "bitmaps/tank.xpm"
#include "bitmaps/thruster.xpm"
#include "bitmaps/Script.xpm"
#include "bitmaps/rt_ImpulsiveBurn.xpm"
#include "bitmaps/rt_FiniteBurn.xpm"
#include "bitmaps/rt_SolarSystem.xpm"
#include "bitmaps/rt_Barycenter.xpm"
#include "bitmaps/rt_LibrationPoint.xpm"
#include "bitmaps/rt_BoundaryValueSolver.xpm"
#include "bitmaps/rt_Optimizer.xpm"
#include "bitmaps/rt_Simulator.xpm"
#include "bitmaps/rt_Estimator.xpm"
#include "bitmaps/rt_Antenna.xpm"
#include "bitmaps/rt_Transmitter.xpm"
#include "bitmaps/rt_Receiver.xpm"
#include "bitmaps/rt_Transponder.xpm"
#include "bitmaps/rt_MeasurementModel.xpm"
#include "bitmaps/rt_Matlab.xpm"
#include "bitmaps/rt_MatlabServer.xpm"
#include "bitmaps/rt_Default.xpm"
#include "bitmaps/rt_GroundTrackPlot.xpm"

#include "GuiInterpreter.hpp"
#include "ResourceTree.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "MissionTree.hpp"
#include "MessageInterface.hpp"
#include "GmatTreeItemData.hpp"
#include "ParameterCreateDialog.hpp"
#include "CoordSysCreateDialog.hpp"
#include "RunScriptFolderDialog.hpp"
#include "ViewTextDialog.hpp"
#include "GmatFunction.hpp"           // for SetNewFunction()
#include "FileManager.hpp"            // for GetFullPathname()
#include "FileUtil.hpp"               // for Compare()
#include "GmatGlobal.hpp"             // for SetBatchMode()
#include "StringUtil.hpp"             // for GmatStringUtil::
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "UtilityException.hpp"
#include <sstream>
#include <fstream>
#include <ctime>                      // for clock()
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/config.h>

// If we are ready to handle Constellations, enable this
//define __ENABLE_CONSTELLATIONS__

#define DEBUG_RESOURCE_TREE
//#define DEBUG_ADD_DEFAULT_OBJECTS
//#define DEBUG_RENAME
//#define DEBUG_DELETE
//#define DEBUG_COMPARE_REPORT
//#define DEBUG_RUN_SCRIPT_FOLDER
//#define DEBUG_RESOURCE_TREE_UPDATE
//#define DEBUG_RESOURCE_ICON
//#define DEBUG_ADD_ICONS
//#define DEBUG_ADD_COMET
//#define DEBUG_ACTIVE_SCRIPT
//#define DEBUG_FIND_SCRIPT
//#define DEBUG_ADD_SCRIPT
//#define DEBUG_ADD_SOLVER
//#define DEBUG_USER_GUI

// ID macros for plug-ins
#define SOLVER_BEGIN 150
#define SOLVER_END 200
#define HARDWARE_BEGIN 250
#define HARDWARE_END 300
#define PLUGIN_BEGIN 400
#define PLUGIN_END 600

#define SUBSCRIBER_BEGIN SOLVER_END + 1
#define SUBSCRIBER_END SUBSCRIBER_BEGIN + 50

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const Integer ResourceTree::MAX_SUN_ORBITERS = 120;


//------------------------------------------------------------------------------
// event tables and other macros for wxWidgets
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ResourceTree, wxTreeCtrl)
   EVT_TREE_ITEM_RIGHT_CLICK(-1, ResourceTree::OnItemRightClick)

   EVT_TREE_ITEM_ACTIVATED(-1, ResourceTree::OnItemActivated)
   EVT_TREE_BEGIN_LABEL_EDIT(-1, ResourceTree::OnBeginLabelEdit)
   EVT_TREE_END_LABEL_EDIT(-1, ResourceTree::OnEndLabelEdit)
   //EVT_TREE_BEGIN_DRAG(-1, ResourceTree::OnBeginDrag)
   //EVT_TREE_BEGIN_RDRAG(-1, ResourceTree::OnBeginRDrag)
   //EVT_TREE_END_DRAG(-1, ResourceTree::OnEndDrag)

   EVT_MENU(POPUP_ADD_GROUND_STATION, ResourceTree::OnAddGroundStation)
   EVT_MENU(POPUP_ADD_SPACECRAFT, ResourceTree::OnAddSpacecraft)
   EVT_MENU(POPUP_ADD_FUELTANK, ResourceTree::OnAddFuelTank)
   EVT_MENU(POPUP_ADD_THRUSTER, ResourceTree::OnAddThruster)
   EVT_MENU(POPUP_ADD_FORMATION, ResourceTree::OnAddFormation)
   EVT_MENU(POPUP_ADD_CONSTELLATION, ResourceTree::OnAddConstellation)
   EVT_MENU(POPUP_ADD_IMPULSIVE_BURN, ResourceTree::OnAddImpulsiveBurn)
   EVT_MENU(POPUP_ADD_FINITE_BURN, ResourceTree::OnAddFiniteBurn)
   EVT_MENU(POPUP_ADD_PROPAGATOR, ResourceTree::OnAddPropagator)
//   EVT_MENU(POPUP_ADD_SPK_PROPAGATOR, ResourceTree::OnAddSPKPropagator)
   EVT_MENU(POPUP_ADD_BODY, ResourceTree::OnAddBody)
   EVT_MENU(POPUP_ADD_DIFF_CORR, ResourceTree::OnAddDiffCorr)
   EVT_MENU(POPUP_ADD_SQP, ResourceTree::OnAddSqp)
   EVT_MENU_RANGE(POPUP_ADD_HARDWARE + HARDWARE_BEGIN,    // Other Hardware
                  POPUP_ADD_HARDWARE + HARDWARE_END, ResourceTree::OnAddHardware)
   EVT_MENU_RANGE(POPUP_ADD_SOLVER + SOLVER_BEGIN,        // Other Solvers
                  POPUP_ADD_SOLVER + SOLVER_END, ResourceTree::OnAddSolver)
   EVT_MENU(POPUP_ADD_REPORT_FILE, ResourceTree::OnAddReportFile)
   EVT_MENU(POPUP_ADD_XY_PLOT, ResourceTree::OnAddXyPlot)
   EVT_MENU(POPUP_ADD_ORBIT_VIEW, ResourceTree::OnAddOrbitView)
   EVT_MENU(POPUP_ADD_EPHEMERIS_FILE, ResourceTree::OnAddEphemerisFile)
   EVT_MENU_RANGE(POPUP_ADD_SUBSCRIBER + SUBSCRIBER_BEGIN, // Other Subscribers
                  POPUP_ADD_SUBSCRIBER + SUBSCRIBER_END, ResourceTree::OnAddSubscriber)
   EVT_MENU(POPUP_ADD_VARIABLE, ResourceTree::OnAddVariable)
   EVT_MENU(POPUP_ADD_ARRAY, ResourceTree::OnAddArray)
   EVT_MENU(POPUP_ADD_STRING, ResourceTree::OnAddString)
   EVT_MENU(POPUP_ADD_MATLAB_FUNCTION, ResourceTree::OnAddMatlabFunction)
   EVT_MENU(POPUP_ADD_GMAT_FUNCTION, ResourceTree::OnAddGmatFunction)
   EVT_MENU(POPUP_NEW_GMAT_FUNCTION, ResourceTree::OnAddGmatFunction)
   EVT_MENU(POPUP_OPEN_GMAT_FUNCTION, ResourceTree::OnAddGmatFunction)
   EVT_MENU(POPUP_ADD_COORD_SYS, ResourceTree::OnAddCoordSys)
   EVT_MENU(POPUP_ADD_BARYCENTER, ResourceTree::OnAddBarycenter)
   EVT_MENU(POPUP_ADD_LIBRATION, ResourceTree::OnAddLibration)
   EVT_MENU(POPUP_ADD_PLANET, ResourceTree::OnAddPlanet)
   EVT_MENU(POPUP_ADD_MOON, ResourceTree::OnAddMoon)
   EVT_MENU(POPUP_ADD_COMET, ResourceTree::OnAddComet)
   EVT_MENU(POPUP_ADD_ASTEROID, ResourceTree::OnAddAsteroid)
   EVT_MENU_RANGE(POPUP_ADD_PLUGIN + PLUGIN_BEGIN,        // Plugin objects
                  POPUP_ADD_PLUGIN + PLUGIN_END, ResourceTree::OnAddUserObject)
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
   theMainFrame = NULL;
   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   mScriptFolderRunning = false;
   mHasUserInterrupted = false;
   mScriptAdded = false;
   mMatlabServerStarted = false;
   
   AddIcons();
   AddDefaultResources();
   AddUserResources(theGuiInterpreter->GetUserResources());

   theGuiManager->UpdateAll();
}


//------------------------------------------------------------------------------
// void SetMainFrame(GmatMainFrame *gmf)
//------------------------------------------------------------------------------
void ResourceTree::SetMainFrame(GmatMainFrame *gmf)
{
   theMainFrame = gmf;
}


//------------------------------------------------------------------------------
// void ClearResource(bool leaveScripts)
//------------------------------------------------------------------------------
void ResourceTree::ClearResource(bool leaveScripts)
{
   // ag: collapse, so folder icon is closed
   // djc: Under Linux, this crashes so it only applies to Windows
   #ifdef __WXMSW__
      Collapse(mGroundStationItem);
      Collapse(mSpacecraftItem);
      Collapse(mHardwareItem);
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

   // Delete children for all but interface and, optionally, script
   wxTreeItemId root = GetRootItem();
   wxTreeItemIdValue cookie;
   wxTreeItemId cf = GetFirstChild(root, cookie);
   for(UnsignedInt i = 0; i < GetChildrenCount(root, false); ++i)
   {
      if (cf != interfaceItem)
      {
         if (cf != mScriptItem)
         {
            #ifdef DEBUG_USER_GUI
               MessageInterface::ShowMessage("Clearing %s\n",
                     GetItemText(cf).c_str());
            #endif
            DeleteChildren(cf);
         }
         if (!leaveScripts && cf == mScriptItem)
         {
            #ifdef DEBUG_USER_GUI
               MessageInterface::ShowMessage("Clearing %s\n",
                     GetItemText(cf).c_str());
            #endif
            DeleteChildren(cf);
         }
      }
      cf = GetNextChild(root, cookie);
   }
   
   //----- SpecialPoint is child of Universe
   AddItemFolder(mUniverseItem, mSpecialPointsItem, "Special Points",
                 GmatTree::SPECIAL_POINT_FOLDER);
   
   //--------------- Boundary Value Solvers is a child of solvers
   AddItemFolder(mSolverItem, mBoundarySolverItem, "Boundary Value Solvers",
                 GmatTree::BOUNDARY_SOLVER_FOLDER);
   
   //--------------- Optimizers is a child of solvers
   AddItemFolder(mSolverItem, mOptimizerItem, "Optimizers",
                 GmatTree::OPTIMIZER_FOLDER);
   
   AddUserResources(theGuiInterpreter->GetUserResources(), true);
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
   #ifdef DEBUG_RESOURCE_TREE_UPDATE
   MessageInterface::ShowMessage("ResourceTree::UpdateResource() entered\n");
   #endif

   if (restartCounter)
      theGuiInterpreter->ResetConfigurationChanged();

   ClearResource(true);

   AddDefaultBodies(mUniverseItem);
   AddDefaultSpecialPoints(mSpecialPointsItem);
   AddDefaultGroundStation(mGroundStationItem, restartCounter);
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

   AddUserObjects();

   theGuiManager->UpdateAll();
   ScrollTo(mSpacecraftItem);

   #ifdef DEBUG_RESOURCE_TREE_UPDATE
   MessageInterface::ShowMessage("ResourceTree::UpdateResource() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void SetActiveScript(const wxString &scriptWithPath)
//------------------------------------------------------------------------------
void ResourceTree::SetActiveScript(const wxString &scriptWithPath)
{
   #ifdef DEBUG_ACTIVE_SCRIPT
   MessageInterface::ShowMessage
      ("ResourceTree::SetActiveScript() entered, mLastActiveScript='%s', "
       "scriptWithPath='%s'\n", mLastActiveScript.c_str(), scriptWithPath.c_str());
   #endif
   
   // Make last active script normal face first
   MakeScriptActive(mLastActiveScript, false);
   
   // Make new active script bold face
   wxString newScript = (GmatFileUtil::ParseFileName(scriptWithPath.c_str())).c_str();
   
   #ifdef DEBUG_ACTIVE_SCRIPT
   MessageInterface::ShowMessage
      ("   scriptWithPath='%s'\n   newScript='%s'\n", scriptWithPath.c_str(), newScript.c_str());
   #endif
   
   MakeScriptActive(newScript, true);
   
   mLastActiveScript = newScript;
   
   #ifdef DEBUG_ACTIVE_SCRIPT
   MessageInterface::ShowMessage("ResourceTree::SetActiveScript() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void AddScript(wxString path)
//------------------------------------------------------------------------------
/*
 * Add script file path and name to resource tree and sets main frame title
 *
 * @param  path  script full path name
 *
 */
//------------------------------------------------------------------------------
void ResourceTree::AddScript(wxString path)
{
   mScriptAdded = AddScriptItem(path);
   if (mScriptAdded)
   {
      // need to set the filename to MainFrame
      // We don't want to set the filename here (LOJ: 2010.12.30)
      // GmatMainFrame will call GetLastScriptAdded() instead
      //theMainFrame->SetScriptFileName(path.c_str());
      mLastScriptAdded = path;
   }
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
   #ifdef DEBUG_ADD_SCRIPT
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
   //wxFileName fn(path);
   //wxString filename = fn.GetName();
   // Use FileUtil for consistency (LOJ: 2010.01.04)
   wxString filename = (GmatFileUtil::ParseFileName(path.c_str())).c_str();
   
   #ifdef DEBUG_ADD_SCRIPT
   MessageInterface::ShowMessage("   filename=%s\n", filename.c_str());
   #endif

   // find child with same path
   while (childId.IsOk())
   {
      childText = GetItemText(childId);

      #ifdef DEBUG_ADD_SCRIPT
      MessageInterface::ShowMessage
         ("ResourceTree::OnAddScriptItem() childText=<%s>\n", childText.c_str());
      #endif
      
      scriptPath = ((GmatTreeItemData *)GetItemData(childId))->GetName();
      
      if (GmatFileUtil::IsSameFileName(childText.c_str(), filename.c_str()))
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
            AppendItem(mScriptItem, path, GmatTree::RESOURCE_ICON_DEFAULT, -1,
                       new GmatTreeItemData(path, GmatTree::SCRIPT_FILE, path));
      }
      else
      {
         // add filename to tree
         scriptId =
            AppendItem(mScriptItem, filename, GmatTree::RESOURCE_ICON_DEFAULT, -1,
                       new GmatTreeItemData(path, GmatTree::SCRIPT_FILE, path));
      }

      Expand(mScriptItem);
      SelectItem(scriptId);
      UpdateRecentFiles(path);
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
void ResourceTree::UpdateRecentFiles(wxString filename)
{
   wxArrayString files;
   wxString aFilename;
   wxString aKey;
   size_t i = 0;

   // get the config object
   wxConfigBase *pConfig = GmatAppData::Instance()->GetPersonalizationConfig();
   pConfig->SetPath(wxT("/RecentFiles"));

   // read filenames from config object
   while (pConfig->Read(wxString::Format(wxT("%d"), (int)i), &aFilename)) 
   {
      files.Add(aFilename);
      i++;
   }

   // if filename already in list, delete it (we will add on end)
   if (files.Index(filename.c_str()) != wxNOT_FOUND) 
   {
      files.Remove(filename.c_str());
   }

   // add latest filename to end of list
   files.Add(filename);

   // remove files from beginning of list
   while (files.GetCount() > 5)
      files.RemoveAt(0);

   // save filenames back to config
   pConfig->SetPath(wxT("/"));
   pConfig->DeleteGroup("/RecentFiles");
   pConfig->SetPath(wxT("/RecentFiles"));
   for (i = 0; i < files.GetCount(); i++)
   {
      aFilename = files[i];
      aKey = wxString::Format(wxT("%d"), (int) i);

      pConfig->Write(aKey, aFilename.c_str());
      //pConfig->Write((GmatFileUtil::ParseFileName(aFilename.c_str())).c_str(), aFilename.c_str());
   }

   theMainFrame->UpdateRecentMenu(files);

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
// void UpdateGuiItem(GmatTree::ItemType itemType)
//------------------------------------------------------------------------------
void ResourceTree::UpdateGuiItem(GmatTree::ItemType itemType)
{
   #ifdef DEBUG_UPDATE
   MessageInterface::ShowMessage
      ("ResourceTree::UpdateGuiItem() entered, itemType=%d\n", itemType);
   #endif
   
   switch (itemType)
   {
   case GmatTree::COORD_SYSTEM:
   case GmatTree::USER_COORD_SYSTEM:
      theGuiManager->UpdateCoordSystem();
      break;
   case GmatTree::SPACECRAFT:
      theGuiManager->UpdateSpacecraft();
      break;
   case GmatTree::FUELTANK:
      theGuiManager->UpdateFuelTank();
      break;
   case GmatTree::THRUSTER:
      theGuiManager->UpdateThruster();
      break;
   case GmatTree::HARDWARE:
      theGuiManager->UpdateSensor();
      break;
   case GmatTree::IMPULSIVE_BURN:
   case GmatTree::FINITE_BURN:
      theGuiManager->UpdateBurn();
      break;
   case GmatTree::REPORT_FILE:
   case GmatTree::XY_PLOT:
   case GmatTree::ORBIT_VIEW:
   case GmatTree::GROUND_TRACK_PLOT:
   case GmatTree::EPHEMERIS_FILE:
      theGuiManager->UpdateSubscriber();
      break;
   case GmatTree::DIFF_CORR:
   case GmatTree::SQP:
   case GmatTree::SOLVER:
      theGuiManager->UpdateSolver();
      break;
   case GmatTree::BARYCENTER:
   case GmatTree::LIBRATION_POINT:
      theGuiManager->UpdateCelestialPoint();
      break;
   case GmatTree::VARIABLE:
   case GmatTree::ARRAY:
   case GmatTree::STRING:
      theGuiManager->UpdateParameter();
      break;
   case GmatTree::MATLAB_FUNCTION:
   case GmatTree::GMAT_FUNCTION:
      theGuiManager->UpdateFunction();
      break;
   default:
      break;
   }

   #ifdef DEBUG_UPDATE
   MessageInterface::ShowMessage
      ("ResourceTree::UpdateGuiItem() leaving, itemType=%d\n", itemType);
   #endif
}


//------------------------------------------------------------------------------
// void AddItemFolder(wxTreeItemId itemId, const wxString &itemName)
//------------------------------------------------------------------------------
/*
 * Adds the item folder to the tree
 *
 * @param parentItemId  parent tree item id
 * @param itemId        tree item id
 * @param itemName      name of the item to be added
 * @param itemType      type of the item to be added
 */
//------------------------------------------------------------------------------
void ResourceTree::AddItemFolder(wxTreeItemId parentItemId, wxTreeItemId &itemId,
                                 const wxString &itemName, GmatTree::ItemType itemType)
{
   itemId =
      AppendItem(parentItemId, wxT(itemName), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT(itemName), itemType));
   
   SetItemImage(itemId, GmatTree::ICON_OPENFOLDER, wxTreeItemIcon_Expanded);
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
   #ifdef DEBUG_RESOURCE_TREE
   MessageInterface::ShowMessage("ResourceTree::AddNode() entered\n");
   #endif

   wxTreeItemId itemId = GetTreeItemId(itemType);
   GmatTree::ResourceIconType iconType = GetTreeItemIcon(itemType);

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

   //----- GroundStation
   mGroundStationItem =
      AppendItem(resource, wxT("Ground Station"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Ground Station"),
                                      GmatTree::GROUND_STATION_FOLDER));

   SetItemImage(mGroundStationItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);
   
   //----- Hardware
   mHardwareItem = 
      AppendItem(resource, wxT("Hardware"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Hardware"),
                                      GmatTree::HARDWARE_FOLDER));

   SetItemImage(mHardwareItem, GmatTree::ICON_OPENFOLDER,
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
      AppendItem(resource, wxT("SolarSystem"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("SolarSystem"),
                                      GmatTree::SOLAR_SYSTEM));

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
      AppendItem(resource, wxT("Output"), GmatTree::ICON_FOLDER,
                 -1, new GmatTreeItemData(wxT("Output"),
                                          GmatTree::SUBSCRIBER_FOLDER));

   SetItemImage(mSubscriberItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);

   //----- Interfaces
   interfaceItem =
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

   //----- Variables
   mVariableItem =
      AppendItem(resource, wxT("Variables/Arrays/Strings"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Variables/Arrays/Strings"),
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
   AddDefaultGroundStation(mGroundStationItem);
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

   AddUserObjects();

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
   SolarSystem   *ss         = theGuiInterpreter->GetSolarSystemInUse();
   StringArray   bodies      = ss->GetBodiesInUse();
   CelestialBody *body       = NULL;
   std::string   centralBody = "";

   std::string  star;
   StringArray  sunOrbiters;

   unsigned int      numBodies = bodies.size();

   GmatTree::ResourceIconType starIcon = GmatTree::ICON_SUN;

   // find the star (assume there's one for now)
   for (unsigned int ii = 0; ii < numBodies; ii++)
   {
      body = ss->GetBody(bodies.at(ii));
      if (body->GetBodyType() == Gmat::STAR)  star = bodies.at(ii);
      break;   // assuming one star, for now at least
   }

   for (unsigned int ii = 0; ii < numBodies; ii++)
   {
      body = ss->GetBody(bodies.at(ii));
      centralBody = body->GetStringParameter(body->GetParameterID("CentralBody"));
      if (centralBody == star)
      {
         sunOrbiters.push_back(bodies.at(ii));
      }
   }
   unsigned int numSunOrbiters = sunOrbiters.size();
//   StringArray otherOrbiters[numSunOrbiters];  // 2009.02.20 MSVC won't allow this ... tsk tsk
   StringArray otherOrbiters[MAX_SUN_ORBITERS];

   for (unsigned int ii = 0; ii < numBodies; ii++)
   {
      // skip the sun for now, as it has the Earth set as its central body, for reference
      if (bodies.at(ii) == star) continue;
      body = ss->GetBody(bodies.at(ii));
      centralBody = body->GetStringParameter(body->GetParameterID("CentralBody"));
      for (unsigned int jj = 0; jj < numSunOrbiters; jj++)
      {
         if (centralBody == sunOrbiters.at(jj))
         {
            otherOrbiters[jj].push_back(bodies.at(ii));
            break;
         }
      }
   }

   AppendItem(itemId, wxT(star.c_str()), starIcon, -1,
         new GmatTreeItemData(wxT(star.c_str()), GmatTree::CELESTIAL_BODY_STAR));

   wxTreeItemId theSunOrbiter;

   for (unsigned int kk = 0; kk < numSunOrbiters; kk++)
   {
      GmatTree::ItemType         iType    = GmatTree::CELESTIAL_BODY;
      GmatTree::ResourceIconType iconType = GmatTree::ICON_MOON;
      std::string bodyName = sunOrbiters.at(kk);
      GetBodyTypeAndIcon(bodyName, iType, iconType);
      theSunOrbiter = AppendItem(itemId, wxT(bodyName.c_str()), iconType, -1,
                   new GmatTreeItemData(wxT(bodyName.c_str()), iType));
      for (unsigned int mm = 0; mm < (otherOrbiters[kk]).size(); mm++)
      {
         bodyName = otherOrbiters[kk].at(mm);
         GetBodyTypeAndIcon(bodyName, iType, iconType);
         AppendItem(theSunOrbiter, wxT(bodyName.c_str()), iconType, -1,
                    new GmatTreeItemData(wxT(bodyName.c_str()), iType));
      }
   }
}


//------------------------------------------------------------------------------
// GmatTree::ResourceIconType GetIconId(const wxString &resourceTypeName)
//------------------------------------------------------------------------------
/**
 * Returns icon id to use when showing in the resource tree
 * Notes: Until we figure out how to use user defined icons for plugins,
 *        provide all plugin object icons here.
 */
//------------------------------------------------------------------------------
GmatTree::ResourceIconType ResourceTree::GetIconId(GmatBase *obj)
{
   #ifdef DEBUG_RESOURCE_ICON
   MessageInterface::ShowMessage
      ("ResourceTree::GetIconId() entered, obj=<%p><%s>'%s'\n", obj,
       obj->GetTypeName().c_str(), obj->GetName().c_str());
   #endif
   
   GmatTree::ResourceIconType iconToUse = GmatTree::RESOURCE_ICON_DEFAULT;
   
   // Hardware
   if (obj->IsOfType("Antenna"))
      iconToUse = GmatTree::RESOURCE_ICON_ANTENNA;
   else if (obj->IsOfType("Transmitter"))
      iconToUse = GmatTree::RESOURCE_ICON_TRANSMITTER;
   else if (obj->IsOfType("Receiver"))
      iconToUse = GmatTree::RESOURCE_ICON_RECEIVER;
   else if (obj->IsOfType("Transponder"))
      iconToUse = GmatTree::RESOURCE_ICON_TRANSPONDER;
   
   // Solver
   else if (obj->IsOfType("DifferentialCorrector"))
      iconToUse = GmatTree::RESOURCE_ICON_BOUNDARY_VALUE_SOLVER;
   else if (obj->IsOfType("Optimizer"))
      iconToUse = GmatTree::RESOURCE_ICON_OPTIMIZER;
   else if (obj->IsOfType("Simulator"))
      iconToUse = GmatTree::RESOURCE_ICON_SIMULATOR;
   else if (obj->IsOfType("Estimator"))
      iconToUse = GmatTree::RESOURCE_ICON_ESTIMATOR;
   
   // MeasurementModel
   else if (obj->IsOfType("MeasurementModel"))
      iconToUse = GmatTree::RESOURCE_ICON_MEASUREMENT_MODEL;
   else if (obj->IsOfType(Gmat::MEASUREMENT_MODEL))
      iconToUse = GmatTree::RESOURCE_ICON_MEASUREMENT_MODEL;
   
   #ifdef DEBUG_RESOURCE_ICON
   MessageInterface::ShowMessage
      ("ResourceTree::GetIconId() returning, icon id to use %d\n", iconToUse);
   #endif
   
   return iconToUse;
}


//------------------------------------------------------------------------------
// void AddDefaultGroundStation(wxTreeItemId itemId, bool restartCounter = true)
//------------------------------------------------------------------------------
/**
 * Add the default ground station
 *
 * @param <itemId> tree item for the ground station folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultGroundStation(wxTreeItemId itemId, bool restartCounter)
{
   StringArray itemNames = theGuiInterpreter->GetListOfObjects(Gmat::GROUND_STATION);
   int size = itemNames.size();
   wxString objName;

   #ifdef DEBUG_ADD_DEFAULT_OBJECTS
   MessageInterface::ShowMessage
      ("ResourceTree::AddDefaultGroundStation() size=%d\n", size);
   #endif

   for (int i = 0; i<size; i++)
   {
      objName = wxString(itemNames[i].c_str());

      #ifdef DEBUG_ADD_DEFAULT_OBJECTS
      MessageInterface::ShowMessage
         ("ResourceTree::AddDefaultGroundStation() objName=%s\n", objName.c_str());
      #endif

      AppendItem(itemId, wxT(objName), GmatTree::ICON_GROUND_STATION, -1,
                 new GmatTreeItemData(wxT(objName), GmatTree::GROUND_STATION));
   };

   if (size > 0)
      Expand(itemId);
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

   #ifdef DEBUG_ADD_DEFAULT_OBJECTS
   MessageInterface::ShowMessage
      ("ResourceTree::AddDefaultSpacecraft() size=%d\n", size);
   #endif

   for (int i = 0; i<size; i++)
   {
      objName = wxString(itemNames[i].c_str());

      #ifdef DEBUG_ADD_DEFAULT_OBJECTS
      MessageInterface::ShowMessage
         ("ResourceTree::AddDefaultSpacecraft() objName=%s\n", objName.c_str());
      #endif

      AppendItem(itemId, wxT(objName), GmatTree::ICON_SPACECRAFT, -1,
                 new GmatTreeItemData(wxT(objName), GmatTree::SPACECRAFT));
   };

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

   for (int i = 0; i<size; i++)
   {
      GmatBase *hw = GetObject(itemNames[i]);
      objName = wxString(itemNames[i].c_str());
      objTypeName = wxString(hw->GetTypeName().c_str());

      if (objTypeName == "FuelTank")
      {
         AppendItem(itemId, wxT(objName), GmatTree::ICON_TANK, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::FUELTANK));
      }
      else if (objTypeName == "Thruster")
      {
         AppendItem(itemId, wxT(objName), GmatTree::ICON_THRUSTER, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::THRUSTER));
      }
   };

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
   StringArray itemNames;
   int size;
   wxString objName;

   itemNames = theGuiInterpreter->GetListOfObjects(Gmat::PROP_SETUP);
   size = itemNames.size();
   for (int i = 0; i<size; i++)
   {
      objName = wxString(itemNames[i].c_str());
      AppendItem(itemId, wxT(objName), GmatTree::ICON_PROPAGATOR, -1,
                 new GmatTreeItemData(wxT(objName), GmatTree::PROPAGATOR));
   };

//   int spkSize = 0;
//   #ifdef __USE_SPICE__
//   itemNames = theGuiInterpreter->GetListOfObjects("SPK");
//   spkSize = itemNames.size();
//   for (int i = 0; i<spkSize; i++)
//   {
//      objName = wxString(itemNames[i].c_str());
//      AppendItem(itemId, wxT(objName), GmatTree::ICON_PROPAGATOR, -1,
//                 new GmatTreeItemData(wxT(objName), GmatTree::SPK_PROPAGATOR));
//   };
//   #endif

   if (size > 0) // || spkSize > 0)
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
   #ifdef DEBUG_DEFAULT_BURNS
   MessageInterface::ShowMessage("ResourceTree::AddDefaultBurns() entered\n");
   #endif

   StringArray itemNames = theGuiInterpreter->GetListOfObjects(Gmat::BURN);
   int size = itemNames.size();
   wxString objName;
   wxString objTypeName;

   for (int i = 0; i<size; i++)
   {
      GmatBase *burn = GetObject(itemNames[i]);
      objName = wxString(itemNames[i].c_str());
      objTypeName = wxString(burn->GetTypeName().c_str());

      if (objTypeName == "ImpulsiveBurn")
      {
         #ifdef DEBUG_DEFAULT_BURNS
         MessageInterface::ShowMessage
            ("ResourceTree::AddDefaultBurns() objTypeName = ImpulsiveBurn\n");
         #endif
         AppendItem(itemId, wxT(objName), GmatTree::ICON_IMPULSIVE_BURN, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::IMPULSIVE_BURN));
      }
      else if (objTypeName == "FiniteBurn")
      {
         #ifdef DEBUG_DEFAULT_BURNS
         MessageInterface::ShowMessage
            ("ResourceTree::AddDefaultBurns() objTypeName = ImpulsiveBurn\n");
         #endif
         AppendItem(itemId, wxT(objName), GmatTree::ICON_FINITE_BURN, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::FINITE_BURN));
      }
   };

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
   #ifdef DEBUG_ADD_SOLVER
   MessageInterface::ShowMessage
      ("ResourceTree::AddDefaultSolvers() entered, restartCounter=%d\n", restartCounter);
   #endif
   
   StringArray itemNames = theGuiInterpreter->GetListOfObjects(Gmat::SOLVER);
   int size = itemNames.size();
   wxString objName;
   wxString objTypeName;
   
   #ifdef DEBUG_ADD_SOLVER
   MessageInterface::ShowMessage("   There are %d configured Solvers\n", size);
   #endif
   
   for (int i = 0; i<size; i++)
   {
      GmatBase *solver = GetObject(itemNames[i]);
      objName = wxString(itemNames[i].c_str());
      objTypeName = wxString(solver->GetTypeName().c_str());
      
      #ifdef DEBUG_ADD_SOLVER
      MessageInterface::ShowMessage
         ("   solver[%d]=<%p><%s>'%s'\n", i, solver, solver->GetTypeName().c_str(),
          solver->GetName().c_str());
      #endif
      
      GmatTree::ResourceIconType iconToUse = GetIconId(solver);
      
      /// @todo:  need to create different types for the solvers and check strings
      if (objTypeName == "DifferentialCorrector")
      {
         AppendItem(mBoundarySolverItem, wxT(objName), iconToUse, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::DIFF_CORR));
      }
      else if (objTypeName == "Broyden")
      {
         //AppendItem(mBoundarySolverItem, wxT(objName), iconToUse, -1,
         //           new GmatTreeItemData(wxT(objName), GmatTree::BROYDEN));
      }
      else if (objTypeName == "Quasi-Newton")
      {
         //AppendItem(mOptimizerItem, wxT(objName), iconToUse, -1,
         //           new GmatTreeItemData(wxT(objName), GmatTree::QUASI_NEWTON));
      }
      else if (objTypeName == "FminconOptimizer")
      {
         AppendItem(mOptimizerItem, wxT(objName), iconToUse, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::SQP));
      }
      else if (solver->IsOfType("Optimizer"))
      {
         // Set generic optimizer stuff here!
         AppendItem(mOptimizerItem, wxT(objName), iconToUse, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::SOLVER));
      }
   };

   if (size > 0)
   {
      Expand(mBoundarySolverItem);
      Expand(mOptimizerItem);
      Expand(itemId);
   }
   
   #ifdef DEBUG_ADD_SOLVER
   MessageInterface::ShowMessage("ResourceTree::AddDefaultSolvers() leaving\n");
   #endif
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

   for (int i = 0; i<size; i++)
   {
      GmatBase *sub = GetObject(itemNames[i]);
      objName = wxString(itemNames[i].c_str());
      objTypeName = wxString(sub->GetTypeName().c_str());

      if (objTypeName == "ReportFile")
      {
         AppendItem(itemId, wxT(objName), GmatTree::ICON_REPORT_FILE, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::REPORT_FILE));
      }
      else if (objTypeName == "XYPlot")
      {
         AppendItem(itemId, wxT(objName), GmatTree::ICON_XY_PLOT, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::XY_PLOT));
      }
      else if (objTypeName == "OrbitView")
      {
         AppendItem(itemId, wxT(objName), GmatTree::ICON_ORBIT_VIEW, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::ORBIT_VIEW));
      }
      else if (objTypeName == "GroundTrackPlot")
      {
         AppendItem(itemId, wxT(objName), GmatTree::RESOURCE_ICON_GROUND_TRACK_PLOT, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::GROUND_TRACK_PLOT));
      }
      else if (objTypeName == "EphemerisFile")
      {
         AppendItem(itemId, wxT(objName), GmatTree::ICON_REPORT_FILE, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::EPHEMERIS_FILE));
      }
      else
      {
         // User subscribers
         AppendItem(itemId, wxT(objName), GmatTree::ICON_REPORT_FILE, -1,
                    new GmatTreeItemData(wxT(objName),
                                         GmatTree::SUBSCRIBER));
      }
   }

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
   if (GmatGlobal::Instance()->IsMatlabAvailable())
   {
      AppendItem(itemId, wxT("Matlab"), GmatTree::RESOURCE_ICON_DEFAULT, -1,
                 new GmatTreeItemData(wxT("Matlab"), GmatTree::MATLAB_INTERFACE));
      AppendItem(itemId, wxT("Matlab Server"), GmatTree::RESOURCE_ICON_DEFAULT, -1,
                 new GmatTreeItemData(wxT("Matlab Server"), GmatTree::MATLAB_SERVER));
   }
   
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
      //if (param->GetKey() == GmatParam::USER_PARAM)
      if (param->GetTypeName() == "Variable")
      {
         AppendItem(itemId, wxT(objName), GmatTree::ICON_VARIABLE, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::VARIABLE));
      }
      else if (param->GetTypeName() == "Array")
      {
         AppendItem(itemId, wxT(objName), GmatTree::ICON_ARRAY, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::ARRAY));
      }
      else if (param->GetTypeName() == "String")
      {
         AppendItem(itemId, wxT(objName), GmatTree::ICON_STRING, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::STRING));
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
   StringArray itemNames = GmatAppData::Instance()->GetGuiInterpreter()->
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

         if (!GmatGlobal::Instance()->IsMatlabAvailable())
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_, "MATLAB is not available.  GMAT will not run\n"
                "if any CallFunction uses MATLAB function: %s\n", objName.c_str());
         }
      }
      else if (objTypeName == "GmatFunction")
      {
         AppendItem(itemId, wxT(objName), GmatTree::ICON_FUNCTION, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::GMAT_FUNCTION));
      }
   }

   //----- Predefined functions
   //mPredefinedFunctItem =
   //   AppendItem(itemId, wxT("Predefined Functions"), GmatTree::ICON_FOLDER, -1,
   //              new GmatTreeItemData(wxT("Functions"),
   //                                   GmatTree::PREDEFINED_FUNCTION_FOLDER));

   //SetItemImage(mPredefinedFunctItem, GmatTree::ICON_OPENFOLDER,
   //             wxTreeItemIcon_Expanded);

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
   StringArray itemNames = GmatAppData::Instance()->GetGuiInterpreter()->
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
   StringArray itemNames =
      GmatAppData::Instance()->GetGuiInterpreter()->GetListOfObjects(Gmat::CALCULATED_POINT);
   int size = itemNames.size();
   wxString objName;
   wxString objTypeName;

   for (int i = 0; i<size; i++)
   {
      GmatBase *cp = GetObject(itemNames[i]);
      objName = wxString(itemNames[i].c_str());
      objTypeName = wxString(cp->GetTypeName().c_str());

      if (objTypeName == "Barycenter")
      {
         AppendItem(itemId, wxT(objName), GmatTree::RESOURCE_ICON_BARYCENTER, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::BARYCENTER));
      }
      else if (objTypeName == "LibrationPoint")
      {
         AppendItem(itemId, wxT(objName), GmatTree::RESOURCE_ICON_LIBRATION_POINT, -1,
                    new GmatTreeItemData(wxT(objName), GmatTree::LIBRATION_POINT));
      }
   };

   if (size > 0)
      Expand(itemId);

}


//------------------------------------------------------------------------------
// void ResourceTree::AddUserObjects()
//------------------------------------------------------------------------------
/**
 * Handle the objects not covered by the defaults
 */
//------------------------------------------------------------------------------
void ResourceTree::AddUserObjects()
{
   #ifdef DEBUG_USER_GUI
   MessageInterface::ShowMessage
      ("ResourceTree::AddUserObjects() entered\n");
   #endif
   
   StringArray itemNames;
   Gmat::ObjectType type;
   std::string subtype;
   wxString objName;
   wxString objTypeName;
   wxTreeItemId itemId;

   #ifdef DEBUG_USER_GUI
      MessageInterface::ShowMessage("Found %d user nodes\n",
            mPluginItems.size());
   #endif
   for (UnsignedInt j = 0; j < mPluginItems.size(); ++j)
   {
      itemId = mPluginItems[j];
      type = nodeTypeMap[GetItemText(itemId).c_str()];
      subtype = nodeSubtypeMap[GetItemText(itemId).c_str()];

      itemNames =
            GmatAppData::Instance()->GetGuiInterpreter()->GetListOfObjects(type);
      int size = itemNames.size();

      #ifdef DEBUG_USER_GUI
         MessageInterface::ShowMessage("Adding %d type %d subtype \"%s\" "
               "objects\n", size, type, subtype.c_str());
      #endif

      for (int i = 0; i < size; i++)
      {
         #ifdef DEBUG_USER_GUI
            MessageInterface::ShowMessage("   Object[%d] \"%s\"\n", i,
                  itemNames[i].c_str());
         #endif
         GmatBase *cp = GetObject(itemNames[i]);
         GmatTree::ResourceIconType iconToUse = GetIconId(cp);
         
         if (subtype == "")
         {
            objName = wxString(itemNames[i].c_str());
            objTypeName = wxString(cp->GetTypeName().c_str());
            
            AppendItem(itemId, wxT(objName), iconToUse, -1,
                  new GmatTreeItemData(wxT(objName),
                        GmatTree::USER_DEFINED_OBJECT));
         }
         else if(cp->IsOfType(subtype))
         {
            objName = wxString(itemNames[i].c_str());
            objTypeName = wxString(cp->GetTypeName().c_str());
            
            AppendItem(itemId, wxT(objName), iconToUse, -1,
                  new GmatTreeItemData(wxT(objName),
                        GmatTree::USER_DEFINED_OBJECT));
         }
      }
      
      if (size > 0)
         Expand(itemId);
   }
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
   #ifdef DEBUG_ITEM_ACTIVATED
   MessageInterface::ShowMessage
      ("ResourceTree::OnItemActivated() title='%s'\n   name='%s'\n",
       item->GetTitle().c_str(), item->GetName().c_str());
   #endif
   theMainFrame->CreateChild(item);
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
   theMainFrame->CreateChild(item);
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
   if (theMainFrame->IsChildOpen(item))
      // close the window
      theMainFrame->CloseActiveChild();
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
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage("ResourceTree::OnRename() entered\n");
   #endif
   
   wxTreeItemId itemId = GetSelection();
   GmatTreeItemData *selItemData = (GmatTreeItemData *) GetItemData(itemId);
   wxString oldName = selItemData->GetName();
   GmatTree::ItemType itemType = selItemData->GetItemType();
   
   wxString newName = oldName;
   wxString name;
   if (GetNameFromUser(name, oldName, "Enter new name:", "Rename") == 1)
      newName = name;
   
   if ( !newName.IsEmpty() && !(newName.IsSameAs(oldName)))
   {
      Gmat::ObjectType objType = GetObjectType(itemType);

      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage
         ("ResourceTree::OnRename() itemType = %d, objType = %d\n",
          itemType, objType);
      #endif

      if (objType == Gmat::UNKNOWN_OBJECT)
         return;
      
      // If user wants to save data from the currently opened panels
      if (theMainFrame->GetNumberOfChildOpen() > 0)
      {
         if (wxMessageBox(_T("GMAT will save data from the currently opened "
                             "panels first.\nDo you want to continue?"),
                          _T("Please confirm"), wxICON_QUESTION | wxYES_NO) == wxYES)
         {
            // save any GUI data before renaming object from base code
            if (!theGuiManager->PrepareObjectNameChange())
            {
               wxMessageBox(wxT("GMAT cannot continue with Rename.\nPlease fix problem "
                                "with the current panel before renaming."),
                            wxT("GMAT Warning"));
               return;
            }
         }
         else
         {
            return;
         }
      }
      
      // update item only if successful
      if (theGuiInterpreter->
          RenameObject(objType, oldName.c_str(), newName.c_str()))
      {
         SetItemText(itemId, newName);
         theMainFrame->RenameChild(selItemData, newName);
         GmatTreeItemData *selItemData = (GmatTreeItemData *) GetItemData(itemId);
         
         // Set item name to new name and title to "" so that GmatMainFrame::CreateChild()
         // can append object type name to title (loj: 2009.06.08, Bug fix: 1478, 1479)
         selItemData->SetTitle("");
         selItemData->SetName(newName);
         
         // notify object name changed to panels which listens to resource update
         UpdateGuiItem(itemType);
         theGuiManager->NotifyObjectNameChange(objType, oldName, newName);
         
         // update formation which may use new spacecraft name
         if (objType == Gmat::SPACECRAFT)
         {
            Collapse(mSpacecraftItem);
            DeleteChildren(mSpacecraftItem);
            
            ////----- Hardware is child of spacecraft, so create a folder
            //AddItemFolder(mSpacecraftItem, mHardwareItem, "Hardware",
            //              GmatTree::HARDWARE_FOLDER);
            //AddDefaultHardware(mHardwareItem);
            AddDefaultSpacecraft(mSpacecraftItem);
            
            //@todo Select the item just renamed, currently it selects Spacecraft folder
            //SelectItem(itemId);
            
            Collapse(mFormationItem);
            DeleteChildren(mFormationItem);
            AddDefaultFormations(mFormationItem);
         }
         
         // update variables which may use new object name
         Collapse(mVariableItem);
         DeleteChildren(mVariableItem);
         AddDefaultVariables(mVariableItem);
         
         // update MissionTree for resource rename
         GmatAppData::Instance()->GetMissionTree()->UpdateMissionForRename();
      }
      else
      {
         MessageInterface::ShowMessage
            ("ResourceTree::OnRename() Unable to rename %s to %s.\n",
             oldName.c_str(), newName.c_str());
      }
   }
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage("ResourceTree::OnRename() rename leavinig\n");
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
   wxTreeItemId itemId = GetSelection();
   GmatTreeItemData *selItemData = (GmatTreeItemData *) GetItemData(itemId);
   #ifdef DEBUG_DELETE
   if (selItemData == NULL)
      MessageInterface::ShowMessage("selItemData is NULL!!!!!!!\n");
   MessageInterface::ShowMessage
      ("ResourceTree::OnDelete() name=%s\n", selItemData->GetName().c_str());
   #endif
   GmatTree::ItemType itemType = selItemData->GetItemType();

   // if panel is currently opened, give warning and return
   // Bug 547 fix (loj: 2008.11.25)
   if (theMainFrame->IsChildOpen(selItemData))
   {
      wxLogWarning("%s %s", selItemData->GetName().c_str(), "cannot be deleted "
                   "while panel is opened");
      wxLog::FlushActive();
      return;
   }

   Gmat::ObjectType objType = GetObjectType(itemType);
   if (objType == Gmat::UNKNOWN_OBJECT)
      return;

   #ifdef DEBUG_DELETE
   MessageInterface::ShowMessage
      ("ResourceTree::OnDelete() name=%s\n", selItemData->GetName().c_str());
   #endif

   wxString itemName = selItemData->GetName();
   // delete item if object successfully deleted
   if (theGuiInterpreter->RemoveObjectIfNotUsed(objType, itemName.c_str()))
   {
      // delete item and close all opened windows
      this->Delete(itemId);

      // We don't want to delete all children (bug 547 fix, loj: 2008.11.25)
      //theMainFrame->CloseAllChildren(false, true);

      #ifdef DEBUG_DELETE
      MessageInterface::ShowMessage
         ("ResourceTree::OnDelete() now calling theGuiManager->UpdateAll(%d)\n",
          objType);
      #endif
      theGuiManager->UpdateAll(objType);
   }
   else
   {
      wxLogWarning("\"%s\" %s", selItemData->GetName().c_str(), "cannot be "
            "deleted.\nIt is currently used in other resource or command "
            "sequence");
      wxLog::FlushActive();
   }
   #ifdef DEBUG_DELETE
   MessageInterface::ShowMessage
      ("exiting ResourceTree::OnDelete()\n");
   #endif
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
   GmatTreeItemData *selItemData = (GmatTreeItemData *) GetItemData(item);
   wxString name = selItemData->GetName();
   GmatTree::ItemType itemType = selItemData->GetItemType();

   if ( (itemType == GmatTree::SPACECRAFT) ||
        (itemType == GmatTree::FUELTANK) ||
        (itemType == GmatTree::THRUSTER) ||
        (itemType == GmatTree::IMPULSIVE_BURN) ||
        (itemType == GmatTree::FINITE_BURN) ||
        (itemType == GmatTree::PROPAGATOR) ||
//        (itemType == GmatTree::SPK_PROPAGATOR) ||
        (itemType == GmatTree::REPORT_FILE) ||
        (itemType == GmatTree::XY_PLOT) ||
        (itemType == GmatTree::ORBIT_VIEW) ||
        (itemType == GmatTree::GROUND_TRACK_PLOT) ||
        (itemType == GmatTree::EPHEMERIS_FILE) ||
        (itemType == GmatTree::DIFF_CORR) ||
        (itemType == GmatTree::SQP) ||
        (itemType == GmatTree::HARDWARE) ||
        (itemType == GmatTree::SOLVER) ||
        (itemType == GmatTree::BARYCENTER) ||
        (itemType == GmatTree::VARIABLE) ||
        (itemType == GmatTree::ARRAY) ||
        (itemType == GmatTree::STRING) ||
        (itemType == GmatTree::LIBRATION_POINT) ||
        (itemType == GmatTree::MATLAB_FUNCTION) || 
        (itemType == GmatTree::GMAT_FUNCTION))
   {
      const std::string stdName = name.c_str();
      std::string newName = theGuiInterpreter->AddClone(stdName);

      if (newName != "")
      {
         AddNode(itemType, newName.c_str());
         UpdateGuiItem(itemType);
      }
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

   GmatTreeItemData *selItemData = (GmatTreeItemData *)
      GetItemData(event.GetItem());

   int itemType = selItemData->GetItemType();
   bool isDefaultFolder = ((itemType == GmatTree::RESOURCES_FOLDER)     ||
                           (itemType == GmatTree::GROUND_STATION_FOLDER)||
                           (itemType == GmatTree::SPACECRAFT_FOLDER)    ||
                           (itemType == GmatTree::HARDWARE_FOLDER)      ||
                           (itemType == GmatTree::FORMATION_FOLDER)     ||
                           (itemType == GmatTree::CONSTELLATION_FOLDER) ||
                           (itemType == GmatTree::BURN_FOLDER)          ||
                           (itemType == GmatTree::PROPAGATOR_FOLDER)    ||
                           (itemType == GmatTree::SOLAR_SYSTEM)         ||
                           (itemType == GmatTree::SOLVER_FOLDER)        ||
                           (itemType == GmatTree::SUBSCRIBER_FOLDER)    ||
                           (itemType == GmatTree::INTERFACE_FOLDER)     ||
                           (itemType == GmatTree::VARIABLE_FOLDER));

   bool isDefaultItem = ((itemType == GmatTree::PROPAGATOR)       ||
//                         (itemType == GmatTree::SPK_PROPAGATOR)   ||
                         (itemType == GmatTree::CELESTIAL_BODY)   ||
                         (itemType == GmatTree::DIFF_CORR)        ||
                         (itemType == GmatTree::REPORT_FILE)      ||
                         (itemType == GmatTree::XY_PLOT)          ||
                         (itemType == GmatTree::ORBIT_VIEW)       ||
                         (itemType == GmatTree::GROUND_TRACK_PLOT)||
                         (itemType == GmatTree::EPHEMERIS_FILE)   ||
                         (itemType == GmatTree::INTERFACE));

   //kind of redundant because OpenPage returns false for some
   //of the default folders
   if ((theMainFrame->IsChildOpen(selItemData))  ||
       (isDefaultFolder)                         ||
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
      GmatTreeItemData *selItemData = (GmatTreeItemData *)
         GetItemData(GetSelection());

      wxString oldLabel = selItemData->GetName();
      int itemType = selItemData->GetItemType();

      selItemData->SetTitle(newLabel);

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
         //     selItemData->SetTitle(label);
         //  else
         //     selItemData->SetTitle(oldLabel);

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
   #ifdef DEBUG_ADD_ICONS
   MessageInterface::ShowMessage
      ("ResourceTree::AddIcons() entered, GmatTree::ICON_COUNT=%d\n", GmatTree::ICON_COUNT);
   #endif
   
   int sizeW = 16;
   int sizeH = 16;
   
   wxImageList *images = new wxImageList ( sizeW, sizeH, true );
   wxBitmap* bitmaps[GmatTree::ICON_COUNT];
   int index = 0;
   long bitmapType = wxBITMAP_TYPE_PNG;

   // Note:
   // Add icons by the order of enum ResourceIconType in GmatTreeItemData.hpp
   
   theGuiManager->LoadIcon("ClosedFolder", bitmapType, &bitmaps[index], ClosedFolder_xpm); 
   theGuiManager->LoadIcon("file", bitmapType, &bitmaps[++index], file_xpm);   
   theGuiManager->LoadIcon("OpenFolder", bitmapType, &bitmaps[++index], OpenFolder_xpm);   
   theGuiManager->LoadIcon("Groundstation", bitmapType, &bitmaps[++index], Groundstation_xpm);   
   theGuiManager->LoadIcon("spacecraft", bitmapType, &bitmaps[++index], spacecraft_xpm);
   theGuiManager->LoadIcon("propagator", bitmapType, &bitmaps[++index], propagator_xpm);
   
   theGuiManager->LoadIcon("tank", bitmapType, &bitmaps[++index], tank_xpm);
   theGuiManager->LoadIcon("thruster", bitmapType, &bitmaps[++index], thruster_xpm);
   theGuiManager->LoadIcon("rt_SolarSystem", bitmapType, &bitmaps[++index], rt_SolarSystem_xpm);
   theGuiManager->LoadIcon("sun", bitmapType, &bitmaps[++index], sun_xpm);
   theGuiManager->LoadIcon("mercury", bitmapType, &bitmaps[++index], mercury_xpm);
   theGuiManager->LoadIcon("venus", bitmapType, &bitmaps[++index], venus_xpm);
      
   theGuiManager->LoadIcon("earth", bitmapType, &bitmaps[++index], earth_xpm);
   theGuiManager->LoadIcon("mars", bitmapType, &bitmaps[++index], mars_xpm);
   theGuiManager->LoadIcon("jupiter", bitmapType, &bitmaps[++index], jupiter_xpm);
   theGuiManager->LoadIcon("saturn", bitmapType, &bitmaps[++index], saturn_xpm);
   theGuiManager->LoadIcon("uranus", bitmapType, &bitmaps[++index], uranus_xpm);
   
   theGuiManager->LoadIcon("neptune", bitmapType, &bitmaps[++index], neptune_xpm);
   theGuiManager->LoadIcon("pluto", bitmapType, &bitmaps[++index], pluto_xpm);
   theGuiManager->LoadIcon("report", bitmapType, &bitmaps[++index], report_xpm);
   theGuiManager->LoadIcon("network", bitmapType, &bitmaps[++index], network_xpm);
   theGuiManager->LoadIcon("rt_ImpulsiveBurn", bitmapType, &bitmaps[++index], rt_ImpulsiveBurn_xpm);
   theGuiManager->LoadIcon("rt_FiniteBurn", bitmapType, &bitmaps[++index], rt_FiniteBurn_xpm);
   
   theGuiManager->LoadIcon("moon", bitmapType, &bitmaps[++index], moon_xpm);
   theGuiManager->LoadIcon("moon_generic", bitmapType, &bitmaps[++index], moon_generic_xpm);
   theGuiManager->LoadIcon("planet_generic", bitmapType, &bitmaps[++index], planet_generic_xpm);
   theGuiManager->LoadIcon("comet", bitmapType, &bitmaps[++index], comet_xpm);
   theGuiManager->LoadIcon("asteroid", bitmapType, &bitmaps[++index], asteroid_xpm);
   theGuiManager->LoadIcon("rt_Barycenter", bitmapType, &bitmaps[++index], rt_Barycenter_xpm);
   theGuiManager->LoadIcon("rt_LibrationPoint", bitmapType, &bitmaps[++index], rt_LibrationPoint_xpm);
   
   theGuiManager->LoadIcon("matlabfunction", bitmapType, &bitmaps[++index], matlabfunction_xpm);
   theGuiManager->LoadIcon("function", bitmapType, &bitmaps[++index], function_xpm);
   theGuiManager->LoadIcon("coordinatesystem", bitmapType, &bitmaps[++index], coordinatesystem_xpm);
   
   theGuiManager->LoadIcon("openglplot", bitmapType, &bitmaps[++index], openglplot_xpm);
   theGuiManager->LoadIcon("xyplot", bitmapType, &bitmaps[++index], xyplot_xpm);
   theGuiManager->LoadIcon("rt_GroundTrackPlot", bitmapType, &bitmaps[++index], rt_GroundTrackPlot_xpm);
   
   theGuiManager->LoadIcon("variable", bitmapType, &bitmaps[++index], variable_xpm);
   theGuiManager->LoadIcon("array", bitmapType, &bitmaps[++index], array_xpm);
   theGuiManager->LoadIcon("string", bitmapType, &bitmaps[++index], string_xpm);
   
   theGuiManager->LoadIcon("rt_BoundaryValueSolver", bitmapType, &bitmaps[++index], rt_BoundaryValueSolver_xpm);
   theGuiManager->LoadIcon("rt_Optimizer", bitmapType, &bitmaps[++index], rt_Optimizer_xpm);
   theGuiManager->LoadIcon("rt_Simulator", bitmapType, &bitmaps[++index], rt_Simulator_xpm);
   theGuiManager->LoadIcon("rt_Estimator", bitmapType, &bitmaps[++index], rt_Estimator_xpm);
   
   theGuiManager->LoadIcon("rt_Antenna", bitmapType, &bitmaps[++index], rt_Antenna_xpm);
   theGuiManager->LoadIcon("rt_Transmitter", bitmapType, &bitmaps[++index], rt_Transmitter_xpm);
   theGuiManager->LoadIcon("rt_Receiver", bitmapType, &bitmaps[++index], rt_Receiver_xpm);
   theGuiManager->LoadIcon("rt_Transponder", bitmapType, &bitmaps[++index], rt_Transponder_xpm);
   
   theGuiManager->LoadIcon("rt_Matlab", bitmapType, &bitmaps[++index], rt_Matlab_xpm);
   theGuiManager->LoadIcon("rt_MatlabServer", bitmapType, &bitmaps[++index], rt_MatlabServer_xpm);

   theGuiManager->LoadIcon("rt_MeasurementModel", bitmapType, &bitmaps[++index], rt_MeasurementModel_xpm);
   theGuiManager->LoadIcon("Script", bitmapType, &bitmaps[++index], Script_xpm);
   theGuiManager->LoadIcon("rt_Default", bitmapType, &bitmaps[++index], rt_Default_xpm);
   
   
   // Let's always recale all icons since size of icon look different on different platforms
   for ( size_t i = 0; i < WXSIZEOF(bitmaps); i++ )
      images->Add(bitmaps[i]->ConvertToImage().Rescale(sizeW, sizeH));
   
   AssignImageList(images);
   
   #ifdef DEBUG_ADD_ICONS
   MessageInterface::ShowMessage
      ("ResourceTree::AddIcons() exiting, %d icons added\n", index + 1);
   #endif
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
   wxString name, centralBody, prompt;
   
   //Get name from the user first
   if (GetNameFromUser(name, name, "Enter name: ", "Celestial Body") == 1)
   {
      if (!name.IsEmpty())
      {
         prompt = "Enter Central Body for " + name;
         if (GetNameFromUser(centralBody, centralBody, prompt, "Celestial Body") == 1)
         {
            if (!centralBody.IsEmpty())
            {
               const std::string newName = name.c_str();
               const std::string newCentralBody = centralBody.c_str();
               GmatBase *obj = theGuiInterpreter->CreateObject("Moon", newName); // ************
               ((CelestialBody*) obj)->SetCentralBody(newCentralBody);
               
               if (obj != NULL)
               {
                  // need to get teh sub-item for the appropriate central body here ...
                  AppendItem(item, name, GmatTree::ICON_MOON, -1,
                             new GmatTreeItemData(name, GmatTree::CELESTIAL_BODY));
                  Expand(item);
                  
                  theGuiManager->UpdateCelestialPoint();
               }
               
               SelectItem(GetLastChild(item));
            }
         }
      }
   }
}


//------------------------------------------------------------------------------
// void OnAddGroundStation(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a ground station to ground station folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddGroundStation(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("GroundStation", 1);
   GmatBase *obj = theGuiInterpreter->CreateObject("GroundStation", newName);

   if (obj != NULL)
   {
      wxString name = newName.c_str();
      AppendItem(item, name, GmatTree::ICON_GROUND_STATION, -1,
                 new GmatTreeItemData(name, GmatTree::GROUND_STATION));
      Expand(item);

      theGuiManager->UpdateGroundStation();
   }
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
   std::string newName = theGuiInterpreter->GetNewName("Spacecraft", 1);
   GmatBase *obj = theGuiInterpreter->CreateObject("Spacecraft", newName);

   if (obj != NULL)
   {
      wxString name = newName.c_str();
      AppendItem(item, name, GmatTree::ICON_SPACECRAFT, -1,
                 new GmatTreeItemData(name, GmatTree::SPACECRAFT));
      Expand(item);
      
      theGuiManager->UpdateSpacecraft();
      // Update CoordindateSystem folder since Moderator creates default
      // coordinate systems if not created already when creating a spacecraft
      theGuiManager->UpdateCoordSystem();
      DeleteChildren(mCoordSysItem);
      AddDefaultCoordSys(mCoordSysItem);
   }
}


//------------------------------------------------------------------------------
// void OnAddFuelTank(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a fuel tank to hardware folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddFuelTank(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("FuelTank", 1);
   GmatBase *obj = theGuiInterpreter->CreateObject("FuelTank", newName);

   if (obj != NULL)
   {
      wxString name = newName.c_str();
      AppendItem(item, name, GmatTree::ICON_TANK, -1,
                 new GmatTreeItemData(name, GmatTree::FUELTANK));
      Expand(item);

      theGuiManager->UpdateFuelTank();
   }
}


//------------------------------------------------------------------------------
// void OnAddThruster(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a thruster to hardware folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddThruster(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("Thruster", 1);
   GmatBase *obj = theGuiInterpreter->CreateObject("Thruster", newName);

   if (obj != NULL)
   {
      wxString name = newName.c_str();
      AppendItem(item, name, GmatTree::ICON_THRUSTER, -1,
                 new GmatTreeItemData(name, GmatTree::THRUSTER));
      Expand(item);

      theGuiManager->UpdateThruster();
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
   std::string newName = theGuiInterpreter->GetNewName("Formation", 1);
   GmatBase *obj = theGuiInterpreter->CreateObject("Formation", newName);

   if (obj != NULL)
   {
      wxString name = newName.c_str();
      wxTreeItemId formationItem =
      AppendItem(item, name, GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(name, GmatTree::FORMATION));
      SetItemImage(formationItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);
      Expand(item);

      theGuiManager->UpdateFormation();
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
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("Propagator", 1);
   GmatBase *obj = theGuiInterpreter->CreateDefaultPropSetup(newName);

   if (obj != NULL)
   {
      wxString name = newName.c_str();
      AppendItem(item, name, GmatTree::ICON_PROPAGATOR, -1,
                 new GmatTreeItemData(name, GmatTree::PROPAGATOR));
      Expand(item);

      theGuiManager->UpdatePropagator();
   }
   else
   {
      MessageInterface::ShowMessage
         ("ResourceTree::OnAddPropagator() propSetup is NULL\n");
   }
}


////------------------------------------------------------------------------------
//// void OnAddSPKPropagator(wxCommandEvent &event)
////------------------------------------------------------------------------------
///**
// * Add a SPK propagator to propagator folder
// *
// * @param <event> command event
// */
////------------------------------------------------------------------------------
//void ResourceTree::OnAddSPKPropagator(wxCommandEvent &event)
//{
//   wxTreeItemId item = GetSelection();
//   std::string newName = theGuiInterpreter->GetNewName("SPKPropagator", 1);
//   GmatBase *obj = theGuiInterpreter->CreateDefaultPropSetup(newName);
//
//   if (obj != NULL)
//   {
//      wxString name = newName.c_str();
//      AppendItem(item, name, GmatTree::ICON_PROPAGATOR, -1,
//                 new GmatTreeItemData(name, GmatTree::SPK_PROPAGATOR));
//      Expand(item);
//
//      theGuiManager->UpdatePropagator();
//   }
//   else
//   {
//      MessageInterface::ShowMessage
//         ("ResourceTree::OnAddSPKPropagator() propSetup is NULL\n");
//   }
//}


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
   std::string newName = theGuiInterpreter->GetNewName("ImpulsiveBurn", 1);
   GmatBase *obj = theGuiInterpreter->CreateObject("ImpulsiveBurn", newName, true);

   if (obj != NULL)
   {
      wxString name = newName.c_str();
      AppendItem(item, name, GmatTree::ICON_IMPULSIVE_BURN, -1,
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
   std::string newName = theGuiInterpreter->GetNewName("FiniteBurn", 1);
   GmatBase *obj = theGuiInterpreter->CreateObject("FiniteBurn", newName);

   if (obj != NULL)
   {
      wxString name = newName.c_str();
      AppendItem(item, name, GmatTree::ICON_FINITE_BURN, -1,
                 new GmatTreeItemData(name, GmatTree::FINITE_BURN));
      Expand(item);

      theGuiManager->UpdateBurn();
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
   #ifdef DEBUG_ADD_SOLVER
   MessageInterface::ShowMessage("Solver event ID = %d\n", event.GetId());
   #endif

   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("DC", 1);
   GmatBase *obj = theGuiInterpreter->CreateObject("DifferentialCorrector", newName);

   if (obj != NULL)
   {
      GmatTree::ResourceIconType iconToUse = GetIconId(obj);
      
      wxString name = newName.c_str();
      AppendItem(item, name, iconToUse, -1,
                 new GmatTreeItemData(name, GmatTree::DIFF_CORR));
      Expand(item);

      theGuiManager->UpdateSolver();
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
   #ifdef DEBUG_ADD_SOLVER
   MessageInterface::ShowMessage("Solver event ID = %d\n", event.GetId());
   #endif

   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("SQP", 1);
   GmatBase *obj = theGuiInterpreter->CreateObject("FminconOptimizer", newName);
   
   if (obj != NULL)
   {
      GmatTree::ResourceIconType iconToUse = GetIconId(obj);
      wxString name = newName.c_str();
      AppendItem(item, name, iconToUse, -1,
                 new GmatTreeItemData(name, GmatTree::SQP));
      Expand(item);
      
      theGuiManager->UpdateSolver();
   }
}


//------------------------------------------------------------------------------
// void OnAddHardware(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a generic hardware to hardware folder
 *
 * The code used here should be generalizable for other plugin elements as well.
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddHardware(wxCommandEvent &event)
{
   // Look up the plugin type based on the ID built with menu that selected it
   std::string selected = pluginMap[event.GetId()];

   // The rest is like the other tree additions
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName(selected, 1);
   
   GmatBase *obj = theGuiInterpreter->CreateObject(selected, newName);
   
   if (obj != NULL)
   {
      GmatTree::ResourceIconType iconToUse = GetIconId(obj);
      
      wxString name = newName.c_str();
      AppendItem(item, name, iconToUse, -1,
                 new GmatTreeItemData(name, GmatTree::HARDWARE));
      Expand(item);
      
      theGuiManager->UpdateSensor();
   }
}


//------------------------------------------------------------------------------
// void OnAddSolver(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a generic solver to solvers folder
 *
 * The code used here should be generalizable for other plugin elements as well.
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddSolver(wxCommandEvent &event)
{
   #ifdef DEBUG_ADD_SOLVER
   MessageInterface::ShowMessage("ResourceTree::OnAddSolver() entered\n");
   #endif
   
   // Look up the plugin type based on the ID built with menu that selected it
   std::string selected = pluginMap[event.GetId()];

   // The rest is like the other tree additions
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName(selected, 1);

   GmatBase *obj = theGuiInterpreter->CreateObject(selected, newName);

   if (obj != NULL)
   {
      GmatTree::ResourceIconType iconToUse = GetIconId(obj);
      wxString name = newName.c_str();
      AppendItem(item, name, iconToUse, -1,
                 new GmatTreeItemData(name, GmatTree::SOLVER));
      Expand(item);

      theGuiManager->UpdateSolver();
   }
   
   #ifdef DEBUG_ADD_SOLVER
   MessageInterface::ShowMessage("ResourceTree::OnAddSolver() leaving\n");
   #endif
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
   std::string newName = theGuiInterpreter->GetNewName("ReportFile", 1);
   GmatBase *obj = theGuiInterpreter->CreateSubscriber("ReportFile", newName);

   if (obj != NULL)
   {
      wxString name = newName.c_str();
      AppendItem(item, name, GmatTree::ICON_REPORT_FILE, -1,
                 new GmatTreeItemData(name, GmatTree::REPORT_FILE));
      Expand(item);

      theGuiManager->UpdateSubscriber();
   }
}

//------------------------------------------------------------------------------
// void OnAddXyPlot(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an xy plot to output folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddXyPlot(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("XYPlot", 1);
   GmatBase *obj = theGuiInterpreter->CreateSubscriber("XYPlot", newName);

   if (obj != NULL)
   {
      wxString name = newName.c_str();
      AppendItem(item, name, GmatTree::ICON_XY_PLOT, -1,
                 new GmatTreeItemData(name, GmatTree::XY_PLOT));
      Expand(item);

      theGuiManager->UpdateSubscriber();
   }
}


//------------------------------------------------------------------------------
// void OnAddOrbitView(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an OrbitView plot to output folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddOrbitView(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("OrbitView", 1);
   GmatBase *obj = theGuiInterpreter->CreateSubscriber("OrbitView", newName);
   
   if (obj != NULL)
   {
      wxString name = newName.c_str();
      AppendItem(item, name, GmatTree::ICON_ORBIT_VIEW, -1,
                 new GmatTreeItemData(name, GmatTree::ORBIT_VIEW));
      Expand(item);
      
      theGuiManager->UpdateSubscriber();
   }
}


//------------------------------------------------------------------------------
// void OnAddEphemerisFile(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an EphemerisFile to output folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddEphemerisFile(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("EphemerisFile", 1);
   GmatBase *obj = theGuiInterpreter->CreateSubscriber("EphemerisFile", newName);
   
   if (obj != NULL)
   {
      wxString name = newName.c_str();
      AppendItem(item, name, GmatTree::ICON_REPORT_FILE, -1,
                 new GmatTreeItemData(name, GmatTree::EPHEMERIS_FILE));
      Expand(item);
      
      theGuiManager->UpdateSubscriber();
   }
}


//------------------------------------------------------------------------------
// void OnAddSubscriber(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an generic subscriber to Subscribers folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddSubscriber(wxCommandEvent &event)
{
   std::string selected = pluginMap[event.GetId()];

   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName(selected, 1);
   GmatBase *obj = theGuiInterpreter->CreateSubscriber(selected, newName);

   if (obj != NULL)
   {
      GmatTree::ResourceIconType iconToUse = GmatTree::RESOURCE_ICON_DEFAULT;
      GmatTree::ItemType itemType = GmatTree::SUBSCRIBER;
      if (selected == "GroundTrackPlot")
      {
         iconToUse = GmatTree::RESOURCE_ICON_GROUND_TRACK_PLOT;
         itemType = GmatTree::GROUND_TRACK_PLOT;
      }
      
      wxString name = newName.c_str();
      AppendItem(item, name, iconToUse, -1,
                 new GmatTreeItemData(name, itemType));
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
   ParameterCreateDialog paramDlg(this, ParameterCreateDialog::VARIABLE);
   paramDlg.ShowModal();
}


//------------------------------------------------------------------------------
// void OnAddArray(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an array to variable folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddArray(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();

   // show dialog to create user parameter
   ParameterCreateDialog paramDlg(this, ParameterCreateDialog::ARRAY);
   paramDlg.ShowModal();
}


//------------------------------------------------------------------------------
// void OnAddString(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a string to variable folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddString(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();

   // show dialog to create user parameter
   ParameterCreateDialog paramDlg(this, ParameterCreateDialog::STRING);
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
   wxString name;
   
   //Get name from the user first
   if (GetNameFromUser(name, name, "Enter name: ", "MATLAB function") == 1)
   {
      if (!name.IsEmpty())
      {
         const std::string newName = name.c_str();
         GmatBase *obj = theGuiInterpreter->CreateObject("MatlabFunction", newName);
         
         if (obj != NULL)
         {
            AppendItem(item, name, GmatTree::ICON_MATLAB_FUNCTION, -1,
                       new GmatTreeItemData(name, GmatTree::MATLAB_FUNCTION));
            Expand(item);
            
            theGuiManager->UpdateFunction();
         }
         
         SelectItem(GetLastChild(item));
      }
   }
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
   #ifdef DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage("ResourceTree::OnAddGmatFunction() entered\n");
   #endif

   wxTreeItemId item = GetSelection();
   wxString name;
   GmatBase *obj = NULL;
   
   //-----------------------------------------------------------------
   // if creating a new GmatFunction
   //-----------------------------------------------------------------
   if (event.GetId() == POPUP_NEW_GMAT_FUNCTION)
   {
      if (GetNameFromUser(name, name, "Enter name: ", "New GMAT function") == 1)
      {
         if (!name.IsEmpty())
         {
            obj = theGuiInterpreter->CreateObject("GmatFunction", name.c_str());
            ((GmatFunction *)(obj))->SetNewFunction(true);
         }
      }
   }
   //-----------------------------------------------------------------
   // if opening an existing GmatFunction
   //-----------------------------------------------------------------
   else if (event.GetId() == POPUP_OPEN_GMAT_FUNCTION)
   {
      wxFileDialog dialog
         (this, _T("Choose a file"), _T(""), _T(""),
          _T("GMAT Function file (*.gmf|*.gmf|"\
             "Text files (*.txt, *.text)|*.txt;*.text|"\
             "All files (*.*)|*.*"));

      if (dialog.ShowModal() == wxID_OK)
      {
         wxString path = dialog.GetPath().c_str();
         wxString filename = dialog.GetFilename().c_str();
         name = filename.Mid(0, (filename.find('.')));

         #ifdef DEBUG_GMAT_FUNCTION
         MessageInterface::ShowMessage("   path='%s'\n", path.c_str());
         wxString directory = dialog.GetDirectory().c_str();
         MessageInterface::ShowMessage("   directory='%s'\n", directory.c_str());
         MessageInterface::ShowMessage("   filename='%s'\n", filename.c_str());
         MessageInterface::ShowMessage("   name='%s'\n", name.c_str());
         #endif

         // Create GmatFunction object
         obj = theGuiInterpreter->CreateObject("GmatFunction", name.c_str());
         // Set function path
         if (obj != NULL)
            obj->SetStringParameter("FunctionPath", path.c_str());
      }
   }

   if (obj != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_FUNCTION, -1,
                 new GmatTreeItemData(name, GmatTree::GMAT_FUNCTION));
      Expand(item);
      theGuiManager->UpdateFunction();
      SelectItem(GetLastChild(item));
   }

   #ifdef DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage("ResourceTree::OnAddGmatFunction() exiting\n");
   #endif

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
   std::string newName = theGuiInterpreter->GetNewName("Barycenter", 1);
   GmatBase *obj = theGuiInterpreter->CreateObject("Barycenter", newName);

   if (obj != NULL)
   {
      wxString name = newName.c_str();
      AppendItem(item, name, GmatTree::RESOURCE_ICON_BARYCENTER, -1,
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
   std::string newName = theGuiInterpreter->GetNewName("Libration", 1);
   GmatBase *obj = theGuiInterpreter->CreateObject("LibrationPoint", newName);

   if (obj != NULL)
   {
      DeleteChildren(mSpecialPointsItem);
      AddDefaultSpecialPoints(mSpecialPointsItem, true, false);
      Expand(item);

      theGuiManager->UpdateCelestialPoint();
   }
}


//------------------------------------------------------------------------------
// void OnAddPlanet(wxCommandEvent &event)
//------------------------------------------------------------------------------
void ResourceTree::OnAddPlanet(wxCommandEvent &event)
{
   // get the central body name
   wxTreeItemId item = GetSelection();
   GmatTreeItemData *selItemData = (GmatTreeItemData *) GetItemData(item);
   wxString cBodyName = selItemData->GetName();
   std::string cBody  = cBodyName.c_str();
   wxString name;
   
   //Get name from the user first
   if (GetNameFromUser(name, name, "Enter name: ", "New Planet") == 1)
   {
      if (!name.IsEmpty())
      {
         const std::string newName = name.c_str();
         SolarSystem   *ss         = theGuiInterpreter->GetSolarSystemInUse();
         if (ss->IsBodyInUse(newName))
         {
            std::string errmsg = "Error creating new planet: celestial body \"";
            errmsg += newName + "\" already exists.";
            MessageInterface::PopupMessage(Gmat::ERROR_, errmsg.c_str());
            return;
         }
         GmatBase *obj = theGuiInterpreter->CreateObject("Planet", newName);
         ((CelestialBody*)obj)->SetCentralBody(cBody);
         // For now, we only have one solar system with one star ...
         if (cBody == SolarSystem::SUN_NAME)
         {
            wxTreeItemId parent = GetItemParent(item);
            AppendItem(parent, name, GmatTree::ICON_PLANET_GENERIC, -1,
                       new GmatTreeItemData(name, GmatTree::CELESTIAL_BODY_PLANET));
            Expand(parent);
         }
         else
         {
            AppendItem(item, name, GmatTree::ICON_PLANET_GENERIC, -1,
                       new GmatTreeItemData(name, GmatTree::CELESTIAL_BODY_PLANET));
            Expand(item);
         }
         
         theGuiManager->UpdateSolarSystem();
      }
   }
}


//------------------------------------------------------------------------------
// void OnAddMoon(wxCommandEvent &event)
//------------------------------------------------------------------------------
void ResourceTree::OnAddMoon(wxCommandEvent &event)
{
   // get the central body name
   wxTreeItemId item = GetSelection();
   GmatTreeItemData *selItemData = (GmatTreeItemData *) GetItemData(item);
   wxString cBodyName = selItemData->GetName();
   std::string cBody  = cBodyName.c_str();
   wxString name;

   //Get name from the user first
   if (GetNameFromUser(name, name, "Enter name: ", "New Moon") == 1)
   {
      if (!name.IsEmpty())
      {
         const std::string newName = name.c_str();
         SolarSystem   *ss         = theGuiInterpreter->GetSolarSystemInUse();
         if (ss->IsBodyInUse(newName))
         {
            std::string errmsg = "Error creating new Moon: celestial body \"";
            errmsg += newName + "\" already exists.";
            MessageInterface::PopupMessage(Gmat::ERROR_, errmsg.c_str());
            return;
         }
         GmatBase *obj = theGuiInterpreter->CreateObject("Moon", newName);
         ((CelestialBody*)obj)->SetCentralBody(cBody);
         // For now, we only have one solar system with one star ...
         // Of course, a moon around a star makes no sense ...
         if (cBody == SolarSystem::SUN_NAME)
         {
            wxTreeItemId parent = GetItemParent(item);
            AppendItem(parent, name, GmatTree::ICON_MOON_GENERIC, -1,
                       new GmatTreeItemData(name, GmatTree::CELESTIAL_BODY_MOON));
            Expand(parent);
         }
         else
         {
            AppendItem(item, name, GmatTree::ICON_MOON_GENERIC, -1,
                       new GmatTreeItemData(name, GmatTree::CELESTIAL_BODY_MOON));
            Expand(item);
         }
         
         theGuiManager->UpdateSolarSystem();
      }
   }
}


//------------------------------------------------------------------------------
// void OnAddComet(wxCommandEvent &event)
//------------------------------------------------------------------------------
void ResourceTree::OnAddComet(wxCommandEvent &event)
{
   // get the central body name
   wxTreeItemId item = GetSelection();
   GmatTreeItemData *selItemData = (GmatTreeItemData *) GetItemData(item);
   wxString cBodyName = selItemData->GetName();
   std::string cBody  = cBodyName.c_str();
   wxString name;

   //Get name from the user first
   if (GetNameFromUser(name, name, "Enter name: ", "New Comet") == 1)
   {
      if (!name.IsEmpty())
      {
         const std::string newName = name.c_str();
         SolarSystem   *ss         = theGuiInterpreter->GetSolarSystemInUse();
         if (ss->IsBodyInUse(newName))
         {
            std::string errmsg = "Error creating new comet: celestial body \"";
            errmsg += newName + "\" already exists.";
            MessageInterface::PopupMessage(Gmat::ERROR_, errmsg.c_str());
            return;
         }
         GmatBase *obj = theGuiInterpreter->CreateObject("Comet", newName);
         #ifdef DEBUG_ADD_COMET
         if (obj == NULL)
            MessageInterface::ShowMessage("ERROR creating body!!\n");
         else
            MessageInterface::ShowMessage("Body %s created!!\n", name.c_str());
         #endif
         ((CelestialBody*)obj)->SetCentralBody(cBody);
         // For now, we only have one solar system with one star ...
         if (cBody == SolarSystem::SUN_NAME)
         {
            wxTreeItemId parent = GetItemParent(item);
            AppendItem(parent, name, GmatTree::ICON_COMET, -1,
                       new GmatTreeItemData(name, GmatTree::CELESTIAL_BODY_COMET));
            Expand(parent);
         }
         else
         {
            AppendItem(item, name, GmatTree::ICON_COMET, -1,
                       new GmatTreeItemData(name, GmatTree::CELESTIAL_BODY_COMET));
            Expand(item);
         }
         
         theGuiManager->UpdateSolarSystem();
      }
   }
   
}


//------------------------------------------------------------------------------
// void OnAddAsteroid(wxCommandEvent &event)
//------------------------------------------------------------------------------
void ResourceTree::OnAddAsteroid(wxCommandEvent &event)
{
   // get the central body name
   wxTreeItemId item = GetSelection();
   GmatTreeItemData *selItemData = (GmatTreeItemData *) GetItemData(item);
   wxString cBodyName = selItemData->GetName();
   std::string cBody  = cBodyName.c_str();
   wxString name;

   //Get name from the user first
   if (GetNameFromUser(name, name, "Enter name: ", "New Asteroid") == 1)
   {
      if (!name.IsEmpty())
      {
         const std::string newName = name.c_str();
         SolarSystem   *ss         = theGuiInterpreter->GetSolarSystemInUse();
         if (ss->IsBodyInUse(newName))
         {
            std::string errmsg = "Error creating new asteroid: celestial body \"";
            errmsg += newName + "\" already exists.";
            MessageInterface::PopupMessage(Gmat::ERROR_, errmsg.c_str());
            return;
         }
         GmatBase *obj = theGuiInterpreter->CreateObject("Asteroid", newName);
         ((CelestialBody*)obj)->SetCentralBody(cBody);
         // For now, we only have one solar system with one star ...
         if (cBody == SolarSystem::SUN_NAME)
         {
            wxTreeItemId parent = GetItemParent(item);
            AppendItem(parent, name, GmatTree::ICON_ASTEROID, -1,
                       new GmatTreeItemData(name, GmatTree::CELESTIAL_BODY_ASTEROID));
            Expand(parent);
         }
         else
         {
            AppendItem(item, name, GmatTree::ICON_ASTEROID, -1,
                       new GmatTreeItemData(name, GmatTree::CELESTIAL_BODY_ASTEROID));
            Expand(item);
         }
         
         theGuiManager->UpdateSolarSystem();
      }
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


   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""),
         _T("Script files (*.script, *.m)|*.script;*.m|"\
            "Text files (*.txt, *.text)|*.txt;*.text|"\
            "All files (*.*)|*.*"));


   if (dialog.ShowModal() == wxID_OK)
   {
      wxString path = dialog.GetPath().c_str();
      AddScript(path);
   }
   else
   {
      mScriptAdded = false;
   }
}


//------------------------------------------------------------------------------
// void OnAddUserObject(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a generic solver to solvers folder
 *
 * The code used here should be generalizable for other plugin elements as well.
 *
 * @param event command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddUserObject(wxCommandEvent &event)
{
   // Look up the plugin type based on the ID built with menu that selected it
   std::string selected = pluginMap[event.GetId()];

   // The rest is like the other tree additions
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName(selected, 1);

   GmatBase *obj = theGuiInterpreter->CreateObject(selected, newName);
   
   if (obj != NULL)
   {
      GmatTree::ResourceIconType iconToUse = GetIconId(obj);
      
      wxString name = newName.c_str();
      AppendItem(item, name, iconToUse, -1,
                 new GmatTreeItemData(name, GmatTree::USER_DEFINED_OBJECT));
      Expand(item);
   }
}


//------------------------------------------------------------------------------
// void UpdateMatlabServerItem(bool start)
//------------------------------------------------------------------------------
void ResourceTree::UpdateMatlabServerItem(bool started)
{
   mMatlabServerStarted = started;
}


//------------------------------------------------------------------------------
// bool WasScriptAdded()
//------------------------------------------------------------------------------
bool ResourceTree::WasScriptAdded()
{
   return mScriptAdded;
}


//------------------------------------------------------------------------------
// wxString GetLastScriptAdded()
//------------------------------------------------------------------------------
wxString ResourceTree::GetLastScriptAdded()
{
   return mLastScriptAdded;
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
      wxString name = ((GmatTreeItemData *)GetItemData(lastChild))->GetName();
      GmatTree::ItemType itemType =
         ((GmatTreeItemData *)GetItemData(lastChild))->GetItemType();

      // close window and delte item
      theMainFrame->RemoveChild(name, itemType);
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
   wxString name = ((GmatTreeItemData *)GetItemData(item))->GetName();
   GmatTree::ItemType itemType = ((GmatTreeItemData *)GetItemData(item))->GetItemType();
   wxTreeItemId parentItem = GetItemParent(item);

   // close window and delete item
   theMainFrame->RemoveChild(name, itemType);
   Delete(item);
}


//------------------------------------------------------------------------------
// bool OnScriptBuildObject(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ResourceTree::OnScriptBuildObject(wxCommandEvent& event)
{
   //MessageInterface::ShowMessage("===> OnScriptBuildObject()\n");

   // Get info from selected item
   GmatTreeItemData *item = (GmatTreeItemData *) GetItemData(GetSelection());
   wxString filename = item->GetName();

   BuildScript(filename, GmatGui::ALWAYS_OPEN_SCRIPT);
}


//------------------------------------------------------------------------------
// bool OnScriptBuildAndRun(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ResourceTree::OnScriptBuildAndRun(wxCommandEvent& event)
{
   #ifdef DEBUG_RESOURCE_TREE
   MessageInterface::ShowMessage("ResourceTree::OnScriptBuildAndRun()\n");
   #endif

   // Get info from selected item
   GmatTreeItemData *item = (GmatTreeItemData *) GetItemData(GetSelection());
   wxString filename = item->GetName();

   if (BuildScript(filename, GmatGui::ALWAYS_OPEN_SCRIPT))
      theMainFrame->RunCurrentMission();

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

      #ifdef DEBUG_RESOURCE_TREE
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
                  GmatTreeItemData *newItemData =
                     new GmatTreeItemData(filepath, GmatTree::SCRIPT_FILE);
                  AppendItem(newItem, filename, GmatTree::RESOURCE_ICON_SCRIPT, -1,
                             newItemData);
               }
               
               ifs.close();
            }
         }
         
         cont = dir.GetNext(&filename);
      }
      
      // Make it bold face if script is the same as currently active script file
      if (mLastActiveScript != "")
         MakeScriptActive(mLastActiveScript, true);
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
   if (theMainFrame == NULL)
   {
      MessageInterface::ShowMessage
         ("===> OnRunScriptsFromFolder() theMainFrame is not set\n");
      return;
   }

   wxTreeItemId itemId = GetSelection();
   wxString filename;
   wxTreeItemIdValue cookie;
   wxTreeItemId scriptId = GetFirstChild(itemId, cookie);
   int numScripts = 0;

   // find only script file, exclude script folder
   while (scriptId.IsOk())
   {
      #ifdef DEBUG_RUN_SCRIPT_FOLDER
      MessageInterface::ShowMessage
         ("ResourceTree::OnRunScriptsFromFolder() scriptText=<%s>\n",
          GetItemText(scriptId).c_str());
      #endif

      if (GetItemImage(scriptId) != GmatTree::ICON_FOLDER)
         numScripts++;

      scriptId = GetNextChild(itemId, cookie);
   }

   int startNum = 1;
   int runCount = numScripts;
   int repeatCount = 1;

   Real absTol = GmatFileUtil::CompareAbsTol;
   wxString compareDir1  = ((GmatTreeItemData*)GetItemData(itemId))->GetName();

   RunScriptFolderDialog dlg(this, numScripts, absTol, compareDir1);
   dlg.ShowModal();

   if (!dlg.RunScripts())
      return;

   startNum = dlg.GetStartingScriptNumber();
   runCount = dlg.GetNumScriptsToRun() + startNum - 1;
   repeatCount = dlg.GetNumTimesToRun();
   bool runFromSavedScripts = dlg.RunFromSavedScripts();
   bool compare = dlg.CompareResults();
   bool saveCompareResults = dlg.SaveCompareResults();
   wxString filterString = dlg.GetFilterString();
   bool builtOk = false;

   // for current output path
   FileManager *fm = FileManager::Instance();
   std::string oldOutPath = fm->GetFullPathname(FileManager::OUTPUT_PATH);
   std::string oldLogFile = MessageInterface::GetLogFileName();
   bool hasOutDirChanged = dlg.HasOutDirChanged();
   bool createRunFolder = dlg.CreateRunFolder();
   wxString sep = fm->GetPathSeparator().c_str();

   wxString currOutPath = dlg.GetCurrentOutDir() + sep;
   wxString savePath = dlg.GetSaveScriptsDir() + sep;

   // Create path if not exist
   if (!::wxDirExists(currOutPath))
      ::wxMkdir(currOutPath);

   int count = 0;
   mHasUserInterrupted = false;
   scriptId = GetFirstChild(itemId, cookie);

   wxTextCtrl *textCtrl = NULL;
   wxString tempStr;

   // create CompareReport Panel
   if (compare)
   {
      GmatMdiChildFrame *textFrame =
         theMainFrame->GetChild("CompareReport");

      if (textFrame == NULL)
      {
         GmatTreeItemData *compareItem =
            new GmatTreeItemData("CompareReport", GmatTree::COMPARE_REPORT);

         textFrame = theMainFrame->CreateChild(compareItem);
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
   
   // Set output path for reports and ephemeris files
   try
   {
      fm->SetAbsPathname(FileManager::OUTPUT_PATH, currOutPath.c_str());
      fm->SetAbsPathname(FileManager::EPHEM_PATH, currOutPath.c_str());
   }
   catch (UtilityException &ue)
   {
      MessageInterface::ShowMessage(ue.GetFullMessage());
      return;
   }
   
   // Change log path and append log messages
   MessageInterface::SetLogPath(currOutPath.c_str(), appendLog);

   // Write run start time
   wxDateTime now = wxDateTime::Now();
   wxString wxNowStr = now.FormatISODate() + " " + now.FormatISOTime() + " ";
   std::string nowStr = wxNowStr.c_str();

   MessageInterface::LogMessage(nowStr + "GMAT starting batch run.\n");

   clock_t t1 = clock();
   bool isFirstScript = true;

   while (scriptId.IsOk())
   {
      if (GetItemImage(scriptId) == GmatTree::ICON_FOLDER)
      {
         scriptId = GetNextChild(itemId, cookie);
         continue;
      }

      if (mHasUserInterrupted)
         break;

      count++;

      if (count < startNum)
      {
         scriptId = GetNextChild(itemId, cookie);
         continue;
      }

      if (count > runCount)
         break;

      // Set main frame title to script file name
      filename = ((GmatTreeItemData*)GetItemData(scriptId))->GetName();
      
      // Filter scripts
      if (filterString != "" && !filename.Contains(filterString))
      {
         scriptId = GetNextChild(itemId, cookie);
         continue;
      }
      
      wxString titleText;
      titleText.Printf("%s - General Mission Analysis Tool (GMAT)", filename.c_str());
      theMainFrame->SetTitle(titleText);

      // Set running script text
      wxString text;
      text.Printf("Running script %d of %d;   %s\n", count, runCount,
                  filename.c_str());

      if (compare)
         textCtrl->AppendText(text);

      for (int i=0; i<repeatCount; i++)
      {
         wxString outPath = currOutPath;

         if (createRunFolder)
         {
            outPath << "Run_" << i+1;
            if (!::wxDirExists(outPath))
               ::wxMkdir(outPath);

            outPath = outPath + "/";

            // Set output path
            fm->SetAbsPathname(FileManager::OUTPUT_PATH, outPath.c_str());

            // Change log path and append log messages
            MessageInterface::SetLogPath(outPath.c_str(), appendLog);

            if (isFirstScript)
            {
               MessageInterface::LogMessage(nowStr + "GMAT starting batch run.\n");
               isFirstScript = false;
            }
         }

         MessageInterface::ShowMessage
            ("\nStarting script %d of %d: %s\n", count, runCount, filename.c_str());

         MessageInterface::ShowMessage
            ("==> Run Count: %d\n", i+1);

         // Write repeat and run count to main frame status bar
         wxString text;
         text.Printf("Repeat Count %d of %d;  Running script %d of %d;  File:%s",
                     i+1, repeatCount, count, runCount, filename.c_str());
         theMainFrame->SetStatusText(text, 2);

         if (compare)
         {
            tempStr.Printf("%d", i+1);
            textCtrl->AppendText("==> Run Count: " + tempStr + "\n");
         }

         try
         {
            // Create objects from script only first time, to test re-run
            if (i == 0)
               builtOk = BuildScript(filename, GmatGui::DO_NOT_OPEN_SCRIPT, true,
                                     runFromSavedScripts, savePath, true);

            if (builtOk)
            {
               Integer retval = theMainFrame->RunCurrentMission();

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
               ("*** Error running: %s\n   %s\n", filename.c_str(),
                e.GetFullMessage().c_str());

            if (compare)
               textCtrl->AppendText(e.GetFullMessage().c_str());
         }
         catch (...)
         {
            MessageInterface::ShowMessage
               ("*** Unknown Error running: %s\n", filename.c_str());
         }
      }

      scriptId = GetNextChild(itemId, cookie);
      appendLog = true;
   }

   mScriptFolderRunning = false;

   // Write run end time
   now = wxDateTime::Now();
   wxNowStr = now.FormatISODate() + " " + now.FormatISOTime() + " ";
   nowStr = wxNowStr.c_str();

   MessageInterface::LogMessage(nowStr + "GMAT ended batch run.\n\n");

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
   theMainFrame->SetStatusText(text, 2);
   theMainFrame->SetStatusText("", 1);

   // Set batch mode to false
   GmatGlobal::Instance()->SetBatchMode(false);

   // Popup errors found message
   wxString msg1;
   wxString msg2;
   wxString msg3;
   std::string msg1Str;
   
   if (mBuildErrorCount > 0)
   {
      wxString scriptNames1;
      msg1.Printf("Script errors found in the following %d script(s):\n",
                  mBuildErrorCount);
      for (int i = 0; i < mBuildErrorCount; i++)
         scriptNames1 = scriptNames1 + mFailedScriptsList[i] + "\n";
      
      msg1 = msg1 + scriptNames1;
   }
   
   if (failedToRunScripts.GetCount() > 0)
   {
      int numFailed = failedToRunScripts.GetCount();
      wxString scriptNames2;
      msg2.Printf("\nThe following %d script(s) failed to run to completion:\n",
                  numFailed);
      
      for (int i = 0; i < numFailed; i++)
         scriptNames2 = scriptNames2 + failedToRunScripts[i] + "\n";
      
      msg2 = msg2 + scriptNames2;
   }
   
   if (runInterruptedScripts.GetCount() > 0)
   {
      int numInterrupted = failedToRunScripts.GetCount();
      wxString scriptNames3;
      msg3.Printf("\nThe following %d script(s) were interrupted by user:\n",
                  numInterrupted);
      
      for (int i = 0; i < numInterrupted; i++)
         scriptNames3 = scriptNames3 + runInterruptedScripts[i] + "\n";
      
      msg3 = msg3 + scriptNames3;
   }
   
   if (msg1 != "" || msg2 != "" || msg3 != "")
   {
      // Show errors to message window
      MessageInterface::ShowMessage(msg1 + msg2 + msg3);
      
      // Show errors to view text dialog
      ViewTextDialog *dlg =
         new ViewTextDialog(this, _T("Information"), false, wxDefaultPosition, wxSize(550, 300));
      wxTextCtrl *text = dlg->GetTextCtrl();
      wxString msg = msg1 + msg2 + msg3;
      text->AppendText(msg);
      dlg->ShowModal();
   }
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
   wxTreeItemId itemId = GetSelection();

   OnRemoveAllScripts(event);

   Delete(itemId);
   Collapse(mScriptItem);
}


//------------------------------------------------------------------------------
// bool BuildScript(const wxString &filename, Integer scriptOpenOpt,
//                  bool closeScript, bool readBack, const wxString &savePath,
//                  bool multScript)
//------------------------------------------------------------------------------
/**
 * Creates objects from script file.
 *
 * @param <filename> input script file name
 * @param <scriptOpenOpt> open script editor option after build script (0)
 *         0, script file to be opened on error only
 *         1, script file to be opened always
 *         2, NO script file to be opened
 * @param <closeScript> true will close opened script editor
 * @param <readBack> true will read scripts, save, and read back in (false)
 * @param <newPath> new path to be used for saving scripts ("")
 * @param <multiScripts> true if running scripts from the folder (false)
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool ResourceTree::BuildScript(const wxString &filename, Integer scriptOpenOpt,
                               bool closeScript, bool readBack,
                               const wxString &savePath, bool multiScripts)
{
   #ifdef DEBUG_RESOURCE_TREE
   MessageInterface::ShowMessage
      ("ResourceTree::BuildScript() filename=%s, scriptOpenOpt=%d, closeScript=%d, "
       "readBack=%d, multiScripts=%d\n   savePath=%s\n", filename.c_str(),
       scriptOpenOpt, closeScript, readBack, multiScripts, savePath.c_str());
   #endif

   // Set the filename to mainframe, so save will not bring up the file dialog
   bool fileSet =
      theMainFrame->SetScriptFileName(filename.c_str());

   if (fileSet)
   {
      // Interpret script
      bool status =
         theMainFrame->InterpretScript(filename, scriptOpenOpt, closeScript,
                                       readBack, savePath, multiScripts);

      if (!status)
      {
         mBuildErrorCount++;
         mFailedScriptsList.Add(filename);
      }

      #ifdef DEBUG_RESOURCE_TREE
      MessageInterface::ShowMessage
         ("ResourceTree::BuildScript() returning %s\n", (status ? "true" : "false"));
      #endif

      return status;
   }

   #ifdef DEBUG_RESOURCE_TREE
   MessageInterface::ShowMessage("ResourceTree::BuildScript() returning false\n");
   #endif

   return false;
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
   GmatTreeItemData *selItemData = (GmatTreeItemData *)GetItemData(itemId);
   wxString title = selItemData->GetName();
   GmatTree::ItemType itemType = selItemData->GetItemType();
   bool showRemoveDelete = true;

   // We don't want to show Remove and Delete menu if any non-plot panel is open
   if (theMainFrame->GetNumberOfChildOpen() > 0)
      showRemoveDelete = false;

   #ifdef DEBUG_SHOW_MENU
   MessageInterface::ShowMessage
      ("ResourceTree::ShowMenu() title=%s, itemType=%d\n", title.c_str(),
       itemType);
   #endif

#if wxUSE_MENUS
   wxMenu menu;

   switch (itemType)
   {
   case GmatTree::GROUND_STATION_FOLDER:
      menu.Append(POPUP_ADD_GROUND_STATION, wxT("Add Ground Station"));
      break;
   case GmatTree::SPACECRAFT_FOLDER:
      menu.Append(POPUP_ADD_SPACECRAFT, wxT("Add Spacecraft"));
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
//      #ifdef __USE_SPICE__
//      menu.Append(POPUP_ADD_SPK_PROPAGATOR, wxT("Add SPK Propagator"));
//      #endif
      break;
   case GmatTree::BOUNDARY_SOLVER_FOLDER:
      menu.Append(POPUP_ADD_BOUNDARY_SOLVER, wxT("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::OPTIMIZER_FOLDER:
      menu.Append(POPUP_ADD_OPTIMIZER, wxT("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::SOLAR_SYSTEM:
      //menu.Append(POPUP_ADD_BODY, wxT("Add Body"));
      //menu.Enable(POPUP_ADD_BODY, false);
      break;
   //case GmatTree::CELESTIAL_BODY:
   case GmatTree::CELESTIAL_BODY_STAR:
   case GmatTree::CELESTIAL_BODY_PLANET:
   case GmatTree::CELESTIAL_BODY_MOON:
   case GmatTree::CELESTIAL_BODY_COMET:
   case GmatTree::CELESTIAL_BODY_ASTEROID:
      menu.Append(POPUP_ADD_BODY, _T("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::SPECIAL_POINT_FOLDER:
      menu.Append(POPUP_ADD_SPECIAL_POINT, _T("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::SUBSCRIBER_FOLDER:
      menu.Append(POPUP_ADD_SUBSCRIBER, wxT("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::VARIABLE_FOLDER:
      menu.Append(POPUP_ADD_VARIABLE, wxT("Add"), CreatePopupMenu(itemType));
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
   case GmatTree::PLUGIN_FOLDER:
      {
         // Get type from itemId
         Gmat::ObjectType gmatType = nodeTypeMap[GetItemText(itemId).c_str()];
         std::string subtype = nodeSubtypeMap[GetItemText(itemId).c_str()];
         menu.Append(POPUP_ADD_SPECIAL_POINT, _T("Add"),
               CreatePopupMenu(itemType, gmatType, subtype));
      }
      break;
   default:
      break;
   }
   
   // Add MATLAB menu if it is installed
   if (GmatGlobal::Instance()->IsMatlabAvailable())
   {
      switch (itemType)
      {
         // Matlab needs GmatMainFrame interface, so use GmatMenu::
      case GmatTree::MATLAB_INTERFACE:
         menu.Append(GmatMenu::MENU_MATLAB_OPEN, wxT("Open"));
         menu.Append(GmatMenu::MENU_MATLAB_CLOSE, wxT("Close"));
         break;
      case GmatTree::MATLAB_SERVER:
         menu.Append(GmatMenu::MENU_MATLAB_SERVER_START, wxT("Start"));
         menu.Append(GmatMenu::MENU_MATLAB_SERVER_STOP, wxT("Stop"));
         if (mMatlabServerStarted)
         {
            menu.Enable(GmatMenu::MENU_MATLAB_SERVER_START, false);
            menu.Enable(GmatMenu::MENU_MATLAB_SERVER_STOP, true);
         }
         else
         {
            menu.Enable(GmatMenu::MENU_MATLAB_SERVER_START, true);
            menu.Enable(GmatMenu::MENU_MATLAB_SERVER_STOP, false);
         }
         break;
      default:
         break;
      }   
   }
   
   // menu items applies to most non-folder items
   if (itemType >= GmatTree::BEGIN_OF_RESOURCE &&
       itemType <= GmatTree::END_OF_RESOURCE)
   {
      switch (itemType)
      {
      case GmatTree::SOLAR_SYSTEM:
      case GmatTree::CELESTIAL_BODY:
      case GmatTree::CELESTIAL_BODY_STAR:
      case GmatTree::CELESTIAL_BODY_PLANET:
      case GmatTree::CELESTIAL_BODY_MOON:
      case GmatTree::CELESTIAL_BODY_COMET:
      case GmatTree::CELESTIAL_BODY_ASTEROID:
      case GmatTree::COORD_SYSTEM:
         menu.Append(POPUP_OPEN, wxT("Open"));
         menu.Append(POPUP_CLOSE, wxT("Close"));
         break;
      case GmatTree::SCRIPT_FILE:
         break; // nothing to add
      case GmatTree::FORMATION:
         menu.Append(POPUP_OPEN, wxT("Open"));
         menu.Append(POPUP_CLOSE, wxT("Close"));
         break;
      default:
         menu.Append(POPUP_OPEN, wxT("Open"));
         menu.Append(POPUP_CLOSE, wxT("Close"));
         if (showRemoveDelete)
         {
            menu.AppendSeparator();
            menu.Append(POPUP_RENAME, wxT("Rename"));
            menu.Append(POPUP_DELETE, wxT("Delete"));
         }
         menu.AppendSeparator();
         menu.Append(POPUP_CLONE, wxT("Clone"));
         break;
      }
   }

   PopupMenu(&menu, pt);
#endif // wxUSE_MENUS
}


//------------------------------------------------------------------------------
// wxMenu* CreatePopupMenu(GmatTree::ItemType itemType,
//       const Gmat::ObjectType gmatType, const std::string &subtype)
//------------------------------------------------------------------------------
/**
 * Create the popup menu
 *
 * @param itemType Type of object to create and add to tree
 * @param gmatType Gmat::ObjectType of object being created (used for plugin
 *                 objects)
 * @param subtype  Identifier for objects that are a subtype of a core type
 *                 (used for plugin objects)
 *
 * @return <wxMenu> Return popup menu
 */
//------------------------------------------------------------------------------
wxMenu* ResourceTree::CreatePopupMenu(GmatTree::ItemType itemType,
      const Gmat::ObjectType gmatType, const std::string &subtype)
{
   wxMenu *menu = new wxMenu;

   StringArray listOfObjects;
   Integer newId;

   switch (itemType)
   {
   case GmatTree::HARDWARE_FOLDER:
      menu->Append(POPUP_ADD_FUELTANK, wxT("Fuel Tank"));
      menu->Append(POPUP_ADD_THRUSTER, wxT("Thruster"));
      listOfObjects = theGuiInterpreter->GetCreatableList(Gmat::HARDWARE);
      newId = HARDWARE_BEGIN;
      for (StringArray::iterator i = listOfObjects.begin();
           i != listOfObjects.end(); ++i, ++newId)
      {
         // Drop the ones that are already there for now
         std::string hardwareType = (*i);
         if ((hardwareType != "FuelTank") &&
             (hardwareType != "Thruster"))
         {
            // Save the ID and type name for event handling
            pluginMap[POPUP_ADD_HARDWARE + newId] = hardwareType;
            menu->Append(POPUP_ADD_HARDWARE + newId, wxT(hardwareType.c_str()));
         }
      }
      break;
   case GmatTree::BURN_FOLDER:
      menu->Append(POPUP_ADD_IMPULSIVE_BURN, wxT("ImpulsiveBurn"));
      menu->Append(POPUP_ADD_FINITE_BURN, wxT("FiniteBurn"));
      break;
   case GmatTree::BOUNDARY_SOLVER_FOLDER:
      menu->Append(POPUP_ADD_DIFF_CORR, wxT("DifferentialCorrector"));
      //menu->Append(POPUP_ADD_BROYDEN, wxT("Broyden"));
      menu->Enable(POPUP_ADD_BROYDEN, false);
      break;
   case GmatTree::OPTIMIZER_FOLDER:
      //menu->Append(POPUP_ADD_QUASI_NEWTON, wxT("Quasi-Newton"));
      if (GmatGlobal::Instance()->IsMatlabAvailable())
         menu->Append(POPUP_ADD_SQP, wxT("SQP (fmincon)"));
      menu->Enable(POPUP_ADD_QUASI_NEWTON, false);
      
      listOfObjects = theGuiInterpreter->GetCreatableList(Gmat::SOLVER,
            "Optimizer");
      newId = SOLVER_BEGIN;
      for (StringArray::iterator i = listOfObjects.begin();
           i != listOfObjects.end(); ++i, ++newId)
      {
         // Drop the ones that are already there for now
         std::string solverType = (*i);
//         if ((solverType != "DifferentialCorrector") &&
//             (solverType != "FminconOptimizer") &&
//             (solverType != "Quasi-Newton"))
         if (solverType != "FminconOptimizer")
         {
            // Save the ID and type name for event handling
            pluginMap[POPUP_ADD_SOLVER + newId] = solverType;
            menu->Append(POPUP_ADD_SOLVER + newId, wxT(solverType.c_str()));
         }
      }
      break;
   case GmatTree::SUBSCRIBER_FOLDER:
      menu->Append(POPUP_ADD_REPORT_FILE, wxT("ReportFile"));
      menu->Append(POPUP_ADD_XY_PLOT, wxT("XYPlot"));
      menu->Append(POPUP_ADD_ORBIT_VIEW, wxT("OrbitView"));
      menu->Append(POPUP_ADD_EPHEMERIS_FILE, wxT("EphemerisFile"));
      
      //listOfObjects = theGuiInterpreter->GetCreatableList(Gmat::SUBSCRIBER);
      listOfObjects = theGuiInterpreter->GetListOfViewableSubtypesOf(Gmat::SUBSCRIBER);
      newId = SUBSCRIBER_BEGIN;
      for (StringArray::iterator i = listOfObjects.begin();
           i != listOfObjects.end(); ++i, ++newId)
      {
         // Drop the ones that are already there for now
         std::string subscriberType = (*i);
         if ((subscriberType != "ReportFile") &&
             (subscriberType != "XYPlot") &&
             (subscriberType != "OrbitView") &&
             (subscriberType != "EphemerisFile") &&
             (subscriberType != "TextEphemFile") &&
             (subscriberType != "MessageWindow") &&
             (subscriberType != "MatlabWS"))
         {
            // Save the ID and type name for event handling
            pluginMap[POPUP_ADD_SUBSCRIBER + newId] = subscriberType;
            menu->Append(POPUP_ADD_SUBSCRIBER + newId,
                     wxT(subscriberType.c_str()));
         }
      }
      
      break;
   case GmatTree::VARIABLE_FOLDER:
      menu->Append(POPUP_ADD_VARIABLE, wxT("Variable"));
      menu->Append(POPUP_ADD_ARRAY, wxT("Array"));
      menu->Append(POPUP_ADD_STRING, wxT("String"));
      break;
   case GmatTree::FUNCTION_FOLDER:
      {
         if (GmatGlobal::Instance()->IsMatlabAvailable())
            menu->Append(POPUP_ADD_MATLAB_FUNCTION, wxT("MATLAB Function"));
         
         wxMenu *gfMenu = new wxMenu;
         gfMenu->Append(POPUP_NEW_GMAT_FUNCTION, wxT("New"));
         gfMenu->Append(POPUP_OPEN_GMAT_FUNCTION, wxT("Open"));
         menu->Append(POPUP_ADD_GMAT_FUNCTION, wxT("GMAT Function"), gfMenu);
         break;
      }
   case GmatTree::CELESTIAL_BODY:
   case GmatTree::CELESTIAL_BODY_STAR:
      menu->Append(POPUP_ADD_PLANET, wxT("Planet"));
      menu->Append(POPUP_ADD_COMET, wxT("Comet"));
      menu->Append(POPUP_ADD_ASTEROID, wxT("Asteroid"));
      break;
   case GmatTree::CELESTIAL_BODY_PLANET:
      menu->Append(POPUP_ADD_MOON, wxT("Moon"));
      break;
   case GmatTree::CELESTIAL_BODY_MOON:
      menu->Append(POPUP_ADD_MOON, wxT("Moon"));  // huh?
      break;
   case GmatTree::CELESTIAL_BODY_COMET:
      menu->Append(POPUP_ADD_COMET, wxT("Comet"));  // huh?
      break;
   case GmatTree::CELESTIAL_BODY_ASTEROID:
      menu->Append(POPUP_ADD_ASTEROID, wxT("Asteroid"));
      break;
   case GmatTree::SPECIAL_POINT_FOLDER:
      menu->Append(POPUP_ADD_BARYCENTER, wxT("Barycenter"));
      menu->Append(POPUP_ADD_LIBRATION, wxT("Libration Point"));
      break;
   case GmatTree::PLUGIN_FOLDER:
      {
         listOfObjects = theGuiInterpreter->GetCreatableList(gmatType, subtype);

         if (listOfObjects.size() > 0)
         {
            newId = PLUGIN_BEGIN;
            for (StringArray::iterator i = listOfObjects.begin();
                 i != listOfObjects.end(); ++i, ++newId)
            {
               // Drop the ones that are already there for now
               std::string itemType = (*i);
               // Save the ID and type name for event handling
               pluginMap[POPUP_ADD_PLUGIN + newId] = itemType;
               menu->Append(POPUP_ADD_PLUGIN + newId, wxT(itemType.c_str()));
            }
         }
         else
         {
            pluginMap[POPUP_ADD_PLUGIN + PLUGIN_BEGIN] =
                  GmatBase::GetObjectTypeString(gmatType);
            menu->Append(POPUP_ADD_PLUGIN + PLUGIN_BEGIN,
                  wxT(GmatBase::GetObjectTypeString(gmatType).c_str()));
         }
      }
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
   case GmatTree::GROUND_STATION:
      objType = Gmat::GROUND_STATION;
      break;
   case GmatTree::SPACECRAFT:
      objType = Gmat::SPACECRAFT;
      break;
   case GmatTree::FORMATION:
      objType = Gmat::FORMATION;
      break;
   case GmatTree::IMPULSIVE_BURN:
      objType = Gmat::IMPULSIVE_BURN;
      break;
   case GmatTree::FINITE_BURN:
      objType = Gmat::FINITE_BURN;
      break;
   case GmatTree::PROPAGATOR:
//   case GmatTree::SPK_PROPAGATOR:
      objType = Gmat::PROP_SETUP;
      break;
   case GmatTree::DIFF_CORR:
   case GmatTree::SQP:
   case GmatTree::SOLVER:
      objType = Gmat::SOLVER;
      break;
   case GmatTree::REPORT_FILE:
   case GmatTree::XY_PLOT:
   case GmatTree::ORBIT_VIEW:
   case GmatTree::GROUND_TRACK_PLOT:      
   case GmatTree::EPHEMERIS_FILE:
      objType = Gmat::SUBSCRIBER;
      break;
   case GmatTree::VARIABLE:
   case GmatTree::ARRAY:
   case GmatTree::STRING:
      objType = Gmat::PARAMETER;
      break;
   case GmatTree::MATLAB_FUNCTION:
   case GmatTree::GMAT_FUNCTION:
      objType = Gmat::FUNCTION;
      break;
   case GmatTree::FUELTANK:
   case GmatTree::THRUSTER:
   case GmatTree::HARDWARE:
   case GmatTree::SENSOR:
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
//   case GmatTree::SPK_PROPAGATOR:
      return mPropagatorItem;
   case GmatTree::DIFF_CORR:
      return mBoundarySolverItem;
   case GmatTree::SQP:
   case GmatTree::SOLVER:
      return mOptimizerItem;
   case GmatTree::REPORT_FILE:
   case GmatTree::XY_PLOT:
   case GmatTree::ORBIT_VIEW:
   case GmatTree::GROUND_TRACK_PLOT:      
   case GmatTree::EPHEMERIS_FILE:
      return mSubscriberItem;
   case GmatTree::VARIABLE:
   case GmatTree::ARRAY:
   case GmatTree::STRING:
      return mVariableItem;
   case GmatTree::MATLAB_FUNCTION:
   case GmatTree::GMAT_FUNCTION:
      return mFunctionItem;
   case GmatTree::FUELTANK:
   case GmatTree::THRUSTER:
   case GmatTree::HARDWARE:
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
// GmatTree::ResourceIconType GetTreeItemIcon(GmatTree::ItemType itemType)
//------------------------------------------------------------------------------
GmatTree::ResourceIconType ResourceTree::GetTreeItemIcon(GmatTree::ItemType itemType)
{
   switch (itemType)
   {
   case GmatTree::GROUND_STATION:
      return GmatTree::ICON_GROUND_STATION;
   case GmatTree::SPACECRAFT:
      return GmatTree::ICON_SPACECRAFT;
   case GmatTree::IMPULSIVE_BURN:
      return GmatTree::ICON_IMPULSIVE_BURN;
   case GmatTree::FINITE_BURN:
      return GmatTree::ICON_FINITE_BURN;
   case GmatTree::PROPAGATOR:
//   case GmatTree::SPK_PROPAGATOR:
      return GmatTree::ICON_PROPAGATOR;
   case GmatTree::REPORT_FILE:
      return GmatTree::ICON_REPORT_FILE;
   case GmatTree::XY_PLOT:
      return GmatTree::ICON_XY_PLOT;
   case GmatTree::ORBIT_VIEW:
      return GmatTree::ICON_ORBIT_VIEW;
   case GmatTree::GROUND_TRACK_PLOT:
      return GmatTree::RESOURCE_ICON_GROUND_TRACK_PLOT;
   case GmatTree::EPHEMERIS_FILE:
      return GmatTree::ICON_REPORT_FILE;
   case GmatTree::VARIABLE:
      return GmatTree::ICON_VARIABLE;
   case GmatTree::ARRAY:
      return GmatTree::ICON_ARRAY;
   case GmatTree::STRING:
      return GmatTree::ICON_STRING;
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
   case GmatTree::SOLVER:
      return GmatTree::RESOURCE_ICON_DEFAULT;
   default:
      if (itemType >= GmatTree::RESOURCES_FOLDER &&
          itemType <= GmatTree::END_OF_RESOURCE_FOLDER)
         return GmatTree::ICON_FOLDER;
      else
         return GmatTree::RESOURCE_ICON_DEFAULT;
   }
}

//------------------------------------------------------------------------------
// void GetBodyTypeAndIcon(const std::string bodyName, ...)
//------------------------------------------------------------------------------
void ResourceTree::GetBodyTypeAndIcon(const std::string bodyName,
                                      GmatTree::ItemType &bodyItemType,
                                      GmatTree::ResourceIconType &iconType)
{
   SolarSystem   *ss           = theGuiInterpreter->GetSolarSystemInUse();
   CelestialBody *body         = ss->GetBody(bodyName);
   Gmat::BodyType typeOfBody   = body->GetBodyType();
   // These bodies have specific icons ...
   if (bodyName      == SolarSystem::MERCURY_NAME) iconType = GmatTree::ICON_MERCURY;
   else if (bodyName == SolarSystem::VENUS_NAME)   iconType = GmatTree::ICON_VENUS;
   else if (bodyName == SolarSystem::EARTH_NAME)   iconType = GmatTree::ICON_EARTH;
   else if (bodyName == SolarSystem::MARS_NAME)    iconType = GmatTree::ICON_MARS;
   else if (bodyName == SolarSystem::JUPITER_NAME) iconType = GmatTree::ICON_JUPITER;
   else if (bodyName == SolarSystem::SATURN_NAME)  iconType = GmatTree::ICON_SATURN;
   else if (bodyName == SolarSystem::URANUS_NAME)  iconType = GmatTree::ICON_URANUS;
   else if (bodyName == SolarSystem::NEPTUNE_NAME) iconType = GmatTree::ICON_NEPTUNE;
   else if (bodyName == SolarSystem::PLUTO_NAME)   iconType = GmatTree::ICON_PLUTO;
   else if (bodyName == SolarSystem::MOON_NAME)    iconType = GmatTree::ICON_MOON;
   else // these bodies don't have specific icons ...
   {
      if (typeOfBody   == Gmat::STAR)        iconType = GmatTree::ICON_SUN;  // should NOT happen
      else if (typeOfBody == Gmat::PLANET)   iconType = GmatTree::ICON_PLANET_GENERIC; // ICON_PLANET TBD
      else if (typeOfBody == Gmat::MOON)     iconType = GmatTree::ICON_MOON_GENERIC;  // ICON_OTHER_MOON TBD
      else if (typeOfBody == Gmat::COMET)    iconType = GmatTree::ICON_COMET;  // ICON_COMET TBD
      else if (typeOfBody == Gmat::ASTEROID) iconType = GmatTree::ICON_ASTEROID; // ICON_ASTEROID TBD
   }
   switch (typeOfBody)
   {
   case Gmat::STAR :
      bodyItemType = GmatTree::CELESTIAL_BODY_STAR;
      break;
   case Gmat::PLANET :
      bodyItemType = GmatTree::CELESTIAL_BODY_PLANET;
      break;
   case Gmat::MOON :
      bodyItemType = GmatTree::CELESTIAL_BODY_MOON;
      break;
   case Gmat::COMET :
      bodyItemType = GmatTree::CELESTIAL_BODY_COMET;
      break;
   case Gmat::ASTEROID :
      bodyItemType = GmatTree::CELESTIAL_BODY_ASTEROID;
      break;
   default:
      bodyItemType = GmatTree::CELESTIAL_BODY;
      break;
   }
}


//------------------------------------------------------------------------------
// int GetNameFromUser(wxString &newName, const wxString &oldName = "",
//                     const wxString &msg = "", const wxString &caption = "")
//------------------------------------------------------------------------------
/**
 * Gets user text input until valid name is entered or canceld
 *
 * @return  1  Valid name entered
 *          2  User canceled or no changes made
 */
//------------------------------------------------------------------------------
int ResourceTree::GetNameFromUser(wxString &newName, const wxString &oldName,
                                  const wxString &msg, const wxString &caption)
{
   newName = wxGetTextFromUser(wxT(msg), wxT(caption), oldName, this);
   
   // @note
   // There is no way of kwowing whether user entered blank and clicked OK
   // or just clicked Cancel, since wxGetTextFromUser() returns blank for
   // both cases. So if blank is returned, no change is assumed. (loj: 2008.08.29)
   if (newName == "")
      return 2;
   
   // Remove leading and trailing white spaces
   newName = newName.Strip(wxString::both);
   
   if (IsValidName(newName.c_str()))
   {
      // Now check for name already in use
      if (GetObject(newName.c_str()) != NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "\"%s\" already exist. Please enter different name.", newName.c_str());
         
         return GetNameFromUser(newName, newName, msg, caption);
      }
      else
      {
         return 1;
      }
   }
   else
   {
      return GetNameFromUser(newName, newName, msg, caption);
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
   #ifdef DEBUG_COMPARE_REPORT
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

         #ifdef DEBUG_COMPARE_REPORT
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

         #ifdef DEBUG_COMPARE_REPORT
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


// Code added to support plugins

//------------------------------------------------------------------------------
// void AddUserResources(std::vector<Gmat::PluginResource*> *rcs,
//       bool onlyChildNodes)
//------------------------------------------------------------------------------
/**
 * Updates the resource tree with new nodes provided by a plugin.
 *
 * @param rcs The raw data used to update the resource tree
 * @param onlyChildNodes Flag used to indicate that the top level nodes do not
 *                       need to be updated.
 */
//------------------------------------------------------------------------------
void ResourceTree::AddUserResources(std::vector<Gmat::PluginResource*> *rcs,
      bool onlyChildNodes)
{
   mPluginItems.clear();
   nodeTypeMap.clear();
   nodeSubtypeMap.clear();

   for (UnsignedInt i = 0; i < rcs->size(); ++i)
   {
      Gmat::PluginResource *r = (*rcs)[i];

      wxTreeItemId id;

      #ifdef DEBUG_USER_GUI
         MessageInterface::ShowMessage("Tree resource node:\n"
               "   Name: %s\n   Parent: %s\n   type: %d\n   subtype: %s\n",
               r->nodeName.c_str(), r->parentNodeName.c_str(), r->type,
               r->subtype.c_str());
      #endif

      if (onlyChildNodes && (r->parentNodeName == ""))
         id = FindIdOfNode(r->nodeName.c_str(), GetRootItem());
      else
         id = AddUserTreeNode(r->nodeName, r->parentNodeName);

      mPluginItems.push_back(id);
      nodeTypeMap[GetItemText(id).c_str()] = ((*rcs)[i])->type;
      nodeSubtypeMap[GetItemText(id).c_str()] = ((*rcs)[i])->subtype;

      #ifdef DEBUG_USER_GUI
         MessageInterface::ShowMessage("Node list:\n");
         for (UnsignedInt k = 0; k < mPluginItems.size(); ++k)
            MessageInterface::ShowMessage("   %d: %s %d %s\n", k,
                  GetItemText(mPluginItems[k]).c_str(),
                  nodeTypeMap[GetItemText(mPluginItems[k]).c_str()],
                  nodeSubtypeMap[GetItemText(mPluginItems[k]).c_str()].c_str());
      #endif
   }
}


//------------------------------------------------------------------------------
// void AddUserTreeNode(const std::string &newNodeName,
//       const std::string &parent)
//------------------------------------------------------------------------------
/**
 * Adds a folder node into the resource tree
 *
 * @param newNodeName The label for the new node
 * @param parent The parent node's label, or an empty string if the parent is
 *               the root node.
 */
//------------------------------------------------------------------------------
wxTreeItemId ResourceTree::AddUserTreeNode(const std::string &newNodeName,
      const std::string &parent)
{
   #ifdef DEBUG_USER_GUI
   MessageInterface::ShowMessage
      ("ResourceTree::AddUserTreeNode() entered, newNodeName='%s', parent='%s'\n",
       newNodeName.c_str(), parent.c_str());
   #endif
   wxTreeItemId parentId = GetRootItem();
   
   if (parent != "")
   {
      // Find the ID of the parent node
      parentId = FindIdOfNode(parent.c_str(), parentId);
   }
   
   wxTreeItemId newTreeItem;
   AddItemFolder(parentId, newTreeItem, newNodeName.c_str(),
                 GmatTree::PLUGIN_FOLDER);
   
   #ifdef DEBUG_USER_GUI
   MessageInterface::ShowMessage
      ("ResourceTree::AddUserTreeNode() returning newTreeItem\n");
   #endif
   
   return newTreeItem;
}


//------------------------------------------------------------------------------
// wxTreeItemId FindIdOfNode(const wxString &txt, wxTreeItemId start)
//------------------------------------------------------------------------------
/**
 * Finds the tree item ID of a node given the node's text string.
 *
 * This method call is set to be recursive, but the recursion is not yet hooked
 * up.  Currently, only the children of the start node are searched, but not
 * grandchildren and below.
 *
 * @param txt The node label that is being sought
 * @param The id of the initial node
 *
 * @return The id of the found node, to the start node if a child was not found
 *         with the text label.
 */
//------------------------------------------------------------------------------
wxTreeItemId ResourceTree::FindIdOfNode(const wxString &txt, wxTreeItemId start)
{
   wxTreeItemId retval = start;
   bool located = false;

   // Traverse the tree, looking for a node with the input name
   if (GetItemText(start) != txt)
   {
      if (GetChildrenCount(start, false) > 0)
      {
         // Check the node's children
         wxTreeItemIdValue cookie;
         wxTreeItemId cf = GetFirstChild(start, cookie);
         for(UnsignedInt i = 0; i < GetChildrenCount(start, false); ++i)
         {
            if (GetItemText(cf) == txt)
            {
               retval = cf;
               located = true;
               break;
            }
            cf = GetNextChild(start, cookie);
         }
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// void MakeScriptActive(const wxString &scriptName, bool active)
//------------------------------------------------------------------------------
/**
 * Makes script active or inactive by changing script text bold face or normal
 *
 * @param  scriptName  Item name of script
 * @param  active      Set this to true if script is active, false otherwise
 */
//------------------------------------------------------------------------------
void ResourceTree::MakeScriptActive(const wxString &scriptName, bool active)
{
   #ifdef DEBUG_ACTIVE_SCRIPT
   MessageInterface::ShowMessage
      ("ResourceTree::MakeScriptActive() entered, scriptName='%s', active=%d\n",
       scriptName.c_str(), active);
   #endif
   
   bool scriptFound = false;
   MakeChildScriptActive(mScriptItem, scriptName, active, scriptFound);
   
   #ifdef DEBUG_ACTIVE_SCRIPT
   MessageInterface::ShowMessage
      ("ResourceTree::MakeScriptActive() leaving, scriptFound=%d\n", scriptFound);
   #endif
}


//------------------------------------------------------------------------------
// void MakeChildScriptActive(const wxTreeItemId &scriptItem,
//                           const wxString &scriptName, bool active,
//                           bool &scriptFound)
//------------------------------------------------------------------------------
/**
 * Makes child script active or inactive by changing script text bold face or normal
 *
 * @param  scriptItem  Id of script node
 * @param  scriptName  Name of script to find
 * @param  scriptFound Output flag indicating script found
 * @return  wxTreeItemId of last script found
 */
//------------------------------------------------------------------------------
void ResourceTree::MakeChildScriptActive(const wxTreeItemId &scriptItem,
                                         const wxString &scriptName, bool active,
                                         bool &scriptFound)
{
   #ifdef DEBUG_ACTIVE_SCRIPT
   MessageInterface::ShowMessage
      ("ResourceTree::MakeChildScriptActive() entered, scriptItem='%s', "
       "scriptName='%s', active=%d\n", GetItemText(scriptItem).c_str(),
       scriptName.c_str(), active);
   #endif
   
   scriptFound = false;
   wxTreeItemIdValue cookie;
   wxTreeItemId childId = GetFirstChild(scriptItem, cookie);
   wxTreeItemId scriptId = childId;
   wxString scriptPath, nodeText;
   
   #ifdef DEBUG_ACTIVE_SCRIPT
   MessageInterface::ShowMessage
      ("   numChildren=%d, firstChildId is %s\n",
       GetChildrenCount(scriptItem), childId.IsOk() ? "OK" : "not OK");
   #endif
   
   wxTreeItemId grandChildId;
   while (childId.IsOk())
   {
      if (ItemHasChildren(childId))
      {
         #ifdef DEBUG_ACTIVE_SCRIPT
         MessageInterface::ShowMessage
            ("   item '%s' has %d children\n", GetItemText(childId).c_str(),
             GetChildrenCount(childId));
         #endif
         MakeChildScriptActive(childId, scriptName, active, scriptFound);
      }
      
      scriptId = childId;
      
      // find child with same path
      nodeText = GetItemText(scriptId);
      
      #ifdef DEBUG_ACTIVE_SCRIPT
      MessageInterface::ShowMessage("   nodeText=<%s>\n", nodeText.c_str());
      #endif
      
      scriptPath = ((GmatTreeItemData *)GetItemData(scriptId))->GetName();
      
      #ifdef DEBUG_ACTIVE_SCRIPT
      MessageInterface::ShowMessage("   scriptPath=<%s>\n", scriptPath.c_str());
      #endif
      
      if (GmatFileUtil::IsSameFileName(nodeText.c_str(), scriptName.c_str()))
      {
         scriptFound = true;
         #ifdef DEBUG_ACTIVE_SCRIPT
         MessageInterface::ShowMessage
            ("   Setting %d %s\n", scriptId, active ? "ACTIVE" : "INACTIVE");
         #endif
         SetItemBold(scriptId, active);
      }
      
      childId = GetNextChild(scriptItem, cookie);
   }
   
   #ifdef DEBUG_ACTIVE_SCRIPT
   MessageInterface::ShowMessage("ResourceTree::MakeChildScriptActive() leaving\n");
   #endif
   
}

