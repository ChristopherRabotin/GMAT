//$Id$
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

class GmatPanel; // forward declaration

class GuiItemManager
{
public:

   enum ShowParamOption
   {
      SHOW_REPORTABLE, // Real, Array, String
      SHOW_PLOTTABLE,  // Real
      SHOW_SETTABLE,   // for Vary command
   };

   //@Note Should we use dynamic array instead of fixed since wxWidgets 2.6.1 or
   // later supports wxArrayString as choices in the wxComboBox or wxListBox?
   
   // for SpacePoint
   static const int MAX_SPACE_POINT = 60;  // CELES_POINT + SPACE_OBJECT
   static const int MAX_CELES_POINT = 20;  // CELES_BODY + CAL_POINT
   static const int MAX_CELES_BODY = 20;
   static const int MAX_CAL_POINT = 20;
   static const int MAX_SPACE_OBJECT = 40; // FORMATION + SPACECRAFT
   static const int MAX_FORMATION = 10;
   static const int MAX_SPACECRAFT = 30;
   static const int MAX_BURN = 40;
   static const int MAX_ALL_OBJECT = 70;   // MAX_SPACE_OBJECT + MAX_BURN
   
   // for Parameter
   static const int MAX_SC_PROPERTY = 100; // Max Spacecraft
   static const int MAX_IB_PROPERTY = 20;  // Max ImpulsiveBurn
   static const int MAX_PROPERTY = 110;
   static const int MAX_USER_VAR = 100;
   static const int MAX_USER_STRING = 50;
   static const int MAX_USER_ARRAY = 100;
   static const int MAX_USER_PARAM = 250;  // MAX_USER_VAR + MAX_USER_STRING + MAX_USER_ARRAY
   static const int MAX_PLOT_PARAM = 250;

   // Other
   static const int MAX_COORD_SYS = 60;
   static const int MAX_HARDWARE = 60;
   static const int MAX_FUNCTION = 20;
   static const int MAX_SOLVER = 40;      // MAX_BOUNDARY_SOLVER + MAX_OPTIMIZER
   static const int MAX_BOUNDARY_SOLVER = 20;
   static const int MAX_OPTIMIZER = 20;
   static const int MAX_REPORT_FILE = 20;
   static const int MAX_SUBSCRIBER = 40;   // MAX_REPORT_FILE + 20
   
   static GuiItemManager* GetInstance();
   
   wxString ToWxString(Real rval);
   int IsValidVariable(const std::string &varName, Gmat::ObjectType ownerType,
                       bool allowNumber = false, bool allowNonPlottable = false);
   
   void UpdateAll();
   void UpdateCelestialPoint();
   void UpdateFormation();
   void UpdateSpacecraft();
   void UpdateBurn();
   void UpdateParameter();
   void UpdateSolarSystem();
   void UpdateCoordSystem();
   void UpdateHardware();
   void UpdateFunction();
   void UpdateSubscriber();
   void UpdateSolver();
   
   // For handling of resource update 
   void AddToResourceUpdateListeners(GmatPanel *panel);
   void RemoveFromResourceUpdateListeners(GmatPanel *panel);
   bool PrepareObjectNameChange();
   void NotifyObjectNameChange(Gmat::ObjectType type,
                               const wxString &oldName,
                               const wxString &newName);
   void UnregisterListBox(const wxString &type, wxListBox *lb,
                          wxArrayString *excList = NULL);
   void UnregisterCheckListBox(const wxString &type, wxCheckListBox *clb,
                               wxArrayString *excList = NULL);
   void UnregisterComboBox(const wxString &type, wxComboBox *cb);
   
   // For resource counters
   int GetNumSpacecraft() { return theNumSpacecraft; }
   int GetNumImpulsiveBurn() { return theNumImpBurn; }
   int GetNumFiniteBurn() { return theNumFiniteBurn; }
   int GetNumFuelTank() { return theNumFuelTank; }
   int GetNumThruster() { return theNumThruster; }
   int GetNumConfigBody() { return theNumCelesBody; }
   int GetNumCoordSystem() { return theNumCoordSys; }
   int GetNumFunction() { return theNumFunction; }
   int GetNumPlottableParameter() { return theNumPlottableParam; }
   int GetNumSystemParameter() { return theNumSystemParam; }
   int GetNumUserVariable() { return theNumUserVariable; }
   int GetNumUserString() { return theNumUserString; }
   int GetNumUserArray() { return theNumUserArray; }
   int GetNumUserParameter() { return theNumUserParam; }
   int GetNumSolver() { return theNumSolver; }
   int GetNumBoundarySolver() { return theNumBoundarySolver; }
   int GetNumOptimizer() { return theNumOptimizer; }
   
