//$Header$
//------------------------------------------------------------------------------
//                              OpenGlPlotSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/02/12
/**
 * Declares OpenGlPlotSetupPanel class. This class allows user to setup OpenGL Plot.
 */
//------------------------------------------------------------------------------
#ifndef OpenGlPlotSetupPanel_hpp
#define OpenGlPlotSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

class OpenGlPlotSetupPanel: public GmatPanel
{
public:
    OpenGlPlotSetupPanel(wxWindow *parent, const wxString &subscriberName);
   
protected:
    wxWindow *theParent;
    GuiItemManager *theGuiManager; //loj: move this to GmatPanel later
    Subscriber *theSubscriber;
    wxString *theParamList;
    
    wxBoxSizer *pageBoxSizer;
    wxBoxSizer *optionBoxSizer;

    wxCheckBox *plotCheckBox;

    void OnPlotCheckBoxChange(wxCommandEvent& event);

    // methods inherited from GmatPanel
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
    virtual void OnHelp();
    virtual void OnScript();
    
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        OPENGL_PLOT_TEXT = 93000,
        OPENGL_PLOT_CHECKBOX,
    };
};
#endif
