//$Header$
//------------------------------------------------------------------------------
//                              ImpulsiveBurnSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: LaMont Ruley
// Created: 2004/02/04
//
/**
 * This class contains the Impulsive Burn Setup window.
 */
//------------------------------------------------------------------------------

#include "ImpulsiveBurnSetupPanel.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ImpulsiveBurnSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_COMBOBOX(ID_FRAME_COMBOBOX, ImpulsiveBurnSetupPanel::OnFrameComboBoxChange)
   EVT_COMBOBOX(ID_FORMAT_COMBOBOX, ImpulsiveBurnSetupPanel::OnFormatComboBoxChange)
   EVT_TEXT(ID_TEXTCTRL, ImpulsiveBurnSetupPanel::OnTextChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// ImpulsiveBurnSetupPanel(wxWindow *parent, const wxString &burnName)
//------------------------------------------------------------------------------
/**
 * Constructs ImpulsiveBurnSetupPanel object.
 *
 * @param <parent> input parent.
 * @param <burnName> input burn name.
 *
 * @note Creates the impulsive burn GUI
 */
//------------------------------------------------------------------------------
   ImpulsiveBurnSetupPanel::ImpulsiveBurnSetupPanel(wxWindow *parent,
                                                    const wxString &burnName)
      : GmatPanel(parent)
{
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();

   theBurn = theGuiInterpreter->GetBurn(std::string(burnName.c_str()));

   Create();
   Show();
   theApplyButton->Disable();
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnFrameComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::OnFrameComboBoxChange(wxCommandEvent& event)
{
   LabelsUnits();
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnFormatComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::OnFormatComboBoxChange(wxCommandEvent& event)
{
   LabelsUnits();
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void AddVector(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * @param <parent> input parent.
 *
 * @note Creates the default objects to put in the vector static box
 */
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::AddVector(wxWindow *parent)
{
   vectorPanel = new wxPanel(parent);
   wxGridSizer *item0 = new wxGridSizer( 1, 0, 0 );

   wxFlexGridSizer *item3 = new wxFlexGridSizer( 6, 3, 0, 0 );
   item3->AddGrowableCol( 0 );
   item3->AddGrowableCol( 1 );
   item3->AddGrowableCol( 2 );

   description1 = new wxStaticText( vectorPanel, ID_TEXT, wxT("Descriptor1     "), 
                                    wxDefaultPosition, wxDefaultSize, 0 );
   item3->Add( description1, 0, wxALIGN_CENTER | wxALL, 5 );
   textCtrl1 = new wxTextCtrl( vectorPanel, ID_TEXTCTRL, wxT(""), 
                               wxDefaultPosition, wxSize(100,-1), 0 );
   item3->Add( textCtrl1, 0, wxALIGN_CENTER | wxALL, 5 );
   label1 = new wxStaticText( vectorPanel, ID_TEXT, wxT("Label1"), 
                              wxDefaultPosition, wxDefaultSize, 0 );
   item3->Add( label1, 0, wxALIGN_CENTER | wxALL, 5 );

   description2 = new wxStaticText( vectorPanel, ID_TEXT, wxT("Descriptor2    "), 
                                    wxDefaultPosition, wxDefaultSize, 0 );
   item3->Add( description2, 0, wxALIGN_CENTER | wxALL, 5 );
   textCtrl2 = new wxTextCtrl( vectorPanel, ID_TEXTCTRL, wxT(""), 
                               wxDefaultPosition, wxSize(100,-1), 0 );
   item3->Add( textCtrl2, 0, wxALIGN_CENTER|wxALL, 5 );
   label2 = new wxStaticText( vectorPanel, ID_TEXT, wxT("Label2"), 
                              wxDefaultPosition, wxDefaultSize, 0 );
   item3->Add( label2, 0, wxALIGN_CENTER | wxALL, 5 );
    
   description3 = new wxStaticText( vectorPanel, ID_TEXT, wxT("Descriptor3    "), 
                                    wxDefaultPosition, wxDefaultSize, 0 );
   item3->Add( description3, 0, wxALIGN_CENTER | wxALL, 5 );
   textCtrl3 = new wxTextCtrl( vectorPanel, ID_TEXTCTRL, wxT(""), 
                               wxDefaultPosition, wxSize(100,-1), 0 );
   item3->Add( textCtrl3, 0, wxALIGN_CENTER | wxALL, 5 );
   label3 = new wxStaticText( vectorPanel, ID_TEXT, wxT("Label3"), 
                              wxDefaultPosition, wxDefaultSize, 0 );
   item3->Add( label3, 0, wxALIGN_CENTER | wxALL, 5 );
 
   item0->Add( item3, 0, wxGROW|wxALL|wxALIGN_CENTER, 5 );

   vectorPanel->SetAutoLayout( TRUE );
   vectorPanel->SetSizer( item0 );

   item0->Fit( vectorPanel );
   item0->SetSizeHints( vectorPanel );
}

//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::OnTextChange(wxCommandEvent& event)
{
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void LabelsUnits()
//------------------------------------------------------------------------------
/**
 * @note Changes the vector descriptors and labels based on the state combo box
 */
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::LabelsUnits()
{
   Integer id;
   wxString coordStr, vectorStr, el;
   std::string element;
    
   // get the string of the combo box selections
   coordStr = frameCB->GetStringSelection();
   vectorStr = formatCB->GetStringSelection();

   // get the ID of the coordinate frame parameter 
   id = theBurn->GetParameterID("CoordinateFrame");

   // store the coordinate from for cancellation
   std::string coordFrame = theBurn->GetStringParameter(id);
    
   // check the combo box selection
   if (coordStr == "Inertial")
      theBurn->SetStringParameter(id, "Inertial");
   else if (coordStr == "VNB")
      theBurn->SetStringParameter(id, "VNB");
    
   // set label for element1 
   id = theBurn->GetParameterID("Element1Label");
   element = theBurn->GetStringParameter(id);
   el = wxString(element.c_str());
   description1->SetLabel(el);
   label1->SetLabel("km/s");
        
   // set label for element2
   id = theBurn->GetParameterID("Element2Label");
   element = theBurn->GetStringParameter(id);
   el = wxString(element.c_str());
   description2->SetLabel(el);
   label2->SetLabel("km/s");
       
   // set label for element3
   id = theBurn->GetParameterID("Element3Label");
   element = theBurn->GetStringParameter(id);
   el = wxString(element.c_str());
   description3->SetLabel(el);
   label3->SetLabel("km/s");

   // reset coordinate frame to original selection if cancel button selected
   id = theBurn->GetParameterID("CoordinateFrame");
   theBurn->SetStringParameter(id, coordFrame);

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
 * @note Creates the panel for the impulsive burn data
 */
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::Create()
{
   unsigned int i, count;
   StringArray items;

   if (theBurn != NULL)
   {
      // create sizers
      wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
      wxGridSizer *topGridSizer = new wxGridSizer(2, 0, 0);
      // static box for the vector
      wxStaticBox *vectorBox = new wxStaticBox(this, ID_STATIC_VECTOR, wxT("Vector"));
      wxStaticBoxSizer *vectorSizer = new wxStaticBoxSizer(vectorBox, wxVERTICAL);
      wxBoxSizer *bodyBoxSizer = new wxBoxSizer(wxHORIZONTAL);

      // label for coordinate frames combo box
      wxStaticText *frameLabel =
         new wxStaticText(this, ID_TEXT, wxT("Coordinate frame:"),
                          wxDefaultPosition, wxDefaultSize, 0);
        
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
         frameCB =
            new wxComboBox(this, ID_FRAME_COMBOBOX, wxT(""), 
                           wxDefaultPosition, wxSize(150,-1), count,
                           frameList, wxCB_DROPDOWN);
      }
      else                 // no coordinate frames exist
      { 
         wxString strs6[] =
         {
            wxT("No coordinate frame available") 
         };    
         frameCB =
            new wxComboBox(this, ID_FRAME_COMBOBOX, wxT(""),
                           wxDefaultPosition, wxSize(150,-1), 1,
                           strs6, wxCB_DROPDOWN);
      }
            
      wxStaticText *formatLabel =
         new wxStaticText(this, ID_TEXT, wxT("Vector format:"),
                          wxDefaultPosition, wxDefaultSize, 0);
       
      // combo box for Vector format
      wxString strs7[] =
      {
         wxT("Cartesian") 
         //wxT("Spherical") 
      };
      formatCB = new wxComboBox(this, ID_FORMAT_COMBOBOX, wxT(""),
                                wxDefaultPosition, wxSize(120,-1), 1,
                                strs7, wxCB_DROPDOWN);

      topGridSizer->Add(frameLabel, 0, wxALIGN_CENTER | wxALL, 5);
      topGridSizer->Add(formatLabel, 0, wxALIGN_CENTER | wxALL, 5);
      topGridSizer->Add(frameCB, 0, wxALIGN_CENTER | wxALL, 5);
      topGridSizer->Add(formatCB, 0, wxALIGN_CENTER | wxALL, 5);

      // panel that has the labels and text fields for the vectors
      // set it to null
      vectorPanel = (wxPanel *)NULL; 
        
      // adds default descriptors/labels 
      AddVector(this);
      vectorSizer->Add(vectorPanel, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);
        
      // create label and text field for the central body
      wxStaticText *item6 = new wxStaticText(this, ID_TEXT, wxT("Central body:"),
                                             wxDefaultPosition, wxDefaultSize, 0);
      wxStaticText *item7 = new wxStaticText(this, ID_TEXT, wxT("Earth"),
                                             wxDefaultPosition, wxDefaultSize, wxCAPTION);
      bodyBoxSizer->Add( item6, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5);
      bodyBoxSizer->Add( item7, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5);

      // add to page sizer
      pageBoxSizer->Add(topGridSizer, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5);
      pageBoxSizer->Add(vectorSizer, 0, wxGROW| wxALIGN_CENTER_VERTICAL| wxALL, 5);
      pageBoxSizer->Add(bodyBoxSizer, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5);

      // add to middle sizer
      theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);     

   }
   else
   {
      // show error message
   }
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::LoadData()
{
   // load data from the core engine
   Integer id, el;
   Real element;
    
   // Set object pointer for "Show Script"
   mObject = theBurn;

   // Coordinate Frame
   id = theBurn->GetParameterID("CoordinateFrame");
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

   // Vector Format
   id = theBurn->GetParameterID("VectorFormat");
   std::string vectorFormat = theBurn->GetStringParameter(id);

   if (vectorFormat == "Cartesian") 
      formatCB->SetSelection(0);
   else
      formatCB->SetSelection(1);

   // Element1
   el = theBurn->GetParameterID("Element1");
   element = theBurn->GetRealParameter(el);
   wxString el1;
   el1.Printf("%f", element);
   textCtrl1->SetValue(el1);
    
   // Element2
   el = theBurn->GetParameterID("Element2");
   element = theBurn->GetRealParameter(el);
   wxString el2;
   el2.Printf("%f", element);
   textCtrl2->SetValue(el2);

   // Element3
   el = theBurn->GetParameterID("Element3");
   element = theBurn->GetRealParameter(el);
   wxString el3;
   el3.Printf("%f", element);
   textCtrl3->SetValue(el3);

   LabelsUnits();
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::SaveData()
{
   // save data to core engine
   Integer id;
   wxString elemString;
    
   // save coordinate frame
   wxString frameString = frameCB->GetStringSelection();
   id = theBurn->GetParameterID("CoordinateFrame");
   std::string coordFrame = std::string (frameString.c_str());
   theBurn->SetStringParameter(id, coordFrame);
    
    // save vector format
   wxString vectorString = formatCB->GetStringSelection();
   id = theBurn->GetParameterID("VectorFormat");
   std::string vectorFormat = std::string (vectorString.c_str());
   theBurn->SetStringParameter(id, vectorFormat);

   // save element1
   elemString = textCtrl1->GetValue();
   id = theBurn->GetParameterID("Element1");
   theBurn->SetRealParameter(id, atof(elemString));

   // save element2
   elemString = textCtrl2->GetValue();
   id = theBurn->GetParameterID("Element2");
   theBurn->SetRealParameter(id, atof(elemString));

   // save element3
   elemString = textCtrl3->GetValue();
   id = theBurn->GetParameterID("Element3");
   theBurn->SetRealParameter(id, atof(elemString));

   theApplyButton->Disable();
}
