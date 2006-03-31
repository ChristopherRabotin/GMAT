//$Header$
//------------------------------------------------------------------------------
//                              TextEphemFileDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2006/03/29
//
/**
 * Shows dialog where various run script folder option can be selected.
 */
//------------------------------------------------------------------------------

#include "TextEphemFileDialog.hpp"
#include "FileManager.hpp"
#include "TextEphemFile.hpp"
#include "MessageInterface.hpp"

#include <wx/dir.h>
#include <wx/filename.h>

//#define DEBUG_EPHEM_FILE_DIALOG 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(TextEphemFileDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, TextEphemFileDialog::OnButtonClick)
END_EVENT_TABLE()

   
//------------------------------------------------------------------------------
// TextEphemFileDialog(wxWindow *parent)
//------------------------------------------------------------------------------
TextEphemFileDialog::TextEphemFileDialog(wxWindow *parent)
   : GmatDialog(parent, -1, wxString(_T("TextEphemFileDialog")))
{
   Create();
   ShowData();
}


//------------------------------------------------------------------------------
// ~TextEphemFileDialog()
//------------------------------------------------------------------------------
TextEphemFileDialog::~TextEphemFileDialog()
{
   theGuiManager->UnregisterListBox("Spacecraft", mSpacecraftListBox);
   theGuiManager->UnregisterComboBox("CoordinateSystem", mCoordSysComboBox);
}


