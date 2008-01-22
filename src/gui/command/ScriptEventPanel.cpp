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
#include "CommandUtil.hpp"       // for GetCommandSeqString()
#include "ShowScriptDialog.hpp"
#include "NoOp.hpp"
#include <sstream>               // for std::stringstream


//#define DBGLVL_SCRIPTEVENTPANEL_LOAD 1
//#define DBGLVL_SCRIPTEVENTPANEL_SAVE 1
//#define DBGLVL_SCRIPTEVENTPANEL_REPLACE_CMDS 1

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
//ScriptEventPanel::ScriptEventPanel(wxWindow *parent, GmatCommand *cmd)
ScriptEventPanel::ScriptEventPanel(wxWindow *parent, MissionTreeItemData *item)
   : GmatPanel(parent)
{
   theItem = item;
   theCommand = item->GetCommand();

   #if DBGLVL_SCRIPTEVENTPANEL
   ShowCommand("ScriptEventPanel() theCommand=", theCommand);
   #endif
   
   mPrevCommand = NULL;
   mNextCommand = NULL;
   mNewCommand = NULL;
   
   if (theCommand != NULL)
   {
     Create();
     Show();
   }
}


//------------------------------------------------------------------------------
// ~ScriptEventPanel()
//------------------------------------------------------------------------------
/**
 * A destructor.
 *
 * If new command sequence was created, it replaces the old sequence of commands
 * with new sequence via command->ForceSetNext() and command->ForceSetPrevious().
 */
