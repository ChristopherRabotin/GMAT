//$Header$
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
#include "GmatAppData.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(TankPanel, wxPanel)
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
    
    theGuiInterpreter = GmatAppData::GetGuiInterpreter();
    
    availableTankCount = 0;
    selectedTankCount = 0;
    currentAvailTank = 0;
    currentSelectedTank = 0;
    
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

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void TankPanel::Create()
{
   // wxButton
   selectButton = new wxButton( this, ID_BUTTON, wxT("->"),
                              wxDefaultPosition, wxDefaultSize, 0 );
   removeButton = new wxButton( this, ID_BUTTON, wxT("<-"),
                              wxDefaultPosition, wxDefaultSize, 0 );
   selectAllButton = new wxButton( this, ID_BUTTON, wxT("=>"),
                              wxDefaultPosition, wxDefaultSize, 0 );
   removeAllButton = new wxButton( this, ID_BUTTON, wxT("<="),
                              wxDefaultPosition, wxDefaultSize, 0 );
                              
   // wxString
   wxString emptyList[] = {};
                            
   // wxListBox
   availableTankListBox = new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                    wxSize(150,200), availableTankCount, emptyList, wxLB_SINGLE);
   selectedTankListBox = new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                    wxSize(150,200), selectedTankCount, emptyList, wxLB_SINGLE);
                            
   Integer bsize = 3; // border size
   
   // wx*Sizers   
   wxBoxSizer *boxSizer1 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer2 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer3 = new wxBoxSizer( wxHORIZONTAL );
   wxStaticBox *staticBox1 = new wxStaticBox( this, -1, wxT("Available Tanks") );
   wxStaticBoxSizer *staticBoxSizer1 = new wxStaticBoxSizer( staticBox1, wxHORIZONTAL );
   wxStaticBox *staticBox2 = new wxStaticBox( this, -1, wxT("Selected Tanks") );
   wxStaticBoxSizer *staticBoxSizer2 = new wxStaticBoxSizer( staticBox2, wxHORIZONTAL );

   // Add to wx*Sizers   
   boxSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);    
   boxSizer2->Add(selectButton, 0, wxALIGN_CENTER|wxALL, bsize );
   boxSizer2->Add(removeButton, 0, wxALIGN_CENTER|wxALL, bsize );
   boxSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   boxSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   boxSizer2->Add(selectAllButton, 0, wxALIGN_CENTER|wxALL, bsize );
   boxSizer2->Add(removeAllButton, 0, wxALIGN_CENTER|wxALL, bsize );
   
   staticBoxSizer1->Add( availableTankListBox, 0, wxALIGN_CENTER|wxALL, bsize ); 
   staticBoxSizer2->Add( selectedTankListBox, 0, wxALIGN_CENTER|wxALL, bsize );
   
   boxSizer3->Add( staticBoxSizer1, 0, wxALIGN_CENTER|wxALL, bsize);
   boxSizer3->Add( boxSizer2, 0, wxALIGN_CENTER|wxALL, bsize);
   boxSizer3->Add( staticBoxSizer2, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   boxSizer1->Add( boxSizer3, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   this->SetAutoLayout( true );  
   this->SetSizer( boxSizer1 );
   boxSizer1->Fit( this );
   boxSizer1->SetSizeHints( this );
   
   selectButton->Enable(false);
   removeButton->Enable(false);
   selectAllButton->Enable(false);
   removeAllButton->Enable(false);
}

//------------------------------------------------------------------------------
// void DisplayData()
//------------------------------------------------------------------------------
void TankPanel::DisplayData()
{
    if (availableTankCount > 0)
    {
        selectButton->Enable(true);
        selectAllButton->Enable(true);
    } 
    else
    {
        selectButton->Enable(false);
        selectAllButton->Enable(false);
    }     
    
    if (selectedTankCount > 0)
    {
        removeButton->Enable(true);
        removeAllButton->Enable(true);
    } 
    else
    {
        removeButton->Enable(false);
        removeAllButton->Enable(false);         
    }     
    
    availableTankListBox->Clear();  
    selectedTankListBox->Clear(); 
    
    for (Integer i = 0; i < availableTankCount; i++) 
        availableTankListBox->Append(availableTanksArray[i].c_str());
        
    for (Integer i = 0; i < selectedTankCount; i++) 
        selectedTankListBox->Append(selectedTanksArray[i].c_str());
        
    availableTankListBox->SetSelection(currentAvailTank, true);
    selectedTankListBox->SetSelection(currentSelectedTank, true);
}    

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void TankPanel::LoadData()
{
    if (theSpacecraft == NULL)
       return;

    // Load list of selected tanks
    Integer paramID = theSpacecraft->GetParameterID("Tanks");
    StringArray tankNames = theSpacecraft->GetStringArrayParameter(paramID);
    
    for (Integer i = 0; i < (Integer)tankNames.size(); i++)
       selectedTanksArray.Add(tankNames[i].c_str());
       
    selectedTankCount = selectedTanksArray.GetCount();
           
    // Load list of available tanks
    availableTankCount = 0;

    StringArray itemNames = theGuiInterpreter->GetListOfConfiguredItems(Gmat::HARDWARE);
    Integer size = itemNames.size();  
      
    for (Integer i = 0; i < size; i++)
    {
        Hardware *hw = theGuiInterpreter->GetHardware(itemNames[i]);
        wxString objTypeName = wxString(hw->GetTypeName().c_str());

        if (objTypeName == "FuelTank")
        {
           if (find(tankNames.begin(), tankNames.end(), itemNames[i].c_str()) == tankNames.end()) 
              availableTanksArray.Add(itemNames[i].c_str());
        }    
    }
    
    availableTankCount = availableTanksArray.GetCount();

    DisplayData();
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void TankPanel::SaveData()
{
    if (!theApplyButton->IsEnabled())
       return;
       
    Integer paramID = 0;
       
    for (Integer i = 0; i < selectedTankCount; i++) 
    {
        //FuelTank* ft = new FuelTank(selectedTanksArray[i].c_str());
        //theSpacecraft->SetRefObject(ft, Gmat::HARDWARE, selectedTanksArray[i].c_str()); 
        
        paramID = theSpacecraft->GetParameterID("Tanks");
        theSpacecraft->SetStringParameter(paramID, selectedTanksArray[i].c_str());       
    }   
    theApplyButton->Disable();
}

//------------------------------------------------------------------------------
// void OnSelect()
//------------------------------------------------------------------------------
void TankPanel::OnSelect(wxCommandEvent &event)
{
    if (event.GetEventObject() == availableTankListBox)
    {
        currentAvailTank = availableTankListBox->GetSelection(); 
        availableTankListBox->SetSelection(currentAvailTank, true);
    }    
    else if (event.GetEventObject() == selectedTankListBox)
    {
        currentSelectedTank = selectedTankListBox->GetSelection(); 
        selectedTankListBox->SetSelection(currentSelectedTank, true);
    }    
}   

//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent &event)
//------------------------------------------------------------------------------
void TankPanel::OnButtonClick(wxCommandEvent &event)
{
    if (event.GetEventObject() == selectButton)
    {  
        wxString tank = availableTanksArray.Item(currentAvailTank);
        availableTanksArray.Remove(tank.c_str());
        availableTankCount = availableTanksArray.GetCount();
        currentAvailTank = 0;
        
        selectedTanksArray.Add(tank);
        selectedTankCount = selectedTanksArray.GetCount();
        currentSelectedTank = 0;
        
        DisplayData();
        theApplyButton->Enable();
    }
    else if (event.GetEventObject() == removeButton)
    {
        wxString tank = selectedTanksArray.Item(currentSelectedTank);
        selectedTanksArray.Remove(tank.c_str());
        selectedTankCount = selectedTanksArray.GetCount();
        currentSelectedTank = 0;
        
        availableTanksArray.Add(tank);
        availableTankCount = availableTanksArray.GetCount();
        currentAvailTank = 0;
        
        DisplayData();
        theApplyButton->Enable();
    }   
    else if (event.GetEventObject() == selectAllButton)
    {
        for (Integer i = 0; i < availableTankCount; i++) 
        {
           wxString tank = availableTanksArray.Item(i);
           selectedTanksArray.Add(tank);
           availableTanksArray.Remove(tank.c_str());
        }  
        
        availableTankCount = availableTanksArray.GetCount();
        currentAvailTank = 0;
        
        selectedTankCount = selectedTanksArray.GetCount();
        currentSelectedTank = 0;
        
        DisplayData();
        theApplyButton->Enable();
    } 
    else if (event.GetEventObject() == removeAllButton)
    {
        for (Integer i = 0; i < selectedTankCount; i++) 
        {
           wxString tank = selectedTanksArray.Item(i);
           availableTanksArray.Add(tank);
           selectedTanksArray.Remove(tank.c_str());
        }  
        
        availableTankCount = availableTanksArray.GetCount();
        currentAvailTank = 0;
        
        selectedTankCount = selectedTanksArray.GetCount();
        currentSelectedTank = 0;
        
        DisplayData();
        theApplyButton->Enable();
    }   
}     
    
