//$Id$
//------------------------------------------------------------------------------
//                              BarycenterPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Allison Greene
// Created: 2005/04/21
/**
 * This class allows user to specify Barycenter.
 */
//------------------------------------------------------------------------------
#include "BarycenterPanel.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_BARYCENTER_PANEL 1

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(BarycenterPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)

   EVT_BUTTON(ID_BUTTON, BarycenterPanel::OnButton)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
//------------------------------------------------------------------------------
// BarycenterPanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs BarycenterPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the Universe GUI
 */
//------------------------------------------------------------------------------
BarycenterPanel::BarycenterPanel(wxWindow *parent, const wxString &name)
                :GmatPanel(parent)
{
   theBarycenter =
      (Barycenter*)theGuiInterpreter->GetConfiguredObject(name.c_str());
   mIsBuiltIn    = theBarycenter->IsBuiltIn();

   mBodyNames.Clear();
   mIsBodySelected = false;

   Create();
   Show();
}

//------------------------------------------------------------------------------
// ~BarycenterPanel()
//------------------------------------------------------------------------------
BarycenterPanel::~BarycenterPanel()
{
   theGuiManager->UnregisterListBox("CelestialBody", bodyListBox, &mExcludedCelesBodyList);
}

//-------------------------------------------
// protected methods inherited from GmatPanel
//-------------------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * @param <scName> input spacecraft name.
 *
 * @note Creates the notebook for spacecraft information
 */