//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void TextEphemFileDialog::Create()
{
   #if DEBUG_EPHEM_FILE_DIALOG
   MessageInterface::ShowMessage("TextEphemFileDialog::Create() entered.\n");
   #endif
   
   int bsize = 2;
   wxString emptyList[] = {};
   
   //------------------------------------------------------
   // Ephemeris file parameters
   //------------------------------------------------------
   
   //----- Ephemeris header filename
   wxStaticText *headerFileLabel =
      new wxStaticText(this, ID_TEXT, wxT("Header File:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mHeaderFileTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(200,20), 0);
   
   mHeaderFileBrowseButton =
      new wxButton(this, ID_BUTTON, wxT("Browse"),
                    wxDefaultPosition, wxSize(60,20), 0);

   //----- Ephemeris data filename
   wxStaticText *dataFileLabel =
      new wxStaticText(this, ID_TEXT, wxT("Data File:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mDataFileTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(200,20), 0);
   
   mDataFileBrowseButton =
      new wxButton(this, ID_BUTTON, wxT("Browse"),
                    wxDefaultPosition, wxSize(60,20), 0);

   //----- Interval
   wxStaticText *intervalLabel =
      new wxStaticText(this, ID_TEXT, wxT("Interval:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mIntervalTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("60"),
                     wxDefaultPosition, wxSize(80,20), 0);
   
   wxStaticText *intervalUnitLabel =
      new wxStaticText(this, ID_TEXT, wxT("Seconds"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   //----- Output CoordinateSystem
   wxStaticText *coordSysLabel =
      new wxStaticText(this, ID_TEXT, wxT("Coordinate System:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mCoordSysComboBox =
      theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX, wxSize(120,-1));

   //----- Output epoch format
   wxStaticText *epochFormatLabel =
      new wxStaticText(this, ID_TEXT, wxT("Epoch Format:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mEpochFormatComboBox =
      new wxComboBox(this, ID_COMBOBOX, wxT(""),
                     wxDefaultPosition, wxSize(150,-1), 0, emptyList,
                     wxCB_DROPDOWN | wxCB_READONLY);

   wxFlexGridSizer *fileSizer = new wxFlexGridSizer(3, 0, 0);
   fileSizer->Add(headerFileLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   fileSizer->Add(mHeaderFileTextCtrl, 0, wxALIGN_CENTER|wxALL|wxGROW, bsize);
   fileSizer->Add(mHeaderFileBrowseButton, 0, wxALIGN_LEFT|wxALL, bsize);
   
   fileSizer->Add(dataFileLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   fileSizer->Add(mDataFileTextCtrl, 0, wxALIGN_CENTER|wxALL|wxGROW, bsize);
   fileSizer->Add(mDataFileBrowseButton, 0, wxALIGN_LEFT|wxALL, bsize);
   
   fileSizer->Add(intervalLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   fileSizer->Add(mIntervalTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   fileSizer->Add(intervalUnitLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   
   fileSizer->Add(coordSysLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   fileSizer->Add(mCoordSysComboBox, 0, wxALIGN_CENTER|wxALL|wxGROW, bsize);
   fileSizer->Add(20, 20, 0, wxALIGN_LEFT|wxALL, bsize);
   
   fileSizer->Add(epochFormatLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   fileSizer->Add(mEpochFormatComboBox, 0, wxALIGN_CENTER|wxALL|wxGROW, bsize);
   fileSizer->Add(20, 20, 0, wxALIGN_LEFT|wxALL, bsize);

   fileSizer->Add(20, 5, 0, wxALIGN_RIGHT|wxALL, bsize);
   fileSizer->Add(20, 5, 0, wxALIGN_CENTER|wxALL|wxGROW, bsize);
   fileSizer->Add(20, 5, 0, wxALIGN_LEFT|wxALL, bsize);

   
   //----- Spacecraft selection
   wxStaticText *scLabel =
      new wxStaticText(this, ID_TEXT, wxT("Spacecraft"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   wxStaticText *selScLabel =
      new wxStaticText(this, ID_TEXT, wxT("Selected Spacecraft"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mSpacecraftListBox = theGuiManager->
      GetSpacecraftListBox(this, ID_LISTBOX, wxSize(150,65));
   
   mSelectedScListBox =
      new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                    wxSize(150,65), 0, emptyList, wxLB_SINGLE);
   
   #if __WXMAC__
   int buttonWidth = 40;
   #else
   int buttonWidth = 25;
   #endif
   
   mAddScButton = new wxButton(this, ID_BUTTON, wxT("-->"),
                               wxDefaultPosition, wxSize(buttonWidth,20), 0);
   mRemoveScButton = new wxButton(this, ID_BUTTON, wxT("<--"),
                                 wxDefaultPosition, wxSize(buttonWidth,20), 0);
   mClearScButton = new wxButton(this, ID_BUTTON, wxT("<="),
                                wxDefaultPosition, wxSize(buttonWidth,20), 0);

   
   wxBoxSizer *arrowsSizer = new wxBoxSizer(wxVERTICAL);
   arrowsSizer->Add(mAddScButton, 0, wxALIGN_CENTER|wxALL, bsize);
   arrowsSizer->Add(mRemoveScButton, 0, wxALIGN_CENTER|wxALL, bsize);
   arrowsSizer->Add(mClearScButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   wxFlexGridSizer *scGridSizer = new wxFlexGridSizer(3, 0, 0);
   scGridSizer->Add(scLabel, 0, wxALIGN_CENTER|wxALL, bsize);
   scGridSizer->Add(20, 20, 0, wxALIGN_CENTER|wxALL, bsize);
   scGridSizer->Add(selScLabel, 0, wxALIGN_CENTER|wxALL, bsize);
   
   scGridSizer->Add(mSpacecraftListBox, 0, wxALIGN_CENTER|wxALL, bsize);
   scGridSizer->Add(arrowsSizer, wxALIGN_CENTER|wxALL, bsize);
   scGridSizer->Add(mSelectedScListBox, 0, wxALIGN_CENTER|wxALL, bsize);
   
   wxBoxSizer *scSizer = new wxBoxSizer(wxVERTICAL);
   scSizer->Add(scGridSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
   //------------------------------------------------------
   // Run
   //------------------------------------------------------
   
   //mSeparateFilesCheckBox =
   //   new wxCheckBox(this, ID_CHECKBOX, wxT("Generate Separate Header and Data File"),
   //                  wxDefaultPosition, wxDefaultSize, 0);
   
   mRunButton =
      new wxButton(this, ID_BUTTON, wxT("Run and Create Ephemeris File"),
                   wxDefaultPosition, wxDefaultSize, 0);
   
   wxBoxSizer *runSizer = new wxBoxSizer(wxVERTICAL);
   //runSizer->Add(mSeparateFilesCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   runSizer->Add(20, 5, 0, wxALIGN_LEFT|wxALL, bsize);
   runSizer->Add(mRunButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   //------------------------------------------------------
   // add to page sizer
   //------------------------------------------------------
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   
   pageBoxSizer->Add(fileSizer, 0, wxALIGN_CENTER|wxALL|wxGROW, bsize);
   pageBoxSizer->Add(scSizer, 0, wxALIGN_CENTER|wxALL|wxGROW, bsize);
   pageBoxSizer->Add(runSizer, 0, wxALIGN_CENTER|wxALL|wxGROW, bsize);

   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTER|wxALL|wxGROW, bsize);
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void TextEphemFileDialog::LoadData()
{
   // Get default path for ephemeris file
   FileManager *fm = FileManager::Instance();
   mEphemDirectory = fm->GetAbsPathname(FileManager::OUTPUT_PATH).c_str();
   
   // Append first spacecraft to selected listbox
   int scCount = mSpacecraftListBox->GetCount();
   if (scCount > 0)
   {
      wxString scName = mSpacecraftListBox->GetStringSelection();
      mSelectedScListBox->Append(scName);
      mSelectedScListBox->SetStringSelection(scName);

      // set default ephemeris headr/data file using spacecraft name
      wxString fname = "/" + scName + "_EphemHeader.txt";
      mHeaderFileTextCtrl->SetValue(mEphemDirectory.c_str() + fname);

      fname = "/" + scName + "_EphemData.txt";
      mDataFileTextCtrl->SetValue(mEphemDirectory.c_str() + fname);
      
      mRunButton->Enable();
   }
   else
   {
      mHeaderFileTextCtrl->SetValue(mEphemDirectory.c_str() +
                                    wxString("/TextEphemHeader.txt"));
      mDataFileTextCtrl->SetValue(mEphemDirectory.c_str() +
                                  wxString("/TextEphemData.txt"));
      
      mRunButton->Disable();
   }

   // Show last position
   mHeaderFileTextCtrl->SetInsertionPointEnd();
   mDataFileTextCtrl->SetInsertionPointEnd();
   
   mCoordSysComboBox->SetSelection(0);
   
   // Load the epoch formats
   StringArray reps = TimeConverterUtil::GetValidTimeRepresentations();
   for (unsigned int i = 0; i < reps.size(); i++)
      mEpochFormatComboBox->Append(reps[i].c_str());
   
   mEpochFormatComboBox->SetSelection(0);
   //mSeparateFilesCheckBox->Enable();

   theOkButton->Enable();
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void TextEphemFileDialog::SaveData()
{
}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void TextEphemFileDialog::ResetData()
{
   canClose = true;
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void TextEphemFileDialog::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mHeaderFileBrowseButton)
   {
      wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
    
      if (dialog.ShowModal() == wxID_OK)
      {
         mEphemDirectory = dialog.GetPath();
         mHeaderFileTextCtrl->SetValue(mEphemDirectory);
         mHeaderFileTextCtrl->SetInsertionPointEnd();
         #if DEBUG_EPHEM_FILE_DIALOG
         MessageInterface::ShowMessage
            ("TextEphemFileDialog::OnButtonClick() mEphemDirectory=%s\n",
             mEphemDirectory.c_str());
         #endif
      }
   }
   else if (event.GetEventObject() == mDataFileBrowseButton)
   {
      wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
    
      if (dialog.ShowModal() == wxID_OK)
      {
         mEphemDirectory = dialog.GetPath();
         mDataFileTextCtrl->SetValue(mEphemDirectory);
         mDataFileTextCtrl->SetInsertionPointEnd();
         
         #if DEBUG_EPHEM_FILE_DIALOG
         MessageInterface::ShowMessage
            ("TextEphemFileDialog::OnButtonClick() mEphemDirectory=%s\n",
             mEphemDirectory.c_str());
         #endif
      }
   }
   else if (event.GetEventObject() == mAddScButton)
   {
      wxString s = mSpacecraftListBox->GetStringSelection();
      int found = mSelectedScListBox->FindString(s);
      
      if (found == wxNOT_FOUND)
      {
         mSelectedScListBox->Append(s);
         mSelectedScListBox->SetStringSelection(s);
      
         // select next available item
         mSpacecraftListBox->
            SetSelection(mSpacecraftListBox->GetSelection()+1);

         mRunButton->Enable();
         
         //if (mSelectedScListBox->GetCount() > 1)
         //{
         //   mSeparateFilesCheckBox->SetValue(true);
         //   mSeparateFilesCheckBox->Disable();
         //}
         //else
         //{
         //   mSeparateFilesCheckBox->SetValue(false);
         //   mSeparateFilesCheckBox->Enable();
         //}
      }
   }
   else if (event.GetEventObject() == mRemoveScButton)
   {
      int sel = mSelectedScListBox->GetSelection();
      mSelectedScListBox->Delete(sel);
   
      if (sel-1 < 0)
         mSelectedScListBox->SetSelection(0);
      else
         mSelectedScListBox->SetSelection(sel-1);

      //if (mSelectedScListBox->GetCount() > 1)
      //{
      //   mSeparateFilesCheckBox->SetValue(true);
      //   mSeparateFilesCheckBox->Disable();
      //}
      //else
      //{
      //   mSeparateFilesCheckBox->SetValue(false);
      //   mSeparateFilesCheckBox->Enable();
      //}
      
      mRunButton->Enable();
      if (mSelectedScListBox->GetCount() == 0)
         mRunButton->Disable();

   }
   else if (event.GetEventObject() == mClearScButton)
   {
      mSelectedScListBox->Clear();
      //mSeparateFilesCheckBox->SetValue(false);
      //mSeparateFilesCheckBox->Enable();
      mRunButton->Disable();
   }
   else if (event.GetEventObject() == mRunButton)
   {
      if (CreateTextEphem())
      {
         // Run mission
         GmatAppData::GetMainFrame()->OnScriptRun(event);
      }
   }
}


//------------------------------------------------------------------------------
// bool CreateTextEphem()
//------------------------------------------------------------------------------
bool TextEphemFileDialog::CreateTextEphem()
{
   std::string headerFileName = mHeaderFileTextCtrl->GetValue().c_str();
   std::string dataFileName = mDataFileTextCtrl->GetValue().c_str();
   
   TextEphemFile *ephemFile = (TextEphemFile*)(theGuiInterpreter->
      CreateSubscriber("TextEphemFile", "TextEphemFile", dataFileName, false));

   // get first spacecraft from the list
   std::string scName = mSelectedScListBox->GetString(0).c_str();
   std::string epochFormat = mEpochFormatComboBox->GetValue().c_str();
   std::string coordSys = mCoordSysComboBox->GetValue().c_str();

   // check interval
   Real interval;
   if (!mIntervalTextCtrl->GetValue().ToDouble(&interval))
   {
      wxMessageBox("Invalid Interval Entered.");
      return false;
   }
   
   // Add parameters
   std::string time = scName + "." + epochFormat;
   std::string xpos = scName + "." + coordSys + ".X";
   std::string ypos = scName + "." + coordSys + ".Y";
   std::string zpos = scName + "." + coordSys + ".Z";
   std::string xvel = scName + "." + coordSys + ".VX";
   std::string yvel = scName + "." + coordSys + ".VY";
   std::string zvel = scName + "." + coordSys + ".VZ";

   try
   {
      ephemFile->SetStringParameter("HeaderFile", headerFileName);
      ephemFile->SetStringParameter("Add", time);
      ephemFile->SetStringParameter("Add", xpos);
      ephemFile->SetStringParameter("Add", ypos);
      ephemFile->SetStringParameter("Add", zpos);
      ephemFile->SetStringParameter("Add", xvel);
      ephemFile->SetStringParameter("Add", yvel);
      ephemFile->SetStringParameter("Add", zvel);
      
      ephemFile->SetStringParameter("EpochFormat", epochFormat);
      ephemFile->SetStringParameter("CoordinateSystem", coordSys);
      ephemFile->SetRealParameter("Interval", interval);
      
      #if DEBUG_EPHEM_FILE_DIALOG
      StringArray params = ephemFile->GetRefObjectNameArray(Gmat::PARAMETER);
      MessageInterface::ShowMessage
         ("TextEphemFileDialog:CreateTextEphem() params=\n");
      for (UnsignedInt i=0; i<params.size(); i++)
         MessageInterface::ShowMessage("%s\n", params[i].c_str());
      #endif

   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("TextEphemFileDialog:CreateTextEphem() error occurred!\n%s\n",
          e.GetMessage().c_str());

      return false;
   }
   
   return true;
}

