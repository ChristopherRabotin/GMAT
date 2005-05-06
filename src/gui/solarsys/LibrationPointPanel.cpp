//$Header$

//------------------------------------------------------------------------------

//                              LibrationPointPanel

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

// Author: LaMont Ruley

// Created: 2005/04/27

/**

 * This class allows user to specify where Universe information is 

 * coming from

 */

//------------------------------------------------------------------------------

#include "LibrationPointPanel.hpp"

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

                   :GmatPanel(parent)

{

   theLibrationPt =

      (LibrationPoint*)theGuiInterpreter->GetCalculatedPoint(std::string(name.c_str()));



   Create();

   Show();

   

   theApplyButton->Disable();

}



LibrationPointPanel::~LibrationPointPanel()

{

}





//-------------------------------

// private methods

//-------------------------------



//------------------------------------------------------------------------------

// void OnComboBoxChange(wxCommandEvent& event)

//------------------------------------------------------------------------------

void LibrationPointPanel::OnComboBoxChange(wxCommandEvent& event)

{

    theApplyButton->Enable();

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

   int bsize = 5; // bordersize



   wxString emptyList[] = {"L1", "L2", "L3", "L4", "L5"};



   if (theLibrationPt != NULL)

   {

      // create sizers

      wxFlexGridSizer *pageSizer = new wxFlexGridSizer(3, 2, bsize, bsize);

      

      // label for primary body combobox

      wxStaticText *primaryBodyLabel = new wxStaticText(this, ID_TEXT,

         wxT("Primary body:"), wxDefaultPosition, wxDefaultSize, 0);

        

      // combo box for avaliable bodies 

      primaryBodyCB = 

         theGuiManager->GetConfigBodyComboBox(this, ID_COMBOBOX, wxSize(100,-1));

      

      // label for secondary body combobox

      wxStaticText *secondaryBodyLabel = new wxStaticText(this, ID_TEXT,

         wxT("Secondary body:"), wxDefaultPosition, wxDefaultSize, 0);

        

      // combo box for avaliable bodies 

      secondaryBodyCB = 

         theGuiManager->GetConfigBodyComboBox(this, ID_COMBOBOX, wxSize(100,-1));



      // label for libration point combobox

      wxStaticText *librationPointLabel = new wxStaticText(this, ID_TEXT,

         wxT("Libration point:"), wxDefaultPosition, wxDefaultSize, 0);

        

      // combo box for libration points 

      librationPointCB = new wxComboBox(this, ID_COMBOBOX, wxT(""), 

         wxDefaultPosition, wxSize(100,-1), 5, emptyList, wxCB_DROPDOWN);



      // add labels and comboboxes to page sizer    

      pageSizer->Add(primaryBodyLabel, 0, wxALIGN_LEFT | wxALL, bsize);

      pageSizer->Add(primaryBodyCB, 0, wxALIGN_LEFT | wxALL, bsize);

      pageSizer->Add(secondaryBodyLabel, 0, wxALIGN_LEFT | wxALL, bsize);

      pageSizer->Add(secondaryBodyCB, 0, wxALIGN_LEFT | wxALL, bsize);

      pageSizer->Add(librationPointLabel, 0, wxALIGN_LEFT | wxALL, bsize);

      pageSizer->Add(librationPointCB, 0, wxALIGN_LEFT | wxALL, bsize);



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

      items = theGuiInterpreter->GetListOfConfiguredItems(Gmat::CALCULATED_POINT);

      count = items.size();

      #if DEBUG_LIBRATIONPOINT_PANEL

      MessageInterface::ShowMessage

         ("LibrationPointPanel::LoadData() count = %d\n", count);

      #endif

      if (count > 0)  // check to see if any barycenters exist

      {

         for (i=0; i<count; i++)

         {

            CalculatedPoint *calpt = theGuiInterpreter->GetCalculatedPoint(items[i]);

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

      int primaryBodyID = theLibrationPt->GetParameterID("PrimaryBodyName");

      std::string primaryBody = theLibrationPt->GetStringParameter(primaryBodyID);



      if (primaryBody == "")

         primaryBodyCB->SetSelection(0);

      else

         primaryBodyCB->SetValue(primaryBody.c_str());



      // load secondary body

      int secondaryBodyID = theLibrationPt->GetParameterID("SecondaryBodyName");

      std::string secondaryBody = theLibrationPt->GetStringParameter(secondaryBodyID);



      if (secondaryBody == "")

         secondaryBodyCB->SetSelection(1);

      else

         secondaryBodyCB->SetValue(secondaryBody.c_str());



      #if DEBUG_LIBRATIONPOINT_PANEL

      MessageInterface::ShowMessage

         ("LibrationPointPanel::LoadData() primary body = %s\n", primaryBody.c_str());

      MessageInterface::ShowMessage

         ("LibrationPointPanel::LoadData() secondary body = %s\n", secondaryBody.c_str());

      #endif



      // load libration

      int librationPointID = theLibrationPt->GetParameterID("WhichLibrationPoint");

      std::string librationPoint = theLibrationPt->GetStringParameter(librationPointID);

      

      if (librationPoint == "")

         librationPointCB->SetSelection(0);

      else

         librationPointCB->SetValue(librationPoint.c_str());



      #if DEBUG_LIBRATIONPOINT_PANEL

      MessageInterface::ShowMessage

         ("LibrationPointPanel::LoadData() libration point ID = %d\n",librationPointID);

      MessageInterface::ShowMessage

         ("LibrationPointPanel::LoadData() libration point = %s\n",librationPoint.c_str());

      #endif

   }

   catch (BaseException &e)

   {

      MessageInterface::ShowMessage

         ("LibrationPointPanel:LoadData() error occurred!\n%s\n",

            e.GetMessage().c_str());

   }


   // Activate "ShowScript"
   mObject = theLibrationPt;
}





//------------------------------------------------------------------------------

// virtual void SaveData()

//------------------------------------------------------------------------------

void LibrationPointPanel::SaveData()

{

   // Save data to core engine

   // Primary body

   wxString primaryBodyString = primaryBodyCB->GetStringSelection();

   int primaryBodyID = theLibrationPt->GetParameterID("PrimaryBodyName");

   std::string primaryBody = std::string (primaryBodyString.c_str());

   theLibrationPt->SetStringParameter(primaryBodyID, primaryBody);



   // Secondary body

   wxString secondaryBodyString = secondaryBodyCB->GetStringSelection();

   int secondaryBodyID = theLibrationPt->GetParameterID("SecondaryBodyName");

   std::string secondaryBody = std::string (secondaryBodyString.c_str());

   theLibrationPt->SetStringParameter(secondaryBodyID, secondaryBody);

    

   // Check to make sure the primary body and secondary body are different          

   if (strcmp(primaryBody.c_str(), secondaryBody.c_str()) == 0)

   {

      MessageInterface::PopupMessage

      (Gmat::WARNING_, "Primary body and Secondary body are the same.\n"

                       "Libration point updates have not been saved");

      canClose = false;

      return;

   }    

   else

      canClose = true;

        

   // Libration point

   wxString librationPointString = librationPointCB->GetStringSelection();

   int librationPointID = theLibrationPt->GetParameterID("WhichLibrationPoint");

   std::string librationPoint = std::string (librationPointString.c_str());

   theLibrationPt->SetStringParameter(librationPointID, librationPoint);



   theApplyButton->Disable();

}

