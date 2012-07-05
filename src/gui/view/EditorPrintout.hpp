//$Id$
//------------------------------------------------------------------------------
//                              EditorPrint
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Linda Jun
// Created: 2009/01/15
//
/**
 * Declares EditorPrintout class.
 */
//------------------------------------------------------------------------------
#ifndef EditorPrintout_hpp
#define EditorPrintout_hpp

#include "Editor.hpp"
#include <wx/print.h>

class EditorPrintout: public wxPrintout
{
public:

   // constructor
   EditorPrintout (Editor *editor, wxChar *title = NULL);

   // event handlers
   bool OnPrintPage (int page);
   bool OnBeginDocument (int startPage, int endPage);

   // print functions
   bool HasPage (int page);
   void GetPageInfo (int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

private:
   Editor *mEditor;
   int mPagePrinted;
   wxRect mPageRect;
   wxRect mPrintRect;

   bool PrintScaling (wxDC *dc);
};

#endif
