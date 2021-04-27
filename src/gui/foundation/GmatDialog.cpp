//$Id$
//------------------------------------------------------------------------------
//                              GmatDialog
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Linda Jun
// Created: 2004/02/02
//
/**
 * Implements GmatDialog class.
 */
//------------------------------------------------------------------------------

#include <wx/utils.h>
#include <wx/config.h>
#include "GmatDialog.hpp"
#include "GmatMainFrame.hpp"       // for GetHelpController(), etc
#include "GmatAppData.hpp"         // for SetIcon(), etc.
#include "FileUtil.hpp"            // for GetPathSeparator(), etc.
#include "StringUtil.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_GMAT_DIALOG_SAVE
//#define DEBUG_GMAT_DIALOG_CLOSE
//#define DEBUG_GMAT_DIALOG_HELP

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(GmatDialog, wxDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON_HELP, GmatDialog::OnHelp)
   EVT_CLOSE(GmatDialog::OnClose)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// GmatDialog(wxWindow *parent, wxWindowID id, const wxString& title,
//            GmatBase *obj = NULL, const wxPoint& pos = wxDefaultPosition,
//            const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
//------------------------------------------------------------------------------
/**
 * Constructs GmatDialog object.
 *
 * @param  parent  parent window
 * @param  id      window id
 * @param  title   window title
 * @param  obj     object to be used for setting values
 *
 */
