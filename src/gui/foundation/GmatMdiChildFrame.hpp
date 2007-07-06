//$Header$
//------------------------------------------------------------------------------
//                              GmatMdiChildFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
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
#include "GmatTreeItemData.hpp"  // for namespace GmatTree::

class GmatMdiChildFrame : public wxMDIChildFrame
{
public:
   // constructors
   GmatMdiChildFrame(wxMDIParentFrame *parent, 
                     const wxString &title = "", 
                     const wxString &name = "",
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

   wxMenuBar* GetMenuBar() { return theMenuBar; }
   wxTextCtrl *GetScriptTextCtrl() { return theScriptTextCtrl; }
   GmatTree::ItemType GetItemType() { return mItemType; }
   void SetDataType(GmatTree::ItemType type) {mItemType = type;}
   void SetScriptTextCtrl(wxTextCtrl *scriptTC) { theScriptTextCtrl = scriptTC; }
   void SetDirty(bool dirty) { mDirty = dirty; }
   bool IsDirty() { return mDirty; }
   bool CanClose() { return mCanClose; }
   
   virtual void OnClose(wxCloseEvent &event);
   
protected:
   
#ifdef __WXMAC__
   wxString childTitle;
#endif
   
   bool mDirty;
   bool mCanClose;
   GmatTree::ItemType mItemType;
   wxTextCtrl *theScriptTextCtrl;
   GmatMenuBar *theMenuBar;
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
      
};


#endif // GmatMdiChildFrame_hpp
