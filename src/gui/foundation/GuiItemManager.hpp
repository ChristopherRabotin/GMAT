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
#include <wx/notebook.h>
#include <wx/button.h>
#include <wx/grid.h>
#include <wx/radiobut.h>
#include <wx/string.h>

class GuiItemManager
{
public:
   static const int MAX_LIST_SIZE = 30;
   static GuiItemManager* GetInstance();
   
   void UpdateAll();
   void UpdateObject();
   void UpdateSpacecraft();
   void UpdateParameter(const wxString &objName = "");
   void UpdateSolarSystem();
   
   int GetNumSpacecraft()
      { return theNumSpacecraft; }
   
   int GetNumParameter()
      { return theNumParam; }
   
   int GetNumConfigParameter()
      { return theNumConfigParam; }
    
   int GetNumConfigBody()
      { return theNumConfigBody; }
   
   wxString* GetParameterList()
      { return theParamList; }
   
   wxString* GetConfigParameterList()
      { return theConfigParamList; }
   
   wxComboBox* GetSpacecraftComboBox(wxWindow *parent, wxWindowID id,
                                     const wxSize &size);
   
   wxListBox* GetSpacecraftListBox(wxWindow *parent, wxWindowID id,
                                   const wxSize &size,
                                   wxArrayString &scsToExclude);
   
   wxListBox* GetObjectListBox(wxWindow *parent, wxWindowID id,
                               const wxSize &size);
   
   wxListBox* GetParameterListBox(wxWindow *parent, wxWindowID id,
                                  const wxSize &size,
                                  const wxString &objName, int numObj);
   
   wxListBox* GetConfigParameterListBox(wxWindow *parent, wxWindowID id,
                                        const wxSize &size,
                                        const wxString &nameToExclude = "");
   
   wxListBox* GetConfigBodyListBox(wxWindow *parent, wxWindowID id,
                                   const wxSize &size,
                                   wxArrayString &bodiesToExclude);
   
   
private:
   
   GuiItemManager();
   virtual ~GuiItemManager();
   GuiItemManager(const GuiItemManager&);
   GuiItemManager& operator=(const GuiItemManager&);
   
   void UpdateObjectList(bool firstTime = false);
   void UpdateSpacecraftList(bool firstTime = false);
   void UpdateParameterList(const wxString &objName, bool firstTime = false);
   void UpdateConfigParameterList(const wxString &objName, bool firstTime = false);
   void UpdateConfigUserVarList(const wxString &objName, bool firstTime = false);
   void UpdateConfigBodyList(bool firstTime = false);
   
   static GuiItemManager *theInstance;
   GuiInterpreter *theGuiInterpreter;
   SolarSystem *theSolarSystem;
   
   int theNumObject;
   int theNumSpacecraft;
   int theNumParam;
   int theNumConfigParam;
   int theNumConfigBody;
   
   wxString theObjectList[MAX_LIST_SIZE];
   wxString theSpacecraftList[MAX_LIST_SIZE];
   wxString theParamList[MAX_LIST_SIZE];
   wxString theConfigParamList[MAX_LIST_SIZE];
   wxString theConfigBodyList[MAX_LIST_SIZE];
   
   wxComboBox *theSpacecraftComboBox;
   wxListBox *theSpacecraftListBox;
   wxListBox *theObjectListBox;
   wxListBox *theParamListBox;
   wxListBox *theConfigParamListBox;
   wxListBox *theConfigBodyListBox;
   
};

#endif // GuiItemManager_hpp
