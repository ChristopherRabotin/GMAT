//$Header$
//------------------------------------------------------------------------------
//                              GmatApp
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/08/05
//
/**
 * This class contains GMAT main application. Program starts here.
 */
//------------------------------------------------------------------------------
#ifndef GmatApp_hpp
#define GmatApp_hpp

#include "gmatwxdefs.hpp"
#include "Moderator.hpp"
#include "wx/docview.h"   // for wxDocument

class wxDocManager;

class GmatApp : public wxApp
{
public:
    GmatApp();
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();

    //loj: added
    int OnExit(void);
    wxFrame *CreateChildFrame(wxDocument *doc, wxView *view, bool isCanvas,
                              bool isScript);
    
protected:

private:
    Moderator *theModerator;
    ViewTextFrame *theMessageWindow;

};

DECLARE_APP(GmatApp)

#endif // GmatApp_hpp
