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

      if (mCoordSys != NULL)
      {
         // will it ever be anything besides null?
      }
      else
      {
         // default settings
         typeComboBox->SetValue("MJ2000Eq");
         originComboBox->SetValue("Earth");
         primaryComboBox->SetValue("Earth");
         secondaryComboBox->SetValue("Earth");
         formatComboBox->SetValue("TAIModJulian");
         epochTextCtrl->SetValue("21545");
         xComboBox->SetValue("R");
         yComboBox->SetValue("V");;
         zComboBox->SetValue("N");
      }

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
   try
   {
      std::string wxCoordName = std::string(nameTextCtrl->GetValue().Trim());
      std::string wxOriginName = std::string(originComboBox->GetValue().Trim());
      std::string wxPrimName = std::string(primaryComboBox->GetValue().Trim());
      std::string wxSecName = std::string(secondaryComboBox->GetValue().Trim());
      std::string wxTypeName = std::string(typeComboBox->GetValue().Trim());
      std::string wxFormatName = std::string(formatComboBox->GetValue().Trim());
      std::string wxXString = std::string(xComboBox->GetValue().Trim());
      std::string wxYString = std::string(yComboBox->GetValue().Trim());
      std::string wxZString = std::string(zComboBox->GetValue().Trim());


      Real epoch = atof(epochTextCtrl->GetValue());

      // need to check if coord name is null

      if (theGuiInterpreter->GetCoordinateSystem(wxCoordName) == NULL)
      {
         mCoordSys = theGuiInterpreter->CreateCoordinateSystem(wxCoordName);
         mCoordName = wxCoordName.c_str();

         mCoordSys->SetStringParameter("OriginName", wxOriginName);

         AxisSystem *axis = (AxisSystem *)theGuiInterpreter->
                  CreateAxisSystem(wxTypeName, wxTypeName);

         if (axis != NULL)
         {
            mCoordSys->SetRefObject(axis, Gmat::AXIS_SYSTEM, "");
   //         mCoordSys->SetRefObject(axis, Gmat::AXIS_SYSTEM, axis->GetName());

            SpacePoint *primary = (SpacePoint *)theGuiInterpreter->
                     GetConfiguredItem(wxPrimName);
            SpacePoint *secondary = (SpacePoint *)theGuiInterpreter->
                     GetConfiguredItem(wxSecName);

            if (primary != NULL)
               axis->SetPrimaryObject(primary);
            else
            {
               SolarSystem *ss = theGuiInterpreter->GetDefaultSolarSystem();
               axis->SetPrimaryObject((SpacePoint *)ss->GetBody(wxPrimName));
            }

            if (secondary != NULL)
               axis->SetSecondaryObject(secondary);
            else
            {
               SolarSystem *ss = theGuiInterpreter->GetDefaultSolarSystem();
               axis->SetSecondaryObject((SpacePoint *)ss->GetBody(wxSecName));
            }

            // get the x, y, z
            /// @todo : Check to see if x,y,z are valid axes
            axis->SetXAxis(wxXString);
            axis->SetYAxis(wxYString);
            axis->SetZAxis(wxZString);

            axis->SetEpoch(epoch);
            mIsCoordCreated = true;

            mCoordPanel->EnableOptions();
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("CoordSysCreateDialog:SaveData() error occurred!\n%s\n",
            e.GetMessage().c_str());
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
void CoordSysCreateDialog::OnOK()
{
   SaveData();
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
      /// @todo OnComboBoxChange epoch format
   }

   if (nameTextCtrl->GetValue().Trim() != "")
      theOkButton->Enable();
}
