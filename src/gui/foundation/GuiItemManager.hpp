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
   static const int MAX_OBJECT_SIZE = 30;
   static const int MAX_PARAM_SIZE = 150;
   
   static GuiItemManager* GetInstance();
   
   void UpdateAll();
   void UpdateSpaceObject();
   void UpdateFormation();
   void UpdateSpacecraft();
   void UpdateParameter(const wxString &objName = "");
   void UpdateSolarSystem();
   
   int GetNumSpacecraft()
      { return theNumSpacecraft; }
   
   int GetNumParameter()
      { return theNumParam; }
   
   int GetNumConfigParameter()
      { return theNumConfigParam; }
    
   int GetNumSystemParameter()
      { return theNumSystemParam; }
    
   int GetNumUserParameter()
      { return theNumUserParam; }
    
   int GetNumConfigBody()
      { return theNumConfigBody; }
   
   wxString* GetParameterList()
      { return theParamList; }
   
   wxString* GetConfigParameterList()
      { return theConfigParamList; }
   
   wxString* GetSystemParameterList()
      { return theSystemParamList; }
   
   wxString* GetUserParameterList()
      { return theUserParamList; }
   
   wxComboBox* GetSpacecraftComboBox(wxWindow *parent, wxWindowID id,
                                     const wxSize &size);
   
   wxListBox* GetSpaceObjectListBox(wxWindow *parent, wxWindowID id,
                                    const wxSize &size,
                                    wxArrayString &namesToExclude);
   
   wxListBox* GetSpacecraftListBox(wxWindow *parent, wxWindowID id,
                                   const wxSize &size,
                                   wxArrayString &namesToExclude);
   
   wxListBox* GetFormationListBox(wxWindow *parent, wxWindowID id,
                                  const wxSize &size,
                                  wxArrayString &sosToExclude);
   
   wxListBox* GetParameterListBox(wxWindow *parent, wxWindowID id,
                                  const wxSize &size,
                                  const wxString &objName);
   
   wxListBox* GetConfigParameterListBox(wxWindow *parent, wxWindowID id,
                                        const wxSize &size,
                                        const wxString &nameToExclude = "");
   
   wxComboBox* GetUserParameterComboBox(wxWindow *parent, wxWindowID id,
                                        const wxSize &size);
   
   wxListBox* GetUserParameterListBox(wxWindow *parent, wxWindowID id,
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
   
   void UpdateSpaceObjectList(bool firstTime = false);
   void UpdateFormationList(bool firstTime = false);
   void UpdateSpacecraftList(bool firstTime = false);
   void UpdateParameterList(const wxString &objName, bool firstTime = false);
   void UpdateConfigParameterList(const wxString &objName, bool firstTime = false);
   void UpdateConfigBodyList(bool firstTime = false);
   
   static GuiItemManager *theInstance;
   GuiInterpreter *theGuiInterpreter;
   SolarSystem *theSolarSystem;
   
   int theNumSpaceObject;
   int theNumFormation;
   int theNumSpacecraft;
   int theNumParam;
   int theNumConfigParam;
   int theNumSystemParam;
   int theNumUserParam;
   int theNumConfigBody;
   
   wxString theSpaceObjectList[MAX_OBJECT_SIZE];
   wxString theFormationList[MAX_OBJECT_SIZE];
   wxString theSpacecraftList[MAX_OBJECT_SIZE];
   wxString theParamList[MAX_PARAM_SIZE];
   wxString theConfigParamList[MAX_PARAM_SIZE];
   wxString theSystemParamList[MAX_PARAM_SIZE];
   wxString theUserParamList[MAX_PARAM_SIZE];
   wxString theConfigBodyList[MAX_OBJECT_SIZE];
   
   wxComboBox *theSpacecraftComboBox;
   wxComboBox *theUserParamComboBox;
   wxListBox  *theSpaceObjectListBox;
   wxListBox  *theFormationListBox;
   wxListBox  *theSpacecraftListBox;
   wxListBox  *theParamListBox;
   wxListBox  *theConfigParamListBox;
   wxListBox  *theUserParamListBox;
   wxListBox  *theConfigBodyListBox;
   
};

#endif // GuiItemManager_hpp
