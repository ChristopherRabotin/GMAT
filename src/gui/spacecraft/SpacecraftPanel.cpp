//$Header$
//------------------------------------------------------------------------------
//                            SpacecraftPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Monisha Butler
// Created: 2003/09/10
// Modified: 2003/09/29
/**
 * This class contains information needed to setup users spacecraft through GUI
 * 
 */
//------------------------------------------------------------------------------
#include "SpacecraftPanel.hpp"
#include "GmatAppData.hpp"

#include "gmatdefs.hpp" //put this one after GUI includes
#include "GuiInterpreter.hpp"
#include "Spacecraft.hpp"

BEGIN_EVENT_TABLE(SpacecraftPanel, wxPanel)
   EVT_CHOICE(ID_CHOICE_BODY,        SpacecraftPanel::OnBodyChoice)
   EVT_CHOICE(ID_CHOICE_FRAME,       SpacecraftPanel::OnFrameChoice)
   EVT_CHOICE(ID_CHOICE_EPOCH,       SpacecraftPanel::OnEpochChoice)
   EVT_CHOICE(ID_CHOICE_STATE,       SpacecraftPanel::OnStateChoice)
   EVT_BUTTON(ID_BUTTON_OK,          SpacecraftPanel::OnOkButton)
   EVT_BUTTON(ID_BUTTON_APPLY,       SpacecraftPanel::OnApplyButton)
   EVT_BUTTON(ID_SC_BUTTON_CANCEL,   SpacecraftPanel::OnCancelButton)
  // EVT_BUTTON(ID_BUTTON_HELP,      SpacecraftPanel::OnHelpButton)  
END_EVENT_TABLE()



SpacecraftPanel::SpacecraftPanel(wxWindow *parent, const wxString &scName)
    :wxPanel(parent)
{
   CreateNotebook(this, scName);
}

void SpacecraftPanel::CreateNotebook(wxWindow *parent, const wxString &scName)
{
    theGuiInterpreter = GmatAppData::GetGuiInterpreter();
    //loj: Need to get spacecraft name somehow when spacecraft name is clicked
    //loj: use "DefaultSC" for testing only
    theSpacecraft = theGuiInterpreter->GetSpacecraft(std::string(scName.c_str()));
    
    if (theSpacecraft != NULL)
    {
        wxGridSizer *item = new wxGridSizer( 1, 0, 0 );
        //   item = new wxBoxSizer( wxVERTICAL );

        mainNotebook = new wxNotebook( parent, ID_NOTEBOOK, wxDefaultPosition, wxSize(350,300), 0 );
        sizer = new wxNotebookSizer( mainNotebook );
    
        wxPanel *orbitPanel = (wxPanel *)NULL;
        orbitPanel = CreateOrbit( mainNotebook );
        mainNotebook->AddPage( orbitPanel, wxT("Orbit") );

        attitude = new wxPanel( mainNotebook, -1 );
        mainNotebook->AddPage( attitude, wxT("Attitude") );

        properties = new wxPanel( mainNotebook, -1 );
        mainNotebook->AddPage( properties, wxT("Ballistic/Mass") );

        actuators = new wxPanel( mainNotebook, -1 );
        mainNotebook->AddPage( actuators, wxT("Actuators") );

        sensors = new wxPanel( mainNotebook, -1 );
        mainNotebook->AddPage( sensors, wxT("Sensors") );

        tanks = new wxPanel( mainNotebook, -1 );
        mainNotebook->AddPage( tanks, wxT("Tanks") );

        visual = new wxPanel( mainNotebook, -1 );
        mainNotebook->AddPage( visual, wxT("Visualization") );

        item->Add( sizer, 0, wxGROW|wxALL, 5 );
    
        parent->SetAutoLayout( TRUE );
        parent->SetSizer( item );
        item->Fit( parent );
        item->SetSizeHints( parent );
    }
    else
    {
        // show error message
    }
}