//------------------------------------------------------------------------------
void BarycenterPanel::Create()
{
   int borderSize = 2;
   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;
   
   // 1. Create Add, Remove, Clear buttons box:
   wxBoxSizer *buttonsBoxSizer = new wxBoxSizer(wxVERTICAL);
   addBodyButton = new wxButton( this, ID_BUTTON, wxT("->"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
   removeBodyButton = new wxButton( this, ID_BUTTON, wxT("<-"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
   clearBodyButton = new wxButton( this, ID_BUTTON, wxT("<="), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
   buttonsBoxSizer->Add(addBodyButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   buttonsBoxSizer->Add(removeBodyButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   buttonsBoxSizer->Add(clearBodyButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   
   // 2. Create Available Bodies box:
   GmatStaticBoxSizer* listStaticBoxSizer = new GmatStaticBoxSizer(wxHORIZONTAL, this, "Available Bodies");
   wxArrayString tmpArrayString;
   bodyListBox = theGuiManager->GetCelestialBodyListBox(this, -1, wxSize(180, 200), &mExcludedCelesBodyList);
   listStaticBoxSizer->Add(bodyListBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   // 3. Create Selected Bodies box:
   GmatStaticBoxSizer* selectedStaticBoxSizer = new GmatStaticBoxSizer(wxHORIZONTAL, this, "Selected Bodies");
   bodySelectedListBox = new wxListBox(this, ID_BODY_SEL_LISTBOX, wxDefaultPosition, wxSize(180, 200), //0,
                                       emptyList, wxLB_SINGLE);
   selectedStaticBoxSizer->Add(bodySelectedListBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   // 4. Create Bodies box:
   wxFlexGridSizer *bodyGridSizer = new wxFlexGridSizer(3, 0, 0);
   bodyGridSizer->Add(listStaticBoxSizer, 0, wxALIGN_CENTER|wxALL, borderSize);
   bodyGridSizer->Add(buttonsBoxSizer, 0, wxALIGN_CENTER|wxALL, borderSize);
   bodyGridSizer->Add(selectedStaticBoxSizer, 0, wxALIGN_CENTER|wxALL, borderSize);
   GmatStaticBoxSizer * bodiesStaticBoxSizer = new GmatStaticBoxSizer(wxVERTICAL, this, "Bodies");
   bodiesStaticBoxSizer->Add(bodyGridSizer, 0, wxALIGN_CENTER|wxALL, borderSize);
   
   // 5. Add to parent sizer:
   theMiddleSizer->Add(bodiesStaticBoxSizer, 0, wxEXPAND|wxALL, borderSize);
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void BarycenterPanel::LoadData()
{
   try
   {
      StringArray selectedBodies;
      if (mIsBuiltIn)
      {
         selectedBodies = theBarycenter->GetBuiltInNames();
      }
      else
      {
         selectedBodies= theBarycenter->
            GetStringArrayParameter("BodyNames");
         if (selectedBodies.empty())
            selectedBodies = theBarycenter->GetDefaultBodies();
      }
      for (unsigned int i=0; i<selectedBodies.size(); i++)
      {
         bodySelectedListBox->Append(selectedBodies[i].c_str());

         // find string in body list and delete it, so there are no dups
         int position = bodyListBox->FindString(selectedBodies[i].c_str());
         bodyListBox->Delete(position);

         // Added to excluded list
         if (!mIsBuiltIn)
            mExcludedCelesBodyList.Add(selectedBodies[i].c_str());
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("BarycenterPanel:LoadData() error occurred!\n%s\n",
            e.GetFullMessage().c_str());
   }
   
   // Activate "ShowScript"
   mObject = theBarycenter;

   /// don't allow the user to modify the built-in Barycenter(s)
   if (mIsBuiltIn)
   {
      addBodyButton->Enable(false);
      removeBodyButton->Enable(false);
      clearBodyButton->Enable(false);
      bodyListBox->Enable(false);
      bodySelectedListBox->Enable(false);
   }
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void BarycenterPanel::SaveData()
{
   canClose = true;
   if (mIsBuiltIn) return;
   
   try
   {

      Integer count = bodySelectedListBox->GetCount();
  
      if (count == 0)
      {
         MessageInterface::PopupMessage(Gmat::ERROR_,
                 "At least one body must be selected!");
         canClose = false;
         return;
      }
      
      theBarycenter->TakeAction("ClearBodies");
      
      // get Earth pointer as J2000Body
      CelestialBody *j2000body =
         (CelestialBody*)theGuiInterpreter->GetConfiguredObject("Earth");
      CelestialBody *body;
      std::string bodyName;
      
      for (Integer i = 0; i < count; i++)
      {
         bodyName = bodySelectedListBox->GetString(i).c_str();
         theBarycenter->SetStringParameter("BodyNames", bodyName, i);
         
         body = (CelestialBody*)theGuiInterpreter->GetConfiguredObject(bodyName);
         
         // set J2000Body for the body
         if (body->GetJ2000Body() == NULL)
            body->SetJ2000Body(j2000body);
         
         theBarycenter->SetRefObject(body, Gmat::SPACE_POINT, bodyName);
         
         #if DEBUG_BARYCENTER_PANEL
         MessageInterface::ShowMessage
            ("BarycenterPanel::SaveData() body[%d]=%d, name=%s, J2000Body=%d\n",
             i, body, bodyName.c_str(), j2000body);
         #endif
         
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("BarycenterPanel:SaveData() error occurred!\n%s\n",
            e.GetFullMessage().c_str());
   }
}


//------------------------------------------------------------------------------
// void OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void BarycenterPanel::OnButton(wxCommandEvent& event)
{
   if ( event.GetEventObject() == addBodyButton )
   {
      wxString str = bodyListBox->GetStringSelection();
      
      if (str.IsEmpty())
         return;
      
      int strId = bodyListBox->FindString(str);
      int found = bodySelectedListBox->FindString(str);
      
      // if the string wasn't found in the second list, insert it
      if (found == wxNOT_FOUND)
      {
         bodySelectedListBox->Append(str);
         bodySelectedListBox->SetStringSelection(str);
         
         // Removed from available list
         bodyListBox->Delete(strId);
         
         // select next available body
         if (strId == 0)
            bodyListBox->SetSelection(0);
         else if (strId > 0)
            bodyListBox->SetSelection(strId - 1);
         
         // Added to excluded list
         mExcludedCelesBodyList.Add(str);
      }
   }
   else if ( event.GetEventObject() == removeBodyButton )
   {
      wxString str = bodySelectedListBox->GetStringSelection();
      int strId = bodySelectedListBox->FindString(str);
      
      if (str.IsEmpty())
         return;
      
      #ifdef DEBUG_REMOVE
      MessageInterface::ShowMessage("Removing body: %s\n", str.c_str());
      #endif
      
      // Add to available list
      bodyListBox->Append(str);
      
      // Remove from selected list
      bodySelectedListBox->Delete(strId);
      
      // select next selected body
      if (strId == 0)
         bodySelectedListBox->SetSelection(0);
      else if (strId > 0)
         bodySelectedListBox->SetSelection(strId - 1);
      
      // Remove from excluded list
      mExcludedCelesBodyList.Remove(str);
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
      
      // Clear excluded list
      mExcludedCelesBodyList.Clear();
   }

   EnableUpdate(true);
}
