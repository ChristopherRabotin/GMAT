//$Id$
//------------------------------------------------------------------------------
//                              GmatMdiChildFrame
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Linda Jun
// Created: 2004/02/02
//
/**
 * Declares GmatMdiChildFrame class.
 */
//------------------------------------------------------------------------------

#ifndef GmatMdiChildFrame_hpp
#define GmatMdiChildFrame_hpp

#include "gmatwxdefs.hpp"
#include "GmatMenuBar.hpp"
#include "GuiInterpreter.hpp"
#include "GmatTreeItemData.hpp"  // for namespace GmatTree::

#ifdef __USE_STC_EDITOR__
#include "Editor.hpp"
#endif

class GmatMdiChildFrame : public wxMDIChildFrame
{
public:
   // constructors
   GmatMdiChildFrame(wxMDIParentFrame *parent, 
                     const wxString &name = "",
                     const wxString &title = "", 
                     const GmatTree::ItemType type = GmatTree::UNKNOWN_ITEM,
                     wxWindowID id = -1, 
                     const wxPoint &pos = wxDefaultPosition, 
                     const wxSize &size = wxDefaultSize, 
                     long style = wxDEFAULT_FRAME_STYLE);
   ~GmatMdiChildFrame();
   
#ifdef __WXMAC__
   wxString GetTitle();
   void SetTitle(wxString newTitle);
#endif

   virtual wxString   GetPlotName();
   virtual void       SetPlotName(const wxString &name);
   virtual void       SetSavedConfigFlag(bool isPreset);
   virtual bool       GetSavedConfigFlag();

   wxMenuBar*         GetMenuBar();
   GmatTree::ItemType GetItemType();
   void               SetDataType(GmatTree::ItemType type);
   wxTextCtrl*        GetScriptTextCtrl();
   void               SetScriptTextCtrl(wxTextCtrl *textCtrl);
   void               SetActiveChild(bool active);
   bool               IsActiveChild();
   void               UpdateGuiItem(int updateEdit, int updateAnimation);
   void               UpdateActiveChild();
   
#ifdef __USE_STC_EDITOR__
   Editor* GetEditor();
   void SetEditor(Editor *editor);
#endif
   
   void SetDirty(bool dirty);
   void OverrideDirty(bool flag);
   void SetCanBeDeleted(bool flag);
   bool IsDirty();
   bool CanClose();
   
   wxWindow* GetMdiParent();
   
   virtual wxWindow* GetAssociatedWindow();
   virtual void SetAssociatedWindow(wxWindow *win);
   
   virtual void OnActivate(wxActivateEvent &event);
   virtual void OnIconize(wxIconizeEvent &event);
   virtual void OnClose(wxCloseEvent &event);
   virtual void UpdateScriptActiveStatus(bool isActive);
   
   virtual void SetSaveLocationFlag(bool tf = true);
   virtual void SaveChildPositionAndSize();

protected:
   
#ifdef __WXMAC__
   wxString childTitle;
#endif
   
   static Integer maxZOrder;
   Integer        relativeZOrder;
   bool           usingSavedConfiguration;
   
   wxString mChildName;
   
   bool mDirty;
   bool mOverrideDirty;
   bool mCanClose;
   bool mCanSaveLocation;
   bool mIsActiveChild;
   bool mCanBeDeleted;
   GmatTree::ItemType mItemType;
   wxTextCtrl *theScriptTextCtrl;
   GmatMenuBar *theMenuBar;
   wxMDIParentFrame *theParent;
   wxWindow *theAssociatedWin;
   
   // GuiInterpreter
   GuiInterpreter *theGuiInterpreter;


#ifdef __USE_STC_EDITOR__
   Editor *theEditor;
#endif

   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
      
};


#endif // GmatMdiChildFrame_hpp
