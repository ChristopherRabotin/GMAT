//$Header$
//------------------------------------------------------------------------------
//                            ThrusterPanel
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
// Created: 2005/01/06
/**
 * This class contains information needed to setup users spacecraft thruster 
 * parameters.
 */
//------------------------------------------------------------------------------
#include "ThrusterPanel.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(ThrusterPanel, wxPanel)
   EVT_LISTBOX(ID_LISTBOX, ThrusterPanel::OnSelect)
   EVT_BUTTON(ID_BUTTON, ThrusterPanel::OnButtonClick)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// ThrusterPanel(wxWindow *parent, Spacecraft *spacecraft, wxButton *theApplyButton)
//------------------------------------------------------------------------------
/**
 * Constructs ThrusterPanel object.
 */
//------------------------------------------------------------------------------
ThrusterPanel::ThrusterPanel(wxWindow *parent, Spacecraft *spacecraft,
                     wxButton *theApplyButton):wxPanel(parent)
{
    this->theSpacecraft = spacecraft;
    this->theApplyButton = theApplyButton;
    
    theGuiInterpreter = GmatAppData::GetGuiInterpreter();
    
    availableThrusterCount = 0;
    selectedThrusterCount = 0;
    
    currentAvailThruster = 0;
    currentSelectedThruster = 0;
    
    Create();
}

