//$Header$
//------------------------------------------------------------------------------
//                              ImpulsiveBurnSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: LaMont Ruley
// Created: 2004/02/04
//
// 2004/12/16 - Added central body combo box selection
//
/**
 * This class contains the Impulsive Burn Setup window.
 */
//------------------------------------------------------------------------------

#include "ImpulsiveBurnSetupPanel.hpp"
#include "StringUtil.hpp"  // for ToDouble()

//#define DEBUG_IMPBURN_SAVE 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ImpulsiveBurnSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_COMBOBOX(ID_COMBOBOX, ImpulsiveBurnSetupPanel::OnComboBoxChange)
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
 */
//------------------------------------------------------------------------------
ImpulsiveBurnSetupPanel::ImpulsiveBurnSetupPanel(wxWindow *parent,
                                                 const wxString &burnName)
   : GmatPanel(parent)
{
   theBurn = (Burn*)theGuiInterpreter->GetConfiguredObject(std::string(burnName.c_str()));
   isTextModified = false;
   canClose = true;
   
   Create();
   Show();

   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// ~ImpulsiveBurnSetupPanel()
//------------------------------------------------------------------------------
ImpulsiveBurnSetupPanel::~ImpulsiveBurnSetupPanel()
{
   theGuiManager->UnregisterComboBox("SpacePoint", centralBodyCB);
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::OnComboBoxChange(wxCommandEvent& event)
{
   if ( (event.GetEventObject() == axesComboBox) || 
        (event.GetEventObject() == vectorFormatComboBox) )
   {
      LabelsUnits();
   }   

   EnableUpdate(true);
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

   description1 = new wxStaticText(vectorPanel, ID_TEXT, wxT("Descriptor1    "), 
                                    wxDefaultPosition, wxDefaultSize, 0 );
   item3->Add( description1, 0, wxALIGN_CENTER | wxALL, 5 );
   textCtrl1 = new wxTextCtrl( vectorPanel, ID_TEXTCTRL, wxT(""), 
                               wxDefaultPosition, wxSize(150,-1), 0 );
   item3->Add( textCtrl1, 0, wxALIGN_CENTER | wxALL, 5 );
   label1 = new wxStaticText( vectorPanel, ID_TEXT, wxT("Label1"), 
                              wxDefaultPosition, wxDefaultSize, 0 );
   item3->Add( label1, 0, wxALIGN_CENTER | wxALL, 5 );

   description2 = new wxStaticText(vectorPanel, ID_TEXT, wxT("Descriptor2    "), 
                                    wxDefaultPosition, wxDefaultSize, 0 );
   item3->Add( description2, 0, wxALIGN_CENTER | wxALL, 5 );
   textCtrl2 = new wxTextCtrl( vectorPanel, ID_TEXTCTRL, wxT(""), 
                               wxDefaultPosition, wxSize(150,-1), 0 );
   item3->Add( textCtrl2, 0, wxALIGN_CENTER|wxALL, 5 );
   label2 = new wxStaticText( vectorPanel, ID_TEXT, wxT("Label2"), 
                              wxDefaultPosition, wxDefaultSize, 0 );
   item3->Add( label2, 0, wxALIGN_CENTER | wxALL, 5 );
    
   description3 = new wxStaticText(vectorPanel, ID_TEXT, wxT("Descriptor3    "), 
                                    wxDefaultPosition, wxDefaultSize, 0 );
   item3->Add( description3, 0, wxALIGN_CENTER | wxALL, 5 );
   textCtrl3 = new wxTextCtrl( vectorPanel, ID_TEXTCTRL, wxT(""), 
                               wxDefaultPosition, wxSize(150,-1), 0 );
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
   if (textCtrl1->IsModified() || textCtrl2->IsModified() ||
       textCtrl3->IsModified())
   {
      isTextModified = true;
      EnableUpdate(true);
   }
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
   coordStr = axesComboBox->GetStringSelection();
   vectorStr = vectorFormatComboBox->GetStringSelection();

   // get the ID of the coordinate frame parameter 
   id = theBurn->GetParameterID("Axes");

   // store the coordinate from for cancellation
   std::string axesStr = theBurn->GetStringParameter(id);
    
   // check the combo box selection
//   if ((coordStr == "Inertial") || (coordStr == "MJ2000Eq"))
//      theBurn->SetStringParameter(id, "MJ2000Eq");
//   else if (coordStr == "VNB")
//      theBurn->SetStringParameter(id, "VNB");

   // ALL validation is in base code, so we pass the string there 
   // and let that code throw if there is a problem.
   std::string framestring = coordStr.c_str();
   theBurn->SetStringParameter(id, framestring);
    
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
   id = theBurn->GetParameterID("Axes");
   theBurn->SetStringParameter(id, axesStr);

   EnableUpdate(true);
   //theApplyButton->Enable();
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
   Integer id, bsize;
   
   if (theBurn != NULL)
   {
      bsize = 2; // border size

      // create sizers
      wxBoxSizer *pageBoxSizer = new wxBoxSizer( wxVERTICAL );
      wxGridSizer *topGridSizer = new wxGridSizer( 2, 5, 5 );

      // Origin
      wxStaticText *centralBodyLabel = 
         new wxStaticText(this, ID_TEXT, wxT("Origin"), wxDefaultPosition,
            wxDefaultSize, 0);
      centralBodyCB = theGuiManager->GetSpacePointComboBox(this, ID_COMBOBOX,
         wxSize(150,-1), false);

      // Axes
      wxStaticText *axesLabel =
         new wxStaticText(this, ID_TEXT, wxT("Axes"),
                          wxDefaultPosition, wxDefaultSize, 0);

      id    = theBurn->GetParameterID("Axes");
      items = theBurn->GetStringArrayParameter(id);
      count = items.size();
      wxString *axesList = new wxString[count];
       
      if (count > 0)        // check to see if any axes exist
      {
         for (i=0; i<count; i++)
            axesList[i] = wxString(items[i].c_str());

         // combo box for avaliable coordinate frames 
         axesComboBox =
            new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition, 
               wxSize(150,-1), count, axesList, wxCB_DROPDOWN|wxCB_READONLY);
      }
      else                 // no coordinate axes exist
      { 
         wxString strs6[] =
         {
            wxT("No axes available") 
         };    
         axesComboBox =
            new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition, 
               wxSize(150,-1), 1, strs6, wxCB_DROPDOWN|wxCB_READONLY);
      }
            
      // Vector format
      wxStaticText *formatLabel =
         new wxStaticText(this, ID_TEXT, wxT("Vector format"),
                          wxDefaultPosition, wxDefaultSize, 0);

      wxString strs7[] =
      {
         wxT("Cartesian") 
         //wxT("Spherical") 
      };
      vectorFormatComboBox = 
         new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition, 
            wxSize(150,-1), 1, strs7, wxCB_DROPDOWN|wxCB_READONLY);

      // add to top grid sizer
    topGridSizer->Add(centralBodyLabel,
                   wxSizerFlags().Align(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL));
    topGridSizer->Add(centralBodyCB,
                   wxSizerFlags(1).Align(wxGROW | wxALIGN_CENTER_VERTICAL));
    topGridSizer->Add(axesLabel,
                   wxSizerFlags().Align(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL));
    topGridSizer->Add(axesComboBox,
                   wxSizerFlags(1).Align(wxGROW | wxALIGN_CENTER_VERTICAL));
    topGridSizer->Add(formatLabel,
                   wxSizerFlags().Align(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL));
    topGridSizer->Add(vectorFormatComboBox,
                   wxSizerFlags(1).Align(wxGROW | wxALIGN_CENTER_VERTICAL));
//      topGridSizer->Add(centralBodyLabel, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
//      topGridSizer->Add(centralBodyCB, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
//      topGridSizer->Add(axesLabel, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
//      topGridSizer->Add(axesComboBox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
//      topGridSizer->Add(formatLabel, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
//      topGridSizer->Add(vectorFormatComboBox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);

      // static box for the vector
      wxStaticBox *vectorBox = 
         new wxStaticBox(this, ID_STATIC_VECTOR, wxT("Vector"));
      wxStaticBoxSizer *vectorSizer = 
         new wxStaticBoxSizer(vectorBox, wxVERTICAL);

      // panel that has the labels and text fields for the vectors
      // set it to null
      vectorPanel = (wxPanel *)NULL; 
        
      // adds default descriptors/labels 
      AddVector(this);
    vectorSizer->Add(vectorPanel,
                   wxSizerFlags().Align(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL));
//      vectorSizer->Add(vectorPanel, 0, wxGROW | wxALIGN_CENTER | wxALL, bsize);
        
      // add to page sizer
    pageBoxSizer->Add(topGridSizer, wxSizerFlags(1).Expand().Border(wxALL, 10));
  pageBoxSizer->Add(vectorSizer, wxSizerFlags().Border(wxALL, 7));
//      pageBoxSizer->
//         Add(topGridSizer, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, bsize);
//      pageBoxSizer->
//         Add(vectorSizer, 0, wxGROW| wxALIGN_CENTER_VERTICAL| wxALL, bsize);

      // add to middle sizer
      theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);     
