//$Header$
//------------------------------------------------------------------------------
//                              FiniteBurnSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: LaMont Ruley
// Created: 2004/03/04
//
/**
 * This class contains the Finite Burn Setup window.
 */
//------------------------------------------------------------------------------

#include "FiniteBurnSetupPanel.hpp"

#include "MessageInterface.hpp"

//#define DEBUG_FINITEBURN_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FiniteBurnSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_COMBOBOX(ID_COMBOBOX, FiniteBurnSetupPanel::OnComboBoxChange)
   EVT_TEXT(ID_TEXTCTRL, FiniteBurnSetupPanel::OnTextChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// FiniteBurnSetupPanel(wxWindow *parent, const wxString &burnName)
//------------------------------------------------------------------------------
/**
 * Constructs FiniteBurnSetupPanel object.
 *
 * @param <parent> input parent.
 * @param <burnName> input burn name.
 *
 * @note Creates the Finite burn GUI
 */
//------------------------------------------------------------------------------
FiniteBurnSetupPanel::FiniteBurnSetupPanel(wxWindow *parent, 
   const wxString &burnName):GmatPanel(parent)
{
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();

   theBurn = 
      (FiniteBurn*) theGuiInterpreter->GetBurn(std::string(burnName.c_str()));

   Create();
   Show();
   theApplyButton->Disable();
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnFrameComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void FiniteBurnSetupPanel::OnComboBoxChange(wxCommandEvent& event)
{
    theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void FiniteBurnSetupPanel::OnTextChange(wxCommandEvent& event)
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
 *
 * @note Creates the panel for the Finite burn data
 */
//------------------------------------------------------------------------------
void FiniteBurnSetupPanel::Create()
{
   int bsize = 5; // bordersize

   wxString emptyList[] = {};

   if (theBurn != NULL)
   {
      // create sizers
      wxFlexGridSizer *pageSizer = new wxFlexGridSizer(3, 2, bsize, bsize);

      // Coordinate frames
      // Implemented in later build
            
      // label for thruster combobox
      wxStaticText *thrusterLabel = new wxStaticText(this, ID_TEXT,
         wxT("Use thruster:"), wxDefaultPosition, wxDefaultSize, 0);
        
      // combo box for avaliable thrusters 
      thrusterCB = new wxComboBox(this, ID_COMBOBOX, wxT(""), 
         wxDefaultPosition, wxSize(150,-1), 0, emptyList, wxCB_DROPDOWN);

      // label for tank combobox
      wxStaticText *tankLabel = new wxStaticText(this, ID_TEXT,
         wxT("Use tank:"), wxDefaultPosition, wxDefaultSize, 0);
        
      // combo box for avaliable tanks 
      tankCB = new wxComboBox(this, ID_COMBOBOX, wxT(""), 
         wxDefaultPosition, wxSize(150,-1), 0, emptyList, wxCB_DROPDOWN);

      // create label for burn scale factor
      wxStaticText *scaleLabel = new wxStaticText(this, ID_TEXT,
         wxT("Burn scale factor:"), wxDefaultPosition, wxDefaultSize, 0);

      // create text control field for burn scale factor
      scaleTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
         wxDefaultPosition, wxSize(50,-1), 0 );

      // add thruster label and combobox to thruster sizer    
      pageSizer->Add(thrusterLabel, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(thrusterCB, 0, wxALIGN_LEFT | wxALL, bsize);

      // add tank label and combobox to tank sizer    
      pageSizer->Add(tankLabel, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(tankCB, 0, wxALIGN_LEFT | wxALL, bsize);

      // add scale factor label and text control field to scale sizer    
      pageSizer->Add(scaleLabel, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(scaleTextCtrl, 0, wxALIGN_LEFT | wxALL, bsize);

      // create grid for tanks and thruster selections
      // Implemented in later build - SPH 2/305
      // Right now user can select only 1 tank and 1 thruster

      // add page sizer to middle sizer
      theMiddleSizer->Add(pageSizer, 0, wxALIGN_CENTRE|wxALL, 5);
   }
   else
   {
      // show error message
      MessageInterface::ShowMessage
         ("FiniteBurnSetupPanel:Create() theBurn is NULL\n");
   }
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void FiniteBurnSetupPanel::LoadData()
{
   // load data from the core engine 
   try
   {
      int tankCount = 0;       // counter for configured tanks
      int thrusterCount = 0;   // counter for configured thrusters
   
      // Set object pointer for "Show Script"
      mObject = theBurn;

      // list of thrusters and tanks
      StringArray itemNames = 
         theGuiInterpreter->GetListOfConfiguredItems(Gmat::HARDWARE);
      int size = itemNames.size();
      
      wxString *tankList = new wxString[size];
      wxString *thrusterList = new wxString[size];
      
      for (int i = 0; i<size; i++)
      {
         Hardware *hw = theGuiInterpreter->GetHardware(itemNames[i]);
         wxString objName = wxString(itemNames[i].c_str());
         wxString objTypeName = wxString(hw->GetTypeName().c_str());

         if (objTypeName == "Thruster")
         {
            thrusterList[thrusterCount] = objName;
            ++thrusterCount;
         }   
         else if (objTypeName == "FuelTank")
         {
            tankList[tankCount] = objName;
            ++tankCount;
         }   
      };

      #if DEBUG_FINITEBURN_PANEL
      MessageInterface::ShowMessage
         ("FiniteBurnSetupPanel::LoadData() thruster count = %d\n",thrusterCount);
      MessageInterface::ShowMessage
         ("FiniteBurnSetupPanel::LoadData() tank count = %d\n",tankCount);
      #endif
      // append list of thrusters and tanks to combobox
      for (int i = 0; i<thrusterCount; i++)
         thrusterCB->Append(thrusterList[i]);

      for (int i = 0; i<tankCount; i++)
         tankCB->Append(tankList[i]);
     
      // load thruster
      int thrusterID = theBurn->GetParameterID("Thrusters");
      StringArray thrusters = theBurn->GetStringArrayParameter(thrusterID);
      std::string thruster = "";

      thrusterCB->SetSelection(0);
            
      if (thrusters.size() > 0)
         thruster = thrusters[0]; 
      
      for (int i = 0; i<=thrusterCount; i++)
      {
         if (!strcmp(thruster.c_str(), thrusterList[i].c_str())) 
            thrusterCB->SetSelection(i);
      }

      // load tanks
      int tankID = theBurn->GetParameterID("Tanks");
      StringArray tanks = theBurn->GetStringArrayParameter(tankID);
      std::string tank = "";

      tankCB->SetSelection(0);

      if (tanks.size() > 0)
         tank = tanks[0];

      for (int i = 0; i<=tankCount; i++)
      {
         if (!strcmp(tank.c_str(), tankList[i].c_str())) 
            tankCB->SetSelection(i);
      }

      // load burn scale factor
      int bsfID = theBurn->GetParameterID("BurnScaleFactor");
      Real bsf = theBurn->GetRealParameter(bsfID);
      wxString bsfStr;
      bsfStr.Printf("%f", bsf);
      scaleTextCtrl->SetValue(bsfStr); 
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("FiniteBurnSetupPanel:LoadData() error occurred!\n%s\n", 
            e.GetMessage().c_str());
   }
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void FiniteBurnSetupPanel::SaveData()
{
   // Save data to core engine
   // Thrusters
   wxString thrusterString = thrusterCB->GetStringSelection();
   int thrusterID = theBurn->GetParameterID("Thrusters");
   std::string thruster = std::string (thrusterString.c_str());
   theBurn->SetStringParameter(thrusterID, thruster, 0);

   // Tanks
   wxString tankString = tankCB->GetStringSelection();
   int tankID = theBurn->GetParameterID("Tanks");
   std::string tank = std::string (tankString.c_str());
   theBurn->SetStringParameter(tankID, tank, 0);
    
   // Burn scale factor
   int bsfID = theBurn->GetParameterID("BurnScaleFactor");
   wxString bsfStr = scaleTextCtrl->GetValue();
   theBurn->SetRealParameter(bsfID, atof(bsfStr));

   theApplyButton->Disable();
}
