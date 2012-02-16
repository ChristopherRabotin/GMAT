//$Id$
//------------------------------------------------------------------------------
//                              EventFilePanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Linda Jun
// Created: 2004/02/02
//
/**
 * Implements EventFilePanel class.
 */
//------------------------------------------------------------------------------

#include "EventFilePanel.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "MessageInterface.hpp"
#include "FileManager.hpp"

#include <wx/file.h> // for wxFile

//#define DEBUG_REPORT_FILE_PANEL

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(EventFilePanel, wxPanel)
   EVT_BUTTON(ID_BUTTON_CLOSE, EventFilePanel::OnClose)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// EventFilePanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs EventFilePanel object.
 *
 * @param <parent> parent window.
 *
 */
//------------------------------------------------------------------------------
EventFilePanel::EventFilePanel(wxWindow *parent, wxString locatorName)
    : wxPanel(parent)
{
   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   mLocatorName = locatorName;
   
   theParent = parent;
   
   theLocator =
      (EventLocator*) theGuiInterpreter->GetRunningObject(mLocatorName.c_str());
   
   Create();
   
   if (theLocator != NULL)
   {
      Show();
   }
   else
   {
      // show error message
      MessageInterface::ShowMessage
         ("**** ERROR **** EventFilePanel:Create() the running EventLocator "
          "\"%s\" is NULL\n", locatorName.c_str());
   }
   
}


//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void EventFilePanel::Create()
//------------------------------------------------------------------------------
void EventFilePanel::Create()
{
   int borderSize = 3;
   wxStaticBox *middleStaticBox = new wxStaticBox( this, -1, wxT("") );
   wxStaticBox *bottomStaticBox = new wxStaticBox( this, -1, wxT("") );
   
   // create sizers
   thePanelSizer = new wxBoxSizer(wxVERTICAL);
   theMiddleSizer = new wxStaticBoxSizer( middleStaticBox, wxVERTICAL );
   theBottomSizer = new wxStaticBoxSizer( bottomStaticBox, wxVERTICAL );
   wxBoxSizer *theButtonSizer = new wxBoxSizer(wxHORIZONTAL);
   wxGridSizer *theGridSizer = new wxGridSizer( 1, 0, 0 );
   
   // create the text ctrl
   mFileContentsTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
                                           wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP |
                                           wxTE_READONLY | wxTE_MULTILINE | wxGROW);
   
   // set font
   mFileContentsTextCtrl->SetFont( GmatAppData::Instance()->GetFont() );
      
   // create bottom buttons
   theCloseButton =
      new wxButton(this, ID_BUTTON_CLOSE, "Close", wxDefaultPosition, wxDefaultSize, 0);
   theHelpButton =
      new wxButton(this, ID_BUTTON_HELP, "Help", wxDefaultPosition, wxDefaultSize, 0);
   
   theGridSizer->Add(mFileContentsTextCtrl, 0, wxGROW | wxALIGN_CENTER | wxALL,
                     borderSize);
   theMiddleSizer->Add(theGridSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, borderSize);
   
   // adds the buttons to button sizer
   theButtonSizer->Add(theCloseButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   theButtonSizer->Add(theHelpButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   
   theBottomSizer->Add(theButtonSizer, 0, wxALIGN_CENTER | wxALL, borderSize);
   
}


//------------------------------------------------------------------------------
// void Show()
//------------------------------------------------------------------------------
/**
 * Shows the panel.
 */
//------------------------------------------------------------------------------
void EventFilePanel::Show()
{
   thePanelSizer->Add(theMiddleSizer, 1, wxGROW | wxALL, 1);
   thePanelSizer->Add(theBottomSizer, 0, wxGROW | wxALL, 1);
   
   // tells the enclosing window to adjust to the size of the sizer
   SetAutoLayout( TRUE );
   SetSizer(thePanelSizer); //use the sizer for layout
   thePanelSizer->Fit(this); //loj: if theParent is used it doesn't show the scroll bar
   thePanelSizer->SetSizeHints(this); //set size hints to honour minimum size
   
   LoadData();
   
   theHelpButton->Disable();   //loj: for build2
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void EventFilePanel::LoadData()
{
   std::string filename = theLocator->GetStringParameter("Filename");
   
   // If filename does not include folder, use output as default
   if ((filename.find('/',0) == std::string::npos) &&
       (filename.find('\\',0) == std::string::npos))
   {
      std::string fileLocation =
            FileManager::Instance()->GetFullPathname(FileManager::OUTPUT_PATH);
      filename = fileLocation + filename;
   }

   wxFile *file = new wxFile();
   bool mFileExists = file->Exists(filename.c_str());
   bool populated   = theLocator->FileWasWritten();
   
   #ifdef DEBUG_REPORT_FILE_PANEL
   MessageInterface::ShowMessage
      ("===> %p, EventFilePanel::LoadData() filename=%s, mFileExists=%d\n",
       theLocator, filename.c_str(), mFileExists);
   #endif
   
   if (mFileExists && populated)
      mFileContentsTextCtrl->LoadFile(filename.c_str());
   else
      mFileContentsTextCtrl->SetValue("\n   *** No events were found ***\n");
   
   delete file;
}


//------------------------------------------------------------------------------
// void OnClose(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Close page.
 */
//------------------------------------------------------------------------------
void EventFilePanel::OnClose(wxCommandEvent &WXUNUSED(event))
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
void EventFilePanel::OnHelp()
{
   // open separate window to show help?
}
