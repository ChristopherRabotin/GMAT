//$Header$
//------------------------------------------------------------------------------
//                              GmatSplitterWindow
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Allison Greene
// Created: 2003/08/28
//
/**
 * This class provides the splitter window to divide the main panel and
 * handles the splitter window events
 */
//------------------------------------------------------------------------------
#ifndef GmatSplitterWindow_hpp
#define GmatSplitterWindow_hpp

#include "gmatwxdefs.h"
#include "wx/splitter.h"

class GmatSplitterWindow : public wxSplitterWindow
{
public:
   // constructor
   GmatSplitterWindow( wxWindow *parent );
   
protected:
private:
   DECLARE_EVENT_TABLE();
   // event handlers
   void OnPositionChanged(wxSplitterEvent &event);
   void OnPositionChanging(wxSplitterEvent &event);
};

#endif // GmatSplitterWindow_hpp
