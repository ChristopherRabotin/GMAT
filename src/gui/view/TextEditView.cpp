//$Header$
//------------------------------------------------------------------------------
//                                  TextEditView
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Original File: view.cpp
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
#include "GmatAppData.hpp" // for GuiInterpreter pointer

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "TextEditView.hpp"

BEGIN_EVENT_TABLE(TextEditView, wxView)
    EVT_MENU(MENU_SCRIPT_BUILD_OBJECT, TextEditView::OnScriptBuildObject)
    EVT_MENU(MENU_SCRIPT_RUN, TextEditView::OnScriptRun)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// TextEditView()
//------------------------------------------------------------------------------
TextEditView::TextEditView()
    : wxView()
{
    frame = (wxFrame *) NULL;
    textsw = (TextSubFrame *) NULL; 
}
    
//------------------------------------------------------------------------------
// ~TextEditView()
//------------------------------------------------------------------------------
TextEditView::~TextEditView()
{
}
    
// Handled by wxTextWindow
//------------------------------------------------------------------------------
// void OnDraw(wxDC *WXUNUSED(dc) )
//------------------------------------------------------------------------------
void TextEditView::OnDraw(wxDC *WXUNUSED(dc) )
{
}

//------------------------------------------------------------------------------
// void OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint) )
//------------------------------------------------------------------------------
void TextEditView::OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint) )
{
}

//------------------------------------------------------------------------------
// bool OnClose(bool deleteWindow)
//------------------------------------------------------------------------------
bool TextEditView::OnClose(bool deleteWindow)
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
bool TextEditView::OnScriptBuildObject(wxCommandEvent& WXUNUSED(event))
{
    wxString filename = GetDocument()->GetFilename();
    
    bool status = GmatAppData::GetGuiInterpreter()->
        InterpretScript(std::string(filename.c_str()));
}

//------------------------------------------------------------------------------
// bool OnScriptRun(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
bool TextEditView::OnScriptRun(wxCommandEvent& WXUNUSED(event))
{
    bool status = GmatAppData::GetGuiInterpreter()->RunScript();
}
