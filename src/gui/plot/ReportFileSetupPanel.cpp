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
    
   EVT_BUTTON(ADD_VAR_BUTTON, ReportFileSetupPanel::OnAddVariable)
   EVT_BUTTON(REMOVE_VAR_BUTTON, ReportFileSetupPanel::OnRemoveVariable)
   EVT_BUTTON(CLEAR_VAR_BUTTON, ReportFileSetupPanel::OnClearVariable)

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
    theApplyButton->Disable();
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

    Integer bsize = 3; // border size
    wxString emptyList[] = {};

    wxBoxSizer *variablesBoxSizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer *mFlexGridSizer = new wxFlexGridSizer(5, 0, 0);
           
   //------------------------------------------------------
   // available variables list (1st column)
   //------------------------------------------------------
   mVarBoxSizer = new wxBoxSizer(wxVERTICAL);
    
   wxStaticText *titleAvailable =
      new wxStaticText(this, -1, wxT("Variables"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxArrayString empty;
   
   mVarListBox = 
       theGuiManager->GetConfigParameterListBox(this, wxSize(150,200));
    
   mVarBoxSizer->Add(titleAvailable, 0, wxALIGN_CENTRE|wxALL, bsize);
   mVarBoxSizer->Add(mVarListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   //------------------------------------------------------
   // add, remove, clear parameter buttons (2nd column)
   //------------------------------------------------------
   wxButton *addScButton = new wxButton(this, ADD_VAR_BUTTON, wxT("-->"),
                              wxDefaultPosition, wxSize(20,20), 0);

   wxButton *removeScButton = new wxButton(this, REMOVE_VAR_BUTTON, wxT("<--"),
                                 wxDefaultPosition, wxSize(20,20), 0);
    
   wxButton *clearScButton = new wxButton(this, CLEAR_VAR_BUTTON, wxT("<="),
                                wxDefaultPosition, wxSize(20,20), 0);
    
   wxBoxSizer *arrowButtonsBoxSizer = new wxBoxSizer(wxVERTICAL);
   arrowButtonsBoxSizer->Add(addScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsBoxSizer->Add(removeScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsBoxSizer->Add(clearScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   //------------------------------------------------------
   // selected spacecraft list (4th column)
   //------------------------------------------------------
   wxStaticText *titleSelected =
      new wxStaticText(this, -1, wxT("Selected Variables"),
                       wxDefaultPosition, wxSize(-1,-1), 0);

   mVarSelectedListBox =
      new wxListBox(this, VAR_SEL_LISTBOX, wxDefaultPosition,
                    wxSize(150,200), 0, emptyList, wxLB_SINGLE);
        
   wxBoxSizer *mVarSelectedBoxSizer = new wxBoxSizer(wxVERTICAL);
   mVarSelectedBoxSizer->Add(titleSelected, 0, wxALIGN_CENTRE|wxALL, bsize);
   mVarSelectedBoxSizer->Add(mVarSelectedListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   //------------------------------------------------------
   // put in the order
   //------------------------------------------------------    
   mFlexGridSizer->Add(mVarBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(arrowButtonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(mVarSelectedBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   variablesBoxSizer->Add(mFlexGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

    //------------------------------------------------------
    // add to parent sizer
    //------------------------------------------------------
    theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    theMiddleSizer->Add(fileSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    theMiddleSizer->Add(variablesBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
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
    
    StringArray varParamList = theSubscriber->GetStringArrayParameter("VarList");
    mNumVarParams = varParamList.size();

    if (mNumVarParams > 0)
    {

       wxString *varParamNames = new wxString[mNumVarParams];
       Parameter *param;
         
       for (int i=0; i<mNumVarParams; i++)
       {
          varParamNames[i] = varParamList[i].c_str();
          param = theGuiInterpreter->GetParameter(varParamList[i]);
       }
    
       mVarSelectedListBox->Set(mNumVarParams, varParamNames);
       mVarSelectedListBox->SetSelection(0);
       delete varParamNames;
    }    

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
                 
            
    mNumVarParams = mVarSelectedListBox->GetCount();
    
    if (mNumVarParams >= 0) // >=0 because the list needs to be cleared
    {
         theSubscriber->SetBooleanParameter("Clear", true);
         for (int i=0; i<mNumVarParams; i++)
         {
            theSubscriber->
               SetStringParameter("Add",
                                  std::string(mVarSelectedListBox->GetString(i).c_str()));
         }
      }

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

//------------------------------------------------------------------------------
// void OnAddSpacecraft(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnAddVariable(wxCommandEvent& event)
{
   // get string in first list and then search for it
   // in the second list
   wxString s = mVarListBox->GetStringSelection();
   int found = mVarSelectedListBox->FindString(s);
    
   // if the string wasn't found in the second list, insert it
   if (found == wxNOT_FOUND)
   {
      mVarSelectedListBox->Append(s);
      mVarSelectedListBox->SetStringSelection(s);
      theApplyButton->Enable();
   }
}

//------------------------------------------------------------------------------
// void OnRemoveSpacecraft(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnRemoveVariable(wxCommandEvent& event)
{
   int sel = mVarSelectedListBox->GetSelection();
   mVarSelectedListBox->Delete(sel);

   if (sel-1 < 0)
   {
      mVarSelectedListBox->SetSelection(0);
   }
   else
   {
      mVarSelectedListBox->SetSelection(sel-1);
   }
   
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnClearSpacecraft(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFileSetupPanel::OnClearVariable(wxCommandEvent& event)
{
   mVarSelectedListBox->Clear();
   theApplyButton->Enable();
}


