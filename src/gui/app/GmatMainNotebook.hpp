//$Header$
//------------------------------------------------------------------------------
//                              GmatMainNotebook
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Allison Greene
// Created: 2003/10/03
/**
 * This class provides the notebook for the right side of the main panel.
 */
//------------------------------------------------------------------------------
#ifndef GmatMainNotebook_hpp
#define GmatMainNotebook_hpp

#include "gmatwxdefs.hpp"
#include "wx/notebook.h"

#include "GmatTreeItemData.hpp"

class GmatMainNotebook : public wxNotebook
{
public:
   // constructors
   GmatMainNotebook( wxWindow *parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0 );
   void CreatePage(GmatTreeItemData *item);
   void ClosePage();
   bool OpenPage(GmatTreeItemData *item);
   wxScrolledWindow *GetPanel();
   
protected:
private:
   wxWindow *parent;
   wxList *curPages;
   wxScrolledWindow *panel;
   wxSize theSize;
    
   DECLARE_EVENT_TABLE();
};

#endif // GmatMainNotebook_hpp
