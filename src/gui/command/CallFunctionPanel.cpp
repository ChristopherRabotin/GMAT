//$Header$
//------------------------------------------------------------------------------
//                              CallFunctionPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
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

//#define DEBUG_CALLFUNCTION_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CallFunctionPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)

   EVT_COMBOBOX(ID_COMBO, CallFunctionPanel::OnComboChange)
   EVT_GRID_CELL_LEFT_CLICK(CallFunctionPanel::OnCellClick)
   EVT_GRID_CELL_RIGHT_CLICK(CallFunctionPanel::OnCellClick)
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
//   theCommand = cmd;

   mObjectTypeList.Add("Spacecraft");
   
   if (theCommand != NULL)
   {
      Create();
      Show();
      theApplyButton->Disable();
   }
}

//------------------------------------------------------------------------------
// ~CallFunctionPanel()
//------------------------------------------------------------------------------
CallFunctionPanel::~CallFunctionPanel()
{
   mObjectTypeList.Clear();
   theGuiManager->UnregisterComboBox("Function", functionComboBox);   
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
   int bsize = 5; // bordersize
   
   wxFlexGridSizer *mflexGridSizer = new wxFlexGridSizer( 2, 0, 0 );
   wxBoxSizer *horizontalSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *outputSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *inputSizer = new wxBoxSizer(wxHORIZONTAL);

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

//   inputTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
//                                   wxDefaultPosition, wxSize(290,-1), wxTE_READONLY );
//   outputTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
//                                   wxDefaultPosition, wxSize(290,-1), wxTE_READONLY );

   // wxStaticText
//   nameStaticText = new wxStaticText( this, ID_TEXT, wxT("Function Name:  "),
//                     wxDefaultPosition, wxDefaultSize, 0 );

   StringArray &list =
         theGuiInterpreter->GetListOfConfiguredItems(Gmat::FUNCTION);
   int size = list.size();
   wxString *choices = new wxString[size];

   for (int i=0; i<size; i++)
   {
        choices[i] = list[i].c_str();
   }

    // combo box for the date type
//    functionComboBox = new wxComboBox( this, ID_COMBO, wxT(""),
//              wxDefaultPosition, wxSize(130,-1), size, choices,
//              wxCB_DROPDOWN | wxCB_READONLY );

   //loj: 6/6/05 get it from theGuiManager
   functionComboBox = theGuiManager->GetFunctionComboBox(this, ID_COMBO, 
                                                         wxDefaultSize);

   // wxGrid
   inputGrid =
      new wxGrid( this, -1, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS );

   inputGrid->CreateGrid( 1, 1, wxGrid::wxGridSelectRows );
   inputGrid->SetColSize(0, 290);
   inputGrid->SetRowSize(0, 23);
   inputGrid->SetColLabelSize(0);
   inputGrid->SetRowLabelSize(0);
   inputGrid->SetMargins(0, 0);
   inputGrid->SetScrollbars(0, 0, 0, 0, 0, 0, FALSE);
   inputGrid->EnableEditing(false);

   outputGrid =
      new wxGrid( this, -1, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS );

   outputGrid->CreateGrid( 1, 1, wxGrid::wxGridSelectRows );
   outputGrid->SetColSize(0, 290);
   outputGrid->SetRowSize(0, 23);
   outputGrid->SetColLabelSize(0);
   outputGrid->SetRowLabelSize(0);
   outputGrid->SetMargins(0, 0);
   outputGrid->SetScrollbars(0, 0, 0, 0, 0, 0, FALSE);
   outputGrid->EnableEditing(false);

   outputSizer->Add(outLeftBracket, 0, wxALIGN_CENTRE|wxALL, bsize);
   outputSizer->Add(outputGrid, 0, wxALIGN_CENTRE|wxALL, bsize);
   outputSizer->Add(outRightBracket, 0, wxALIGN_CENTRE|wxALL, bsize);

   inputSizer->Add(inLeftBracket, 0, wxALIGN_CENTRE|wxALL, bsize);
   inputSizer->Add(inputGrid, 0, wxALIGN_CENTRE|wxALL, bsize);
   inputSizer->Add(inRightBracket, 0, wxALIGN_CENTRE|wxALL, bsize);

//   horizontalSizer->Add(nameStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   horizontalSizer->Add(equalSign, 0, wxALIGN_CENTRE|wxALL, bsize);
   horizontalSizer->Add(functionComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);

   mflexGridSizer->Add(outputSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mflexGridSizer->Add(outStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   mflexGridSizer->Add(horizontalSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mflexGridSizer->Add(functionStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   mflexGridSizer->Add(inputSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mflexGridSizer->Add(inStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);

//   theMiddleSizer->Add(outputSizer, 0, wxALIGN_CENTER|wxALL, bsize);
//   theMiddleSizer->Add(horizontalSizer, 0, wxALIGN_CENTER|wxALL, bsize);
//   theMiddleSizer->Add(mHorizontalSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
//   theMiddleSizer->Add(inputSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   theMiddleSizer->Add(mflexGridSizer, 0, wxALIGN_CENTER|wxALL, bsize);

}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void CallFunctionPanel::LoadData()
{
   #ifdef DEBUG_CALLFUNCTION_PANEL
      MessageInterface::ShowMessage("Loading data...\n");
   #endif

   // Set the pointer for the "Show Script" button
   mObject = theCommand;

   int id = theCommand->GetParameterID("FunctionName");
   std::string filename = theCommand->GetStringParameter(id);
   functionComboBox->SetValue(wxT(filename.c_str()));

   #ifdef DEBUG_CALLFUNCTION_PANEL
      MessageInterface::ShowMessage("   Function name is: %s\n",
         filename.c_str());
   #endif

   // get input parameters
   StringArray inputList = theCommand->GetStringArrayParameter("AddInput");
   mNumInput = inputList.size();
   inputStrings.Clear();

   #ifdef DEBUG_CALLFUNCTION_PANEL
      MessageInterface::ShowMessage("   Found %d input parameters:\n",
         mNumInput);
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
         #ifdef DEBUG_CALLFUNCTION_PANEL
            MessageInterface::ShowMessage("   Looking up " + inputList[i] + "\n");
         #endif
         param = theGuiInterpreter->GetConfiguredItem(inputList[i]);

         if (i == mNumInput-1)
            delimiter = "";

         cellValue = cellValue + param->GetName().c_str() + delimiter;
         inputStrings.Add(param->GetName().c_str());
      }

      inputGrid->SetCellValue(0, 0, cellValue);
      delete [] inputNames;
   }

   // get output parameters
   StringArray outputList = theCommand->GetStringArrayParameter("AddOutput");
   mNumOutput = outputList.size();
   outputStrings.Clear();

   #ifdef DEBUG_CALLFUNCTION_PANEL
      MessageInterface::ShowMessage("   Found %d output parameters:\n",
         mNumOutput);
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
         #ifdef DEBUG_CALLFUNCTION_PANEL
            MessageInterface::ShowMessage("   Looking up " + outputList[i] + "\n");
         #endif
         param = theGuiInterpreter->GetConfiguredItem(outputList[i]);

         if (i == mNumOutput-1)
            delimiter = "";

         cellValue = cellValue + param->GetName().c_str() + delimiter;
         outputStrings.Add(param->GetName().c_str());
      }

      outputGrid->SetCellValue(0, 0, cellValue);
      delete [] outputNames;
   }

//   int numInput = theCommand->GetNumInputParams();
//   int numOutput = theCommand->GetNumOutputParams();

   // get input params
//   if (numInput > 0)
//   {
//      // for now - needs to be done in the sandbox
//      Parameter *param = (Parameter *)theCommand->GetRefObject(Gmat::PARAMETER,
//                           "Input", 0);
//      inputStrings.Add(param->GetName().c_str());
//      cellValue = cellValue + param->GetName().c_str();
//
//      for (int i=1; i<numInput; i++)
//      {
//         param = (Parameter *)theCommand->GetRefObject(Gmat::PARAMETER,
//                           "Input", i);
//         cellValue = cellValue + ", " + param->GetName().c_str();
//         inputStrings.Add(param->GetName().c_str());
//      }
//
//      inputGrid->SetCellValue(0, 0, cellValue);
//   }
//   else
//      inputGrid->SetCellValue(0, 0, "");
//
//   // reset the cell value
//   cellValue = "";
//
//   // get output params
//   if (numOutput > 0)
//   {
//      Parameter *param = (Parameter *)theCommand->GetRefObject(Gmat::PARAMETER,
//                           "Output", 0);
//      outputStrings.Add(param->GetName().c_str());
//      cellValue = cellValue + param->GetName().c_str();
//
//      for (int i=1; i<numOutput; i++)
//      {
//         param = (Parameter *)theCommand->GetRefObject(Gmat::PARAMETER,
//                           "Output", i);
//         cellValue = cellValue + ", " + param->GetName().c_str();
//         outputStrings.Add(param->GetName().c_str());
//      }
//
//      outputGrid->SetCellValue(0, 0, cellValue);
//   }
//   else
//      outputGrid->SetCellValue(0, 0, "");
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void CallFunctionPanel::SaveData()
{
//   MessageInterface::ShowMessage("Saving data...\n");
   wxString functionName = functionComboBox->GetStringSelection();

   // arg: for now to avoid a crash
   if (functionName != "")
   {
      theCommand->SetStringParameter("FunctionName",
                     std::string(functionName.c_str()));
   }


    mNumInput = inputStrings.Count();

    if (mNumInput >= 0) // >=0 because the list needs to be cleared
    {
         theCommand->TakeAction("ClearInput");
         for (int i=0; i<mNumInput; i++)
         {
            std::string selInName = std::string(inputStrings[i]);
            theCommand->SetStringParameter("AddInput", selInName, i);
         }
    }

    mNumOutput = outputStrings.Count();

    if (mNumOutput >= 0) // >=0 because the list needs to be cleared
    {
         theCommand->TakeAction("ClearOutput");
         for (int i=0; i<mNumOutput; i++)
         {
            std::string selOutName = std::string(outputStrings[i]);
            theCommand->SetStringParameter("AddOutput", selOutName, i);
         }
      }

   // clear out previous parameters
//   theCommand->ClearObject(Gmat::PARAMETER);
//
//   // set input parameters
//   for (unsigned int i=0; i<inputStrings.Count(); i++)
//   {
//      Parameter *parameter = (Parameter *)theGuiInterpreter->GetConfiguredItem(
//            std::string(inputStrings[i]));
//      theCommand->SetRefObject(parameter, Gmat::PARAMETER, "Input", i);
//   }
//
//   // set output parameters
//   for (unsigned int i=0; i<outputStrings.Count(); i++)
//   {
//      Parameter *parameter = (Parameter *)theGuiInterpreter->GetConfiguredItem(
//            std::string(outputStrings[i]));
//      theCommand->SetRefObject(parameter, Gmat::PARAMETER, "Output", i);
//   }
//
}

//------------------------------------------------------------------------------
// void OnCellClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void CallFunctionPanel::OnCellClick(wxGridEvent& event)
{
   unsigned int row = event.GetRow();
   unsigned int col = event.GetCol();

   if (event.GetEventObject() == inputGrid)
   {
      //loj: 5/2/06 added "Spacecraft"
      ParameterSelectDialog paramDlg(this, mObjectTypeList, "Spacecraft",
                                     GuiItemManager::SHOW_REPORTABLE,
                                     true, true, true, true, true); 
      paramDlg.SetParamNameArray(inputStrings);
      paramDlg.ShowModal();
      
      inputStrings = paramDlg.GetParamNameArray();
      wxString cellValue = "";
      wxString delimiter = ", ";

      if (inputStrings.Count() > 0)
      {
         cellValue = cellValue + inputStrings[0];

         for (unsigned int i=1; i<inputStrings.Count(); i++)
         {
            cellValue = cellValue + delimiter + inputStrings[i];
         }

         inputGrid->SetCellValue(row, col, cellValue);
         theApplyButton->Enable();
      }
      else     // no selections
         inputGrid->SetCellValue(row, col, "");

      //theApplyButton->Enable();
   }
   else if (event.GetEventObject() == outputGrid)
   {
      //loj: 5/2/06 added "Spacecraft"
      ParameterSelectDialog paramDlg(this, mObjectTypeList, "Spacecraft",
                                     GuiItemManager::SHOW_REPORTABLE,
                                     true, true, false, true);
      paramDlg.SetParamNameArray(outputStrings);
      paramDlg.ShowModal();
      
      outputStrings = paramDlg.GetParamNameArray();
      wxString cellValue = "";
      wxString delimiter = ", ";

      if (outputStrings.Count() > 0)
      {
         cellValue = cellValue + outputStrings[0];

         for (unsigned int i=1; i<outputStrings.Count(); i++)
         {
            cellValue = cellValue + delimiter + outputStrings[i];
         }

         outputGrid->SetCellValue(row, col, cellValue);
         theApplyButton->Enable();
      }
      else     // no selections
         outputGrid->SetCellValue(row, col, "");

      //theApplyButton->Enable();
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
    theApplyButton->Enable();
}
