//$Id$
//------------------------------------------------------------------------------
//                             GmatTreeItemData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// ** Legal **
//
// Author: Allison Greene
// Created: 2003/10/08
/**
 * This class provides a data type for tree nodes.
 */
//------------------------------------------------------------------------------
#ifndef GmatTreeItemData_hpp
#define GmatTreeItemData_hpp

#include "gmatwxdefs.hpp"
#include "wx/treectrl.h"

#include "GmatCommand.hpp"

namespace GmatTree
{
   // ICON_FOLDER should start from 0 since ResourceTree::AddIcons()
   // creates icons by the order in ResourceIconType
   enum ResourceIconType
   {
      ICON_FOLDER,
      ICON_FILE,
      ICON_OPENFOLDER,
      ICON_GROUND_STATION,
      ICON_SPACECRAFT,

      ICON_TANK,
      ICON_THRUSTER,
      ICON_SOLARSYSTEM,
      ICON_SUN,
      ICON_MERCURY,
      ICON_VENUS,

      ICON_EARTH,
      ICON_MARS,
      ICON_JUPITER,
      ICON_SATURN,
      ICON_URANUS,

      ICON_NEPTUNE,
      ICON_PLUTO,
      ICON_REPORT_FILE,
      ICON_NETWORK,
      ICON_BURN,

      ICON_MOON,
      ICON_MOON_GENERIC,
      ICON_PLANET_GENERIC,
      ICON_COMET,
      ICON_ASTEROID,
      
      ICON_MATLAB_FUNCTION,
      ICON_FUNCTION,
      ICON_COORDINATE_SYSTEM,
      ICON_ORBIT_VIEW,

      ICON_PROPAGATOR,
      ICON_VARIABLE,
      ICON_ARRAY,
      ICON_STRING,
      ICON_XY_PLOT,

      RESOURCE_ICON_BARYCENTER,
      RESOURCE_ICON_LIBRATION_POINT,
      
      RESOURCE_ICON_BOUNDARY_VALUE_SOLVER,
      RESOURCE_ICON_OPTIMIZER,
      RESOURCE_ICON_SIMULATOR,
      RESOURCE_ICON_ESTIMATOR,
      
      RESOURCE_ICON_ANTENNA,
      RESOURCE_ICON_TRANSMITTER,
      RESOURCE_ICON_RECEIVER,
      RESOURCE_ICON_TRANSPONDER,
      
      RESOURCE_ICON_MATLAB,
      RESOURCE_ICON_MATLAB_SERVER,
      
      RESOURCE_ICON_MEASUREMENT_MODEL,
      RESOURCE_ICON_SCRIPT,
      RESOURCE_ICON_DEFAULT,
      
      // ICON_COUNT should be the last one, it is used in ResourceTree::AddIcons()
      ICON_COUNT,
   };

   // MISSION_ICON_PROPAGATE should start from 0 since MissionTree::AddIcons()
   // creates icons by the order in MissionIconType
   enum MissionIconType
   {
      MISSION_ICON_PROPAGATE,
      MISSION_ICON_TARGET,
      MISSION_ICON_FOLDER,
      MISSION_ICON_FILE,
      MISSION_ICON_OPENFOLDER,

      MISSION_ICON_WHILE,
      MISSION_ICON_FOR,
      MISSION_ICON_IF,
      MISSION_ICON_SCRIPTEVENT,
      MISSION_ICON_VARY,

      MISSION_ICON_ACHIEVE,
      MISSION_ICON_DELTA_V,
      MISSION_ICON_CALL_FUNCTION,
      MISSION_ICON_NEST_RETURN,
      MISSION_ICON_SAVE,

      MISSION_ICON_ASSIGNMENT,
      MISSION_ICON_TOGGLE,
      MISSION_ICON_BEGIN_FB,
      MISSION_ICON_END_FB,
      MISSION_ICON_REPORT,

      MISSION_ICON_STOP,
      MISSION_ICON_PEN_UP,
      MISSION_ICON_PEN_DOWN,
      MISSION_ICON_MARK_POINT,
      MISSION_ICON_CLEAR_PLOT,
      
      MISSION_ICON_GLOBAL,
      MISSION_ICON_SAVE_MISSION,
      MISSION_ICON_OPTIMIZE,
      MISSION_ICON_MINIMIZE,
      MISSION_ICON_NONLINEAR_CONSTRAINT,
      
      MISSION_ICON_RUN_SIMULATOR,
      MISSION_ICON_RUN_ESTIMATOR,
      MISSION_ICON_DEFAULT,
      
      MISSION_ICON_COUNT,
      MISSION_NO_ICON,
   };
   
   // OutputTree icons used in OutputTree::AddIcons()
   enum OutputIconType
   {
      OUTPUT_ICON_CLOSEDFOLDER,
      OUTPUT_ICON_OPENFOLDER,
      OUTPUT_ICON_REPORTFILE,
      OUTPUT_ICON_ORBITVIEW,
      OUTPUT_ICON_XYPLOT,
      OUTPUT_ICON_FILE,
      OUTPUT_ICON_DEFAULT,
      
      // OUTPUT_ICON_COUNT should be the last one
      OUTPUT_ICON_COUNT
   };
   
   //----------------------------------------------------------------------
   // Note: Do not change the order of ItemType. The wrong order of dataType
   // will not work propery in GmatMainFrame::CreateChild()
   //----------------------------------------------------------------------
   enum ItemType
   {
      UNKNOWN_ITEM = -1,

