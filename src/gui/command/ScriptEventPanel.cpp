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


//#define DEBUG_SCRIPTEVENT_PANEL 1
//#define DEBUG_SCRIPTEVENT_PANEL_SAVE 2
//#define DEBUG_SCRIPTEVENT_PANEL_EXIT 1

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
   //theCommand = *cmd;
   theCommand = item->GetCommand();

   #if DEBUG_SCRIPTEVENT_PANEL
   MessageInterface::ShowMessage("ScriptEventPanel() theCommand=%p\n", theCommand);
   if (theCommand != NULL)
      MessageInterface::ShowMessage("   theCommand=%s\n", theCommand->GetTypeName().c_str());
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
 */
//------------------------------------------------------------------------------
ScriptEventPanel::~ScriptEventPanel()
{
   #if DEBUG_SCRIPTEVENT_PANEL_EXIT
   MessageInterface::ShowMessage("~ScriptEventPanel() Entered\n");
   if (theCommand == NULL)
      MessageInterface::ShowMessage("   (%p)theCommand=NULL\n", theCommand);
   else
      MessageInterface::ShowMessage
         ("   theCommand=%s\n", theCommand->GetTypeName().c_str());
   
   if (mNewCommand == NULL)
      MessageInterface::ShowMessage("   (%p)mNewCommand=NULL\n", mNewCommand);
   else
      MessageInterface::ShowMessage
         ("   mNewCommand=%s\n", mNewCommand->GetTypeName().c_str());
   
   if (mPrevCommand == NULL)
      MessageInterface::ShowMessage("   (%p)mPrevCommand=NULL\n", mPrevCommand);
   else
      MessageInterface::ShowMessage
         ("   mPrevCommand=%s\n", mPrevCommand->GetTypeName().c_str());
   #endif
   
   if (theCommand != NULL && mPrevCommand != NULL && mNewCommand != NULL)
   {      
      mPrevCommand->ForceSetNext(mNewCommand);
      mNewCommand->ForceSetPrevious(mPrevCommand);
      GmatCommand *lastCommand = GmatCommandUtil::GetLastCommand(mNewCommand);
      
      if (lastCommand)
      {
         mNextCommand = GmatCommandUtil::GetNextCommand(theCommand);
         
         #if DEBUG_SCRIPTEVENT_PANEL_EXIT
         MessageInterface::ShowMessage
            ("   lastCommand=(%p)%s\n", lastCommand, lastCommand->GetTypeName().c_str());
         if (mNextCommand == NULL)
            MessageInterface::ShowMessage("   (%p)mNextCommand=NULL\n", mNextCommand);
         else
            MessageInterface::ShowMessage
               ("   mNextCommand=%s\n", mNextCommand->GetTypeName().c_str());
         #endif
         
         if (mNextCommand != NULL)
            mNextCommand->ForceSetPrevious(lastCommand);
         
         lastCommand->ForceSetNext(mNextCommand);
         theItem->SetCommand(mNewCommand);
         
         // Delete current BeginScript
         #if DEBUG_SCRIPTEVENT_PANEL_EXIT
         MessageInterface::ShowMessage
            ("   Deleting theCommand=%s, addr=%p\n",
             theCommand->GetTypeName().c_str(), theCommand);
         #endif
         
         GmatCommand *temp = theGuiInterpreter->DeleteCommand(theCommand);
         if (temp != NULL)
         {
            temp->ForceSetNext(NULL);
            delete temp;
            temp = NULL;
         }
         
         if (mNewCommand->GetPrevious() != mPrevCommand)
         {
            MessageInterface::ShowMessage
               ("~ScriptEventPanel() *** INTERNAL ERROR ***  "
                "mNewCommand->GetPrevious() != mPrevCommand\n");
         }
         
         if (mNewCommand->GetNext() == NULL)
         {
            MessageInterface::ShowMessage
               ("~ScriptEventPanel() *** INTERNAL ERROR ***  "
                "mNewCommand->GetNext() == NULL\n");
         }
         else
         {
            #if DEBUG_SCRIPTEVENT_PANEL_EXIT
            MessageInterface::ShowMessage
               ("   mNewCommand->GetNext()=(%p)%s\n", mNewCommand->GetNext(),
                mNewCommand->GetNext()->GetTypeName().c_str());            
            std::string cmdString = GmatCommandUtil::GetCommandSeqString(mNewCommand);            
            MessageInterface::ShowMessage
               ("===> mNewCommand sequence=%s\n", cmdString.c_str());
            #endif
         }
      }
      else
      {
         MessageInterface::ShowMessage
            ("~ScriptEventPanel() *** INTERNAL ERROR ***  "
             "lastCommand should not be NULL\n");
      }
   }
   
   #if DEBUG_SCRIPTEVENT_PANEL_EXIT
   MessageInterface::ShowMessage("~ScriptEventPanel() Leaving\n\n");
   #endif
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
   #if DEBUG_SCRIPTEVENT_PANEL
   MessageInterface::ShowMessage("===> ScriptEventPanel::LoadData()\n");
   #endif
   
   // Set the pointer for the "Show Script" button
   mObject = theCommand;

   std::stringstream text;
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
   #if DEBUG_SCRIPTEVENT_PANEL_SAVE
   MessageInterface::ShowMessage("ScriptEventPanel::SaveData() Entered\n");
   #endif
   
   canClose = false;
   std::stringstream scriptText1;
   
   int numLines = mFileContentsTextCtrl->GetNumberOfLines();
   int commentIndex = -1;
   int beginIndex = -1;
   int endIndex = -1;
   int beginCount = 0;
   int endCount = 0;
   wxString line;
   
   for (int i=0; i<numLines; i++)
   {
      line = mFileContentsTextCtrl->GetLineText(i);
      scriptText1 << line << "\n";
      commentIndex = line.Find("%");
      beginIndex = line.Find("BeginScript");
      endIndex = line.Find("EndScript");
      
      #if DEBUG_SCRIPTEVENT_PANEL_SAVE > 1
      MessageInterface::ShowMessage
         ("===> line=<%s>, commentIndex=%d, beginIndex=%d, endIndex=%d\n",
          line.c_str(), commentIndex, beginIndex, endIndex);
      #endif
      
      if (beginIndex == -1 && endIndex == -1)
         continue;

      if (commentIndex == -1)
      {
         if (beginIndex > -1)
            beginCount++;
         else if (endIndex > -1)
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
   
   #if DEBUG_SCRIPTEVENT_PANEL_SAVE > 1
   MessageInterface::ShowMessage
      ("===> beginCount=%d, endCount=%d\n", beginCount, endCount);
   #endif
   
   // Check for Begin/EndScript keyword
   if (beginCount == 0 || endCount == 0)
   {
      if (beginCount == 0)
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "\"BeginScript\" not found in this script block\n");
      
      if (endCount == 0)
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "\"EndScript\" not found in this script block\n");
      return;
   }
   
   if (beginCount > 1 || endCount > 1)
   {
      if (beginCount > 1 )
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "Multiple \"BeginScript\" found in this script block\n");
   
      if (endCount > 1)
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "Multiple \"EndScript\" found in this script block\n");
      
      return;
   }
   
   std::string cmdString = GmatCommandUtil::GetCommandSeqString(theCommand);
   
   #if DEBUG_SCRIPTEVENT_PANEL_SAVE
   MessageInterface::ShowMessage
      ("===> Current BeginScript sequence=%s\n", cmdString.c_str());
   #endif
   
   // Get previous command
   mPrevCommand = theCommand->GetPrevious();
   #if DEBUG_SCRIPTEVENT_PANEL_SAVE
   MessageInterface::ShowMessage("===> mPrevCommand=%p\n", mPrevCommand);
   #endif
   
   if (mPrevCommand == NULL)
   {      
      #if DEBUG_SCRIPTEVENT_PANEL_SAVE
      GmatCommand *firstCmd = theGuiInterpreter->GetFirstCommand();
      if (firstCmd)
         MessageInterface::ShowMessage
            ("===> first command=%s, addr=%p\n", firstCmd->GetTypeName().c_str(),
             firstCmd);
      else
         MessageInterface::ShowMessage("===> first command is NULL\n");
      #endif
      
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "*** Internal Error Occurred ***\n"
          "The previous command is empty. Cannot continue.\n");
      
      return;
   }
   
   #if DEBUG_SCRIPTEVENT_PANEL_SAVE
   MessageInterface::ShowMessage
      ("===> mPrevCommand=%s\n", mPrevCommand->GetTypeName().c_str());
   #endif
   
   GmatCommand *tempNoOp = new NoOp;
   GmatCommand *inCommand = tempNoOp;
   
   //
   try
   {
      #if DEBUG_SCRIPTEVENT_PANEL_SAVE
      MessageInterface::ShowMessage
         ("===> scriptText1 =\n%s\n", scriptText1.str().c_str());
      #endif
      
      std::istringstream *inStringStream = new std::istringstream;
      inStringStream->str(scriptText1.str());
      
      #if DEBUG_SCRIPTEVENT_PANEL_SAVE
      MessageInterface::ShowMessage
         ("====================> ScriptEvent calling "
          "theGuiInterpreter->Interpret(%p)\n", inCommand);
      #endif
      
      canClose = theGuiInterpreter->Interpret(inCommand, inStringStream);
      
      
      #if DEBUG_SCRIPTEVENT_PANEL_SAVE
      MessageInterface::ShowMessage("===> inCommand=%s\n", inCommand->GetTypeName().c_str());
      cmdString = GmatCommandUtil::GetCommandSeqString(inCommand);
      MessageInterface::ShowMessage("===> temp noOp seuqence=%s\n", cmdString.c_str());
      #endif
      
      if (!canClose)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_,
             "Error parsing the ScriptEvent; Script not saved.\n"
             "Please correct the text.\n");
         
         #if DEBUG_SCRIPTEVENT_PANEL_SAVE
         MessageInterface::ShowMessage
            ("===> Deleting tempNoOp->GetNext()=%p\n", tempNoOp->GetNext());
         #endif
         
         // Delete commands appended to BeginScript
         GmatCommand *temp =
            theGuiInterpreter->DeleteCommand(tempNoOp->GetNext());
         
         if (temp != NULL)
         {
            delete temp;
            temp = NULL;
         }
                  
         #if DEBUG_SCRIPTEVENT_PANEL_SAVE
         MessageInterface::ShowMessage
            ("===> New Script not saved. Deleting tempNoOp=%p\n", tempNoOp);
         #endif

         if (tempNoOp != NULL)
         {
            tempNoOp->ForceSetNext(NULL);
            delete tempNoOp;
            tempNoOp = NULL;
         }
         
         return;
      }
      
      mNewCommand = tempNoOp->GetNext();
      
      #if DEBUG_SCRIPTEVENT_PANEL_SAVE
      MessageInterface::ShowMessage
         ("===> New Script saved. mNewCommand=%p, Deleting tempNoOp=%p\n",
          mNewCommand, tempNoOp);
      #endif
      
      tempNoOp->ForceSetNext(NULL);
      delete tempNoOp;
      tempNoOp = NULL;
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
// void OnScript()
//------------------------------------------------------------------------------
/**
 * Shows Scripts
 */
//------------------------------------------------------------------------------
void ScriptEventPanel::OnScript(wxCommandEvent &event)
{
   wxString title = "Object Script";
   // open separate window to show scripts?
   if (mObject != NULL)
   {
      title = "Scripting for ";
      title += mObject->GetName().c_str();
   }
   
   if (mNewCommand == NULL)
   {
      ShowScriptDialog ssd(this, -1, title, mObject);
      ssd.ShowModal();
   }
   else
   {
      ShowScriptDialog ssd(this, -1, title, mNewCommand);
      ssd.ShowModal();
   }
}


