//$Header:
//------------------------------------------------------------------------------
//                            TankPanel
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
// Author: Waka Waktola
// Created: 2004/11/19
/**
 * This class contains information needed to setup users spacecraft tank 
 * parameters.
 */
//------------------------------------------------------------------------------
#include "TankPanel.hpp"
#include "MessageInterface.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(TankPanel, wxPanel)
   EVT_TEXT(ID_TEXTCTRL, TankPanel::OnTextChange)
   EVT_LISTBOX(ID_LISTBOX, TankPanel::OnSelect)
   EVT_BUTTON(ID_BUTTON, TankPanel::OnButtonClick)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// TankPanel(wxWindow *parent, Spacecraft *spacecraft, wxButton *theApplyButton)
//------------------------------------------------------------------------------
/**
 * Constructs TankPanel object.
 */
//------------------------------------------------------------------------------
TankPanel::TankPanel(wxWindow *parent, Spacecraft *spacecraft,
                     wxButton *theApplyButton):wxPanel(parent)
{
    this->theSpacecraft = spacecraft;
    this->theApplyButton = theApplyButton;
    
//    theTanks.clear();
//    theFuelTanks.clear();
//    fuelTankNames.clear();
    
    tankCount = 0;
    currentTank = 0;
    
    Create();
}

//------------------------------------------------------------------------------
// ~TankPanel()
//------------------------------------------------------------------------------
TankPanel::~TankPanel()
{
}

