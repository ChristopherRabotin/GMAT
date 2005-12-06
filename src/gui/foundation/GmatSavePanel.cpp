//$Header$
//------------------------------------------------------------------------------
//                              GmatSavePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/02
//
/**
 * Implements GmatSavePanel class.
 */
//------------------------------------------------------------------------------

#include "GmatSavePanel.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"

#include "ShowScriptDialog.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(GmatSavePanel, wxPanel)
   EVT_BUTTON(ID_BUTTON_SAVE, GmatSavePanel::OnSave)
   EVT_BUTTON(ID_BUTTON_SAVE_AS, GmatSavePanel::OnSaveAs)
   EVT_BUTTON(ID_BUTTON_CLOSE, GmatSavePanel::OnClose)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatSavePanel::OnScript)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// GmatSavePanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs GmatSavePanel object.
 *
 * @param <parent> parent window.
 *
 */
//------------------------------------------------------------------------------
GmatSavePanel::GmatSavePanel(wxWindow *parent, bool showScriptButton,
    wxString filename)
    : wxPanel(parent)
{
    
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   canClose = true;
   mShowScriptButton = showScriptButton;
   mFilename = filename;

   theParent = parent;

   int borderSize = 3;
   wxStaticBox *topStaticBox = new wxStaticBox( this, -1, wxT("") );
   wxStaticBox *middleStaticBox = new wxStaticBox( this, -1, wxT("") );
   wxStaticBox *bottomStaticBox = new wxStaticBox( this, -1, wxT("") );

   // create sizers
   thePanelSizer = new wxBoxSizer(wxVERTICAL);
   theTopSizer = new wxStaticBoxSizer( topStaticBox, wxVERTICAL );
   theMiddleSizer = new wxStaticBoxSizer( middleStaticBox, wxVERTICAL );
   theBottomSizer = new wxStaticBoxSizer( bottomStaticBox, wxVERTICAL );
   wxBoxSizer *theButtonSizer = new wxBoxSizer(wxHORIZONTAL);

   // create script button
   theScriptButton = new wxButton(this, ID_BUTTON_SCRIPT, "Show Script",
                                  wxDefaultPosition, wxDefaultSize, 0);

   // create bottom buttons
   theSaveButton =
      new wxButton(this, ID_BUTTON_SAVE, "Save", wxDefaultPosition, wxDefaultSize, 0);
   theSaveAsButton =
      new wxButton(this, ID_BUTTON_SAVE_AS, "Save As", wxDefaultPosition, wxDefaultSize, 0);
   theCloseButton =
      new wxButton(this, ID_BUTTON_CLOSE, "Close", wxDefaultPosition, wxDefaultSize, 0);
   theHelpButton =
      new wxButton(this, ID_BUTTON_HELP, "Help", wxDefaultPosition, wxDefaultSize, 0);

   // add items to top sizer
   theTopSizer->Add(theScriptButton, 0, wxALIGN_RIGHT | wxALL, borderSize);

   // adds the buttons to button sizer
   theButtonSizer->Add(theSaveButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   theButtonSizer->Add(theSaveAsButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   theButtonSizer->Add(theCloseButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   theButtonSizer->Add(theHelpButton, 0, wxALIGN_CENTER | wxALL, borderSize);

   theBottomSizer->Add(theButtonSizer, 0, wxALIGN_CENTER | wxALL, borderSize);

   topStaticBox->Show(mShowScriptButton);

   mObject = NULL;
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void Show()
//------------------------------------------------------------------------------
/**
 * Shows the panel.
 */
//------------------------------------------------------------------------------
void GmatSavePanel::Show()
{
    // add items to middle sizer

    thePanelSizer->Add(theTopSizer, 0, wxGROW | wxALL, 1);
    thePanelSizer->Add(theMiddleSizer, 1, wxGROW | wxALL, 1);
    thePanelSizer->Add(theBottomSizer, 0, wxGROW | wxALL, 1);

    // displays the script button
    thePanelSizer->Show(theTopSizer, mShowScriptButton);
    theScriptButton->Show(mShowScriptButton);
    thePanelSizer->Layout();

    // tells the enclosing window to adjust to the size of the sizer
    SetAutoLayout( TRUE );
    SetSizer(thePanelSizer); //use the sizer for layout
    thePanelSizer->Fit(this); //loj: if theParent is used it doesn't show the scroll bar
    thePanelSizer->SetSizeHints(this); //set size hints to honour minimum size

    theSaveButton->Disable();
    LoadData();

//    theScriptButton->Disable(); //loj: for build2
    theHelpButton->Disable();   //loj: for build2
}

//------------------------------------------------------------------------------
// void OnOk()
//------------------------------------------------------------------------------
/**
 * Saves the data and closes the page
 */
//------------------------------------------------------------------------------
void GmatSavePanel::OnSave(wxCommandEvent &event)
{
   if (mFilename == "")
   {
      OnSaveAs(event);
      return;
   }

   SaveData();
}

//------------------------------------------------------------------------------
// void OnClose()
//------------------------------------------------------------------------------
/**
 * Close page.
 */
//------------------------------------------------------------------------------
void GmatSavePanel::OnClose(wxCommandEvent &event)
{
   if (theSaveButton->IsEnabled())
   {
      wxMessageDialog *msgDlg = new wxMessageDialog(this,
         "Would you like to save changes?", "Save...", wxYES_NO | wxICON_QUESTION ,
         wxDefaultPosition);
      int result = msgDlg->ShowModal();

      if (result == wxID_YES)
      {
         OnSave(event);
      }
   }

   GmatAppData::GetMainFrame()->CloseActiveChild();
}

//------------------------------------------------------------------------------
// void OnApply()
//------------------------------------------------------------------------------
/**
 * Saves the data and remain unclosed.
 */
//------------------------------------------------------------------------------
void GmatSavePanel::OnSaveAs(wxCommandEvent &event)
{
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""),
         _T("Script files (*.script, *.m)|*.script;*.m|"\
            "Text files (*.txt, *.text)|*.txt;*.text|"\
            "All files (*.*)|*.*"), wxSAVE);

   if (dialog.ShowModal() == wxID_OK)
   {
//      wxString filename = dialog.GetFilename().c_str();
      wxString path = dialog.GetPath().c_str();

      if(FileExists(path.c_str()))
      {
           if (wxMessageBox(_T("File already exists.\nDo you want to overwrite?"), 
               _T("Please confirm"), wxICON_QUESTION | wxYES_NO) == wxYES)
           {
              // just for now
              mFilename = path;
              SaveData();
           }
           else
           {
                return;
           }
      }
      else
      {
          // just for now
          mFilename = path;
          SaveData();
      }


   }


}

//------------------------------------------------------------------------------
// void OnHelp()
//------------------------------------------------------------------------------
/**
 * Shows Helps
 */
//------------------------------------------------------------------------------
void GmatSavePanel::OnHelp(wxCommandEvent &event)
{
   // open separate window to show help?
}

//------------------------------------------------------------------------------
// void OnScript()
//------------------------------------------------------------------------------
/**
 * Shows Scripts
 */
//------------------------------------------------------------------------------
void GmatSavePanel::OnScript(wxCommandEvent &event)
{
   wxString title = "Object Script";
   // open separate window to show scripts?
   if (mObject != NULL) {
      title = "Scripting for ";
      title += mObject->GetName().c_str();
   }
   ShowScriptDialog ssd(this, -1, title, mObject);
   ssd.ShowModal();
}

bool GmatSavePanel::FileExists(std::string scriptFilename)
{
  FILE * pFile;
  pFile = fopen (scriptFilename.c_str(),"rt+");
  if (pFile!=NULL)
  {
    fclose (pFile);
    return true;
  }
  else
    return false;

}

