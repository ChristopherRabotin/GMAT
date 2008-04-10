//$Id$
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
#include "StringUtil.hpp"  // for ToReal()

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
   theBurn = 
      (FiniteBurn*) theGuiInterpreter->GetConfiguredObject(burnName.c_str());
   
   thrusterSelected = "";
   isThrusterEmpty = false;
   canClose = true;
   
   Create();
   Show();
   
   EnableUpdate(false);
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
   theGuiManager->UnregisterComboBox("Thruster", mThrusterComboBox);
   theGuiManager->UnregisterComboBox("SpacePoint", mCentralBodyComboBox);
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnFrameComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void FiniteBurnSetupPanel::OnComboBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mAxesComboBox)
   {
      // get the string of the combo box selections
      wxString axesStr = mAxesComboBox->GetStringSelection();
      
      // get the ID of the axes parameter 
      Integer id = theBurn->GetParameterID("Axes");
      
      if (axesStr == "Inertial")
         theBurn->SetStringParameter(id, "Inertial");
      else if (axesStr == "VNB")
         theBurn->SetStringParameter(id, "VNB");
      
      theBurn->SetStringParameter(id, axesStr.c_str());
      
      EnableUpdate(true);
   }
   else if (event.GetEventObject() == mThrusterComboBox)
   {
      if (!isThrusterEmpty)
      {
         thrusterSelected = mThrusterComboBox->GetStringSelection().c_str();
         EnableUpdate(true);
      }
   }
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
   EnableUpdate(true);
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

   unsigned int i, count;
   StringArray items;
   Integer id, bsize;

   if (theBurn != NULL)
   {
      bsize = 2; // border size
      
      // create sizers
      wxFlexGridSizer *pageSizer = new wxFlexGridSizer(4, 2, bsize, bsize);
      
      // Coordinate frames
      // Implemented in later build
      
      // Origin
      wxStaticText *centralBodyLabel = new wxStaticText(this, ID_TEXT, 
         wxT("Origin"), wxDefaultPosition, wxDefaultSize, 0);
      
      mCentralBodyComboBox = 
         theGuiManager->GetSpacePointComboBox(this, ID_COMBOBOX, wxSize(150,-1),
         false);

      // Axes
      wxStaticText *axesLabel = new wxStaticText(this, ID_TEXT,
         wxT("Axes"), wxDefaultPosition, wxDefaultSize, 0);
      
      // list of axes frames
      id    = theBurn->GetParameterID("Axes");
      items = theBurn->GetStringArrayParameter(id);
      count = items.size();
      wxString *axesList = new wxString[count];
      
      if (count > 0)        // check to see if any axes exist
      {
         for (i=0; i<count; i++)
            axesList[i] = wxString(items[i].c_str());

         // combo box for avaliable coordinate frames 
         mAxesComboBox =
            new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition, 
               wxSize(150,-1), count, axesList, wxCB_DROPDOWN|wxCB_READONLY);
      }
      else                 // no coordinate axes exist
      { 
         wxString str[] =
         {
            wxT("No axes available") 
         };   
          
         mAxesComboBox =
            new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition, 
               wxSize(150,-1), 1, str, wxCB_DROPDOWN|wxCB_READONLY);
      }
      
      // Thrusters
      wxStaticText *thrusterLabel = new wxStaticText(this, ID_TEXT,
         wxT("Thruster"), wxDefaultPosition, wxDefaultSize, 0);
      mThrusterComboBox =
         theGuiManager->GetThrusterComboBox(this, ID_COMBOBOX, wxSize(150,-1));
      
      // Burn Scale Factor
      wxStaticText *scaleLabel = new wxStaticText(this, ID_TEXT,
         wxT("Burn scale factor"), wxDefaultPosition, wxDefaultSize, 0);
      
      // create text control field for burn scale factor
      scaleTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
         wxDefaultPosition, wxSize(50,-1), 0 );

      // label for tank combobox
      //wxStaticText *tankLabel = new wxStaticText(this, ID_TEXT,
      //   wxT("Use tank"), wxDefaultPosition, wxDefaultSize, 0);
      
      // combo box for avaliable tanks 
      //mTankComboBox =
      //   theGuiManager->GetFuelTankComboBox(this, ID_COMBOBOX, wxSize(150,-1));
      
      // add to page sizer    
      pageSizer->Add(centralBodyLabel, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(mCentralBodyComboBox, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(axesLabel, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(mAxesComboBox, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(thrusterLabel, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(mThrusterComboBox, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(scaleLabel, 0, wxALIGN_LEFT | wxALL, bsize);
      pageSizer->Add(scaleTextCtrl, 0, wxALIGN_LEFT | wxALL, bsize);
      
      //pageSizer->Add(tankLabel, 0, wxALIGN_LEFT | wxALL, bsize);
      //pageSizer->Add(mTankComboBox, 0, wxALIGN_LEFT | wxALL, bsize);
      
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
      Integer thrusterID = theBurn->GetParameterID("Thrusters");
      StringArray thrusters = theBurn->GetStringArrayParameter(thrusterID);
            
      if (thrusters.size() > 0)
      {
         thrusterSelected = thrusters[0].c_str(); 
         mThrusterComboBox->SetValue(thrusterSelected.c_str());
         theBurn->SetStringParameter(thrusterID, thrusterSelected.c_str(), 0);
      }
      
      if (mThrusterComboBox->IsEmpty())
      {
              mThrusterComboBox->Insert(wxT("No Thrusters Available"), 0);
         mThrusterComboBox->SetSelection(0);
         isThrusterEmpty = true;
      }
      else if (thrusters.empty())
      {
         mThrusterComboBox->Insert(wxT("No Thrusters Selected"), 0);
         mThrusterComboBox->SetSelection(0);
         isThrusterEmpty = false;
      }
      else
      {
         mThrusterComboBox->SetValue(thrusters[0].c_str());
         isThrusterEmpty = false;
      }
      
      // load burn scale factor
      Integer bsfID = theBurn->GetParameterID("BurnScaleFactor");
      Real bsf = theBurn->GetRealParameter(bsfID);
      scaleTextCtrl->SetValue(theGuiManager->ToWxString(bsf));
      
      // load central body
      Integer burnOriginID = theBurn->GetParameterID("Origin");
      std::string burnOriginName = theBurn->GetStringParameter(burnOriginID);
      mCentralBodyComboBox->SetValue(burnOriginName.c_str());
      
      // load axes
      Integer axesId = theBurn->GetParameterID("Axes");
      std::string axesStr = theBurn->GetStringParameter(axesId);
      StringArray frames = theBurn->GetStringArrayParameter(axesId);
      Integer index = 0;
      for (StringArray::iterator iter = frames.begin(); 
           iter != frames.end(); ++iter) 
      {
         if (axesStr == *iter) 
            mAxesComboBox->SetSelection(index);
         else
            ++index;
      }
      
      #if DEBUG_FINITEBURN_PANEL
      MessageInterface::ShowMessage
         ( "thruster list size = %d\n",thrusters.size() );
      //MessageInterface::ShowMessage
      //   ( "tank list size = %d\n",tanks.size() );
      #endif
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("FiniteBurnSetupPanel:LoadData() error occurred!\n%s\n", 
            e.GetFullMessage().c_str());
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
      
   canClose = true;
   Real bsf;
   std::string inputString;
   
   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   inputString = scaleTextCtrl->GetValue();      
   CheckReal(bsf, inputString, "BurnScaleFactor", "Real Number > 0");
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      Integer id;
      
      // save burn scale factor
      id = theBurn->GetParameterID("BurnScaleFactor");
      theBurn->SetRealParameter(id, bsf);
      
      // save central body
      wxString burnOriginStr = mCentralBodyComboBox->GetStringSelection();
      id = theBurn->GetParameterID("Origin");
      std::string origin = std::string (burnOriginStr.c_str());
      theBurn->SetStringParameter(id, origin);
      
      // save axes
      wxString axesStr = mAxesComboBox->GetStringSelection();
      id = theBurn->GetParameterID("Axes");
      std::string axes = std::string (axesStr.c_str());
      theBurn->SetStringParameter(id, axes);
      
      // save thrusters
      id = theBurn->GetParameterID("Thrusters");
      theBurn->SetStringParameter(id, thrusterSelected.c_str(), 0);
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
}
