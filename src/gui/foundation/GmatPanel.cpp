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
#include "MessageInterface.hpp"

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
 * @param <parent> parent window.
 *
 */
//------------------------------------------------------------------------------
GmatPanel::GmatPanel(wxWindow *parent)
    : wxPanel(parent)
{
    int borderSize = 3;
    
    theGuiInterpreter = GmatAppData::GetGuiInterpreter();
    theGuiManager = GuiItemManager::GetInstance();
    theParent = parent;
    
    wxStaticBox *topStaticBox = new wxStaticBox( this, -1, wxT("") );
    wxStaticBox *middleStaticBox = new wxStaticBox( this, -1, wxT("") );
    wxStaticBox *bottomStaticBox = new wxStaticBox( this, -1, wxT("") );
    
    // create sizers
    thePanelSizer = new wxBoxSizer(wxVERTICAL);
    theTopSizer = new wxStaticBoxSizer( topStaticBox, wxVERTICAL );
    theMiddleSizer = new wxStaticBoxSizer( middleStaticBox, wxVERTICAL );
    theBottomSizer = new wxStaticBoxSizer( bottomStaticBox, wxVERTICAL );
    wxBoxSizer *theButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    
    // create script button
    theScriptButton = new wxButton(this, ID_BUTTON_SCRIPT, "Show Script", 
                                   wxDefaultPosition, wxDefaultSize, 0);

    // create bottom buttons
    theOkButton =
        new wxButton(this, ID_BUTTON_OK, "OK", wxDefaultPosition, wxDefaultSize, 0);
    theApplyButton =
        new wxButton(this, ID_BUTTON_APPLY, "Apply", wxDefaultPosition, wxDefaultSize, 0);
    theCancelButton =
        new wxButton(this, ID_BUTTON_CANCEL, "Cancel", wxDefaultPosition, wxDefaultSize, 0);
    theHelpButton =
        new wxButton(this, ID_BUTTON_HELP, "Help", wxDefaultPosition, wxDefaultSize, 0);
    
    // add items to top sizer
    theTopSizer->Add(theScriptButton, 0, wxALIGN_RIGHT | wxALL, borderSize);
    
    // adds the buttons to button sizer    
    theButtonSizer->Add(theOkButton, 0, wxALIGN_CENTER | wxALL, borderSize);
    theButtonSizer->Add(theApplyButton, 0, wxALIGN_CENTER | wxALL, borderSize);
    theButtonSizer->Add(theCancelButton, 0, wxALIGN_CENTER | wxALL, borderSize);
    theButtonSizer->Add(theHelpButton, 0, wxALIGN_CENTER | wxALL, borderSize);
    
//      // adds the buttons to bottom sizer    
//      theBottomSizer->Add(theOkButton, 0, wxALIGN_CENTER | wxALL, borderSize);
//      theBottomSizer->Add(theApplyButton, 0, wxALIGN_CENTER | wxALL, borderSize);
//      theBottomSizer->Add(theCancelButton, 0, wxALIGN_CENTER | wxALL, borderSize);
//      theBottomSizer->Add(theHelpButton, 0, wxALIGN_CENTER | wxALL, borderSize);

    theBottomSizer->Add(theButtonSizer, 0, wxALIGN_CENTER | wxALL, borderSize);
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
    // add items to middle sizer
    
    thePanelSizer->Add(theTopSizer, 0, wxGROW | wxALL, 1);
    thePanelSizer->Add(theMiddleSizer, 1, wxGROW | wxALL, 1);
    thePanelSizer->Add(theBottomSizer, 0, wxGROW | wxALL, 1);
    
    // tells the enclosing window to adjust to the size of the sizer
    SetAutoLayout( TRUE );
    SetSizer(thePanelSizer); //use the sizer for layout
    thePanelSizer->Fit(this); //loj: if theParent is used it doesn't show the scroll bar
    thePanelSizer->SetSizeHints(this); //set size hints to honour minimum size

    LoadData();

    theApplyButton->Disable();
    theScriptButton->Disable(); //loj: for build2
    theHelpButton->Disable();   //loj: for build2
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
    
    // Close page from main notebook    
    GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
    gmatMainNotebook->ClosePage();
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
    // open separate window to show help?
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
    // open separate window to show scripts?
}

