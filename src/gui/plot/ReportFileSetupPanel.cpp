//$Header$
//------------------------------------------------------------------------------
//                              ReportFileSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/02/12
/**
 * Implements ReportFileSetupPanel class.
 */
//------------------------------------------------------------------------------
#include "ReportFileSetupPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "GuiItemManager.hpp"

#include "MessageInterface.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(ReportFileSetupPanel, GmatPanel)
    EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
    EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
    EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
    EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
    EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
    
    EVT_BUTTON(RF_WRITE_CHECKBOX, ReportFileSetupPanel::OnWriteCheckBoxChange)
    EVT_CHECKBOX(RF_WRITE_CHECKBOX, ReportFileSetupPanel::OnWriteCheckBoxChange)
    
    EVT_BUTTON(ID_BROWSE_BUTTON, ReportFileSetupPanel::OnBrowseButton)
    EVT_TEXT(ID_TEXT_CTRL, ReportFileSetupPanel::OnTextChange)
    EVT_TEXT(ID_TEXT, ReportFileSetupPanel::OnTextChange)

END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
    
//------------------------------------------------------------------------------
// ReportFileSetupPanel(wxWindow *parent, const wxString &subscriberName)
//------------------------------------------------------------------------------
/**
 * Constructs ReportFileSetupPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the ReportFileSetupPanel GUI
 */
//------------------------------------------------------------------------------
ReportFileSetupPanel::ReportFileSetupPanel(wxWindow *parent,
                                   const wxString &subscriberName)
    : GmatPanel(parent)
{
    //MessageInterface::ShowMessage("ReportFileSetupPanel() entering...\n");
    //MessageInterface::ShowMessage("ReportFileSetupPanel() subscriberName = " +
    //                              std::string(subscriberName.c_str()) + "\n");
    
    theSubscriber =
        theGuiInterpreter->GetSubscriber(std::string(subscriberName.c_str()));

    Create();
    Show();
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnWriteCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnWriteCheckBoxChange(wxCommandEvent& event)
{
    theApplyButton->Enable();
}

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
void ReportFileSetupPanel::Create()
{
    //MessageInterface::ShowMessage("ReportFileSetupPanel::Create() entering...\n");

    pageBoxSizer = new wxBoxSizer(wxVERTICAL);

    //------------------------------------------------------
    // plot option, (1st column)
    //------------------------------------------------------
    writeCheckBox = new wxCheckBox(this, RF_WRITE_CHECKBOX, wxT("Write Report"),
                                   wxDefaultPosition, wxSize(100, -1), 0);
            
    optionBoxSizer = new wxBoxSizer(wxVERTICAL);
    optionBoxSizer->Add(writeCheckBox, 0, wxALIGN_LEFT|wxALL, 5);
            
    //------------------------------------------------------
    // put in the order
    //------------------------------------------------------
    pageBoxSizer->Add(optionBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    
    //------------------------------------------------------
    // file option
    //------------------------------------------------------   
    wxBoxSizer *fileSizer = new wxBoxSizer(wxHORIZONTAL);
    // will need to change
    fileStaticText = new wxStaticText( this, ID_TEXT, 
                                       wxT("File: "), 
                                       wxDefaultPosition, wxSize(80,-1), 0 );
    fileTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""), 
                                              wxDefaultPosition, 
                                              wxSize(250, -1),  0);
    browseButton = new wxButton( this, ID_BROWSE_BUTTON, wxT("Browse"), 
                                              wxDefaultPosition, wxDefaultSize, 0 );
 
    fileSizer->Add(fileStaticText, 0, wxALIGN_CENTER|wxALL, 5);
    fileSizer->Add(fileTextCtrl, 0, wxALIGN_CENTER|wxALL, 5);
    fileSizer->Add(browseButton, 0, wxALIGN_CENTER|wxALL, 5);

    //------------------------------------------------------
    // add to parent sizer
    //------------------------------------------------------
    theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    theMiddleSizer->Add(fileSizer, 0, wxALIGN_CENTRE|wxALL, 5);

}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ReportFileSetupPanel::LoadData()
{
    // load data from the core engine
    writeCheckBox->SetValue(theSubscriber->IsActive());

    // load file name data from core engine
    int filenameId = theSubscriber->GetParameterID("Filename");
    std::string filename = theSubscriber->GetStringParameter(filenameId);
    fileTextCtrl->SetValue(wxT(filename.c_str()));
    
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ReportFileSetupPanel::SaveData()
{
    // save data to core engine
    theSubscriber->Activate(writeCheckBox->IsChecked());
    
    // save file name data to core engine
    wxString filename = fileTextCtrl->GetValue();
    int filenameId = theSubscriber->GetParameterID("Filename");
    theSubscriber->SetStringParameter(filenameId, 
                  std::string (filename.c_str()));
}

//------------------------------------------------------------------------------
// void OnBrowseButton()
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnBrowseButton(wxCommandEvent& event)
{
    wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
    
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString filename;
        
        filename = dialog.GetPath().c_str();
        fileTextCtrl->SetValue(filename); 
    }
}

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnTextChange()
{
    theApplyButton->Enable();
}

