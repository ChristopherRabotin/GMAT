//$Id$
//------------------------------------------------------------------------------
//                              LibrationPointPanel
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
// Author: LaMont Ruley
// Created: 2005/04/27
/**
 * This class allows user to specify where Universe information is 
 * coming from
 */
//------------------------------------------------------------------------------

#include "LibrationPointPanel.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_LIBRATIONPOINT_PANEL 1

//------------------------------
// event tables for wxWindows
//------------------------------

BEGIN_EVENT_TABLE(LibrationPointPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_COMBOBOX(ID_COMBOBOX, LibrationPointPanel::OnComboBoxChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// LibrationPointPanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs LibrationPointPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the Universe GUI
 */
//------------------------------------------------------------------------------
LibrationPointPanel::LibrationPointPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent)
{
   theLibrationPt =
      (LibrationPoint*)theGuiInterpreter->GetConfiguredObject(name.c_str());

   Create();
   Show();

   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// ~LibrationPointPanel()
//------------------------------------------------------------------------------
LibrationPointPanel::~LibrationPointPanel()
{
   // Unregister GUI components
   theGuiManager->UnregisterComboBox("CelestialBody", primaryBodyCB);
   theGuiManager->UnregisterComboBox("CelestialBody", secondaryBodyCB);
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void LibrationPointPanel::OnComboBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * @param <scName> input spacecraft name.
 *
 * @note Creates the notebook for spacecraft information
 */
//------------------------------------------------------------------------------
void LibrationPointPanel::Create()
{
   int bsize = 2; // border size
   
   wxString librationList[] = {"L1", "L2", "L3", "L4", "L5"};
   
   if (theLibrationPt != NULL)
   {
      // create sizers
      wxFlexGridSizer *pageSizer = new wxFlexGridSizer(3, 2, bsize, bsize);
      
      // label for primary body combobox
      wxStaticText *primaryBodyLabel = new wxStaticText(this, ID_TEXT,
         wxT("Primary Body:"), wxDefaultPosition, wxDefaultSize, 0);
      
      // combo box for avaliable bodies 
      primaryBodyCB = 
         theGuiManager->GetCelestialBodyComboBox(this, ID_COMBOBOX, wxSize(100,-1));
      
      // label for secondary body combobox
      wxStaticText *secondaryBodyLabel = new wxStaticText(this, ID_TEXT,
         wxT("Secondary Body:"), wxDefaultPosition, wxDefaultSize, 0);
      
      // combo box for avaliable bodies 
      secondaryBodyCB = 
         theGuiManager->GetCelestialBodyComboBox(this, ID_COMBOBOX, wxSize(100,-1));
      
      // label for libration point combobox
      wxStaticText *librationPointLabel = new wxStaticText(this, ID_TEXT,
         wxT("Libration Point:"), wxDefaultPosition, wxDefaultSize, 0);
      
      // combo box for libration points 
      librationPtCB = new wxComboBox(this, ID_COMBOBOX, wxT(""), 
         wxDefaultPosition, wxSize(100,-1), 5, librationList, wxCB_READONLY);
      
      // add labels and comboboxes to page sizer    
      pageSizer->Add(primaryBodyLabel, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(primaryBodyCB, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(secondaryBodyLabel, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(secondaryBodyCB, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(librationPointLabel, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(librationPtCB, 0, wxALIGN_LEFT | wxALL, bsize);
      
      // create Options box:
      GmatStaticBoxSizer *optionsStaticBoxSizer =
         new GmatStaticBoxSizer(wxVERTICAL, this, "Options");
      optionsStaticBoxSizer->Add(pageSizer, 0, wxALIGN_LEFT|wxALL,bsize);
      
      // add page sizer to middle sizer
      theMiddleSizer->Add(optionsStaticBoxSizer, 0, wxEXPAND|wxALL, bsize);
   }
   else
   {
      // show error message
      MessageInterface::ShowMessage
         ("LibrationPointPanel:Create() theLP is NULL\n");
   }
   
}

/*
void LibrationPointPanel::Create()
{
   int bsize = 5; // bordersize

   wxString librationList[] = {"L1", "L2", "L3", "L4", "L5"};

   if (theLibrationPt != NULL)
   {
      // create sizers
      wxFlexGridSizer *pageSizer = new wxFlexGridSizer(3, 2, bsize, bsize);
     
      // label for primary body combobox
      wxStaticText *primaryBodyLabel = new wxStaticText(this, ID_TEXT,
         wxT("Primary Body:"), wxDefaultPosition, wxDefaultSize, 0);
      
      // combo box for avaliable bodies 
      primaryBodyCB = 
         theGuiManager->GetCelestialBodyComboBox(this, ID_COMBOBOX, wxSize(100,-1));
      
      // label for secondary body combobox
      wxStaticText *secondaryBodyLabel = new wxStaticText(this, ID_TEXT,
         wxT("Secondary Body:"), wxDefaultPosition, wxDefaultSize, 0);
        
      // combo box for avaliable bodies 
      secondaryBodyCB = 
         theGuiManager->GetCelestialBodyComboBox(this, ID_COMBOBOX, wxSize(100,-1));

      // label for libration point combobox
      wxStaticText *librationPointLabel = new wxStaticText(this, ID_TEXT,
         wxT("Libration Point:"), wxDefaultPosition, wxDefaultSize, 0);
        
      // combo box for libration points 
      librationPtCB = new wxComboBox(this, ID_COMBOBOX, wxT(""), 
         wxDefaultPosition, wxSize(100,-1), 5, librationList, wxCB_READONLY);

      // add labels and comboboxes to page sizer    
      pageSizer->Add(primaryBodyLabel, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(primaryBodyCB, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(secondaryBodyLabel, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(secondaryBodyCB, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(librationPointLabel, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(librationPtCB, 0, wxALIGN_LEFT | wxALL, bsize);

      // add page sizer to middle sizer
      theMiddleSizer->Add(pageSizer, 0, wxALIGN_CENTRE|wxALL, 5);
   }
   else
   {
      // show error message
      MessageInterface::ShowMessage
         ("LibrationPointPanel:Create() theLP is NULL\n");
   }

}
*/

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void LibrationPointPanel::LoadData()
{
   unsigned int i, count;
   StringArray items;

   // load data from the core engine 
   try
   {
      // list of calculated points
      items = theGuiInterpreter->GetListOfObjects(Gmat::CALCULATED_POINT);
      count = items.size();
      #if DEBUG_LIBRATIONPOINT_PANEL
      MessageInterface::ShowMessage
         ("LibrationPointPanel::LoadData() count = %d\n", count);
      #endif

      if (count > 0)  // check to see if any barycenters exist
      {
         for (i=0; i<count; i++)
         {
            CalculatedPoint *calpt
               = (CalculatedPoint*)theGuiInterpreter->GetConfiguredObject(items[i]);
            wxString objName = wxString(items[i].c_str());
            wxString objTypeName = wxString(calpt->GetTypeName().c_str());

            // append barycenters to the primary and secondary body lists   
            if (objTypeName == "Barycenter")
            {
               primaryBodyCB->Append(objName);
               secondaryBodyCB->Append(objName);
            }
         }
      }

      // load primary body
      std::string primaryBody = theLibrationPt->GetStringParameter("Primary");
      primaryBodyCB->SetValue(primaryBody.c_str());

      // load secondary body
      std::string secondaryBody = theLibrationPt->GetStringParameter("Secondary");
      secondaryBodyCB->SetValue(secondaryBody.c_str());

      #if DEBUG_LIBRATIONPOINT_PANEL
         MessageInterface::ShowMessage
            ("LibrationPointPanel::LoadData() primary body = %s\n", 
               primaryBody.c_str());
         MessageInterface::ShowMessage
            ("LibrationPointPanel::LoadData() secondary body = %s\n", 
               secondaryBody.c_str());
      #endif

      // load libration
      std::string librationPoint = theLibrationPt->GetStringParameter("Point");
      librationPtCB->SetValue(librationPoint.c_str());

      #if DEBUG_LIBRATIONPOINT_PANEL
         MessageInterface::ShowMessage
            ("LibrationPointPanel::LoadData() libration point = %s\n",
               librationPoint.c_str());
      #endif
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage().c_str());
   }
   
   // Activate "ShowScript"
   mObject = theLibrationPt;
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void LibrationPointPanel::SaveData()
{
   canClose = true;
   
   //-----------------------------------------------------------------
   // check user input
   //-----------------------------------------------------------------
   wxString primaryBodyString = primaryBodyCB->GetValue().Trim();
   wxString secondaryBodyString = secondaryBodyCB->GetValue().Trim();

   // Check to make sure the primary body and secondary body are different        
   if (primaryBodyString == secondaryBodyString)
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "Primary and Secondary bodies can not be the same");
      canClose = false;
      
      return;
   }
   
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      std::string spName;
      // get Earth pointer as j2000body
      CelestialBody *j2000body =
         (CelestialBody*)theGuiInterpreter->GetConfiguredObject("Earth");      
      
      // Primary body
      spName = primaryBodyString.c_str();
      int primaryBodyID = theLibrationPt->GetParameterID("Primary");
      theLibrationPt->SetStringParameter(primaryBodyID, spName);
      SpacePoint *primary = (SpacePoint*)theGuiInterpreter->GetConfiguredObject(spName);
      theLibrationPt->SetRefObject(primary, Gmat::SPACE_POINT, spName);
      
      // set Earth as J000Body of primary body if NULL
      if (primary->GetJ2000Body() == NULL)
         primary->SetJ2000Body(j2000body);
      
      #if DEBUG_LIBRATIONPOINT_PANEL
      MessageInterface::ShowMessage
         ("LibrationPointPanel::SaveData() primary body ID = %d\n", 
          primaryBodyID);
      MessageInterface::ShowMessage
         ("LibrationPointPanel::SaveData() primary body = %s\n", 
          primaryBodyString.c_str());
      #endif
      
      // Secondary body
      spName = secondaryBodyString.c_str();
      int secondaryBodyID = theLibrationPt->GetParameterID("Secondary");
      theLibrationPt->SetStringParameter(secondaryBodyID, spName);
      SpacePoint *secondary = (SpacePoint*)theGuiInterpreter->GetConfiguredObject(spName);
      theLibrationPt->SetRefObject(secondary, Gmat::SPACE_POINT, spName);
      
      // set Earth as J000Body of secondary body if NULL
      if (secondary->GetJ2000Body() == NULL)
         secondary->SetJ2000Body(j2000body);
      
      #if DEBUG_LIBRATIONPOINT_PANEL
      MessageInterface::ShowMessage
         ("LibrationPointPanel::SaveData() secondary body ID = %d\n", 
          secondaryBodyID);
      MessageInterface::ShowMessage
         ("LibrationPointPanel::SaveData() secondary body = %s\n", 
          secondaryBodyString.c_str());
      #endif

      
      // Libration point
      wxString librationPointString = librationPtCB->GetValue().Trim();
      int librationPointID = theLibrationPt->GetParameterID("Point");
      
      #if DEBUG_LIBRATIONPOINT_PANEL
      MessageInterface::ShowMessage
         ("LibrationPointPanel::SaveData() libration point ID = %d\n", 
          librationPointID);
      MessageInterface::ShowMessage
         ("LibrationPointPanel::SaveData() libration point = %s\n", 
          librationPointString.c_str());
      #endif
      
      theLibrationPt->SetStringParameter(librationPointID, librationPointString.c_str());
      
      EnableUpdate(false);
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage().c_str());
      canClose = false;
   }
}

