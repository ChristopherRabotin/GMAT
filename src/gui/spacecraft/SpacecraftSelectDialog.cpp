//$Header$
//------------------------------------------------------------------------------
//                              SpacecraftSelectDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Implements SpacecraftSelectDialog class. This class shows dialog window where
 * spacecraft can be selected.
 * 
 */
//------------------------------------------------------------------------------

#include "SpacecraftSelectDialog.hpp"
#include "MessageInterface.hpp"

#define DEBUG_SC_DIALOG 0

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(SpacecraftSelectDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatDialog::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, SpacecraftSelectDialog::OnButton)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// SpacecraftSelectDialog(wxWindow *parent, wxArrayString &scsToExclude)
//------------------------------------------------------------------------------
SpacecraftSelectDialog::SpacecraftSelectDialog(wxWindow *parent,
                                               wxArrayString &scSelList,
                                               wxArrayString &scExcList)
   : GmatDialog(parent, -1, wxString(_T("SpacecraftSelectDialog")))
{
   mScNameList.Clear();
   mIsSpacecraftSelected = false;
   mScSelList = scSelList;
   mScExcList = scExcList;
   
   Create();
   Show();
}

//------------------------------------------------------------------------------
// wxArrayString& GetSpacecraftNames()
//------------------------------------------------------------------------------
wxArrayString& SpacecraftSelectDialog::GetSpacecraftNames()
{
   return mScNameList;
}

