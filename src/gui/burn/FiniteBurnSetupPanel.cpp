//$Header$
//------------------------------------------------------------------------------
//                              FiniteBurnSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: LaMont Ruley
// Created: 2004/03/04
//
/**
 * This class contains the Finite Burn Setup window.
 */
//------------------------------------------------------------------------------

#include "FiniteBurnSetupPanel.hpp"

#include "MessageInterface.hpp"
//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FiniteBurnSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_COMBOBOX(ID_FRAME_COMBOBOX, FiniteBurnSetupPanel::OnFrameComboBoxChange)
   EVT_TEXT(ID_TEXTCTRL, FiniteBurnSetupPanel::OnTextChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// FiniteBurnSetupPanel(wxWindow *parent, const wxString &burnName)
//------------------------------------------------------------------------------
/**
 * Constructs FiniteBurnSetupPanel object.
 *
 * @param <parent> input parent.
 * @param <burnName> input burn name.
 *
 * @note Creates the Finite burn GUI
 */
//------------------------------------------------------------------------------
FiniteBurnSetupPanel::FiniteBurnSetupPanel(wxWindow *parent, const wxString &burnName)
   :GmatPanel(parent)
{
   theBurn = theGuiInterpreter->GetBurn(std::string(burnName.c_str()));

   Create();
   Show();
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnFrameComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void FiniteBurnSetupPanel::OnFrameComboBoxChange(wxCommandEvent& event)
{
    theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void FiniteBurnSetupPanel::OnTextChange(wxCommandEvent& event)
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
 *
 * @note Creates the panel for the Finite burn data
 */
//------------------------------------------------------------------------------
void FiniteBurnSetupPanel::Create()
{
   unsigned int i, count;
   StringArray items;
    
   int bsize = 3; // bordersize

   if (theBurn != NULL)
   {
      // create sizers
      wxBoxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);
      wxBoxSizer *topSizer = new wxBoxSizer(wxHORIZONTAL);
      wxBoxSizer *middleSizer = new wxBoxSizer(wxHORIZONTAL);
      wxBoxSizer *bottomSizer = new wxBoxSizer(wxHORIZONTAL);

      // label for coordinate frames combo box
      wxStaticText *frameLabel = new wxStaticText(this, ID_TEXT,
         wxT("Coordinate frame:"), wxDefaultPosition, wxDefaultSize, 0);
        
      // list of coordinate frames
      Integer id = theBurn->GetParameterID("CoordinateFrame");
      items = theBurn->GetStringArrayParameter(id);
      count = items.size();
      wxString *frameList = new wxString[count];
       
      if (count > 0)        // check to see if any coordinate frames exist
      {
         for (i=0; i<count; i++)
            frameList[i] = wxString(items[i].c_str());

            // combo box for avaliable coordinate frames 
            frameCB = new wxComboBox(this, ID_FRAME_COMBOBOX, wxT(""), 
               wxDefaultPosition, wxSize(150,-1), count, frameList, wxCB_DROPDOWN);
      }
      else                 // no coordinate frames exist
      { 
         wxString strs6[] =
         {
            wxT("No coordinate frame available") 
         };    
         frameCB = new wxComboBox(this, ID_FRAME_COMBOBOX, wxT(""),
            wxDefaultPosition, wxSize(150,-1), 1, strs6, wxCB_DROPDOWN);
      }
        
      // add frame label and combobox to top sizer    
      topSizer->Add(frameLabel, 0, wxALIGN_CENTER | wxALL, 5);
      topSizer->Add(frameCB, 0, wxALIGN_CENTER | wxALL, 5);

      // create grid
      wxGrid *engGrid =
              new wxGrid(this, -1, wxDefaultPosition, wxSize(200,125), wxWANTS_CHARS);

      engGrid->CreateGrid(MAX_PROP_ROW, 2, wxGrid::wxGridSelectRows);
      engGrid->SetColSize(0, 50);
      engGrid->SetColSize(1, 150);
      engGrid->SetMargins(0, 0);
      engGrid->SetColLabelValue(0, _T("Use"));
      engGrid->SetColLabelValue(1, _T("Engine"));
      engGrid->SetRowLabelSize(0);
      engGrid->SetScrollbars(5, 8, 15, 15);

      // set the engine column as read only
      wxGridCellAttr *attrRead = new wxGridCellAttr;
      attrRead->SetReadOnly(true);
      engGrid->SetColAttr(1, attrRead);

      // create and insert boolean choice editor  for enable disable column
      wxGridCellAttr *attrBool = new wxGridCellAttr;
      attrBool->SetEditor(new wxGridCellBoolEditor);
      attrBool->SetRenderer(new wxGridCellBoolRenderer);
      engGrid->SetColAttr(0, attrBool);
      engGrid->SetColFormatBool(0);

      // add grid to middle sizer        
      middleSizer->Add(engGrid, 0, wxALIGN_CENTER|wxALL, bsize);

      // create label and text field for thrust scale factor
      wxStaticText *scaleLabel = new wxStaticText(this, ID_TEXT,
         wxT("Burn scale factor:"), wxDefaultPosition, wxDefaultSize, 0);
      wxTextCtrl *scaleTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
         wxDefaultPosition, wxSize(50,-1), 0 );

      // add scale factor label and text field to bottom sizer    
      bottomSizer->Add(scaleLabel, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5);
      bottomSizer->Add(scaleTextCtrl, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5);

      // add frame sizer and table size to page sizer
      pageSizer->Add(topSizer, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5);
      pageSizer->Add(middleSizer, 0, wxGROW| wxALIGN_CENTER_VERTICAL| wxALL, 5);
      pageSizer->Add(bottomSizer, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5);

      // add page sizer to middle sizer
      theMiddleSizer->Add(pageSizer, 0, wxALIGN_CENTRE|wxALL, 5);
   }
   else
   {
      // show error message
      MessageInterface::ShowMessage("FiniteBurnSetupPanel:Create() theBurn is NULL\n");
   }
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void FiniteBurnSetupPanel::LoadData()
{
    // Load data from the core engine 
    // Coordinate frame   
    Integer id = theBurn->GetParameterID("CoordinateFrame");
    std::string coordFrame = theBurn->GetStringParameter(id);
    StringArray frames = theBurn->GetStringArrayParameter(id);
    int index = 0;
    for (StringArray::iterator iter = frames.begin(); 
        iter != frames.end(); ++iter) 
    {
        if (coordFrame == *iter) 
            frameCB->SetSelection(index);
        else
            ++index;
    }

    // Engines

    // Burn scale factor
//    int bsfID = theBurn->GetParameterID("BurnScaleFactor");
//    Real bsf = theBurn->GetRealParameter(bsfID);
//    wxString bsfStr;
//    bsfStr.Printf("%f", bsf);
//    scaleTextCtrl->SetValue(bsfStr);
    
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void FiniteBurnSetupPanel::SaveData()
{
    // Save data to core engine
    // Coordinate frame
    wxString frameString = frameCB->GetStringSelection();
    Integer id = theBurn->GetParameterID("CoordinateFrame");
    std::string coordFrame = std::string (frameString.c_str());
    theBurn->SetStringParameter(id, coordFrame);
    
    // Engine

    // Burn scale factor
//    int bsfID = theBurn->GetParameterID("BurnScaleFactor");
//    wxString bsfStr = scaleTextCtrl->GetValue();
//    theBurn->SetRealParameter(bsfID, atof(bsfStr));

    theApplyButton->Disable();
}
