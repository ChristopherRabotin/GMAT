//$Header$
//------------------------------------------------------------------------------
//                              ReportFileSetupPanel
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
 * Implements ReportFileSetupPanel class.
 */
//------------------------------------------------------------------------------
#include "ReportFileSetupPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "GuiItemManager.hpp"

#include "MessageInterface.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(ReportFileSetupPanel, wxPanel)
    EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
    EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
    EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
    EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
    EVT_BUTTON(RF_WRITE_CHECKBOX, ReportFileSetupPanel::OnWriteCheckBoxChange)

    EVT_CHECKBOX(RF_WRITE_CHECKBOX, ReportFileSetupPanel::OnWriteCheckBoxChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
    
//------------------------------------------------------------------------------
// ReportFileSetupPanel(wxWindow *parent, const wxString &subscriberName)
//------------------------------------------------------------------------------
/**
 * Constructs ReportFileSetupPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the ReportFileSetupPanel GUI
 */
//------------------------------------------------------------------------------
ReportFileSetupPanel::ReportFileSetupPanel(wxWindow *parent,
                                   const wxString &subscriberName)
    : GmatPanel(parent)
{
    //MessageInterface::ShowMessage("ReportFileSetupPanel() entering...\n");
    //MessageInterface::ShowMessage("ReportFileSetupPanel() subscriberName = " +
    //                              std::string(subscriberName.c_str()) + "\n");
    
    theParent = parent;
    theSubscriber =
        theGuiInterpreter->GetSubscriber(std::string(subscriberName.c_str()));

    theGuiManager = GuiItemManager::GetInstance();
    
    Create(this);
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnWriteCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnWriteCheckBoxChange(wxCommandEvent& event)
{
    theApplyButton->Enable();
}

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * @param <parent> input parent.
 * @param <scName> input spacecraft name.
 *
 * @note Creates the notebook for spacecraft information
 */
//------------------------------------------------------------------------------
void ReportFileSetupPanel::Create(wxWindow *parent)
{
    //MessageInterface::ShowMessage("ReportFileSetupPanel::Create() entering...\n");

    pageBoxSizer = new wxBoxSizer(wxVERTICAL);

    //------------------------------------------------------
    // plot option, (1st column)
    //------------------------------------------------------
    writeCheckBox = new wxCheckBox(parent, RF_WRITE_CHECKBOX, wxT("Write Report"),
                                   wxDefaultPosition, wxSize(100, -1), 0);
            
    optionBoxSizer = new wxBoxSizer(wxVERTICAL);
    optionBoxSizer->Add(writeCheckBox, 0, wxALIGN_LEFT|wxALL, 5);
        
    //------------------------------------------------------
    // Ok, Apply, Cancel, Help buttons
    //------------------------------------------------------
    //loj: recreate buttons here until GmatPanel works!!
    theOkButton = new wxButton(parent, ID_BUTTON_OK, "OK", 
                               wxDefaultPosition, wxDefaultSize, 0);
    theApplyButton = new wxButton(parent, ID_BUTTON_APPLY, "Apply", 
                                  wxDefaultPosition, wxDefaultSize, 0);
    theCancelButton = new wxButton(parent, ID_BUTTON_CANCEL, "Cancel", 
                                   wxDefaultPosition, wxDefaultSize, 0);
    theHelpButton = new wxButton(parent, ID_BUTTON_HELP, "Help", 
                                 wxDefaultPosition, wxDefaultSize, 0);
    
    //loj: recreate buttons here until GmatPanel works!!
    theBottomSizer = new wxBoxSizer(wxHORIZONTAL);
    
    // adds the buttons to button sizer    
    theBottomSizer->Add(theOkButton, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);
    theBottomSizer->Add(theApplyButton, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);
    theBottomSizer->Add(theCancelButton, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);
    theBottomSizer->Add(theHelpButton, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);
    
    //------------------------------------------------------
    // put in the order
    //------------------------------------------------------
    pageBoxSizer->Add(optionBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    
    //loj: add the theBottomSizer here until GmatPanel works!!
    pageBoxSizer->Add(theBottomSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    
    //------------------------------------------------------
    // add to parent sizer
    //------------------------------------------------------
    theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    Show();

}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ReportFileSetupPanel::LoadData()
{
    // load data from the core engine
    writeCheckBox->SetValue(theSubscriber->IsActive());

}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ReportFileSetupPanel::SaveData()
{
    // save data to core engine
    theSubscriber->Activate(writeCheckBox->IsChecked());
}

//------------------------------------------------------------------------------
// virtual void OnHelp()
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnHelp()
{
    // open the window
    GmatPanel::OnHelp();

    // fill help text
}

//------------------------------------------------------------------------------
// virtual void OnScript()
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnScript()
{
    // open the window
    GmatPanel::OnScript();

    // fill scripts
}