   wxString* GetPlottableParameterList() { return thePlottableParamList; }
   wxString* GetSystemParameterList() { return theSystemParamList; }
   wxString* GetUserVariableList() { return theUserVariableList; }
   wxString* GetUserStringList() { return theUserStringList; }
   wxString* GetUserParameterList() { return theUserParamList; }
   wxString* GetCoordSysList() { return theCoordSysList; }
   wxString* GetConfigBodyList() { return theCelesBodyList; }
   wxString* GetSpacecraftList() { return theSpacecraftList; }
   wxString* GetImpulsiveBurnList() { return theImpBurnList; }
   wxString* GetFiniteBurnList() { return theFiniteBurnList; }
   wxString* GetSolverList() { return theSolverList; }
   wxString* GetOptimizerList() { return theOptimizerList; }
   
   //wxArrayString GetSettablePropertyList(const wxString &objType);
   wxArrayString GetPropertyList(const wxString &objType,
                                 int showOption = SHOW_PLOTTABLE);
   
   int GetNumProperty(const wxString &objType);
   wxString* GetPropertyList(const wxString &objType);

   //-----------------------------------------------------------------
   //Note: To enables automatic updates when new object is added to
   //      resrouce, the following function automatically registers
   //      components. If caller calls any of the following methods
   //      to get a component, the caller needs to unregister
   //      components by calling appropriate Unregister*() methods
   //      in the caller's destructor, such as UnregisterListBox(),
   //      UnregisterCheckListBox(), UnregisterComboBox().
   //-----------------------------------------------------------------

   // ComboBox
   wxComboBox* GetObjectTypeComboBox(wxWindow *parent, wxWindowID id,
                                     const wxSize &size,
                                     const wxArrayString objectTypeList);
   
   wxComboBox* GetSpacecraftComboBox(wxWindow *parent, wxWindowID id,
                                     const wxSize &size);
   
   wxComboBox* GetImpBurnComboBox(wxWindow *parent, wxWindowID id,
                                  const wxSize &size);
   
   wxComboBox* GetFiniteBurnComboBox(wxWindow *parent, wxWindowID id,
                                     const wxSize &size);
   
   wxComboBox* GetCoordSysComboBox(wxWindow *parent, wxWindowID id,
                                   const wxSize &size);
   
   wxComboBox* GetConfigBodyComboBox(wxWindow *parent, wxWindowID id,
                                     const wxSize &size);
   
   wxComboBox* GetFunctionComboBox(wxWindow *parent, wxWindowID id,
                                   const wxSize &size);

   wxComboBox* GetSpacePointComboBox(wxWindow *parent, wxWindowID id, 
                                     const wxSize &size, bool addVector = false);
   
   wxComboBox* GetCelestialPointComboBox(wxWindow *parent, wxWindowID id, 
                                         const wxSize &size, bool addVector = false);
   
   wxComboBox* GetUserVariableComboBox(wxWindow *parent, wxWindowID id,
                                       const wxSize &size);
   
   wxComboBox* GetFuelTankComboBox(wxWindow *parent, wxWindowID id,
                                   const wxSize &size);
   
   wxComboBox* GetThrusterComboBox(wxWindow *parent, wxWindowID id,
                                   const wxSize &size);
   
   wxComboBox* GetSubscriberComboBox(wxWindow *parent, wxWindowID id,
                                     const wxSize &size);
   wxComboBox* GetReportFileComboBox(wxWindow *parent, wxWindowID id,
                                     const wxSize &size);
   wxComboBox* GetSolverComboBox(wxWindow *parent, wxWindowID id,
                                     const wxSize &size);
   wxComboBox* GetBoundarySolverComboBox(wxWindow *parent, wxWindowID id,
                                         const wxSize &size);
   wxComboBox* GetOptimizerComboBox(wxWindow *parent, wxWindowID id,
                                     const wxSize &size);

   
   // CheckListBox
   wxCheckListBox* GetSubscriberCheckListBox(wxWindow *parent, wxWindowID id,
                                             const wxSize &size,
                                             wxArrayString *excList = NULL);
   wxCheckListBox* GetSpacecraftCheckListBox(wxWindow *parent, wxWindowID id,
                                             const wxSize &size,
                                             wxArrayString *excList = NULL);
   
   
   wxCheckListBox* GetAllObjectCheckListBox(wxWindow *parent, wxWindowID id,
                                            const wxSize &size,
                                            wxArrayString *excList = NULL);
   
