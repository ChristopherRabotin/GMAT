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
   static const int MAX_OBJECT_SIZE = 20;
   static const int MAX_FORMATION_SIZE = 10;
   static const int MAX_SPACECRAFT_SIZE = 30;
   static const int MAX_PROPERTY_SIZE = 100;
   static const int MAX_USER_VAR_SIZE = 20;
   static const int MAX_USER_ARRAY_SIZE = 20;
   static const int MAX_USER_PARAM_SIZE = 40;
   static const int MAX_PLOT_PARAM_SIZE = 140;
   
   static GuiItemManager* GetInstance();
   
   void UpdateAll();
   void UpdateSpaceObject();
   void UpdateFormation();
   void UpdateSpacecraft();
   void UpdateParameter();
   void UpdateSolarSystem();
   void UpdateCoordSystem();
   
   int GetNumSpacecraft()
      { return theNumSpacecraft; }
   
   int GetNumCoordSystem()
      { return theNumCoordSys; }
   
   int GetNumPlottableParameter()
      { return theNumPlottableParam; }
   
   int GetNumSystemParameter()
      { return theNumSystemParam; }
   
   int GetNumUserVariable()
      { return theNumUserVariable; }
   
   int GetNumUserArray()
      { return theNumUserArray; }
   
   int GetNumUserParameter()
      { return theNumUserParam; }
   
   int GetNumConfigBody()
      { return theNumConfigBody; }
   
   wxString* GetPlottableParameterList()
      { return thePlottableParamList; }
   
   wxString* GetSystemParameterList()
      { return theSystemParamList; }
   
   wxString* GetUserVariableList()
      { return theUserVarList; }
   
   wxString* GetUserParameterList() //loj: 11/8/04 added
      { return theUserParamList; }
   
   int GetNumProperty(const wxString &objName);
   wxString* GetPropertyList(const wxString &objName);
   
   wxComboBox* GetSpacecraftComboBox(wxWindow *parent, wxWindowID id,
                                     const wxSize &size);
   
   wxComboBox* GetCoordSysComboBox(wxWindow *parent, wxWindowID id,
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
   
   wxListBox* GetPropertyListBox(wxWindow *parent, wxWindowID id,
                                 const wxSize &size,
                                 const wxString &objName);
   
   wxListBox* GetPlottableParameterListBox(wxWindow *parent, wxWindowID id,
                                        const wxSize &size,
                                        const wxString &nameToExclude = "");
   
   wxComboBox* GetUserVariableComboBox(wxWindow *parent, wxWindowID id,
                                       const wxSize &size);
   
   wxListBox* GetUserVariableListBox(wxWindow *parent, wxWindowID id,
                                     const wxSize &size,
                                     const wxString &nameToExclude = "");
   
   wxListBox* GetUserArrayListBox(wxWindow *parent, wxWindowID id,
                                  const wxSize &size,
                                  const wxString &nameToExclude = "");
   
   wxListBox* GetUserParameterListBox(wxWindow *parent, wxWindowID id,
                                      const wxSize &size);
   
   wxListBox* GetConfigBodyListBox(wxWindow *parent, wxWindowID id,
                                   const wxSize &size,
                                   wxArrayString &bodiesToExclude);
   
   wxBoxSizer* CreateParameterSizer(wxWindow *parent,
                                    wxButton **createVarButton,
                                    wxWindowID createVarButtonId,
                                    wxComboBox **objectComboBox,
                                    wxWindowID objectComboBoxId,
                                    wxListBox **userParamListBox,
                                    wxWindowID userParamListBoxId,
                                    wxListBox **propertyListBox,
                                    wxWindowID propertyListBoxId,
                                    wxComboBox **coordSysComboBox,
                                    wxWindowID coordSysComboBoxId,
                                    wxStaticText **coordSysLabel);
   
private:
   
   GuiItemManager();
   virtual ~GuiItemManager();
   GuiItemManager(const GuiItemManager&);
   GuiItemManager& operator=(const GuiItemManager&);
   
   void UpdatePropertyList(const wxString &objName);
   
   void UpdateSpaceObjectList();
   void UpdateFormationList();
   void UpdateSpacecraftList();
   void UpdateParameterList();
   void UpdateConfigBodyList();
   void UpdateCoordSystemList();
   
   static GuiItemManager *theInstance;
   GuiInterpreter *theGuiInterpreter;
   SolarSystem *theSolarSystem;
   
   int theNumScProperty;
   int theNumSpaceObject;
   int theNumFormation;
   int theNumSpacecraft;
   int theNumCoordSys;
   int theNumPlottableParam;
   int theNumSystemParam;
   int theNumUserVariable;
   int theNumUserArray;
   int theNumUserParam;
   int theNumConfigBody;
   
   wxString theSpaceObjectList[MAX_OBJECT_SIZE];
   wxString theFormationList[MAX_FORMATION_SIZE];
   wxString theSpacecraftList[MAX_OBJECT_SIZE];
   wxString theConfigBodyList[MAX_OBJECT_SIZE];
   wxString theCoordSysList[MAX_OBJECT_SIZE];
   
   wxString theScPropertyList[MAX_PROPERTY_SIZE];
   wxString thePlottableParamList[MAX_PLOT_PARAM_SIZE];
   wxString theSystemParamList[MAX_PROPERTY_SIZE];
   wxString theUserVarList[MAX_USER_VAR_SIZE];
   wxString theUserArrayList[MAX_USER_ARRAY_SIZE];
   wxString theUserParamList[MAX_USER_PARAM_SIZE];
   
   wxComboBox *theSpacecraftComboBox;
   wxComboBox *theUserParamComboBox;
   wxComboBox *theCoordSysComboBox;
   wxListBox  *theSpaceObjectListBox;
   wxListBox  *theFormationListBox;
   wxListBox  *theSpacecraftListBox;
   wxListBox  *theScPropertyListBox;
   wxListBox  *thePlottableParamListBox;
   wxListBox  *theSystemParamListBox;
   wxListBox  *theUserVarListBox;
   wxListBox  *theUserArrayListBox;
   wxListBox  *theUserParamListBox;
   wxListBox  *theConfigBodyListBox;

   wxBoxSizer *theParamBoxSizer;
};

#endif // GuiItemManager_hpp