//------------------------------------------------------------------------------
ScriptEventPanel::~ScriptEventPanel()
{
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
   
   mFileContentsTextCtrl->SetFont( GmatAppData::GetFont() );
   
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
   
   #if DBGLVL_SCRIPTEVENTPANEL_LOAD
   MessageInterface::ShowMessage("ScriptEventPanel::LoadData()\n");
   MessageInterface::ShowMessage("     theCommand=%s\n", theCommand->GetTypeName().c_str());
   std::string cmdString = GmatCommandUtil::GetCommandSeqString(theCommand);
   MessageInterface::ShowMessage("     theCommand seuqence=%s\n", cmdString.c_str());
   
   GmatCommand *firstCmd = theGuiInterpreter->GetFirstCommand();
   cmdString = GmatCommandUtil::GetCommandSeqString(firstCmd);
   MessageInterface::ShowMessage("===> mission seuqence=%s\n", cmdString.c_str());

   mPrevCommand = theCommand->GetPrevious();
   ShowCommand("     mPrevCommand = ", mPrevCommand);
   #endif
   
   std::stringstream text;
   text << theCommand->GetGeneratingString();
   
   wxString scriptText = text.str().c_str();
   mFileContentsTextCtrl->AppendText(scriptText);
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/*
 * In order for ScriptEvent to save new scripts, it creates new sequence of
 * commands and replace the old sequence of commands via command->ForceSetNext()
 * and command->ForceSetPrevious().
 */
//------------------------------------------------------------------------------
void ScriptEventPanel::SaveData()
{
   #if DBGLVL_SCRIPTEVENTPANEL_SAVE
   MessageInterface::ShowMessage("===> ScriptEventPanel::SaveData() Entered\n");
   #endif
   
   canClose = false;
   std::stringstream scriptText1;
   
   //-----------------------------------------------------------------
   // Get text lines and do some sanity checking
   //-----------------------------------------------------------------
   int numLines = mFileContentsTextCtrl->GetNumberOfLines();
   int beginCount = 0;
   int endCount = 0;
   std::string::size_type commentIndex = std::string::npos;
   std::string::size_type beginIndex = std::string::npos;
   std::string::size_type endIndex = std::string::npos;
   wxString line;
   
   for (int i=0; i<numLines; i++)
   {
      line = mFileContentsTextCtrl->GetLineText(i);

      scriptText1 << line << "\n";
      commentIndex = line.Find("%");
      beginIndex = line.Find("BeginScript");
      endIndex = line.Find("EndScript");
      
      #if DBGLVL_SCRIPTEVENTPANEL_SAVE > 1
      MessageInterface::ShowMessage
         ("     line=<%s>, commentIndex=%u, beginIndex=%u, endIndex=%u\n",
          line.c_str(), commentIndex, beginIndex, endIndex);
      #endif
      
      if (beginIndex == line.npos && endIndex == line.npos)
         continue;
      
      if (commentIndex == line.npos)
      {
         if (beginIndex != line.npos)
            beginCount++;
         else if (endIndex != line.npos)
            endCount++;
      }
      else
      {
         if (beginIndex < commentIndex)
            beginCount++;
         else if (endIndex < commentIndex)
            endCount++;            
      }
   }
   
   #if DBGLVL_SCRIPTEVENTPANEL_SAVE > 1
   MessageInterface::ShowMessage
      ("     beginCount=%d, endCount=%d\n", beginCount, endCount);
   #endif
   
   // Check for Begin/EndScript keyword
   if (beginCount != endCount)
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "Unbalanced \"Begin/EndScript\" found in this "
          "script block.\nScript not saved.\n");
      
      return;
   }
   
   
   //-----------------------------------------------------------------
   // Get previous command
   //-----------------------------------------------------------------
   std::string cmdString = GmatCommandUtil::GetCommandSeqString(theCommand);
   
   #if DBGLVL_SCRIPTEVENTPANEL_SAVE
   MessageInterface::ShowMessage
      ("     ==> Current BeginScript sequence=%s\n", cmdString.c_str());
   #endif
   
   // Get previous command
   mPrevCommand = theCommand->GetPrevious();
   
   #if DBGLVL_SCRIPTEVENTPANEL_SAVE
   GmatCommand *firstCmd = theGuiInterpreter->GetFirstCommand();
   ShowCommand("     mPrevCommand = ", mPrevCommand);
   ShowCommand("     theCommand->GetPrevious() = ", theCommand->GetPrevious());   
   #endif
   
   //-----------------------------------------------------------------
   // If previous command is NULL, just return
   //-----------------------------------------------------------------
   if (mPrevCommand == NULL)
   {      
      #if DBGLVL_SCRIPTEVENTPANEL_SAVE
      GmatCommand *firstCmd = theGuiInterpreter->GetFirstCommand();
      ShowCommand("     firstCmd = ", firstCmd);
      #endif
      
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "ScriptEventPanal::SaveData() *** Internal Error "
          "Occurred ***\nthe previous command is empty. Cannot continue.\n");
      
      return;
   }
   
   #if DBGLVL_SCRIPTEVENTPANEL_SAVE
   ShowCommand("     mPrevCommand = ", mPrevCommand);
   #endif
   
   //-----------------------------------------------------------------
   // Create temporary NoOp command, so that commands can be appended
   //-----------------------------------------------------------------
   GmatCommand *tempNoOp = new NoOp;
   GmatCommand *inCommand = tempNoOp;
   
   try
   {
      //--------------------------------------------------------------
      // Set text lines to istringstream and interpret
      //--------------------------------------------------------------
      #if DBGLVL_SCRIPTEVENTPANEL_SAVE
      MessageInterface::ShowMessage
         ("     scriptText1 =\n%s\n", scriptText1.str().c_str());
      #endif
      
      std::istringstream *inStringStream = new std::istringstream;
      inStringStream->str(scriptText1.str());
      
      #if DBGLVL_SCRIPTEVENTPANEL_SAVE
      MessageInterface::ShowMessage
         ("====================> ScriptEvent calling "
          "theGuiInterpreter->Interpret(%p)\n", inCommand);
      #endif
      
      canClose = theGuiInterpreter->Interpret(inCommand, inStringStream);
      
      
      #if DBGLVL_SCRIPTEVENTPANEL_SAVE
      ShowCommand("     inCommand = ", inCommand);
      cmdString = GmatCommandUtil::GetCommandSeqString(inCommand);
      MessageInterface::ShowMessage
         ("     ==> temp noOp seuqence=%s\n", cmdString.c_str());
      #endif
      
      //--------------------------------------------------------------
      // If error occurred during interpretation, handle and return
      //--------------------------------------------------------------
      if (!canClose)
      {         
         MessageInterface::PopupMessage
            (Gmat::ERROR_,
             "Error parsing the ScriptEvent; Script not saved.\n"
             "Please correct the text.\n");
         
         #if DBGLVL_SCRIPTEVENTPANEL_SAVE
         MessageInterface::ShowMessage
            ("     Deleting tempNoOp->GetNext()=%p\n", tempNoOp->GetNext());
         #endif

         //--------------------------------------------
         // Delete commands appended to BeginScript
         //--------------------------------------------
         GmatCommand *temp =
            theGuiInterpreter->DeleteCommand(tempNoOp->GetNext());
         
         if (temp != NULL)
         {
            temp->ForceSetNext(NULL);
            delete temp;
            temp = NULL;
         }
         
         #if DBGLVL_SCRIPTEVENTPANEL_SAVE
         MessageInterface::ShowMessage
            ("     New Script not saved. Deleting tempNoOp=%p\n", tempNoOp);
         #endif
         
         if (tempNoOp != NULL)
         {
            tempNoOp->ForceSetNext(NULL);
            delete tempNoOp;
            tempNoOp = NULL;
         }
         
         return;
      }
      
      //--------------------------------------------------------------
      // Everthing is good to go, delete temporary NoOp
      //--------------------------------------------------------------
      mNewCommand = tempNoOp->GetNext();
      
      #if DBGLVL_SCRIPTEVENTPANEL_SAVE
      ShowCommand("     saving  mNewCommand = ", mNewCommand);
      ShowCommand("     deleting tempNoOp   = ", tempNoOp);
      #endif
      
      tempNoOp->ForceSetNext(NULL);
      delete tempNoOp;
      tempNoOp = NULL;
      
      //--------------------------------------------------------------
      // Replace current ScrptEvent with new one
      //--------------------------------------------------------------
      ReplaceScriptEvent();
      mObject = mNewCommand;
      theCommand = mNewCommand;
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_,
         "Error parsing the ScriptEvent; please correct the text");
      canClose = false;
   }
}


