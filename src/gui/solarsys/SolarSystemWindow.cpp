//$Header$
//------------------------------------------------------------------------------
//                              SolarSystemWindow
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Monisha Butler
// Created: 2003/09/10
// Modified: 2003/09/29
/**
 * This class provides information for the setup of the SolarSystem
 * coming from
 */
//------------------------------------------------------------------------------
#include "SolarSystemWindow.hpp"


BEGIN_EVENT_TABLE(SolarSystemWindow, wxScrolledWindow)
   EVT_BUTTON(ID_BUTTON_POS,          SolarSystemWindow::OnPositionButton)
   EVT_BUTTON(ID_BUTTON_BODYROTATION, SolarSystemWindow::OnBodyRotationButton)
   EVT_BUTTON(ID_BUTTON_BROWSE,       SolarSystemWindow::OnBrowseButton)
   EVT_BUTTON(ID_BUTTON_FIXEDCOORD,   SolarSystemWindow::OnFixedCoordinatesButton)
   EVT_BUTTON(ID_BUTTON_CREATE,       SolarSystemWindow::OnCreateButton)
   EVT_BUTTON(ID_BUTTON_CANCEL,       SolarSystemWindow::OnCancelButton)
END_EVENT_TABLE()

SolarSystemWindow::SolarSystemWindow(wxWindow *parent):wxScrolledWindow(parent)
{
    SolarSystemSetup(this);
}

