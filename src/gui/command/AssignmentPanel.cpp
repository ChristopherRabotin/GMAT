//$Id$
//------------------------------------------------------------------------------
//                              AssignmentPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Allison Greene
// Created: 2004/12/23
/**
 * This class contains the Assignment Setup window.
 */
//------------------------------------------------------------------------------

#include "AssignmentPanel.hpp"
#include "StringUtil.hpp"           // for GmatStringUtil::
#include "MessageInterface.hpp"
#include <wx/config.h>

//#define DEBUG_ASSIGNMENT_PANEL 1
//#define DEBUG_ASSIGNMENT_PANEL_SAVE 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(AssignmentPanel, GmatPanel)
   EVT_TEXT(ID_TEXTCTRL, AssignmentPanel::OnTextChange)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// AssignmentPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
AssignmentPanel::AssignmentPanel( wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   #if DEBUG_ASSIGNMENT_PANEL
   MessageInterface::ShowMessage("AssignmentPanel() entered\n");
   #endif
   
   theCommand = (Assignment *)cmd;
   mIsTextModified = false;
   
   if (theCommand != NULL)
   {
      Create();
      Show();
   }
}


//------------------------------------------------------------------------------
// ~AssignmentPanel()
//------------------------------------------------------------------------------
AssignmentPanel::~AssignmentPanel()
{
}

//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void AssignmentPanel::Create()
{
   #if DEBUG_ASSIGNMENT_PANEL
   MessageInterface::ShowMessage("AssignmentPanel::Create() entered\n");
   #endif
   
   int bsize = 2; // bordersize
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Equation"));
   
   wxFlexGridSizer *pageSizer = new wxFlexGridSizer(3, 0, 0);
   
   wxStaticText *mLhsLabel =
      new wxStaticText( this, ID_TEXT, wxT(GUI_ACCEL_KEY"Left-Hand Side") );
   mLhsTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                     wxDefaultPosition, wxSize(150,-1), 0);
   mLhsTextCtrl->SetToolTip(pConfig->Read(_T("LeftHandSideHint")));
   
   wxStaticText *equalSign =
      new wxStaticText(this, ID_TEXT, wxT(" = "),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   wxStaticText *mRhsLabel =
      new wxStaticText( this, ID_TEXT, wxT(GUI_ACCEL_KEY"Right-Hand Side") );
   mRhsTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                     wxDefaultPosition, wxSize(400,-1), 0);
   mRhsTextCtrl->SetToolTip(pConfig->Read(_T("RightHandSideHint")));
   
   pageSizer->Add(mLhsLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   pageSizer->Add(0, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(mRhsLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   pageSizer->Add(mLhsTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   pageSizer->Add(equalSign, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(mRhsTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   
   theMiddleSizer->Add(pageSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void AssignmentPanel::LoadData()
{
   #if DEBUG_ASSIGNMENT_PANEL
   MessageInterface::ShowMessage("AssignmentPanel::LoadData() entered\n");
   #endif
   
   mObject = theCommand;
   
   wxString lhs = theCommand->GetLHS().c_str();
   wxString rhs = theCommand->GetRHS().c_str();
   
   mLhsTextCtrl->SetValue(lhs);
   mRhsTextCtrl->SetValue(rhs);
   
   if (lhs == "" || rhs == "")
   {
      // force to call SaveData() for empty LHS and RHS checking
      mIsTextModified = true;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void AssignmentPanel::SaveData()
{
   #if DEBUG_ASSIGNMENT_PANEL_SAVE
   MessageInterface::ShowMessage
      ("AssignmentPanel::SaveData() mIsTextModified=%d\n", mIsTextModified);
   #endif
   
   canClose = true;
   std::string lhs = mLhsTextCtrl->GetValue().c_str();
   std::string rhs = mRhsTextCtrl->GetValue().c_str();
   
   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   if (mIsTextModified)
   {
      Real rval;
      
      // check if it has blank lhs or rhs
      if (lhs == "" || rhs == "")
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "LHS or RHS cannot be blank");
         canClose = false;
         return;
      }
      
      // Lhs cannot be a number
      if (GmatStringUtil::ToReal(lhs, rval))
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "Left hand side cannot be a number. %f",
             rval);
         canClose = false;
      }
      
      // Lhs should be an existing variable or valid object property
      // Valid object property should be checked by Assignment::InterpretAction()
      if (GmatStringUtil::IsValidName(lhs))
      {
         if (theGuiInterpreter->GetConfiguredObject(lhs) == NULL)
         {
            MessageInterface::PopupMessage
               (Gmat::ERROR_, "Left hand side should be an existing Variable "
                "or Object Property");
            canClose = false;
         }
      }
   }
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the syntax checking
   //-----------------------------------------------------------------
   if (mIsTextModified)
   {
      wxString genStr = "GMAT " + mLhsTextCtrl->GetValue() + " = " +
         mRhsTextCtrl->GetValue();
      
      #if DEBUG_ASSIGNMENT_PANEL_SAVE
      MessageInterface::ShowMessage("     genStr=%s\n", genStr.c_str());
      #endif
      
      try
      {
         // Interpret assignment string         
         theCommand->SetGeneratingString(genStr.c_str());
         theCommand->InterpretAction();
         
         // Create element wrappers
         if (!theGuiInterpreter->ValidateCommand(theCommand))
            canClose = false;
         
         mIsTextModified = false;
      }
      catch (BaseException &e)
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
         canClose = false;
      }
   }
}


//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void AssignmentPanel::OnTextChange(wxCommandEvent& event)
{
   if (mLhsTextCtrl->IsModified() || mRhsTextCtrl->IsModified())
   {
      mIsTextModified = true;
      EnableUpdate(true);
   }
}

