//$Header: 
//------------------------------------------------------------------------------
//                              TankSelectionDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2005/01/26
//
/**
 * Implements TankSelectionDialog class. This class shows dialog window where
 * available tanks can be selected.
 * 
 */
//------------------------------------------------------------------------------

#include "TankSelectionDialog.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(TankSelectionDialog, GmatDialog)
   EVT_LISTBOX(ID_LISTBOX, TankSelectionDialog::OnSelect)
   EVT_BUTTON(ID_BUTTON, TankSelectionDialog::OnButtonClick)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// TankSelectionDialog(wxWindow *parent)
//------------------------------------------------------------------------------
TankSelectionDialog::TankSelectionDialog(wxWindow *parent, Spacecraft *spacecraft,
                                         StringArray selectedTanks)
   : GmatDialog(parent, -1, wxString(_T("TankSelectionDialog")))
{   
   selectedCount = 0;
   availableCount = 0;
   
   selectedTankNames.clear();
   availableTankNames.clear();
   
   this->theSpacecraft = spacecraft;
   selectedTankNames = selectedTanks;
   
   Create();
   ShowData();
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void TankSelectionDialog::Create()
{
   // Integer
   Integer bsize = 3; // border size
    
   // wxString
   wxString emptyList[] = {};
    
   // wxButton
   selectButton = new wxButton( this, ID_BUTTON, wxT("->"),
                              wxDefaultPosition, wxDefaultSize, 0 );
   removeButton = new wxButton( this, ID_BUTTON, wxT("<-"),
                              wxDefaultPosition, wxDefaultSize, 0 );
   selectAllButton = new wxButton( this, ID_BUTTON, wxT("=>"),
                              wxDefaultPosition, wxDefaultSize, 0 );
   removeAllButton = new wxButton( this, ID_BUTTON, wxT("<="),
                              wxDefaultPosition, wxDefaultSize, 0 );
                              
   // wxListBox
   availableListBox = new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                    wxSize(150,200), availableCount, emptyList, wxLB_SINGLE);
   selectedListBox = new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                    wxSize(150,200), selectedCount, emptyList, wxLB_SINGLE);
                    
   // wx*Sizers                                            
   wxBoxSizer *boxSizer1 = new wxBoxSizer( wxVERTICAL );
   wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 3, 0, 0 );
   
   // Add to wx*Sizers 
   boxSizer1->Add(selectButton, 0, wxALIGN_CENTER|wxALL, bsize );
   boxSizer1->Add(removeButton, 0, wxALIGN_CENTER|wxALL, bsize );
   boxSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   boxSizer1->Add(selectAllButton, 0, wxALIGN_CENTER|wxALL, bsize );
   boxSizer1->Add(removeAllButton, 0, wxALIGN_CENTER|wxALL, bsize );
   
   flexGridSizer1->Add(availableListBox, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(boxSizer1, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(selectedListBox, 0, wxALIGN_CENTER|wxALL, bsize );
                              
   // wxSizers   
   theMiddleSizer->Add(flexGridSizer1, 0, wxALIGN_CENTRE|wxALL, 3);
   
   selectButton->Enable(false);
   removeButton->Enable(false);
   
   selectAllButton->Enable(false);
   removeAllButton->Enable(false);
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void TankSelectionDialog::LoadData()
{
    if (theSpacecraft == NULL)
       return;

    availableTankNames = theSpacecraft->GetRefObjectNameArray(Gmat::FUEL_TANK);
    
    selectedCount = selectedTankNames.size();
    availableCount = availableTankNames.size();    
    
    for (Integer i = 0; i < selectedCount; i++) 
      selectedListBox->Append(selectedTankNames[i].c_str()); 
      
    for (Integer i = 0; i < availableCount; i++) 
      availableListBox->Append(availableTankNames[i].c_str());    
        
    availableListBox->SetSelection(0, true);
    selectedListBox->SetSelection(0, true);
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void TankSelectionDialog::SaveData()
{
}  

//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void TankSelectionDialog::ResetData()
{
}     

//------------------------------------------------------------------------------
// void OnSelect()
//------------------------------------------------------------------------------
void TankSelectionDialog::OnSelect(wxCommandEvent &event)
{   
}  

//------------------------------------------------------------------------------
// void OnButtonClick()
//------------------------------------------------------------------------------
void TankSelectionDialog::OnButtonClick(wxCommandEvent &event)
{
}    
@