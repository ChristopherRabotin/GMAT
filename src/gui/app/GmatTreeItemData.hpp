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

class GmatTreeItemData : public wxTreeItemData
{
public:
//    GmatTreeItemData(const wxString& desc) : m_desc(desc) { }
    GmatTreeItemData(const wxString desc,
                     const int dType);

    wxString GetDesc();
    int GetDataType();
    void SetDesc(wxString description);
    
protected:
private:
    wxString m_desc;
    int dataType;
};

   enum
   {
      // Resource Tree
      RESOURCES_FOLDER,
      SPACECRAFT_FOLDER,
      FORMATIONS_FOLDER,
      CONSTELLATIONS_FOLDER,
      PROPAGATORS_FOLDER,
      UNIVERSE_FOLDER,
      SOLVERS_FOLDER,
      PLOTS_FOLDER,
      INTERFACES_FOLDER,
      REPORTS_FOLDER,
      SUBSCRIPTS_FOLDER,
      VARIABLES_FOLDER,
      GROUNDSTATIONS_FOLDER,

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

      DEFAULT_BODY,
      CREATED_BODY,

      DEFAULT_SOLVER,
      CREATED_SOLVER,

      DEFAULT_PLOT,
      CREATED_PLOT,

      DEFAULT_INTERFACE,
      CREATED_INTERFACE,

      DEFAULT_REPORT,
      CREATED_REPORT,

      DEFAULT_SUBSCRIPT,
      CREATED_SUBSCRIPT,

      DEFAULT_VARIABLE,
      CREATED_VARIABLE,

      DEFAULT_GROUNDSTATION,
      CREATED_GROUNDSTATION,

      //Mission Tree
      MISSIONS_FOLDER,
      DEFAULT_PROPAGATE_COMMAND,
      MANEUVER_COMMAND,
      PROPAGATE_COMMAND,
      TARGET_COMMAND,
  };


#endif // GmatTreeItemData_hpp
