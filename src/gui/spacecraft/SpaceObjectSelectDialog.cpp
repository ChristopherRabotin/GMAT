//$Header$
//------------------------------------------------------------------------------
//                              SpaceObjectSelectDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Implements SpaceObjectSelectDialog class. This class shows dialog window where
 * spacecraft object can be selected.
 * 
 */
//------------------------------------------------------------------------------

#include "SpaceObjectSelectDialog.hpp"
#include "MessageInterface.hpp"

#define DEBUG_SO_DIALOG 0

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(SpaceObjectSelectDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatDialog::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, SpaceObjectSelectDialog::OnButton)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// SpaceObjectSelectDialog(wxWindow *parent, wxArrayString &soSelList,
//                         wxArrayString &soExcList)
//------------------------------------------------------------------------------
SpaceObjectSelectDialog::SpaceObjectSelectDialog(wxWindow *parent,
                                                 wxArrayString &soSelList,
                                                 wxArrayString &soExcList)
   : GmatDialog(parent, -1, wxString(_T("SpaceObjectSelectDialog")))
{
   mSoNameList.Clear();
   mIsSpaceObjectSelected = false;
   mSoSelList = soSelList;
   mSoExcList = soExcList;
   
   Create();
   Show();
}

//------------------------------------------------------------------------------
// wxArrayString& GetSpaceObjectNames()
//------------------------------------------------------------------------------
wxArrayString& SpaceObjectSelectDialog::GetSpaceObjectNames()
{
   return mSoNameList;
}

