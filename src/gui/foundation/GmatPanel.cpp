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

#include "ShowScriptDialog.hpp"
#include "ShowSummaryDialog.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(GmatPanel, wxPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_SUMMARY, GmatPanel::OnSummary)
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
GmatPanel::GmatPanel(wxWindow *parent, bool showScriptButton)
    : wxPanel(parent)
{
    
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   canClose = true;
   enableApply = true;

   mShowScriptButton = showScriptButton;

   theParent = parent;

   int borderSize = 3;
   wxStaticBox *topStaticBox = new wxStaticBox( this, -1, wxT("") );
   wxStaticBox *middleStaticBox = new wxStaticBox( this, -1, wxT("") );
   wxStaticBox *bottomStaticBox = new wxStaticBox( this, -1, wxT("") );

   // create sizers
   thePanelSizer = new wxBoxSizer(wxVERTICAL);
   theTopSizer = new wxStaticBoxSizer( topStaticBox, wxVERTICAL );
   theMiddleSizer = new wxStaticBoxSizer( middleStaticBox, wxVERTICAL );
   theBottomSizer = new wxStaticBoxSizer( bottomStaticBox, wxVERTICAL );
   wxBoxSizer *theButtonSizer = new wxBoxSizer(wxHORIZONTAL);

   wxBoxSizer *theTopButtonSizer = new wxBoxSizer(wxHORIZONTAL);

   // create script button
   theScriptButton = new wxButton(this, ID_BUTTON_SCRIPT, "Show Script",
                                  wxDefaultPosition, wxDefaultSize, 0);

   theSummaryButton = new wxButton(this, ID_BUTTON_SUMMARY, "Command Summary",
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
   theTopButtonSizer->Add(theScriptButton, 0, wxALIGN_RIGHT | wxALL, borderSize);
   theTopButtonSizer->Add(theSummaryButton, 0, wxALIGN_RIGHT | wxALL, borderSize);
   theTopSizer->Add(theTopButtonSizer, 0, wxALIGN_RIGHT | wxALL, borderSize);

   // adds the buttons to button sizer
   theButtonSizer->Add(theOkButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   theButtonSizer->Add(theApplyButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   theButtonSizer->Add(theCancelButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   theButtonSizer->Add(theHelpButton, 0, wxALIGN_CENTER | wxALL, borderSize);

   theBottomSizer->Add(theButtonSizer, 0, wxALIGN_CENTER | wxALL, borderSize);

   topStaticBox->Show(mShowScriptButton);

   mObject = NULL;
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

    // displays the script button
    thePanelSizer->Show(theTopSizer, mShowScriptButton);
    theScriptButton->Show(mShowScriptButton);
    thePanelSizer->Layout();
    
    // tells the enclosing window to adjust to the size of the sizer
    SetAutoLayout( TRUE );
    SetSizer(thePanelSizer); //use the sizer for layout
    thePanelSizer->Fit(this); //loj: if theParent is used it doesn't show the scroll bar
    thePanelSizer->SetSizeHints(this); //set size hints to honour minimum size
    
    LoadData();
    
    theApplyButton->Disable(); //loj: 9/25/06 Moved from before LoadData()
    theOkButton->Disable();    //loj: 9/25/06 Added
    theHelpButton->Disable();  // Not implemented yet
    
    if ((mObject == NULL) || (!mObject->IsOfType(Gmat::COMMAND)))
    theSummaryButton->Hide();
}


//------------------------------------------------------------------------------
// virtual void EnableUpdate(bool enable = true)
//------------------------------------------------------------------------------
void GmatPanel::EnableUpdate(bool enable)
{
   theApplyButton->Enable(enable);
   theOkButton->Enable(enable);
}


//------------------------------------------------------------------------------
// void OnOk()
//------------------------------------------------------------------------------
/**
 * Saves the data and closes the page
 */
//------------------------------------------------------------------------------
void GmatPanel::OnOK(wxCommandEvent &event)
{
    if (theApplyButton->IsEnabled())
        OnApply(event);
    
    // Close page from main notebook    
    // GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
    // gmatMainNotebook->ClosePage();
    if (canClose)
       GmatAppData::GetMainFrame()->CloseActiveChild();
}


//------------------------------------------------------------------------------
// void OnCancel()
//------------------------------------------------------------------------------
/**
 * Close page.
 */
//------------------------------------------------------------------------------
void GmatPanel::OnCancel(wxCommandEvent &event)
{
   // Close page from main notebook
   // GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
   // gmatMainNotebook->ClosePage();
   GmatAppData::GetMainFrame()->CloseActiveChild();
}


//------------------------------------------------------------------------------
// void OnApply()
//------------------------------------------------------------------------------
/**
 * Saves the data and remain unclosed.
 */
//------------------------------------------------------------------------------
void GmatPanel::OnApply(wxCommandEvent &event)
{
   SaveData();
   theGuiInterpreter->ConfigurationChanged(mObject, true);
   theApplyButton->Enable(!enableApply);
   theOkButton->Enable(!enableApply);
}


//------------------------------------------------------------------------------
// void OnHelp()
//------------------------------------------------------------------------------
/**
 * Shows Helps
 */
//------------------------------------------------------------------------------
void GmatPanel::OnHelp(wxCommandEvent &event)
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
void GmatPanel::OnScript(wxCommandEvent &event)
{
   wxString title = "Object Script";
   // open separate window to show scripts?
   if (mObject != NULL) {
      title = "Scripting for ";
      title += mObject->GetName().c_str();
   }
   ShowScriptDialog ssd(this, -1, title, mObject);
   ssd.ShowModal();
}


//------------------------------------------------------------------------------
// void OnSummary()
//------------------------------------------------------------------------------
/**
 * Shows command summary
 */
//------------------------------------------------------------------------------
void GmatPanel::OnSummary(wxCommandEvent &event)
{
   wxString title = "Object Script";
   // open separate window to show scripts?
   if (mObject != NULL) {
      title = "Command Summary for ";
      title += mObject->GetName().c_str();
   }
   ShowSummaryDialog ssd(this, -1, title, (GmatCommand*)mObject);
   ssd.ShowModal();
}