   // ListBox
   wxListBox* GetSpacePointListBox(wxWindow *parent, wxWindowID id, 
                                   const wxSize &size, bool addVector = false);
   
   wxListBox* GetCelestialPointListBox(wxWindow *parent, wxWindowID id,
                                       const wxSize &size,
                                       wxArrayString &excList);
   
   wxListBox* GetSpaceObjectListBox(wxWindow *parent, wxWindowID id,
                                    const wxSize &size,
                                    wxArrayString *excList = NULL,
                                    bool includeFormation = true);
   
   wxListBox* GetSpacecraftListBox(wxWindow *parent, wxWindowID id,
                                   const wxSize &size,
                                   wxArrayString *excList = NULL);
   
   wxListBox* GetFormationListBox(wxWindow *parent, wxWindowID id,
                                  const wxSize &size,
                                  wxArrayString &sosToExclude);
   
   wxListBox* GetPropertyListBox(wxWindow *parent, wxWindowID id,
                                 const wxSize &size,
                                 const wxString &objType,
                                 int showOption = SHOW_PLOTTABLE);
   
   wxListBox* GetPlottableParameterListBox(wxWindow *parent, wxWindowID id,
                                           const wxSize &size,
                                           const wxString &nameToExclude = "");
   
   wxListBox* GetAllUserParameterListBox(wxWindow *parent, wxWindowID id,
                                         const wxSize &size, bool showArray);
   
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
                                   wxArrayString &excList);
   wxListBox* GetFuelTankListBox(wxWindow *parent, wxWindowID id,
                                 const wxSize &size,
                                 wxArrayString *excList = NULL);
   wxListBox* GetThrusterListBox(wxWindow *parent, wxWindowID id,
                                 const wxSize &size,
                                 wxArrayString *excList);
   
   wxBoxSizer*
   CreateParameterSizer(wxWindow *parent,
                        wxListBox **userParamListBox, wxWindowID userParamListBoxId,
                        wxButton **createVarButton, wxWindowID createVarButtonId,
                        wxComboBox **objectTypeComboBox, wxWindowID objectTypeComboBoxId,
                        wxComboBox **spacecraftComboBox, wxWindowID spacecraftComboBoxId,
                        wxComboBox **impBurnComboBox, wxWindowID impBurnComboBoxId,
                        wxListBox **propertyListBox, wxWindowID propertyListBoxId,
                        wxComboBox **coordSysComboBox, wxWindowID coordSysComboBoxId,
                        wxComboBox **originComboBox, wxWindowID originComboBoxId,
                        wxStaticText **coordSysLabel, wxBoxSizer **coordSysBoxSizer,
                        const wxArrayString &objectTypeList,
                        int showOption = SHOW_PLOTTABLE, bool showVariable = true,
                        bool showArray = false, const wxString &onwer = "Spacecraft");
   
   wxBoxSizer*
   CreateUserVarSizer(wxWindow *parent,
                      wxListBox **userParamListBox, wxWindowID userParamListBoxId,
                      wxButton **createVarButton, wxWindowID createVarButtonId,
                      int showOption = SHOW_REPORTABLE, bool showArray = false);
   