//------------------------------------------------------------------------------
// bool IsSpaceObjectSelected()
//------------------------------------------------------------------------------
bool SpaceObjectSelectDialog::IsSpaceObjectSelected()
{
   return mIsSpaceObjectSelected;
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void SpaceObjectSelectDialog::Create()
{
   int bsize = 2; // border size
   int soSelCount = mSoSelList.Count();
   wxString emptyList[] = {};
   wxString *tempList = NULL;
   
#if DEBUG_SO_DIALOG
   for (int i=0; i<soSelCount; i++)
   {
      MessageInterface::ShowMessage
         ("SpaceObjectSelectDialog::Create() mSoExcList[%d]=%s\n",
          i, mSoSelList[i].c_str());
   }
#endif
   
   if (soSelCount > 0)
   {
      tempList = new wxString[soSelCount];
      for (int i=0; i<soSelCount; i++)
      {
         tempList[i] = mSoExcList[i];
      }
   }
   
   //wxStaticText
   wxStaticText *spaceObjStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Available SpaceObject"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   
   wxStaticText *spaceObjSelectStaticText =
      new wxStaticText( this, ID_TEXT, wxT("SpaceObject Selected"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   
   wxStaticText *emptyStaticText =
      new wxStaticText( this, ID_TEXT, wxT("  "),
                        wxDefaultPosition, wxDefaultSize, 0 );
   
   // wxButton
   addSpaceObjectButton = new wxButton( this, ID_BUTTON, wxT("->"),
                                       wxDefaultPosition, wxSize(20,20), 0 );
   removeSpaceObjectButton = new wxButton( this, ID_BUTTON, wxT("<-"),
                                          wxDefaultPosition, wxSize(20,20), 0 );
   clearSpaceObjectButton = new wxButton( this, ID_BUTTON, wxT("<="),
                                         wxDefaultPosition, wxSize(20,20), 0 );

#if DEBUG_SO_DIALOG
   MessageInterface::ShowMessage
      ("SpaceObjectSelectDialog::Create() Calling GetSpaceObjectListBox\n");
#endif
   
   // wxListBox
   spaceObjAvailableListBox =
      theGuiManager->GetSpaceObjectListBox(this, -1, wxSize(150, 100), mSoExcList);
   
   if (soSelCount > 0)
   {
      spaceObjSelectedListBox =
         new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                       wxSize(150, 100), soSelCount, tempList, wxLB_SINGLE);
   }
   else
   {
      spaceObjSelectedListBox =
         new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                       wxSize(150, 100), 0, emptyList, wxLB_SINGLE);
   }
   
   // wxSizers
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   wxFlexGridSizer *spaceObjGridSizer = new wxFlexGridSizer(3, 0, 0);
   wxBoxSizer *buttonsBoxSizer = new wxBoxSizer(wxVERTICAL);
   
   // add buttons to sizer
   buttonsBoxSizer->Add(addSpaceObjectButton, 0, wxALIGN_CENTER|wxALL, bsize);
   buttonsBoxSizer->Add(removeSpaceObjectButton, 0, wxALIGN_CENTER|wxALL, bsize);
   buttonsBoxSizer->Add(clearSpaceObjectButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   // 1st row
   spaceObjGridSizer->Add(spaceObjStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   spaceObjGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   spaceObjGridSizer->Add(spaceObjSelectStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   
   // 2nd row
   spaceObjGridSizer->Add(spaceObjAvailableListBox, 0, wxALIGN_CENTER|wxALL, bsize);
   spaceObjGridSizer->Add(buttonsBoxSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   spaceObjGridSizer->Add(spaceObjSelectedListBox, 0, wxALIGN_CENTER|wxALL, bsize);
   
   pageBoxSizer->Add( spaceObjGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   delete tempList;
}

//------------------------------------------------------------------------------
// void OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void SpaceObjectSelectDialog::OnButton(wxCommandEvent& event)
{
   if ( event.GetEventObject() == addSpaceObjectButton )  
   {
      //-----------------------------------
      // add spacecraft object
      //-----------------------------------
      wxString s = spaceObjAvailableListBox->GetStringSelection();
      int strId = spaceObjSelectedListBox->FindString(s);
        
      // if the string wasn't found in the second list, insert it
      if (strId == wxNOT_FOUND)
      {
         spaceObjSelectedListBox->Append(s);
         spaceObjSelectedListBox->SetStringSelection(s);
         // select next available item (loj: 8/18/04)
         spaceObjAvailableListBox->
            SetSelection(spaceObjAvailableListBox->GetSelection()+1);
      }
      
      theOkButton->Enable();
   }
   else if ( event.GetEventObject() == removeSpaceObjectButton )  
   {
      //-----------------------------------
      // remove spacecraft object
      //-----------------------------------
      int sel = spaceObjSelectedListBox->GetSelection();
      wxString s = spaceObjSelectedListBox->GetStringSelection();
      spaceObjSelectedListBox->Delete(sel);
      spaceObjAvailableListBox->Append(s);
      spaceObjAvailableListBox->SetStringSelection(s);
         
      if (sel-1 < 0)
         spaceObjSelectedListBox->SetSelection(0);
      else
         spaceObjSelectedListBox->SetSelection(sel-1);
     
      if (spaceObjSelectedListBox->GetCount() > 0)
         theOkButton->Enable();
      else
         theOkButton->Disable();
   }
   else if ( event.GetEventObject() == clearSpaceObjectButton )  
   {
      //-----------------------------------
      // clear spacecraft object
      //-----------------------------------
      for(int i=0; i<spaceObjSelectedListBox->GetCount(); i++)
         spaceObjAvailableListBox->Append(spaceObjSelectedListBox->GetString(i));
      
      spaceObjSelectedListBox->Clear();
      theOkButton->Disable();
   }
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void SpaceObjectSelectDialog::LoadData()
{
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void SpaceObjectSelectDialog::SaveData()
{
#if DEBUG_SO_DIALOG
   MessageInterface::
      ShowMessage("SpaceObjectSelectDialog::SaveData() sc count = %d\n",
                  spaceObjSelectedListBox->GetCount());
#endif
   
   mSoNameList.Clear();
   for(int i=0; i<spaceObjSelectedListBox->GetCount(); i++)
   {
      mSoNameList.Add(spaceObjSelectedListBox->GetString(i));
#if DEBUG_SO_DIALOG
      MessageInterface::
         ShowMessage("SpaceObjectSelectDialog::SaveData()name = %s\n",
                     mSoNameList[i].c_str());
#endif
   }

   if (spaceObjSelectedListBox->GetCount() > 0)
      mIsSpaceObjectSelected = true;
   else
      mIsSpaceObjectSelected = false;
}

//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void SpaceObjectSelectDialog::ResetData()
{
   mIsSpaceObjectSelected = false;
}
