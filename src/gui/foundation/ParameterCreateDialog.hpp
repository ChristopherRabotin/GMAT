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

#include "Parameter.hpp"

class ParameterCreateDialog : public GmatDialog
{
public:
   
   ParameterCreateDialog(wxWindow *parent, int paramType);
   ~ParameterCreateDialog();
   
   wxArrayString& GetParamNames()
      { return mParamNames; }
   bool IsParamCreated()
      { return mIsParamCreated; }
   
protected:

   Parameter *mCurrParam;
   
   wxArrayString mParamNames;
   wxArrayString mExcludedScList;
   
   int mParamType;
   bool mIsParamCreated;
   bool mCreateVariable;
   bool mCreateString;
   bool mCreateArray;
   
   wxColour mColor;
   
   wxStaticText *mCoordSysLabel;
   
   wxTextCtrl *mVarNameTextCtrl;
   wxTextCtrl *mExprTextCtrl;
   wxTextCtrl *mStringNameTextCtrl;
   wxTextCtrl *mStringValueTextCtrl;
   wxTextCtrl *mArrNameTextCtrl;
   wxTextCtrl *mArrRowTextCtrl;
   wxTextCtrl *mArrColTextCtrl;
   wxTextCtrl *mArrValueTextCtrl;

   wxButton *mCreateVariableButton;
   wxButton *mPastePropertyButton;
   wxButton *mPasteUserVarButton;
   wxButton *mColorButton;
   wxButton *mCreateStringButton;
   wxButton *mCreateArrayButton;
   wxButton *mAssignArrayButton;
   wxButton *mInitArrayButton;
   
   wxListBox *mObjectListBox;
   wxListBox *mPropertyListBox;
   wxListBox *mUserVarListBox;
   wxListBox *mUserStringListBox;
   wxListBox *mUserArrayListBox;
   
   wxComboBox *mCoordSysComboBox;
   wxComboBox *mCentralBodyComboBox; //loj: 1/3/05 Added
   
   wxBoxSizer *mDetailsBoxSizer;

   // abstract methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   // event handling
   void OnTextUpdate(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   void OnButton(wxCommandEvent& event);
   void OnColorButtonClick(wxCommandEvent& event);
   void OnSelectProperty(wxCommandEvent& event);

   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9200,
      ID_LISTBOX,
      ID_PROPERTY_LISTBOX,
      ID_BUTTON,
      ID_COLOR_BUTTON,
      ID_COMBO,
      ID_TEXTCTRL
   };
   
private:
   void ShowCoordSystem();
   Parameter* CreateParameter(const wxString &name);
   wxString GetParamName();
   void CreateVariable();
   void CreateString();
   void CreateArray();
   
};

#endif
