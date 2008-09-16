//$Header$
//------------------------------------------------------------------------------
//                              FunctionSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2004/12/15
//
/**
 * Declares FunctionSetupPanel class.
 */
//------------------------------------------------------------------------------

#ifndef FunctionSetupPanel_hpp
#define FunctionSetupPanel_hpp

#include "GmatPanel.hpp"
#include "GmatFunction.hpp"

class FunctionSetupPanel: public GmatPanel
{
public:
   // constructors
   FunctionSetupPanel(wxWindow *parent, const wxString &name); 
   wxTextCtrl *mFileContentsTextCtrl;
    
private:
   GmatFunction *theGmatFunction;

   bool mEnableLoad;
   bool mEnableSave;

   wxStaticBoxSizer *mTopSizer;
   wxBoxSizer *mMiddleSizer;
   wxGridSizer *mBottomSizer;
//   wxFlexGridSizer *mPageSizer;
   wxBoxSizer *mPageSizer;

   wxTextCtrl *mFileNameTextCtrl;
   
   wxButton *mBrowseButton;
   wxButton *mLoadButton;
   wxButton *mSaveButton;

   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
    
   // event handling
   void OnTextUpdate(wxCommandEvent& event);
   void OnButton(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9000,
      ID_LISTBOX,
      ID_BUTTON,
      ID_COLOR_BUTTON,
      ID_COMBO,
      ID_TEXTCTRL
   };
};

#endif
