//$Header$
//------------------------------------------------------------------------------
//                              ParameterSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Implements ParameterSetupPanel class.
 */
//------------------------------------------------------------------------------

#include "ParameterSetupPanel.hpp"
#include "RgbColor.hpp"
#include "MessageInterface.hpp"

#include "wx/colordlg.h"   // for wxColourDialog

//#define DEBUG_PARAM_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ParameterSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   
   EVT_BUTTON(ID_COLOR_BUTTON, ParameterSetupPanel::OnColorButtonClick)
   EVT_COMBOBOX(ID_COMBO, ParameterSetupPanel::OnComboSelection)
   EVT_TEXT(ID_TEXTCTRL, ParameterSetupPanel::OnTextUpdate)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ParameterSetupPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
ParameterSetupPanel::ParameterSetupPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent)
{
   mVarName = name;
   mIsColorChanged = false;
   mIsExpChanged = false;
   
   Create();
   Show();
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void ParameterSetupPanel::Create()
{
   int bsize = 3; // border size
   
   //-------------------------------------------------------
   // for Variable Setup
   //-------------------------------------------------------
   // wxString
   wxString strCoordArray[] = { wxT("") };

   //wxStaticText
   wxStaticText *coordStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Coordinate System"),
                       wxDefaultPosition, wxDefaultSize, 0);
    
   wxStaticText *nameStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Name"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *emptyStaticText =
      new wxStaticText(this, ID_TEXT, wxT("  "),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *equalSignStaticText =
      new wxStaticText(this, ID_TEXT, wxT("="),
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *expStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Expression"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *colorStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Color"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   // wxTextCtrl
   mVarNameTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                 wxDefaultPosition, wxSize(150,20), 0);
   mVarExpTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                 wxDefaultPosition, wxSize(300,20), 0);
    
   // wxButton
   mColorButton =
      new wxButton(this, ID_COLOR_BUTTON, wxT(""),
                   wxDefaultPosition, wxSize(25, 20), 0);
   mColorButton->SetBackgroundColour(mColor);
   
   // wxComboBox
   mCoordComboBox = new wxComboBox(this, ID_COMBO, wxT(""), wxDefaultPosition,
                                  wxSize(100,-1), 1, strCoordArray, wxCB_DROPDOWN);
    
    
   // wxSizers
   mPageBoxSizer = new wxBoxSizer(wxVERTICAL);
   wxFlexGridSizer *top1FlexGridSizer = new wxFlexGridSizer(3, 0, 0);
   wxBoxSizer *detailsBoxSizer = new wxBoxSizer(wxHORIZONTAL);   
   //wxStaticBox *variableStaticBox = new wxStaticBox(this, -1, wxT("Variable"));
   wxStaticBox *variableStaticBox = new wxStaticBox(this, -1, wxT(""));
   mVarStaticBoxSizer = new wxStaticBoxSizer(variableStaticBox, wxVERTICAL);
   
   // Add to wx*Sizers
   // for Variable Setup
   // 1st row
   top1FlexGridSizer->Add(nameStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   top1FlexGridSizer->Add(expStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
    
   // 1st row
   top1FlexGridSizer->Add(mVarNameTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(equalSignStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(mVarExpTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);

   // detail
   detailsBoxSizer->Add(coordStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(mCoordComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(colorStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   detailsBoxSizer->Add(mColorButton, 0, wxALIGN_CENTRE|wxALL, bsize);

   mVarStaticBoxSizer->Add(top1FlexGridSizer, 0, wxALIGN_TOP|wxALL, bsize);
   mVarStaticBoxSizer->Add(detailsBoxSizer, 0, wxALIGN_LEFT|wxALL, bsize);

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
      new wxStaticText(this, ID_TEXT, wxT("="),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *arrTimesStaticText =
      new wxStaticText(this, ID_TEXT, wxT(" X"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mArrNameTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                     wxDefaultPosition, wxSize(120,20), 0);
   mArrRowTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                    wxDefaultPosition, wxSize(35,20), 0);
   mArrColTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                    wxDefaultPosition, wxSize(35,20), 0);
   
   //wxStaticBox *arrayStaticBox = new wxStaticBox(this, -1, wxT("Array"));
   wxStaticBox *arrayStaticBox = new wxStaticBox(this, -1, wxT(""));
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
   
   mArrStaticBoxSizer->Add(arr1FlexGridSizer, 0, wxALIGN_TOP|wxALL, bsize);
   
   mPageBoxSizer->Add(mVarStaticBoxSizer, 0, wxALIGN_TOP|wxALL, bsize);
   mPageBoxSizer->Add(mArrStaticBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(mPageBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

   
   mPageBoxSizer->Show(mVarStaticBoxSizer, false);
   mPageBoxSizer->Show(mArrStaticBoxSizer, false);
   mPageBoxSizer->Layout();
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ParameterSetupPanel::LoadData()
{
   mParam = theGuiInterpreter->GetParameter(std::string(mVarName.c_str()));

   if (mParam != NULL)
   {
      
#if DEBUG_PARAM_PANEL
      MessageInterface::ShowMessage
         ("ParameterSetupPanel::LoadData() paramName=%s\n", mParam->GetName().c_str());
#endif

      try
      {
         if (mParam->GetTypeName() == "Variable")
         {
            //std::string varDesc = mParam->GetStringParameter("Description");
            // Show expression (loj: 9/23/04)
            std::string varExp = mParam->GetStringParameter("Expression");
            mVarNameTextCtrl->SetValue(mVarName);
            mVarExpTextCtrl->SetValue(varExp.c_str());

            UnsignedInt intColor = mParam->GetUnsignedIntParameter("Color");
            RgbColor color(intColor);
            mColor.Set(color.Red(), color.Green(), color.Blue());
            mColorButton->SetBackgroundColour(mColor);

            mPageBoxSizer->Show(mVarStaticBoxSizer, true);
            mPageBoxSizer->Show(mArrStaticBoxSizer, false);
            mPageBoxSizer->Layout();
         }
         else if(mParam->GetTypeName() == "Array")
         {
            mArrNameTextCtrl->SetValue(mVarName);
            wxString str;
            str << mParam->GetIntegerParameter("NumRows");
            mArrRowTextCtrl->SetValue(str);
            str = "";
            str << mParam->GetIntegerParameter("NumCols");
            mArrColTextCtrl->SetValue(str);
            
            mPageBoxSizer->Show(mVarStaticBoxSizer, false);
            mPageBoxSizer->Show(mArrStaticBoxSizer, true);
            mPageBoxSizer->Layout();
         }
      }
      catch (BaseException &e)
      {
         wxLogError(wxT(e.GetMessage().c_str()));
         wxLog::FlushActive();
      }
   }

   // if expression is just a number, enable editing
   double realVal;
   if (mVarExpTextCtrl->GetValue().ToDouble(&realVal))
      mVarExpTextCtrl->Enable();
   else
      mVarExpTextCtrl->Disable();
   
   mVarNameTextCtrl->Disable();
   mArrNameTextCtrl->Disable();
   mArrRowTextCtrl->Disable();
   mArrColTextCtrl->Disable();
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ParameterSetupPanel::SaveData()
{
   if (mIsColorChanged)
   {
      mIsColorChanged = false;
      RgbColor color(mColor.Red(), mColor.Green(), mColor.Blue());
      mParam->SetUnsignedIntParameter("Color", color.GetIntColor());
   }

   if (mIsExpChanged)
   {
      mIsExpChanged = false;
      mParam->SetStringParameter("Expression",
                                 std::string(mVarExpTextCtrl->GetValue().c_str()));
   }
}

//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSetupPanel::OnTextUpdate(wxCommandEvent& event)
{
   if (mVarExpTextCtrl->IsModified())
   {
      mIsExpChanged = true;
      theApplyButton->Enable();
   }
}

//------------------------------------------------------------------------------
// void OnComboSelection(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSetupPanel::OnComboSelection(wxCommandEvent& event)
{
   theApplyButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnColorButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSetupPanel::OnColorButtonClick(wxCommandEvent& event)
{    
   wxColourData data;
   data.SetColour(mColor);

   wxColourDialog dialog(this, &data);
   //dialog.CenterOnParent();
   dialog.Center();
   
   if (dialog.ShowModal() == wxID_OK)
   {
      mColor = dialog.GetColourData().GetColour();
      mColorButton->SetBackgroundColour(mColor);
      mIsColorChanged = true;
      theApplyButton->Enable();
   }
}
