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
#include "CelesBodySelectDialog.hpp"

#include "MessageInterface.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(OpenGlPlotSetupPanel, GmatPanel)
    EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
    EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
    EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
    EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
    EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)

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
    
    theSubscriber =
        theGuiInterpreter->GetSubscriber(std::string(subscriberName.c_str()));
    
    Create();
    Show();

    //===================================================
    //loj: just for testing CelesBodySelectDialog
    //===================================================

    //----- no exclusion
    wxArrayString exclBodies;
    CelesBodySelectDialog dlg1(this, exclBodies);
    dlg1.ShowModal();

    if (dlg1.IsBodySelected())
    {
        wxArrayString &names = dlg1.GetBodyNames();
        for (int i=0; i<names.GetCount(); i++)
            MessageInterface::ShowMessage("OpenGlPlotSetupPanel::**JUST TEST** body name = %s\n",
                                          names[i].c_str());
    }
    
    //----- exclude Sun
    exclBodies.Add("Sun");
    
    CelesBodySelectDialog dlg2(this, exclBodies);
    dlg2.ShowModal();


    if (dlg2.IsBodySelected())
    {
        wxArrayString &names = dlg2.GetBodyNames();
        for (int i=0; i<names.GetCount(); i++)
            MessageInterface::ShowMessage("OpenGlPlotSetupPanel::**JUST TEST** body name = %s\n",
                                          names[i].c_str());
    }
    
    //----- exclude Sun, Earth
    exclBodies.Add("Earth");
    
    CelesBodySelectDialog dlg3(this, exclBodies);
    dlg3.ShowModal();


    if (dlg3.IsBodySelected())
    {
        wxArrayString &names = dlg3.GetBodyNames();
        for (int i=0; i<names.GetCount(); i++)
            MessageInterface::ShowMessage("OpenGlPlotSetupPanel:: **JUST TEST** body name = %s\n",
                                          names[i].c_str());
    }

    //----- exclude Sun, Earth, Luna
    exclBodies.Add("Luna");
    
    CelesBodySelectDialog dlg4(this, exclBodies);
    dlg4.ShowModal();

    if (dlg4.IsBodySelected())
    {
        wxArrayString &names = dlg4.GetBodyNames();
        for (int i=0; i<names.GetCount(); i++)
            MessageInterface::ShowMessage("OpenGlPlotSetupPanel:: **JUST TEST** body name = %s\n",
                                          names[i].c_str());
    }


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
    optionBoxSizer->Add(plotCheckBox, 0, wxALIGN_CENTER|wxALL, 5);
            
    //------------------------------------------------------
    // put in the order
    //------------------------------------------------------    
    pageBoxSizer->Add(optionBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    
    //------------------------------------------------------
    // add to parent sizer
    //------------------------------------------------------
    theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);

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
