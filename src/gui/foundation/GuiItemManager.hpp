//$Header$
//------------------------------------------------------------------------------
//                              GuiItemManager
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/06
//
/**
 * Declares GuiItemManager class. This class creates varisous GUI components.
 * It is a singleton - only one instance of this class can be created.
 */
//------------------------------------------------------------------------------

#ifndef GuiItemManager_hpp
#define GuiItemManager_hpp

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"

#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include "wx/notebook.h"
#include <wx/button.h>
#include <wx/grid.h>
#include "wx/radiobut.h"

class GuiItemManager
{
public:
    static GuiItemManager* GetInstance();

    static int GetNumSpacecraft()
        { return theNumSpacecraft; }
    
    static wxComboBox* GetSpacecraftComboBox(wxWindow *parent, int eventId);
    
    static void UpdateSpacecraft();
    
private:

    static void UpdateSpacecraftList(bool firstTime = false);
    
    static GuiItemManager *theInstance;
    static GuiInterpreter *theGuiInterpreter;
    
    static wxComboBox *theSpacecraftComboBox;

    static int theNumSpacecraft;
    static wxString *theSpacecraftList;
    
};

#endif // GuiItemManager_hpp
