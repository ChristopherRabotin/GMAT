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
//    GmatTreeItemData(const wxString& desc) : m_desc(desc) { }
    GmatTreeItemData(const wxString desc,
                     const int dType);

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
    };
    
    enum ItemType
    {
        // Resource Tree
        RESOURCES_FOLDER = 20000,
        SPACECRAFT_FOLDER,
        FORMATIONS_FOLDER,
        CONSTELLATIONS_FOLDER,
        BURNS_FOLDER,
        PROPAGATORS_FOLDER,
        UNIVERSE_FOLDER,
        SOLVERS_FOLDER,
        SUBSCRIBERS_FOLDER,
        INTERFACES_FOLDER,
        SUBSCRIPTS_FOLDER,
        VARIABLES_FOLDER,
        GROUNDSTATIONS_FOLDER,
        MATLAB_FUNCT_FOLDER,

        DEFAULT_SPACECRAFT,
        CREATED_SPACECRAFT,

        DEFAULT_FORMATION_FOLDER,
        DEFAULT_FORMATION_SPACECRAFT,
        CREATED_FORMATION_FOLDER,
        CREATED_FORMATION_SPACECRAFT,

        DEFAULT_CONSTELLATION_FOLDER,
        DEFAULT_CONSTELLATION_SATELLITE,
        CREATED_CONSTELLATION_FOLDER,
        CREATED_CONSTELLATION_SATELLITE,

        DEFAULT_PROPAGATOR,
        CREATED_PROPAGATOR,

        DEFAULT_IMPULSIVE_BURN,
        CREATED_IMPULSIVE_BURN,

        DEFAULT_BODY,
        CREATED_BODY,

        DEFAULT_DIFF_CORR,
        CREATED_DIFF_CORR,

        DEFAULT_REPORT_FILE,
        CREATED_REPORT_FILE,
      
        DEFAULT_XY_PLOT,
        CREATED_XY_PLOT,
      
        DEFAULT_OPENGL_PLOT,
        CREATED_OPENGL_PLOT,
      
        DEFAULT_INTERFACE,
        CREATED_INTERFACE,

        DEFAULT_SUBSCRIPT,
        CREATED_SUBSCRIPT,

        DEFAULT_VARIABLE,
        CREATED_VARIABLE,

        DEFAULT_GROUNDSTATION,
        CREATED_GROUNDSTATION,

        //Mission Tree
        MISSIONS_FOLDER,
        MISSION_SEQ_TOP_FOLDER,
        MISSION_SEQ_SUB_FOLDER,

        MISSION_SEQ_COMMAND,
        DEFAULT_PROPAGATE_COMMAND,
        MANEUVER_COMMAND,
        PROPAGATE_COMMAND,
        TARGET_COMMAND,

        // Temporary to show the panel
        VIEW_SOLVER_VARIABLES,
        VIEW_SOLVER_GOALS, 

        // Instead of Variable_Folder
        VARIABLES,
        
        IF_CONTROL,
        WHILE_CONTROL,
        FOR_CONTROL,
        DO_CONTROL,
        SWITCH_CONTROL,
        END_IF_CONTROL,
        ELSE_IF_CONTROL,
        ELSE_CONTROL,
    };
}
#endif // GmatTreeItemData_hpp
