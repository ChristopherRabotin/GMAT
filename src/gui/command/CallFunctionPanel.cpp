//$Id$
//------------------------------------------------------------------------------
//                              CallFunctionPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Allison Greene
// Created: 2004/9/15
/**
 * This class contains the Matlab Command Setup window.
 */
//------------------------------------------------------------------------------

#include "CallFunctionPanel.hpp"
#include "MessageInterface.hpp"

#include "Function.hpp"
#include "Parameter.hpp"
#include "ParameterSelectDialog.hpp"

// To show input and output as Grid, otherwise input and output field
// is multiple line TextCtrl
//#define __USE_GRID_FOR_INPUT_OUTPUT__

//#define DEBUG_CALLFUNCTION_PANEL 1
//#define DEBUG_CALLFUNCTION_PANEL_LOAD 1
//#define DEBUG_CALLFUNCTION_PANEL_SAVE 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CallFunctionPanel, GmatPanel)
   EVT_COMBOBOX(ID_COMBO, CallFunctionPanel::OnComboChange)
   EVT_BUTTON(ID_BUTTON, CallFunctionPanel::OnButtonClick)
   EVT_GRID_CELL_RIGHT_CLICK(CallFunctionPanel::OnCellRightClick)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// CallFunctionPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
CallFunctionPanel::CallFunctionPanel( wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   theCommand = (CallFunction *)cmd;
   
   mObjectTypeList.Add("Spacecraft");
   mObjectTypeList.Add("ImpulsiveBurn");
   
   if (theCommand != NULL)
   {
      Create();
      Show();
      
      // If function name is blank, set update flag to ask for function
      // name when save
      if (theFunctionComboBox->GetValue() == "")
         EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// ~CallFunctionPanel()
//------------------------------------------------------------------------------
CallFunctionPanel::~CallFunctionPanel()
{
   mObjectTypeList.Clear();
   theGuiManager->UnregisterComboBox("Function", theFunctionComboBox);   
}

//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void CallFunctionPanel::Create()
{
   //MessageInterface::ShowMessage("CallFunctionPanel::Create() entered\n");
   
   int bsize = 3; // bordersize
   
   wxStaticText *outLeftBracket =
                     new wxStaticText( this, ID_TEXT, wxT("[  "),
                     wxDefaultPosition, wxDefaultSize, 0 );
   
   wxStaticText *outRightBracket =
                     new wxStaticText( this, ID_TEXT, wxT("  ]"),
                     wxDefaultPosition, wxDefaultSize, 0 );
   
   wxStaticText *inLeftBracket =
                     new wxStaticText( this, ID_TEXT, wxT("[  "),
                     wxDefaultPosition, wxDefaultSize, 0 );
   
   wxStaticText *inRightBracket =
                     new wxStaticText( this, ID_TEXT, wxT("  ]"),
                     wxDefaultPosition, wxDefaultSize, 0 );
   
   wxStaticText *equalSign =
                     new wxStaticText( this, ID_TEXT, wxT("  =  "),
                     wxDefaultPosition, wxDefaultSize, 0 );

   wxStaticText *outStaticText =
                     new wxStaticText( this, ID_TEXT, wxT("  Output  "),
                     wxDefaultPosition, wxDefaultSize, 0 );
   
   wxStaticText *inStaticText =
                     new wxStaticText( this, ID_TEXT, wxT("  Input  "),
                     wxDefaultPosition, wxDefaultSize, 0 );
   
   wxStaticText *functionStaticText =
                     new wxStaticText( this, ID_TEXT, wxT("  Function  "),
                     wxDefaultPosition, wxDefaultSize, 0 );
   
   //-----------------------------------------------------------------
   #ifdef __USE_GRID_FOR_INPUT_OUTPUT__
   //-----------------------------------------------------------------
   
   // Get available function ComboBox from theGuiManager
   theFunctionComboBox =
      theGuiManager->GetFunctionComboBox(this, ID_COMBO, wxSize(280, -1));
   
   theInputGrid =
      new wxGrid( this, -1, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS );
   
   theInputGrid->CreateGrid( 1, 1, wxGrid::wxGridSelectRows );
   theInputGrid->SetColSize(0, 290);
   theInputGrid->SetRowSize(0, 23);
   theInputGrid->SetColLabelSize(0);
   theInputGrid->SetRowLabelSize(0);
   theInputGrid->SetMargins(0, 0);
   theInputGrid->SetScrollbars(0, 0, 0, 0, 0, 0, FALSE);
   theInputGrid->EnableEditing(false);
   
   theOutputGrid =
      new wxGrid( this, -1, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS );
   
   theOutputGrid->CreateGrid( 1, 1, wxGrid::wxGridSelectRows );
   theOutputGrid->SetColSize(0, 290);
   theOutputGrid->SetRowSize(0, 23);
   theOutputGrid->SetColLabelSize(0);
   theOutputGrid->SetRowLabelSize(0);
   theOutputGrid->SetMargins(0, 0);
   theOutputGrid->SetScrollbars(0, 0, 0, 0, 0, 0, FALSE);
   theOutputGrid->EnableEditing(false);
   
   wxBoxSizer *outputSizer = new wxBoxSizer(wxHORIZONTAL);
   outputSizer->Add(outLeftBracket, 0, wxALIGN_CENTRE|wxALL, bsize);
   outputSizer->Add(theOutputGrid, 0, wxEXPAND|wxALIGN_CENTRE|wxALL, bsize);
   outputSizer->Add(outRightBracket, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   wxBoxSizer *inputSizer = new wxBoxSizer(wxHORIZONTAL);
   inputSizer->Add(inLeftBracket, 0, wxALIGN_CENTRE|wxALL, bsize);
   inputSizer->Add(theInputGrid, 0, wxEXPAND|wxALIGN_CENTRE|wxALL, bsize);
   inputSizer->Add(inRightBracket, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   wxBoxSizer *functionSizer = new wxBoxSizer(wxHORIZONTAL);
   functionSizer->Add(equalSign, 0, wxALIGN_CENTRE|wxALL, bsize);
   functionSizer->Add(theFunctionComboBox, 0, wxEXPAND|wxALIGN_CENTRE|wxALL, bsize);
   
   wxFlexGridSizer *pageSizer = new wxFlexGridSizer( 2, 0, 0 );
   pageSizer->Add(outputSizer, 1, wxEXPAND|wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(outStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(functionSizer, 1, wxEXPAND|wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(functionStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(inputSizer, 1, wxEXPAND|wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(inStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   theMiddleSizer->Add(pageSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
   //-----------------------------------------------------------------
   #else
   //-----------------------------------------------------------------
   
   // Get available function ComboBox from theGuiManager
   theFunctionComboBox =
      theGuiManager->GetFunctionComboBox(this, ID_COMBO, wxSize(300, -1));
   
   theInputTextCtrl =
      new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(50, -1),
                      wxTE_MULTILINE|wxTE_READONLY);
   
   theOutputTextCtrl =
      new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(50, -1),
                      wxTE_MULTILINE|wxTE_READONLY);
   
   theInputViewButton = new
      wxButton(this, ID_BUTTON, wxT("View"), wxDefaultPosition, wxDefaultSize, 0);
   
   theOutputViewButton = new
      wxButton(this, ID_BUTTON, wxT("View"), wxDefaultPosition, wxDefaultSize, 0);
   
   wxFlexGridSizer *pageSizer = new wxFlexGridSizer( 5, 0, 0 );
   pageSizer->Add(outLeftBracket, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(theOutputTextCtrl, 0, wxEXPAND|wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(outRightBracket, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(outStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(theOutputViewButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   pageSizer->Add(equalSign, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(theFunctionComboBox, 0, wxEXPAND|wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(5, 5);
   pageSizer->Add(functionStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(5, 5);
   
   pageSizer->Add(inLeftBracket, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(theInputTextCtrl, 0, wxEXPAND|wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(inRightBracket, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(inStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(theInputViewButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   theMiddleSizer->Add(pageSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
   //-----------------------------------------------------------------
   #endif
   //-----------------------------------------------------------------
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void CallFunctionPanel::LoadData()
{
   #ifdef DEBUG_CALLFUNCTION_PANEL_LOAD
   MessageInterface::ShowMessage("CallFunctionPanel::LoadData() entered\n");
   #endif
   
   // Set the pointer for the "Show Script" button
   mObject = theCommand;
   
   int id = theCommand->GetParameterID("FunctionName");
   std::string functionName = theCommand->GetStringParameter(id);
   
   // If function name is not in the ComboBox list, add blank
   if (!theFunctionComboBox->SetStringSelection(wxT(functionName.c_str())))
   {
      theFunctionComboBox->Append("");
      theFunctionComboBox->SetStringSelection("");
   }
   
   #ifdef DEBUG_CALLFUNCTION_PANEL_LOAD
   MessageInterface::ShowMessage
      ("   Function name is: <%s>\n", functionName.c_str());
   #endif
   
   // get input parameters
   StringArray inputList = theCommand->GetStringArrayParameter("AddInput");
   mNumInput = inputList.size();
   mInputWxStrings.Clear();
   
   #ifdef DEBUG_CALLFUNCTION_PANEL_LOAD
   MessageInterface::ShowMessage
      ("   Found %d input parameters:\n", mNumInput);
   for (StringArray::iterator i = inputList.begin(); i != inputList.end(); ++i)
      MessageInterface::ShowMessage("      '%s'\n", i->c_str());
   #endif
   
   if (mNumInput > 0)
   {
      wxString *inputNames = new wxString[mNumInput];
      GmatBase *param;
      wxString cellValue = "";
      wxString delimiter = ", ";
      
      for (int i=0; i<mNumInput; i++)
      {
         inputNames[i] = inputList[i].c_str();
         
         #ifdef DEBUG_CALLFUNCTION_PANEL_LOAD
         MessageInterface::ShowMessage("   Looking up " + inputList[i] + "\n");
         #endif
         
         param = theGuiInterpreter->GetConfiguredObject(inputList[i]);
         
         if (i == mNumInput-1)
            delimiter = "";
         
         if (param != NULL)
         {
            cellValue = cellValue + param->GetName().c_str() + delimiter;
            mInputWxStrings.Add(param->GetName().c_str());
         }
      }
      
      #ifdef __USE_GRID_FOR_INPUT_OUTPUT__
      theInputGrid->SetCellValue(0, 0, cellValue);
      #else
      theInputTextCtrl->SetValue(cellValue);
      #endif
      
      delete [] inputNames;
   }
   
   // get output parameters
   StringArray outputList = theCommand->GetStringArrayParameter("AddOutput");
   mNumOutput = outputList.size();
   mOutputWxStrings.Clear();
   
   #ifdef DEBUG_CALLFUNCTION_PANEL_LOAD
   MessageInterface::ShowMessage
      ("   Found %d output parameters:\n", mNumOutput);
   for (StringArray::iterator i = outputList.begin(); i != outputList.end(); ++i)
      MessageInterface::ShowMessage("      '%s'\n", i->c_str());
   #endif
   
   if (mNumOutput > 0)
   {
      wxString *outputNames = new wxString[mNumOutput];
      GmatBase *param;
      wxString cellValue = "";
      wxString delimiter = ", ";
      
      for (int i=0; i<mNumOutput; i++)
      {
         outputNames[i] = outputList[i].c_str();
         
         #ifdef DEBUG_CALLFUNCTION_PANEL_LOAD
         MessageInterface::ShowMessage("   Looking up " + outputList[i] + "\n");
         #endif
         
         param = theGuiInterpreter->GetConfiguredObject(outputList[i]);
         
         if (i == mNumOutput-1)
            delimiter = "";
         
         if (param != NULL)
         {
            cellValue = cellValue + param->GetName().c_str() + delimiter;
            mOutputWxStrings.Add(param->GetName().c_str());
         }
      }
      
      #ifdef __USE_GRID_FOR_INPUT_OUTPUT__
      theOutputGrid->SetCellValue(0, 0, cellValue);
      #else
      theOutputTextCtrl->SetValue(cellValue);
      #endif
      
      delete [] outputNames;
   }

}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void CallFunctionPanel::SaveData()
{
   wxString functionName = theFunctionComboBox->GetValue();
   
   #ifdef DEBUG_CALLFUNCTION_PANEL_SAVE
   MessageInterface::ShowMessage
      ("CallFunctionPanel::SaveData() functionName=%s\n", functionName.c_str());
   #endif
   
   canClose = true;
   
   //-----------------------------------------------------------------
   // check function name
   //-----------------------------------------------------------------
   if (functionName == "" || functionName.Find(" ") != wxNOT_FOUND)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, + "Please select function name.\n");
      canClose = false;
   }
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      theCommand->SetStringParameter("FunctionName", functionName.c_str());
      
      mNumInput = mInputWxStrings.Count();
      
      #ifdef DEBUG_CALLFUNCTION_PANEL_SAVE
      MessageInterface::ShowMessage("   Found %d input parameters:\n", mNumInput);
      #endif
      
      if (mNumInput >= 0) // >=0 because the list needs to be cleared
      {
         theCommand->TakeAction("ClearInput");
         for (int i=0; i<mNumInput; i++)
         {
            std::string selInName = std::string(mInputWxStrings[i]);
            theCommand->SetStringParameter("AddInput", selInName, i);
            
            #ifdef DEBUG_CALLFUNCTION_PANEL_SAVE
            MessageInterface::ShowMessage("      '%s'\n", selInName.c_str());
            #endif
         }
      }
      
      mNumOutput = mOutputWxStrings.Count();
      
      #ifdef DEBUG_CALLFUNCTION_PANEL_SAVE
      MessageInterface::ShowMessage("   Found %d output parameters:\n", mNumOutput);
      #endif
      
      if (mNumOutput >= 0) // >=0 because the list needs to be cleared
      {
         theCommand->TakeAction("ClearOutput");
         for (int i=0; i<mNumOutput; i++)
         {
            std::string selOutName = std::string(mOutputWxStrings[i]);
            theCommand->SetStringParameter("AddOutput", selOutName, i);
            
            #ifdef DEBUG_CALLFUNCTION_PANEL_SAVE
            MessageInterface::ShowMessage("      '%s'\n", selOutName.c_str());
            #endif
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
   
   #ifdef DEBUG_CALLFUNCTION_PANEL_SAVE
   MessageInterface::ShowMessage
      ("CallFunctionPanel::SaveData() theCommand=<%s>\n",
       theCommand->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif
}


//------------------------------------------------------------------------------
// void OnCellRightClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void CallFunctionPanel::OnCellRightClick(wxGridEvent& event)
{    
   #ifdef DEBUG_CALLFUNCTION_PANEL
   MessageInterface::ShowMessage
      ("CallFunctionPanel::OnCellRightClick() entered\n");
   #endif
   
   unsigned int row = event.GetRow();
   unsigned int col = event.GetCol();
   
   #ifdef DEBUG_CALLFUNCTION_PANEL
   MessageInterface::ShowMessage
      ("CallFunctionPanel::OnCellRightClick row = %d, col = %d\n", row, col);
   #endif
   
   if (event.GetEventObject() == theInputGrid)
   {
      ParameterSelectDialog paramDlg(this, mObjectTypeList, "Spacecraft",
                                     GuiItemManager::SHOW_REPORTABLE,
                                     true, true, true, true, true); 
      theOutputGrid->DeselectRow(0);
      paramDlg.SetParamNameArray(mInputWxStrings);
      paramDlg.ShowModal();
      
      mInputWxStrings = paramDlg.GetParamNameArray();
      wxString cellValue = "";
      wxString delimiter = ", ";
      
      if (mInputWxStrings.Count() > 0)
      {
         cellValue = cellValue + mInputWxStrings[0];
         
         for (unsigned int i=1; i<mInputWxStrings.Count(); i++)
            cellValue = cellValue + delimiter + mInputWxStrings[i];
         
         theInputGrid->SetCellValue(row, col, cellValue);
         EnableUpdate(true);
      }
      else     // no selections
         theInputGrid->SetCellValue(row, col, "");
   }
   else if (event.GetEventObject() == theOutputGrid)
   {
      ParameterSelectDialog paramDlg(this, mObjectTypeList, "Spacecraft",
                                     GuiItemManager::SHOW_REPORTABLE,
                                     true, true, false, true);
      theInputGrid->DeselectRow(0);
      paramDlg.SetParamNameArray(mOutputWxStrings);
      paramDlg.ShowModal();
      
      mOutputWxStrings = paramDlg.GetParamNameArray();
      wxString cellValue = "";
      wxString delimiter = ", ";
      
      if (mOutputWxStrings.Count() > 0)
      {
         cellValue = cellValue + mOutputWxStrings[0];

         for (unsigned int i=1; i<mOutputWxStrings.Count(); i++)
            cellValue = cellValue + delimiter + mOutputWxStrings[i];
         
         theOutputGrid->SetCellValue(row, col, cellValue);
         EnableUpdate(true);
      }
      else     // no selections
         theOutputGrid->SetCellValue(row, col, "");
   }
}


//------------------------------------------------------------------------------
// void OnComboChange()
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void CallFunctionPanel::OnComboChange(wxCommandEvent &event)
{
    EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CallFunctionPanel::OnButtonClick(wxCommandEvent& event)
{
   wxString value = "";
   wxString delimiter = ", ";
   
   if (event.GetEventObject() == theInputViewButton)
   {
      ParameterSelectDialog paramDlg(this, mObjectTypeList, "Spacecraft",
                                     GuiItemManager::SHOW_REPORTABLE,
                                     true, true, true, true, true);
      
      paramDlg.SetParamNameArray(mInputWxStrings);
      paramDlg.ShowModal();
      
      mInputWxStrings = paramDlg.GetParamNameArray();
      
      if (mInputWxStrings.Count() > 0)
      {
         value = value + mInputWxStrings[0];
         
         for (unsigned int i=1; i<mInputWxStrings.Count(); i++)
            value = value + delimiter + mInputWxStrings[i];
         
         theInputTextCtrl->SetValue(value);
         EnableUpdate(true);
      }
      else     // no selections
      {
         theInputTextCtrl->SetValue(value);
         EnableUpdate(true);
      }
   }
   else if (event.GetEventObject() == theOutputViewButton)
   {
      ParameterSelectDialog paramDlg(this, mObjectTypeList, "Spacecraft",
                                     GuiItemManager::SHOW_REPORTABLE,
                                     true, true, true, true, true);
      
      paramDlg.SetParamNameArray(mOutputWxStrings);
      paramDlg.ShowModal();
      
      mOutputWxStrings = paramDlg.GetParamNameArray();
      
      if (mOutputWxStrings.Count() > 0)
      {
         value = value + mOutputWxStrings[0];
         
         for (unsigned int i=1; i<mOutputWxStrings.Count(); i++)
            value = value + delimiter + mOutputWxStrings[i];
         
         theOutputTextCtrl->SetValue(value);
         EnableUpdate(true);
      }
      else     // no selections
      {
         theOutputTextCtrl->SetValue(value);
         EnableUpdate(true);
      }
   }
}
