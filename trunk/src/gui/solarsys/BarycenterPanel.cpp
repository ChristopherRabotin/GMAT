//$Header$
//------------------------------------------------------------------------------
//                              BarycenterPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
//
// **Legal**
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
                       wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );

   removeBodyButton = new wxButton( this, ID_BUTTON, wxT("<-"), 
                          wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
                                    

   clearBodyButton = new wxButton( this, ID_BUTTON, wxT("<="),
                         wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );

   wxArrayString tmpArrayString;

   // wxListBox
   bodyListBox =
      theGuiManager->GetConfigBodyListBox(this, -1, wxSize(150, 200), tmpArrayString);

   bodySelectedListBox = new wxListBox
      (this, ID_BODY_SEL_LISTBOX, wxDefaultPosition, wxSize(150, 200), //0,
       emptyList, wxLB_SINGLE);
   
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
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void BarycenterPanel::LoadData()
{
   try
   {
      StringArray selectedBodies = theBarycenter->
            GetStringArrayParameter("BodyNames");

      for (unsigned int i=0; i<selectedBodies.size(); i++)
      {
         bodySelectedListBox->Append(selectedBodies[i].c_str());

         // find string in body list and delete it, so there are no dups
         int position = bodyListBox->FindString(selectedBodies[i].c_str());
         bodyListBox->Delete(position);
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
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void BarycenterPanel::SaveData()
{
   canClose = true;
   
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

   EnableUpdate(true);
}
