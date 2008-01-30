//$Id$
//------------------------------------------------------------------------------
//                              CallFunctionPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Allison Greene
// Created: 2004/9/15
/**
 * This class contains the Matlab Command Setup window.
 */
//------------------------------------------------------------------------------

#ifndef CallFunctionPanel_hpp
#define CallFunctionPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "CallFunction.hpp"

class CallFunctionPanel : public GmatPanel
{
public:
   // constructors
   CallFunctionPanel( wxWindow *parent, GmatCommand *cmd);
   ~CallFunctionPanel();
   
   virtual bool PrepareObjectNameChange();
   virtual void ObjectNameChanged(Gmat::ObjectType type,
                                  const wxString &oldName,
                                  const wxString &newName);
   
private:
   CallFunction *theCommand;
   
   wxArrayString mInputWxStrings;
   wxArrayString mOutputWxStrings;
   wxArrayString mObjectTypeList;
   int  mNumInput;
   int  mNumOutput;
   
   wxGrid *theInputGrid;
   wxGrid *theOutputGrid;
   
   wxComboBox *theFunctionComboBox;
   
   wxTextCtrl *theInputTextCtrl;
   wxTextCtrl *theOutputTextCtrl;
   
   wxButton *theInputViewButton;
   wxButton *theOutputViewButton;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // event handling
   void OnComboChange(wxCommandEvent &event);
   void OnCellRightClick(wxGridEvent &event);
   void OnButtonClick(wxCommandEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {
      ID_TEXTCTRL = 44000,
      ID_COMBO,
      ID_BUTTON,
      ID_CHECKBOX,
      ID_GRID,
      MENU_INSERT_P,
      MENU_DELETE_P,
      MENU_CLEAR_P,
      ID_TEXT,
   };
};

#endif // CallFunctionPanel_hpp