//-------------------------------
// private methods
//-------------------------------

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void TankPanel::Create()
{
   // wxString
   wxString emptyList[] = {};
   
   // wxButton
   addButton = new wxButton( this, ID_BUTTON, wxT("Add Tank"),
                             wxDefaultPosition, wxDefaultSize, 0 );
   removeButton = new wxButton( this, ID_BUTTON, wxT("Remove Tank"),
                             wxDefaultPosition, wxDefaultSize, 0 );
   
   // wxTextCtrl
   temperatureTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
   refTemperatureTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
   fuelMassTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
   fuelDensityTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
   pressureTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
   volumeTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
    
   //wxStaticText
   temperatureStaticText = new wxStaticText( this, ID_TEXT, wxT("Temperature"),
                            wxDefaultPosition,wxDefaultSize, 0);
   refTemperatureStaticText = new wxStaticText( this, ID_TEXT, wxT("Reference Temperature"),
                            wxDefaultPosition,wxDefaultSize, 0);
   fuelMassStaticText = new wxStaticText( this, ID_TEXT, wxT("Fuel Mass"),
                            wxDefaultPosition,wxDefaultSize, 0);
   fuelDensityStaticText = new wxStaticText( this, ID_TEXT, wxT("Fuel Density"),
                            wxDefaultPosition,wxDefaultSize, 0);
   pressureStaticText = new wxStaticText( this, ID_TEXT, wxT("Pressure"),
                            wxDefaultPosition,wxDefaultSize, 0);
   volumeStaticText = new wxStaticText( this, ID_TEXT, wxT("Volume"),
                            wxDefaultPosition,wxDefaultSize, 0);  
   unit1StaticText = new wxStaticText( this, ID_TEXT, wxT("C"),
                            wxDefaultPosition,wxDefaultSize, 0); 
   unit2StaticText = new wxStaticText( this, ID_TEXT, wxT("C"),
                            wxDefaultPosition,wxDefaultSize, 0); 
   unit3StaticText = new wxStaticText( this, ID_TEXT, wxT("kg"),
                            wxDefaultPosition,wxDefaultSize, 0); 
   unit4StaticText = new wxStaticText( this, ID_TEXT, wxT("kg/m^3"),
                            wxDefaultPosition,wxDefaultSize, 0); 
   unit5StaticText = new wxStaticText( this, ID_TEXT, wxT("kPa"),
                            wxDefaultPosition,wxDefaultSize, 0); 
   unit6StaticText = new wxStaticText( this, ID_TEXT, wxT("m^3"),
                            wxDefaultPosition,wxDefaultSize, 0); 
                            
   // wxListBox
   tankListBox = new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                    wxSize(150,200), tankCount, emptyList, wxLB_SINGLE);
                            
   Integer bsize = 3; // border size
   
    // wx*Sizers      
   wxBoxSizer *boxSizer1 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer2 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *boxSizer3 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer4 = new wxBoxSizer( wxVERTICAL );
   
   wxStaticBox *staticBox1 = new wxStaticBox( this, -1, wxT("Tank List") );
   wxStaticBoxSizer *staticBoxSizer1 = new wxStaticBoxSizer( staticBox1, wxVERTICAL );
   wxStaticBox *staticBox2 = new wxStaticBox( this, -1, wxT("Tank Parameters") );
   wxStaticBoxSizer *staticBoxSizer2 = new wxStaticBoxSizer( staticBox2, wxVERTICAL );
   
   wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 3, 0, 0 );
   wxFlexGridSizer *flexGridSizer2 = new wxFlexGridSizer( 2, 0, 0 );

   // Add to wx*Sizers 
   flexGridSizer1->Add(temperatureStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(temperatureTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(unit1StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(refTemperatureStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(refTemperatureTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(unit2StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(fuelMassStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(fuelMassTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(unit3StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(fuelDensityStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(fuelDensityTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(unit4StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(pressureStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(pressureTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(unit5StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(volumeStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(volumeTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(unit6StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   
   boxSizer2->Add(addButton, 0, wxALIGN_CENTER|wxALL, bsize );
   boxSizer2->Add(removeButton, 0, wxALIGN_CENTER|wxALL, bsize );
    
   boxSizer3->Add(flexGridSizer1, 0, wxALIGN_CENTER|wxALL, bsize );
   
   boxSizer4->Add(tankListBox, 0, wxALIGN_CENTER|wxALL, bsize );
   boxSizer4->Add(boxSizer2, 0, wxALIGN_CENTER|wxALL, bsize );
      
   staticBoxSizer1->Add( boxSizer4, 0, wxALIGN_CENTER|wxALL, bsize ); 
   staticBoxSizer2->Add( boxSizer3, 0, wxALIGN_CENTER|wxALL, bsize );
   
   flexGridSizer2->Add( staticBoxSizer1, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer2->Add( staticBoxSizer2, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   boxSizer1->Add( flexGridSizer2, 0, wxGROW, bsize);
   
   this->SetAutoLayout( true );  
   this->SetSizer( boxSizer1 );
   boxSizer1->Fit( this );
   boxSizer1->SetSizeHints( this );
   
   removeButton->Enable(false);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void TankPanel::LoadData()
{
    if (theSpacecraft == NULL)
       return;
          
    theFuelTanks = theSpacecraft->GetRefObjectArray(Gmat::FUEL_TANK);
    fuelTankNames = theSpacecraft->GetRefObjectNameArray(Gmat::FUEL_TANK);
    
    if (theFuelTanks.empty())
       return;     
      
    tankCount = fuelTankNames.size();
    
    Integer paramID;
      
    for (Integer i = 0; i < tankCount; i++)
    {    
        FuelTank * ft = (FuelTank *)theFuelTanks[i];
        
        //FuelTank *ft = (FuelTank *)theSpacecraft->GetRefObject(Gmat::FUEL_TANK, fuelTankNames[i]);
        //theTanks.push_back(new Tank(ft, fuelTankNames[i]));
        
        theTanks[i]->fuelTank = ft;
        theTanks[i]->tankName = fuelTankNames[i];
           
        paramID = ft->GetParameterID("Temperature");
        theTanks[i]->temperature = ft->GetRealParameter(paramID);
         
        paramID = ft->GetParameterID("RefTemperature");
        theTanks[i]->refTemperature = ft->GetRealParameter(paramID);
    
        paramID = ft->GetParameterID("FuelMass");
        theTanks[i]->fuelMass = ft->GetRealParameter(paramID);

        paramID = ft->GetParameterID("FuelDensity");
        theTanks[i]->fuelDensity = ft->GetRealParameter(paramID);
   
        paramID = ft->GetParameterID("Pressure");
        theTanks[i]->pressure = ft->GetRealParameter(paramID);
   
        paramID = ft->GetParameterID("Volume");
        theTanks[i]->volume = ft->GetRealParameter(paramID);
    }   
    currentTank = tankCount-1;
    DisplayData();  
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void TankPanel::SaveData()
{
    if (!theApplyButton->IsEnabled())
       return;
    
    FuelTank *ft;
    Integer paramID;
    
    for (Integer i = 0; i < tankCount; i++)
    {   
        ft = theTanks[i]->fuelTank;

        paramID = ft->GetParameterID("Temperature");
        ft->SetRealParameter(paramID, theTanks[i]->temperature);
        
        paramID = ft->GetParameterID("RefTemperature");
        ft->SetRealParameter(paramID, theTanks[i]->refTemperature);
        
        paramID = ft->GetParameterID("FuelMass");
        ft->SetRealParameter(paramID, theTanks[i]->fuelMass);
        
        paramID = ft->GetParameterID("FuelDensity");
        ft->SetRealParameter(paramID, theTanks[i]->fuelDensity);
        
        paramID = ft->GetParameterID("Pressure");
        ft->SetRealParameter(paramID, theTanks[i]->pressure);
        
        paramID = ft->GetParameterID("Volume");
        ft->SetRealParameter(paramID, theTanks[i]->volume);
        
        paramID = theSpacecraft->GetParameterID("Tanks");
        theSpacecraft->SetStringParameter(paramID, theTanks[i]->tankName);

        theSpacecraft->SetRefObject(theTanks[i]->fuelTank, Gmat::HARDWARE, 
                                    theTanks[i]->tankName); 
    }
}

//------------------------------------------------------------------------------
// void DisplayData()
//------------------------------------------------------------------------------
void TankPanel::DisplayData()
{   
    wxString temperatureString;
    wxString refTemperatureString;
    wxString fuelMassString;
    wxString fuelDensityString;
    wxString pressureString;
    wxString volumeString;
    
    removeButton->Enable(true); 
    tankListBox->Clear();
    
    for (Integer i = 0; i < tankCount; i++) 
        tankListBox->Append(theTanks[i]->tankName.c_str());    
        
    tankListBox->SetSelection(currentTank, true);
    
    temperatureString.Printf("%f", theTanks[currentTank]->temperature);
    refTemperatureString.Printf("%f", theTanks[currentTank]->refTemperature);
    fuelMassString.Printf("%f", theTanks[currentTank]->fuelMass);
    fuelDensityString.Printf("%f", theTanks[currentTank]->fuelDensity);
    pressureString.Printf("%f", theTanks[currentTank]->pressure);
    volumeString.Printf("%f", theTanks[currentTank]->volume);

    temperatureTextCtrl->Clear();
    refTemperatureTextCtrl->Clear();
    fuelMassTextCtrl->Clear();
    fuelDensityTextCtrl->Clear();
    pressureTextCtrl->Clear();
    volumeTextCtrl->Clear();
    
    temperatureTextCtrl->SetValue(temperatureString);
    refTemperatureTextCtrl->SetValue(refTemperatureString);
    fuelMassTextCtrl->SetValue(fuelMassString);
    fuelDensityTextCtrl->SetValue(fuelDensityString);
    pressureTextCtrl->SetValue(pressureString);
    volumeTextCtrl->SetValue(volumeString);   
}    

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void TankPanel::OnTextChange()
{
    theTanks[currentTank]->temperature = atof(temperatureTextCtrl->GetValue());
    theTanks[currentTank]->refTemperature = atof(refTemperatureTextCtrl->GetValue());
    theTanks[currentTank]->fuelMass = atof(fuelMassTextCtrl->GetValue());
    theTanks[currentTank]->fuelDensity = atof(fuelDensityTextCtrl->GetValue());
    theTanks[currentTank]->pressure = atof(pressureTextCtrl->GetValue());
    theTanks[currentTank]->volume = atof(volumeTextCtrl->GetValue());

    theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnSelect()
//------------------------------------------------------------------------------
void TankPanel::OnSelect()
{
    Integer id = tankListBox->GetSelection();
    
    if (id <= tankCount)
    {
        currentTank = id;
        DisplayData(); 
    }    
}    

//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent &event)
//------------------------------------------------------------------------------
void TankPanel::OnButtonClick(wxCommandEvent &event)
{
    wxString tname;
    Integer paramID;

    if (event.GetEventObject() == addButton)
    {  
       tname.Printf("Tank%d", ++tankCount);
       FuelTank *ft = new FuelTank(tname.c_str());
       theTanks.push_back( new Tank(ft, tname.c_str()) );
       
       tankCount = theTanks.size();
       currentTank = tankCount-1;
       
       theTanks[currentTank]->fuelTank = ft;
       theTanks[currentTank]->tankName = tname.c_str();
       
       paramID = ft->GetParameterID("Temperature");
       theTanks[currentTank]->temperature = ft->GetRealParameter(paramID);
  
       paramID = ft->GetParameterID("RefTemperature");
       theTanks[currentTank]->refTemperature = ft->GetRealParameter(paramID);
    
       paramID = ft->GetParameterID("FuelMass");
       theTanks[currentTank]->fuelMass = ft->GetRealParameter(paramID);

       paramID = ft->GetParameterID("FuelDensity");
       theTanks[currentTank]->fuelDensity = ft->GetRealParameter(paramID);
   
       paramID = ft->GetParameterID("Pressure");
       theTanks[currentTank]->pressure = ft->GetRealParameter(paramID);
   
       paramID = ft->GetParameterID("Volume");
       theTanks[currentTank]->volume = ft->GetRealParameter(paramID);
        
       DisplayData();
    }
    else if (event.GetEventObject() == removeButton)
    {
       if (theTanks.empty())
          return;
       
       int count = 0;  
       std::vector <Tank*>::iterator iter;
       for (iter = theTanks.begin(); iter < theTanks.end(); iter++)
       {
          if (count == currentTank)
          {
             theTanks.erase(iter);  
          }    
          count++;     
       }
       
       tankCount = theTanks.size();
       currentTank = tankCount-1;
       
       if (theTanks.empty())  
       {
          removeButton->Enable(false);  
          tankListBox->Clear();
       }    
       else
          DisplayData();
    }     
}    
