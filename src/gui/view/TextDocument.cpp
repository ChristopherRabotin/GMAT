//$Header$
//------------------------------------------------------------------------------
//                                  TextDocument
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Original File: doc.hpp
// Author: Julian Smart
// Created: 1998/04/01
// Modified:
//   2003/11/04 Linda Jun - Followed GMAT coding style.
//
/**
 * Defines operations on text edit document.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "wx/txtstrm.h"

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "TextDocument.hpp"
#include "TextEditView.hpp"

IMPLEMENT_DYNAMIC_CLASS(TextDocument, wxDocument)

// Since text windows have their own method for saving to/loading from files,
// we override OnSave/OpenDocument instead of Save/LoadObject
//------------------------------------------------------------------------------
// bool OnSaveDocument(const wxString& filename)
//------------------------------------------------------------------------------
bool TextDocument::OnSaveDocument(const wxString& filename)
{
    TextEditView *view = (TextEditView *)GetFirstView();
    
    if (!view->textsw->SaveFile(filename))
        return FALSE;
    Modify(FALSE);
    return TRUE;
}

//------------------------------------------------------------------------------
// bool OnOpenDocument(const wxString& filename)
//------------------------------------------------------------------------------
bool TextDocument::OnOpenDocument(const wxString& filename)
{
    TextEditView *view = (TextEditView *)GetFirstView();
    if (!view->textsw->LoadFile(filename))
        return FALSE;
    
    SetFilename(filename, TRUE);
    Modify(FALSE);
    UpdateAllViews();
    return TRUE;
}

//------------------------------------------------------------------------------
// bool IsModified(void) const
//------------------------------------------------------------------------------
bool TextDocument::IsModified(void) const
{
    TextEditView *view = (TextEditView *)GetFirstView();
    if (view)
    {
        return (wxDocument::IsModified() || view->textsw->IsModified());
    }
    else
        return wxDocument::IsModified();
}

//------------------------------------------------------------------------------
// void Modify(bool mod)
//------------------------------------------------------------------------------
void TextDocument::Modify(bool mod)
{
    TextEditView *view = (TextEditView *)GetFirstView();
    
    wxDocument::Modify(mod);
    
    if (!mod && view && view->textsw)
        view->textsw->DiscardEdits();
}