private:
   
   GuiItemManager();
   virtual ~GuiItemManager();
   GuiItemManager(const GuiItemManager&);
   GuiItemManager& operator=(const GuiItemManager&);
   
   void UpdatePropertyList();
   void UpdateParameterList();

   void UpdateSpacecraftList();
   void UpdateFormationList();
   void UpdateSpaceObjectList();
   void UpdateCelestialBodyList();
   void UpdateCelestialPointList();
   void UpdateSpacePointList();
   void UpdateBurnList();
   void UpdateCoordSystemList();
   void UpdateHardwareList();
   void UpdateFunctionList();
   void UpdateSubscriberList();
   void UpdateSolverList();

   void AddToAllObjectList();
   
   static GuiItemManager *theInstance;
   GuiInterpreter *theGuiInterpreter;
   SolarSystem *theSolarSystem;
   
   Integer theDataPrecision;
   
   // Register for resource change
   std::vector<GmatPanel*> mResourceUpdateListeners;
   
   std::vector<wxListBox*> mSpaceObjectLBList;
   std::vector<wxListBox*> mSpacecraftLBList;
   std::vector<wxCheckListBox*> mSubscriberCLBList;
   std::vector<wxCheckListBox*> mSpacecraftCLBList;
   std::vector<wxCheckListBox*> mAllObjectCLBList;
   std::vector<wxListBox*> mFuelTankLBList;
   std::vector<wxListBox*> mThrusterLBList;
   
   std::vector<wxComboBox*> mSpacePointCBList;
   std::vector<wxComboBox*> mSpacecraftCBList;
   std::vector<wxComboBox*> mImpBurnCBList;
   std::vector<wxComboBox*> mFiniteBurnCBList;
   std::vector<wxComboBox*> mCoordSysCBList;
   std::vector<wxComboBox*> mFunctionCBList;
   std::vector<wxComboBox*> mFuelTankCBList;
   std::vector<wxComboBox*> mThrusterCBList;
   std::vector<wxComboBox*> mSubscriberCBList;
   std::vector<wxComboBox*> mReportFileCBList;
   std::vector<wxComboBox*> mSolverCBList;
   std::vector<wxComboBox*> mBoundarySolverCBList;
   std::vector<wxComboBox*> mOptimizerCBList;
   
   std::vector<wxArrayString*> mSpaceObjectExcList;
   std::vector<wxArrayString*> mSpacecraftExcList;
   std::vector<wxArrayString*> mAllObjectExcList;
   std::vector<wxArrayString*> mFuelTankExcList;
   std::vector<wxArrayString*> mThrusterExcList;
   std::vector<wxArrayString*> mSubscriberExcList;
   std::vector<wxArrayString*> mReportFileExcList;
   
   int theNumScProperty;
   int theNumImpBurnProperty;
   int theNumFiniteBurnProperty;
   int theNumSpaceObject;
   int theNumFormation;
   int theNumSpacecraft;
   int theNumImpBurn;
   int theNumFiniteBurn;
   int theNumCoordSys;
   int theNumFunction;
   int theNumFuelTank;
   int theNumThruster;
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
   int theNumSubscriber;
   int theNumReportFile;
   int theNumSolver;
   int theNumBoundarySolver;
   int theNumOptimizer;
   int theNumAllObject;
   
   wxString theSpacePointList[MAX_SPACE_POINT];
   wxString theCelesPointList[MAX_CELES_POINT];
   wxString theCelesBodyList[MAX_CELES_BODY];
   wxString theCalPointList[MAX_CAL_POINT];
   
   wxString theSpaceObjectList[MAX_SPACE_OBJECT];
   wxString theFormationList[MAX_FORMATION];
   wxString theSpacecraftList[MAX_SPACECRAFT];
   wxString theImpBurnList[MAX_BURN];
   wxString theFiniteBurnList[MAX_BURN];
   wxString theCoordSysList[MAX_COORD_SYS];
   wxString theFunctionList[MAX_FUNCTION];
   wxString theFuelTankList[MAX_HARDWARE];
   wxString theThrusterList[MAX_HARDWARE];
   wxString theReportFileList[MAX_REPORT_FILE];
   wxString theSubscriberList[MAX_SUBSCRIBER];
   wxString theSolverList[MAX_SOLVER];
   wxString theBoundarySolverList[MAX_BOUNDARY_SOLVER];
   wxString theOptimizerList[MAX_OPTIMIZER];
   wxString theAllObjectList[MAX_ALL_OBJECT];
   
   wxString theScPropertyList[MAX_SC_PROPERTY];
   wxString theImpBurnPropertyList[MAX_IB_PROPERTY];
   wxString theFiniteBurnPropertyList[MAX_IB_PROPERTY];
   wxString thePlottableParamList[MAX_PLOT_PARAM];
   wxString theSystemParamList[MAX_PROPERTY];
   wxString theUserVariableList[MAX_USER_VAR];
   wxString theUserStringList[MAX_USER_STRING];
   wxString theUserArrayList[MAX_USER_ARRAY];
   wxString theUserParamList[MAX_USER_PARAM];
   
};

#endif // GuiItemManager_hpp
