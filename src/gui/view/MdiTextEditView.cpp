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
// Original File: /samples/docvwmdi/view.cpp
// Author: Julian Smart
// Created: 1998/04/01
// Modified:
//   2003/11/04 Linda Jun - Followed GMAT coding style.
//
/**
 * Declares operations on text edit view.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"  // for GuiInterpreter pointer

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "MdiTextEditView.hpp"

BEGIN_EVENT_TABLE(MdiTextEditView, wxView)
    EVT_MENU(MENU_SCRIPT_BUILD_OBJECT, MdiTextEditView::OnScriptBuildObject)
    EVT_MENU(MENU_SCRIPT_RUN, MdiTextEditView::OnScriptRun)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// MdiTextEditView()
//------------------------------------------------------------------------------
MdiTextEditView::MdiTextEditView()
    : wxView()
{
    frame = (wxMDIChildFrame *) NULL;
    textsw = (MdiTextSubFrame *) NULL; 
}

//------------------------------------------------------------------------------
// ~MdiTextEditView()
//------------------------------------------------------------------------------
MdiTextEditView::~MdiTextEditView()
{
}
    
// Handled by wxTextWindow
//------------------------------------------------------------------------------
// void OnDraw(wxDC *WXUNUSED(dc) )
//------------------------------------------------------------------------------
void MdiTextEditView::OnDraw(wxDC *WXUNUSED(dc) )
{
}

//------------------------------------------------------------------------------
// void OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint) )
//------------------------------------------------------------------------------
void MdiTextEditView::OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint) )
{
}

//------------------------------------------------------------------------------
// bool OnClose(bool deleteWindow)
//------------------------------------------------------------------------------
bool MdiTextEditView::OnClose(bool deleteWindow)
{
    if (!GetDocument()->Close())
        return FALSE;
    
    Activate(FALSE);
    
    if (deleteWindow)
    {
        delete frame;
        return TRUE;
    }
    return TRUE;
}

//------------------------------------------------------------------------------
// bool OnScriptBuildObject(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
bool MdiTextEditView::OnScriptBuildObject(wxCommandEvent& WXUNUSED(event))
{
    //GmatAppData::theMessageWindow->ClearText();
    wxString filename = GetDocument()->GetFilename();
    
    bool status = GmatAppData::GetGuiInterpreter()->
        InterpretScript(std::string(filename.c_str()));

    // Update ResourceTree
    GmatAppData::GetResourceTree()->UpdateResources();
}

//------------------------------------------------------------------------------
// bool OnScriptRun(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
bool MdiTextEditView::OnScriptRun(wxCommandEvent& WXUNUSED(event))
{
    //GmatAppData::theMessageWindow->ClearText();
    bool status = GmatAppData::GetGuiInterpreter()->RunScript();
}
