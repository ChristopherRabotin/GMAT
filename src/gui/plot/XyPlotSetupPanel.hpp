//$Header$
//------------------------------------------------------------------------------
//                              XyPlotSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/02/02
/**
 * Declares XyPlotSetupPanel class. This class allows user to setup XY Plot.
 */
//------------------------------------------------------------------------------
#ifndef XyPlotSetupPanel_hpp
#define XyPlotSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "wx/sizer.h"

#include <wx/image.h>
#include <wx/statline.h>
#include <wx/spinbutt.h>
#include <wx/spinctrl.h>
#include <wx/splitter.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include <wx/notebook.h>
#include <wx/grid.h>


class XyPlotSetupPanel: public wxPanel
{
public:
    XyPlotSetupPanel(wxWindow *parent);
   
private:
    wxBoxSizer *pageBoxSizer;
    wxGridSizer *paramGridSizer;
    wxBoxSizer *paramBoxSizer;
    wxBoxSizer *xButtonsBoxSizer;
    wxBoxSizer *yButtonsBoxSizer;
    wxBoxSizer *xSelelectedBoxSizer;
    wxBoxSizer *ySelelectedBoxSizer;
    
    wxStaticText *titleAvailbleText;
    wxStaticText *titleXText;
    wxStaticText *titleYText;
    
    wxListBox *paramListBox;
    wxListBox *xSelectedListBox;
    wxListBox *ySelectedListBox;
    
    wxButton *addXButton;
    wxButton *addYButton;
    wxButton *clearYButton;
    wxButton *removeXButton;
    wxButton *removeYButton;
    
    void CreatePanel(wxWindow *parent);
    void OnAddX(wxCommandEvent& event);
    void OnAddY(wxCommandEvent& event);
    void OnRemoveX(wxCommandEvent& event);
    void OnRemoveY(wxCommandEvent& event);
    void OnClearY(wxCommandEvent& event);
    
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        XY_TEXT = 92000,
        XY_LISTBOX,
        XY_PLOT_ADD_X,
        XY_PLOT_ADD_Y,
        XY_PLOT_REMOVE_X,
        XY_PLOT_REMOVE_Y,
        XY_PLOT_CLEAR_Y
    };
};
#endif
