//$Header$
//------------------------------------------------------------------------------
//                              GmatPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/02
//
/**
 * Implements GmatPanel class.
 */
//------------------------------------------------------------------------------

#include "GmatPanel.hpp"
#include "GmatAppData.hpp"
#include "gmatdefs.hpp" //put this one after GUI includes

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(GmatPanel, wxPanel)
    EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
    EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
    EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
    EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// GmatPanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs GmatPanel object.
 *
 * @param <parent> input parent.
 *
 */
//------------------------------------------------------------------------------
GmatPanel::GmatPanel(wxWindow *parent)
    : wxPanel(parent)
{
    theGuiInterpreter = GmatAppData::GetGuiInterpreter();
    theParent = parent;
    
    // create sizers
    //thePanelSizer = new wxFlexGridSizer(3, 1, 0, 0 );
    thePanelSizer = new wxBoxSizer(wxVERTICAL);
    theTopSizer = new wxBoxSizer(wxVERTICAL);
    theMiddleSizer = new wxBoxSizer(wxVERTICAL);
    theBottomSizer = new wxBoxSizer(wxVERTICAL);
   
    // create script button
    theScriptButton = new wxButton(parent, ID_BUTTON_SCRIPT, "Show Script", 
                                   wxDefaultPosition, wxDefaultSize, 0);

    // create other buttons
    theOkButton = new wxButton(parent, ID_BUTTON_OK, "OK", 
                               wxDefaultPosition, wxDefaultSize, 0);
    theApplyButton = new wxButton(parent, ID_BUTTON_APPLY, "Apply", 
                                  wxDefaultPosition, wxDefaultSize, 0);
    theCancelButton = new wxButton(parent, ID_BUTTON_CANCEL, "Cancel", 
                                   wxDefaultPosition, wxDefaultSize, 0);
    theHelpButton = new wxButton(parent, ID_BUTTON_HELP, "Help", 
                                 wxDefaultPosition, wxDefaultSize, 0);
    
    
    // add items to top sizer
    theTopSizer->Add(theScriptButton, 0, wxALIGN_RIGHT | wxALL, 5);
    
    // adds the buttons to button sizer    
    theBottomSizer->Add(theOkButton, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);
    theBottomSizer->Add(theApplyButton, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);
    theBottomSizer->Add(theCancelButton, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);
    theBottomSizer->Add(theHelpButton, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);
   
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void Show()
//------------------------------------------------------------------------------
/**
 * Shows the panel.
 */
//------------------------------------------------------------------------------
void GmatPanel::Show()
{
    // add items to panel sizer
    thePanelSizer->Add(theTopSizer, 0, wxGROW | wxALL, 5);
    thePanelSizer->Add(theMiddleSizer, 0, wxGROW | wxALL, 5);
    thePanelSizer->Add(theBottomSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
    
    // tells the enclosing window to adjust to the size of the sizer
    theParent->SetAutoLayout( TRUE );
    theParent->SetSizer(thePanelSizer);
    thePanelSizer->Fit(theParent);
    thePanelSizer->SetSizeHints(theParent);
    
}

//------------------------------------------------------------------------------
// void OnOk()
//------------------------------------------------------------------------------
/**
 * Saves the data and closes the page
 */
//------------------------------------------------------------------------------
void GmatPanel::OnOK()
{
    if (theApplyButton->IsEnabled())
        OnApply();
}

//------------------------------------------------------------------------------
// void OnCancel()
//------------------------------------------------------------------------------
/**
 * Close page.
 */
//------------------------------------------------------------------------------
void GmatPanel::OnCancel()
{
    // Close page from main notebook
    GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
    gmatMainNotebook->ClosePage();
}

//------------------------------------------------------------------------------
// void OnApply()
//------------------------------------------------------------------------------
/**
 * Saves the data and remain unclosed.
 */
//------------------------------------------------------------------------------
void GmatPanel::OnApply()
{
    SaveData();
    GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
    gmatMainNotebook->ClosePage();
}

//------------------------------------------------------------------------------
// void OnHelp()
//------------------------------------------------------------------------------
/**
 * Shows Helps
 */
//------------------------------------------------------------------------------
void GmatPanel::OnHelp()
{
    // open separate window to show help
}

//------------------------------------------------------------------------------
// void OnScript()
//------------------------------------------------------------------------------
/**
 * Shows Scripts
 */
//------------------------------------------------------------------------------
void GmatPanel::OnScript()
{
    // open separate window to show scripts
}

