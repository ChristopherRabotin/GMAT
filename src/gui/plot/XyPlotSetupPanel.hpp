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
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

class XyPlotSetupPanel: public GmatPanel
{
public:
    XyPlotSetupPanel(wxWindow *parent, const wxString &subscriberName);
   
protected:
    Subscriber *theSubscriber;
    wxString *theParamList;
    int  mNumXParams;
    int  mNumYParams;
    bool mXParamChanged;
    bool mYParamChanged;
    
    wxBoxSizer *pageBoxSizer;
    wxFlexGridSizer *paramGridSizer;
    wxBoxSizer *paramBoxSizer;
    wxBoxSizer *xButtonsBoxSizer;
    wxBoxSizer *yButtonsBoxSizer;
    wxBoxSizer *xSelelectedBoxSizer;
    wxBoxSizer *ySelelectedBoxSizer;
    wxBoxSizer *optionBoxSizer;
    
    wxStaticText *titleAvailbleText;
    wxStaticText *titleXText;
    wxStaticText *titleYText;
    wxStaticText *titleScText;
    wxStaticText *emptyText;
    
    wxListBox *paramListBox;
    wxListBox *xSelectedListBox;
    wxListBox *ySelectedListBox;
    
    wxButton *addXButton;
    wxButton *addYButton;
    wxButton *clearYButton;
    wxButton *removeXButton;
    wxButton *removeYButton;
    
    wxCheckBox *plotCheckBox;
    wxComboBox *scComboBox;

    void OnAddX(wxCommandEvent& event);
    void OnAddY(wxCommandEvent& event);
    void OnRemoveX(wxCommandEvent& event);
    void OnRemoveY(wxCommandEvent& event);
    void OnClearY(wxCommandEvent& event);
    void OnPlotCheckBoxChange(wxCommandEvent& event);

    // methods inherited from GmatPanel
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
    
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        XY_PLOT_TEXT = 92000,
        XY_PLOT_LISTBOX,
        XY_PLOT_ADD_X,
        XY_PLOT_ADD_Y,
        XY_PLOT_REMOVE_X,
        XY_PLOT_REMOVE_Y,
        XY_PLOT_CLEAR_Y,
        XY_PLOT_CHECKBOX,
    };
};
#endif