//------------------------------------------------------------------------------
// ~ThrusterPanel()
//------------------------------------------------------------------------------
ThrusterPanel::~ThrusterPanel()
{
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void ThrusterPanel::Create()
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
   availableThrusterListBox = new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                    wxSize(150,200), availableThrusterCount, emptyList, wxLB_SINGLE);
   selectedThrusterListBox = new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                    wxSize(150,200), selectedThrusterCount, emptyList, wxLB_SINGLE);
                            
   Integer bsize = 3; // border size
   
   // wx*Sizers   
   wxBoxSizer *boxSizer1 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer2 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer3 = new wxBoxSizer( wxHORIZONTAL );
   wxStaticBox *staticBox1 = new wxStaticBox( this, -1, wxT("Available Thrusters") );
   wxStaticBoxSizer *staticBoxSizer1 = new wxStaticBoxSizer( staticBox1, wxHORIZONTAL );
   wxStaticBox *staticBox2 = new wxStaticBox( this, -1, wxT("Selected Thrusters") );
   wxStaticBoxSizer *staticBoxSizer2 = new wxStaticBoxSizer( staticBox2, wxHORIZONTAL );

   // Add to wx*Sizers   
   boxSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);    
   boxSizer2->Add(selectButton, 0, wxALIGN_CENTER|wxALL, bsize );
   boxSizer2->Add(removeButton, 0, wxALIGN_CENTER|wxALL, bsize );
   boxSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   boxSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   boxSizer2->Add(selectAllButton, 0, wxALIGN_CENTER|wxALL, bsize );
   boxSizer2->Add(removeAllButton, 0, wxALIGN_CENTER|wxALL, bsize );
   
   staticBoxSizer1->Add( availableThrusterListBox, 0, wxALIGN_CENTER|wxALL, bsize ); 
   staticBoxSizer2->Add( selectedThrusterListBox, 0, wxALIGN_CENTER|wxALL, bsize );
   
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
void ThrusterPanel::DisplayData()
{
    if (availableThrusterCount > 0)
    {
        selectButton->Enable(true);
        selectAllButton->Enable(true);
    } 
    else
    {
        selectButton->Enable(false);
        selectAllButton->Enable(false);
    }     
    
    if (selectedThrusterCount > 0)
    {
        removeButton->Enable(true);
        removeAllButton->Enable(true);
    } 
    else
    {
        removeButton->Enable(false);
        removeAllButton->Enable(false);         
    }     
    
    availableThrusterListBox->Clear();  
    selectedThrusterListBox->Clear(); 
    
    for (Integer i = 0; i < availableThrusterCount; i++) 
        availableThrusterListBox->Append(availableThrusterArray[i].c_str());
        
    for (Integer i = 0; i < selectedThrusterCount; i++) 
        selectedThrusterListBox->Append(selectedThrusterArray[i].c_str());
        
    availableThrusterListBox->SetSelection(currentAvailThruster, true);
    selectedThrusterListBox->SetSelection(currentSelectedThruster, true);
}    

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ThrusterPanel::LoadData()
{
    if (theSpacecraft == NULL)
       return;

    // Load list of selected thruster
    Integer paramID = theSpacecraft->GetParameterID("Thrusters");
    StringArray thrusterNames = theSpacecraft->GetStringArrayParameter(paramID);
    
    for (Integer i = 0; i < (Integer)thrusterNames.size(); i++)
       selectedThrusterArray.Add(thrusterNames[i].c_str());
       
    selectedThrusterCount = selectedThrusterArray.GetCount();
           
    // Load list of available thrusters
    availableThrusterCount = 0;

    StringArray itemNames = theGuiInterpreter->GetListOfConfiguredItems(Gmat::HARDWARE);
    Integer size = itemNames.size();  
      
    for (Integer i = 0; i < size; i++)
    {
        Hardware *hw = theGuiInterpreter->GetHardware(itemNames[i]);
        wxString objTypeName = wxString(hw->GetTypeName().c_str());

        if (objTypeName == "Thruster")
        {
           if (find(thrusterNames.begin(), thrusterNames.end(), itemNames[i].c_str()) == thrusterNames.end()) 
              availableThrusterArray.Add(itemNames[i].c_str());
        }    
    }
    
    availableThrusterCount = availableThrusterArray.GetCount();

    DisplayData();
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ThrusterPanel::SaveData()
{
    if (!theApplyButton->IsEnabled())
       return;
    
    Integer paramID = 0; 
    theSpacecraft->TakeAction("RemoveThruster", "");    
    for (Integer i = 0; i < selectedThrusterCount; i++) 
    {       
        paramID = theSpacecraft->GetParameterID("Thrusters");
        theSpacecraft->SetStringParameter(paramID, selectedThrusterArray[i].c_str());     
    }   
}

//------------------------------------------------------------------------------
// void OnSelect()
//------------------------------------------------------------------------------
void ThrusterPanel::OnSelect(wxCommandEvent &event)
{
    if (event.GetEventObject() == availableThrusterListBox)
    {
        currentAvailThruster = availableThrusterListBox->GetSelection(); 
        availableThrusterListBox->SetSelection(currentAvailThruster, true);
    }    
    else if (event.GetEventObject() == selectedThrusterListBox)
    {
        currentSelectedThruster = selectedThrusterListBox->GetSelection(); 
        selectedThrusterListBox->SetSelection(currentSelectedThruster, true);
    }    
}   

//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent &event)
//------------------------------------------------------------------------------
void ThrusterPanel::OnButtonClick(wxCommandEvent &event)
{
    if (event.GetEventObject() == selectButton)
    {  
        wxString thruster = availableThrusterArray.Item(currentAvailThruster);
        availableThrusterArray.Remove(thruster.c_str());
        availableThrusterCount = availableThrusterArray.GetCount();
        currentAvailThruster = 0;
        
        selectedThrusterArray.Add(thruster);
        selectedThrusterCount = selectedThrusterArray.GetCount();
        currentSelectedThruster = 0;
        
        DisplayData();
        theApplyButton->Enable();
    }
    else if (event.GetEventObject() == removeButton)
    {
        wxString thruster = selectedThrusterArray.Item(currentSelectedThruster);
        selectedThrusterArray.Remove(thruster.c_str());
        selectedThrusterCount = selectedThrusterArray.GetCount();
        currentSelectedThruster = 0;
        
        availableThrusterArray.Add(thruster);
        availableThrusterCount = availableThrusterArray.GetCount();
        currentAvailThruster = 0;
        
        DisplayData();
        theApplyButton->Enable();
    }   
    else if (event.GetEventObject() == selectAllButton)
    {
        for (Integer i = 0; i < availableThrusterCount; i++) 
           selectedThrusterArray.Add(availableThrusterArray.Item(i));
  
        availableThrusterArray.Clear();
        availableThrusterCount = availableThrusterArray.GetCount();
        currentAvailThruster = 0;
        
        selectedThrusterCount = selectedThrusterArray.GetCount();
        currentSelectedThruster = 0;
        
        DisplayData();
        theApplyButton->Enable();
    } 
    else if (event.GetEventObject() == removeAllButton)
    {
        for (Integer i = 0; i < selectedThrusterCount; i++) 
           availableThrusterArray.Add(selectedThrusterArray.Item(i));
        
        selectedThrusterArray.Clear();
        selectedThrusterCount = selectedThrusterArray.GetCount();
        currentSelectedThruster = 0;
        
        availableThrusterCount = availableThrusterArray.GetCount();
        currentAvailThruster = 0;
        
        DisplayData();
        theApplyButton->Enable();
    }   
}     
    
