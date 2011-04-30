//$Id$
//------------------------------------------------------------------------------
//                              ThrusterCoefficientDialog
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Waka Waktola
// Created: 2005/01/13
//
/**
 * Implements ThrusterCoefficientDialog class. This class shows dialog window where
 * thruster coefficients can be modified.
 * 
 */
//------------------------------------------------------------------------------

#include "ThrusterCoefficientDialog.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"
#include <wx/variant.h>

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(ThrusterCoefficientDialog, GmatDialog)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ThrusterCoefficientDialog(wxWindow *parent, wxWindowID id, 
//                           const wxString &title, GmatBase *obj,
//                           const std::string &type)
//------------------------------------------------------------------------------
ThrusterCoefficientDialog::
ThrusterCoefficientDialog(wxWindow *parent, wxWindowID id, 
                          const wxString &title, GmatBase *obj,
                          const wxString &type)
   : GmatDialog(parent, id, title, obj, wxDefaultPosition, wxDefaultSize)
{
   coefType = type;
   theObject = obj;
   
   coefNames.clear();
   coefValues.clear();
   
   if (obj != NULL)
   {
      Create();
      ShowData();
   }
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void ThrusterCoefficientDialog::Create()
{
   coefGrid =
      new wxGrid( this, ID_GRID, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS);
   
   coefCount = Thruster::COEFFICIENT_COUNT;
   
   coefGrid->EnableDragGridSize(false);
   coefGrid->EnableDragColSize(false);
   coefGrid->CreateGrid(coefCount, 3);
   coefGrid->SetRowLabelSize(0);
   coefGrid->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_CENTRE);
   
   coefGrid->SetColLabelValue(0, _T("Coefficient"));
   coefGrid->SetColSize(0, 70);
   coefGrid->SetColLabelValue(1, _T("Value"));
   coefGrid->SetColSize(1, 135);
   coefGrid->SetColLabelValue(2, _T("Unit"));
   coefGrid->SetColSize(2, 80);
   
   // The first and third columns are read only
   for (int i=0; i<coefCount; i++)
   {
      coefGrid->SetReadOnly(i, 0);
      coefGrid->SetReadOnly(i, 2);
   }
   
   // wxSizers   
   theMiddleSizer->Add(coefGrid, 0, wxALIGN_CENTRE|wxGROW|wxALL, 3);
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ThrusterCoefficientDialog::LoadData()
{
   Integer paramID = 0;
   int coefCount = Thruster::COEFFICIENT_COUNT;
   
   if (coefType == "C")
   {
      paramID = theObject->GetParameterID("C1");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("C1");
      
      paramID = theObject->GetParameterID("C2");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("C2");
      
      paramID = theObject->GetParameterID("C3");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("C3");
      
      paramID = theObject->GetParameterID("C4");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("C4");
      
      paramID = theObject->GetParameterID("C5");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("C5");
      
      paramID = theObject->GetParameterID("C6");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("C6");
      
      paramID = theObject->GetParameterID("C7");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("C7");
      
      paramID = theObject->GetParameterID("C8");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("C8");
      
      paramID = theObject->GetParameterID("C9");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("C9");
      
      paramID = theObject->GetParameterID("C10");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("C10");
      
      paramID = theObject->GetParameterID("C11");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("C11");
      
      paramID = theObject->GetParameterID("C12");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("C12");
      
      paramID = theObject->GetParameterID("C13");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("C13");
      
      paramID = theObject->GetParameterID("C14");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("C14");
   
      paramID = theObject->GetParameterID("C15");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("C15");
      
      paramID = theObject->GetParameterID("C16");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("C16");

      paramID = theObject->GetParameterID("C_UNITS");
      StringArray coefUnits = theObject->GetStringArrayParameter(paramID);
      
      for (Integer i = 0; i < coefCount; i++)
      {
         coefGrid->SetCellValue(i, 0, coefNames[i].c_str());
         coefGrid->SetCellValue(i, 1, wxVariant(coefValues[i]));
         coefGrid->SetCellValue(i, 2, coefUnits[i].c_str());
      }
   }
   else if (coefType == "K")
   {
      paramID = theObject->GetParameterID("K1");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("K1");
      
      paramID = theObject->GetParameterID("K2");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("K2");
      
      paramID = theObject->GetParameterID("K3");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("K3");
      
      paramID = theObject->GetParameterID("K4");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("K4");
      
      paramID = theObject->GetParameterID("K5");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("K5");
      
      paramID = theObject->GetParameterID("K6");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("K6");
      
      paramID = theObject->GetParameterID("K7");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("K7");
      
      paramID = theObject->GetParameterID("K8");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("K8");
      
      paramID = theObject->GetParameterID("K9");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("K9");
      
      paramID = theObject->GetParameterID("K10");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("K10");
      
      paramID = theObject->GetParameterID("K11");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("K11");
      
      paramID = theObject->GetParameterID("K12");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("K12");
      
      paramID = theObject->GetParameterID("K13");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("K13");
      
      paramID = theObject->GetParameterID("K14");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("K14");
      
      paramID = theObject->GetParameterID("K15");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("K15");
      
      paramID = theObject->GetParameterID("K16");
      coefValues.push_back(theObject->GetRealParameter(paramID));
      coefNames.push_back("K16");
      
      paramID = theObject->GetParameterID("K_UNITS");
      StringArray coefUnits = theObject->GetStringArrayParameter(paramID);
      
      for (Integer i = 0; i < coefCount; i++)
      {
         coefGrid->SetCellValue(i, 0, coefNames[i].c_str());
         coefGrid->SetCellValue(i, 1, wxVariant(coefValues[i]));
         coefGrid->SetCellValue(i, 2, coefUnits[i].c_str());
      }
   }
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ThrusterCoefficientDialog::SaveData()
{
   #ifdef DEBUG_COEF_SAVE
   MessageInterface::ShowMessage
      ("ThrusterCoefficientDialog::SaveData() entered\n");
   #endif
   
   canClose = true;
   
   // Validate input values
   for (int i = 0; i < coefCount; i++)
   {
      std::string field = coefGrid->GetCellValue(i, 0).c_str();
      std::string input = coefGrid->GetCellValue(i, 1).c_str();
      #ifdef DEBUG_COEF_SAVE
      MessageInterface::ShowMessage("   %s = '%s'\n", field.c_str(), input.c_str());
      #endif
      CheckReal(coefValues[i], input, field, "Real Number");
   }
   
   if (!canClose)
      return;
   
   // Save values
   Integer paramID = 0;
   
   #ifdef DEBUG_COEF_SAVE
   MessageInterface::ShowMessage("   Now saving coef values to Thruster\n");
   #endif
   
   for (int i = 0; i < coefCount; i++)
   {
      paramID = theObject->GetParameterID(coefNames[i]);
      theObject->SetRealParameter(paramID, coefValues[i]);
   }
   
   #ifdef DEBUG_COEF_SAVE
   MessageInterface::ShowMessage
      ("ThrusterCoefficientDialog::SaveData() exiting\n");
   #endif
}  

//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void ThrusterCoefficientDialog::ResetData()
{
}     

