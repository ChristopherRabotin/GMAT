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
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CelesBodySelectDialog, GmatDialog)
    EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
    EVT_BUTTON(ID_BUTTON_APPLY, GmatDialog::OnApply)
    EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
    EVT_BUTTON(ID_BUTTON, CelesBodySelectDialog::OnButton)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// CelesBodySelectDialog(wxWindow *parent, wxArrayString &bodiesToExclude)
//------------------------------------------------------------------------------
CelesBodySelectDialog::CelesBodySelectDialog(wxWindow *parent,
                                             wxArrayString &bodiesToExclude,
                                             wxArrayString &bodiesToHide)
    : GmatDialog(parent, -1, wxString(_T("CelesBodySelectDialog")))
{
    mBodyNames.Clear();
    mBodiesToHide.Clear();
    
    mBodiesToExclude = bodiesToExclude;
    mBodiesToHide = bodiesToHide;
    
    mIsBodySelected = false;
        
    Create();
    Show();
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void CelesBodySelectDialog::Create()
{
    int borderSize = 2;
    wxString emptyList[] = {};

    //wxStaticText
    wxStaticText *bodyStaticText =
        new wxStaticText( this, ID_TEXT, wxT("Available Bodies"),
                          wxDefaultPosition, wxDefaultSize, 0 );
    
    wxStaticText *bodySelectStaticText =
        new wxStaticText( this, ID_TEXT, wxT("Bodies Selected"),
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

    // wxListBox
    bodyListBox =
        theGuiManager->GetConfigBodyListBox(this, -1, wxSize(150, 200), mBodiesToExclude);

    if (! mBodiesToExclude.IsEmpty())
    {
       Integer count = mBodiesToExclude.GetCount();
       wxString selectedBodyList[MAX_LIST_SIZE];
       for (Integer i = 0; i < count; i++)
       {
          selectedBodyList[i] = mBodiesToExclude[i].c_str(); 
       }   
          
       bodySelectedListBox = new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                                       wxSize(150, 200), count, selectedBodyList, wxLB_SINGLE);
    }
    else
    {
       bodySelectedListBox = new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                                       wxSize(150, 200), 0, emptyList, wxLB_SINGLE);
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
        
    pageBoxSizer->Add( bodyGridSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);

    //------------------------------------------------------
    // add to parent sizer
    //------------------------------------------------------
    theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
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
        }
//waw: 09/17/04
//        theOkButton->Enable();
    }
    else if ( event.GetEventObject() == removeBodyButton )  
    {
        wxString s = bodySelectedListBox->GetStringSelection();
        
        if (s.IsEmpty())
           return;
           
//        MessageInterface::ShowMessage("Removing body: %s\n", s.c_str());
        bodyListBox->Append(s);
        int sel = bodySelectedListBox->GetSelection();
        bodySelectedListBox->Delete(sel);
//waw: 09/17/04        
//        if (bodySelectedListBox->GetCount() > 0)
//            theOkButton->Enable();
//        else
//            theOkButton->Disable();
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
//waw: 09/17/04
//        theOkButton->Disable();
    }
    theOkButton->Enable();
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
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void CelesBodySelectDialog::SaveData()
{  
    mBodyNames.Clear();
    for(int i=0; i<bodySelectedListBox->GetCount(); i++)
    {
        mBodyNames.Add(bodySelectedListBox->GetString(i));
    }
    mIsBodySelected = true;
}

//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void CelesBodySelectDialog::ResetData()
{
    mIsBodySelected = false;
}


