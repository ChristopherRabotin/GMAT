//$Header$
//------------------------------------------------------------------------------
//                              ParameterCreateDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Declares ParameterCreateDialog class. This class shows dialog window where a
 * user parameter can be created.
 * 
 */
//------------------------------------------------------------------------------
#ifndef ParameterCreateDialog_hpp
#define ParameterCreateDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"
#include <wx/string.h> // for wxArrayString

class ParameterCreateDialog : public GmatDialog
{
public:
    
   ParameterCreateDialog(wxWindow *parent);

   wxArrayString& GetParamNames() //loj: 9/27/04 return wxArrayString&
      { return mParamNames; }
   bool IsParamCreated()
      { return mIsParamCreated; }
    
private:
   wxArrayString mParamNames;
   bool mIsParamCreated;
   bool mCreateVariable;
   bool mCreateArray;
   
   wxColour mColor;
   wxTextCtrl *mVarNameTextCtrl;
   wxTextCtrl *mExprTextCtrl;
   wxTextCtrl *mArrNameTextCtrl;
   wxTextCtrl *mArrRowTextCtrl;
   wxTextCtrl *mArrColTextCtrl;
   wxTextCtrl *mArrValueTextCtrl;

   wxButton *mCreateVariableButton;
   wxButton *mPastePropertyButton;
   wxButton *mPasteUserVarButton;
   wxButton *mColorButton;
   wxButton *mCreateArrayButton;
   wxButton *mAssignArrayButton;
   wxButton *mInitArrayButton;
   
   wxListBox *mObjectListBox;
   wxListBox *mPropertyListBox;
   wxListBox *mUserVarListBox;
   wxListBox *mUserArrayListBox;
   
   wxComboBox *mCoordComboBox;
   
   void CreateVariable();
   void CreateArray();
   
   // abstract methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   // virtual methods from GmatDialog
   virtual void OnOK();

   // event handling
   void OnTextUpdate(wxCommandEvent& event);
   void OnComboSelection(wxCommandEvent& event);
   void OnButton(wxCommandEvent& event);
   void OnColorButtonClick(wxCommandEvent& event);

   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9200,
      ID_LISTBOX,
      ID_BUTTON,
      ID_COLOR_BUTTON,
      ID_COMBO,
      ID_TEXTCTRL
   };
};

#endif
