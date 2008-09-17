//$Header$
//------------------------------------------------------------------------------
//                             GmatTreeItemData
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
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
   enum IconType
   {
      ICON_FOLDER,
      ICON_FILE,
      ICON_OPENFOLDER,
      ICON_SPACECRAFT,
      ICON_TANK,
      ICON_THRUSTER,
      
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
      
      ICON_REPORT,
      ICON_NETWORK,
      ICON_BURN,
      ICON_MOON,
      
      ICON_MATLAB_FUNCTION,
      ICON_FUNCTION,
      ICON_COORDINATE_SYSTEM,
      ICON_OPEN_GL_PLOT,
      ICON_PROPAGATOR,
      ICON_VARIABLE,
      ICON_ARRAY,
      ICON_STRING,
      ICON_XY_PLOT,
      
      ICON_DEFAULT,
   };

   enum MissionIconType
   {
      MISSION_ICON_PROPAGATE,
      MISSION_ICON_TARGET,

      MISSION_ICON_FOLDER,
      MISSION_ICON_FILE,
      MISSION_ICON_OPENFOLDER,

      MISSION_ICON_SPACECRAFT,
      MISSION_ICON_WHILE,
      MISSION_ICON_FOR,
      MISSION_ICON_IF,
      MISSION_ICON_SCRIPTEVENT,
      MISSION_ICON_VARY,
      MISSION_ICON_ACHIEVE,
      MISSION_ICON_DELTA_V,
      MISSION_ICON_FUNCTION,
      MISSION_ICON_NEST_RETURN,
      MISSION_ICON_SAVE,
      MISSION_ICON_ASSIGNMENT,
      MISSION_ICON_TOGGLE,
      MISSION_ICON_BEGIN_FB,
      MISSION_ICON_END_FB,
      MISSION_ICON_REPORT,
      MISSION_ICON_STOP,

      MISSION_NO_ICON,
   };

   //----------------------------------------------------------------------
   // Note: Do not change the order of ItemType. The wrong order of dataType
   // will not work propery in GmatMainFrame::CreateChild()
   //----------------------------------------------------------------------
   enum ItemType
   {
      UNKNOWN_ITEM = -1,
      
      //---------- Resource Tree
      RESOURCES_FOLDER = 10000,
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
      END_OF_RESOURCE_FOLDER,
      
      // not openable
      INTERFACE,
      MATLAB_INTERFACE,
      MATLAB_SERVER,
      
      VIEW_SOLVER_VARIABLES,
      VIEW_SOLVER_GOALS,
      
      // openable resource
      BEGIN_OF_RESOURCE = 11000,
      SPACECRAFT,
      FUELTANK,
      THRUSTER,
      
      FORMATION,
      FORMATION_SPACECRAFT,
      CONSTELLATION_SATELLITE,
      
      PROPAGATOR,
      
      IMPULSIVE_BURN,
      FINITE_BURN,
      
      UNIVERSE_FOLDER,
      CELESTIAL_BODY,
      BARYCENTER,
      LIBRATION_POINT,
      
      SOLVER,
      DIFF_CORR,
      BROYDEN,
      QUASI_NEWTON,
      SQP,
      
      REPORT_FILE,
      XY_PLOT,
      OPENGL_PLOT,
      
      VARIABLE,
      ARRAY,
      STRING,
      MATLAB_FUNCTION,
      GMAT_FUNCTION,
      COORD_SYSTEM,
      USER_COORD_SYSTEM,
      GROUNDSTATION,
      
      // scripts
      SCRIPT_FILE,
      END_OF_RESOURCE,
      
      //------------------------------------------
      // Note: MissionTree uses EVT_MENU_RANGE
      //------------------------------------------
      //---------- Mission Tree
      MISSIONS_FOLDER = 20000,
      MISSION_SEQ_TOP_FOLDER,
      MISSION_SEQ_SUB_FOLDER,
      MISSION_SEQ_COMMAND,
      
      // command
      BEGIN_OF_COMMAND = 21000,
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
      REPORT,
      CALL_FUNCTION,
      ASSIGNMENT,
      FREE_FORM_SCRIPT,
      SCRIPT_EVENT,
      END_OF_COMMAND,
      
      // control logic
      BEGIN_OF_CONTROL,
      IF_CONTROL,
      ELSE_IF_CONTROL,
      FOR_CONTROL,
      WHILE_CONTROL,
      DO_CONTROL,
      SWITCH_CONTROL,
      END_OF_CONTROL,
      
      //---------- Output Tree
      OUTPUT_FOLDER = 30000,
      REPORTS_FOLDER,
      OPENGL_PLOTS_FOLDER,
      XY_PLOTS_FOLDER,
      
      // for output
      BEGIN_OF_OUTPUT = 31000,
      OUTPUT_REPORT,
      OUTPUT_OPENGL_PLOT,
      OUTPUT_XY_PLOT,
      COMPARE_REPORT,
      END_OF_OUTPUT,
      
      //---------- NO panels will be created
      BEGIN_NO_PANEL = 40000,
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
      END_NO_PANEL,
   };
}


class GmatTreeItemData : public wxTreeItemData
{
public:
   GmatTreeItemData(const wxString desc, GmatTree::ItemType type);
   
   GmatTree::ItemType GetItemType() { return mItemType; }
   wxString GetDesc() { return mDesc; }
   
   void SetDesc(wxString desc) { mDesc = desc; }
   void SetItemType(GmatTree::ItemType type) { mItemType = type; }
   
   virtual GmatCommand* GetCommand();
   virtual wxString GetCommandName();
    
protected:
private:
   wxString mDesc;
   GmatTree::ItemType mItemType;
};

#endif // GmatTreeItemData_hpp
