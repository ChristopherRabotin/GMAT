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

   // for SpacePoint
   static const int MAX_SPACE_POINT_SIZE = 60;  // CELES_POINT + SPACE_OBJECT
   static const int MAX_CELES_POINT_SIZE = 20;  // CELES_BODY + CAL_POINT
   static const int MAX_CELES_BODY_SIZE = 20;
   static const int MAX_CAL_POINT_SIZE = 20;
   static const int MAX_SPACE_OBJECT_SIZE = 40; // FORMATION + SPACECRAFT
   static const int MAX_FORMATION_SIZE = 10;
   static const int MAX_SPACECRAFT_SIZE = 30;

   // for Parameter
   static const int MAX_PROPERTY_SIZE = 100;
   static const int MAX_USER_VAR_SIZE = 20;
   static const int MAX_USER_STRING_SIZE = 20;
   static const int MAX_USER_ARRAY_SIZE = 20;
   static const int MAX_USER_PARAM_SIZE = 40;
   static const int MAX_PLOT_PARAM_SIZE = 140;

   // for CoordinateSystem
   static const int MAX_COORD_SYS_SIZE = 20;
   
   static GuiItemManager* GetInstance();
   
   void UpdateAll();
   void UpdateCelestialPoint();
   void UpdateFormation();
   void UpdateSpacecraft();
   void UpdateParameter();
   void UpdateSolarSystem();
   void UpdateCoordSystem();
   
   int GetNumSpacecraft()
      { return theNumSpacecraft; }
   
   int GetNumConfigBody()
      { return theNumCelesBody; }
   
   int GetNumCoordSystem()
      { return theNumCoordSys; }
   
   int GetNumPlottableParameter()
      { return theNumPlottableParam; }
   
   int GetNumSystemParameter()
      { return theNumSystemParam; }
   
   int GetNumUserVariable()
      { return theNumUserVariable; }
   
   int GetNumUserString()
      { return theNumUserString; }
   
   int GetNumUserArray()
      { return theNumUserArray; }
   
   int GetNumUserParameter()
      { return theNumUserParam; }
   
   wxString* GetPlottableParameterList()
      { return thePlottableParamList; }
   
   wxString* GetSystemParameterList()
      { return theSystemParamList; }
   
   wxString* GetUserVariableList()
      { return theUserVariableList; }
   
   wxString* GetUserStringList()
      { return theUserStringList; }
   
   wxString* GetUserParameterList()
      { return theUserParamList; }
   
   wxString* GetCoordSysList()
      { return theCoordSysList; }
   
   wxString* GetConfigBodyList()
      { return theCelesBodyList; }
   
   int GetNumProperty(const wxString &objName);
   wxString* GetPropertyList(const wxString &objName);

   // ComboBox
   wxComboBox* GetSpacecraftComboBox(wxWindow *parent, wxWindowID id,
                                     const wxSize &size);
   
   wxComboBox* GetCoordSysComboBox(wxWindow *parent, wxWindowID id,
                                   const wxSize &size);
   
   wxComboBox* GetConfigBodyComboBox(wxWindow *parent, wxWindowID id,
                                     const wxSize &size);
   
   wxComboBox* GetSpacePointComboBox(wxWindow *parent, wxWindowID id, 
                                     const wxSize &size, bool addVector = false);
   
   wxComboBox* GetCelestialPointComboBox(wxWindow *parent, wxWindowID id, 
                                         const wxSize &size, bool addVector = false);
   
   wxComboBox* GetUserVariableComboBox(wxWindow *parent, wxWindowID id,
                                       const wxSize &size);

   // ListBox
   wxListBox* GetSpacePointListBox(wxWindow *parent, wxWindowID id, 
                                   const wxSize &size, bool addVector = false);
   
   wxListBox* GetCelestialPointListBox(wxWindow *parent, wxWindowID id,
                                       const wxSize &size,
                                       wxArrayString &bodiesToExclude);
   
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
   
   wxListBox* GetAllUserParameterListBox(wxWindow *parent, wxWindowID id,
                                         const wxSize &size);
   
   wxListBox* GetUserVariableListBox(wxWindow *parent, wxWindowID id,
                                     const wxSize &size,
                                     const wxString &nameToExclude = "");
   
   wxListBox* GetUserStringListBox(wxWindow *parent, wxWindowID id,
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

   // BoxSizer
   wxBoxSizer*
   CreateParameterSizer(wxWindow *parent,
                        wxListBox **userParamListBox, wxWindowID userParamListBoxId,
                        wxButton **createVarButton, wxWindowID createVarButtonId,
                        wxComboBox **objectComboBox, wxWindowID objectComboBoxId,
                        wxListBox **propertyListBox, wxWindowID propertyListBoxId,
                        wxComboBox **coordSysComboBox, wxWindowID coordSysComboBoxId,
                        wxComboBox **originComboBox, wxWindowID originComboBoxId,
                        wxStaticText **coordSysLabel, wxBoxSizer **coordSysBoxSizer,
                        bool showArrayAndString = false);
   wxBoxSizer*
   CreateUserVarSizer(wxWindow *parent,
                      wxListBox **userParamListBox, wxWindowID userParamListBoxId,
                      wxButton **createVarButton, wxWindowID createVarButtonId,
                      bool showArrayAndString = false);
   
private:
   
   GuiItemManager();
   virtual ~GuiItemManager();
   GuiItemManager(const GuiItemManager&);
   GuiItemManager& operator=(const GuiItemManager&);
   
   void UpdatePropertyList(const wxString &objName);
   void UpdateParameterList();

   void UpdateSpaceObjectList();
   void UpdateFormationList();
   void UpdateSpacePointList();
   
   void UpdateCelestialBodyList();
   void UpdateCelestialPointList();
   void UpdateSpacecraftList();
   
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
   int theNumUserString;
   int theNumUserArray;
   int theNumUserParam;
   int theNumCelesBody;
   int theNumCelesPoint;
   int theNumCalPoint;
   int theNumSpacePoint;
   
   wxString theSpacePointList[MAX_SPACE_POINT_SIZE];
   wxString theCelesPointList[MAX_CELES_POINT_SIZE];
   wxString theCelesBodyList[MAX_CELES_BODY_SIZE];
   wxString theCalPointList[MAX_CAL_POINT_SIZE];
   
   wxString theSpaceObjectList[MAX_SPACE_OBJECT_SIZE];
   wxString theFormationList[MAX_FORMATION_SIZE];
   wxString theSpacecraftList[MAX_SPACECRAFT_SIZE];
   
   wxString theCoordSysList[MAX_COORD_SYS_SIZE];
   
   wxString theScPropertyList[MAX_PROPERTY_SIZE];
   wxString thePlottableParamList[MAX_PLOT_PARAM_SIZE];
   wxString theSystemParamList[MAX_PROPERTY_SIZE];
   wxString theUserVariableList[MAX_USER_VAR_SIZE];
   wxString theUserStringList[MAX_USER_STRING_SIZE];
   wxString theUserArrayList[MAX_USER_ARRAY_SIZE];
   wxString theUserParamList[MAX_USER_PARAM_SIZE];
   
   wxComboBox *theSpacecraftComboBox;
   wxComboBox *theUserParamComboBox;
   wxComboBox *theCoordSysComboBox;
   wxComboBox *theCelesBodyComboBox;
   wxComboBox *theSpacePointComboBox;
   wxComboBox *theCelestialPointComboBox;
   
   wxListBox  *theSpacePointListBox;
   wxListBox  *theCelesBodyListBox;
   wxListBox  *theCelesPointListBox;
   wxListBox  *theSpaceObjectListBox;
   wxListBox  *theFormationListBox;
   wxListBox  *theSpacecraftListBox;
   
   wxListBox  *theScPropertyListBox;
   wxListBox  *thePlottableParamListBox;
   wxListBox  *theSystemParamListBox;
   wxListBox  *theAllUserParamListBox;
   wxListBox  *theUserVariableListBox;
   wxListBox  *theUserStringListBox;
   wxListBox  *theUserArrayListBox;
   wxListBox  *theUserParamListBox;

   wxBoxSizer *theParamBoxSizer;
};

#endif // GuiItemManager_hpp
