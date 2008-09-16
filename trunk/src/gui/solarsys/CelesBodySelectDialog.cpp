//$Id$
//------------------------------------------------------------------------------
//                              CelesBodySelectDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Implements CelesBodySelectDialog class. This class shows dialog window where
 * celestial bodies can be selected.
 * 
 */
//------------------------------------------------------------------------------

#include "CelesBodySelectDialog.hpp"
#include "ColorTypes.hpp"           // for namespace GmatColor::
#include "MessageInterface.hpp"

#include "wx/colordlg.h"            // for wxColourDialog

//#define DEBUG_CELESBODY_DIALOG 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CelesBodySelectDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, CelesBodySelectDialog::OnButton)
   EVT_BUTTON(ID_COLOR_BUTTON, CelesBodySelectDialog::OnColorButtonClick)
   EVT_LISTBOX(ID_BODY_SEL_LISTBOX, CelesBodySelectDialog::OnSelectBody)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// CelesBodySelectDialog(wxWindow *parent, wxArrayString &bodiesToExclude, ...
//------------------------------------------------------------------------------
CelesBodySelectDialog::CelesBodySelectDialog(wxWindow *parent,
                                             wxArrayString &bodiesToExclude,
                                             wxArrayString &bodiesToHide,
                                             bool showCalPoints)
   : GmatDialog(parent, -1, wxString(_T("CelesBodySelectDialog")))
{
   mBodyNames.Clear();
   mBodiesToHide.Clear();
   
   mBodiesToExclude = bodiesToExclude;
   mBodiesToHide = bodiesToHide;
   
   mIsBodySelected = false;
   mShowCalPoints = showCalPoints;
   
   Create();
   ShowData();
}


