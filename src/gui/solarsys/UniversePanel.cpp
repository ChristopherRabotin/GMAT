//$Header$
//------------------------------------------------------------------------------
//                              UniversePanel
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
// Author: Monisha Butler
// Created: 2003/09/10
// Modified: 2004/01/13 by Allison Greene for action handling
/**
 * This class allows user to specify where Universe information is 
 * coming from
 */
//------------------------------------------------------------------------------
#include "UniversePanel.hpp"
#include "MessageInterface.hpp"
#include "ShowScriptDialog.hpp"
#include "StringUtil.hpp"
#include <fstream>

//#define DEBUG_UNIV_PANEL 1

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(UniversePanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)

   EVT_BUTTON(ID_BUTTON_BROWSE, UniversePanel::OnBrowseButton)
   EVT_COMBOBOX(ID_COMBOBOX, UniversePanel::OnComboBoxChange)
   EVT_CHECKBOX(CHECKBOX, UniversePanel::OnCheckBoxChange)
   EVT_TEXT(ID_TEXT_CTRL, UniversePanel::OnTextCtrlChange)

END_EVENT_TABLE()
   
   
//---------------------------------
// public methods
//---------------------------------
//------------------------------------------------------------------------------
// UniversePanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs UniversePanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the Universe GUI
 */
//------------------------------------------------------------------------------
UniversePanel::UniversePanel(wxWindow *parent):GmatPanel(parent)
{
   mHasFileTypesInUseChanged = false;
   mHasFileNameChanged = false;
   mHasAnaModelChanged = false;
   mHasTextModified = false;
   
   Create();
   Show();
}

//------------------------------------------------------------------------------
// ~UniversePanel()
//------------------------------------------------------------------------------
UniversePanel::~UniversePanel()
{
}


//------------------------------------------------------------------------------
// void OnScript()
//------------------------------------------------------------------------------
/**
 * Shows Scripts
 */
//------------------------------------------------------------------------------
void UniversePanel::OnScript(wxCommandEvent &event)
{
   wxString title = "Object Script";
   // open separate window to show scripts?
   if (mObject != NULL) {
      title = "Scripting for ";
      title += mObject->GetName().c_str();
   }
   ShowScriptDialog ssd(this, -1, title, mObject, true);
   ssd.ShowModal();
}



//---------------------------------
// methods inherited from GmatPanel
//---------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * @param <scName> input spacecraft name.
 *
 * @note Creates the notebook for spacecraft information
 */
