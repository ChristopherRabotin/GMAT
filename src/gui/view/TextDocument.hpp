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
 * Declares operations on text edit document.
 */
//------------------------------------------------------------------------------
#ifndef TextDocument_hpp
#define TextDocument_hpp

#include "gmatwxdefs.hpp"

#include "wx/docview.h"
#include "wx/cmdproc.h"


class TextDocument: public wxDocument
{
    DECLARE_DYNAMIC_CLASS(TextDocument)
public:
    virtual bool OnSaveDocument(const wxString& filename);
    virtual bool OnOpenDocument(const wxString& filename);
    virtual bool IsModified(void) const;
    virtual void Modify(bool mod);
    
    TextDocument(void) {}
    ~TextDocument(void) {}
private:
};

#endif
