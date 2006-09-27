//$Header$
//------------------------------------------------------------------------------
//                              CoordSysCreateDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2005/03/03
//
/**
 * Implements CoordSysCreateDialog class. This class shows dialog window where a
 * coordinate system can be created.
 * 
 */
//------------------------------------------------------------------------------

#include "CoordSysCreateDialog.hpp"
#include "MessageInterface.hpp"
#include "CoordinateSystem.hpp"
#include "AxisSystem.hpp"

//#define DEBUG_COORD_DIALOG 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CoordSysCreateDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, CoordSysCreateDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_TEXT(ID_TEXTCTRL, CoordSysCreateDialog::OnTextUpdate)
   EVT_COMBOBOX(ID_COMBO, CoordSysCreateDialog::OnComboBoxChange)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// CoordSysCreateDialog(wxWindow *parent)
//------------------------------------------------------------------------------
CoordSysCreateDialog::CoordSysCreateDialog(wxWindow *parent)
   : GmatDialog(parent, -1, wxString(_T("CoordSysCreateDialog")))
{
   mIsCoordCreated = false;
   Create();
   ShowData();
}

//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void CoordSysCreateDialog::Create()
{
    // wxStaticText
    nameStaticText = new wxStaticText( this, ID_TEXT,
         wxT("Coordinate System Name"), wxDefaultPosition, wxDefaultSize, 0 );

    // wxTextCtrl
    nameTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
         wxDefaultPosition, wxSize(160,-1), 0 );

    mCoordPanel = new CoordPanel(this, true);

    // wx*Sizers
    wxBoxSizer *boxsizer1 = new wxBoxSizer( wxVERTICAL );

    boxsizer1->Add( nameStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    boxsizer1->Add( nameTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );

    theMiddleSizer->Add(boxsizer1, 0, wxALIGN_CENTRE|wxALL, 5);
    theMiddleSizer->Add( mCoordPanel, 0, wxALIGN_CENTRE|wxALL, 5);
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void CoordSysCreateDialog::LoadData()
{
   try
   {
      epochTextCtrl = mCoordPanel->GetEpochTextCtrl();

      originComboBox = mCoordPanel->GetOriginComboBox();
      typeComboBox = mCoordPanel->GetTypeComboBox();
      primaryComboBox = mCoordPanel->GetPrimaryComboBox();
      formatComboBox = mCoordPanel->GetFormatComboBox();
      secondaryComboBox = mCoordPanel->GetSecondaryComboBox();

      xComboBox = mCoordPanel->GetXComboBox();
      yComboBox = mCoordPanel->GetYComboBox();
      zComboBox = mCoordPanel->GetZComboBox();

      intervalTextCtrl = mCoordPanel->GetIntervalTextCtrl();

      mCoordPanel->SetDefaultAxis();
            
      wxFormatName = formatComboBox->GetValue().Trim();
      mCoordPanel->EnableOptions();
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("CoordSysCreateDialog:LoadData() error occurred!\n%s\n",
            e.GetMessage().c_str());
   }
   
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void CoordSysCreateDialog::SaveData()
{
   mCanClose = true;
   std::string coordName = std::string(nameTextCtrl->GetValue().Trim().c_str());
   
   CoordinateSystem *coord =
      (CoordinateSystem*)theGuiInterpreter->GetObject(coordName);
   
   if (mIsCoordCreated)
   {
      AxisSystem *axis =
         (AxisSystem *)coord->GetRefObject(Gmat::AXIS_SYSTEM, "");
      
      mCanClose = mCoordPanel->SaveData(coordName, axis, wxFormatName);
   }
   else
   {
      if (coord == NULL)
      {
         // create AxisSystem
         AxisSystem *axis = mCoordPanel->CreateAxis();
         
         if (axis != NULL)
         {
            mCanClose = mCoordPanel->SaveData(coordName, axis, wxFormatName);
            mIsCoordCreated = true;
            mCoordName = wxString(coordName.c_str());
         }
      }
      else
      {
         wxLogError("The CoordinateSystem: " + wxString(coordName.c_str()) +
                    " already exist. Please enter different name\n");
      }
   }
}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void CoordSysCreateDialog::ResetData()
{
}

//------------------------------------------------------------------------------
// void OnOK()
//------------------------------------------------------------------------------
/**
 * Saves the data and closes the page
 */
//------------------------------------------------------------------------------
void CoordSysCreateDialog::OnOK(wxCommandEvent &event)
{
   SaveData();
   
   if (mIsCoordCreated && mCanClose)
      Close();
}

//---------------------------------
// event handling
//---------------------------------

//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CoordSysCreateDialog::OnTextUpdate(wxCommandEvent& event)
{
   if (nameTextCtrl->GetValue().Trim() != "")
   {
      theOkButton->Enable();
   }
   else
      theOkButton->Disable();
}

//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CoordSysCreateDialog::OnComboBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == originComboBox)
   {
   }
   else if (event.GetEventObject() == typeComboBox)
   {
      mCoordPanel->EnableOptions();
   }
   else if (event.GetEventObject() == formatComboBox)
   {
      mCoordPanel->ChangeEpoch(wxFormatName);
   }
   
   if (nameTextCtrl->GetValue().Trim() != "")
      theOkButton->Enable();
}

