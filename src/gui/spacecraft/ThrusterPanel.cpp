//$Header:
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
#include "ThrusterCoefficientDialog.hpp"
#include "TankSelectionDialog.hpp"
#include "MessageInterface.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(ThrusterPanel, wxPanel)
   EVT_TEXT(ID_TEXTCTRL, ThrusterPanel::OnTextChange)
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
    thrusterCount = 0;
    tankCount = 0;
    coordsysCount = 0;
    
    this->theSpacecraft = spacecraft;
    this->theApplyButton = theApplyButton;
    
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

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void ThrusterPanel::Create()
{
    // Integer
    Integer bsize = 3; // border size
    
    
    // wxString
    wxString emptyList[] = {};
    
    // wxButton
    tankButton = new wxButton( this, ID_BUTTON, wxT("Select"),
                              wxDefaultPosition, wxDefaultSize, 0 );
    addButton = new wxButton( this, ID_BUTTON, wxT("Add Thruster"),
                              wxDefaultPosition, wxDefaultSize, 0 );
    removeButton = new wxButton( this, ID_BUTTON, wxT("Remove Thruster"),
                              wxDefaultPosition, wxDefaultSize, 0 );
    cCoefButton = new wxButton( this, ID_BUTTON, wxT("Edit Thruster Coef."),
                              wxDefaultPosition, wxDefaultSize, 0 );
    kCoefButton = new wxButton( this, ID_BUTTON, wxT("Edit Impulse Coef."),
                              wxDefaultPosition, wxDefaultSize, 0 );  
                                
    // wxComboBox 
    tankComboBox = new wxComboBox( this, ID_COMBO, wxT(""),
                      wxDefaultPosition, wxSize(80,-1), tankCount,
                      emptyList, wxCB_DROPDOWN|wxCB_READONLY );;
    coordsysComboBox = new wxComboBox( this, ID_COMBO, wxT(""),
                      wxDefaultPosition, wxSize(125,-1), coordsysCount,
                      emptyList, wxCB_DROPDOWN|wxCB_READONLY );;
   
    // wxTextCtrl
    XTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
    YTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
    ZTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
                            
    //wxStaticText
    tankStaticText = new wxStaticText( this, ID_TEXT, wxT("Tank"),
                            wxDefaultPosition,wxDefaultSize, 0);
    coordsysStaticText = new wxStaticText( this, ID_TEXT, 
                            wxT("Coordinate System"), wxDefaultPosition,
                            wxDefaultSize, 0);
    XStaticText = new wxStaticText( this, ID_TEXT, wxT("X Direction"),
                            wxDefaultPosition,wxDefaultSize, 0);
    YStaticText = new wxStaticText( this, ID_TEXT, wxT("Y Direction"),
                            wxDefaultPosition,wxDefaultSize, 0);  
    ZStaticText = new wxStaticText( this, ID_TEXT, wxT("Z Direction"),
                            wxDefaultPosition,wxDefaultSize, 0); 
                            
    // wxListBox
    thrusterListBox = new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                    wxSize(150,200), thrusterCount, emptyList, wxLB_SINGLE);
    
    // wx*Sizers                                            
    wxBoxSizer *boxSizer1 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *boxSizer2 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *boxSizer3 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *boxSizer4 = new wxBoxSizer( wxVERTICAL );
   
    wxStaticBox *staticBox1 = new wxStaticBox( this, -1, wxT("Thruster List") );
    wxStaticBoxSizer *staticBoxSizer1 = new wxStaticBoxSizer( staticBox1, wxVERTICAL );
    wxStaticBox *staticBox2 = new wxStaticBox( this, -1, wxT("Thruster Parameters") );
    wxStaticBoxSizer *staticBoxSizer2 = new wxStaticBoxSizer( staticBox2, wxVERTICAL );

    wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 3, 0, 0 );
    wxFlexGridSizer *flexGridSizer2 = new wxFlexGridSizer( 2, 0, 0 );
    
    // Add to wx*Sizers 
    flexGridSizer1->Add(tankStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
    flexGridSizer1->Add(tankComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
    flexGridSizer1->Add(tankButton, 0, wxALIGN_LEFT|wxALL, bsize );
    flexGridSizer1->Add(coordsysStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
    flexGridSizer1->Add(coordsysComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
    flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
    flexGridSizer1->Add(XStaticText, 0, wxALIGN_CENTRE|wxALL, bsize );
    flexGridSizer1->Add(XTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
    flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
    flexGridSizer1->Add(YStaticText, 0, wxALIGN_CENTRE|wxALL, bsize );
    flexGridSizer1->Add(YTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
    flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
    flexGridSizer1->Add(ZStaticText, 0, wxALIGN_CENTRE|wxALL, bsize );
    flexGridSizer1->Add(ZTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
    flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
    flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
    flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
    flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
    flexGridSizer1->Add(cCoefButton, 0, wxALIGN_CENTRE|wxALL, bsize );
    flexGridSizer1->Add(kCoefButton, 0, wxALIGN_LEFT|wxALL, bsize );
    flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
    
    boxSizer2->Add(addButton, 0, wxALIGN_CENTER|wxALL, bsize );
    boxSizer2->Add(removeButton, 0, wxALIGN_CENTER|wxALL, bsize );
    
    boxSizer3->Add(flexGridSizer1, 0, wxALIGN_CENTER|wxALL, bsize );
    
    boxSizer4->Add(thrusterListBox, 0, wxALIGN_CENTER|wxALL, bsize );
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
    cCoefButton->Enable(false);
    kCoefButton->Enable(false);
    tankButton->Enable(false);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ThrusterPanel::LoadData()
{  
    if (theSpacecraft == NULL)
       return;
    
    theThrusters = theSpacecraft->GetRefObjectArray(Gmat::THRUSTER);
    thrusterNames = theSpacecraft->GetRefObjectNameArray(Gmat::THRUSTER);
    
    if (theThrusters.empty())
       return;  
       
    thrusterCount = theThrusters.size();
    
    Integer paramID;
      
    for (Integer i = 0; i < thrusterCount; i++)
    {    
        Thruster *tr = (Thruster *)theThrusters[i];
        
        thrusters[i]->thruster = tr;
        thrusters[i]->thrusterName = thrusterNames[i];
        
        paramID = tr->GetParameterID("Tank");
        thrusters[i]->tanks = tr->GetStringArrayParameter(paramID);
        
        paramID = tr->GetParameterID("CoordinateSystem");
        thrusters[i]->coordName = tr->GetStringParameter(paramID);
  
        paramID = tr->GetParameterID("X_Direction");
        thrusters[i]->x_direction = tr->GetRealParameter(paramID);
    
        paramID = tr->GetParameterID("Y_Direction");
        thrusters[i]->y_direction = tr->GetRealParameter(paramID);
        
        paramID = tr->GetParameterID("Z_Direction");
        thrusters[i]->z_direction = tr->GetRealParameter(paramID);
    }   
    currentThruster = thrusterCount-1;
    DisplayData();
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ThrusterPanel::SaveData()
{
    if (!theApplyButton->IsEnabled())
       return; 
}

//------------------------------------------------------------------------------
// void DisplayData()
//------------------------------------------------------------------------------
void ThrusterPanel::DisplayData()
{ 
   tankNames = thrusters[currentThruster]->tanks;
   tankCount = tankNames.size();
   
   tankComboBox->Clear();
   
   if (tankCount == 0)
      tankComboBox->Append("None");
   else
   {
      for (Integer i = 0; i < tankCount; i++)
      {
         tankComboBox->Append(tankNames[i].c_str());
      }    
   }  
   tankComboBox->SetSelection(0);
   
   coordsysCount = 1; // Temp value
   coordsysComboBox->Clear();
   for (Integer i = 0; i < coordsysCount; i++)
   {
       coordsysComboBox->Append(thrusters[currentThruster]->coordName.c_str());
   }
   coordsysComboBox->SetSelection(0);
          
   //removeButton->Enable(true);
   cCoefButton->Enable(true);
   kCoefButton->Enable(true); 
   tankButton->Enable(true);
   
   thrusterListBox->Clear();
   for (Integer i = 0; i < thrusterCount; i++) 
      thrusterListBox->Append(thrusters[i]->thrusterName.c_str());    
        
   thrusterListBox->SetSelection(currentThruster, true);
   
   wxString xString;
   wxString yString;
   wxString zString;
   
   xString.Printf("%f", thrusters[currentThruster]->x_direction);
   yString.Printf("%f", thrusters[currentThruster]->y_direction);
   zString.Printf("%f", thrusters[currentThruster]->z_direction);

   XTextCtrl->Clear();
   YTextCtrl->Clear();
   ZTextCtrl->Clear();
    
   XTextCtrl->SetValue(xString);
   YTextCtrl->SetValue(yString);
   ZTextCtrl->SetValue(zString);    
}

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void ThrusterPanel::OnTextChange()
{
    theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnSelect()
//------------------------------------------------------------------------------
void ThrusterPanel::OnSelect()
{
    Integer id = thrusterListBox->GetSelection();
    
    if (id <= thrusterCount)
    {
        currentThruster = id;
        DisplayData(); 
    }    
}  

//------------------------------------------------------------------------------
// void OnButtonClick()
//------------------------------------------------------------------------------
void ThrusterPanel::OnButtonClick(wxCommandEvent &event)
{
    wxString tname;
    Integer paramID;

    if (event.GetEventObject() == addButton)
    {  
       tname.Printf("Engine%d", ++thrusterCount);
       Thruster *tr = new Thruster(tname.c_str());
       thrusters.push_back( new Thrusters(tr, tname.c_str()) );
       
       thrusterCount = thrusters.size();
       currentThruster = thrusterCount-1;     
       
       thrusters[currentThruster]->thruster = tr;
       thrusters[currentThruster]->thrusterName = tname.c_str();

       paramID = tr->GetParameterID("Tank");
       thrusters[currentThruster]->tanks = tr->GetStringArrayParameter(paramID);
               
       paramID = tr->GetParameterID("CoordinateSystem");
       thrusters[currentThruster]->coordName = tr->GetStringParameter(paramID);

       paramID = tr->GetParameterID("X_Direction");
       thrusters[currentThruster]->x_direction = tr->GetRealParameter(paramID);
    
       paramID = tr->GetParameterID("Y_Direction");
       thrusters[currentThruster]->y_direction = tr->GetRealParameter(paramID);
        
       paramID = tr->GetParameterID("Z_Direction");
       thrusters[currentThruster]->z_direction = tr->GetRealParameter(paramID);
        
       DisplayData();
    }
    else if (event.GetEventObject() == removeButton)
    {
       if (thrusters.empty())
          return;
       
       int count = 0;  
       std::vector <Thrusters*>::iterator iter;
       for (iter = thrusters.begin(); iter < thrusters.end(); iter++)
       {
          if (count == thrusterCount)
          {
             thrusters.erase(iter);  
          }    
          count++;     
       }
       
       thrusterCount = thrusters.size();
       currentThruster = thrusterCount-1;
       
       if (thrusters.empty())  
       {
          removeButton->Enable(false); 
          cCoefButton->Enable(false);
          kCoefButton->Enable(false);
          tankButton->Enable(false);
          thrusterListBox->Clear(); 
       }    
       else
          DisplayData();
    }     
    else if (event.GetEventObject() == tankButton)
    {
       TankSelectionDialog tsDlg(this, theSpacecraft, tankNames);
       tsDlg.ShowModal(); 
    }    
    else if (event.GetEventObject() == cCoefButton)
    {
       std::string type = "C";
       
       ThrusterCoefficientDialog tcDlg(this, thrusters[currentThruster]->thruster, type);
       tcDlg.ShowModal();       
    } 
    else if (event.GetEventObject() == kCoefButton)
    {
       std::string type = "K";
       
       ThrusterCoefficientDialog tcDlg(this, thrusters[currentThruster]->thruster, type);
       tcDlg.ShowModal();
    }            
}    