//------------------------------------------------------------------------------
void UniversePanel::Create()
{
   // SetParent(new wxFrame(0,-1,"title"));
   // parent = GetParent();

   #if DEBUG_UNIV_PANEL
   MessageInterface::ShowMessage("UniversePanel::Create() entering\n");
   #endif

   int bsize = 2;
   //VC++ error C2466: cannot allocate an array of constant size 0
   //wxString emptyArray[] = { };
   wxArrayString emptyArray;
   
   //-------------------------------------------------------
   // ephemeris update interval
   //-------------------------------------------------------
   wxStaticText *intervalStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Ephemeris Update Interval"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mIntervalTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""),
                                 wxDefaultPosition, wxSize(50,-1), 0);
   
   //-------------------------------------------------------
   // ephemeris source
   //-------------------------------------------------------
   wxStaticText *fileTypeLabel =
      new wxStaticText(this, ID_TEXT, wxT("Ephemeris Source"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   mFileTypeComboBox = 
      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxDefaultSize, //0,
                     emptyArray, wxCB_READONLY);
   
   //-------------------------------------------------------
   // ephemeris file
   //-------------------------------------------------------
   wxStaticText *fileNameLabel =
      new wxStaticText(this, ID_TEXT, wxT("Ephemeris Name"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   mFileNameTextCtrl =
      new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""),
                     wxDefaultPosition, wxSize(300, -1),  0);
   
   mBrowseButton =
      new wxButton(this, ID_BUTTON_BROWSE, wxT("Browse"),
                   wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
                   
   //-------------------------------------------------------
   // use TT for ephemeris
   //-------------------------------------------------------
   mOverrideCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Use TT for Ephemeris"),
                     wxDefaultPosition, wxSize(-1, -1), 0);

   //-------------------------------------------------------
   // analytic motel
   //-------------------------------------------------------
   
   wxStaticText *anaModelLabel =
      new wxStaticText(this, ID_TEXT, wxT("Analytic Model"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   mAnalyticModelComboBox = 
      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxDefaultSize, //0,
                     emptyArray, wxCB_READONLY);
   
   //-------------------------------------------------------
   // Add to bottom grid sizer
   //-------------------------------------------------------    
   wxFlexGridSizer *bottomGridSizer = new wxFlexGridSizer(3, 0, 0);
   bottomGridSizer->Add(intervalStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(mIntervalTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(20,20,0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(fileTypeLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(mFileTypeComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(20,20,0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(fileNameLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(mFileNameTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(mBrowseButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   bottomGridSizer->Add(mOverrideCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(20,20,0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(20,20,0, wxALIGN_LEFT|wxALL, bsize);

   //-------------------------------------------------------
   // Add to analytical model sizer
   //-------------------------------------------------------    
   mAnaModelSizer = new wxBoxSizer(wxHORIZONTAL);
   mAnaModelSizer->Add(anaModelLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   mAnaModelSizer->Add(mAnalyticModelComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mAnaModelSizer->Add(20,20, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //-------------------------------------------------------
   // Add to pageSizer
   //------------------------------------------------------- 
   mPageSizer = new wxBoxSizer(wxVERTICAL);
   mPageSizer->Add(bottomGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mPageSizer->Add(mAnaModelSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   theMiddleSizer->Add(mPageSizer, 0, wxALIGN_CENTER|wxALL, bsize);
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void UniversePanel::LoadData()
{
   // load data from the core engine
   theSolarSystem = theGuiInterpreter->GetSolarSystemInUse();
   mAllFileTypes = theGuiInterpreter->GetPlanetarySourceTypes();
   mAnalyticModels = theGuiInterpreter->GetAnalyticModelNames();
   StringArray fileTypesInUse = theGuiInterpreter->GetPlanetarySourceTypesInUse();
   
   // load  ephemeris update interval
   Real interval = theSolarSystem->GetEphemUpdateInterval();
   mIntervalTextCtrl->SetValue(theGuiManager->ToWxString(interval));
   
   // available source
   for (unsigned int i=0; i<mAllFileTypes.size(); i++)
   {
      wxString type = mAllFileTypes[i].c_str();
      mFileTypeNameMap[type] =
         theGuiInterpreter->GetPlanetarySourceName(mAllFileTypes[i]).c_str();
      
      mFileTypeComboBox->Append(type);
   }
   
   // available analytic models
   for (unsigned int i=0; i<mAnalyticModels.size(); i++)
   {
      mAnalyticModelComboBox->Append(mAnalyticModels[i].c_str());
   }
   
   // set defaults
   mAnalyticModelComboBox->SetSelection(0);
   
   mFileTypeComboBox->SetStringSelection(fileTypesInUse[0].c_str());
   if (mFileTypeComboBox->GetStringSelection() == "Analytic")
   {
      mBrowseButton->Disable();
      mFileNameTextCtrl->Disable();
      mPageSizer->Show(mAnaModelSizer, true);
   }
   else
   {
      mBrowseButton->Enable();
      mFileNameTextCtrl->Enable();
      mPageSizer->Show(mAnaModelSizer, false);
   }
   
   mFileNameTextCtrl->
      SetValue(mFileTypeNameMap[mFileTypeComboBox->GetStringSelection()]);

   mOverrideCheckBox->SetValue(theSolarSystem->GetBooleanParameter(
      "UseTTForEphemeris"));

   mPageSizer->Layout();
   
   mObject = theSolarSystem;

   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void UniversePanel::SaveData()
{
   canClose = true;
   std::string str;
   Real interval;
   
   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   
   if (mHasTextModified)
   {
      str = mIntervalTextCtrl->GetValue();
      CheckReal(interval, str, "Interval", "Real Number >= 0.0");
   }
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      // save ephemeris update interval, if changed
      if (mHasTextModified)
      {
         theSolarSystem->SetEphemUpdateInterval(interval);
         mHasTextModified = false;
      }
      
      // save planetary file types in use, if changed
      if (mHasFileTypesInUseChanged)
      {
         mFileTypesInUse.clear();
         mFileTypesInUse.push_back(std::string(mFileTypeComboBox->GetStringSelection().c_str()));
         #if DEBUG_UNIV_PANEL
         MessageInterface::ShowMessage
            ("UniversePanel::SaveData() types=%s\n",
             mFileTypesInUse[0].c_str());
         #endif
         theGuiInterpreter->SetPlanetarySourceTypesInUse(mFileTypesInUse);
         mHasFileTypesInUseChanged = false;
      }
         
      // save planetary file name, if changed
      if (mHasFileNameChanged)
      {
         wxString type = mFileTypeComboBox->GetStringSelection();
         str = mFileNameTextCtrl->GetValue();
         std::ifstream filename(str.c_str());
         
         // Check if the file doesn't exist then stop
         if (type != "Analytic" && !filename) 
         {
            MessageInterface::PopupMessage
               (Gmat::ERROR_, mMsgFormat.c_str(),
                str.c_str(), "File Name", "File must exist");
            canClose = false;
            return;
         }
         filename.close();
         
         for (unsigned int i=0; i<mAllFileTypes.size(); i++)
         {
            wxString type = mAllFileTypes[i].c_str();
            std::string name = std::string(mFileTypeNameMap[type].c_str());
            theGuiInterpreter->SetPlanetarySourceName(mAllFileTypes[i], name);
         }
         
         mHasFileNameChanged = false;
      }

      // save analytical model, if changed 
      if (mHasAnaModelChanged)
      {
         theGuiInterpreter->
            SetAnalyticModelToUse(mAnalyticModelComboBox->GetStringSelection().c_str());
         mHasAnaModelChanged = false;
      }
      
      theSolarSystem->SetBooleanParameter("UseTTForEphemeris",
                                          mOverrideCheckBox->IsChecked());
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
}// end SaveData()


//------------------------------------------------------------------------------
// void OnBrowseButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void UniversePanel::OnBrowseButton(wxCommandEvent& event)
{
   wxString oldname = mFileNameTextCtrl->GetValue();
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
   
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString filename;
      
      filename = dialog.GetPath().c_str();

      if (!filename.IsSameAs(oldname))
      {
         mFileNameTextCtrl->SetValue(filename);
         mFileTypeNameMap[mFileTypeComboBox->GetStringSelection()] = filename;
         mHasFileNameChanged = true;
         EnableUpdate(true);
      }
   }
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void UniversePanel::OnComboBoxChange(wxCommandEvent& event)
{
   mFileNameTextCtrl->Enable();
   if (event.GetEventObject() == mFileTypeComboBox)
   {
      wxString type = mFileTypeComboBox->GetStringSelection();
      mFileNameTextCtrl->SetValue(mFileTypeNameMap[type]);

      if (type == "Analytic")
      {
         mPageSizer->Show(mAnaModelSizer, true);
         mBrowseButton->Disable();
         mFileNameTextCtrl->Disable();
      }
      else
      {
         mPageSizer->Show(mAnaModelSizer, false);
         mBrowseButton->Enable();
      }

      mPageSizer->Layout();
   }
   else if (event.GetEventObject() == mAnalyticModelComboBox)
   {
      mHasAnaModelChanged = true;
   }
   
   mHasFileTypesInUseChanged = true;
   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void UniversePanel::OnCheckBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnTextCtrlChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void UniversePanel::OnTextCtrlChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mIntervalTextCtrl)
   {
      if (mIntervalTextCtrl->IsModified())
         mHasTextModified = true;
   }
   
   if (event.GetEventObject() == mFileNameTextCtrl)
   {
      mHasFileNameChanged = true;
   }
   
   EnableUpdate(true);
}
