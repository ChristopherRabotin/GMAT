//$Header$
//------------------------------------------------------------------------------
//                                  MdiTextEditView
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Original File: /samples/docvwmdi/view.hpp
// Author: Julian Smart
// Created: 1998/04/01
// Modified:
//   2003/11/04 Linda Jun - Followed GMAT coding style.
//
/**
 * Declares operations on text edit view.
 */
//------------------------------------------------------------------------------
#ifndef MdiTextEditView_hpp
#define MdiTextEditView_hpp

#include "gmatwxdefs.hpp"
#include "wx/docview.h"
#include "MdiTextSubFrame.hpp"

class MdiTextEditView: public wxView
{
    DECLARE_DYNAMIC_CLASS(MdiTextEditView)
public:
        
    wxMDIChildFrame *frame;
    MdiTextSubFrame *textsw;
    
    MdiTextEditView();    
    ~MdiTextEditView();
    
    bool OnCreate(wxDocument *doc, long flags);
    void OnDraw(wxDC *dc);
    void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
    bool OnClose(bool deleteWindow = TRUE);

    bool OnScriptBuildObject(wxCommandEvent& WXUNUSED(event));
    bool OnScriptRun(wxCommandEvent& WXUNUSED(event));
    
    DECLARE_EVENT_TABLE();
//      enum
//      {
//          MENU_SCRIPT_BUILD = 1,
//          MENU_SCRIPT_RUN
//      };
    
private:
};

//loj: need here because OnCreate is implemented in GmatApp.cpp
#define MENU_SCRIPT_BUILD_OBJECT 100
#define MENU_SCRIPT_RUN          101

#endif