//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ScriptEventPanel::OnTextUpdate(wxCommandEvent& event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void ReplaceScriptEvent()
//------------------------------------------------------------------------------
/**
 * If new command sequence was created, it replaces the old sequence of commands
 * with new sequence via command->ForceSetNext() and command->ForceSetPrevious().
 */
//------------------------------------------------------------------------------
void ScriptEventPanel::ReplaceScriptEvent()
{
   #if DBGLVL_SCRIPTEVENTPANEL_REPLACE_CMDS
   MessageInterface::ShowMessage("ScriptEventPanel::ReplaceScriptEvent() Entered\n");
   ShowCommand("     theCommand   = ", theCommand);
   ShowCommand("     mNewCommand  = ", mNewCommand);
   ShowCommand("     mPrevCommand = ", mPrevCommand);
   #endif
   
   if (theCommand != NULL && mPrevCommand != NULL && mNewCommand != NULL)
   {
      // Get first command from the mission sequence
      GmatCommand *first = theGuiInterpreter->GetFirstCommand();
      // Get parent of current ScriptEvent
      GmatCommand *parent = GmatCommandUtil::GetParentCommand(first, theCommand);
      // Find matching EndScript for new ScriptEvent
      GmatCommand *endScript = GmatCommandUtil::GetMatchingEnd(mNewCommand);
      
      if (parent == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "ScriptEventPanel::ReplaceScriptEvent() "
             "*** INTERNAL ERROR ***  parent is NULL\n");
         return;
      }
      
      #if DBGLVL_SCRIPTEVENTPANEL_REPLACE_CMDS
      ShowCommand("     first     = ", first);
      ShowCommand("     parent    = ", parent);
      ShowCommand("     endScript = ", endScript);
      #endif
      
      // If endScript found continue, otherwise internal error found      
      if (endScript)
      {
         //-----------------------------------------------------------
         // Get command after EndScript from current ScriptEvent
         //-----------------------------------------------------------
         mNextCommand = GmatCommandUtil::GetNextCommand(theCommand);
         
         #if DBGLVL_SCRIPTEVENTPANEL_REPLACE_CMDS
         ShowCommand("     mNextCommand = ", mNextCommand);
         #endif
         
         //-----------------------------------------------------------
         // Delete old ScriptEvent
         //-----------------------------------------------------------
         #if DBGLVL_SCRIPTEVENTPANEL_REPLACE_CMDS
         ShowCommand("     ==> calling gui->DeleteCommand() ", theCommand);
         #endif
         
         //Note: DeleteCommand() does not delete theCommand
         GmatCommand *temp = theGuiInterpreter->DeleteCommand(theCommand);
         
         if (temp != NULL)
         {
            temp->ForceSetNext(NULL);
            delete temp;
            temp = NULL;
         }
         
         #if DBGLVL_SCRIPTEVENTPANEL_REPLACE_CMDS
         MessageInterface::ShowMessage
            ("     ==> after Calling gui->DeleteCommand()\n");
         #endif
         
         //-----------------------------------------------------------
         // If this ScriptEvent is within BranchCommand, Insert Command
         //-----------------------------------------------------------
         if (parent->IsOfType("BranchCommand"))
         {
            #if DBGLVL_SCRIPTEVENTPANEL_REPLACE_CMDS
            ShowCommand("     insert commands to BranchCommand = ", parent);
            #endif
            
            GmatCommand *prevCmd = mPrevCommand;
            GmatCommand *current = mNewCommand;
            GmatCommand *next = NULL;
            
            // if previous command is BranchCommand or BeginScript,
            // find matching end command, and set it to previous command
            GmatCommand *realPrevCmd = prevCmd;
            if (prevCmd->GetTypeName() == "BeginScript")
            {
               realPrevCmd = GmatCommandUtil::GetMatchingEnd(prevCmd);
               
               #if DBGLVL_SCRIPTEVENTPANEL_REPLACE_CMDS
               ShowCommand("     realPrevCmd = ", realPrevCmd);
               #endif
               
               prevCmd = realPrevCmd;
            }
            
            // insert commands in the new script event
            while (current != mNextCommand && current != NULL && prevCmd != NULL)
            {
               next = current->GetNext();
               
               #if DBGLVL_SCRIPTEVENTPANEL_REPLACE_CMDS
               ShowCommand("     Inserting current = ", current);
               ShowCommand("     after     prevCmd = ", prevCmd);
               ShowCommand("               next    = ", next);
               #endif
               
               current->ForceSetNext(NULL);
               
               #if DBGLVL_SCRIPTEVENTPANEL_REPLACE_CMDS
               MessageInterface::ShowMessage("     ==> calling gui->InsertCommand()\n");
               #endif
               
               realPrevCmd = prevCmd;
               
               //----------------------------------------------------------
               // Handle nested BranchCommand inside Begin/EndScript.
               // Since Insert BranchCommand inserts whole branch to command
               // sequence, if previous command is BranchCommand, and current
               // command is not BeginScript, find matching End BranchCommand
               // and insert after it.
               //----------------------------------------------------------
               if ((prevCmd->IsOfType("BranchCommand") &&
                    current->GetTypeName() != "BeginScript"))
               {
                  realPrevCmd = GmatCommandUtil::GetMatchingEnd(prevCmd);
                  
                  #if DBGLVL_SCRIPTEVENTPANEL_REPLACE_CMDS
                  ShowCommand("     realPrevCmd = ", realPrevCmd);
                  #endif

                  prevCmd = realPrevCmd;
               }
               
               theGuiInterpreter->InsertCommand(current, prevCmd);               
               
               #if DBGLVL_SCRIPTEVENTPANEL_REPLACE_CMDS
               MessageInterface::ShowMessage
                  ("     ==> after calling gui->InsertCommand()\n");
               ShowCommand("     prevCmd->next = ", prevCmd->GetNext());
               
               std::string cmdString = GmatCommandUtil::GetCommandSeqString(first);
               MessageInterface::ShowMessage
                  ("===> new sequence=%s\n", cmdString.c_str());
               #endif
               
               prevCmd = current;
               current = next;
               
               #if DBGLVL_SCRIPTEVENTPANEL_REPLACE_CMDS
               ShowCommand("     Inserting current = ", current);
               ShowCommand("     after     prevCmd = ", prevCmd);
               #endif
            } // end while
            
            mNewCommand->ForceSetPrevious(mPrevCommand);
         }
         //-----------------------------------------------------------
         // Else, just set previous/next pointer
         //-----------------------------------------------------------
         else
         {
            // Set previous/next command of BeginScript
            mPrevCommand->ForceSetNext(mNewCommand);
            mNewCommand->ForceSetPrevious(mPrevCommand);
            
            // Set previous/next command of EndScript
            if (mNextCommand != NULL)
               mNextCommand->ForceSetPrevious(endScript);
            
            endScript->ForceSetNext(mNextCommand);            
         }
         
         //-----------------------------------------------------------
         // Set new ScriptEvent to item command
         //-----------------------------------------------------------
         theItem->SetCommand(mNewCommand);
         
         //-----------------------------------------------------------
         // Some sanity check
         //-----------------------------------------------------------
         if (mNewCommand->GetPrevious() != mPrevCommand)
         {
            MessageInterface::PopupMessage
               (Gmat::ERROR_, "ScriptEventPanel::ReplaceScriptEvent() "
                "*** INTERNAL ERROR ***  \nmNewCommand->GetPrevious() != "
                "mPrevCommand\n");
            //ShowCommand("mNewCommand->GetPrevious() = ", mNewCommand->GetPrevious());
            ShowCommand("mPrevCommand = ", mPrevCommand);
         }
         
         if (mNewCommand->GetNext() == NULL)
         {
            MessageInterface::PopupMessage
               (Gmat::ERROR_, "ScriptEventPanel::ReplaceScriptEvent() "
                "*** INTERNAL ERROR ***  mNewCommand->GetNext() == NULL\n");
         }
         else
         {
            #if DBGLVL_SCRIPTEVENTPANEL_REPLACE_CMDS
            std::string cmdString = GmatCommandUtil::GetCommandSeqString(first);    
            MessageInterface::ShowMessage
               ("===> final new sequence=%s\n", cmdString.c_str());
            #endif
         }
      }
      else
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "ScriptEventPanel::ReplaceScriptEvent() "
             "*** INTERNAL ERROR ***  last command should not be NULL\n");
      }
   }
   
   #if DBGLVL_SCRIPTEVENTPANEL_REPLACE_CMDS
   MessageInterface::ShowMessage
      ("ScriptEventPanel::ReplaceScriptEvent() Leaving\n\n");
   #endif
}


//------------------------------------------------------------------------------
// void ShowCommand(const std::string &title1, GmatCommand *cmd1,
//                   const std::string &title2, GmatCommand *cmd2)
//------------------------------------------------------------------------------
void ScriptEventPanel::ShowCommand(const std::string &title1, GmatCommand *cmd1,
                                   const std::string &title2, GmatCommand *cmd2)
{
   if (title2 == "")
   {
      if (cmd1 == NULL)
         MessageInterface::ShowMessage("%s(%p)NULL\n", title1.c_str(), cmd1);
      else
         MessageInterface::ShowMessage
            ("%s(%p)%s\n", title1.c_str(), cmd1, cmd1->GetTypeName().c_str());
   }
   else
   {
      if (cmd2 == NULL)
         MessageInterface::ShowMessage
            ("%s(%p)NULL%s(%p)NULL\n", title1.c_str(), cmd1, title2.c_str(), cmd2);
      else
         MessageInterface::ShowMessage
            ("%s(%p)%s%s(%p)%s\n", title1.c_str(), cmd1, cmd1->GetTypeName().c_str(),
             title2.c_str(), cmd2, cmd2->GetTypeName().c_str());
   }
}

