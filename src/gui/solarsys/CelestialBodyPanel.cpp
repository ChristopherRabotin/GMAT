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
   "",
   "deg",
   "deg",
   "deg",
   "deg",
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
   EVT_COMBOBOX(ID_COMBO, CelestialBodyPanel::OnComboBoxChange)
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
   
   thePlanet = (Planet*)theCelestialBody;         
   bodyName = name.c_str();

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
   wxStaticBox *analyticStaticBox = new wxStaticBox(this, -1, wxT("Low Fidelity Model"));

   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *horizontalBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 2, 0, 0 );
   wxFlexGridSizer *flexGridSizer = new wxFlexGridSizer( 3, 0, 0 );
   wxStaticBoxSizer *staticBoxSizer = new wxStaticBoxSizer(staticBox, wxVERTICAL);
   wxStaticBoxSizer *analyticBoxSizer = new wxStaticBoxSizer(analyticStaticBox, wxVERTICAL);
   wxBoxSizer *horizontalBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
   wxString emptyList[] = {};

   mEpochTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                 wxDefaultPosition, wxSize(150,-1), 0);
   mIntervalTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
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
   intervalStaticText =
      new wxStaticText(this, ID_TEXT, "Nutation Update Interval",
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

   rotDataSourceText =
      new wxStaticText(this, ID_TEXT, "Rotation Data Source",
                       wxDefaultPosition, wxDefaultSize, 0);
                       
   rotDataSourceCB = new wxComboBox( this, ID_COMBO, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0, emptyList, wxCB_DROPDOWN | wxCB_READONLY );

   flexGridSizer1->Add( epochStaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer1->Add( mEpochTextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer1->Add( intervalStaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer1->Add( mIntervalTextCtrl, 20, wxGROW|wxALIGN_CENTER|wxALL, bsize);

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

   horizontalBoxSizer2->Add( rotDataSourceText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   horizontalBoxSizer2->Add( rotDataSourceCB, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   staticBoxSizer->Add(horizontalBoxSizer, 0, wxALIGN_CENTER | wxALL, bsize);
   staticBoxSizer->Add(flexGridSizer, 0, wxALIGN_CENTER | wxALL, bsize);

   pageBoxSizer->Add(noCentralBodyText, 0, wxALIGN_CENTER | wxALL, bsize);
   pageBoxSizer->Add(flexGridSizer1, 0, wxALIGN_CENTER | wxALL, bsize);
   pageBoxSizer->Add(staticBoxSizer, 0, wxALIGN_CENTER | wxALL, bsize);
   pageBoxSizer->Add(horizontalBoxSizer2, 0, wxALIGN_CENTER | wxALL, bsize);

   analyticBoxSizer->Add(pageBoxSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
   theMiddleSizer->Add(analyticBoxSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
  
   if (bodyName == "Earth")
   {
      intervalStaticText->Show(true);
      mIntervalTextCtrl->Show(true);
   }
   else
   {
      intervalStaticText->Show(false);
      mIntervalTextCtrl->Show(false);
   }
   
   if (bodyName == "Luna")
   {
      rotDataSourceText->Show(true);
      rotDataSourceCB->Show(true);   
   }
   else
   {
      rotDataSourceText->Show(false);
      rotDataSourceCB->Show(false);   
   }
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void CelestialBodyPanel::LoadData()
{
   try
   {
      std::string centralBody;
      Real epoch;
      Rvector6 elements;
      
      if (theCelestialBody->GetBodyType() != Gmat::STAR)
      {
         Real intervalUpdate;
         wxString intervalStr;

         centralBody = theCelestialBody->GetCentralBody();
         epoch = theCelestialBody->GetLowFidelityEpoch().Get();
         elements = theCelestialBody->GetLowFidelityElements();

         centralBodyText->SetLabel(centralBody.c_str());
         
         if (theCelestialBody->GetBodyType() == Gmat::PLANET)
         {
             intervalUpdate = thePlanet->GetUpdateInterval();
             intervalStr.Printf("%f", intervalUpdate);
             mIntervalTextCtrl->SetValue(intervalStr);
         }
         
         if (theCelestialBody->GetName() == "Luna")
         {
            wxString rotStrs[] =
            {
               wxT("DeFile"),
               wxT("IAU Data"),
            };
      
            for (unsigned int i=0; i<2; i++)
               rotDataSourceCB->Append(wxString(rotStrs[i].c_str()));
            
            int dataSourceIndex = theCelestialBody->GetRotationDataSource();
            
            if (dataSourceIndex == Gmat::DE_FILE)
               rotDataSourceCB->SetValue("DeFile");
            else if (dataSourceIndex == Gmat::IAU_DATA)
               rotDataSourceCB->SetValue("IAU Data");
            else
               rotDataSourceCB->SetValue("");
            
         }
      }
      else
      {
         noCentralBodyText->SetLabel("Computing from initial Earth elements");
         EnableAll(false);

         centralBody = theCelestialBody->GetCentralBody();
         epoch = theCelestialBody->GetLowFidelityEpoch().Get();
         elements = theCelestialBody->GetLowFidelityElements();

         centralBodyText->Show(false);
         initialStaticText->Show(false);
      }
      
      
      wxString epochStr, elementStr;

      epochStr.Printf("%f", epoch);
      mEpochTextCtrl->SetValue(epochStr);

      elementStr.Printf("%f", elements.Get(0));
      mElement1TextCtrl->SetValue(elementStr);

      elementStr.Printf("%f", elements.Get(1));
      mElement2TextCtrl->SetValue(elementStr);

      elementStr.Printf("%f", elements.Get(2));
      mElement3TextCtrl->SetValue(elementStr);

      elementStr.Printf("%f", elements.Get(3));
      mElement4TextCtrl->SetValue(elementStr);

      elementStr.Printf("%f", elements.Get(4));
      mElement5TextCtrl->SetValue(elementStr);

      elementStr.Printf("%f", elements.Get(5));
      mElement6TextCtrl->SetValue(elementStr);

   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("CelestialBodyPanel:LoadData() error occurred!\n%s\n",
            e.GetMessage().c_str());
   }

   // Activate "ShowScript"
   mObject = theCelestialBody;
   theApplyButton->Enable(false);
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void CelestialBodyPanel::SaveData()
{
   try
   {
      if (theCelestialBody->GetBodyType() == Gmat::PLANET)
      {
          thePlanet->SetUpdateInterval(atof(mIntervalTextCtrl->GetValue()));
      }
      
      theCelestialBody = thePlanet;
      
      A1Mjd a1mjd =  A1Mjd(atof(mEpochTextCtrl->GetValue()));
      theCelestialBody->SetLowFidelityEpoch(a1mjd);

      Rvector6 elements;
      elements.Set(atof(mElement1TextCtrl->GetValue()),
                  atof(mElement2TextCtrl->GetValue()),
                  atof(mElement3TextCtrl->GetValue()),
                  atof(mElement4TextCtrl->GetValue()),
                  atof(mElement5TextCtrl->GetValue()),
                  atof(mElement6TextCtrl->GetValue()));

      theCelestialBody->SetLowFidelityElements(elements);
      
       if (theCelestialBody->GetName() == "Luna")
       {
          wxString rotDataString = rotDataSourceCB->GetValue();

          if (rotDataString == "DeFile")
            theCelestialBody->SetRotationDataSource(Gmat::DE_FILE);
          else if (rotDataString == "IAU Data")
            theCelestialBody->SetRotationDataSource(Gmat::IAU_DATA);
          else
            theCelestialBody->SetRotationDataSource(Gmat::NOT_APPLICABLE);
       }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("CelestialBodyPanel:SaveData() error occurred!\n%s\n",
            e.GetMessage().c_str());
   }

}

//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CelestialBodyPanel::OnTextUpdate(wxCommandEvent& event)
{
   theApplyButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CelestialBodyPanel::OnComboBoxChange(wxCommandEvent& event)
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


