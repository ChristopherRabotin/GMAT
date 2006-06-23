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

//#define DEBUG_UNIV_PANEL 1

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(UniversePanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)

   EVT_BUTTON(ID_BUTTON_ADD, UniversePanel::OnAddButton)
   EVT_BUTTON(ID_BUTTON_SORT, UniversePanel::OnSortButton)
   EVT_BUTTON(ID_BUTTON_REMOVE, UniversePanel::OnRemoveButton)
   EVT_BUTTON(ID_BUTTON_CLEAR, UniversePanel::OnClearButton)
   EVT_BUTTON(ID_BUTTON_BROWSE, UniversePanel::OnBrowseButton)
   
   EVT_LISTBOX(ID_AVAILABLE_LIST, UniversePanel::OnListBoxSelect)
   
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

   int bsize = 3;
   wxString emptyArray[] = { };
   
   //-------------------------------------------------------
   // EphemerisUpdateInterval
   //-------------------------------------------------------
   wxStaticText *intervalStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Ephemeris Update Interval"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mIntervalTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""),
                                 wxDefaultPosition, wxSize(150,-1), 0);
                                 
   wxBoxSizer *intervalSizer = new wxBoxSizer(wxHORIZONTAL);
   intervalSizer->Add(intervalStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   intervalSizer->Add(mIntervalTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   #if DEBUG_UNIV_PANEL
   MessageInterface::ShowMessage
      ("UniversePanel::Create() created intervalSizer\n");
   #endif
   
   //-------------------------------------------------------
   // 1st column (Available file types)
   //-------------------------------------------------------
   wxStaticText *availableLabel =
      new wxStaticText(this, ID_TEXT, wxT("Available Planetary Source"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   availableListBox =
      new wxListBox(this, ID_AVAILABLE_LIST, wxDefaultPosition, 
                     wxSize(140,125), 0, emptyArray, wxLB_SINGLE);
   
   wxBoxSizer *availableSizer = new wxBoxSizer(wxVERTICAL);
   availableSizer->Add(availableLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   availableSizer->Add(availableListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   #if DEBUG_UNIV_PANEL
   MessageInterface::ShowMessage
      ("UniversePanel::Create() created availableSizer\n");
   #endif
   
   //-------------------------------------------------------
   // 2nd column (Add, Remove, Clear, Prioritize buttons)
   //-------------------------------------------------------
   addButton = new wxButton(this, ID_BUTTON_ADD, wxT("-->"),
                            wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
   
   removeButton = new wxButton(this, ID_BUTTON_REMOVE, wxT("<--"), 
                               wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
   
   clearButton = new wxButton(this, ID_BUTTON_CLEAR, wxT("<="), 
                              wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
   
   prioritizeButton = new wxButton(this, ID_BUTTON_SORT, wxT("Prioritize"), 
                                   wxDefaultPosition, wxDefaultSize, 
                                   wxBU_EXACTFIT);
   
   wxBoxSizer *buttonSizer = new wxBoxSizer(wxVERTICAL);
   buttonSizer->Add(20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   buttonSizer->Add(addButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   buttonSizer->Add(removeButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   buttonSizer->Add(clearButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   buttonSizer->Add(prioritizeButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   #if DEBUG_UNIV_PANEL
   MessageInterface::ShowMessage
      ("UniversePanel::Create() created buttonSizer\n");
   #endif
   
   //-------------------------------------------------------
   // 3rd column (Selected file types)
   //-------------------------------------------------------
   wxStaticText *selLabel =
      new wxStaticText(this, ID_TEXT, wxT("Selected Source"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   selectedListBox =
      new wxListBox(this, ID_SELECTED_LIST, wxDefaultPosition, 
                    wxSize(140,125), 0, emptyArray, wxLB_SINGLE);
   
   wxBoxSizer *selectedSizer = new wxBoxSizer(wxVERTICAL);
   selectedSizer->Add(selLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   selectedSizer->Add(selectedListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   #if DEBUG_UNIV_PANEL
   MessageInterface::ShowMessage
      ("UniversePanel::Create() created selectedSizer\n");
   #endif
   
   //-------------------------------------------------------
   // Add to topGridSizer
   //-------------------------------------------------------
   wxGridSizer *topGridSizer = new wxGridSizer(3, 0, 0);
   topGridSizer->Add(availableSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   topGridSizer->Add(buttonSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   topGridSizer->Add(selectedSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-------------------------------------------------------
   // file path
   //-------------------------------------------------------
   
   wxStaticText *fileTypeLabel =
      new wxStaticText(this, ID_TEXT, wxT("Planetary Source"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   wxStaticText *fileNameLabel =
      new wxStaticText(this, ID_TEXT, wxT("File Name"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   mFileTypeComboBox = 
      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition,
                     wxDefaultSize, 0, emptyArray, wxCB_READONLY);

   mFileNameTextCtrl =
      new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""),
                     wxDefaultPosition, wxSize(300, -1),  0);
   
   mBrowseButton =
      new wxButton(this, ID_BUTTON_BROWSE, wxT("Browse"),
                   wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
                   
   mOverrideCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Use TT for Ephemeris"),
                     wxDefaultPosition, wxSize(-1, -1), 0);

   
   wxFlexGridSizer *bottomGridSizer = new wxFlexGridSizer(3, 0, 0);
   bottomGridSizer->Add(fileTypeLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(fileNameLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   bottomGridSizer->Add(20,20, 0, wxALIGN_CENTRE|wxALL, bsize);
   bottomGridSizer->Add(mFileTypeComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   bottomGridSizer->Add(mFileNameTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   bottomGridSizer->Add(mBrowseButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   bottomGridSizer->Add(mOverrideCheckBox, 0, wxALIGN_CENTRE|wxALL, bsize);

   //-------------------------------------------------------
   // analytic motel
   //-------------------------------------------------------
   
   wxStaticText *anaModelLabel =
      new wxStaticText(this, ID_TEXT, wxT("Analytic Model"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   mAnalyticModelComboBox = 
      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition,
                     wxDefaultSize, 0, emptyArray, wxCB_READONLY);
   
   mAnaModelSizer = new wxBoxSizer(wxVERTICAL);
   mAnaModelSizer->Add(20, 10, 0, wxALIGN_LEFT|wxALL, bsize);
   mAnaModelSizer->Add(anaModelLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   mAnaModelSizer->Add(mAnalyticModelComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //-------------------------------------------------------
   // Add to pageSizer
   //------------------------------------------------------- 
   mPageSizer = new wxBoxSizer(wxVERTICAL);
   mPageSizer->Add(intervalSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mPageSizer->Add(topGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
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
   
   // Loading EphemerisUpdateInterval
   Real interval = theSolarSystem->GetEphemUpdateInterval();
   wxString intervalStr;
   intervalStr.Printf("%f", interval);
   mIntervalTextCtrl->SetValue(intervalStr);
      
   // available source
   for (unsigned int i=0; i<mAllFileTypes.size(); i++)
   {
      availableListBox->Append(mAllFileTypes[i].c_str());
   }
   
   wxString str;
   
   // selected source
   for (unsigned int i=0; i<fileTypesInUse.size(); i++)
   {
      str = fileTypesInUse[i].c_str();
      selectedListBox->Append(str);
      availableListBox->Delete(availableListBox->FindString(str));
      
      #if DEBUG_UNIV_PANEL
      MessageInterface::ShowMessage
         ("UniversePanel::LoadData() fileTypesInUse[%d] = %s\n", i,
          fileTypesInUse[i].c_str());
      #endif
   }
   
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
   availableListBox->SetSelection(0);
   selectedListBox->SetSelection(0);
   mAnalyticModelComboBox->SetSelection(0);
   
   if (fileTypesInUse.size() > 0)
      removeButton->Enable(true);
   else
      removeButton->Enable(false);
   
   if (fileTypesInUse.size() > 1)
      prioritizeButton->Enable(true);
   else
      prioritizeButton->Enable(false);
   
   //mFileTypeComboBox->SetSelection(0);
   mFileTypeComboBox->SetStringSelection(selectedListBox->GetString(0));
   if (mFileTypeComboBox->GetStringSelection() == "Analytic")
   {
      mBrowseButton->Disable();
      mPageSizer->Show(mAnaModelSizer, true);
   }
   else
   {
      mBrowseButton->Enable();
      mPageSizer->Show(mAnaModelSizer, false);
   }
   
   mFileNameTextCtrl->
      SetValue(mFileTypeNameMap[mFileTypeComboBox->GetStringSelection()]);

   mOverrideCheckBox->SetValue(theSolarSystem->GetBooleanParameter(
      "UseTTForEphemeris"));

   mPageSizer->Layout();
   
   mObject = theSolarSystem;

   theApplyButton->Disable();
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void UniversePanel::SaveData()
{
   // save data to core engine

   if (selectedListBox->GetCount() == 0)
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "Need to select at least one planetary source file.\n"
          "Added SLP file as default\n");
      
      selectedListBox->Insert("DE405", 0);
      selectedListBox->SetSelection(0);
   }
   else
   {
      // save planetary file name, if changed
      if (mHasFileNameChanged)
      {
         mHasFileNameChanged = false;
         
         for (unsigned int i=0; i<mAllFileTypes.size(); i++)
         {
            wxString type = mAllFileTypes[i].c_str();
            std::string name = std::string(mFileTypeNameMap[type].c_str());
            theGuiInterpreter->SetPlanetarySourceName(mAllFileTypes[i], name);
         }
      }
      
      // save planetary file types in use, if changed
      if (mHasFileTypesInUseChanged)
      {
         mHasFileTypesInUseChanged = false;
         mFileTypesInUse.clear();
         
         // put planetary file types in the priority order
         for (int i=0; i<selectedListBox->GetCount(); i++)
         {
            mFileTypesInUse.push_back(std::string(selectedListBox->GetString(i)));
         
            #if DEBUG_UNIV_PANEL
            MessageInterface::ShowMessage("UniversePanel::SaveData() types=%s\n",
                                          mFileTypesInUse[i].c_str());
            #endif
         }
         
         Integer status = theGuiInterpreter->SetPlanetarySourceTypesInUse(mFileTypesInUse);
         
         // if error opening the first file type, remove the file type from the list
         if (status == 1)
         {
            selectedListBox->Delete(0);
         }
      }
   }

   if (mHasAnaModelChanged)
   {
      mHasAnaModelChanged = false;
      theGuiInterpreter->
         SetAnalyticModelToUse(mAnalyticModelComboBox->GetStringSelection().c_str());
   }
   
   theSolarSystem->SetBooleanParameter("UseTTForEphemeris",
      mOverrideCheckBox->IsChecked());
      
   // Saving EphemerisUpdateInterval
   theSolarSystem->SetEphemUpdateInterval(atof(mIntervalTextCtrl->GetValue()));
   
   theApplyButton->Enable(false);
   
}// end SaveData()


//------------------------------------------------------------------------------
// void OnAddButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void UniversePanel::OnAddButton(wxCommandEvent& event)
{
   // get string in first list and then search for it
   // in the second list
   wxString str = availableListBox->GetStringSelection();
   int sel = availableListBox->GetSelection();
   int found = selectedListBox->FindString(str);
    
   // if no selection then do nothing
   if (sel == -1)
      return;

   // if the string wasn't found in the second list, insert it
   if (found == wxNOT_FOUND)
   {
      selectedListBox->Insert(str, 0);
      availableListBox->Delete(sel);
      selectedListBox->SetSelection(0);
      
      if (sel-1 < 0)
         availableListBox->SetSelection(0);
      else
         availableListBox->SetSelection(sel-1);
      
      removeButton->Enable(false);
      prioritizeButton->Enable(false);
    
      if (selectedListBox->GetCount() > 0)
         removeButton->Enable(true);

      if (selectedListBox->GetCount() > 1)
         prioritizeButton->Enable(true);
      
      mHasFileTypesInUseChanged = true;
      theApplyButton->Enable();
   }
}


//------------------------------------------------------------------------------
// void OnRemoveButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void UniversePanel::OnRemoveButton(wxCommandEvent& event)
{
   wxString str = selectedListBox->GetStringSelection();
   int sel = selectedListBox->GetSelection();
   
   // if no selection then do nothing
   if (sel == -1)
      return;

   selectedListBox->Delete(sel);
   availableListBox->Append(str);
   availableListBox->SetStringSelection(str);
   
   if (sel-1 < 0)
      selectedListBox->SetSelection(0);
   else
      selectedListBox->SetSelection(sel-1);
   
   removeButton->Enable(false);
   prioritizeButton->Enable(false);
   
   if (selectedListBox->GetCount() > 0)
      removeButton->Enable(true);

   if (selectedListBox->GetCount() > 1)
      prioritizeButton->Enable(true);

   mHasFileTypesInUseChanged = true;
   theApplyButton->Enable(true);
}


//------------------------------------------------------------------------------
// void OnClearButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void UniversePanel::OnClearButton(wxCommandEvent& event)
{
   Integer count = selectedListBox->GetCount();
   
   if (count == 0)
      return;
   
   for (Integer i=0; i<count; i++)
   {
      availableListBox->Append(selectedListBox->GetString(i));
   }
   
   selectedListBox->Clear();
   availableListBox->SetSelection(0);
   removeButton->Enable(false);
   prioritizeButton->Enable(false);
   
   mHasFileTypesInUseChanged = true;
   theApplyButton->Enable();
}


// moves selected item to the top of the lsit
//------------------------------------------------------------------------------
// void OnSortButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void UniversePanel::OnSortButton(wxCommandEvent& event)
{
   // get string
   wxString str = selectedListBox->GetStringSelection();
   
   if (!str.IsEmpty())
   {
      // remove string
      int sel = selectedListBox->GetSelection();
      selectedListBox->Delete(sel);
      
      // add string to top
      selectedListBox->Insert(str, 0);
      selectedListBox->SetSelection(0);
      
      if (str == "Analytic")
      {
         mPageSizer->Show(mAnaModelSizer, true);
         mBrowseButton->Disable();
      }
      else
      {
         mPageSizer->Show(mAnaModelSizer, false);
         mBrowseButton->Enable();
      }
      
      mPageSizer->Layout();
      mHasFileTypesInUseChanged = true;
      theApplyButton->Enable(true);
   }
}


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
         theApplyButton->Enable(true);
      }
   }
}


//------------------------------------------------------------------------------
// void OnListBoxSelect(wxCommandEvent& event)
//------------------------------------------------------------------------------
void UniversePanel::OnListBoxSelect(wxCommandEvent& event)
{
   // get string
   wxString s = availableListBox->GetStringSelection();

   if (selectedListBox->FindString(s) == wxNOT_FOUND)
   {
      addButton->Enable(true);
   }
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void UniversePanel::OnComboBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mFileTypeComboBox)
   {
      wxString type = mFileTypeComboBox->GetStringSelection();
      mFileNameTextCtrl->SetValue(mFileTypeNameMap[type]);

      if (type == "Analytic")
      {
         mPageSizer->Show(mAnaModelSizer, true);
         mBrowseButton->Disable();
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
   
   theApplyButton->Enable();

}

//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void UniversePanel::OnCheckBoxChange(wxCommandEvent& event)
{
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnTextCtrlChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void UniversePanel::OnTextCtrlChange(wxCommandEvent& event)
{
   theApplyButton->Enable();
}