wxPanel *SpacecraftPanel::CreateOrbit(wxWindow *parent)
{
    //loj: since Spacecraft class is not complete,
    //loj: use theSpacecraft->GetRealParameter(0) for epoch
    //loj: use theSpacecraft->GetRealParameter(1) for state[0], etc

    Real epoch = theSpacecraft->GetRealParameter(0);
    
    wxPanel *panel = new wxPanel(parent);

    wxFlexGridSizer *orbitSizer = new wxFlexGridSizer( 1, 0, 0 );
    orbitSizer->AddGrowableCol( 0 );
    orbitSizer->AddGrowableCol( 1 );
    orbitSizer->AddGrowableRow( 0 );
    orbitSizer->AddGrowableRow( 1 );

    wxStaticBox *item2 = new wxStaticBox( panel, -1, wxT("Coordinate System") );
    wxStaticBoxSizer *item1 = new wxStaticBoxSizer( item2, wxVERTICAL );

    wxGridSizer *item3 = new wxGridSizer( 2, 0, 0 );

    wxStaticText *item4 = new wxStaticText( item2, ID_TEXT, wxT("Reference Body"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item5 = new wxStaticText( item2, ID_TEXT, wxT("Reference Frame"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxString strs6[] =
    {
        wxT("ComboItem")
    };
    wxComboBox *item6 = new wxComboBox( item2, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs6, wxCB_DROPDOWN );
    item3->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    wxString strs7[] =
    {
        wxT("ComboItem")
    };
    wxComboBox *item7 = new wxComboBox( item2, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs7, wxCB_DROPDOWN );
    item3->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item9 = new wxStaticBox( panel, -1, wxT("Orbit State") );
    wxStaticBoxSizer *item8 = new wxStaticBoxSizer( item9, wxVERTICAL );

    wxGridSizer *item10 = new wxGridSizer( 2, 0, 0 );

    wxStaticText *item11 = new wxStaticText( panel, ID_TEXT, wxT("Epoch"), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item11, 0, wxALIGN_CENTER|wxALL, 5 );

    item10->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxString strs12[] =
    {
        wxT("UTC1"),
        wxT("Julian Date"),
        wxT("TAI Julian"),
        wxT("Gregorian")
    };
    wxComboBox *item12 = new wxComboBox( panel, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 4, strs12, wxCB_DROPDOWN );
    item10->Add( item12, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item13 = new wxTextCtrl( panel, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item10->Add( item13, 0, wxALIGN_CENTER|wxALL, 5 );

    item10->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item14 = new wxStaticText( panel, ID_TEXT, wxT("State Type"), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item14, 0, wxALIGN_CENTER|wxALL, 5 );

    item10->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxString strs15[] =
    {
        wxT("Cartesian"),
        wxT("Keplerian"),
        wxT("Modified Keplerian"),
        wxT("Spherical"),
        wxT("Equinotical")
    };
    wxComboBox *item15 = new wxComboBox( panel, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 5, strs15, wxCB_DROPDOWN );
    item10->Add( item15, 0, wxALIGN_CENTER|wxALL, 5 );

    item8->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    elements = new wxPanel( panel );
    //should do on change
    KapElements(elements);
    item8->Add( elements, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    orbitSizer->Add( item1, 0, wxGROW|wxALL, 5 );
    orbitSizer->Add( item8, 0, wxGROW|wxALL, 5 );

    panel->SetAutoLayout( TRUE );
    panel->SetSizer( orbitSizer );
    orbitSizer->Fit( panel );
    orbitSizer->SetSizeHints( panel );
    
    return panel;
}

void SpacecraftPanel::OnBodyChoice(wxCommandEvent& event)
{

}

void SpacecraftPanel::OnFrameChoice(wxCommandEvent& event)
{

}

void SpacecraftPanel::OnEpochChoice(wxCommandEvent& event)
{
  /* int n;
   wxString coord;
   //choice3->SetStringSelection(wxT("Keplerian"));
   if(coord->GetString(n->GetSelection())strcmp(wxT( TAIJulian))))
   {
   epochText->WriteText(wxT("22000"));
   }
   else
     epochText->WriteText(wxT(""));*/
}

void SpacecraftPanel::OnStateChoice(wxCommandEvent& event)
{
  //wxST_NO_AUTORESIZE = false; Don't know if this is the right way to say don't
   //             automatically resize labels.
   
  /* int n;
   wxString coord;
   
   
   if(GetString(strcmp(n->GetSelection()),(wxT( Keplerian)))
   {
     describeLabel->SetLabel(wxT("A"));
     describeLabel2->SetLabel(wxT("E"));
     describleLabel3->SetLabel(wxT("I"));
     describleLabel4->SetLabel(wxT("AOP"));
     describeLabel5->SatLabel(wxT("RAAN"));
     describeLabel6->SatLabel(wxT("TA"));
   }*/
}

void SpacecraftPanel::OnOkButton(wxCommandEvent& event)
{
}

void SpacecraftPanel::OnApplyButton(wxCommandEvent& event)
{
}

void SpacecraftPanel::OnCancelButton(wxCommandEvent& event)
{
   //wxMessageBox(wxT("This is some message - everything is ok so far.")); //used for testing
   Close();
}

void SpacecraftPanel::OnHelpButton(wxCommandEvent& event)
{
  
}

void SpacecraftPanel::KapElements(wxWindow *parent)
{

    wxGridSizer *item0 = new wxGridSizer( 1, 0, 0 );

    wxStaticBox *item2 = new wxStaticBox( parent, -1, wxT("Elements") );
    wxStaticBoxSizer *item1 = new wxStaticBoxSizer( item2, wxVERTICAL );

    wxFlexGridSizer *item3 = new wxFlexGridSizer( 3, 0, 0 );
    item3->AddGrowableCol( 0 );
    item3->AddGrowableCol( 1 );
    item3->AddGrowableCol( 2 );

    wxStaticText *item4 = new wxStaticText( parent, ID_TEXT, wxT("Semi-Major Axis"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item5 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item3->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item6 = new wxStaticText( parent, ID_TEXT, wxT("Km"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item7 = new wxStaticText( parent, ID_TEXT, wxT("Eccentricity"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item8 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item3->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );

    item3->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item9 = new wxStaticText( parent, ID_TEXT, wxT("Inclination"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item9, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item10 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item3->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item11 = new wxStaticText( parent, ID_TEXT, wxT("deg"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item11, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item12 = new wxStaticText( parent, ID_TEXT, wxT("R.A. of Ascending Node"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item12, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item13 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item3->Add( item13, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item14 = new wxStaticText( parent, ID_TEXT, wxT("deg"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item14, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item15 = new wxStaticText( parent, ID_TEXT, wxT("Arguement of Perigee"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item15, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item16 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item3->Add( item16, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item17 = new wxStaticText( parent, ID_TEXT, wxT("deg"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item17, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item18 = new wxStaticText( parent, ID_TEXT, wxT("True Anomaly"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item18, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item19 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item3->Add( item19, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item20 = new wxStaticText( parent, ID_TEXT, wxT("deg"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item20, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALL, 5 );


    parent->SetAutoLayout( TRUE );
    parent->SetSizer( item0 );

    item0->Fit( parent );
    item0->SetSizeHints( parent );

}

void SpacecraftPanel::CartElements(wxWindow *parent)
{
    wxGridSizer *item0 = new wxGridSizer( 1, 0, 0 );

    wxStaticBox *item2 = new wxStaticBox( parent, -1, wxT("Elements") );
    wxStaticBoxSizer *item1 = new wxStaticBoxSizer( item2, wxVERTICAL );

    wxFlexGridSizer *item3 = new wxFlexGridSizer( 4, 0, 0 );
    item3->AddGrowableCol( 0 );
    item3->AddGrowableCol( 1 );
    item3->AddGrowableCol( 2 );
    item3->AddGrowableCol( 3 );

    item3->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item4 = new wxStaticText( parent, ID_TEXT, wxT("X"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item5 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item3->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item6 = new wxStaticText( parent, ID_TEXT, wxT("Km"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    item3->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item7 = new wxStaticText( parent, ID_TEXT, wxT("Y"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item8 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item3->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item9 = new wxStaticText( parent, ID_TEXT, wxT("Km"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item9, 0, wxALIGN_CENTER|wxALL, 5 );

    item3->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item10 = new wxStaticText( parent, ID_TEXT, wxT("Z"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item11 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item3->Add( item11, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item12 = new wxStaticText( parent, ID_TEXT, wxT("Km"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item12, 0, wxALIGN_CENTER|wxALL, 5 );

    item3->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item13 = new wxStaticText( parent, ID_TEXT, wxT("Vx"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item13, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item14 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item3->Add( item14, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item15 = new wxStaticText( parent, ID_TEXT, wxT("Km/s"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item15, 0, wxALIGN_CENTER|wxALL, 5 );

    item3->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item16 = new wxStaticText( parent, ID_TEXT, wxT("Vy"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item16, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item17 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item3->Add( item17, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item18 = new wxStaticText( parent, ID_TEXT, wxT("Km/s"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item18, 0, wxALIGN_CENTER|wxALL, 5 );

    item3->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item19 = new wxStaticText( parent, ID_TEXT, wxT("Vz"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item19, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item20 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item3->Add( item20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item21 = new wxStaticText( parent, ID_TEXT, wxT("Km/s"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item21, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALL, 5 );

    parent->SetAutoLayout( TRUE );
    parent->SetSizer( item0 );
    item0->Fit( parent );
    item0->SetSizeHints( parent );

}


