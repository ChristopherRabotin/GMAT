//$Header$
//------------------------------------------------------------------------------
//                              CoordPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2005/03/11
/**
 * This class contains the Coordinate System Panel for CoordSystemConfigPanel
 * and CoordSysCreateDialog.
 */
//------------------------------------------------------------------------------
#include "CoordPanel.hpp"

#include "AxisSystem.hpp"
#include "SpacePoint.hpp"

//#define DEBUG_COORD_PANEL 1

//------------------------------------------------------------------------------
// CoordPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
CoordPanel::CoordPanel(wxWindow *parent, bool enableAll)
    : wxPanel(parent)
{
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();

   mShowPrimaryBody = false;
   mShowSecondaryBody = false;
   mShowEpoch = false;
   mShowXyz = false;

   mEnableAll = enableAll;

   Create();
}

CoordPanel::~CoordPanel()
{
}

//-------------------------------
// private methods
//-------------------------------
void CoordPanel::Create()
{
   Setup(this);
   LoadData();
}

void CoordPanel::Setup( wxWindow *parent)
{
    // wxStaticText
   originStaticText = new wxStaticText( parent, ID_TEXT, wxT("Origin"),
      wxDefaultPosition, wxDefaultSize, 0 );
   typeStaticText = new wxStaticText( parent, ID_TEXT, wxT("Type"),
      wxDefaultPosition, wxDefaultSize, 0 );
   primaryStaticText = new wxStaticText( parent, ID_TEXT, wxT("Primary"),
      wxDefaultPosition, wxDefaultSize, 0 );
   formatStaticText = new wxStaticText( parent, ID_TEXT, wxT("Epoch Format"),
      wxDefaultPosition, wxDefaultSize, 0 );
   secondaryStaticText = new wxStaticText( parent, ID_TEXT, wxT("Secondary"),
      wxDefaultPosition, wxDefaultSize, 0 );
   epochStaticText = new wxStaticText( parent, ID_TEXT, wxT("Epoch"),
      wxDefaultPosition, wxDefaultSize, 0 );

   xStaticText = new wxStaticText( parent, ID_TEXT, wxT("X: "),
      wxDefaultPosition, wxDefaultSize, 0 );
   yStaticText = new wxStaticText( parent, ID_TEXT, wxT("Y: "),
      wxDefaultPosition, wxDefaultSize, 0 );
   zStaticText = new wxStaticText( parent, ID_TEXT, wxT("Z: "),
      wxDefaultPosition, wxDefaultSize, 0 );

    // wxTextCtrl
    wxString emptyList[] = {};

    // wxComboBox
   originComboBox = theGuiManager->GetSpacePointComboBox(this, ID_COMBO,
      wxSize(120,-1), false);
   typeComboBox = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition,
      wxSize(120,-1), 0, emptyList, wxCB_DROPDOWN );
   primaryComboBox = theGuiManager->GetSpacePointComboBox(this, ID_COMBO,
      wxSize(120,-1), false);
   formatComboBox = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition,
      wxSize(120,-1), 0, emptyList, wxCB_DROPDOWN );
   secondaryComboBox = theGuiManager->GetSpacePointComboBox(this, ID_COMBO,
      wxSize(120,-1), false);

   xComboBox = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition,
      wxSize(50,-1), 0, emptyList, wxCB_DROPDOWN );
   yComboBox = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition,
      wxSize(50,-1), 0, emptyList, wxCB_DROPDOWN );
   zComboBox = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition,
      wxSize(50,-1), 0, emptyList, wxCB_DROPDOWN );

   //wxTextCtrl
   epochTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(120,-1), 0 );

   // wx*Sizers
   wxBoxSizer *theMainSizer = new wxBoxSizer( wxVERTICAL );
   wxStaticBox *staticbox1 = new wxStaticBox( parent, -1, wxT("Axes") );
   wxStaticBoxSizer *staticboxsizer1 = new wxStaticBoxSizer( staticbox1,
      wxVERTICAL );
   wxFlexGridSizer *flexgridsizer1 = new wxFlexGridSizer( 3, 4, 0, 0 );
   wxBoxSizer *boxsizer1 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *boxsizer2 = new wxBoxSizer( wxHORIZONTAL );

   boxsizer1->Add( originStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer1->Add( originComboBox, 0, wxALIGN_CENTER|wxALL, 5 );

   // row 1
   flexgridsizer1->Add( typeStaticText, 0, wxALIGN_LEFT|wxALL, 5 );
   flexgridsizer1->Add( typeComboBox, 0, wxALIGN_LEFT|wxALL, 5 );
   flexgridsizer1->Add( 20, 20, 0, wxALIGN_LEFT|wxALL, 5 );
   flexgridsizer1->Add( 20, 20, 0, wxALIGN_LEFT|wxALL, 5 );

   // row 2
   flexgridsizer1->Add( primaryStaticText, 0, wxALIGN_LEFT|wxALL, 5 );
   flexgridsizer1->Add( primaryComboBox, 0, wxALIGN_LEFT|wxALL, 5 );
   flexgridsizer1->Add( secondaryStaticText, 0, wxALIGN_LEFT|wxALL, 5 );
   flexgridsizer1->Add( secondaryComboBox, 0, wxALIGN_LEFT|wxALL, 5 );

   // row 3
   flexgridsizer1->Add( formatStaticText, 0, wxALIGN_LEFT|wxALL, 5 );
   flexgridsizer1->Add( formatComboBox, 0, wxALIGN_LEFT|wxALL, 5 );
   flexgridsizer1->Add( epochStaticText, 0, wxALIGN_LEFT|wxALL, 5 );
   flexgridsizer1->Add( epochTextCtrl, 0, wxALIGN_LEFT|wxALL, 5 );

   boxsizer2->Add(xStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer2->Add(xComboBox, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer2->Add(yStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer2->Add(yComboBox, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer2->Add(zStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer2->Add(zComboBox, 0, wxALIGN_CENTER|wxALL, 5 );

   staticboxsizer1->Add( flexgridsizer1, 0, wxALIGN_CENTER|wxALL, 5 );
   staticboxsizer1->Add( boxsizer2, 0, wxALIGN_CENTER|wxALL, 5 );

   theMainSizer->Add(boxsizer1, 0, wxALIGN_CENTRE|wxALL, 5);
   theMainSizer->Add(staticboxsizer1, 0, wxALIGN_CENTRE|wxALL, 5);

   this->SetAutoLayout( true );
   this->SetSizer( theMainSizer );
   theMainSizer->Fit( this );
   theMainSizer->SetSizeHints( this );
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void CoordPanel::LoadData()
{
   try
   {
      // Load axes types
      StringArray itemNames =
         theGuiInterpreter->GetListOfFactoryItems(Gmat::AXIS_SYSTEM);
      for (unsigned int i = 0; i<itemNames.size(); i++)
         typeComboBox->Append(wxString(itemNames[i].c_str()));
     
      // insert a blank option for primary and secondary
//      primaryComboBox->Append("");
      secondaryComboBox->Append("");
         
      // Load epoch types - hard coded for now
      wxString epochStrs[] =
      {
         wxT("TAIModJulian"),
         wxT("UTCModJulian"),
         wxT("TAIGregorian"),
         wxT("UTCGregorian"),
      };

      for (unsigned int i = 0; i<4; i++)
         formatComboBox->Append(wxString(epochStrs[i].c_str()));

      wxString xyzStrs[] =
      {
         wxT(""),
         wxT("R"),
         wxT("-R"),
         wxT("V"),
         wxT("-V"),
         wxT("N"),
         wxT("-N"),
      };

      for (unsigned int i=0; i<7; i++)
      {
         xComboBox->Append(wxString(xyzStrs[i].c_str()));
         yComboBox->Append(wxString(xyzStrs[i].c_str()));
         zComboBox->Append(wxString(xyzStrs[i].c_str()));
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("CoordPanel:LoadData() error occurred!\n%s\n",
            e.GetMessage().c_str());
   }
}

void CoordPanel::EnableOptions()
{
   #if DEBUG_COORD_PANEL
      MessageInterface::ShowMessage("CoordPanel::EnableOptions() type =%s\n",
      typeComboBox->GetStringSelection().c_str());
   #endif

   if ((typeComboBox->GetStringSelection() == "Equator") ||
      (typeComboBox->GetStringSelection() == "BodyFixed"))
   {
      mShowPrimaryBody = false;   // 05/03/05: ag changed from true to false
      mShowSecondaryBody = false;
      mShowEpoch = false;
      mShowXyz = false;
   }
   else if (typeComboBox->GetStringSelection() == "ObjectReferenced")
   {
      mShowPrimaryBody = true;
      mShowSecondaryBody = true;
      mShowEpoch = false;
      mShowXyz = true;
   }
   else if ((typeComboBox->GetStringSelection() == "TOEEq") ||
            (typeComboBox->GetStringSelection() == "TOEEc") ||
            (typeComboBox->GetStringSelection() == "MOEEq") ||
            (typeComboBox->GetStringSelection() == "MOEEc"))
   {
      mShowPrimaryBody = false;
      mShowSecondaryBody = false;
      mShowEpoch = true;
      mShowXyz = false;
   }
   else
   {
      mShowPrimaryBody = false;
      mShowSecondaryBody = false;
      mShowEpoch = false;
      mShowXyz = false;
   }

   if (mEnableAll)
   {
      primaryStaticText->Enable(mShowPrimaryBody);
      primaryComboBox->Enable(mShowPrimaryBody);
      secondaryStaticText->Enable(mShowSecondaryBody);
      secondaryComboBox->Enable(mShowSecondaryBody);
      formatStaticText->Enable(mShowEpoch);
      formatComboBox->Enable(mShowEpoch);
      epochStaticText->Enable(mShowEpoch);
      epochTextCtrl->Enable(mShowEpoch);
      xStaticText->Enable(mShowXyz);
      xComboBox->Enable(mShowXyz);
      yStaticText->Enable(mShowXyz);
      yComboBox->Enable(mShowXyz);
      zStaticText->Enable(mShowXyz);
      zComboBox->Enable(mShowXyz);
   }
   else  // disable all of them
   {
      originStaticText->Enable(mEnableAll);
      typeStaticText->Enable(mEnableAll);
      primaryStaticText->Enable(mEnableAll);
      formatStaticText->Enable(mEnableAll);
      secondaryStaticText->Enable(mEnableAll);
      epochStaticText->Enable(mEnableAll);
      originComboBox->Enable(mEnableAll);
      typeComboBox->Enable(mEnableAll);
      primaryComboBox->Enable(mEnableAll);
      formatComboBox->Enable(mEnableAll);
      secondaryComboBox->Enable(mEnableAll);
      epochTextCtrl->Enable(mEnableAll);
      xStaticText->Enable(mEnableAll);
      xComboBox->Enable(mEnableAll);
      yStaticText->Enable(mEnableAll);
      yComboBox->Enable(mEnableAll);
      zStaticText->Enable(mEnableAll);
      zComboBox->Enable(mEnableAll);
   }
}
