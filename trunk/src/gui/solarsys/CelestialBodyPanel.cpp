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
#include "StringUtil.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_PARAM_PANEL 1
//#define DEBUG_CELESBODY_SAVE 1

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
   "km",
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
      theGuiInterpreter->GetConfiguredObject(name.c_str());
   
   thePlanet = (Planet*)theCelestialBody;         
   bodyName = name.c_str();
   
   isTextModified = false;
   isStateTextModified = false;
   isRotDataSourceChanged = false;
   
   Create();
   Show();
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void CelestialBodyPanel::Create()
{
   int bsize = 2; // border size

   wxStaticBox *staticBox = new wxStaticBox(this, -1, wxT(""));
   wxStaticBox *analyticStaticBox = new wxStaticBox(this, -1, wxT("Low Fidelity Model"));

   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *horizontalBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   wxFlexGridSizer *flexGridSizer0 = new wxFlexGridSizer( 3, 0, 0 );
   wxFlexGridSizer *flexGridSizer = new wxFlexGridSizer( 3, 0, 0 );
   wxStaticBoxSizer *staticBoxSizer = new wxStaticBoxSizer(staticBox, wxVERTICAL);
   wxStaticBoxSizer *analyticBoxSizer = new wxStaticBoxSizer(analyticStaticBox, wxVERTICAL);
   wxBoxSizer *horizontalBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;

   // Mu
   muStaticText = new wxStaticText(this, ID_TEXT, wxT("Mu"), wxDefaultPosition,
      wxDefaultSize, 0);
   mMuTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition,
      wxSize(150,-1), 0);
   unitStaticTextMu = new wxStaticText(this, ID_TEXT, wxT("km^3/s^2"), 
      wxDefaultPosition, wxDefaultSize, 0);

   // Equatorial Radius
   equatorialRadiusStaticText = new wxStaticText(this, ID_TEXT, 
      wxT("Equatorial Radius"),wxDefaultPosition,wxDefaultSize, 0);
   mEquatorialRadiusTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0);
   unitStaticTextER =  new wxStaticText(this, ID_TEXT, wxT("km"), 
      wxDefaultPosition, wxDefaultSize, 0);

   // Flattening
   flatteningStaticText = new wxStaticText(this, ID_TEXT, wxT("Flattening"),
      wxDefaultPosition, wxDefaultSize, 0);
   mFlatteningTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0);
   unitStaticTextFlat =  new wxStaticText(this, ID_TEXT, wxT(""), 
      wxDefaultPosition, wxDefaultSize, 0);

   // Nutation Update Interval
   intervalStaticText =
      new wxStaticText(this, ID_TEXT, "Nutation Update Interval",
         wxDefaultPosition, wxDefaultSize, 0);
   mIntervalTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0);
   unitStaticTextinterval =
      new wxStaticText(this, ID_TEXT, wxT("sec"), wxDefaultPosition, 
         wxDefaultSize, 0);

   // Initial State with Respect to Central Body
   initialStaticText =
      new wxStaticText(this, ID_TEXT, 
         wxT("Initial State with Respect to Central Body: "), wxDefaultPosition, 
         wxDefaultSize, 0);
   centralBodyText =
      new wxStaticText(this, ID_TEXT, wxT("central body"),
                       wxDefaultPosition, wxDefaultSize, 0);
   centralBodyText->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxBOLD));
   noCentralBodyText =
      new wxStaticText(this, ID_TEXT, wxT(""),
                       wxDefaultPosition, wxDefaultSize, 0);

   // Initial Epoch
   epochStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Initial Epoch"), wxDefaultPosition,
         wxDefaultSize, 0);
   mEpochTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0);
   unitStaticTextepoch =
      new wxStaticText(this, ID_TEXT, wxT("A1ModJulian"), wxDefaultPosition, 
         wxDefaultSize, 0);

   // Elements
   nameStaticText1 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_NAMES[0].c_str(),
         wxDefaultPosition, wxDefaultSize, 0);
   mElement1TextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0);
   unitStaticText1 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_UNITS[0].c_str(),
         wxDefaultPosition, wxDefaultSize, 0);

   nameStaticText2 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_NAMES[1].c_str(),
         wxDefaultPosition, wxDefaultSize, 0);
   mElement2TextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0);
   unitStaticText2 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_UNITS[1].c_str(),
         wxDefaultPosition, wxDefaultSize, 0);

   nameStaticText3 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_NAMES[2].c_str(),
         wxDefaultPosition, wxDefaultSize, 0);
   mElement3TextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0);
   unitStaticText3 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_UNITS[2].c_str(),
         wxDefaultPosition, wxDefaultSize, 0);

   nameStaticText4 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_NAMES[3].c_str(),
         wxDefaultPosition, wxDefaultSize, 0);
   mElement4TextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0);
   unitStaticText4 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_UNITS[3].c_str(),
         wxDefaultPosition, wxDefaultSize, 0);

   nameStaticText5 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_NAMES[4].c_str(),
         wxDefaultPosition, wxDefaultSize, 0);
   mElement5TextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0);
   unitStaticText5 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_UNITS[4].c_str(),
         wxDefaultPosition, wxDefaultSize, 0);

   nameStaticText6 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_NAMES[5].c_str(),
         wxDefaultPosition, wxDefaultSize, 0);
   mElement6TextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0);
   unitStaticText6 =
      new wxStaticText(this, ID_TEXT, KEP_ELEMENT_UNITS[5].c_str(),
         wxDefaultPosition, wxDefaultSize, 0);
                                 
   // Rotation Data Source
   rotDataSourceText =
      new wxStaticText(this, ID_TEXT, "Rotation Data Source",
         wxDefaultPosition, wxDefaultSize, 0);
   rotDataSourceCB = new wxComboBox
      ( this, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(150,-1), //0,
        emptyList, wxCB_DROPDOWN | wxCB_READONLY );

   flexGridSizer0->Add( muStaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer0->Add( mMuTextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer0->Add( unitStaticTextMu, 0, wxGROW|wxALIGN_CENTER|wxALL, 
                        bsize);

   flexGridSizer0->Add( equatorialRadiusStaticText, 0, 
                        wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer0->Add( mEquatorialRadiusTextCtrl, 0, 
                        wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer0->Add( unitStaticTextER, 0, wxGROW|wxALIGN_CENTER|wxALL, 
                        bsize);

   flexGridSizer0->Add( flatteningStaticText, 0, 
                        wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer0->Add( mFlatteningTextCtrl, 0, 
                        wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer0->Add( unitStaticTextFlat, 0, wxGROW|wxALIGN_CENTER|wxALL, 
                        bsize);

   flexGridSizer0->Add( intervalStaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, 
                        bsize);
   flexGridSizer0->Add( mIntervalTextCtrl, 20, wxGROW|wxALIGN_CENTER|wxALL, 
                        bsize);
   flexGridSizer0->Add( unitStaticTextinterval, 0, wxGROW|wxALIGN_CENTER|wxALL, 
                        bsize);

   flexGridSizer->Add( epochStaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer->Add( mEpochTextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer->Add( unitStaticTextepoch, 0, wxGROW|wxALIGN_CENTER|wxALL, 
                        bsize);

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
   pageBoxSizer->Add(flexGridSizer0, 0, wxALIGN_CENTER | wxALL, bsize);
   pageBoxSizer->Add(staticBoxSizer, 0, wxALIGN_CENTER | wxALL, bsize);
   pageBoxSizer->Add(horizontalBoxSizer2, 0, wxALIGN_CENTER | wxALL, bsize);

   analyticBoxSizer->Add(pageBoxSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
   theMiddleSizer->Add(analyticBoxSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
  
   if (bodyName == "Earth")
   {
      intervalStaticText->Show(true);
      mIntervalTextCtrl->Show(true);
      unitStaticTextinterval->Show(true);
   }
   else
   {
      intervalStaticText->Show(false);
      mIntervalTextCtrl->Show(false);
      unitStaticTextinterval->Show(false);
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
      Real rMu;
      Real equatorialRadius; 
      Real flattening;
      Real epoch;
      Rvector6 elements;
     
      rMu = theCelestialBody->GetRealParameter(
                      theCelestialBody->GetParameterID("Mu"));

      equatorialRadius = theCelestialBody->GetRealParameter(
                      theCelestialBody->GetParameterID("EquatorialRadius"));

      flattening = theCelestialBody->GetRealParameter(
                      theCelestialBody->GetParameterID("Flattening"));
      
      if (theCelestialBody->GetBodyType() != Gmat::STAR)
      {
         Real intervalUpdate;
         wxString intervalStr;

         centralBody = theCelestialBody->GetCentralBody();
         epoch = theCelestialBody->GetAnalyticEpoch().Get();
         elements = theCelestialBody->GetAnalyticElements();

         centralBodyText->SetLabel(centralBody.c_str());
         
         if (theCelestialBody->GetBodyType() == Gmat::PLANET)
         {
             intervalUpdate = thePlanet->GetNutationUpdateInterval();
             mIntervalTextCtrl->
                SetValue(theGuiManager->ToWxString(intervalUpdate));
         }
         
         if (theCelestialBody->GetName() == "Luna")
         {
            wxString rotStrs[] =
            {
               wxT("DE405"),
               wxT("IAU2002"),
            };
      
            for (unsigned int i=0; i<2; i++)
               rotDataSourceCB->Append(wxString(rotStrs[i].c_str()));
            
            int dataSourceIndex = theCelestialBody->GetRotationDataSource();
            
            if (dataSourceIndex == Gmat::DE_FILE)
               rotDataSourceCB->SetValue("DE405");
            else if (dataSourceIndex == Gmat::IAU_DATA)
               rotDataSourceCB->SetValue("IAU2002");
            else
               rotDataSourceCB->SetValue("");
            
         }
      }
      else
      {
         noCentralBodyText->SetLabel("Computing from initial Earth elements");
         EnableAll(false);

         centralBody = theCelestialBody->GetCentralBody();
         epoch = theCelestialBody->GetAnalyticEpoch().Get();
         elements = theCelestialBody->GetAnalyticElements();

         centralBodyText->Show(false);
         initialStaticText->Show(false);
      }
      
      
      wxString epochStr, elementStr;

      mMuTextCtrl->SetValue(theGuiManager->ToWxString(rMu));
      mEquatorialRadiusTextCtrl->SetValue(
                                 theGuiManager->ToWxString(equatorialRadius));
      mFlatteningTextCtrl->SetValue(
                                 theGuiManager->ToWxString(flattening));

      mEpochTextCtrl->SetValue(theGuiManager->ToWxString(epoch));
      mElement1TextCtrl->SetValue(theGuiManager->ToWxString(elements.Get(0)));
      mElement2TextCtrl->SetValue(theGuiManager->ToWxString(elements.Get(1)));
      mElement3TextCtrl->SetValue(theGuiManager->ToWxString(elements.Get(2)));
      mElement4TextCtrl->SetValue(theGuiManager->ToWxString(elements.Get(3)));
      mElement5TextCtrl->SetValue(theGuiManager->ToWxString(elements.Get(4)));
      mElement6TextCtrl->SetValue(theGuiManager->ToWxString(elements.Get(5)));

   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("CelestialBodyPanel:LoadData() error occurred!\n%s\n",
            e.GetFullMessage().c_str());
   }

   // Activate "ShowScript"
   mObject = theCelestialBody;
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void CelestialBodyPanel::SaveData()
{
   #if DEBUG_CELESBODY_SAVE
   MessageInterface::ShowMessage
      ("CelestialBodyPanel::SaveData() isTextModified=%d, isStateTextModified=%d,"
       " isRotDataSourceChanged=%d\n", isTextModified, isStateTextModified,
       isRotDataSourceChanged);
   #endif
   
   canClose = true;
   std::string str;
   Real mu, radius, flattening, interval;
   
   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   
   if (isTextModified)
   {
      str = mMuTextCtrl->GetValue();
      CheckReal(mu, str, "Mu", "Real Number > 0.0");
      
      str = mEquatorialRadiusTextCtrl->GetValue();
      CheckReal(radius, str, "EquatorialRadius", "Real Number > 0.0");
      
      str = mFlatteningTextCtrl->GetValue();
      CheckReal(flattening, str, "Flattening", "Real Number");
      
      if (theCelestialBody->GetBodyType() == Gmat::PLANET)
      {
         str = mIntervalTextCtrl->GetValue();
         CheckReal(interval, str, "Nutation Update Interval", "Real Number >= 0.0");
      }
      
   }
   
   //-----------------------------------------------------------------
   // check values from state text field
   //-----------------------------------------------------------------
   
   Real initEpoch, elements[6];
   
   if (isStateTextModified)
   {
      str = mEpochTextCtrl->GetValue();
      CheckReal(initEpoch, str, "Initial Epoch", "Real Number");
      
      str =  mElement1TextCtrl->GetValue();
      CheckReal(elements[0], str, "SMA", "Real Number");
      
      str =  mElement2TextCtrl->GetValue();
      CheckReal(elements[1], str, "ECC", "Real Number");
      
      str =  mElement3TextCtrl->GetValue();
      CheckReal(elements[2], str, "INC", "Real Number");
      
      str =  mElement4TextCtrl->GetValue();
      CheckReal(elements[3], str, "RAAN", "Real Number");
      
      str =  mElement5TextCtrl->GetValue();
      CheckReal(elements[4], str, "AOP", "Real Number");
      
      str =  mElement6TextCtrl->GetValue();
      CheckReal(elements[5], str, "TA", "Real Number");
   }
   
   #if DEBUG_CELESBODY_SAVE
   MessageInterface::ShowMessage("   canClose=%d\n", canClose);
   #endif
   
   if (!canClose)
      return;
   
   #if DEBUG_CELESBODY_SAVE
   MessageInterface::ShowMessage("   Saving to base code\n");
   #endif
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      Integer paramID;
      theCelestialBody = thePlanet;

      // To show all the error messages, multiple try/catch block is used
      // Is it OK to catch one error at a time?
      // If so, we don't need multiple try/catch block
      
      if (isTextModified)
      {
         // Mu
         try
         {
            paramID = theCelestialBody->GetParameterID("Mu");
            theCelestialBody->SetRealParameter(paramID, mu);
         }
         catch (BaseException &e)
         {
            MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
            canClose = false;
         }
         
         // EquatorialRadius
         try
         {
            paramID = theCelestialBody->GetParameterID("EquatorialRadius");
            theCelestialBody->SetRealParameter(paramID, radius);
         }
         catch (BaseException &e)
         {
            MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
            canClose = false;
         }
         
         // Flattening
         paramID = theCelestialBody->GetParameterID("Flattening");
         theCelestialBody->SetRealParameter(paramID, flattening);
         
         // Nutation Update Interval
         if (theCelestialBody->GetBodyType() == Gmat::PLANET)
         {
                 try
                 {
                    thePlanet->SetNutationUpdateInterval(interval);
                 }
                 catch (BaseException &e)
                 {
                    MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
                    canClose = false;
                 }
//            thePlanet->SetUpdateInterval(interval);
         }
         
         if (canClose)
            isTextModified = false;
      }
      
      if (isStateTextModified)
      {         
         // Initial Epoch
         A1Mjd a1mjd =  A1Mjd(initEpoch);
         theCelestialBody->SetAnalyticEpoch(a1mjd);
         
         Rvector6 state(elements);
         
         // Set SMA, for range check
         try
         {
            theCelestialBody->SetSMA(state[0]);
         }
         catch (BaseException &e)
         {
            MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
            canClose = false;
         }
         
         // Set ECC, for range check
         try
         {
            theCelestialBody->SetECC(state[1]);
         }
         catch (BaseException &e)
         {
            MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
            canClose = false;
         }
         
         if (canClose)
         {
            theCelestialBody->SetAnalyticElements(state);
            isStateTextModified = false;
         }
      }
      
      if (isRotDataSourceChanged && theCelestialBody->GetName() == "Luna")
      {
         wxString rotDataString = rotDataSourceCB->GetValue();
         
         if (rotDataString == "DE405")
            theCelestialBody->SetRotationDataSource(Gmat::DE_FILE);
         else if (rotDataString == "IAU2002")
            theCelestialBody->SetRotationDataSource(Gmat::IAU_DATA);
         else
            theCelestialBody->SetRotationDataSource(Gmat::NOT_APPLICABLE);
         
         isRotDataSourceChanged = false;
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
}


//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CelestialBodyPanel::OnTextUpdate(wxCommandEvent& event)
{
   if (mMuTextCtrl->IsModified() || mEquatorialRadiusTextCtrl->IsModified() ||
       mFlatteningTextCtrl->IsModified() || mIntervalTextCtrl->IsModified())
   {
      isTextModified = true;
      EnableUpdate(true);
   }
   
   if (mEpochTextCtrl->IsModified() ||
       mElement1TextCtrl->IsModified() || mElement2TextCtrl->IsModified() ||
       mElement3TextCtrl->IsModified() || mElement4TextCtrl->IsModified() ||
       mElement5TextCtrl->IsModified() || mElement6TextCtrl->IsModified())
   {
      isStateTextModified = true;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CelestialBodyPanel::OnComboBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == rotDataSourceCB)
   {
      isRotDataSourceChanged = true;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void EnableAll(bool enable)
//------------------------------------------------------------------------------
void CelestialBodyPanel::EnableAll(bool enable)
{
   mMuTextCtrl->Enable(enable);
   mEquatorialRadiusTextCtrl->Enable(enable);
   mFlatteningTextCtrl->Enable(enable);
   mEpochTextCtrl->Enable(enable);
   mElement1TextCtrl->Enable(enable);
   mElement2TextCtrl->Enable(enable);
   mElement3TextCtrl->Enable(enable);
   mElement4TextCtrl->Enable(enable);
   mElement5TextCtrl->Enable(enable);
   mElement6TextCtrl->Enable(enable);

   muStaticText->Enable(enable);
   unitStaticTextMu->Enable(enable);
   equatorialRadiusStaticText->Enable(enable);
   unitStaticTextER->Enable(enable);
   flatteningStaticText->Enable(enable);
   unitStaticTextFlat->Enable(enable);
   epochStaticText->Enable(enable);
   unitStaticTextepoch->Enable(enable);
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
}