//------------------------------------------------------------------------------
// bool IsSpacecraftSelected()
//------------------------------------------------------------------------------
bool SpacecraftSelectDialog::IsSpacecraftSelected()
{
   return mIsSpacecraftSelected;
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void SpacecraftSelectDialog::Create()
{
   int bsize = 2; // border size
   int scSelCount = mScSelList.Count();
   wxString emptyList[] = {};
   wxString *tempList = NULL;
   
#if DEBUG_SC_DIALOG
   for (int i=0; i<scSelCount; i++)
   {
      MessageInterface::ShowMessage
         ("SpacecraftSelectDialog::Create() mScExcList[%d]=%s\n",
          i, mScSelList[i].c_str());
   }
#endif
   
   if (scSelCount >0)
   {
      tempList = new wxString[scSelCount];
      for (int i=0; i<scSelCount; i++)
      {
         tempList[i] = mScExcList[i];
      }
   }
   
   //wxStaticText
   wxStaticText *spacecraftStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Available Spacecraft"),
                        wxDefaultPosition, wxDefaultSize, 0 );
    
   wxStaticText *spacecraftSelectStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Spacecraft Selected"),
                        wxDefaultPosition, wxDefaultSize, 0 );
    
   wxStaticText *emptyStaticText =
      new wxStaticText( this, ID_TEXT, wxT("  "),
                        wxDefaultPosition, wxDefaultSize, 0 );
    
   // wxButton
   addSpacecraftButton = new wxButton( this, ID_BUTTON, wxT("->"),
                                       wxDefaultPosition, wxSize(20,20), 0 );
   removeSpacecraftButton = new wxButton( this, ID_BUTTON, wxT("<-"),
                                          wxDefaultPosition, wxSize(20,20), 0 );
   clearSpacecraftButton = new wxButton( this, ID_BUTTON, wxT("<="),
                                         wxDefaultPosition, wxSize(20,20), 0 );
       
   // wxListBox
   scAvailableListBox =
      theGuiManager->GetSpacecraftListBox(this, wxSize(150, 100), mScExcList);

   if (scSelCount > 0)
   {
      scSelectedListBox =
         new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                       wxSize(150, 100), scSelCount, tempList, wxLB_SINGLE);
   }
   else
   {
      scSelectedListBox =
         new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                       wxSize(150, 100), 0, emptyList, wxLB_SINGLE);
   }
   
   // wxSizers
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   wxFlexGridSizer *spacecraftGridSizer = new wxFlexGridSizer(3, 0, 0);
   wxBoxSizer *buttonsBoxSizer = new wxBoxSizer(wxVERTICAL);

   // add buttons to sizer
   buttonsBoxSizer->Add(addSpacecraftButton, 0, wxALIGN_CENTER|wxALL, bsize);
   buttonsBoxSizer->Add(removeSpacecraftButton, 0, wxALIGN_CENTER|wxALL, bsize);
   buttonsBoxSizer->Add(clearSpacecraftButton, 0, wxALIGN_CENTER|wxALL, bsize);
    
   // 1st row
   spacecraftGridSizer->Add(spacecraftStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   spacecraftGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   spacecraftGridSizer->Add(spacecraftSelectStaticText, 0, wxALIGN_CENTER|wxALL, bsize);

   // 2nd row
   spacecraftGridSizer->Add(scAvailableListBox, 0, wxALIGN_CENTER|wxALL, bsize);
   spacecraftGridSizer->Add(buttonsBoxSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   spacecraftGridSizer->Add(scSelectedListBox, 0, wxALIGN_CENTER|wxALL, bsize);
        
   pageBoxSizer->Add( spacecraftGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

   delete tempList;
}

//------------------------------------------------------------------------------
// void OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void SpacecraftSelectDialog::OnButton(wxCommandEvent& event)
{
   if ( event.GetEventObject() == addSpacecraftButton )  
   {
      //-----------------------------------
      // add spacecraft
      //-----------------------------------
      wxString s = scAvailableListBox->GetStringSelection();
      int strId = scSelectedListBox->FindString(s);
        
      // if the string wasn't found in the second list, insert it
      if (strId == wxNOT_FOUND)
      {
         scSelectedListBox->Append(s);
         scSelectedListBox->SetStringSelection(s);
      }
      
      theOkButton->Enable();
   }
   else if ( event.GetEventObject() == removeSpacecraftButton )  
   {
      //-----------------------------------
      // remove spacecraft
      //-----------------------------------
      int sel = scSelectedListBox->GetSelection();
      wxString s = scSelectedListBox->GetStringSelection();
      scSelectedListBox->Delete(sel);
      scAvailableListBox->Append(s);
      scAvailableListBox->SetStringSelection(s);
         
      if (sel-1 < 0)
         scSelectedListBox->SetSelection(0);
      else
         scSelectedListBox->SetSelection(sel-1);
     
      if (scSelectedListBox->GetCount() > 0)
         theOkButton->Enable();
      else
         theOkButton->Disable();
   }
   else if ( event.GetEventObject() == clearSpacecraftButton )  
   {
      //-----------------------------------
      // clear spacecraft
      //-----------------------------------
      for(int i=0; i<scSelectedListBox->GetCount(); i++)
         scAvailableListBox->Append(scSelectedListBox->GetString(i));
      
      scSelectedListBox->Clear();
      theOkButton->Disable();
   }
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void SpacecraftSelectDialog::LoadData()
{
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void SpacecraftSelectDialog::SaveData()
{
#if DEBUG_SC_DIALOG
   MessageInterface::
      ShowMessage("SpacecraftSelectDialog::SaveData() sc count = %d\n",
                  scSelectedListBox->GetCount());
#endif
   
   mScNameList.Clear();
   for(int i=0; i<scSelectedListBox->GetCount(); i++)
   {
      mScNameList.Add(scSelectedListBox->GetString(i));
#if DEBUG_SC_DIALOG
      MessageInterface::
         ShowMessage("SpacecraftSelectDialog::SaveData()name = %s\n",
                     mScNameList[i].c_str());
#endif
   }

   if (scSelectedListBox->GetCount() > 0)
      mIsSpacecraftSelected = true;
   else
      mIsSpacecraftSelected = false;
}

//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void SpacecraftSelectDialog::ResetData()
{
   mIsSpacecraftSelected = false;
}
