//$Id$
//------------------------------------------------------------------------------
//                              ArraySetupDialog
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Linda Jun
// Created: 2004/12/30
//
/**
 * Declares ArraySetupDialog class. This panel enables user to see and modify
 * array values.
 */
//------------------------------------------------------------------------------

#ifndef ArraySetupDialog_hpp
#define ArraySetupDialog_hpp

#include "GmatDialog.hpp"
#include "Parameter.hpp"
#include "Rmatrix.hpp"

class ArraySetupDialog: public GmatDialog
{
public:
   // constructors
   ArraySetupDialog(wxWindow *parent, const wxString &name); 
   
private:
   
   Rmatrix mRmat;  // for saving array values
   Parameter *mParam;
   bool mIsArrValChanged;
   
   int mNumRows;
   int mNumCols;
   
   wxString mVarName;
   
   wxTextCtrl *mArrNameTextCtrl;
   wxTextCtrl *mArrRowTextCtrl;
   wxTextCtrl *mArrColTextCtrl;
   wxTextCtrl *mArrValTextCtrl;
   
   wxButton *mUpdateButton;
   
   wxComboBox *mRowComboBox;
   wxComboBox *mColComboBox;
   
   wxGrid *mArrGrid;
   
   wxBoxSizer *mPageBoxSizer;
   wxStaticBoxSizer *mArrStaticBoxSizer;
   wxBoxSizer *mArrValBoxSizer;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   // event handling
   void OnTextUpdate(wxCommandEvent& event);
   void OnTextEnter(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   void OnButtonClick(wxCommandEvent& event);
   void OnGridCellChange(wxGridEvent& event);
   
   void UpdateCellValue();
   bool CheckCellValue(Real &rval, int row, int col, const std::string &str);
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9000,
      ID_LISTBOX,
      ID_BUTTON,
      ID_COMBO,
      ID_TEXTCTRL,
      ID_GRID
   };
};

#endif
