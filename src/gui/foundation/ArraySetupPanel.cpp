//$Header$
//------------------------------------------------------------------------------
//                              ArraySetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/12/30
//
/**
 * Implements ArraySetupPanel class. This panel enables user to see and modify
 * array values.
 */
//------------------------------------------------------------------------------

#include "ArraySetupPanel.hpp"
#include "RgbColor.hpp"
#include "Array.hpp"
#include "MessageInterface.hpp"

#include "wx/colordlg.h"   // for wxColourDialog

//#define DEBUG_ARRAY_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ArraySetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   
   EVT_BUTTON(ID_BUTTON, ArraySetupPanel::OnButtonClick)
   EVT_COMBOBOX(ID_COMBO, ArraySetupPanel::OnComboBoxChange)
   EVT_TEXT(ID_TEXTCTRL, ArraySetupPanel::OnTextUpdate)
   EVT_TEXT_ENTER(ID_TEXTCTRL, ArraySetupPanel::OnTextEnter)
   EVT_GRID_CELL_CHANGE(ArraySetupPanel::OnGridCellChange)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ArraySetupPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
ArraySetupPanel::ArraySetupPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent)
{
   mVarName = name;
   mIsArrValChanged = false;
   mNumRows = 0;
   mNumCols = 0;
   
   Create();
   Show();
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void ArraySetupPanel::Create()
{
   int bsize = 3; // border size
   
   wxString arrValArray[] = { wxT("") };
   
   wxStaticText *emptyStaticText =
      new wxStaticText(this, ID_TEXT, wxT("  "),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   //-------------------------------------------------------
   // for Array Setup
   //-------------------------------------------------------
   wxStaticText *arrNameStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Name"),
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *arr1RowStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Row"),
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *arr1ColStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Column"),
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *arrEqualSignStaticText =
      new wxStaticText(this, ID_TEXT, wxT(" = "),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *arrTimesStaticText =
      new wxStaticText(this, ID_TEXT, wxT(" X "),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mArrNameTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                     wxDefaultPosition, wxSize(120,20), 0);
   mArrRowTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                    wxDefaultPosition, wxSize(35,20), 0);
   mArrColTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                    wxDefaultPosition, wxSize(35,20), 0);
   
   wxStaticBox *arrayStaticBox = new wxStaticBox(this, -1, wxT("Array")); //loj: 1/7/05 added Array
   mArrStaticBoxSizer = new wxStaticBoxSizer(arrayStaticBox, wxVERTICAL);
   wxFlexGridSizer *arr1FlexGridSizer = new wxFlexGridSizer(5, 0, 0);
   
   // 1st row
   arr1FlexGridSizer->Add(arrNameStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(arr1RowStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(arr1ColStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   // 2nd row
   arr1FlexGridSizer->Add(mArrNameTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(arrEqualSignStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(mArrRowTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(arrTimesStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(mArrColTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   // show single element
   wxStaticText *commaStaticText =
      new wxStaticText(this, ID_TEXT, wxT(","),
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *equalStaticText =
      new wxStaticText(this, ID_TEXT, wxT("="),
                        wxDefaultPosition, wxDefaultSize, 0);
   
   mRowComboBox = new wxComboBox(this, ID_COMBO, wxT(""), wxDefaultPosition,
                                 wxSize(40,-1), 0, arrValArray, wxCB_DROPDOWN);
   mColComboBox = new wxComboBox(this, ID_COMBO, wxT(""), wxDefaultPosition,
                                 wxSize(40,-1), 0, arrValArray, wxCB_DROPDOWN);
   
   mArrValTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                     wxDefaultPosition, wxSize(100,20), 0);
   mUpdateButton =
      new wxButton(this, ID_BUTTON, wxT("Update"),
                   wxDefaultPosition, wxDefaultSize, 0);
   
   wxBoxSizer *singleValBoxSizer = new wxBoxSizer(wxHORIZONTAL);

   singleValBoxSizer->Add(mRowComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   singleValBoxSizer->Add(commaStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   singleValBoxSizer->Add(mColComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   singleValBoxSizer->Add(equalStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   singleValBoxSizer->Add(mArrValTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   singleValBoxSizer->Add(mUpdateButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   // use wxGrid show array values
   mArrGrid =
      new wxGrid(this, ID_GRID, wxDefaultPosition, wxSize(300,157), wxWANTS_CHARS);

   mArrGrid->SetRowLabelSize(20);
   mArrGrid->SetColLabelSize(20);
   mArrGrid->SetScrollbars(5, 8, 15, 15);
   mArrGrid->EnableEditing(true);

   mArrValBoxSizer = new wxBoxSizer(wxVERTICAL);
   mArrValBoxSizer->Add(singleValBoxSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   mArrValBoxSizer->Add(mArrGrid, 0, wxALIGN_CENTER|wxALL, bsize);

   mArrStaticBoxSizer->Add(arr1FlexGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mArrStaticBoxSizer->Add(mArrValBoxSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
   mPageBoxSizer = new wxBoxSizer(wxVERTICAL);
   mPageBoxSizer->Add(mArrStaticBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(mPageBoxSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);

}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ArraySetupPanel::LoadData()
{
   mParam = theGuiInterpreter->GetParameter(std::string(mVarName.c_str()));

   // Set the pointer for the "Show Script" button
   mObject = mParam;

   if (mParam != NULL)
   {
      
#if DEBUG_ARRAY_PANEL
      MessageInterface::ShowMessage
         ("ArraySetupPanel::LoadData() paramName=%s\n", mParam->GetName().c_str());
#endif

      try
      {
         mNumRows = mParam->GetIntegerParameter("NumRows");
         mNumCols = mParam->GetIntegerParameter("NumCols");
            
         mArrNameTextCtrl->SetValue(mVarName);
         wxString str;
         str << mNumRows;
         mArrRowTextCtrl->SetValue(str);
         str = "";
         str << mNumCols;
         mArrColTextCtrl->SetValue(str);

         // append row index to ComboBox
         for (int i=0; i<mNumRows; i++)
         {
            wxString intStr;
            intStr << i;
            mRowComboBox->Append(intStr);
         }
         mRowComboBox->SetSelection(0);
            
         // append column index to ComboBox
         for (int i=0; i<mNumCols; i++)
         {
            wxString intStr;
            intStr << i;
            mColComboBox->Append(intStr);
         }
         mColComboBox->SetSelection(0);

         Array *arrParam = (Array*)mParam;
         
         // set value (wxTextCtrl)
         Real val = arrParam->GetRealParameter("SingleValue", 0, 0);
         str.Printf("%g", val);
         mArrValTextCtrl->SetValue(str);
         
#ifdef DEBUG_ARRAY_PANEL
         MessageInterface::ShowMessage("mNumRows=%d, mNumCols=%d\n",
                                       mNumRows, mNumCols);
#endif
         
         // set value (wxGrid)
         mArrGrid->CreateGrid(mNumRows, mNumCols);
         
         int row, col;
            
         for (row=0; row<mNumRows; row++)
         {
            str.Printf("%d", row);
            mArrGrid->SetRowLabelValue(row, str);
         }
         
         for (col=0; col<mNumCols; col++)
         {
            //mArrGrid->SetColFormatFloat(col, 6, 2);
            str.Printf("%d", col);
            mArrGrid->SetColLabelValue(col, str);
         }
         
         // set size to Rmatrix and initialize
         mRmat.SetSize(mNumRows, mNumCols);
         for (row=0; row<mNumRows; row++)
         {
            for (col=0; col<mNumCols; col++)
            {
               mRmat.SetElement(row, col, arrParam->
                                GetRealParameter("SingleValue", row, col));
               str.Printf("%g", mRmat.GetElement(row, col));
               mArrGrid->SetCellValue(row, col, str);
            }
         }
      }
      catch (BaseException &e)
      {
         wxLogError(wxT(e.GetMessage().c_str()));
         wxLog::FlushActive();
      }
   }

   mArrNameTextCtrl->Disable();
   mArrRowTextCtrl->Disable();
   mArrColTextCtrl->Disable();
   mUpdateButton->Disable();
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ArraySetupPanel::SaveData()
{
   if (mIsArrValChanged)
   {
      mIsArrValChanged = false;
      Array *arrParam = (Array*)mParam;
      
      try
      {
         for (int i=0; i<mNumRows; i++)
            for (int j=0; j<mNumCols; j++)
               arrParam->SetRealParameter("SingleValue", mRmat.GetElement(i,j), i, j);
      }
      catch (BaseException &e)
      {
         wxLogError(wxT(e.GetMessage().c_str()));
         wxLog::FlushActive();
      }
   }
}

//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ArraySetupPanel::OnTextUpdate(wxCommandEvent& event)
{
   if (event.GetEventObject() == mArrValTextCtrl &&
       mArrValTextCtrl->IsModified())
   {
      mUpdateButton->Enable();
   }
}

//------------------------------------------------------------------------------
// void OnTextEnter(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ArraySetupPanel::OnTextEnter(wxCommandEvent& event)
{
   if (event.GetEventObject() == mArrValTextCtrl)
   {
      UpdateCellValue();
   }
}

//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ArraySetupPanel::OnComboBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mRowComboBox ||
       event.GetEventObject() == mColComboBox)
   {
      wxString str;
      str.Printf("%g", mRmat.GetElement(mRowComboBox->GetSelection(),
                                        mColComboBox->GetSelection()));
      mArrValTextCtrl->SetValue(str);
   }
}

//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ArraySetupPanel::OnButtonClick(wxCommandEvent& event)
{    
   if (event.GetEventObject() == mUpdateButton)
   {
      UpdateCellValue();
   }
}

//------------------------------------------------------------------------------
// void OnGridCellChange(wxGridEvent& event)
//------------------------------------------------------------------------------
void ArraySetupPanel::OnGridCellChange(wxGridEvent& event)
{
   int row = mArrGrid->GetGridCursorRow();
   int col = mArrGrid->GetGridCursorCol();
   
   wxString strVal = mArrGrid->GetCellValue(row, col);
   
   Real val;
   strVal.ToDouble(&val);
   
   if (strVal.ToDouble(&val))
   {
      mIsArrValChanged = true;
      mRmat.SetElement(row, col, val);
      theApplyButton->Enable(true);

      // update value text if same row and col
      if (mRowComboBox->GetSelection() == row &&
          mColComboBox->GetSelection() == col)
      {
         mArrValTextCtrl->SetValue(strVal);
      }
   }
   else
   {
      wxLogError(wxT("Please enter a valid number."));
      strVal.Printf("%g", mRmat.GetElement(row, col));
      mArrGrid->SetCellValue(row, col, strVal);
      wxLog::FlushActive();
   }
}

//------------------------------------------------------------------------------
// void UpdateCellValue()
//------------------------------------------------------------------------------
void ArraySetupPanel::UpdateCellValue()
{
   mUpdateButton->Disable();
   
   int row = mRowComboBox->GetSelection();
   int col = mColComboBox->GetSelection();
      
   wxString strVal = mArrValTextCtrl->GetValue();
   Real val;
   
   if (strVal.ToDouble(&val))
   {
      mIsArrValChanged = true;
      mRmat.SetElement(row, col, val);
      mArrGrid->SetCellValue(row, col, strVal);
      theApplyButton->Enable(true);
   }
   else
   {
      wxLogError(wxT("Please enter a valid number."));
      wxLog::FlushActive();
   }
}
