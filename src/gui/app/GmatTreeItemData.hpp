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

#include "Command.hpp"

class GmatTreeItemData : public wxTreeItemData
{
public:
   GmatTreeItemData(const wxString desc, const int dType);
   
   int GetDataType();
   wxString GetDesc();
   
   void SetDesc(wxString description);
   
   virtual GmatCommand* GetCommand();
   virtual wxString GetCommandName();
    
protected:
private:
   wxString m_desc;
   int dataType;
};

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
      ICON_ARRAY,
      ICON_COORDINATE_SYSTEM,
      ICON_OPEN_GL_PLOT,
      ICON_PROPAGATOR,
      ICON_VARIABLE,
      ICON_XY_PLOT,

      ICON_DEFAULT,
   };

   enum MissionIconType
   {
      MISSION_ICON_PROPAGATE_EVENT,
      MISSION_ICON_TARGET,

      MISSION_ICON_FOLDER,
      MISSION_ICON_FILE,
      MISSION_ICON_OPENFOLDER,

      MISSION_ICON_SPACECRAFT,
      MISSION_ICON_WHILE,
      MISSION_ICON_VARY,
      MISSION_ICON_ACHIEVE,
      MISSION_ICON_DELTA_V,
      MISSION_ICON_FUNCTION,
      MISSION_ICON_NEST_RETURN,

      MISSION_NO_ICON,
   };

   //----------------------------------------------------------------------
   // Note: Do not change the order of ItemType. The wrong order of dataType
   // will not work propery in GmatMainFrame::CreateChild()
   //----------------------------------------------------------------------
   enum ItemType
   {
      // Resource Tree
      RESOURCES_FOLDER = 20000,
      SPACECRAFT_FOLDER,
      HARDWARE_FOLDER,
      FORMATIONS_FOLDER,
      CONSTELLATIONS_FOLDER,
      BURNS_FOLDER,
      PROPAGATORS_FOLDER,
      UNIVERSE_FOLDER,
      SOLVERS_FOLDER,
      BOUNDARY_SOLVERS_FOLDER,
      OPTIMIZERS_FOLDER,
      SUBSCRIBERS_FOLDER,
      INTERFACES_FOLDER,
      SUBSCRIPTS_FOLDER,
      VARIABLES_FOLDER,
      GROUNDSTATIONS_FOLDER,
      FUNCT_FOLDER,
      COORD_SYS_FOLDER,
      SPECIAL_POINTS_FOLDER,
      PREDEFINED_FUNCTIONS_FOLDER,
      SCRIPTS_FOLDER,
      SAMPLE_SCRIPTS_FOLDER,

      // resource
      SPACECRAFT,

      FUELTANK,
      THRUSTER,

      FORMATION_FOLDER,
      FORMATION_SPACECRAFT,

      CONSTELLATION_FOLDER,
      CONSTELLATION_SATELLITE,

      PROPAGATOR,
      IMPULSIVE_BURN,
      FINITE_BURN,
      CELESTIAL_BODY,
        
      DIFF_CORR,
      BROYDEN,
      QUASI_NEWTON,
      SQP,
      
      REPORT_FILE,
      XY_PLOT,
      OPENGL_PLOT,
      INTERFACE,
      SUBSCRIPT,
      VARIABLE,
      MATLAB_FUNCTION,
      GMAT_FUNCTION,
      COORD_SYSTEM,
      USER_COORD_SYSTEM,
      GROUNDSTATION,
      SCRIPT_FILE,
      SCRIPT_FOLDER,
      BARYCENTER,
      LIBRATION_POINT,
      
      //Mission Tree
      MISSIONS_FOLDER,
      MISSION_SEQ_TOP_FOLDER,
      MISSION_SEQ_SUB_FOLDER,
      MISSION_SEQ_COMMAND,

      // command
      PROPAGATE_COMMAND,
      MANEUVER_COMMAND,
      TARGET_COMMAND,
      END_TARGET_COMMAND,
      ACHIEVE_COMMAND,
      VARY_COMMAND,
      SAVE_COMMAND,
      TOGGLE_COMMAND,
      CALL_FUNCTION_COMMAND,
      ASSIGNMENT_COMMAND,
      FREE_FORM_SCRIPT_COMMAND, //arg: 1/12/05 added FREE_FORM_SCRIPT
      SCRIPT_COMMAND,
      
      // Temporary to show the panel
      VIEW_SOLVER_VARIABLES,
      VIEW_SOLVER_GOALS, 
      
      // Instead of Variable_Folder
      VARIABLES,

      // control logic
      IF_CONTROL,
      ELSE_IF_CONTROL,
      ELSE_CONTROL,
      END_IF_CONTROL,
      FOR_CONTROL,
      END_FOR_CONTROL,
      WHILE_CONTROL,
      END_WHILE_CONTROL,
      DO_CONTROL,
      END_DO_CONTROL,
      SWITCH_CONTROL,
      END_SWITCH_CONTROL,
      
      // for the output tree
      OUTPUT_FOLDER,
      REPORTS_FOLDER,
      OPENGL_PLOTS_FOLDER,
      XY_PLOTS_FOLDER,

      // for output
      OUTPUT_REPORT,
      OUTPUT_OPENGL_PLOT,
      OUTPUT_XY_PLOT,
      COMPARE_REPORT,
   };
}
#endif // GmatTreeItemData_hpp
