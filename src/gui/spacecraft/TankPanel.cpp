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
    theGuiManager = GuiItemManager::GetInstance();
    
    Create();
}

//------------------------------------------------------------------------------
// ~TankPanel()
//------------------------------------------------------------------------------
TankPanel::~TankPanel()
{
   theGuiManager->UnregisterListBox("FuelTank", availableTankListBox,
                                    &mExcludedTankList);
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
                            
   Integer paramID = theSpacecraft->GetParameterID("Tanks");
   StringArray tankNames = theSpacecraft->GetStringArrayParameter(paramID);
   
   int count = tankNames.size();
   for (int i=0; i<count; i++)
      mExcludedTankList.Add(tankNames[i].c_str());
   
   availableTankListBox =
      theGuiManager->GetFuelTankListBox(this, ID_LISTBOX, wxSize(150,200),
                                        &mExcludedTankList);
   
   selectedTankListBox = new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                    wxSize(150,200), 0, emptyList, wxLB_SINGLE);
   
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
   
   selectButton->Enable(true);
   removeButton->Enable(true);
   selectAllButton->Enable(true);
   removeAllButton->Enable(true);
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
    
    int count = tankNames.size();
    for (Integer i = 0; i < count; i++) 
        selectedTankListBox->Append(tankNames[i].c_str());
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void TankPanel::SaveData()
{
    if (!theApplyButton->IsEnabled())
       return;
       
    Integer paramID = 0;
    
    theSpacecraft->TakeAction("RemoveTank", "");
    int count = selectedTankListBox->GetCount();
    
    for (Integer i = 0; i < count; i++) 
    {        
        paramID = theSpacecraft->GetParameterID("Tanks");
        theSpacecraft->SetStringParameter(paramID,
           std::string(selectedTankListBox->GetString(i).c_str()));       
    }
}

//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent &event)
//------------------------------------------------------------------------------
void TankPanel::OnButtonClick(wxCommandEvent &event)
{   
    if (event.GetEventObject() == selectButton)
    {
       wxString str = availableTankListBox->GetStringSelection();
       int sel = availableTankListBox->GetSelection();
       int found = selectedTankListBox->FindString(str);
       
       if (found == wxNOT_FOUND)
       {
          selectedTankListBox->Append(str);
          availableTankListBox->Delete(sel);
          selectedTankListBox->SetStringSelection(str);
          mExcludedTankList.Add(str);
          
          if (sel-1 < 0)
             availableTankListBox->SetSelection(0);
          else
             availableTankListBox->SetSelection(sel-1);
      
       }
       theApplyButton->Enable();
    }
    else if (event.GetEventObject() == removeButton)
    {
       wxString str = selectedTankListBox->GetStringSelection();
       int sel = selectedTankListBox->GetSelection();
       
       selectedTankListBox->Delete(sel);
       availableTankListBox->Append(str);
       availableTankListBox->SetStringSelection(str);
       mExcludedTankList.Remove(str.c_str());
       
       if (sel-1 < 0)
          selectedTankListBox->SetSelection(0);
       else
          selectedTankListBox->SetSelection(sel-1);
   
       theApplyButton->Enable();
    }   
    else if (event.GetEventObject() == selectAllButton)
    {
       Integer count = availableTankListBox->GetCount();
       
       if (count == 0)
          return;
       
       for (Integer i=0; i<count; i++)
       {
          selectedTankListBox->Append(availableTankListBox->GetString(i));
          mExcludedTankList.Add(availableTankListBox->GetString(i));
       }
       
       availableTankListBox->Clear();
       selectedTankListBox->SetSelection(0);
    
       theApplyButton->Enable();
    } 
    else if (event.GetEventObject() == removeAllButton)
    {
       Integer count = selectedTankListBox->GetCount();
       
       if (count == 0)
          return;
       
       for (Integer i=0; i<count; i++)
       {
          availableTankListBox->Append(selectedTankListBox->GetString(i));
       }
       
       selectedTankListBox->Clear();
       mExcludedTankList.Clear();
       availableTankListBox->SetSelection(0);
   
       theApplyButton->Enable();
    }
}     
    