//------------------------------------------------------------------------------
// void SetBodyColors(const wxArrayString &bodyNames,
//                    const UnsignedIntArray &bodyColors)
//------------------------------------------------------------------------------
void CelesBodySelectDialog::SetBodyColors(const wxArrayString &bodyNames,
                                          const UnsignedIntArray &bodyColors)
{
   for (unsigned int i=0; i<bodyNames.GetCount(); i++)
   {
      mBodyColorMap[bodyNames[i]] = bodyColors[i];
      
      #if DEBUG_CELESBODY_DIALOG
      MessageInterface::ShowMessage
         ("CelesBodySelectDialog::SetBodyColors() body=%s, color=%d\n",
          bodyNames[i].c_str(), bodyColors[i]);
      #endif
   }

   ShowBodyOption(mBodySelectedListBox->GetStringSelection(), true);
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void CelesBodySelectDialog::Create()
{
   int borderSize = 2;
   wxArrayString emptyList;
   
   // body GridSizer
   wxFlexGridSizer *bodyGridSizer = new wxFlexGridSizer(3, 0, 0);
   
   //-------------------------------------------------------
   // 1st row
   //-------------------------------------------------------
   wxStaticText *bodySelectStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Bodies Selected"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   wxStaticText *bodyStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Available Bodies"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   bodyGridSizer->Add(bodyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   bodyGridSizer->Add(20, 20);
   bodyGridSizer->Add(bodySelectStaticText, 0, wxALIGN_CENTER|wxALL, borderSize);
   
   //-------------------------------------------------------
   // 2nd row
   //-------------------------------------------------------
   
   // availabe celetestial body ListBox
   if (mShowCalPoints)
   {
      mBodyListBox =
         theGuiManager->GetCelestialPointListBox(this, -1, wxSize(150, 200),
                                                 mBodiesToExclude);
   }
   else
   {
      mBodyListBox =
         theGuiManager->GetConfigBodyListBox(this, -1, wxSize(150, 200),
                                             mBodiesToExclude);
   }
   
   // arrow buttons
   mAddBodyButton =
      new wxButton( this, ID_BUTTON, wxT("->"), wxDefaultPosition, wxSize(20,20), 0 );
   mRemoveBodyButton =
      new wxButton( this, ID_BUTTON, wxT("<-"), wxDefaultPosition, wxSize(20,20), 0 );
   mClearBodyButton =
      new wxButton( this, ID_BUTTON, wxT("<="), wxDefaultPosition, wxSize(20,20), 0 );
   mBodyColorButton =
      new wxButton( this, ID_COLOR_BUTTON, wxT(""), wxDefaultPosition, wxSize(20,20), 0 );
   
   // add buttons to sizer
   wxBoxSizer *buttonsBoxSizer = new wxBoxSizer(wxVERTICAL);
   buttonsBoxSizer->Add(mAddBodyButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   buttonsBoxSizer->Add(mRemoveBodyButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   buttonsBoxSizer->Add(mClearBodyButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   
   // selected celetestial body ListBox
   if (! mBodiesToExclude.IsEmpty())
   {
      Integer count = mBodiesToExclude.GetCount();
      wxArrayString selectedBodyList;
      
      for (Integer i = 0; i < count; i++)
         selectedBodyList.Add(mBodiesToExclude[i]);
      
      mBodySelectedListBox =
         new wxListBox(this, ID_BODY_SEL_LISTBOX, wxDefaultPosition,
                       wxSize(150, 200), selectedBodyList, wxLB_SINGLE);
   }
   else
   {
      mBodySelectedListBox =
         new wxListBox(this, ID_BODY_SEL_LISTBOX, wxDefaultPosition, wxSize(150, 200),
                       emptyList, wxLB_SINGLE);
   }
   
   bodyGridSizer->Add(mBodyListBox, 0, wxALIGN_CENTER|wxALL, borderSize);
   bodyGridSizer->Add(buttonsBoxSizer, 0, wxALIGN_CENTER|wxALL, borderSize);
   bodyGridSizer->Add(mBodySelectedListBox, 0, wxALIGN_CENTER|wxALL, borderSize);
   
   //-------------------------------------------------------
   // 3rd row
   //-------------------------------------------------------
   wxStaticText *bodyColorStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Orbit Color"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   bodyGridSizer->Add(bodyColorStaticText, 0, wxALIGN_CENTER|wxALL, borderSize);
   bodyGridSizer->Add(20, 20);
   bodyGridSizer->Add(mBodyColorButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   
   //-------------------------------------------------------
   // add to parent sizer
   //-------------------------------------------------------
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   pageBoxSizer->Add( bodyGridSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
}


//------------------------------------------------------------------------------
// void OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CelesBodySelectDialog::OnButton(wxCommandEvent& event)
{
   if ( event.GetEventObject() == mAddBodyButton )  
   {
      wxString s = mBodyListBox->GetStringSelection();
      
      if (s.IsEmpty())
         return;
      
      int strId1 = mBodyListBox->FindString(s);
      int strId2 = mBodySelectedListBox->FindString(s);
      
      // if the string wasn't found in the second list, insert it
      if (strId2 == wxNOT_FOUND)
      {
         mBodySelectedListBox->Append(s);
         mBodyListBox->Delete(strId1);
         mBodySelectedListBox->SetStringSelection(s);
         
         // select first available body
         mBodyListBox->SetSelection(0);
         
         // show body color
         ShowBodyOption(s, true);
      }
   }
   else if ( event.GetEventObject() == mRemoveBodyButton )  
   {
      wxString s = mBodySelectedListBox->GetStringSelection();
        
      if (s.IsEmpty())
         return;

      #ifdef DEBUG_BODY_REMOVE
      MessageInterface::ShowMessage("Removing body: %s\n", s.c_str());
      #endif
      
      mBodyListBox->Append(s);
      int sel = mBodySelectedListBox->GetSelection();
      mBodySelectedListBox->Delete(sel);
   }
   else if ( event.GetEventObject() == mClearBodyButton )  
   {
      Integer count = mBodySelectedListBox->GetCount();
      
      if (count == 0)
         return;
      
      for (Integer i = 0; i < count; i++)
         mBodyListBox->Append(mBodySelectedListBox->GetString(i));
      
      mBodySelectedListBox->Clear();
   }
   
   theOkButton->Enable();
}


//------------------------------------------------------------------------------
// void OnColorButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CelesBodySelectDialog::OnColorButtonClick(wxCommandEvent& event)
{    
   wxColourData data;
   data.SetColour(mSelBodyColor);

   wxColourDialog dialog(this, &data);
   dialog.Center();
   
   if (dialog.ShowModal() == wxID_OK)
   {
      mSelBodyName = mBodySelectedListBox->GetStringSelection();
      
      mSelBodyColor = dialog.GetColourData().GetColour();
      mBodyColorButton->SetBackgroundColour(mSelBodyColor);
      
      mBodyColorMap[mSelBodyName].Set(mSelBodyColor.Red(),
                                      mSelBodyColor.Green(),
                                      mSelBodyColor.Blue());
      
      #if DEBUG_CELESBODY_DIALOG
      MessageInterface::ShowMessage("OnColorButtonClick() r=%d g=%d b=%d\n",
                                    mSelBodyColor.Red(), mSelBodyColor.Green(),
                                    mSelBodyColor.Blue());

      MessageInterface::ShowMessage("OnColorButtonClick() UnsignedInt=%d\n",
                                    mBodyColorMap[mSelBodyName].GetIntColor());
      #endif
      
      theOkButton->Enable();
   
   }
}


//------------------------------------------------------------------------------
// void OnSelectBody(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CelesBodySelectDialog::OnSelectBody(wxCommandEvent& event)
{
   ShowBodyOption(mBodySelectedListBox->GetStringSelection(), true);
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void CelesBodySelectDialog::LoadData()
{
   if (! mBodiesToHide.IsEmpty())
   {
      // Check mBodyListBox
      for (Integer i = 0; i < (Integer)mBodiesToHide.GetCount(); i++)
      {
         for (Integer j = 0; j < (Integer)mBodyListBox->GetCount(); j++)
         {
            std::string bName = mBodyListBox->GetString(j).c_str();
            if (strcmp(mBodiesToHide[i].c_str(), bName.c_str()) == 0)
               mBodyListBox->Delete(j);
         }
      }
      
      // Check mBodySelectedListBox
      for (Integer i = 0; i < (Integer)mBodiesToHide.GetCount(); i++)
      {
         for (Integer j = 0; j < (Integer)mBodySelectedListBox->GetCount(); j++)
         {
            std::string bName = mBodySelectedListBox->GetString(j).c_str();
            if (strcmp(mBodiesToHide[i].c_str(), bName.c_str()) == 0)
               mBodySelectedListBox->Delete(j);
         }    
      }
   }
   
   // Show body color
   mBodySelectedListBox->SetSelection(0);
   ShowBodyOption(mBodySelectedListBox->GetStringSelection(), true);
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void CelesBodySelectDialog::SaveData()
{
   mBodyNames.Clear();
   for(unsigned int i=0; i<mBodySelectedListBox->GetCount(); i++)
      mBodyNames.Add(mBodySelectedListBox->GetString(i));
   
   mBodyColors.clear();
   for (unsigned int i=0; i<mBodyNames.GetCount(); i++)
      mBodyColors.push_back(mBodyColorMap[mBodyNames[i]].GetIntColor());
   
   mIsBodySelected = true;
   
   #if DEBUG_CELESBODY_DIALOG
   MessageInterface::ShowMessage("CelesBodySelectDialog::SaveData()\n");
   for (unsigned int i=0; i<mBodyNames.GetCount(); i++)
   {
      MessageInterface::ShowMessage("body name=%s, color=%d\n",
                                    mBodyNames[i].c_str(), mBodyColors[i]);
   }
   #endif
}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void CelesBodySelectDialog::ResetData()
{
   mIsBodySelected = false;
}


//------------------------------------------------------------------------------
// void ShowBodyOption(const wxString &name, bool show = true)
//------------------------------------------------------------------------------
void CelesBodySelectDialog::ShowBodyOption(const wxString &name, bool show)
{
   #if DEBUG_CELESBODY_DIALOG
   MessageInterface::ShowMessage
      ("CelesBodySelectDialog::ShowBodyOption() name=%s\n",
       name.c_str());
   #endif
   
   if (!name.IsSameAs(""))
   {
      mSelBodyName = name;

      // if body name not found, insert
      if (mBodyColorMap.find(mSelBodyName) == mBodyColorMap.end())
      {
         mBodyColorMap[mSelBodyName] = RgbColor(GmatColor::L_BROWN32);
      }
      
      RgbColor bodyColor = mBodyColorMap[mSelBodyName];
      
      #if DEBUG_CELESBODY_DIALOG
      MessageInterface::ShowMessage
         ("CelesBodySelectDialog::ShowBodyOption() mSelBodyName=%s bodyColor=%08x\n",
          mSelBodyName.c_str(), bodyColor.GetIntColor());
      #endif
      
      mSelBodyColor.Set(bodyColor.Red(), bodyColor.Green(), bodyColor.Blue());
      mBodyColorButton->SetBackgroundColour(mSelBodyColor);
   }
}