      //---------- Resource Tree
      RESOURCES_FOLDER = 40000,
      GROUND_STATION_FOLDER,
      SPACECRAFT_FOLDER,
      HARDWARE_FOLDER,
      FORMATION_FOLDER,
      BURN_FOLDER,
      PROPAGATOR_FOLDER,
      SOLVER_FOLDER,
      BOUNDARY_SOLVER_FOLDER,
      OPTIMIZER_FOLDER,
      SUBSCRIBER_FOLDER,
      INTERFACE_FOLDER,
      VARIABLE_FOLDER,
      FUNCTION_FOLDER,
      COORD_SYSTEM_FOLDER,
      SPECIAL_POINT_FOLDER,
      PREDEFINED_FUNCTION_FOLDER,
      SCRIPT_FOLDER,
      CONSTELLATION_FOLDER,
      // Added for user modules
      PLUGIN_FOLDER,
      END_OF_RESOURCE_FOLDER,

      // not openable
      INTERFACE,
      MATLAB_INTERFACE,
      MATLAB_SERVER,

      VIEW_SOLVER_VARIABLES,
      VIEW_SOLVER_GOALS,

      // openable resource
      BEGIN_OF_RESOURCE = 41000,
      GROUND_STATION,
      SPACECRAFT,
      
      FUELTANK,
      THRUSTER,
      SENSOR,
      HARDWARE,
      
      FORMATION,
      FORMATION_SPACECRAFT,
      CONSTELLATION_SATELLITE,

      PROPAGATOR = 41100,
//      SPK_PROPAGATOR,

      IMPULSIVE_BURN = 41200,
      FINITE_BURN,

      SOLAR_SYSTEM = 41300,
      CELESTIAL_BODY,
      CELESTIAL_BODY_STAR,    // need to differentiate these because they will have different
      CELESTIAL_BODY_PLANET,  // submenus for adding orbiting body(ies)
      CELESTIAL_BODY_MOON,    //
      CELESTIAL_BODY_COMET,   // wcs 2009.01.09
      CELESTIAL_BODY_ASTEROID,//
      BARYCENTER,
      LIBRATION_POINT,

      SOLVER = 41400,
      SOLVER,
      DIFF_CORR,
      BROYDEN,
      QUASI_NEWTON,
      SQP,

      SUBSCRIBER = 41500,
      REPORT_FILE,
      XY_PLOT,
      ORBIT_VIEW,
      EPHEMERIS_FILE,
      
      VARIABLE = 41600,
      ARRAY,
      STRING,

      MATLAB_FUNCTION = 41700,
      GMAT_FUNCTION,
      COORD_SYSTEM,
      USER_COORD_SYSTEM,

      USER_DEFINED_OBJECT = 41750,

      // scripts
      SCRIPT_FILE = 41800,
      END_OF_RESOURCE,

      //------------------------------------------
      // Note: MissionTree uses EVT_MENU_RANGE
      //------------------------------------------
      //---------- Mission Tree
      MISSIONS_FOLDER = 42000,
      MISSION_SEQ_TOP_FOLDER,
      MISSION_SEQ_SUB_FOLDER,
      MISSION_SEQ_COMMAND,

      // command
      BEGIN_OF_COMMAND = 43000,
      PROPAGATE,
      MANEUVER,
      BEGIN_FINITE_BURN,
      END_FINITE_BURN,
      TARGET,
      OPTIMIZE,
      ACHIEVE,
      VARY,
      OPTIMIZE_VARY,
      MINIMIZE,
      NON_LINEAR_CONSTRAINT,
      SAVE,
      TOGGLE,
      XY_PLOT_ACTION,
      REPORT,
      CALL_FUNCTION,
      ASSIGNMENT,
      FREE_FORM_SCRIPT,
      SCRIPT_EVENT,
      OTHER_COMMAND,
      END_OF_COMMAND,

      // control logic
      BEGIN_OF_CONTROL = 44000,
      IF_CONTROL,
      ELSE_IF_CONTROL,
      FOR_CONTROL,
      WHILE_CONTROL,
      DO_CONTROL,
      SWITCH_CONTROL,
      END_OF_CONTROL,

      //---------- Output Tree
      OUTPUT_FOLDER = 45000,
      REPORTS_FOLDER,
      ORBIT_VIEWS_FOLDER,
      XY_PLOTS_FOLDER,

      // for output
      BEGIN_OF_OUTPUT = 46000,
      OUTPUT_REPORT,
      OUTPUT_ORBIT_VIEW,
      OUTPUT_XY_PLOT,
      COMPARE_REPORT,
      END_OF_OUTPUT,

      //---------- NO panels will be created
      BEGIN_NO_PANEL = 47000,
      BEGIN_MISSION_SEQUENCE,
      STOP,
      ADDED_SCRIPT_FOLDER,
      END_TARGET,
      END_OPTIMIZE,
      ELSE_CONTROL,
      END_IF_CONTROL,
      END_FOR_CONTROL,
      END_DO_CONTROL,
      END_WHILE_CONTROL,
      END_SWITCH_CONTROL,
      END_SCRIPT_EVENT,
      END_NO_PANEL,
   };
}


class GmatTreeItemData : public wxTreeItemData
{
public:
   GmatTreeItemData(const wxString &name, GmatTree::ItemType type,
                    const wxString &title = "");

   wxString GetName();
   wxString GetTitle();
   GmatTree::ItemType GetItemType();

   void SetName(const wxString &objName);
   void SetTitle(const wxString &title);
   void SetItemType(GmatTree::ItemType type);

   virtual GmatCommand* GetCommand();

protected:
   wxString mItemTitle;
   wxString mItemName;
   GmatTree::ItemType mItemType;
};

#endif // GmatTreeItemData_hpp
