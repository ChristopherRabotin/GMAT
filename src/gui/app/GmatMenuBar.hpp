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

class GmatMenuBar : public wxMenuBar
{
public:
   // constructors
   GmatMenuBar(int dataType, long style = 0 );
       
protected:
private:
  void CreateMenu(int dataType);
  DECLARE_EVENT_TABLE();

};

#endif // GmatMenuBar_hpp
