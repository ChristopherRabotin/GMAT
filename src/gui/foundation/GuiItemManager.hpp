//$Id$
//------------------------------------------------------------------------------
//                              GuiItemManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
      SHOW_REPORTABLE,         // Real, Array, String
      SHOW_PLOTTABLE,          // Real
      SHOW_SETTABLE,           // for Vary command
      SHOW_WHOLE_OBJECT_ONLY,
   };
   
   static GuiItemManager* GetInstance();
   
   void LoadIcon(const wxString &filename, long bitmapType, wxBitmap **bitmap,
                 const char* xpm[]);
   
   int  GetGuiStatus(); // 1 = clean, 2 = dirty, 3 = error
   void SetGuiStatus(int status);
   int  GetActiveScriptStatus();
   void SetActiveScriptStatus(int status);
   
   wxString ToWxString(Real rval);
   wxString ToWxString(Integer ival);
   wxArrayString ToWxArrayString(const StringArray &array);
   int IsValidVariable(const std::string &varName, Gmat::ObjectType allowedType,
                       bool allowNumber = false, bool allowNonPlottable = false);
   
   void UpdateAll(Gmat::ObjectType objType = Gmat::UNKNOWN_OBJECT);
   void UpdateGroundStation(bool updateObjectArray = true);
   void UpdateBurn(bool updateObjectArray = true);
   void UpdateCelestialPoint(bool updateObjectArray = true);
   void UpdateCoordSystem(bool updateObjectArray = true);
   void UpdateForceModel(bool updateObjectArray = true);
   void UpdateFormation(bool updateObjectArray = true);
   void UpdateFunction(bool updateObjectArray = true);
   void UpdateFuelTank(bool updateObjectArray = true);
   void UpdateThruster(bool updateObjectArray = true);
   void UpdateSensor(bool updateObjectArray = true);
   void UpdateParameter(bool updateObjectArray = true);
   void UpdatePropagator(bool updateObjectArray = true);
   void UpdateSolarSystem(bool updateObjectArray = true);
   void UpdateSpacecraft(bool updateObjectArray = true);
   void UpdateSpacePoint(bool updateObjectArray = true);
   void UpdateSolver(bool updateObjectArray = true);
   void UpdateSubscriber(bool updateObjectArray = true);
   
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
   int GetNumGroundStation() { return theNumGroundStation; }
   int GetNumSpacecraft() { return theNumSpacecraft; }
   int GetNumImpulsiveBurn() { return theNumImpBurn; }
   int GetNumFiniteBurn() { return theNumFiniteBurn; }
   int GetNumFuelTank() { return theNumFuelTank; }
   int GetNumThruster() { return theNumThruster; }
   int GetNumSensor() { return theNumSensor; }
   int GetNumAntenna() { return theNumAntenna; }
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
   wxArrayString GetConfigBodyList() { return theCelestialBodyList; }
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
   
   // This method will be deprecated in the future
   wxComboBox* GetCoordSysComboBox(wxWindow *parent, wxWindowID id,
                                   const wxSize &size);
   
   wxComboBox* GetCoordSystemComboBox(wxWindow *parent, wxWindowID id,
                                      const wxSize &size);
   
   wxComboBox* GetCelestialBodyComboBox(wxWindow *parent, wxWindowID id,
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
   wxComboBox* GetSensorComboBox(wxWindow *parent, wxWindowID id,
                                 const wxSize &size);
   wxComboBox* GetAntennaComboBox(wxWindow *parent, wxWindowID id,
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
   wxCheckListBox* GetXyPlotCheckListBox(wxWindow *parent, wxWindowID id,
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
                                       wxArrayString *excList = NULL);
   
   wxListBox* GetCelestialBodyListBox(wxWindow *parent, wxWindowID id,
                                      const wxSize &size,
                                      wxArrayString *excList = NULL);
   
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
                                  wxArrayString *excList = NULL);
   
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
   
   wxListBox* GetFuelTankListBox(wxWindow *parent, wxWindowID id,
                                 const wxSize &size,
                                 wxArrayString *excList = NULL);
   wxListBox* GetThrusterListBox(wxWindow *parent, wxWindowID id,
                                 const wxSize &size,
                                 wxArrayString *excList = NULL);
   wxListBox* GetSensorListBox(wxWindow *parent, wxWindowID id,
                               const wxSize &size,
                               wxArrayString *excList = NULL);
   
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
          const wxString &onwer = "Spacecraft",
          const wxString configSection = "Parameter Select");
   
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
   
   void UpdateGroundStationList();
   void UpdateSpacecraftList();
   void UpdateFormationList();
   void UpdateSpaceObjectList();
   void UpdateCelestialBodyList();
   void UpdateCelestialPointList();
   void UpdateSpacePointList();
   void UpdateBurnList();
   void UpdateCoordSystemList();
   void UpdateFuelTankList();
   void UpdateThrusterList();
   void UpdateSensorList();
   void UpdateAntennaList();
   void UpdateFunctionList();
   void UpdateSubscriberList();
   void UpdateXyPlotList();
   void UpdateSolverList();
   void UpdatePropagatorList();
   void UpdateForceModelList();
   
   //void AddToAllObjectList();
   void AddToAllObjectArray();
   
   static GuiItemManager *theInstance;
   GuiInterpreter *theGuiInterpreter;
   
   bool mPngHandlerLoaded;
   wxString mPngIconLocation;
   
   int mGuiStatus;
   int mActiveScriptStatus;
   
   Integer theDataPrecision;
   
   // Register for resource change
   std::vector<GmatPanel*> mResourceUpdateListeners;
   
   std::vector<wxListBox*> mCelestialPointLBList;
   std::vector<wxListBox*> mCelestialBodyLBList;
   std::vector<wxListBox*> mSpaceObjectLBList;
   std::vector<wxListBox*> mSpacecraftLBList;
   std::vector<wxListBox*> mImpBurnLBList;
   std::vector<wxCheckListBox*> mSubscriberCLBList;
   std::vector<wxCheckListBox*> mXyPlotCLBList;
   std::vector<wxCheckListBox*> mSpacecraftCLBList;
   std::vector<wxCheckListBox*> mAllObjectCLBList;
   std::vector<wxListBox*> mFuelTankLBList;
   std::vector<wxListBox*> mThrusterLBList;
   std::vector<wxListBox*> mSensorLBList;
   
   std::vector<wxComboBox*> mSpacePointCBList;
   std::vector<wxComboBox*> mCelestialBodyCBList;
   std::vector<wxComboBox*> mSpacecraftCBList;
   std::vector<wxComboBox*> mImpBurnCBList;
   std::vector<wxComboBox*> mFiniteBurnCBList;
   std::vector<wxComboBox*> mCoordSysCBList;
   std::vector<wxComboBox*> mFunctionCBList;
   std::vector<wxComboBox*> mFuelTankCBList;
   std::vector<wxComboBox*> mThrusterCBList;
   std::vector<wxComboBox*> mSensorCBList;
   std::vector<wxComboBox*> mAntennaCBList;
   std::vector<wxComboBox*> mSubscriberCBList;
   std::vector<wxComboBox*> mReportFileCBList;
   std::vector<wxComboBox*> mSolverCBList;
   std::vector<wxComboBox*> mBoundarySolverCBList;
   std::vector<wxComboBox*> mOptimizerCBList;
   
   std::vector<wxArrayString*> mCelestialPointExcList;
   std::vector<wxArrayString*> mCelestialBodyExcList;
   std::vector<wxArrayString*> mSpaceObjectExcList;
   std::vector<wxArrayString*> mSpacecraftExcList;
   std::vector<wxArrayString*> mImpBurnExcList;
   std::vector<wxArrayString*> mAllObjectExcList;
   std::vector<wxArrayString*> mFuelTankExcList;
   std::vector<wxArrayString*> mThrusterExcList;
   std::vector<wxArrayString*> mSensorExcList;
   std::vector<wxArrayString*> mAntennaExcList;
   std::vector<wxArrayString*> mSubscriberExcList;
   std::vector<wxArrayString*> mReportFileExcList;
   std::vector<wxArrayString*> mXyPlotExcList;
   
   // Spacecraft property
   int theNumScProperty;
   
   // Burn Properties
   int theNumImpBurnProperty;
   int theNumFiniteBurnProperty;
   
   // All Objects
   int theNumAllObject;
   
   // SpacePoint
   int theNumSpacePoint;
   int theNumCelesPoint;
   int theNumCelesBody;
   int theNumCalPoint;
   int theNumSpaceObject;
   int theNumSpacecraft;
   int theNumFormation;
   int theNumGroundStation;
   
   // CoordinateSystem
   int theNumCoordSys;
   
   // Propagator, ForceModel
   int theNumPropagator;
   int theNumForceModel;
   
   // Burn
   int theNumImpBurn;
   int theNumFiniteBurn;
   
   // Solver
   int theNumSolver;
   int theNumBoundarySolver;
   int theNumOptimizer;
   
   // Hardware
   int theNumFuelTank;
   int theNumThruster;
   int theNumSensor;
   int theNumAntenna;
   
   // Function
   int theNumFunction;
   
   // Subscriber
   int theNumSubscriber;
   int theNumReportFile;
   int theNumXyPlot;
   
   // Parameter
   int theNumPlottableParam;
   int theNumSystemParam;
   int theNumUserVariable;
   int theNumUserString;
   int theNumUserArray;
   int theNumUserParam;
   
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
   wxArrayString theSpacePointList;
   wxArrayString theCelestialPointList;
   wxArrayString theCelestialBodyList;
   wxArrayString theCalPointList;
   wxArrayString theSpaceObjectList;
   wxArrayString theSpacecraftList;
   wxArrayString theFormationList;
   wxArrayString theGroundStationList;
   
   // Subscriber
   wxArrayString theSubscriberList;
   wxArrayString theReportFileList;
   wxArrayString theXyPlotList;
   
   // Hardware
   wxArrayString theFuelTankList;
   wxArrayString theThrusterList;
   wxArrayString theSensorList;
   wxArrayString theAntennaList;
   
   // Parameter
   wxArrayString thePlottableParamList;
   wxArrayString theSystemParamList;
   wxArrayString theUserVariableList;
   wxArrayString theUserStringList;
   wxArrayString theUserArrayList;
   wxArrayString theUserParamList;
   
};

#endif // GuiItemManager_hpp
