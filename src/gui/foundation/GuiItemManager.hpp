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

    static int GetNumParameter()
        { return theNumParam; }
    
    static int GetNumConfigParameter()
        { return theNumConfigParam; }
    
    static wxString* GetParameterList()
        { return theParamList; }
    
    static wxString* GetConfigParameterList()
        { return theConfigParamList; }
        
    static wxComboBox* GetSpacecraftComboBox(wxWindow *parent, wxWindowID id,
                                             const wxSize &size);
    
    static wxListBox* GetParameterListBox(wxWindow *parent, const wxSize &size,
                                          const wxString &objName, int numObj);
    
    static wxListBox* GetConfigParameterListBox(wxWindow *parent, const wxSize &size);
    
    static void UpdateSpacecraft();
    static void UpdateParameter(const wxString &objName = "");
    static void UpdateConfigParameter(const wxString &objName = "");
    
private:

    static void UpdateSpacecraftList(bool firstTime = false);
    static void UpdateParameterList(const wxString &objName, bool firstTime = false);
    static void UpdateConfigParameterList(const wxString &objName, bool firstTime = false);
    
    static GuiItemManager *theInstance;
    static GuiInterpreter *theGuiInterpreter;
    
    static int theNumSpacecraft;
    static int theNumParam;
    static int theNumConfigParam;
    
    static wxString *theParamList;
    static wxString *theConfigParamList;
    static wxString *theSpacecraftList;
    
    static wxComboBox *theSpacecraftComboBox;
    static wxListBox *theParamListBox;
    static wxListBox *theConfigParamListBox;
    
};

#endif // GuiItemManager_hpp
