//$Header$
//------------------------------------------------------------------------------
//                              UniversePanel
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
// Author: Monisha Butler
// Created: 2003/09/10
// Modified: 2004/01/13 by Allison Greene for action handling
/**
 * This class allows user to specify where Universe information is 
 * coming from
 */
//------------------------------------------------------------------------------
#include "UniversePanel.hpp"
#include "MessageInterface.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(UniversePanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)

   EVT_BUTTON(ID_BUTTON_ADD,       UniversePanel::OnAddButton)
   EVT_BUTTON(ID_BUTTON_SORT,      UniversePanel::OnSortButton)
   EVT_BUTTON(ID_BUTTON_REMOVE,    UniversePanel::OnRemoveButton)
   EVT_BUTTON(ID_SLP_BUTTON_BROWSE,    UniversePanel::OnBrowseButton)
   EVT_BUTTON(ID_DE405_BUTTON_BROWSE,    UniversePanel::OnBrowseButton)
   EVT_BUTTON(ID_DE200_BUTTON_BROWSE,    UniversePanel::OnBrowseButton)

   EVT_LISTBOX(ID_AVAILABLE_LIST, UniversePanel::OnAvailableSelectionChange)
   EVT_LISTBOX(ID_SELECTED_LIST, UniversePanel::OnSelectedSelectionChange)

END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
//------------------------------------------------------------------------------
// UniversePanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs UniversePanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the Universe GUI
 */
//------------------------------------------------------------------------------
UniversePanel::UniversePanel(wxWindow *parent):GmatPanel(parent)
{
    Create();
    Show();
}

UniversePanel::~UniversePanel()
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
/**
 * @param <scName> input spacecraft name.
 *
 * @note Creates the notebook for spacecraft information
 */
