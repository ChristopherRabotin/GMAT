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
    
   ParameterCreateDialog(wxWindow *parent);

   wxArrayString& GetParamNames()
      { return mParamNames; }
   bool IsParamCreated()
      { return mIsParamCreated; }
    
protected:

   Parameter *mCurrParam;
   
   wxArrayString mParamNames;
   bool mIsParamCreated;
   bool mCreateVariable;
   bool mCreateArray;
   
   wxColour mColor;
   
   wxStaticText *mCoordSysLabel;
   
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
   
   wxComboBox *mCoordSysComboBox;
   wxComboBox *mCentralBodyComboBox; //loj: 1/3/05 Added
   
   wxBoxSizer *mDetailsBoxSizer;

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
};

#endif