void SolarSystemWindow::SolarSystemSetup(wxWindow *parent)
{

     item0 = new wxBoxSizer( wxVERTICAL );

     item1 = new wxGridSizer( 4, 0, 3 );

     item2 = new wxStaticText( parent, ID_TEXT, wxT("Graviational Constant"), wxDefaultPosition, wxSize(150,-1), 0 );
     item1->Add( item2, 0, wxALIGN_CENTRE|wxALL, 5 );

     item3 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
     item1->Add( item3, 0, wxALIGN_CENTRE|wxALL, 5 );

     item4 = new wxButton( parent, ID_BUTTON_POS, wxT("Pos/Vel Source"), wxDefaultPosition, wxSize(100,-1), 0 );
     item1->Add( item4, 0, wxALIGN_CENTRE|wxALL, 5 );

     item1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item5 = new wxStaticText( parent, ID_TEXT, wxT("Body Radius             "), wxDefaultPosition, wxSize(150,-1), 0 );
     item1->Add( item5, 0, wxALIGN_CENTRE|wxALL, 5 );

     item6 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
     item1->Add( item6, 0, wxALIGN_CENTRE|wxALL, 5 );

     item7 = new wxButton( parent, ID_BUTTON_BODYROTATION, wxT("Body Rot. Source"), wxDefaultPosition, wxSize(100,-1), 0 );
     item1->Add( item7, 0, wxALIGN_CENTRE|wxALL, 5 );

     item0->Add( item1, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8 = new wxFlexGridSizer( 4, 0, 0 );

     item9 = new wxStaticText( parent, ID_TEXT, wxT("Enhanced Potential File (ODEAS Format)"), wxDefaultPosition, wxSize(150,-1), 0 );
     item8->Add( item9, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 10, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 10, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 10, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item10 = new wxStaticText( parent, ID_TEXT, wxT("Potential Field File      "), wxDefaultPosition, wxSize(120,-1), 0 );
     item8->Add( item10, 0, wxALIGN_CENTRE|wxALL, 5 );

     item11 = new wxTextCtrl( parent, ID_FILE_NAME, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
     item8->Add( item11, 0, wxALIGN_CENTRE|wxALL, 5 );

     item12 = new wxButton( parent, ID_BUTTON_BROWSE, wxT("Browse"), wxDefaultPosition, wxSize(100,-1), 0 );
     item8->Add( item12, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item13 = new wxStaticText( parent, ID_TEXT, wxT("Effective Field Range"), wxDefaultPosition, wxSize(120,-1), 0 );
     item8->Add( item13, 0, wxALIGN_CENTRE|wxALL, 5 );

     item14 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
     item8->Add( item14, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 40, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 60, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item15 = new wxStaticText( parent, ID_TEXT, wxT("Zonals:                      "), wxDefaultPosition, wxSize(120,-1), 0 );
     item8->Add( item15, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item16 = new wxStaticText( parent, ID_TEXT, wxT("J2                      "), wxDefaultPosition, wxDefaultSize, 0 );
     item8->Add( item16, 0, wxALIGN_CENTRE|wxALL, 5 );

     item17 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
     item8->Add( item17, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item18 = new wxStaticText( parent, ID_TEXT, wxT("J3                      "), wxDefaultPosition, wxDefaultSize, 0 );
     item8->Add( item18, 0, wxALIGN_CENTRE|wxALL, 5 );

     item19 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
     item8->Add( item19, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item20 = new wxStaticText( parent, ID_TEXT, wxT("J4                     "), wxDefaultPosition, wxDefaultSize, 0 );
     item8->Add( item20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item21 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
     item8->Add( item21, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item22 = new wxStaticText( parent, ID_TEXT, wxT("J5                      "), wxDefaultPosition, wxDefaultSize, 0 );
     item8->Add( item22, 0, wxALIGN_CENTRE|wxALL, 5 );

     item23 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
     item8->Add( item23, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item24 = new wxButton( parent, ID_BUTTON_FIXEDCOORD, wxT("Body Fixed Coordinate System"), wxDefaultPosition, wxSize(160,-1), 0 );
     item8->Add( item24, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 70, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 80, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 30, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item25 = new wxStaticText( parent, ID_TEXT, wxT("Flattening Coefficient "), wxDefaultPosition, wxSize(120,-1), 0 );
     item8->Add( item25, 0, wxALIGN_CENTRE|wxALL, 5 );

     item26 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
     item8->Add( item26, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item27 = new wxStaticText( parent, ID_TEXT, wxT("Body Rotation Rate   "), wxDefaultPosition, wxSize(120,-1), 0 );
     item8->Add( item27, 0, wxALIGN_CENTRE|wxALL, 5 );

     item28 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
     item8->Add( item28, 0, wxALIGN_CENTRE|wxALL, 5 );

     item29 = new wxStaticText( parent, ID_TEXT, wxT("rad/sec"), wxDefaultPosition, wxDefaultSize, 0 );
     item8->Add( item29, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item0->Add( item8, 0, wxALIGN_CENTRE, 5 );

     item30 = new wxBoxSizer( wxHORIZONTAL );
     
     item8->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item8->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

     item31 = new wxButton( parent, ID_BUTTON_CREATE, wxT("Create"), wxDefaultPosition, wxSize(100,-1), 0 );
     item30->Add( item31, 0, wxALIGN_CENTRE|wxALL, 5 );

     item32 = new wxButton( parent, ID_BUTTON_CANCEL, wxT("Cancel"), wxDefaultPosition, wxSize(100,-1), 0 );
     item30->Add( item32, 0, wxALIGN_CENTRE|wxALL, 5 );

     item0->Add( item30, 0, wxALIGN_CENTRE|wxALL, 5 );

    
        parent->SetAutoLayout( TRUE );
        parent->SetSizer( item0 );
        item0->Fit( parent );
        item0->SetSizeHints( parent );
       
    
   
}

void SolarSystemWindow::OnPositionButton(wxCommandEvent& WXUNUSED (event))
{
   //Don't know what event to attach to this.  In Argosy pops up different file
   //names like DE-405, DE-200, SLP.........
}

void SolarSystemWindow::OnBodyRotationButton(wxCommandEvent& WXUNUSED (event))
{
  // wxDialog *popup = new wxDialog();
  
   //CreateRadiobox();
}

void SolarSystemWindow::OnBrowseButton(wxCommandEvent& WXUNUSED (event))
{
    wxFileDialog dialog
                 (
                    this,
                    _T("Open File"),
                    _T(""),
                    _T(""),
#ifdef __WXMOTIF__
                    _T("C++ files (*.cpp)|*.cpp")
#else
                    _T("C++ files (*.h;*.cpp)|*.h;*.cpp")
#endif
                 );

    dialog.SetDirectory(wxGetHomeDir());

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString info;
        info.Printf(dialog.GetPath().c_str(),
                    dialog.GetDirectory().c_str(),
                    dialog.GetFilename().c_str());
    //                item11.LoadFile(info);
        //wxMessageDialog dialog2(this, info, _T("Selected file"));
        //dialog2.ShowModal();
    }
}

void SolarSystemWindow::OnFixedCoordinatesButton(wxCommandEvent& WXUNUSED (event))
{
    
}

void SolarSystemWindow::OnCreateButton(wxCommandEvent& WXUNUSED (event))
{
   Close();  //Edit this later
}

void SolarSystemWindow::OnCancelButton(wxCommandEvent& WXUNUSED (event))
{
     Close();
}

/*wxDialog *SolarSystemWindow::CreateRadiobox()
{
  
}*/
