//$Header$
//------------------------------------------------------------------------------
//                              ThrusterCoefficientDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
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
   EVT_GRID_CELL_CHANGE(ThrusterCoefficientDialog::OnCellValueChange)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ThrusterCoefficientDialog(wxWindow *parent)
//------------------------------------------------------------------------------
ThrusterCoefficientDialog::ThrusterCoefficientDialog(wxWindow *parent,
                                            Thruster *thruster, std::string type)
   : GmatDialog(parent, -1, wxString(_T("ThrusterCoefficientDialog")))
{   
   coefName.clear();
   coefValue.clear();
   
   coefType = type;
   theThruster = thruster;
   
   Create();
   ShowData();
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void ThrusterCoefficientDialog::Create()
{
   coefficientGrid = new wxGrid( this, ID_GRID, wxDefaultPosition, 
                    wxSize(200,315), wxWANTS_CHARS );
   coefficientGrid->CreateGrid( 14, 2 );
   coefficientGrid->SetRowLabelSize(0);
   coefficientGrid->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_CENTRE);
   
   coefficientGrid->SetColLabelValue(0, _T("Coef"));
   coefficientGrid->SetColSize(0, 60);
   coefficientGrid->SetColLabelValue(1, _T("in Newton"));
   coefficientGrid->SetColSize(1, 135);
   
   // wxSizers   
   theMiddleSizer->Add(coefficientGrid, 0, wxALIGN_CENTRE|wxALL, 3);
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ThrusterCoefficientDialog::LoadData()
{
   Integer count = 14;
   Integer paramID = 0;

   if (coefType == "C")
   {
      paramID = theThruster->GetParameterID("C1");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("C1");
 
      paramID = theThruster->GetParameterID("C2");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("C2");
      
      paramID = theThruster->GetParameterID("C3");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("C3");
      
      paramID = theThruster->GetParameterID("C4");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("C4");
      
      paramID = theThruster->GetParameterID("C5");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("C5");
      
      paramID = theThruster->GetParameterID("C6");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("C6");
      
      paramID = theThruster->GetParameterID("C7");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("C7");
      
      paramID = theThruster->GetParameterID("C8");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("C8");
      
      paramID = theThruster->GetParameterID("C9");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("C9");
      
      paramID = theThruster->GetParameterID("C10");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("C10");
      
      paramID = theThruster->GetParameterID("C11");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("C11");
      
      paramID = theThruster->GetParameterID("C12");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("C12");
      
      paramID = theThruster->GetParameterID("C13");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("C13");
      
      paramID = theThruster->GetParameterID("C14");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("C14");
   
      for (Integer i = 0; i < count; i++)
      {
         coefficientGrid->SetCellValue(i, 0, coefName[i].c_str());
         coefficientGrid->SetCellValue(i, 1, wxVariant(coefValue[i]));
      }
   }
   else if (coefType == "K")
   {
      paramID = theThruster->GetParameterID("K1");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("K1");
 
      paramID = theThruster->GetParameterID("K2");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("K2");
      
      paramID = theThruster->GetParameterID("K3");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("K3");
      
      paramID = theThruster->GetParameterID("K4");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("K4");
      
      paramID = theThruster->GetParameterID("K5");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("K5");
      
      paramID = theThruster->GetParameterID("K6");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("K6");
      
      paramID = theThruster->GetParameterID("K7");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("K7");
      
      paramID = theThruster->GetParameterID("K8");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("K8");
      
      paramID = theThruster->GetParameterID("K9");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("K9");
      
      paramID = theThruster->GetParameterID("K10");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("K10");
      
      paramID = theThruster->GetParameterID("K11");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("K11");
      
      paramID = theThruster->GetParameterID("K12");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("K12");
      
      paramID = theThruster->GetParameterID("K13");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("K13");
      
      paramID = theThruster->GetParameterID("K14");
      coefValue.push_back(theThruster->GetRealParameter(paramID));
      coefName.push_back("K14");
   
      for (Integer i = 0; i < count; i++)
      {
         coefficientGrid->SetCellValue(i, 0, coefName[i].c_str());
         coefficientGrid->SetCellValue(i, 1, wxVariant(coefValue[i]));
      }
   }        
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ThrusterCoefficientDialog::SaveData()
{
   Integer paramID = 0;

   if (coefType == "C")
   {
      paramID = theThruster->GetParameterID("C1");
      theThruster->SetRealParameter(paramID, coefValue[0]);
 
      paramID = theThruster->GetParameterID("C2");
      theThruster->SetRealParameter(paramID, coefValue[1]);
      
      paramID = theThruster->GetParameterID("C3");
      theThruster->SetRealParameter(paramID, coefValue[2]);
      
      paramID = theThruster->GetParameterID("C4");
      theThruster->SetRealParameter(paramID, coefValue[3]);
      
      paramID = theThruster->GetParameterID("C5");
      theThruster->SetRealParameter(paramID, coefValue[4]);
      
      paramID = theThruster->GetParameterID("C6");
      theThruster->SetRealParameter(paramID, coefValue[5]);
      
      paramID = theThruster->GetParameterID("C7");
      theThruster->SetRealParameter(paramID, coefValue[6]);
      
      paramID = theThruster->GetParameterID("C8");
      theThruster->SetRealParameter(paramID, coefValue[7]);
      
      paramID = theThruster->GetParameterID("C9");
      theThruster->SetRealParameter(paramID, coefValue[8]);
      
      paramID = theThruster->GetParameterID("C10");
      theThruster->SetRealParameter(paramID, coefValue[9]);
      
      paramID = theThruster->GetParameterID("C11");
      theThruster->SetRealParameter(paramID, coefValue[10]);
      
      paramID = theThruster->GetParameterID("C12");
      theThruster->SetRealParameter(paramID, coefValue[11]);
      
      paramID = theThruster->GetParameterID("C13");
      theThruster->SetRealParameter(paramID, coefValue[12]);
      
      paramID = theThruster->GetParameterID("C14");
      theThruster->SetRealParameter(paramID, coefValue[13]);
   }
   else if (coefType == "K")
   {
      paramID = theThruster->GetParameterID("K1");
      theThruster->SetRealParameter(paramID, coefValue[0]);
 
      paramID = theThruster->GetParameterID("K2");
      theThruster->SetRealParameter(paramID, coefValue[1]);
      
      paramID = theThruster->GetParameterID("K3");
      theThruster->SetRealParameter(paramID, coefValue[2]);
      
      paramID = theThruster->GetParameterID("K4");
      theThruster->SetRealParameter(paramID, coefValue[3]);
      
      paramID = theThruster->GetParameterID("K5");
      theThruster->SetRealParameter(paramID, coefValue[4]);
      
      paramID = theThruster->GetParameterID("K6");
      theThruster->SetRealParameter(paramID, coefValue[5]);
      
      paramID = theThruster->GetParameterID("K7");
      theThruster->SetRealParameter(paramID, coefValue[6]);
      
      paramID = theThruster->GetParameterID("K8");
      theThruster->SetRealParameter(paramID, coefValue[7]);
      
      paramID = theThruster->GetParameterID("K9");
      theThruster->SetRealParameter(paramID, coefValue[8]);
      
      paramID = theThruster->GetParameterID("K10");
      theThruster->SetRealParameter(paramID, coefValue[9]);
      
      paramID = theThruster->GetParameterID("K11");
      theThruster->SetRealParameter(paramID, coefValue[10]);
      
      paramID = theThruster->GetParameterID("K12");
      theThruster->SetRealParameter(paramID, coefValue[11]);
      
      paramID = theThruster->GetParameterID("K13");
      theThruster->SetRealParameter(paramID, coefValue[12]);
      
      paramID = theThruster->GetParameterID("K14");
      theThruster->SetRealParameter(paramID, coefValue[13]);
   }        
}  

//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void ThrusterCoefficientDialog::ResetData()
{
}     

//------------------------------------------------------------------------------
// virtual void OnCellValueChange()
//------------------------------------------------------------------------------
void ThrusterCoefficientDialog::OnCellValueChange(wxGridEvent &event)
{
    int row = event.GetRow();
    int col = event.GetCol();
    
    if (col == 0)
    {
       coefficientGrid->SetCellValue(row, col, coefName[row].c_str());    
    }    
    else if (col == 1)
    {
       Real rvalue;
       std::string inputString(coefficientGrid->GetCellValue(row, col).c_str());
       std::string msg = "The value of \"" + inputString + 
//                         "\" for field \"%s\" on object \"" +
//                          theFuelTank->GetName() + "\" 
                        "\" is not an allowed value.  "
                        "\nThe allowed values are: [ Real Number ].";

       if (!GmatStringUtil::ToReal(inputString,&rvalue))
       {
          MessageInterface::PopupMessage(Gmat::ERROR_, msg);
          return;
       }

       coefValue[row] = rvalue;
//       coefValue[row] = atof(coefficientGrid->GetCellValue(row, col));
       theOkButton->Enable(true);
    }    
}


