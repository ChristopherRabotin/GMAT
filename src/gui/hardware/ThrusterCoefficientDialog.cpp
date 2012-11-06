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

//#define DEBUG_COEFS
//#define DEBUG_COEF_SAVE

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
                          const wxString &type, const RealArray &coefsCOrK)
   : GmatDialog(parent, id, title, obj, wxDefaultPosition, wxDefaultSize)
{
   coefType      = type;
   theObject     = obj;
   coefsModified = false;
   
   coefNames.clear();
   coefValues.clear();
   
   coefValues    = coefsCOrK;
   #ifdef DEBUG_COEFS
      MessageInterface::ShowMessage("In TCD constructor, size of coefs array is %d\n", (Integer) coefValues.size());
      for (unsigned int ii = 0; ii < coefValues.size(); ii++)
         MessageInterface::ShowMessage("coefValues[%d] = %lf\n", ii, coefValues.at(ii));
   #endif

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

   std::stringstream cStrings("");
   std::stringstream kStrings("");

   if (coefType == "C")
   {
      std::stringstream cStrings("");
      for (Integer ii = 0; ii < coefCount; ii++)
      {
         cStrings << "C" << ii + 1;
         coefNames.push_back(cStrings.str());
         cStrings.str("");
      }

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
      std::stringstream kStrings("");
      for (Integer ii = 0; ii < coefCount; ii++)
      {
         kStrings << "K" << ii + 1;
         coefNames.push_back(kStrings.str());
         kStrings.str("");
      }
      
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
   std::string field = "";
   std::string input = "";
   Real cellValue;
   bool cellValueOK = false;
   for (int i = 0; i < coefCount; i++)
   {
      field = coefGrid->GetCellValue(i, 0).c_str();
      input = coefGrid->GetCellValue(i, 1).c_str();
      #ifdef DEBUG_COEF_SAVE
         MessageInterface::ShowMessage("   %s = '%s'\n", field.c_str(), input.c_str());
      #endif
      cellValueOK = CheckReal(cellValue, input, field, "Real Number");
      #ifdef DEBUG_COEF_SAVE
         MessageInterface::ShowMessage("   check real is %s, cellValue = %lf, coefValues[%d] = %lf\n",
               (cellValueOK? "true" : "false"), cellValue, (Integer) i, coefValues.at(i));
      #endif
      if (cellValueOK)
      {
         if (cellValue != coefValues.at(i))  coefsModified = true;
         coefValues.at(i) = cellValue;
      }
      else
      {
         canClose = false;
      }
   }
   
   if (!canClose)
   {
      ResetData();
      return;
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
   coefsModified = false;
}     

