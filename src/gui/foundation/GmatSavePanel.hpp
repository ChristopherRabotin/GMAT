//$Id$
//------------------------------------------------------------------------------
//                              GmatSavePanel
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
 * Declares GmatSavePanel class.
 */
//------------------------------------------------------------------------------

#ifndef GmatSavePanel_hpp
#define GmatSavePanel_hpp

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

#include "GmatPanel.hpp"
#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

class GmatSavePanel : public GmatPanel
{
public:
   // constructors
   GmatSavePanel(wxWindow *parent, bool showScriptButton = true,
                 wxString filename = "", bool showScriptActiveStatus = false,
                 bool isScriptActive = false);
   
   virtual void OnSave(wxCommandEvent &event);
   virtual void OnSaveAs(wxCommandEvent &event);
   virtual void OnClosePanel(wxCommandEvent &event);
   virtual void OnScript(wxCommandEvent &event);
   
   virtual void SetEditorModified(bool modified, bool updateSyncStatus = true);
   void UpdateScriptActiveStatus(bool isActive);
   bool UpdateStatusOnClose();
   void ReloadFile();
   
protected:
   // member functions
   virtual void Create() = 0;
   virtual void Show();
   virtual void LoadData() = 0;
   virtual void SaveData() = 0;
   
   bool DoesFileExist(std::string scriptFilename);
   void MakeScriptActive(wxCommandEvent &event, bool isScriptModified);
   void RefreshScriptActiveStatus(bool isActive);
   void SaveScript();
   void SaveAndBuildScript(wxCommandEvent &event);
   
   // member data
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   bool canClose;
   bool mShowScriptButton;
   bool hasFileLoaded;
   bool mShowScriptActiveStatus;
   bool mIsScriptActive;
   bool mSyncGui;
   bool mDelayBuild;
   bool mSaveCanceled;
   
   wxString mFilename;
   wxString mScriptFilename;
   
   wxWindow *theParent;
   
   wxBoxSizer       *thePanelSizer;
   wxStaticBoxSizer *theTopSizer;
   wxStaticBoxSizer *theMiddleSizer;
   wxStaticBoxSizer *theBottomSizer;
   wxBoxSizer       *theButtonSizer;
   
   wxButton *theSaveButton;
   wxButton *theSaveAsButton;
   wxButton *theCloseButton;
   wxButton *theScriptButton;
   wxButton *mSaveSyncButton;
   wxButton *mSaveSyncRunButton;
   
   wxStaticText *mScriptActiveLabel;
   wxStaticText *mScriptDirtyLabel;
   
   GmatBase *mObject;
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_BUTTON_SAVE = 8050,
      ID_BUTTON_SAVE_AS,
      ID_BUTTON_CLOSE,
      ID_BUTTON_HELP,
      ID_BUTTON_SCRIPT,
   };
   
};

#endif // GmatSavePanel_hpp
