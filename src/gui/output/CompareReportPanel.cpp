//$Id$
//------------------------------------------------------------------------------
//                              CompareReportPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/02
//
/**
 * Implements CompareReportPanel class.
 */
//------------------------------------------------------------------------------

#include "CompareReportPanel.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CompareReportPanel, wxPanel)
   EVT_BUTTON(ID_BUTTON_CLOSE, CompareReportPanel::OnClose)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// CompareReportPanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs CompareReportPanel object.
 *
 * @param <parent> parent window.
 *
 */
//------------------------------------------------------------------------------
CompareReportPanel::CompareReportPanel(wxWindow *parent, const wxString &name)
    : wxPanel(parent)
{
   mName = name;
   
   int borderSize = 3;
   
   // create the text ctrl
   mTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                              wxDefaultPosition, wxDefaultSize, //wxSize(800, 400),
                              wxTE_DONTWRAP | wxTE_READONLY | wxTE_MULTILINE);
   
   // set font
   mTextCtrl->SetFont( GmatAppData::Instance()->GetFont() );
   
   // create bottom buttons
   theCloseButton =
      new wxButton(this, ID_BUTTON_CLOSE, "Close", wxDefaultPosition, wxDefaultSize, 0);
   theHelpButton =
      new wxButton(this, ID_BUTTON_HELP, "Help", wxDefaultPosition, wxDefaultSize, 0);

   // adds the buttons to button sizer
   wxBoxSizer *theButtonSizer = new wxBoxSizer(wxHORIZONTAL);
   theButtonSizer->Add(theCloseButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   theButtonSizer->Add(theHelpButton, 0, wxALIGN_CENTER | wxALL, borderSize);

   wxStaticBoxSizer *theBottomSizer = new wxStaticBoxSizer(wxVERTICAL, this, "");
   theBottomSizer->Add(theButtonSizer, 0, wxALIGN_CENTER | wxALL, borderSize);
   
   wxBoxSizer *thePanelSizer = new wxBoxSizer(wxVERTICAL);
   thePanelSizer->Add(mTextCtrl, 1, wxGROW | wxALL, 1);
   thePanelSizer->Add(theBottomSizer, 0, wxGROW | wxALL, 1);

   // tells the enclosing window to adjust to the size of the sizer
   SetAutoLayout( TRUE );
   SetSizer(thePanelSizer); //use the sizer for layout
   thePanelSizer->Fit(this); //loj: if theParent is used it doesn't show the scroll bar
   thePanelSizer->SetSizeHints(this); //set size hints to honour minimum size
   
   theHelpButton->Disable();
}


//------------------------------------------------------------------------------
// void OnClose()
//------------------------------------------------------------------------------
/**
 * Close page.
 */
//------------------------------------------------------------------------------
void CompareReportPanel::OnClose(wxCommandEvent &event)
{
   GmatAppData::Instance()->GetMainFrame()->CloseActiveChild();
}

//------------------------------------------------------------------------------
// void OnHelp()
//------------------------------------------------------------------------------
/**
 * Shows Helps
 */
//------------------------------------------------------------------------------
void CompareReportPanel::OnHelp()
{
   // open separate window to show help?
}
