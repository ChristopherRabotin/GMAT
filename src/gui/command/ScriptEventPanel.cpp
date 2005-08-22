//$Header$
//------------------------------------------------------------------------------
//                              ScriptEventPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2005/1/12
//
/**
 * Implements ScriptEventPanel class.
 */
//------------------------------------------------------------------------------

#include "ScriptEventPanel.hpp"
#include "MessageInterface.hpp"

#include <sstream>      // for std::stringstream


//#define DEBUG_SCRIPTEVENT_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ScriptEventPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   
   EVT_TEXT(ID_TEXTCTRL, ScriptEventPanel::OnTextUpdate)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ScriptEventPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
ScriptEventPanel::ScriptEventPanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   theCommand = cmd;
   
   if (theCommand != NULL)
   {
     Create();
     Show();
   }
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void ScriptEventPanel::Create()
{
   int bsize = 3; // border size
   
    // create sizers
   mBottomSizer = new wxGridSizer( 1, 0, 0 );
   mPageSizer = new wxBoxSizer(wxVERTICAL);


   mFileContentsTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE | wxGROW);


   //------------------------------------------------------
   // add to sizer
   //------------------------------------------------------

   mBottomSizer->Add(mFileContentsTextCtrl, 0, wxGROW | wxALIGN_CENTER | wxALL, 
                     bsize);

   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   mPageSizer->Add(mBottomSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
   theMiddleSizer->Add(mPageSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ScriptEventPanel::LoadData()
{
   // Set the pointer for the "Show Script" button
   mObject = theCommand;

//   GmatCommand *current = theCommand;
   std::stringstream text;
//   if (current->GetTypeName() == "BeginScript") {
//      do {
//         text << current->GetGeneratingString() << "\n";
//         current = current->GetNext();
//         if (current == NULL)
//            break;
//      } while (current->GetTypeName() != "EndScript");
//      // Be sure to write out the last one!
//      text << current->GetGeneratingString() << "\n";
//      theCommand->SetGeneratingString(text.str());
//   }
//   else
      text << theCommand->GetGeneratingString();

   wxString scriptText = text.str().c_str();
   mFileContentsTextCtrl->AppendText(scriptText);
   
   theApplyButton->Disable();
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ScriptEventPanel::SaveData()
{
   std::stringstream scriptText;
   scriptText << mFileContentsTextCtrl->GetValue().c_str();
   std::string firstBlock;
   scriptText >> firstBlock;

   #ifdef DEBUG_SCRIPTEVENT_PANEL
      MessageInterface::PopupMessage(Gmat::INFO_, "First block is [" +
         firstBlock + "]");
   #endif

   if (firstBlock != theCommand->GetTypeName()) {
      MessageInterface::PopupMessage(Gmat::WARNING_,
         "Changing command types from " + theCommand->GetTypeName() +
         " to " + firstBlock + " is not yet supported.");
      return;
   }

   theCommand->SetGeneratingString(scriptText.str());
   
   try
   {
      theGuiInterpreter->Interpret(theCommand, scriptText.str());
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_,
         "Error parsing the ScriptEvent; please correct the text");
   }
}

//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ScriptEventPanel::OnTextUpdate(wxCommandEvent& event)
{
   theApplyButton->Enable();
}



