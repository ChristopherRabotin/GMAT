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
   
   static GuiItemManager* GetInstance();
   
   wxString ToWxString(Real rval);
   int IsValidVariable(const std::string &varName, Gmat::ObjectType ownerType,
                       bool allowNumber = false, bool allowNonPlottable = false);
   
   void UpdateAll();
   void UpdateBurn();
   void UpdateCelestialPoint();
   void UpdateCoordSystem();
   void UpdateForceModel();
   void UpdateFormation();
   void UpdateFunction();
   void UpdateHardware();
   void UpdateParameter();
   void UpdatePropagator();
   void UpdateSolarSystem();
   void UpdateSpacecraft();
   void UpdateSolver();
   void UpdateSubscriber();
   
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
   int GetNumUserArray() { return theNumUserArray; }
   int GetNumUserString() { return theNumUserString; }
   int GetNumUserParameter() { return theNumUserParam; }
   int GetNumSolver() { return theNumSolver; }
   int GetNumBoundarySolver() { return theNumBoundarySolver; }
   int GetNumOptimizer() { return theNumOptimizer; }
   
   wxArrayString GetPlottableParameterList() { return thePlottableParamList; }
   wxArrayString GetSystemParameterList() { return theSystemParamList; }
   wxArrayString GetUserVariableList() { return theUserVariableList; }
   wxArrayString GetUserArrayList() { return theUserArrayList; }
   wxArrayString GetUserStringList() { return theUserStringList; }
   wxArrayString GetUserParameterList() { return theUserParamList; }
   wxArrayString GetCoordSysList() { return theCoordSysList; }
   wxArrayString GetConfigBodyList() { return theCelesBodyList; }
   wxArrayString GetSpacecraftList() { return theSpacecraftList; }
   wxArrayString GetImpulsiveBurnList() { return theImpBurnList; }
   wxArrayString GetFiniteBurnList() { return theFiniteBurnList; }
   wxArrayString GetSolverList() { return theSolverList; }
   wxArrayString GetOptimizerList() { return theOptimizerList; }
   
   wxArrayString GetPropertyList(const wxString &objType,
                                 int showOption = SHOW_PLOTTABLE);
   
   int GetNumProperty(const wxString &objType);
   
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
                                   wxArrayString *excList = NULL,
                                   bool multiSelect = false);
   
   wxListBox* GetFormationListBox(wxWindow *parent, wxWindowID id,
                                  const wxSize &size,
                                  wxArrayString &sosToExclude);
   
   wxListBox* GetImpBurnListBox(wxWindow *parent, wxWindowID id,
                                const wxSize &size,
                                wxArrayString *excList = NULL,
                                bool multiSelect = false);
   
   wxListBox* GetPropertyListBox(wxWindow *parent, wxWindowID id,
                                 const wxSize &size,
                                 const wxString &objType,
                                 int showOption = SHOW_PLOTTABLE,
                                 bool multiSelect = false);
   
   wxListBox* GetPlottableParameterListBox(wxWindow *parent, wxWindowID id,
                                           const wxSize &size, 
                                           const wxString &nameToExclude = "");
   
   wxListBox* GetAllUserParameterListBox(wxWindow *parent, wxWindowID id,
                                         const wxSize &size, bool showArray);
   
   wxListBox* GetUserVariableListBox(wxWindow *parent, wxWindowID id,
                                     const wxSize &size,
                                     const wxString &nameToExclude = "",
                                      bool multiSelect = false);
   
   wxListBox* GetUserStringListBox(wxWindow *parent, wxWindowID id,
                                   const wxSize &size,
                                   const wxString &nameToExclude = "",
                                   bool multiSelect = false);
   
   wxListBox* GetUserArrayListBox(wxWindow *parent, wxWindowID id,
                                  const wxSize &size,
                                  const wxString &nameToExclude = "",
                                  bool multiSelect = false);
   
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
   
   wxBoxSizer* CreateParameterSizer
         (wxWindow *parent,
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
   
   wxSizer* Create3ColParameterSizer
         (wxWindow *parent,
          wxCheckBox **entireObjCheckBox, wxWindowID entireObjCheckBoxId,
          wxComboBox **objectTypeComboBox, wxWindowID objectTypeComboBoxId,
          wxListBox **objectListBox, wxWindowID objectListBoxId,
          wxStaticText **rowStaticText, wxWindowID rowStaticTextId,
          wxStaticText **colStaticText, wxWindowID colStaticTextId,
          wxTextCtrl **rowTextCtrl, wxWindowID rowTextCtrlId,
          wxTextCtrl **colTextCtrl, wxWindowID colTextCtrlId,
          wxListBox **propertyListBox, wxWindowID propertyListBoxId,
          wxComboBox **coordSysComboBox, wxWindowID coordSysComboBoxId,
          wxComboBox **originComboBox, wxWindowID originComboBoxId,
          wxStaticText **coordSysLabel, wxBoxSizer **coordSysBoxSizer,
          wxButton **upButton, wxWindowID upButtonId,
          wxButton **downButton, wxWindowID downButtonId,
          wxButton **addButton, wxWindowID addButtonId,
          wxButton **removeButton, wxWindowID removeButtonId,
          wxButton **addAllButton, wxWindowID addAllButtonId,
          wxButton **removeAllButton, wxWindowID removeAllButtonId,
          wxListBox **selectedListBox, wxWindowID selectedListBoxId,
          const wxArrayString &objectTypeList, int showOption = SHOW_PLOTTABLE,
          bool allowMultiSelect = false, bool showString = false,
          bool allowWholeObject = false, bool showSysParam = true,
          bool showVariable = false, bool showArray = false,
          const wxString &onwer = "Spacecraft");
   
   wxSizer*
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
   void UpdatePropagatorList();
   void UpdateForceModelList();
   
   //void AddToAllObjectList();
   void AddToAllObjectArray();
   
   static GuiItemManager *theInstance;
   GuiInterpreter *theGuiInterpreter;
   SolarSystem *theSolarSystem;
   
   Integer theDataPrecision;
   
   // Register for resource change
   std::vector<GmatPanel*> mResourceUpdateListeners;
   
   std::vector<wxListBox*> mSpaceObjectLBList;
   std::vector<wxListBox*> mSpacecraftLBList;
   std::vector<wxListBox*> mImpBurnLBList;
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
   std::vector<wxArrayString*> mImpBurnExcList;
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
   int theNumForceModel;
   int theNumSpacecraft;
   int theNumCalPoint;
   int theNumCelesBody;
   int theNumCelesPoint;
   int theNumSpacePoint;
   int theNumImpBurn;
   int theNumFiniteBurn;
   int theNumCoordSys;
   int theNumFunction;
   int theNumFuelTank;
   int theNumThruster;
   int theNumSubscriber;
   int theNumReportFile;
   int theNumSolver;
   int theNumBoundarySolver;
   int theNumOptimizer;
   int theNumPropagator;
   int theNumPlottableParam;
   int theNumSystemParam;
   int theNumUserVariable;
   int theNumUserString;
   int theNumUserArray;
   int theNumUserParam;
   int theNumAllObject;
   
   // Spacecraft Properties
   wxArrayString theScPropertyList;
   
   // Burn Properties
   wxArrayString theImpBurnPropertyList;
   wxArrayString theFiniteBurnPropertyList;
   
   // All Objects
   wxArrayString theAllObjectList;
   
   // Propagator, ForceModel
   wxArrayString thePropagatorList;
   wxArrayString theForceModelList;
   
   // CoordinateSystem
   wxArrayString theCoordSysList;
   
   // Burn
   wxArrayString theImpBurnList;
   wxArrayString theFiniteBurnList;

   // Function
   wxArrayString theFunctionList;
   
   // Solver
   wxArrayString theSolverList;
   wxArrayString theBoundarySolverList;
   wxArrayString theOptimizerList;
   
   // SpacePoint
   wxArrayString theCelesBodyList;
   wxArrayString theCelesPointList;
   wxArrayString theCalPointList;
   wxArrayString theSpacePointList;
   wxArrayString theSpacecraftList;
   wxArrayString theSpaceObjectList;
   wxArrayString theFormationList;
   
   // Subscriber
   wxArrayString theSubscriberList;
   wxArrayString theReportFileList;
   
   // Hardware
   wxArrayString theFuelTankList;
   wxArrayString theThrusterList;
   
   // Parameter
   wxArrayString thePlottableParamList;
   wxArrayString theSystemParamList;
   wxArrayString theUserVariableList;
   wxArrayString theUserStringList;
   wxArrayString theUserArrayList;
   wxArrayString theUserParamList;
   
};

#endif // GuiItemManager_hpp
