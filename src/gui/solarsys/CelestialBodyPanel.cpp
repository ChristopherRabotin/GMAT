//$Header$
//------------------------------------------------------------------------------
//                              CelestialBodyPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2005/07/06
//
/**
 * Implements CelestialBodyPanel class.
 */
//------------------------------------------------------------------------------

#include "CelestialBodyPanel.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_PARAM_PANEL 1
const std::string
CelestialBodyPanel::KEP_ELEMENT_NAMES[6] =
{
   "SMA",
   "ECC",
   "INC",
   "RAAN",
   "AOP",
   "TA",
};

const std::string
CelestialBodyPanel::KEP_ELEMENT_UNITS[6] =
{
   "Km",
   "Km",
   "Km",
   "Km/s",
   "Km/s",
   "Km/s",
};
//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CelestialBodyPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   
   EVT_TEXT(ID_TEXTCTRL, CelestialBodyPanel::OnTextUpdate)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// CelestialBodyPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
CelestialBodyPanel::CelestialBodyPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent, true)
{
   theCelestialBody = (CelestialBody*)
            theGuiInterpreter->GetConfiguredItem(std::string(name.c_str()));

   Create();
   Show();
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void CelestialBodyPanel::Create()
{
   int bsize = 3; // border size

   wxStaticBox *staticBox = new wxStaticBox(this, -1, wxT(""));

   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *horizontalBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 2, 0, 0 );
   wxFlexGridSizer *flexGridSizer = new wxFlexGridSizer( 3, 0, 0 );
   wxStaticBoxSizer *staticBoxSizer = new wxStaticBoxSizer(staticBox, wxVERTICAL);

   mEpochTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                 wxDefaultPosition, wxSize(150,-1), 0);
   mElement1TextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                 wxDefaultPosition, wxSize(150,-1), 0);
   mElement2TextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                 wxDefaultPosition, wxSize(150,-1), 0);
   mElement3TextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                 wxDefaultPosition, wxSize(150,-1), 0);
   mElement4TextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                 wxDefaultPosition, wxSize(150,-1), 0);
   mElement5TextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                 wxDefaultPosition, wxSize(150,-1), 0);
   mElement6TextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                 wxDefaultPosition, wxSize(150,-1), 0);

   epochStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Initial Epoch: "),
                       wxDefaultPosition, wxDefaultSize, 0);
   initialStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Initial State with Respect to Central Body: "),
                       wxDefaultPosition, wxDefaultSize, 0);
   centralBodyText=
      new wxStaticText(this, ID_TEXT, wxT("central body"),
                       wxDefaultPosition, wxDefaultSize, 0);
   centralBodyText->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxBOLD));
   noCentralBodyText=
      new wxStaticText(this, ID_TEXT, wxT(""),
                       wxDefaultPosition, wxDefaultSize, 0);

   nameStaticText1 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_NAMES[0].c_str(),
                       wxDefaultPosition, wxDefaultSize, 0);
   unitStaticText1 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_UNITS[0].c_str(),
                       wxDefaultPosition, wxDefaultSize, 0);

   nameStaticText2 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_NAMES[1].c_str(),
                       wxDefaultPosition, wxDefaultSize, 0);
   unitStaticText2 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_UNITS[1].c_str(),
                       wxDefaultPosition, wxDefaultSize, 0);
                       
   nameStaticText3 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_NAMES[2].c_str(),
                       wxDefaultPosition, wxDefaultSize, 0);
   unitStaticText3 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_UNITS[2].c_str(),
                       wxDefaultPosition, wxDefaultSize, 0);
                       
   nameStaticText4 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_NAMES[3].c_str(),
                       wxDefaultPosition, wxDefaultSize, 0);
   unitStaticText4 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_UNITS[3].c_str(),
                       wxDefaultPosition, wxDefaultSize, 0);
                       
   nameStaticText5 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_NAMES[4].c_str(),
                       wxDefaultPosition, wxDefaultSize, 0);
   unitStaticText5 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_UNITS[4].c_str(),
                       wxDefaultPosition, wxDefaultSize, 0);
                       
   nameStaticText6 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_NAMES[5].c_str(),
                       wxDefaultPosition, wxDefaultSize, 0);
   unitStaticText6 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_UNITS[5].c_str(),
                       wxDefaultPosition, wxDefaultSize, 0);


   flexGridSizer1->Add( epochStaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer1->Add( mEpochTextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   horizontalBoxSizer->Add( initialStaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   horizontalBoxSizer->Add( centralBodyText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   flexGridSizer->Add( nameStaticText1, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer->Add( mElement1TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer->Add( unitStaticText1, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   flexGridSizer->Add( nameStaticText2, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer->Add( mElement2TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer->Add( unitStaticText2, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   flexGridSizer->Add( nameStaticText3, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer->Add( mElement3TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer->Add( unitStaticText3, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   flexGridSizer->Add( nameStaticText4, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer->Add( mElement4TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer->Add( unitStaticText4, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   flexGridSizer->Add( nameStaticText5, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer->Add( mElement5TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer->Add( unitStaticText5, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   flexGridSizer->Add( nameStaticText6, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer->Add( mElement6TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer->Add( unitStaticText6, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   staticBoxSizer->Add(horizontalBoxSizer, 0, wxALIGN_CENTER | wxALL, bsize);
   staticBoxSizer->Add(flexGridSizer, 0, wxALIGN_CENTER | wxALL, bsize);

   pageBoxSizer->Add(noCentralBodyText, 0, wxALIGN_CENTER | wxALL, bsize);
   pageBoxSizer->Add(flexGridSizer1, 0, wxALIGN_CENTER | wxALL, bsize);
   pageBoxSizer->Add(staticBoxSizer, 0, wxALIGN_CENTER | wxALL, bsize);
   theMiddleSizer->Add(pageBoxSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void CelestialBodyPanel::LoadData()
{
   if (theCelestialBody->GetBodyType() != Gmat::STAR)
   {
      std::string centralBody = theCelestialBody->GetCentralBody();
      centralBodyText->SetLabel(centralBody.c_str());
   }
   else
   {
      EnableAll(false);
   }

}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void CelestialBodyPanel::SaveData()
{


}

//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CelestialBodyPanel::OnTextUpdate(wxCommandEvent& event)
{
   theApplyButton->Enable(true);
}

//------------------------------------------------------------------------------
// void EnableAll(bool enable)
//------------------------------------------------------------------------------
void CelestialBodyPanel::EnableAll(bool enable)
{
   mEpochTextCtrl->Enable(enable);
   mElement1TextCtrl->Enable(enable);
   mElement2TextCtrl->Enable(enable);
   mElement3TextCtrl->Enable(enable);
   mElement4TextCtrl->Enable(enable);
   mElement5TextCtrl->Enable(enable);
   mElement6TextCtrl->Enable(enable);

   epochStaticText->Enable(enable);
   initialStaticText->Enable(enable);
   centralBodyText->Enable(enable);

   nameStaticText1->Enable(enable);
   unitStaticText1->Enable(enable);
   nameStaticText2->Enable(enable);
   unitStaticText2->Enable(enable);
   nameStaticText3->Enable(enable);
   unitStaticText3->Enable(enable);
   nameStaticText4->Enable(enable);
   unitStaticText4->Enable(enable);
   nameStaticText5->Enable(enable);
   unitStaticText5->Enable(enable);
   nameStaticText6->Enable(enable);
   unitStaticText6->Enable(enable);

//   wxStaticText *noCentralBodyText;
}


