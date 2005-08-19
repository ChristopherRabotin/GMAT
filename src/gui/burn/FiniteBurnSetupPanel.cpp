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

//------------------------------------------------------------------------------
// ~FiniteBurnSetupPanel()
//------------------------------------------------------------------------------
/**
 * Destroys FiniteBurnSetupPanel object.
 *
 */
//------------------------------------------------------------------------------
FiniteBurnSetupPanel::~FiniteBurnSetupPanel()
{
   theGuiManager->UnregisterComboBox("FuelTank", mTankComboBox);
   theGuiManager->UnregisterComboBox("Thruster", mThrusterComboBox);
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
   #if DEBUG_FINITEBURN_PANEL
      MessageInterface::ShowMessage( "FiniteBurnSetupPanel::Create() \n" );
   #endif

   int bsize = 5; // bordersize

   if (theBurn != NULL)
   {
      // create sizers
      wxFlexGridSizer *pageSizer = new wxFlexGridSizer(4, 2, bsize, bsize);

      // Coordinate frames
      // Implemented in later build
            
      // label for thruster combobox
      wxStaticText *thrusterLabel = new wxStaticText(this, ID_TEXT,
         wxT("Use thruster:"), wxDefaultPosition, wxDefaultSize, 0);
        
      // combo box for avaliable thrusters 
      mThrusterComboBox =
         theGuiManager->GetThrusterComboBox(this, ID_COMBOBOX, wxSize(150,-1));

      // label for tank combobox
      wxStaticText *tankLabel = new wxStaticText(this, ID_TEXT,
         wxT("Use tank:"), wxDefaultPosition, wxDefaultSize, 0);
        
      // combo box for avaliable tanks 
      mTankComboBox =
         theGuiManager->GetFuelTankComboBox(this, ID_COMBOBOX, wxSize(150,-1));

      // create label for burn scale factor
      wxStaticText *scaleLabel = new wxStaticText(this, ID_TEXT,
         wxT("Burn scale factor:"), wxDefaultPosition, wxDefaultSize, 0);

      // create text control field for burn scale factor
      scaleTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
         wxDefaultPosition, wxSize(50,-1), 0 );

      // create label and text field for the central body
      wxStaticText *centralBodyLabel = new wxStaticText(this, ID_TEXT, 
         wxT("Central body:"), wxDefaultPosition, wxDefaultSize, 0);

      // combo box for avaliable bodies 
      mCentralBodyComboBox = 
         theGuiManager->GetSpacePointComboBox(this, ID_COMBOBOX, wxSize(120,-1),
         false);

      // add thruster label and combobox to thruster sizer    
      pageSizer->Add(thrusterLabel, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(mThrusterComboBox, 0, wxALIGN_LEFT | wxALL, bsize);

      // add tank label and combobox to tank sizer    
      pageSizer->Add(tankLabel, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(mTankComboBox, 0, wxALIGN_LEFT | wxALL, bsize);

      // add scale factor label and text control field to scale sizer    
      pageSizer->Add(scaleLabel, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(scaleTextCtrl, 0, wxALIGN_LEFT | wxALL, bsize);

      // add central body label and combobox to page sizer    
      pageSizer->Add(centralBodyLabel, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(mCentralBodyComboBox, 0, wxALIGN_LEFT | wxALL, bsize);

      // create grid for tanks and thruster selections
      // Implemented in later build - SPH 2/3/05
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
   #if DEBUG_FINITEBURN_PANEL
      MessageInterface::ShowMessage( "FiniteBurnSetupPanel::LoadData() \n" );
   #endif

   // load data from the core engine 
   try
   {
      // Set object pointer for "Show Script"
      mObject = theBurn;
    
      // load thruster
      int thrusterID = theBurn->GetParameterID("Thrusters");
      StringArray thrusters = theBurn->GetStringArrayParameter(thrusterID);
      std::string thruster = "";
            
      if (thrusters.size() > 0)
      {
         thruster = thrusters[0]; 
         mThrusterComboBox->SetValue(thruster.c_str());
      }

      // load tanks
      int tankID = theBurn->GetParameterID("Tanks");
      StringArray tanks = theBurn->GetStringArrayParameter(tankID);
      std::string tank = "";

      if (tanks.size() > 0)
      {
         tank = tanks[0];
         mTankComboBox->SetValue(tank.c_str());
      }

      // load burn scale factor
      int bsfID = theBurn->GetParameterID("BurnScaleFactor");
      Real bsf = theBurn->GetRealParameter(bsfID);
      wxString bsfStr;
      bsfStr.Printf("%f", bsf);
      scaleTextCtrl->SetValue(bsfStr);
      
      // load central body
      int burnOriginID = theBurn->GetParameterID("Origin");
      std::string burnOriginName = theBurn->GetStringParameter(burnOriginID);
      mCentralBodyComboBox->SetValue(burnOriginName.c_str());

      #if DEBUG_FINITEBURN_PANEL
      MessageInterface::ShowMessage
         ( "thruster list size = %d\n",thrusters.size() );
      MessageInterface::ShowMessage
         ( "tank list size = %d\n",tanks.size() );
      #endif
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
   #if DEBUG_FINITEBURN_PANEL
      MessageInterface::ShowMessage( "FiniteBurnSetupPanel::SaveData() \n" );
   #endif

   // Save data to core engine
   Integer id;

   // Thrusters
   wxString thrusterString = mThrusterComboBox->GetStringSelection();
   id = theBurn->GetParameterID("Thrusters");
   std::string thruster = std::string (thrusterString.c_str());
   theBurn->SetStringParameter(id, thruster, 0);

   // Tanks
   wxString tankString = mTankComboBox->GetStringSelection();
   id = theBurn->GetParameterID("Tanks");
   std::string tank = std::string (tankString.c_str());
   theBurn->SetStringParameter(id, tank, 0);
    
   // Burn scale factor
   id = theBurn->GetParameterID("BurnScaleFactor");
   wxString bsfStr = scaleTextCtrl->GetValue();
   theBurn->SetRealParameter(id, atof(bsfStr));

   // save central body
   wxString burnOriginStr = mCentralBodyComboBox->GetStringSelection();
   id = theBurn->GetParameterID("Origin");
   std::string origin = std::string (burnOriginStr.c_str());
   theBurn->SetStringParameter(id, origin);

   theApplyButton->Disable();
}
