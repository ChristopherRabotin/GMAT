//$Id$
//------------------------------------------------------------------------------
//                              GmatPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/02/02
//
/**
 * Declares GmatPanel class.
 */
//------------------------------------------------------------------------------

#ifndef GmatPanel_hpp
#define GmatPanel_hpp

#include "gmatwxdefs.hpp"
#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include "wx/notebook.h"
#include <wx/button.h>
#include <wx/grid.h>
#include "wx/radiobut.h"

#include "UserInputValidator.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

class GmatPanel : public wxPanel, public UserInputValidator
{
public:
   
   // constructors
   GmatPanel(wxWindow *parent, bool showBottomSizer = true,
             bool showScriptButton = true);

   virtual wxPanel* GetPanel();
   
   virtual bool PrepareObjectNameChange();
   virtual void ObjectNameChanged(Gmat::ObjectType type,
                                  const wxString &oldName,
                                  const wxString &newName);
   virtual void EnableUpdate(bool enable = true);
   virtual void SetCanClose(bool flag);
   virtual void SetEditorModified(bool isModified);
   virtual bool IsEditorModified();
   
   virtual void OnApply(wxCommandEvent &event);
   virtual void OnOK(wxCommandEvent &event);
   virtual void OnCancel(wxCommandEvent &event);
   virtual void OnHelp(wxCommandEvent &event);
   virtual void OnScript(wxCommandEvent &event);
   virtual void OnSummary(wxCommandEvent &event);
   
protected:
   
   // member functions
   virtual bool SetObject(GmatBase *obj);
   virtual void Show();
   virtual void Create() = 0;
   virtual void LoadData() = 0;
   virtual void SaveData() = 0;
   
   // member data
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   bool canClose;
   bool mShowBottomSizer;
   bool mShowScriptButton;
   bool mDataChanged;
   bool mEditorModified;
   
   wxWindow *theParent;
   
   wxBoxSizer *thePanelSizer;
   #ifdef __SHOW_TOP_SIZER__
   wxStaticBoxSizer *theTopSizer;
   #endif
   //wxStaticBoxSizer *theMiddleSizer;
   wxSizer *theMiddleSizer;
   wxStaticBoxSizer *theBottomSizer;
   
   wxButton *theOkButton;
   wxButton *theApplyButton;
   wxButton *theCancelButton;
   wxButton *theHelpButton;
   wxButton *theScriptButton;
   wxButton *theSummaryButton;
   
   std::string mObjectName;
   GmatBase *mObject;
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_BUTTON_OK = 8000,
      ID_BUTTON_APPLY,
      ID_BUTTON_CANCEL,
      ID_BUTTON_HELP,
      ID_BUTTON_SCRIPT,
      ID_BUTTON_SUMMARY,
   };
   
};

#endif // GmatPanel_hpp
