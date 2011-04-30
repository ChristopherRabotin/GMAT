//$Id$
//------------------------------------------------------------------------------
//                              GmatCommandPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2010/07/15
/**
 * Implements GmatCommandPanel class.
 */
//------------------------------------------------------------------------------
#include "GmatCommandPanel.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_GMATCOMMAND_PANEL
//#define DEBUG_GMATCOMMAND_PANEL_LOAD
//#define DEBUG_GMATCOMMAND_PANEL_SAVE

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(GmatCommandPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
   
   EVT_TEXT(ID_TEXT_CTRL, GmatCommandPanel::OnTextChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
   
//------------------------------------------------------------------------------
// GmatCommandPanel(wxWindow *parent, const wxString &subscriberName)
//------------------------------------------------------------------------------
/**
 * Constructs GmatCommandPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the GmatCommandPanel GUI
 */
//------------------------------------------------------------------------------
GmatCommandPanel::GmatCommandPanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   #ifdef DEBUG_GMATCOMMAND_PANEL
   MessageInterface::ShowMessage
      ("GmatCommandPanel() entered, cmd=<%p><%s>\n", cmd,
       cmd ? cmd->GetTypeName().c_str() : "NULL");
   #endif
   
   if (cmd != NULL)
   {
      theCommand = cmd;
      mObject = (GmatBase*)cmd;
      Create();
      Show();
      EnableUpdate(false);
   }
   
   #ifdef DEBUG_GMATCOMMAND_PANEL
   MessageInterface::ShowMessage("GmatCommandPanel() exiting...\n");
   #endif
}


//------------------------------------------------------------------------------
// ~GmatCommandPanel()
//------------------------------------------------------------------------------
GmatCommandPanel::~GmatCommandPanel()
{
}


//-------------------------------
// protected methods
//-------------------------------

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
void GmatCommandPanel::Create()
{
   #ifdef DEBUG_GMATCOMMAND_PANEL
   MessageInterface::ShowMessage("GmatCommandPanel::Create() entering...\n");
   #endif
   
   Integer bsize = 2; // border size
   wxArrayString emptyList;   
   
   //-------------------------------------------------------
   // show get generating string
   //-------------------------------------------------------
   wxBoxSizer *textSizer = new wxBoxSizer(wxVERTICAL);
   
   commandTextCtrl = new
      wxTextCtrl(this, ID_TEXT_CTRL, wxT(""), wxDefaultPosition, wxSize(450, -1));
   
   textSizer->Add(commandTextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   //-------------------------------------------------------
   // add to parent sizer
   //-------------------------------------------------------
   theMiddleSizer->Add(textSizer, 1, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   #ifdef DEBUG_GMATCOMMAND_PANEL
   MessageInterface::ShowMessage("GmatCommandPanel::Create() exiting...\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void GmatCommandPanel::LoadData()
{
   #ifdef DEBUG_GMATCOMMAND_PANEL_LOAD
   MessageInterface::ShowMessage("GmatCommandPanel::LoadData() entering...\n");
   #endif
   
   try
   {
      wxString genStr = theCommand->GetGeneratingString().c_str();
      commandTextCtrl->SetValue(genStr);
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   #ifdef DEBUG_GMATCOMMAND_PANEL_LOAD
   MessageInterface::ShowMessage("GmatCommandPanel::LoadData() exiting...\n");
   #endif
   
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void GmatCommandPanel::SaveData()
{
   #ifdef DEBUG_GMATCOMMAND_PANEL_SAVE
   MessageInterface::ShowMessage("GmatCommandPanel::SaveData() entered\n");
   #endif
   
   canClose = true;
   std::string str;
//   Integer width, prec;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      std::string cmdStr = commandTextCtrl->GetValue().c_str();
      theCommand->SetGeneratingString(cmdStr);
      theCommand->InterpretAction();
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
   
   #ifdef DEBUG_GMATCOMMAND_PANEL_SAVE
   MessageInterface::ShowMessage("GmatCommandPanel::SaveData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void GmatCommandPanel::OnTextChange(wxCommandEvent &event)
{
    EnableUpdate(true);
}