//------------------------------------------------------------------------------
GmatDialog::GmatDialog(wxWindow *parent, wxWindowID id, const wxString& title,
                       GmatBase *obj, const wxPoint& pos, const wxSize& size,
                       long style)
   : wxDialog(parent, id, title, pos, size, style | wxMAXIMIZE_BOX | wxMINIMIZE_BOX,
              title)
{
   mObject = obj;
   UserInputValidator::SetObject(obj);
   UserInputValidator::SetWindow(this);

   canClose = true;
   mDataChanged = false;
   mDataUpdated = false;
   int borderSize = 2;

   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   UserInputValidator::SetGuiManager(theGuiManager);

   theParent = parent;

   theDialogSizer = new wxBoxSizer(wxVERTICAL);
   theButtonSizer = new wxBoxSizer(wxHORIZONTAL);

   #if __WXMAC__
   theMiddleSizer = new wxBoxSizer(wxVERTICAL);
   theBottomSizer = new wxBoxSizer(wxVERTICAL);
   #else
   wxStaticBox *middleStaticBox = new wxStaticBox(this, -1, wxT(""));
   wxStaticBox *bottomStaticBox = new wxStaticBox(this, -1, wxT(""));
   theMiddleSizer = new wxStaticBoxSizer(middleStaticBox, wxVERTICAL);
   theBottomSizer = new wxStaticBoxSizer(bottomStaticBox, wxVERTICAL);
   #endif

   // create bottom buttons
   theOkButton =
      new wxButton(this, ID_BUTTON_OK, "OK", wxDefaultPosition, wxDefaultSize, 0);

   theCancelButton =
      new wxButton(this, ID_BUTTON_CANCEL, "Cancel", wxDefaultPosition, wxDefaultSize, 0);

   theHelpButton = new wxButton
      (this, ID_BUTTON_HELP, GUI_ACCEL_KEY"Help", wxDefaultPosition, wxDefaultSize, 0);
   theHelpButton->SetToolTip("Panel-specific Help (F1)");

   // adds the buttons to button sizer
   theButtonSizer->Add(0, 1, wxALIGN_LEFT | wxALL);
   theButtonSizer->Add(theOkButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   theButtonSizer->Add(theCancelButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   theButtonSizer->Add(0, 1, wxALIGN_RIGHT | wxALL);
   theButtonSizer->Add(theHelpButton, 0, wxALIGN_RIGHT | wxALL, borderSize);

   theBottomSizer->Add(theButtonSizer, 0, wxALIGN_CENTER | wxALL, borderSize);

   // shortcut keys
   wxAcceleratorEntry entries[2];
   entries[0].Set(wxACCEL_NORMAL,  WXK_F1, ID_BUTTON_HELP);
   entries[1].Set(wxACCEL_CTRL,  (int) 'W', ID_BUTTON_CANCEL);
   wxAcceleratorTable accel(2, entries);
   this->SetAcceleratorTable(accel);

}


//------------------------------------------------------------------------------
// virtual void EnableUpdate(bool enable = true)
//------------------------------------------------------------------------------
void GmatDialog::EnableUpdate(bool enable)
{
   #ifdef DEBUG_GMAT_DIALOG_SAVE
   MessageInterface::ShowMessage
      ("GmatDialog::EnableUpdate() enable=%d\n", enable);
   #endif

   if (enable)
      mDataChanged = true;
   else
      mDataChanged = false;
}


//------------------------------------------------------------------------------
// bool HasDataUpdated
//------------------------------------------------------------------------------
bool GmatDialog::HasDataUpdated()
{
   return mDataUpdated;
}


//------------------------------------------------------------------------------
// void SetCanClose(bool flag)
//------------------------------------------------------------------------------
void GmatDialog::SetCanClose(bool flag)
{
   canClose = flag;
}


//------------------------------------------------------------------------------
// void OnOK()
//------------------------------------------------------------------------------
/**
 * Saves the data and closes the page
 */
//------------------------------------------------------------------------------
void GmatDialog::OnOK(wxCommandEvent &event)
{
   SaveData();

   #ifdef DEBUG_GMAT_DIALOG_SAVE
   MessageInterface::ShowMessage
      ("GmatDialog::OnOK() canClose=%d\n", canClose);
   #endif

   if (canClose)
   {
      mDataChanged = false;
      Close();
   }
}


//------------------------------------------------------------------------------
// void OnCancel()
//------------------------------------------------------------------------------
/**
 * Close page.
 */
//------------------------------------------------------------------------------
void GmatDialog::OnCancel(wxCommandEvent &event)
{
   ResetData();
   mDataChanged = false;
   Close();
}


//------------------------------------------------------------------------------
// void OnHelp()
//------------------------------------------------------------------------------
/**
 * Shows Helps
 */
//------------------------------------------------------------------------------
void GmatDialog::OnHelp(wxCommandEvent &event)
{
   #ifdef DEBUG_GMAT_DIALOG_HELP
   MessageInterface::ShowMessage
      ("GmatDialog::OnHelp() entered, mObject=<%p><%s>'%s'\n", mObject,
       mObject ? mObject->GetTypeName().c_str() : "NULL",
       mObject ? mObject->GetName().c_str() : "NULL");
   #endif
   
   wxString objLink;
   wxString sHTML;
   wxString baseHelpLink;
   char msgBuffer[255];
   
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   pConfig->SetPath(wxT("/Help"));
   if (mObject != NULL)
   {
      objLink = mObject->GetTypeName().c_str();
   }
   else
   {
      wxString prefix = "Scripting for ";
      objLink = GetName().c_str();
      if (objLink.Find(prefix) != wxNOT_FOUND)
		objLink = objLink.Mid(prefix.size());
      GmatBase *obj = theGuiInterpreter->GetConfiguredObject(objLink.c_str());
      if (obj != NULL)
         objLink = obj->GetTypeName().c_str();
   }
   
   wxHelpController *theHelpController = GmatAppData::Instance()->GetMainFrame()->GetHelpController();
	if (theHelpController != NULL)
	{
		#ifdef DEBUG_GMAT_DIALOG_HELP
		MessageInterface::ShowMessage
			("GmatPanel::OnHelp() theHelpController=<%p>\n   "
			"File to display=%s\n", theHelpController,
			s);
		#endif
		// displays chm, not html
		// see if there is an override for panel (e.g., PropSetupKeyword=Propagator)
		sHTML = objLink+".html";
		objLink = pConfig->Read(objLink+"Keyword", sHTML);

      // work around for wxWidgets bug: http://trac.wxwidgets.org/ticket/14888
      // chm help fails for Windows 8,10.
      // solution taken from: https://stackoverflow.com/questions/29944745/

      bool useHelpController = true;

      #ifdef _WIN32  // All Win platforms define this, even when 64-bit       
            //For Windows 8 is dwMajorVersion = 6, dwMinorVersion = 2.
            OSVERSIONINFOEX info;
            ZeroMemory(&info, sizeof(OSVERSIONINFOEX));
            info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
            GetVersionEx((LPOSVERSIONINFO)&info);//info requires typecasting

            Real winVersion = (Real)info.dwMajorVersion + (Real)info.dwMinorVersion / 10.0;
            if (winVersion > 6.1)
               useHelpController = false;
      #endif

      if (useHelpController)
         theHelpController->DisplaySection(objLink);
      else
         theHelpController->DisplayContents();
	}
	else
	{
	   // get base help link if available
      baseHelpLink = pConfig->Read(_T("BaseHelpLink"),_T("http://gmat.sourceforge.net/docs/latest/html/%s.html"));
	   sprintf( msgBuffer, baseHelpLink.c_str(), objLink.WX_TO_C_STRING);
      
	   #ifdef DEBUG_GMAT_DIALOG_HELP
	   MessageInterface::ShowMessage
		  ("   objLink = '%s', baseHelpLink = '%s'\n   helpLink = '%s'\n",
		   objLink.c_str(), baseHelpLink.c_str(), msgBuffer);
	   #endif
      
	   // open separate window to show help
	   objLink = pConfig->Read(objLink, wxString(&msgBuffer[0]));
	   #ifdef DEBUG_GMAT_DIALOG_HELP
	   MessageInterface::ShowMessage("   actual help Link = '%s'\n", objLink.c_str());
	   #endif
      
      // if path is relative, try to append it to gmat root 
	   if (GmatFileUtil::IsPathRelative(objLink.c_str()))
	   {
			FileManager *fm = FileManager::Instance();
			if (GmatStringUtil::EndsWithPathSeparator(fm->GetRootPath()))
				objLink = fm->GetRootPath().c_str() + objLink;
			else
			{
			   wxString pathSep = GmatFileUtil::GetPathSeparator().c_str();
				objLink = fm->GetRootPath().c_str() + pathSep + objLink;
			}

	   }
	   wxLaunchDefaultBrowser(objLink);
	}
   
   #ifdef DEBUG_GMAT_DIALOG_HELP
   MessageInterface::ShowMessage("GmatDialog::OnHelp() leaving\n");
   #endif

}


//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent &event)
//------------------------------------------------------------------------------
void GmatDialog::OnClose(wxCloseEvent &event)
{
   #ifdef DEBUG_GMAT_DIALOG_CLOSE
   MessageInterface::ShowMessage
      ("GmatDialog::OnClose() mDataChanged=%d\n", mDataChanged);
   #endif

   if (mDataChanged)
   {
      if ( wxMessageBox(_T("Changes will be lost. \nDo you really want to close?"),
                        _T("Please confirm"), wxICON_QUESTION | wxYES_NO) != wxYES )
      {
         event.Veto();
         return;
      }
   }

   event.Skip();
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void ShowData()
//------------------------------------------------------------------------------
/**
 * Shows the dialog.
 */
//------------------------------------------------------------------------------
void GmatDialog::ShowData()
{
   // add items to middle sizer

   theDialogSizer->Add(theMiddleSizer, 1, wxGROW | wxALL, 1);
   theDialogSizer->Add(theBottomSizer, 0, wxGROW | wxALL, 1);

   // tells the enclosing window to adjust to the size of the sizer
   SetAutoLayout(TRUE);
   SetSizer(theDialogSizer); //use the sizer for layout
   theDialogSizer->Fit(this); //loj: if theParent is used it doesn't show the scroll bar
   theDialogSizer->SetSizeHints(this); //set size hints to honour minimum size

   // Set GMAT main icon
   GmatAppData::Instance()->SetIcon(this, "GmatDialog");
   
   CenterOnScreen(wxBOTH);   
   LoadData();
}


