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
 * Implements OpenGlPlotSetupPanel class.
 */
//------------------------------------------------------------------------------
#include "OpenGlPlotSetupPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "GuiItemManager.hpp"

#include "MessageInterface.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(OpenGlPlotSetupPanel, GmatPanel)
    EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
    EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
    EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
    EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
    EVT_BUTTON(OPENGL_PLOT_CHECKBOX, OpenGlPlotSetupPanel::OnPlotCheckBoxChange)

    EVT_CHECKBOX(OPENGL_PLOT_CHECKBOX, OpenGlPlotSetupPanel::OnPlotCheckBoxChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
//------------------------------------------------------------------------------
// OpenGlPlotSetupPanel(wxWindow *parent, const wxString &subscriberName)
//------------------------------------------------------------------------------
/**
 * Constructs OpenGlPlotSetupPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the OpenGlPlotSetupPanel GUI
 */
//------------------------------------------------------------------------------
OpenGlPlotSetupPanel::OpenGlPlotSetupPanel(wxWindow *parent,
                                   const wxString &subscriberName)
    : GmatPanel(parent)
{
    //MessageInterface::ShowMessage("OpenGlPlotSetupPanel() entering...\n");
    //MessageInterface::ShowMessage("OpenGlPlotSetupPanel() subscriberName = " +
    //                              std::string(subscriberName.c_str()) + "\n");
    
    theParent = parent;
    theSubscriber =
        theGuiInterpreter->GetSubscriber(std::string(subscriberName.c_str()));

    theGuiManager = GuiItemManager::GetInstance();
    
    Create();
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnPlotCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnPlotCheckBoxChange(wxCommandEvent& event)
{
    theApplyButton->Enable();
}

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * @param <scName> input spacecraft name.
 *
 * @note Creates the notebook for spacecraft information
 */
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::Create()
{
    //MessageInterface::ShowMessage("OpenGlPlotSetupPanel::Create() entering...\n");

    pageBoxSizer = new wxBoxSizer(wxVERTICAL);

    //------------------------------------------------------
    // plot option, (1st column)
    //------------------------------------------------------
    plotCheckBox = new wxCheckBox(this, OPENGL_PLOT_CHECKBOX, wxT("Show Plot"),
                                  wxDefaultPosition, wxSize(100, -1), 0);
            
    optionBoxSizer = new wxBoxSizer(wxVERTICAL);
    optionBoxSizer->Add(plotCheckBox, 0, wxALIGN_LEFT|wxALL, 5);
            
    //------------------------------------------------------
    // put in the order
    //------------------------------------------------------    
    pageBoxSizer->Add(optionBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    
    //------------------------------------------------------
    // add to parent sizer
    //------------------------------------------------------
    theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    Show();

}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::LoadData()
{
    // load data from the core engine
    plotCheckBox->SetValue(theSubscriber->IsActive());

}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::SaveData()
{
    // save data to core engine
    theSubscriber->Activate(plotCheckBox->IsChecked());
}

//------------------------------------------------------------------------------
// virtual void OnHelp()
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnHelp()
{
    // open the window
    GmatPanel::OnHelp();

    // fill help text
}

//------------------------------------------------------------------------------
// virtual void OnScript()
//------------------------------------------------------------------------------
void OpenGlPlotSetupPanel::OnScript()
{
    // open the window
    GmatPanel::OnScript();

    // fill scripts
}