//  this->SetSizer( pageBoxSizer );
  
  // don't allow frame to get smaller than what the sizers tell it and also set
  // the initial size as calculated by the sizers
  pageBoxSizer->SetSizeHints( this );

   }
   else
   {
      // show error message
      MessageInterface::ShowMessage("ImpulsiveBurnSetupPanel:Create() theBurn is NULL\n");
   }
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::LoadData()
{
   // load data from the core engine
   try
   {
      Integer id, el;
      Real element;
    
      // Set object pointer for "Show Script"
      mObject = theBurn;
      
      // Axes
      id = theBurn->GetParameterID("Axes");
      std::string axesStr = theBurn->GetStringParameter(id);
      StringArray frames = theBurn->GetStringArrayParameter(id);
      int index = 0;
      for (StringArray::iterator iter = frames.begin(); 
        iter != frames.end(); ++iter) 
      {
         if (axesStr == *iter) 
            axesComboBox->SetSelection(index);
         else
            ++index;
      }
      
      // Vector Format
      id = theBurn->GetParameterID("VectorFormat");
      std::string vectorFormat = theBurn->GetStringParameter(id);

      if (vectorFormat == "Cartesian") 
         vectorFormatComboBox->SetSelection(0);
      else
         vectorFormatComboBox->SetSelection(1);
      
      // Element1
      el = theBurn->GetParameterID("Element1");
      element = theBurn->GetRealParameter(el);
      textCtrl1->SetValue(theGuiManager->ToWxString(element));
      
      // Element2
      el = theBurn->GetParameterID("Element2");
      element = theBurn->GetRealParameter(el);
      textCtrl2->SetValue(theGuiManager->ToWxString(element));
      
      // Element3
      el = theBurn->GetParameterID("Element3");
      element = theBurn->GetRealParameter(el);
      textCtrl3->SetValue(theGuiManager->ToWxString(element));
      
      LabelsUnits();
      
      // load central body
      int burnOriginID = theBurn->GetParameterID("Origin");
      std::string burnOriginName = theBurn->GetStringParameter(burnOriginID);
      centralBodyCB->SetValue(burnOriginName.c_str());
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::SaveData()
{
   #if DEBUG_IMPBURN_SAVE
   MessageInterface::ShowMessage
      ("ImpulsiveBurnSetupPanel::SaveData() isTextModified=%d\n", isTextModified);
   #endif

   canClose = true;
   Real elem1, elem2, elem3;
   std::string str;
   
   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   if (isTextModified)
   {
      str = textCtrl1->GetValue();      
      CheckReal(elem1, str, "Element1", "Real Number");
      
      str = textCtrl2->GetValue();      
      CheckReal(elem2, str, "Element2", "Real Number");
      
      str = textCtrl3->GetValue();      
      CheckReal(elem3, str, "Element3", "Real Number");
   }
   
   if (!canClose)
      return;
   
   #if DEBUG_IMPBURN_SAVE
   MessageInterface::ShowMessage("   Saving to base code\n");
   #endif
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      // save data to core engine
      Integer id;
      
      // save axes
      wxString axesStr = axesComboBox->GetStringSelection();
      id = theBurn->GetParameterID("Axes");
      std::string axes = std::string (axesStr.c_str());
      theBurn->SetStringParameter(id, axes);
      
      // save vector format
      wxString vectorStr = vectorFormatComboBox->GetStringSelection();
      id = theBurn->GetParameterID("VectorFormat");
      std::string vectorFormat = std::string (vectorStr.c_str());
      theBurn->SetStringParameter(id, vectorFormat);
      
      // save central body
      wxString burnOriginStr = centralBodyCB->GetStringSelection();
      id = theBurn->GetParameterID("Origin");
      std::string origin = std::string (burnOriginStr.c_str());
      theBurn->SetStringParameter(id, origin);
      
      // save elements
      if (isTextModified)
      {
         id = theBurn->GetParameterID("Element1");
         theBurn->SetRealParameter(id, elem1);
         
         id = theBurn->GetParameterID("Element2");
         theBurn->SetRealParameter(id, elem2);
         
         id = theBurn->GetParameterID("Element3");
         theBurn->SetRealParameter(id, elem3);
         
         isTextModified = false;
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
}
