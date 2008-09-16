//$Header$
//------------------------------------------------------------------------------
//                             GmatMenuBar
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Allison Greene
// Created: 2006/05/09
/**
 * This class provides the menu bar for the main frame.
 */
//------------------------------------------------------------------------------
#ifndef GmatMenuBar_hpp
#define GmatMenuBar_hpp

#include "gmatwxdefs.hpp"
#include "GmatTreeItemData.hpp"  // for GmatTree::

class GmatMenuBar : public wxMenuBar
{
public:
   // constructors
   GmatMenuBar(GmatTree::ItemType itemType, wxMenu *windowMenu, long style = 0);
   
protected:
private:
   void CreateMenu(GmatTree::ItemType itemType, wxMenu *windowMenu);
   
   DECLARE_EVENT_TABLE();

};

#endif // GmatMenuBar_hpp
