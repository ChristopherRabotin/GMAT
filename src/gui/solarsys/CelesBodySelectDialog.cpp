//$Header$
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

   ShowBodyOption(bodySelectedListBox->GetStringSelection(), true);
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void CelesBodySelectDialog::Create()
{
   int borderSize = 2;
   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;

   //wxStaticText
   wxStaticText *bodyStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Available Bodies"),
                        wxDefaultPosition, wxDefaultSize, 0 );
    
   wxStaticText *bodySelectStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Bodies Selected"),
                        wxDefaultPosition, wxDefaultSize, 0 );
    
   wxStaticText *bodyColorStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Orbit Color"),
                        wxDefaultPosition, wxDefaultSize, 0 );
    
   wxStaticText *emptyStaticText =
      new wxStaticText( this, ID_TEXT, wxT("  "),
                        wxDefaultPosition, wxDefaultSize, 0 );
    
   // wxButton
   addBodyButton = new wxButton( this, ID_BUTTON, wxT("->"),
                                 wxDefaultPosition, wxSize(20,20), 0 );
   removeBodyButton = new wxButton( this, ID_BUTTON, wxT("<-"),
                                    wxDefaultPosition, wxSize(20,20), 0 );
   clearBodyButton = new wxButton( this, ID_BUTTON, wxT("<="),
                                   wxDefaultPosition, wxSize(20,20), 0 );
   mBodyColorButton = new wxButton( this, ID_COLOR_BUTTON, wxT(""),
                                   wxDefaultPosition, wxSize(20,20), 0 );
   
   // wxListBox
   if (mShowCalPoints)
   {
      bodyListBox =
         theGuiManager->GetCelestialPointListBox(this, -1, wxSize(150, 200),
                                                 mBodiesToExclude);
   }
   else
   {
      bodyListBox =
         theGuiManager->GetConfigBodyListBox(this, -1, wxSize(150, 200),
                                             mBodiesToExclude);
   }
   
   if (! mBodiesToExclude.IsEmpty())
   {
      Integer count = mBodiesToExclude.GetCount();
      wxString selectedBodyList[MAX_LIST_SIZE];
      for (Integer i = 0; i < count; i++)
      {
         selectedBodyList[i] = mBodiesToExclude[i].c_str(); 
      }   
      
      bodySelectedListBox =
         new wxListBox(this, ID_BODY_SEL_LISTBOX, wxDefaultPosition,
                       wxSize(150, 200), count, selectedBodyList, wxLB_SINGLE);
   }
   else
   {
      bodySelectedListBox =
         new wxListBox(this, ID_BODY_SEL_LISTBOX, wxDefaultPosition, wxSize(150, 200), //0,
                       emptyList, wxLB_SINGLE);
   }
   
   // wxSizers
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   wxFlexGridSizer *bodyGridSizer = new wxFlexGridSizer(3, 0, 0);
   wxBoxSizer *buttonsBoxSizer = new wxBoxSizer(wxVERTICAL);

   // add buttons to sizer
   buttonsBoxSizer->Add(addBodyButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   buttonsBoxSizer->Add(removeBodyButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   buttonsBoxSizer->Add(clearBodyButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   
   // 1st row
   bodyGridSizer->Add(bodyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   bodyGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   bodyGridSizer->Add(bodySelectStaticText, 0, wxALIGN_CENTER|wxALL, borderSize);
   
   // 2nd row
   bodyGridSizer->Add(bodyListBox, 0, wxALIGN_CENTER|wxALL, borderSize);
   bodyGridSizer->Add(buttonsBoxSizer, 0, wxALIGN_CENTER|wxALL, borderSize);
   bodyGridSizer->Add(bodySelectedListBox, 0, wxALIGN_CENTER|wxALL, borderSize);
   
   // 3rd row
   bodyGridSizer->Add(bodyColorStaticText, 0, wxALIGN_CENTER|wxALL, borderSize);
   bodyGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTER|wxALL, borderSize);
   bodyGridSizer->Add(mBodyColorButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   
   pageBoxSizer->Add( bodyGridSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);

   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
}

//------------------------------------------------------------------------------
// void OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CelesBodySelectDialog::OnButton(wxCommandEvent& event)
{    
   if ( event.GetEventObject() == addBodyButton )  
   {
      wxString s = bodyListBox->GetStringSelection();
        
      if (s.IsEmpty())
         return;
           
      int strId1 = bodyListBox->FindString(s);
      int strId2 = bodySelectedListBox->FindString(s);
        
      // if the string wasn't found in the second list, insert it
      if (strId2 == wxNOT_FOUND)
      {
         bodySelectedListBox->Append(s);
         bodyListBox->Delete(strId1);
         bodySelectedListBox->SetStringSelection(s);

         // select first available body
         bodyListBox->SetSelection(0);

         // show body color
         ShowBodyOption(s, true);
      }
   }
   else if ( event.GetEventObject() == removeBodyButton )  
   {
      wxString s = bodySelectedListBox->GetStringSelection();
        
      if (s.IsEmpty())
         return;
      
      //MessageInterface::ShowMessage("Removing body: %s\n", s.c_str());
      bodyListBox->Append(s);
      int sel = bodySelectedListBox->GetSelection();
      bodySelectedListBox->Delete(sel);
   }
   else if ( event.GetEventObject() == clearBodyButton )  
   {
      Integer count = bodySelectedListBox->GetCount();
        
      if (count == 0)
         return;
           
      for (Integer i = 0; i < count; i++)
      {
         bodyListBox->Append(bodySelectedListBox->GetString(i));
      }
      bodySelectedListBox->Clear();
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
      mSelBodyName = bodySelectedListBox->GetStringSelection();
      
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
   ShowBodyOption(bodySelectedListBox->GetStringSelection(), true);
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void CelesBodySelectDialog::LoadData()
{
   if (! mBodiesToHide.IsEmpty())
   {
      // Check bodyListBox
      for (Integer i = 0; i < (Integer)mBodiesToHide.GetCount(); i++)
      {
         for (Integer j = 0; j < (Integer)bodyListBox->GetCount(); j++)
         {
            std::string bName = bodyListBox->GetString(j).c_str();
            if (strcmp(mBodiesToHide[i].c_str(), bName.c_str()) == 0)
               bodyListBox->Delete(j);
         }    
      }
      
      // Check bodySelectedListBox
      for (Integer i = 0; i < (Integer)mBodiesToHide.GetCount(); i++)
      {
         for (Integer j = 0; j < (Integer)bodySelectedListBox->GetCount(); j++)
         {
            std::string bName = bodySelectedListBox->GetString(j).c_str();
            if (strcmp(mBodiesToHide[i].c_str(), bName.c_str()) == 0)
               bodySelectedListBox->Delete(j);
         }    
      }
   }
   
   // Show body color (loj: 12/15/04 Added)
   bodySelectedListBox->SetSelection(0);
   ShowBodyOption(bodySelectedListBox->GetStringSelection(), true);
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void CelesBodySelectDialog::SaveData()
{  
   mBodyNames.Clear();
   for(unsigned int i=0; i<bodySelectedListBox->GetCount(); i++)
   {
      mBodyNames.Add(bodySelectedListBox->GetString(i));
   }

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