//------------------------------------------------------------------------------
void UniversePanel::Create()
{
   // SetParent(new wxFrame(0,-1,"title"));
   // parent = GetParent();
    
    //MessageInterface::ShowMessage("UniversePanel::Create() entering\n");
    
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
    wxGridSizer *item1 = new wxGridSizer( 3, 0, 0 );
    wxBoxSizer *item2 = new wxBoxSizer( wxVERTICAL );
    
    item3 = new wxStaticText( this, ID_TEXT, wxT("Available"), wxDefaultPosition, 
                              wxSize(80,-1), 0 );
    item2->Add( item3, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxString availableStrs [] = 
    {
    };

//        ag: not needed for build 2
//        wxT("Low Accuracy Analytic"), 
//        wxT("High Accuracy Analytic")

    availableListBox = new wxListBox( this, ID_AVAILABLE_LIST, wxDefaultPosition, 
                                      wxSize(140,125), 0,
                                      availableStrs, wxLB_SINGLE );
    item2->Add( availableListBox, 0, wxALIGN_CENTRE|wxALL, 5 );
    item1->Add( item2, 0, wxALIGN_CENTRE|wxALL, 5 );
    wxBoxSizer *item5 = new wxBoxSizer( wxVERTICAL );
    item5->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    addButton = new wxButton( this, ID_BUTTON_ADD, wxT("Add"), wxDefaultPosition, 
                              wxDefaultSize, 0 );
    item5->Add( addButton, 0, wxALIGN_CENTRE|wxALL, 5 );

    // ag:  changed button from "sort" to "prioritize"
    prioritizeButton = new wxButton( this, ID_BUTTON_SORT, wxT("Prioritize"), 
                                      wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( prioritizeButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    prioritizeButton->Enable(false);

    removeButton = new wxButton( this, ID_BUTTON_REMOVE, wxT("Remove"), 
                                 wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( removeButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    removeButton->Enable(false);
    item1->Add( item5, 0, wxALIGN_CENTRE|wxALL, 5 );
    wxBoxSizer *item9 = new wxBoxSizer( wxVERTICAL );
    item10 = new wxStaticText( this, ID_TEXT, wxT("Selected"), wxDefaultPosition, 
                               wxSize(80,-1), 0 );
    item9->Add( item10, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxString strs11[] = 
    {
    };
    selectedListBox = new wxListBox( this, ID_SELECTED_LIST, wxDefaultPosition, 
                                     wxSize(140,125), 0, strs11, wxLB_SINGLE );
    item9->Add( selectedListBox, 0, wxALIGN_CENTRE|wxALL, 5 );

    item1->Add( item9, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxBoxSizer *fileSizer = new wxBoxSizer(wxHORIZONTAL);
    // will need to change
    slpStaticText = new wxStaticText( this, ID_TEXT, 
                                       wxT("SLP File: "), 
                                       wxDefaultPosition, wxSize(80,-1), 0 );
    slpFileTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""), 
                                              wxDefaultPosition, 
                                              wxSize(250, -1),  0);
    browseButton = new wxButton( this, ID_SLP_BUTTON_BROWSE, wxT("Browse"), 
                                              wxDefaultPosition, wxDefaultSize, 0 );
 
    fileSizer->Add(slpStaticText, 0, wxALIGN_CENTER|wxALL, 5);
    fileSizer->Add(slpFileTextCtrl, 0, wxALIGN_CENTER|wxALL, 5);
    fileSizer->Add(browseButton, 0, wxALIGN_CENTER|wxALL, 5);


    wxBoxSizer *de405FileSizer = new wxBoxSizer(wxHORIZONTAL);
    de405StaticText = new wxStaticText( this, ID_TEXT, 
                                       wxT("DE405 File: "), 
                                       wxDefaultPosition, wxSize(80,-1), 0 );
    de405FileTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""), 
                                              wxDefaultPosition, 
                                              wxSize(250, -1),  0);
    browseButton = new wxButton( this, ID_DE405_BUTTON_BROWSE, wxT("Browse"), 
                                              wxDefaultPosition, wxDefaultSize, 0 );
 
    de405FileSizer->Add(de405StaticText, 0, wxALIGN_CENTER|wxALL, 5);
    de405FileSizer->Add(de405FileTextCtrl, 0, wxALIGN_CENTER|wxALL, 5);
    de405FileSizer->Add(browseButton, 0, wxALIGN_CENTER|wxALL, 5);


    wxBoxSizer *de200FileSizer = new wxBoxSizer(wxHORIZONTAL);
    de200StaticText = new wxStaticText( this, ID_TEXT, 
                                       wxT("DE200 File: "), 
                                       wxDefaultPosition, wxSize(80,-1), 0 );
    de200FileTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""), 
                                              wxDefaultPosition, 
                                              wxSize(250, -1),  0);
    browseButton = new wxButton( this, ID_DE200_BUTTON_BROWSE, wxT("Browse"), 
                                              wxDefaultPosition, wxDefaultSize, 0 );
 
    de200FileSizer->Add(de200StaticText, 0, wxALIGN_CENTER|wxALL, 5);
    de200FileSizer->Add(de200FileTextCtrl, 0, wxALIGN_CENTER|wxALL, 5);
    de200FileSizer->Add(browseButton, 0, wxALIGN_CENTER|wxALL, 5);

    item0->Add( item1, 0, wxALIGN_CENTRE|wxALL, 5 );
    item0->Add(fileSizer, 0, wxALIGN_CENTER|wxALL, 5);
    item0->Add(de405FileSizer, 0, wxALIGN_CENTER|wxALL, 5);
    item0->Add(de200FileSizer, 0, wxALIGN_CENTER|wxALL, 5);

    theMiddleSizer->Add(item0, 0, wxALIGN_CENTER|wxALL, 5);
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void UniversePanel::LoadData()
{
    // load data from the core engine
    
    //loj: 4/6/04 updated due to GuiInterpreter change
    StringArray fileTypes = theGuiInterpreter->GetPlanetaryFileTypes();
    StringArray fileTypesInUse = theGuiInterpreter->GetPlanetaryFileTypesInUse();
    
    // available source
    for (unsigned int i=0; i<fileTypes.size(); i++)
    {
        availableListBox->Append(fileTypes[i].c_str());
    }
    
    // selected source
    for (unsigned int i=0; i<fileTypesInUse.size(); i++)
    {
        selectedListBox->Append(fileTypesInUse[i].c_str());
        //MessageInterface::ShowMessage("UniversePanel::LoadData() filesInUse = %s\n",
        //                              filesInUse[i].c_str());
    }

    //loj: 4/6/04
    //@todo - use combobox for DE files
    for (unsigned int i=0; i<fileTypes.size(); i++)
    {
        //MessageInterface::ShowMessage("UniversePanel::LoadData() fileNames = %s\n",
        //                              fileNames[i].c_str());
        
        if (fileTypes[i] == "SLP")
        {
            slpFileTextCtrl->SetValue(theGuiInterpreter->
                                      GetPlanetaryFileName("SLP").c_str());
        }
        else if (fileTypes[i] == "DE200")
        {
            de200FileTextCtrl->SetValue(theGuiInterpreter->
                                        GetPlanetaryFileName("DE200").c_str());
        }
        else if (fileTypes[i] == "DE405")
        {
            de405FileTextCtrl->SetValue(theGuiInterpreter->
                                        GetPlanetaryFileName("DE405").c_str());
        }
    }

    if (fileTypesInUse.size() > 0)
        removeButton->Enable(true);

    if (fileTypesInUse.size() > 1)
        prioritizeButton->Enable(true); //loj: 4/14/04 added
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void UniversePanel::SaveData()
{
    // save data to core engine

    //loj: 4/5/04 updated due to GuiInterpreter change:
    theFileTypesInUse.clear();

    // put planetary file types in the priority order
    for (int i=0; i<selectedListBox->GetCount(); i++)
    {
        theFileTypesInUse.push_back(std::string(selectedListBox->GetString(i)));
//        MessageInterface::ShowMessage("UniversePanel::SaveData() types=%s\n",
//                                      theFileTypesInUse[i].c_str());
    }

    theGuiInterpreter->SetPlanetaryFileTypesInUse(theFileTypesInUse);

    theApplyButton->Enable(false);
    
//      //if first item in the list is "SLP"
//      if (selectedListBox->GetString(0).IsSameAs("SLP"))
//      {
//          wxString absoluteFilename = slpFileTextCtrl->GetValue();

//          MessageInterface::ShowMessage("UniversePanel::SaveData() "
//                                        "calling theGuiInterpreter->SetSlpFileInUse()\n");
        
//          //ag:        theGuiInterpreter->SetSlpFileInUse("mn2000-little.dat"); 
//          //loj: 4/6/04 - use SetPlanetaryFileInUse(....)
//          theGuiInterpreter->SetSlpFileInUse(std::string (absoluteFilename.c_str()));
//      }
}

void UniversePanel::OnAddButton(wxCommandEvent& event)
{
    // get string in first list and then search for it
    // in the second list
    wxString s = availableListBox->GetStringSelection();
    int found = selectedListBox->FindString(s);
    
    // if the string wasn't found in the second list, insert it
    if ( found == wxNOT_FOUND )
    {
      selectedListBox->Insert(s, 0);
      selectedListBox->SetSelection(0);
      theApplyButton->Enable(); //loj: 2/27/04 added
    }

    removeButton->Enable(false);
    prioritizeButton->Enable(false);
    
    if (selectedListBox->GetCount() > 0)
        removeButton->Enable(true);

    if (selectedListBox->GetCount() > 1)
        prioritizeButton->Enable(true);
    
}

// moves selected item to the top of the lsit
void UniversePanel::OnSortButton(wxCommandEvent& event)
{
    // get string
    wxString s = selectedListBox->GetStringSelection();

    if (!s.IsEmpty())
    {
      // remove string
      int sel = selectedListBox->GetSelection();
      selectedListBox->Delete(sel);
      // add string to top
      selectedListBox->Insert(s, 0);
    }
    
    theApplyButton->Enable(true);
}

void UniversePanel::OnRemoveButton(wxCommandEvent& event)
{
    int sel = selectedListBox->GetSelection();
    selectedListBox->Delete(sel);

    removeButton->Enable(false);
    prioritizeButton->Enable(false);
    
    if (selectedListBox->GetCount() > 0)
        removeButton->Enable(true);

    if (selectedListBox->GetCount() > 1)
        prioritizeButton->Enable(true);
    
    theApplyButton->Enable(true);
}

void UniversePanel::OnBrowseButton(wxCommandEvent& event)
{
    int textCtrlId = event.GetId();
    wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
    
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString filename;
        
        filename = dialog.GetPath().c_str();
        
        if (textCtrlId == ID_SLP_BUTTON_BROWSE)
          slpFileTextCtrl->SetValue(filename); 
        else if (textCtrlId == ID_DE405_BUTTON_BROWSE)
          de405FileTextCtrl->SetValue(filename); 
        else if (textCtrlId == ID_DE200_BUTTON_BROWSE)
          de200FileTextCtrl->SetValue(filename); 
    }
}

void UniversePanel::OnAvailableSelectionChange(wxCommandEvent& event)
{
    // get string
    wxString s = availableListBox->GetStringSelection();

    if (selectedListBox->FindString(s) == wxNOT_FOUND)
    {
         addButton->Enable(true);
    }
}
void UniversePanel::OnSelectedSelectionChange(wxCommandEvent& event)
{
    // get string
//      wxString s = availableListBox->GetStringSelection();

//      if (s.IsSameAs("SLP"))
//      {
//      }
//      else
//      {

//      }
    
}
